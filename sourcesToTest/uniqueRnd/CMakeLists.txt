cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(random-number LANGUAGES CXX)

# Если необходима интеграция с CDash, следует раскомментировать следующую строку
#include(CTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.54 REQUIRED COMPONENTS filesystem)

add_executable(test-random 
	test.cpp
	uniqueRnd.cpp
	uniqueRnd.h)

target_link_libraries(test-random
	PUBLIC
		Boost::filesystem
	)

# Активируем функционал Unit-тестирования проекта
enable_testing()

# Добавляем тест - запуск собранного приложения с тестами
add_test(
	NAME main_test
		COMMAND $<TARGET_FILE:test-random> "init"
	)
	
add_test(
	NAME persistence_test
		COMMAND $<TARGET_FILE:test-random>
	)
