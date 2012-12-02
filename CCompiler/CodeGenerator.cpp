/**
* Компилятор подмножества языка C
*
* Класс, реализующий генератор ассемблерного кода.
*
* Автор:			Алексей Пяткин, гр. ВТ-110
* Дата создания:	2012-05-19 16:00
*/

#include "CodeGenerator.h"

/**
* Инициализация генератора кода и начало выполнения.
*
* Параметры:
* pT	- указатель на вершину дерева, по которому будет осуществляться генерация кода.
* ids	- список идентификаторов, существующих в программе.
* fn	- имя файла, в который будет записываться сгенерированный код.
*/
CodeGenerator::CodeGenerator(Tree *pT, map<string,Variable> vars, string fn) {

	headTree = pT;

	this->vars = vars;

	status = 1;
	marks = 0;

	casm.open(fn + ".asm");	// Открываем поток для записи ассемблерного кода
	
	genBegin();				// Генерируем код начала ассемблерной программы
	if (headTree != NULL) {	// Если дерево - не пустое, то начинаем генерацию алгоритмического кода программы
		genCode(headTree);
	}
	genEnd();				// Генерируем код конца ассемблерной программы
	
}

/**
* Метод генерации кода для поддерева pt.
*
* Параметры:
* pt - указатель на поддерева, для которого нужно сгенерировать код.
*/
void CodeGenerator::genCode(Tree *pt) {

	if (pt->info == ";") { // Пустой оператор
		genEmpty();
	} else if (pt->info == "=") { // Оператор присваивания
		genAssignment(pt);
		return;
	} else if (pt->info == "if") { // Условный оператор IF
		genIf(pt);
		return;
	} else if (pt->info == "while") { // Оператор цикла WHILE
		genWhile(pt);
		return;
	}

	if (pt->llink != NULL) {genCode(pt->llink);}
	if (pt->rlink != NULL) {genCode(pt->rlink);}

}

/**
* Метод генерации кода для пустого оператора.
*/
void CodeGenerator::genEmpty() {

	casm << "\tNOP" << endl;

}

/**
* Метод генерации кода для выражений.
*
* Параметры:
* pt - указатель на поддерево, для которого будет сгенерирован код.
* lvl - уровень рекурсии.
*/
void CodeGenerator::genExpression(Tree *pt, int lvl) {

	if (pt->info == "-" && pt->llink != NULL && pt->rlink == NULL) { // Оператор смены знака
		genNegative(pt);
		if (lvl) {
			casm << "\tPUSH\tAX" << endl;
		}
		return;
	}

	if (pt->llink != NULL && pt->rlink != NULL) {
		genExpression(pt->llink, lvl+1);
		genExpression(pt->rlink, lvl+1);
	} else {
		casm << "\tMOV\tAX, " << pt->info << endl;
		if (lvl) {
			casm << "\tPUSH\tAX" << endl;
		}
	}

	if (pt->info == "+") {
		casm << "\tPOP\tBX" << endl;
		casm << "\tPOP\tAX" << endl;
		casm << "\tADD\tAX, BX" << endl;
		if (lvl) {
			casm << "\tPUSH\tAX" << endl;
		}
	} else if (pt->info == "-") {
		casm << "\tPOP\tBX" << endl;
		casm << "\tPOP\tAX" << endl;
		casm << "\tSUB\tAX, BX" << endl;
		if (lvl) {
			casm << "\tPUSH\tAX" << endl;
		}
	} else if (pt->info == "*") {
		casm << "\tPOP\tBX" << endl;
		casm << "\tPOP\tAX" << endl;
		casm << "\tIMUL\tBX" << endl;
		if (lvl) {
			casm << "\tPUSH\tAX" << endl;
		}
	} else if (pt->info == "/") {
		casm << "\tPOP\tBX" << endl;
		casm << "\tPOP\tAX" << endl;
		casm << "\tIDIV\tBX" << endl;
		if (lvl) {
			casm << "\tPUSH\tAX" << endl;
		}
	} else if (pt->info == "%") {
		casm << "\tPOP\tBX" << endl;
		casm << "\tPOP\tAX" << endl;
		casm << "\tIDIV\tBX" << endl;
		if (lvl) {
			casm << "\tPUSH\tDX" << endl;
		} else {
			casm << "\tMOV\tAX, DX" << endl;
		}
	} else if (pt->info == "<" || pt->info == "<=" || pt->info == "==" || pt->info == "!="
				|| pt->info == ">" || pt->info == ">=") {
		int mark1 = marks;
		marks++;
		int mark2 = marks;
		marks++;
		casm << "\tPOP\tBX" << endl;
		casm << "\tPOP\tAX" << endl;
		casm << "\tCMP\tAX, BX" << endl;
		if (pt->info == "<") {
			casm << "\tJL\tm" << mark1 << endl;
		} else if (pt->info == "<=") {
			casm << "\tJLE\tm" << mark1 << endl;
		} else if (pt->info == "==") {
			casm << "\tJZ\tm" << mark1 << endl;
		} else if (pt->info == "!=") {
			casm << "\tJNZ\tm" << mark1 << endl;
		} else if (pt->info == ">") {
			casm << "\tJG\tm" << mark1 << endl;
		} else if (pt->info == ">=") {
			casm << "\tJGE\tm" << mark1 << endl;
		}
		casm << "\tXOR\tAX, AX" << endl;
		casm << "\tJMP\tm" << mark2 << endl;
		casm << "m" << mark1 << ":\tMOV\tAX, 1" << endl;
		casm << "m" << mark2 << ":" << endl;
		if (lvl) {
			casm << "\tPUSH\tAX" << endl;
		}
	}

}

