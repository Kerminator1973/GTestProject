cmake_minimum_required(VERSION 3.10)

project(SQLiteInsertionBenchmark)

# Командой include можно подключить внешний скрипт. Ниже приведён пример запуска скрипта с именем "FindBenchmark.cmake"
#include(FindBenchmark)

# Командой message() можно выводить результаты трассировки
#message("Google Benchmark include directory: " ${benchmark_INCLUDE_DIRS})

# Ищем пути к заголовочным файлам и library-файлам библиотеки Google Benchmark
find_package(benchmark)

# Флаги актуальные для GCC. Бесполезны для компиляторов Visual Studio
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lbenchmark -std=c++11")

# Имя исполняемого файла будет соответствовать имени проекта
add_executable(${PROJECT_NAME} "")

# Указываем список файлов, которые должны использоваться для сборки приложения
target_sources(${PROJECT_NAME}
PRIVATE
    SQLiteInsertionBenchmark.cpp
    sqlite3.c
)

# Подключаем к проекту найденную библиотеку Google Benchmark
target_link_libraries(${PROJECT_NAME} benchmark::benchmark)
