/**
* Компилятор подмножества языка C
*
* Класс, описывающий синтаксический анализатор.
* Фаза лексического анализа объединена с фазой синтаксического анализа.
*
* Автор:			Алексей Пяткин, гр. ВТ-110
* Дата создания:	2012-05-11 23:50
*/

#include "SyntaxAnalizer.h"

/**
* Инициализация синтаксического анализатора.
*
* Параметры:
* c - указатель на объект класса Compiler для доступа к общим методам и полям компилятора.
* fn - имя файла с исходной программой.
*/
SyntaxAnalizer::SyntaxAnalizer(Compiler *c, string fn) {

	code.open(fn + ".c");	// Открываем входной поток с исходным текстом программы
	
	comp = c;
	
	// Заполняем словарь приоритетов операций и их идентификаторов
	operation["="]		= 0;
	operation["=="]		= 1;
	operation["!="]		= 1;
	operation["<"]		= 2;
	operation["<="]		= 2;
	operation[">"]		= 2;
	operation[">="]		= 2;
	operation["+"]		= 3;
	operation["-"]		= 3;
	operation["*"]		= 4;
	operation["/"]		= 4;
	operation["%"]		= 4;

	// Заполняем массив типов данных:
	types[0] = "int";
	
	status	= 1;
	ch		= -1;

	program(); // Начало разбора

}

/**
* Определение ошибки по ее коду.
*
* Параметры:
* ec - код ошибки.
* value - значение, при котором возникла ошибка.
*/
void SyntaxAnalizer::getError(int ec, string value) {

	switch (ec) {
		case EOF_BEFORE_LE:		comp->printError("End of file before logical end of the program.");			break;
		case UNKNOWN_CHAR:		comp->printError("Unknown character " + value + ".");						break;
		case MUST_BE_OFB:		comp->printError(value + ": Must be \"{\".");								break;
		case MUST_BE_EFB:		comp->printError(value + ": Must be \"}\".");								break;
		case MUST_BE_OPER:		comp->printError(value + ": Must be operator.");							break;
		case MUST_BE_EQV:		comp->printError(value + ": Must be \"=\".");								break;
		case MUST_BE_IDCSOB:	comp->printError(value + ": Must be ID, constant, \"-\" or \"(\".");		break;
		case MUST_BE_MINUS:		comp->printError(value + ": Must be \"-\".");								break;
		case MUST_BE_OB:		comp->printError(value + ": Must be \"(\".");								break;
		case MUST_BE_EB:		comp->printError(value + ": Must be \")\".");								break;
		case UNDEF_TYPE:		comp->printError("Use of undefined type '" + value + "'");					break;
		case MUST_BE_ID:		comp->printError(value + ": Must be an identifier.");						break;
		case MUST_BE_COEB:		comp->printError(value + ": Must be \",\" or \")\".");						break;
		case MUST_BE_EBOT:		comp->printError(value + ": Must be \")\" or a type.");						break;
		case MUST_BE_SEMI:		comp->printError("Expected ';' but found '" + value + "'");					break;
		case MUST_RETURN:		comp->printError("Function " + head->info + " must to return any "
									+ functions[head->info].type + " value.");								break;
		case FUNC_AH_BODY:		comp->printError("Function " + value + " already has a body.");				break;
		case EXP_OBOAS:			comp->printError("Expected '(', '=' or ';' but found '" + value + "'");		break;
		case EXP_CONST:			comp->printError("Expected constant but found '" + value + "'");			break;
		case ID_WAS_DEF:		comp->printError("The identifier was already defined as '" + value + "'");	break;
		default:				comp->printError("Unknown error message.");									break;
	}

	status = -1;

	exit(status);

}

/**
* Получение следующего символа из входного потока.
*/
void SyntaxAnalizer::getChar() {
	
	if (!code.eof()) {
		code >> noskipws >> ch;
	} else {
		getError(EOF_BEFORE_LE, "");
	}
	
}

