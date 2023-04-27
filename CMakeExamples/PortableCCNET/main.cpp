#include <iostream> // cout
#include <sstream>  // Потребитель: vectorToString()
#include <iomanip>  // setfill(), setw()
#include <string>
#include <locale>   // Локализация текстовой консоли
#include <clocale>  // Локализация текстовой консоли
#include <chrono>   // Замер времени выполнения обмена данными с прибором BVS
#include "ccnet/device.h"

#ifdef __linux__
using BYTE = unsigned char;
using WORD = unsigned short;
using DWORD = unsigned long;
using DWORD_PTR = unsigned long;
using PDWORD_PTR = DWORD_PTR*;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#endif


std::string vectorToString(const std::span<uint8_t> vec)
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

    auto begin = std::chrono::high_resolution_clock::now();

    CCNetDevice device(strPortName);
    // Выполняем десять операций подряд, с целью определить производительность
    // обмена данными между прибором BVS и персональным компьютером
    for (int i = 0; i < 10; i++) {

        CCNetDeviceIdentification ident;
        if (device.Identification(ident))
        {
            std::cout << "Request #" << i + 1 << " if successfully requested\n";
            //std::cout << vectorToString(device.GetResult()) << std::endl;
            std::cout << "  Part Number: " << ident.PartNumber << '\n';
            std::cout << "  Module Number: " << ident.ModuleNumber << '\n';
            std::cout << "  Software Version: " << (int)HIBYTE(HIWORD(ident.SoftwareVersion)) 
                << "." << (int)LOBYTE(HIWORD(ident.SoftwareVersion))
                << "." << (int)LOWORD(ident.SoftwareVersion) << '\n';
            std::cout << "  Notebase Version: " << (int)HIWORD(ident.NotebaseVersion)
                << "." << (int)HIBYTE(LOWORD(ident.NotebaseVersion))
                << "." << std::setw(3) << std::setfill('0') << (int)LOBYTE(LOWORD(ident.NotebaseVersion)) << '\n';
        }
        else
        {
            device.PrintError();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    std::cout << "Time measured: " << elapsed.count() * 1e-9 << " seconds\n";
}
