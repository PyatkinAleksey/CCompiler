/**
* Компилятор подмножества языка C
*
* Класс, реализующий генератор ассемблерного кода.
*
* Автор:			Алексей Пяткин, гр. ВТ-110
* Дата создания:	2012-05-19 16:00
*/

#include <fstream>
#include <string>
#include <list>
#include <map>

#include "Tree.h"
#include "Variable.h"
#include "Compiler.h"

/**
* Класс, реализующий генератор ассемблерного кода.
* Генерация производится по дереву, составленному синтаксическим анализатором.
*/
class CodeGenerator {

	private:

		ofstream casm; // Поток для ассемблерного кода

		Tree *headTree; // Указатель на вершину дерева, сгенерированного синтаксическим анализатором

		int marks;					// Счетчик меток
		map<string,Variable> vars;	// Список идентификаторов исходной программы

		void genBegin();						// Генерация кода начала ассемблерной программы
		void genCode(Tree *pt);					// Генерация кода для поддерева pt
		void genEmpty();						// Генерация кода пустого оператора
		void genExpression(Tree *pt, int lvl);	// Генерация кода для выражений
		void genNegative(Tree *pt);				// Генерация кода операции смены знака
		void genAssignment(Tree *pt);			// Генерация кода для оператора присваивания
		void genIf(Tree *pt);					// Генерация кода для условного оператора IF
		void genWhile(Tree *pt);				// Генерация кода для оператора цикла WHILE
		void genEnd();							// Генерация кода конца ассемблерной программы

	public:

		int status; // Статус выполнения генерации кода (1 - успешно, другое - ошибка)

		CodeGenerator(Tree *pT, map<string,Variable> vars, string fn);	// Инициализация генератора кода

};