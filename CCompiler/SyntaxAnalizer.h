/**
* Компилятор подмножества языка C
*
* Класс, описывающий синтаксический анализатор.
* Фаза лексического анализа объединена с фазой синтаксического анализа.
*
* Автор:			Алексей Пяткин, гр. ВТ-110
* Дата создания:	2012-05-11 23:30
*/

#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <algorithm>

#include "Tree.h"
#include "Variable.h"
#include "Function.h"
#include "Argument.h"
#include "Compiler.h"

using namespace std;

/**
* Класс, описывающий синтаксический анализатор (фаза лексического анализа включена).
*/
class SyntaxAnalizer {

	private:

		// Константы, обозначающие ошибки и их кодов:
		static const int EOF_BEFORE_LE	= 0;	// Достигнут конец файла до логического окончания программы
		static const int UNKNOWN_CHAR	= 1;	// Неизвестный символ
		static const int MUST_BE_OFB	= 2;	// Должна быть открывающая фигурная скобка
		static const int MUST_BE_EFB	= 3;	// Должна быть закрывающая фигурная скобка
		static const int MUST_BE_OPER	= 4;	// Должен быть оператор
		static const int MUST_BE_EQV	= 5;	// Должен быть знак равенства
		static const int MUST_BE_IDCSOB	= 6;	// Должен быть идентификатор, константа, минус или открывающая скобка
		static const int MUST_BE_MINUS	= 7;	// Должен быть знак "минус"
		static const int MUST_BE_OB		= 8;	// Должна быть открывающая скобка
		static const int MUST_BE_EB		= 9;	// Должна быть закрывающая скобка
		static const int UNDEF_TYPE		= 10;	// Необъявленный тип данных
		static const int MUST_BE_ID		= 11;	// Должен быть идентификатор
		static const int MUST_BE_COEB	= 12;	// Должна быть точка с запятой или закрывающая скобка
		static const int MUST_BE_EBOT	= 13;	// Должна быть закрывающая скобка или тип данных
		static const int MUST_RETURN	= 14;	// Функция должна возвращать значение
		static const int MUST_BE_SEMI	= 15;	// Должна быть точка с запятой
		static const int FUNC_AH_BODY	= 16;	// Функция с таким именем уже определена
		static const int EXP_OBOAS		= 17;	// Ожидается открывающая скобка, знак присваивания или точка с запятой
		static const int EXP_CONST		= 18;	// Ожидается константа
		static const int ID_WAS_DEF		= 19;	// Идентификатор был объявлен
		static const int UNDEC_ID		= 20;	// Необъявленный идентификатор
		// Перечень лексем и соответствующих им идентификационных номеров:
		static const int OFB		= 0;	// Открывающая фигурная скобка
		static const int EFB		= 1;	// Закрывающая фигурная скобка
		static const int SEMI		= 2;	// Точка с запятой
		static const int COMMA		= 3;	// Запятая
		static const int ASSIGN		= 4;	// Присваивание
		static const int LESS_EQV	= 5;	// Меньше или равно
		static const int GRT_EQV	= 6;	// Больше или равно
		static const int LESS		= 7;	// Меньше
		static const int GRT		= 8;	// Больше
		static const int NOT_EQV	= 9;	// Не равно
		static const int EQV		= 10;	// Равно
		static const int ADD		= 11;	// Сложение
		static const int SUB		= 12;	// Вычитание
		static const int DIV		= 13;	// Деление
		static const int MUL		= 14;	// Умножение
		static const int MOD		= 15;	// Деление по модулю
		static const int NOT		= 16;	// Отрицание
		static const int OB			= 17;	// Открывающая скобка
		static const int EB			= 18;	// Закрывающая скобка
		static const int IF			= 19;	// "Если"
		static const int ELSE		= 20;	// "Иначе"
		static const int WHILE		= 21;	// "Пока"
		static const int ID			= 22;	// Идентификатор
		static const int CNST		= 23;	// Константа
		static const int TYPE		= 24;	// Тип данных
		static const int RET		= 25;	// Оператор возврата значения

		Compiler *comp;

		// Ассоциативный массив, содержащий список операций и их приоритеты:
		map<string,int> operation;
		// Массив, содержащий все возможные типы данных:
		string types[1];

		ifstream code;	// Поток для файла с исходным кодом
		
		char ch;		// Текущий выбранный символ
		string lex;		// Текущая лексема
		int lexType;	// Тип текущей лексемы

		Tree	*pt;		// Указатель на дерево программы
		Tree	*sTRoot;	// Указатель на корень поддерева (используется при перестройке деревьев выражений)

		// Общие методы синтаксического анализатора:
		void getError(int ec, string value);	// Определение ошибки по ее коду
		void getChar();							// Получение следующего символа из входного потока
		void getLex();							// Определение лексемы
		void deleteTree(Tree *p);				// Удаление из памяти сгенерированного дерева

		// Методы, реализующие построение дерева для определенных элементов программы:
		void program();														// Программа
		void declaration(string type, string id, int lvl, Tree *p, int b);	// Объявление переменных
		void function();													// Функция
		void functionArguments();											// Аргументы функций
		void compound(Tree *p, int b);										// Составной оператор
		void oper(Tree *p, int b);											// Оператор
		void expression(Tree *p, int b, int be);							// Выражение

	public:

		Tree					*head;		// Указатель на корень дерева программы
		list<string>			ids;		// Перечень идентификаторов
		map<string,Variable>	variables;	// Перечень оъявленных переменных
		map<string,Function>	functions;	// Перечень функций и адресов их деревьев
		map<string,Argument>	arguments;	// Перечень функций и их аргументов
		int status;							// Статус стадии разбора (1 - успешно, другое - ошибка)

		SyntaxAnalizer(Compiler *c, string fn);	// Инициализация синтаксического анализатора
		~SyntaxAnalizer();			// Освобождение памяти и закрытие потоков

		void printTree(Tree *p);	// Вывод семантического дерева исходной программы

};