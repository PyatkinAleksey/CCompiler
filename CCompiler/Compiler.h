/**
* Компилятор подмножества языка C
*
* Класс, содержащий методы, используемые во всем компиляторе.
*
* Автор:			Алексей Пяткин, гр. ВТ-110
* Дата создания:	2012-09-08 13:40
*/

#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
* Класс, содержащий методы, используемые во всем компиляторе.
*/
class Compiler {

	public:

		ofstream logs; // Поток для файла ведения логов

		Compiler(string fn);	// Инициализация
		~Compiler();			// Освобождение памяти и закрытие потоков

		void printError(string msg);	// Вывод сообщений об ошибках

};