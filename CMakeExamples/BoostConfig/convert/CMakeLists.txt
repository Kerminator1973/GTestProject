cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(convert)

# Указываем файлы, которые входят в состав библиотеки
add_library(${PROJECT_NAME}
    convert.cpp
    convert.h
    )

# Include the Boost configuration
include(../BoostConfig.cmake)

find_package(Boost 1.74 REQUIRED COMPONENTS filesystem)

# Указываем подкаталог Boost-а, в котором следует искать его include-файлы
target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# Указываем нужные нам библиотеки Boost. В общем случае, указываем все библиотеки
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