/**
* Определение лексемы.
*/
void SyntaxAnalizer::getLex() {
	
	while (ch == -1 || ch == ' ' || ch == '\n' || ch == '\t') {
		getChar();
	}
	
	lex = ch;
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {lexType = ID;}
	else if (ch >= '0' && ch <= '9') {lexType = CNST;}
	else {
		switch (ch) {
			case '{': lexType = OFB; break;
			case '}': lexType = EFB; break;
			case ',': lexType = COMMA; break;
			case ';': lexType = SEMI; break;
			case '=': lexType = ASSIGN; break;
			case '+': lexType = ADD; break;
			case '*': lexType = MUL; break;
			case '-': lexType = SUB; break;
			case '/': lexType = DIV; break;
			case '%': lexType = MOD; break;
			case '<': lexType = LESS; break;
			case '>': lexType = GRT; break;
			case '!': lexType = NOT; break;
			case '(': lexType = OB; break;
			case ')': lexType = EB; break;
			case -1:
				return;
				break; // Исходный файл пуст
			default: getError(UNKNOWN_CHAR, lex); break;
		}
	}
	
	if (lexType == LESS || lexType == ASSIGN || lexType == NOT || lexType == GRT) {
		getChar();
		if (ch == '=') {
			lex += ch;
		} else {
			if (lexType == NOT) {
				getError(MUST_BE_EQV, lex);
			}
		}
		if (lex == "<")			{lexType = LESS;}
		else if (lex == "<=")	{lexType = LESS_EQV;}
		else if (lex == "==")	{lexType = EQV;}
		else if (lex == "!=")	{lexType = NOT_EQV;}
		else if (lex == ">")	{lexType = GRT;}
		else if (lex == ">=")	{lexType = GRT_EQV;}
		return;
	}

	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) { // Идентификаторы
		getChar();
		while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
			lex += ch;
			getChar();
		}
		// Ключевые слова:
		if (lex == "if") {lexType = IF;}
		else if (lex == "else") {lexType = ELSE;}
		else if (lex == "while") {lexType = WHILE;}
		else if (lex == "void") {lexType = TYPE;}
		else if (lex == "int") {lexType = TYPE;}
		else if (lex == "return") {lexType = RET;}
		else { // Помещаем идентификатор в список идентификаторов, если его там еще нет
			list<string>::iterator res = find(ids.begin(), ids.end(), lex);
			if (res == ids.end()) { // Идентификатор не найден в списке
				ids.push_back(lex);
			}
		}
		return;
	}
	
	if (ch >= '0' && ch <= '9') { // Константы
		getChar();
		while (ch >= '0' && ch <= '9') {
			lex += ch;
			getChar();
		}
	}
	
}

/**
* Метод, реализующий построение дерева для программы.
*/
void SyntaxAnalizer::program() {

	while (!code.eof()) {
		string	type;
		string	id;
		getChar();
		if (code.eof()) {break;}
		head	= NULL;
		pt		= NULL;
		getLex();
		switch (lexType) {
			case TYPE:
				type = lex;
				// После типа должен идти идентификатор
				getLex();
				if (lexType != ID) {
					getError(MUST_BE_ID, lex);
				}
				if (functions.find(lex) != functions.end()) {
					getError(FUNC_AH_BODY, lex);
				}
				id = lex;
				getLex();
				switch (lexType) {
					case OB: // Функция
						// Идентификатор функции становится корнем дерева функции
						head = new Tree();
						head->info = id;
						head->llink = NULL;
						head->rlink = NULL;
						pt = head;
						// В массив заносим идентификатор функции и адрес корня ее дерева
						functions[id] = Function();
						functions[id].address	= head;
						functions[id].type		= type;
						function();
						break;
					case ASSIGN: // Объявление переменных
					case SEMI:
						declaration(type, id, 0, NULL, 0);
						break;
					default:
						getError(EXP_OBOAS, lex);
						break;
				}
				break;
			default:
				getError(UNDEF_TYPE, lex);
				break;
		}
		getChar();
	}
	
}

/**
* Метод объявления переменных.
*
* Параметры:
* type - тип переменной.
* id - идентификатор объявляемой переменной.
*/
void SyntaxAnalizer::declaration(string type, string id, int lvl, Tree *p, int b) {

	if (type != "void") {
		
			Variable var;
			
			var.type	= type;
			if (lexType == ASSIGN) {
				getLex();
				if (lexType != CNST) {
					getError(EXP_CONST, lex);
				}
				var.initVal = lex;
				getLex();
			} else {
				var.initVal = "?";
			}
			var.level	= lvl;
			if (head != NULL) {
				var.funcName = head->info;
			} else {
				var.funcName = "";
			}
			
			map<string,Variable>::iterator it = variables.find(id);
			if ((it == variables.end()) || ((*it).second.level != lvl) || ((*it).second.level == lvl && (*it).second.funcName != var.funcName)) {
				Tree *q;
				variables[id] = var;
				if (p != NULL) {
					//p = new Tree;
					//p->info = "$";
					if (b) {
						p->rlink = new Tree();
						q = p->rlink;
					} else {
						p->llink = new Tree();
						q = p->llink;
					}
					q->info = "dec";
					q->llink = new Tree();
					q->llink->info = id;
					q->llink->llink = NULL;
					q->llink->rlink = NULL;
					q->rlink = NULL;
				}
			} else {
				getError(ID_WAS_DEF, type);
			}

	} else {
		getError(UNDEF_TYPE, type);
	}

}

