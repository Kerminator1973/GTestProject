# Минимальной версий CMake является 3.22. В действительности, никакая специфика
# версий CMake во внимание, пока, не принимается и могли бы быть использованы более
# ранние версии. В Ubuntu 22.04 устанавливается CMake 3.22, а на Windows-машинах
# КБ ДОРС используются CMake 3.24
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

# Имя проекта PortableDSlip, или dsliportable одной строкой
project(dsliportable LANGUAGES CXX)

add_subdirectory(ccnet)

# В C++ 17 очень много вкусных возможностей: std::string_view, std::optional, std::variant
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем список файлов, участвующих в сборке проекта
add_executable(${PROJECT_NAME} main.cpp)

# Подключаем библиотеку, которая собирается в папке "ccnet"
target_link_libraries(${PROJECT_NAME} ccnet)
