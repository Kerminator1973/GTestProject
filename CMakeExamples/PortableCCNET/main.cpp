#include <iostream>
#include <sstream>  // Потребитель: vectorToString()
#include <iomanip>  // setfill(), setw()
#include <string>
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
#elif _WIN32
	const char* strPortName = "COM3";
#else
	// Android!?
#endif

	CCommPortBoost port(strPortName);

    std::vector<uint8_t> command = { 0x02,0x03,0x06,0x37,0xfe,0xc7 };

    //const std::vector<uint8_t> g_command = {0x02,0x03,0x06,0x37,0xfe,0xc7,0x02,0x03,0x06,0x37,0xfe,0xc7,0x02,0x03,0x06,0x37,0xfe,0xc7,0x02,0x03,0x06,0x37,0xfe,0xc7
     //                                      ,0x02,0x03,0x06,0x37,0xfe,0xc7,0x02,0x03,0x06,0x37,0xfe,0xc7,0x02,0x03,0x06,0x37,0xfe,0xc7,0x02,0x03,0x06,0x37,0xfe};

    if(port.Execute(command) != CCommPortBoost::OK)
    {
        std::cout<<__FUNCTION__<<"() Err"<<std::endl;
    }	

    auto data = port.GetResult();
	std::cout << vectorToString(data) << std::endl;
}
