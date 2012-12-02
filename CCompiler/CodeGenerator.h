/**
* ���������� ������������ ����� C
*
* �����, ����������� ��������� ������������� ����.
*
* �����:			������� ������, ��. ��-110
* ���� ��������:	2012-05-19 16:00
*/

#include <fstream>
#include <string>
#include <list>
#include <map>

#include "Tree.h"
#include "Variable.h"
#include "Compiler.h"

/**
* �����, ����������� ��������� ������������� ����.
* ��������� ������������ �� ������, ������������� �������������� ������������.
*/
class CodeGenerator {

	private:

		ofstream casm; // ����� ��� ������������� ����

		Tree *headTree; // ��������� �� ������� ������, ���������������� �������������� ������������

		int marks;					// ������� �����
		map<string,Variable> vars;	// ������ ��������������� �������� ���������

		void genBegin();						// ��������� ���� ������ ������������ ���������
		void genCode(Tree *pt);					// ��������� ���� ��� ��������� pt
		void genEmpty();						// ��������� ���� ������� ���������
		void genExpression(Tree *pt, int lvl);	// ��������� ���� ��� ���������
		void genNegative(Tree *pt);				// ��������� ���� �������� ����� �����
		void genAssignment(Tree *pt);			// ��������� ���� ��� ��������� ������������
		void genIf(Tree *pt);					// ��������� ���� ��� ��������� ��������� IF
		void genWhile(Tree *pt);				// ��������� ���� ��� ��������� ����� WHILE
		void genEnd();							// ��������� ���� ����� ������������ ���������

	public:

		int status; // ������ ���������� ��������� ���� (1 - �������, ������ - ������)

		CodeGenerator(Tree *pT, map<string,Variable> vars, string fn);	// ������������� ���������� ����

};