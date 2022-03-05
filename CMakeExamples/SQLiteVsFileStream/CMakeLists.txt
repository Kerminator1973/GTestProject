cmake_minimum_required(VERSION 3.10)

project(SQLiteInsertionBenchmark)

# Ищем установленную копию Google Benchmark
#find_package(benchmark)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lbenchmark -std=c++11")

# 
add_executable(${PROJECT_NAME} "")

# Указываем список файлов, которые должны использоваться для сборки приложения
target_sources(${PROJECT_NAME}
PRIVATE
    SQLiteInsertionBenchmark.cpp
    sqlite3.c
)    

# Подключаем к проекту найденную библиотеку Google Benchmark
#target_link_libraries(${PROJECT_NAME} benchmark)