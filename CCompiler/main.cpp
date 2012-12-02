/**
* ���������� ������������ ����� C
*
* ������� �������� ���� �����������.
*
* �����:			������� ������, ��. ��-110
* ���� ��������:	2012-05-11 23:30
*/

#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <tchar.h>
#include <map>

#include "Compiler.h"
#include "SyntaxAnalizer.h"
#include "CodeGenerator.h"
#include "Function.h"

using namespace std;

Compiler *comp;

/**
* ����� �����.
* argc - ���������� ����������, ���������� �� ��������� ������.
* argv - ������ ����������.
*/
int main(int argc, char *argv[]) {

	cout << endl << "C compiler ver. 0.1" << endl;
	cout << "Copyright (C) Aleksey Pyatkin (VT-110 group), 2012" << endl;

	if (argc > 1) { // ������� ������� �����

		char *sfn = argv[1];			// ��� ����� �� �������� ����� C ��� ����������
		char *esfn = new char[1024];	// ��� ����� ��������� � �����������
		strcpy(esfn, sfn);
		// ����������� ���������� � ����� �����
		if (strlen(sfn) > 1 && !(sfn[strlen(sfn)-2] == '.' && sfn[strlen(sfn)-1] == 'c')) {
			strcat(esfn, ".c");
		} else {
			sfn[strlen(sfn)-2] = 0;
		}
		char *efn = new char[1024]; // ��� ������������ �����
		if (argc > 2) {
			strcpy(efn, argv[2]);
		} else {
			strcpy(efn, "");
		}

		comp = new Compiler(sfn);

		// ������ ����������:
		cout << endl << "Starting of the compilation..." << endl;
		cout << "Program: " << esfn << endl;

		// ���� ��������������� �������:
		cout << endl << "============================ Parsing ===========================" << endl << endl;

		SyntaxAnalizer *sa = new SyntaxAnalizer(comp, sfn);
		if (sa->status == 1) { // ������ ������ �������
	
			if (!sa->functions.empty()) {

				cout << "Program parsing is OK. See the information in the " << sfn << ".log file." << endl;

				// ������� ������� � ���� �����
				for (map<string,Function>::iterator iter = sa->functions.begin(); iter != sa->functions.end(); iter++) {
					sa->printTree((*iter).second.address);
					comp->logs << endl;
				}

				// ���� ��������� ����
				cout << endl << "==================== Generating of the code ====================" << endl << endl;
				CodeGenerator *cg = new CodeGenerator(sa->head, sa->variables, sfn);
				if (cg->status == 1) { // ��������� ���� ����������� �������
			
					cout << "The code generating had already done. You can see the code in the " << sfn << ".asm file." << endl;

					// ���������������
					cout << endl << "========================== Assembling ==========================" << endl << endl;
					char *cAsm = new char[1000];
					strcat(strcat(strcpy(cAsm, "cmd /C ml /c "), sfn), ".asm");
					WinExec(cAsm, NULL);
					Sleep(350);
					// ��������
					cout << endl << "============================ Linking ===========================" << endl << endl;
					char *cLink = new char[1000];
					strcat(strcat(strcpy(cLink, "cmd /C link16 "), sfn), ".obj");
					if (argc > 2) {
						strcat(strcat(strcat(cLink, ", "), efn), ".exe");
					}
					strcat(cLink, " ;");
					WinExec(cLink, NULL);
					Sleep(350);

					delete[] cAsm;
					delete[] cLink;
				
				} else {
					comp->printError("There was a problem during the code generating.");
				}
				delete cg;
			
			} else {
				cout << "The program is empty!" << endl;
			}
			
		} else {
			comp->printError("There was a problem during the parsing of a code.");
		}
		delete sa;
		delete[] esfn;
		delete[] efn;
		
	} else {
		cerr << endl << "Format of the command:" << endl;
		cerr << "\tfirststep source_name [exe_name]" << endl;
	}
	
	return 0;

}