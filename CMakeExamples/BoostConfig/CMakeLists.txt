cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(getfilename LANGUAGES CXX)

# Подкаталоги, в которых следует искать дополнительные проекты
add_subdirectory(convert)

# Требование стандарта C++20 связано с использованием std::span<>
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем список файлов, участвующих в сборке проекта
add_executable(${PROJECT_NAME} main.cpp)

# Include the Boost configuration
include(BoostConfig.cmake)

find_package(Boost 1.74 REQUIRED COMPONENTS program_options)

# Указываем подкаталог Boost-а, в котором следует искать его include-файлы
target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# Подключаем библиотеку, которая собирается в папке "convert"
target_link_libraries(${PROJECT_NAME} convert ${Boost_LIBRARIES})
