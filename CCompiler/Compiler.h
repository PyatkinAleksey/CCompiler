/**
* ���������� ������������ ����� C
*
* �����, ���������� ������, ������������ �� ���� �����������.
*
* �����:			������� ������, ��. ��-110
* ���� ��������:	2012-09-08 13:40
*/

#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
* �����, ���������� ������, ������������ �� ���� �����������.
*/
class Compiler {

	public:

		ofstream logs; // ����� ��� ����� ������� �����

		Compiler(string fn);	// �������������
		~Compiler();			// ������������ ������ � �������� �������

		void printError(string msg);	// ����� ��������� �� �������

};