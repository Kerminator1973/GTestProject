cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(playground LANGUAGES CXX)

# Подкаталоги, в которых следует искать дополнительные проекты
add_subdirectory(somelib)
add_subdirectory(unittest)

# Требование стандарта C++20 связано с использованием std::span<>
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем список файлов, участвующих в сборке проекта
add_executable(${PROJECT_NAME} main.cpp)

# Подключаем библиотеку, которая собирается в папке "ccnet"
target_link_libraries(${PROJECT_NAME} somelib)
