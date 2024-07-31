#include <iostream>     // cout
#include <boost/program_options.hpp>
#include "convert/convert.h"

namespace po = boost::program_options;
using namespace std;


int main(int argc, char *argv[]) {

    po::options_description desc{ "Options" };
    po::variables_map vm;

    desc.add_options()
        ("help,h", "Display the list of options")
        ("getfilename,f", "Display only a filename")
        ;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc),vm);

        // Если параметры не указаны, или если указан ключ "help", то показываем справочную информацию
        if (vm.count("help") || argc == 1) {
            cout << desc << endl;
            return 0;
        }

        po::notify(vm);
       
        if (vm.count("getfilename")) {

            std::string strFullname("c:\\temp\\123.txt");
            cout << GetOnlyFilename(strFullname) << endl;
            
            return 0;
        }

        return 0;
    }
    catch (po::error& err)
    {
        std::cout << err.what() << std::endl;
        std::cout << desc << std::endl;
        return -1;
    }
}
