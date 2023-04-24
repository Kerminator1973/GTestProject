#include <iostream> // cout
#include <sstream>  // Потребитель: vectorToString()
#include <iomanip>  // setfill(), setw()
#include <string>
#include <locale>
#include <clocale>
#include"CommportBoost.h"


std::string vectorToString(const std::vector<uint8_t>& vec)
{
    std::stringstream result;
    for (const auto& v : vec)
    {
        result 
            << std::setfill('0') << std::setw(sizeof(v) * 2)
            << std::hex << +v;
    }
    return result.str();
}


int main(int argc, char *argv[]) {
	
#ifdef __linux__ 
   	const char* strPortName = "/dev/ttyDBA";

    std::setlocale(LC_ALL, "");
    std::locale loc("ru_RU.utf8");	// Указываем локализацию
    std::locale::global(loc);
    std::cout.imbue(loc);
    std::cerr.imbue(loc);
    std::clog.imbue(loc);
    std::wcout.imbue(loc);
    std::wcerr.imbue(loc);
    std::wclog.imbue(loc);
    std::ios::sync_with_stdio(false);

#elif _WIN32
	const char* strPortName = "COM3";

    std::setlocale(LC_ALL, "Russian");
    std::locale loc(".1251");	// Указываем кодовую страницу
    std::locale::global(loc);
#else
	// Android!?
#endif

	CCommPortBoost port(strPortName);
    switch (port.Execute(std::vector<uint8_t>{0x02, 0x03, 0x06, 0x37, 0xfe, 0xc7}))
    {
        case CCommPortBoost::OK:
            std::cout << __FUNCTION__ << "() Successful" << std::endl;
            std::cout << vectorToString(port.GetResult()) << std::endl;
            break;
        case CCommPortBoost::CONNECT_ERR:
            std::cout << __FUNCTION__ << "() Connection error" << std::endl;
            std::cout << port.GetErrorMessage() << std::endl;
            break;
        case CCommPortBoost::WRITE_ERR:
            std::cout << __FUNCTION__ << "() Write error" << std::endl;
            std::cout << port.GetErrorMessage() << std::endl;
            break;
        case CCommPortBoost::READ_ERR:
            std::cout << __FUNCTION__ << "() Read error" << std::endl;
            std::cout << port.GetErrorMessage() << std::endl;
            break;
    }
}