/**
* Метод, отвечающий за построение деревьев для функций.
*/
void SyntaxAnalizer::function() {
	
	sTRoot	= NULL;
	// Список аргументов функции
	getChar();
	getLex();
	if (lexType == TYPE) {
		functionArguments();
	}
	if (lexType != EB) {
		getError(MUST_BE_EBOT, lex);
	}
	// Последовательность действий, выполняемых в функциях, записана в составном операторе:
	getChar();
	getLex();
	compound(NULL, 1);
	
}

/**
* Метод, разбирающий аргументы функций. Заносит их в массив аргументов.
*/
void SyntaxAnalizer::functionArguments() {
	
	string type;
	string id;

	if (lexType == EB) {
		return;
	}
	if (lexType != TYPE) {
		getError(UNDEF_TYPE, lex);
	}

	type = lex;
	getLex();
	if (lexType != ID) {
		getError(MUST_BE_ID, lex);
	}
	id = lex;
	// Добавляем в список переменных
	declaration(type, id, 1, pt, 0);
	// Сохраняем информацию об аргументе в массив:
	arguments[id] = Argument();
	arguments[id].funcName = head->info;
	arguments[id].argType = type;
	// Смотрим, что дальше
	getLex();
	switch (lexType) {
		case COMMA:
			getChar();
			getLex();
			functionArguments();
			break;
		case EB: break;
		default:
			getError(MUST_BE_COEB, lex);
			break;
	}
	
}

/**
* Метод, реализующий построение дерева для составного оператора.
*
* Параметры:
* p - указатель на элемент дерева, к которому присоединять поддерево составного оператора.
* b - флаг, к какой ветви присоединять поддерево составного оператора (1 - правое, другое - левое).
*/
void SyntaxAnalizer::compound(Tree *p, int b) {
	
	int isFunc = 0;		// Является телом функции
	int isReturned = 0; // Есть оператор возврата значения
	int isIf = 0;		// Встретился оператор If
	int opCounter = 0;
	Tree *q = NULL;
	
	if (lexType == OFB) {
		if (p == NULL) {
			isFunc = 1;
			if (b) { // Правая ветвь
				pt->rlink = new Tree();
				pt = pt->rlink;
			} else { // Левая ветвь
				pt->llink = new Tree();
				pt = pt->llink;
			}
			p = pt;
		} else {
			if (b) { // Правая ветвь
				p->rlink = new Tree();
				p = p->rlink;
			} else { // Левая ветвь
				p->llink = new Tree();
				p = p->llink;
			}
			pt = p;
		}
		p->info		= lex;
		p->llink	= NULL;
		p->rlink	= NULL;
		getChar();
		getLex();
		if (lexType == IF) {
			isIf = 1;
		}
		while (lexType != EFB) {
			opCounter++;
			if (opCounter > 1) {
				p->rlink = new Tree();
				q = p->rlink;
				q->info		= "$";
				q->llink	= NULL;
				q->rlink	= NULL;
				p = q;
			} else {
				q = p;
			}
			if (lexType == RET) {
				isReturned = 1;
			}
			oper(q, 0);
			if (!isIf) {
				getChar();
				getLex();
			}
		}
		if (isFunc) { // Составной оператор - тело функции
			// Если тип возвращаемого значения указан, как отличный от void,
			// то необходим возврат значения соответствующего типа
			if (functions[head->info].type != "void") {
				if (!isReturned) {
					getError(MUST_RETURN, "");
				}
			}
		}
	} else {
		getError(MUST_BE_OFB, lex);
	}
	
}

