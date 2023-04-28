cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(unittest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOMELIB_DIRECTORY "./../somelib")
message(status "Set path to library directory ${SOMELIB_DIRECTORY}")

add_executable(cpp_test "")

target_include_directories(cpp_test PRIVATE ${SOMELIB_DIRECTORY})

target_sources(cpp_test
	PRIVATE
	test.cpp
)

#	${SOMELIB_DIRECTORY}/somelib.h

target_link_libraries(cpp_test
	PRIVATE
		somelib
)
