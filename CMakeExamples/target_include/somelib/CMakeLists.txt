cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(somelib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем файлы, которые входят в состав библиотеки
add_library(${PROJECT_NAME}
    somelib.cpp
    somelib.h
    )

#target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