/**
* Метод, реализующий построение дерева для операторов.
*
* Параметры:
* p - указатель на элемент дерева, к которому присоединяется поддерево оператора.
* b - флаг, к какой ветви присоединять поддерево оператора (1 - правая, другое - левая).
*/
void SyntaxAnalizer::oper(Tree *p, int b) {
	
	Tree *q;
	int lext = lexType;
	string l = lex;
	if (p == NULL) {p = pt;}
	string type;
	string id;

	switch (lexType) {
		case SEMI:	// Пустой оператор
			if (b) { // Правая ветвь
				p->rlink = new Tree();
				q = p->rlink;
			} else { // Левая ветвь
				p->llink = new Tree();
				q = p->llink;
			}
			q->info = lex;
			q->llink = NULL;
			q->rlink = NULL;
			break;
		case OFB:	// Составной оператор
			compound(p, b);
			break;
		case IF:	// Условный оператор IF
			if (b) { // Правая ветвь
				p->rlink = new Tree();
				p = p->rlink;
			} else { // Левая ветвь
				p->llink = new Tree();
				p = p->llink;
			}
			p->info = lex;
			p->llink = NULL;
			p->rlink = NULL;
			getLex();
			if (lexType == OB) {
				getChar();
				sTRoot = p;
				expression(p, 0, 0);
				getLex();
				if (lexType == EB) {
					p->rlink = new Tree();
					p = p->rlink;
					p->info = "fi";
					p->llink = NULL;
					p->rlink = NULL;
					getChar();
					getLex();
					oper(p, 0);
					getChar();
					getLex();
					if (lexType == ELSE) {
						getLex();
						oper(p, 1);
					}
				} else {
					getError(MUST_BE_EB, lex);
				}
			} else {
				getError(MUST_BE_OB, lex);
			}
			break;
		case WHILE:	// Оператор цикла WHILE
			if (b) { // Правая ветвь
				p->rlink = new Tree();
				p = p->rlink;
			} else { // Левая ветвь
				p->llink = new Tree();
				p = p->llink;
			}
			p->info = lex;
			p->llink = NULL;
			p->rlink = NULL;
			getLex();
			if (lexType == OB) {
				getChar();
				sTRoot = p;
				expression(p, 0, 0);
				getLex();
				if (lexType == EB) {
					getChar();
					getLex();
					oper(p, 1);
				} else {
					getError(MUST_BE_EB, lex);
				}
			} else {
				getError(MUST_BE_OB, lex);
			}
			break;
		case ID:	// Оператор присваивания
			getLex();
			if (lexType == ASSIGN) {
				if (b) { // Правая ветвь
					p->rlink = new Tree();
					q = p->rlink;
				} else { // Левая ветвь
					p->llink = new Tree();
					q = p->llink;
				}
				q->info = lex;
				q->llink = new Tree();
				q->llink->info = l;
				q->llink->llink = NULL;
				q->llink->rlink = NULL;
				q->rlink = NULL;
				sTRoot = q;
				expression(q, 1, 0);
			} else {
				getError(MUST_BE_EQV, lex);
			}
			break;
		case TYPE: // Объявление переменной
			type = lex;
			getLex();
			if (lexType != ID) {
				getError(MUST_BE_ID, lex);
			}
			id = lex;
			declaration(type, id, 2, p, 0);
			getLex();
			if (lexType != SEMI) {
				getError(MUST_BE_SEMI, lex);
			}
			break;
		case RET:	// Оператор возврата из функции
			if (b) {
				p->rlink = new Tree();
				q = p->rlink;
			} else {
				p->llink = new Tree();
				q = p->llink;
			}
			q->info = lex;
			q->llink = NULL;
			q->rlink = NULL;
			if (functions[head->info].type == "int") { // Должно быть возвращено значение типа int
				expression(q, 1, 0);
			}
			getChar();
			getLex();
			break;
		default: // Ошибка: должен быть оператор
			getError(MUST_BE_OPER, lex);
			break;
	}
	
}

