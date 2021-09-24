#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include "uniqueRnd.h"

using namespace std;
using namespace boost::filesystem;

int main(int argc, char *argv[]) 
{
	std::setlocale(LC_ALL, "");
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale());
	std::wcerr.imbue(std::locale());

	// Определяем, запущен ли тест в режиме инициализации, или проверки
	// корректности работы с постоянным хранилищем
	const bool bInit = (argc == 2) && (std::string("init") == argv[1]);

	// Заменяем имя приложения, именем файла, в котором будут храниться
	// ранее сгенерированные случайные номера.
	// Оператор / реализует функцию добавление ещё одной группы подкаталогов,
	// или имени файла
	path p;
	auto filename = ((p = argv[0]).remove_filename() / "emv.random.history");

	// При первом запуске - удаляем файл с псевдо-случайными числами
	if (bInit) {
		cout << "Trying to remove a file: " << filename << std::endl;
		boost::filesystem::remove(filename);
	}

	// Инициализируем объект генерации случайных чисел
	UniqueRandomNumbersChecker checker(filename.string());
	
	// TODO: Сделать шаблон с инициализатором количества элементов

	uint32_t valueToRegister = 8500;

	if (bInit) {

		// Первоначальное циклическое добавление элементов в список
		for (int i = 1; i < 13600; i++) {
			if (!checker.RegisterIfUnique(i)) {
				cout << "Failure on Insertion a number: " << i << std::endl;
				return EXIT_FAILURE;
			}
		}

		valueToRegister++;
	}

	// Проверяем, что в списке есть числа, которые были установлены во время
	// последних 5000 вызовов
	if (checker.RegisterIfUnique(12404)) {
		cout << "Failure: the buffer has no actual number" << std::endl;
		return EXIT_FAILURE;
	}

	// Проверяем, что самых первых чисел в массиве уже нет
	if (!checker.RegisterIfUnique(valueToRegister)) {
		cout << "Failure: the buffer is not cycling" << std::endl;
		return EXIT_FAILURE;
	}

	cout << "Success: everything is OK" << std::endl;
	return EXIT_SUCCESS;
}