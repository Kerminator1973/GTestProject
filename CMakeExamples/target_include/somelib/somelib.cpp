#include <iostream> // std::cout
#include "somelib.h"


SomeLib::SomeLib()
{
	std::cout << "SomeLib constructor\n";
}

int SomeLib::hello()
{
	std::cout << "SomeLib hello\n";
	return 9;
}