/**
* Метод генерации дерева для выражений.
*
* Параметры:
* p - указатель на элемент существующего дерева, к которому присоединяется поддерево выражения.
* b - флаг, к какой ветви присоединяется поддерево выражения (1 - правое, другое - левое).
* be - флаг, окружено ли выражение скобками (1 - окружено, другое - нет).
*/
void SyntaxAnalizer::expression(Tree *p, int b, int be) {
	
	Tree *s;
	
	getLex();
	string l = lex;
	int lType = lexType;
	
	switch (lexType) {	// В выражении встретился
		case ID:		// идентификатор
		case CNST:		// или константа
			getLex();
			switch (lexType) {	// После идентификатора или константы стоит
				case ADD:		// знак сложения
				case SUB:		// или вычитания
				case DIV:		// или деления
				case MOD:		// или деления по модулю
				case MUL:		// или умножения
				case LESS:		// или "меньше"
				case LESS_EQV:	// или "меньше или равно"
				case EQV:		// или "равно"
				case NOT_EQV:	// или "не равно"
				case GRT:		// или "больше"
				case GRT_EQV:	// или "больше или равно"
					if (be || !operation.count(p->info) || (operation.find(lex)->second > operation.find(p->info)->second)) {
						// Приоритет текущей операции БОЛЬШЕ приоритета предыдущей
						// или выражение заключено в скобки
						if (b) { // Правая ветвь
							p->rlink = new Tree();
							s = p->rlink;
						} else { // Левая ветвь
							p->llink = new Tree();
							s = p->llink;
						}
						s->info = lex;
						s->llink = new Tree();
						s->llink->info = l;
						s->llink->llink = NULL;
						s->llink->rlink = NULL;
						s->rlink = NULL;
						sTRoot = p;
					} else {
						// Приоритет текущей операции МЕНЬШЕ ИЛИ РАВЕН приоритету предыдущей
						if (b) { // Правая ветвь
							p->rlink = new Tree();
							p->rlink->info = l;
							p->rlink->llink = NULL;
							p->rlink->rlink = NULL;
						} else { // Левая ветвь
							p->llink = new Tree();
							p->llink->info = l;
							p->llink->llink = NULL;
							p->llink->rlink = NULL;
						}
						if (sTRoot->llink == p) {
							sTRoot->llink = new Tree();
							sTRoot->llink->info = lex;
							sTRoot->llink->llink = p;
							sTRoot->llink->rlink = NULL;
							s = sTRoot->llink;
						} else {
							sTRoot->rlink = new Tree();
							sTRoot->rlink->info = lex;
							sTRoot->rlink->llink = p;
							sTRoot->rlink->rlink = NULL;
							s = sTRoot->rlink;
						}
					}
					getChar();
					expression(s, 1, 0);
					break;
				default:	// После идентификатора или константы стоит любой символ, кроме операций
							// выражение завершено
					if (b) { // Правая ветвь
						p->rlink = new Tree();
						s = p->rlink;
					} else { // Левая ветвь
						p->llink = new Tree();
						s = p->llink;
					}
					s->info = l;
					s->llink = NULL;
					s->rlink = NULL;
					break;
			}
			break;
		case SUB: // Знак "минус"
			s = new Tree();
			if (b) { // Правая ветвь
				p->rlink = s;
			} else { // Левая ветвь
				p->llink = s;
			}
			s->info = lex;
			s->llink = NULL;
			s->rlink = NULL;
			getChar();
			expression(s, 0, 0);
			break;
		case OB: // Открывающая скобка
			getChar();
			expression(p, b, 1);
			getLex();
			if (lexType != EB) {
				getError(MUST_BE_EB, lex);
			}
			getChar();
			getLex();
			if (lexType == ADD || lexType == SUB || lexType == MUL || lexType == DIV
				|| lexType == LESS || lexType == LESS_EQV || lexType == EQV
				|| lexType == NOT_EQV || lexType == GRT || lexType == GRT_EQV) {
				Tree *q = new Tree();
				q->info = lex;
				if (b) { // Правая ветвь
					q->llink = p->rlink;
					p->rlink = q;
				} else { // Левая ветвь
					q->llink = p->llink;
					p->llink = q;
				}
				q->rlink = NULL;
				getChar();
				expression(q, b, 0);
			}
			break;
		default:
			// Ошибка: должен быть идентификатор, или константа, или знак "минус", или открывающая скобка
			getError(MUST_BE_IDCSOB, lex);
			break;
	}
	
}

/**
* Метод вывода семантического дерева программы.
*
* Параметры:
* p - указатель на текущий элемент дерева, который будет выведен.
*/
void SyntaxAnalizer::printTree(Tree *p) {
	
	if (head != NULL) {
		
		if (p == NULL) {
			p = head;
			comp->logs << "Semantic Tree:" << endl;
		}
		
		comp->logs << p << "\t" << p->info << "\t" << p->llink << "\t" << p->rlink << endl;
		
		if (p->llink != NULL) {printTree(p->llink);}
		if (p->rlink != NULL) {printTree(p->rlink);}

	}

}

/**
* Удаление из памяти дерева, сгенерированного синтаксическим анализатором.
*
* Параметры:
* p - указатель на элемент дерева для удаления.
*/
void SyntaxAnalizer::deleteTree(Tree *p) {
	
	if (p->llink != NULL) {deleteTree(p->llink);}
	if (p->rlink != NULL) {deleteTree(p->rlink);}
	
	if (p->llink == NULL && p->rlink == NULL) {
		delete p;
	}
	
}

/**
* Освобождение памяти, занятой синтаксическим анализатором и закрытие потоков.
*/
SyntaxAnalizer::~SyntaxAnalizer() {
	
	for (map<string,Function>::iterator iter = functions.begin(); iter != functions.end(); iter++) {
		deleteTree((*iter).second.address);
	}
	
	code.close();
	
}