/**
* Метод генерации кода для оператора смены знака.
*
* Параметры:
* pt - указатель на поддерево оператора смены знака.
*/
void CodeGenerator::genNegative(Tree *pt) {

	genExpression(pt->llink, 0);
	casm << "\tNEG\tAX" << endl;

}

/**
* Метод генерации кода для оператора присваивания.
*
* Параметры:
* pt - указатель на поддерево с оператором присваивания, для которого будет сгенерирован код.
*/
void CodeGenerator::genAssignment(Tree *pt) {
	
	if (pt->rlink->llink != NULL || pt->rlink->rlink != NULL) {
		genExpression(pt->rlink, 0);
		casm << "\tMOV\t" << pt->llink->info << ", AX" << endl;
	} else {
		if (pt->rlink->info[0] >= '0' && pt->rlink->info[0] <= '9') {
			casm << "\tMOV\t" << pt->llink->info << ", " << pt->rlink->info << endl;
		} else {
			casm << "\tMOV\tAX, " << pt->rlink->info << endl;
			casm << "\tMOV\t" << pt->llink->info << ", AX" << endl;
		}
	}

}

/**
* Метод генерации кода для условного оператора IF.
*
* Параметры:
* pt - указатель на поддерево с оператором IF.
*/
void CodeGenerator::genIf(Tree *pt) {

	int mark1 = marks;
	marks++;
	int mark2 = marks;
	marks++;

	if (pt->llink != NULL) {genExpression(pt->llink, 0);}
	casm << "\tCMP\tAX, 0" << endl;
	casm << "\tJZ\tm" << mark1 << endl;
	pt = pt->rlink;
	if (pt->llink != NULL) {genCode(pt->llink);}
	casm << "\tJMP\tm" << mark2 << endl;
	casm << "m" << mark1 << ":" << endl;
	if (pt->rlink != NULL) {genCode(pt->rlink);}
	casm << "m" << mark2 << ":" << endl;

}

/**
* Метод генерации кода для оператора цикла WHILE.
*
* Параметры:
* pt - указатель на поддерево с оператором цикла WHILE, для которого будет сгенерирован код.
*/
void CodeGenerator::genWhile(Tree *pt) {

	int mark1 = marks;
	marks++;
	int mark2 = marks;
	marks++;

	casm << "m" << mark1 << ":" << endl;
	if (pt->llink != NULL) {genExpression(pt->llink, 0);}
	casm << "\tCMP\tAX, 0" << endl;
	casm << "\tJZ\tm" << mark2 << endl;
	if (pt->rlink != NULL) {genCode(pt->rlink);}
	casm << "\tJMP\tm" << mark1 << endl;
	casm << "m" << mark2 << ":" << endl;

}

/**
* Метод генерации кода начала ассемблерной программы.
*/
void CodeGenerator::genBegin() {

	// Сегмент данных
	casm << "dseg\tSEGMENT";
	for (map<string,Variable>::iterator varIter = vars.begin(); varIter != vars.end(); varIter++) {
		if (!(*varIter).second.level) {
			if ((*varIter).second.type == "int") {
				casm << endl << (*varIter).first << "\tDW\t" << (*varIter).second.initVal;
			}
		}
	}
	casm << endl;
	casm << "dseg\tENDS" << endl << endl;
	// Начало сегмента кода
	casm << "cseg\tSEGMENT" << endl;
	casm << "ASSUME\tds:dseg, cs:cseg" << endl;
	casm << "__program_start__:" << endl << endl;

}

/**
* Метод генерации кода окончания ассемблерной программы.
*/
void CodeGenerator::genEnd() {

	// Завершение программы
	casm << endl << "\tMOV\tAX, 4C00h" << endl;
	casm << "\tINT\t21h" << endl;
	// Окончание сегмента кода
	casm << endl << "cseg\tENDS" << endl;
	casm << "\tEND __program_start__" << endl;

}