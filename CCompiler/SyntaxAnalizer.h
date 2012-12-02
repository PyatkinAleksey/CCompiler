/**
* ���������� ������������ ����� C
*
* �����, ����������� �������������� ����������.
* ���� ������������ ������� ���������� � ����� ��������������� �������.
*
* �����:			������� ������, ��. ��-110
* ���� ��������:	2012-05-11 23:30
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
* �����, ����������� �������������� ���������� (���� ������������ ������� ��������).
*/
class SyntaxAnalizer {

	private:

		// ���������, ������������ ������ � �� �����:
		static const int EOF_BEFORE_LE	= 0;	// ��������� ����� ����� �� ����������� ��������� ���������
		static const int UNKNOWN_CHAR	= 1;	// ����������� ������
		static const int MUST_BE_OFB	= 2;	// ������ ���� ����������� �������� ������
		static const int MUST_BE_EFB	= 3;	// ������ ���� ����������� �������� ������
		static const int MUST_BE_OPER	= 4;	// ������ ���� ��������
		static const int MUST_BE_EQV	= 5;	// ������ ���� ���� ���������
		static const int MUST_BE_IDCSOB	= 6;	// ������ ���� �������������, ���������, ����� ��� ����������� ������
		static const int MUST_BE_MINUS	= 7;	// ������ ���� ���� "�����"
		static const int MUST_BE_OB		= 8;	// ������ ���� ����������� ������
		static const int MUST_BE_EB		= 9;	// ������ ���� ����������� ������
		static const int UNDEF_TYPE		= 10;	// ������������� ��� ������
		static const int MUST_BE_ID		= 11;	// ������ ���� �������������
		static const int MUST_BE_COEB	= 12;	// ������ ���� ����� � ������� ��� ����������� ������
		static const int MUST_BE_EBOT	= 13;	// ������ ���� ����������� ������ ��� ��� ������
		static const int MUST_RETURN	= 14;	// ������� ������ ���������� ��������
		static const int MUST_BE_SEMI	= 15;	// ������ ���� ����� � �������
		static const int FUNC_AH_BODY	= 16;	// ������� � ����� ������ ��� ����������
		static const int EXP_OBOAS		= 17;	// ��������� ����������� ������, ���� ������������ ��� ����� � �������
		static const int EXP_CONST		= 18;	// ��������� ���������
		static const int ID_WAS_DEF		= 19;	// ������������� ��� ��������
		static const int UNDEC_ID		= 20;	// ������������� �������������
		// �������� ������ � ��������������� �� ����������������� �������:
		static const int OFB		= 0;	// ����������� �������� ������
		static const int EFB		= 1;	// ����������� �������� ������
		static const int SEMI		= 2;	// ����� � �������
		static const int COMMA		= 3;	// �������
		static const int ASSIGN		= 4;	// ������������
		static const int LESS_EQV	= 5;	// ������ ��� �����
		static const int GRT_EQV	= 6;	// ������ ��� �����
		static const int LESS		= 7;	// ������
		static const int GRT		= 8;	// ������
		static const int NOT_EQV	= 9;	// �� �����
		static const int EQV		= 10;	// �����
		static const int ADD		= 11;	// ��������
		static const int SUB		= 12;	// ���������
		static const int DIV		= 13;	// �������
		static const int MUL		= 14;	// ���������
		static const int MOD		= 15;	// ������� �� ������
		static const int NOT		= 16;	// ���������
		static const int OB			= 17;	// ����������� ������
		static const int EB			= 18;	// ����������� ������
		static const int IF			= 19;	// "����"
		static const int ELSE		= 20;	// "�����"
		static const int WHILE		= 21;	// "����"
		static const int ID			= 22;	// �������������
		static const int CNST		= 23;	// ���������
		static const int TYPE		= 24;	// ��� ������
		static const int RET		= 25;	// �������� �������� ��������

		Compiler *comp;

		// ������������� ������, ���������� ������ �������� � �� ����������:
		map<string,int> operation;
		// ������, ���������� ��� ��������� ���� ������:
		string types[1];

		ifstream code;	// ����� ��� ����� � �������� �����
		
		char ch;		// ������� ��������� ������
		string lex;		// ������� �������
		int lexType;	// ��� ������� �������

		Tree	*pt;		// ��������� �� ������ ���������
		Tree	*sTRoot;	// ��������� �� ������ ��������� (������������ ��� ����������� �������� ���������)

		// ����� ������ ��������������� �����������:
		void getError(int ec, string value);	// ����������� ������ �� �� ����
		void getChar();							// ��������� ���������� ������� �� �������� ������
		void getLex();							// ����������� �������
		void deleteTree(Tree *p);				// �������� �� ������ ���������������� ������

		// ������, ����������� ���������� ������ ��� ������������ ��������� ���������:
		void program();														// ���������
		void declaration(string type, string id, int lvl, Tree *p, int b);	// ���������� ����������
		void function();													// �������
		void functionArguments();											// ��������� �������
		void compound(Tree *p, int b);										// ��������� ��������
		void oper(Tree *p, int b);											// ��������
		void expression(Tree *p, int b, int be);							// ���������

	public:

		Tree					*head;		// ��������� �� ������ ������ ���������
		list<string>			ids;		// �������� ���������������
		map<string,Variable>	variables;	// �������� ���������� ����������
		map<string,Function>	functions;	// �������� ������� � ������� �� ��������
		map<string,Argument>	arguments;	// �������� ������� � �� ����������
		int status;							// ������ ������ ������� (1 - �������, ������ - ������)

		SyntaxAnalizer(Compiler *c, string fn);	// ������������� ��������������� �����������
		~SyntaxAnalizer();			// ������������ ������ � �������� �������

		void printTree(Tree *p);	// ����� �������������� ������ �������� ���������

};