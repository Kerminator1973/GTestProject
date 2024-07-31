#include <boost/filesystem.hpp>
#include "convert.h"

std::string GetOnlyFilename(const std::string& strFullName)
{
    return boost::filesystem::path(strFullName).filename().string();
}
