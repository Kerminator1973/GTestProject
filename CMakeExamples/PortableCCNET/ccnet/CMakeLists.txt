cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(ccnet)

if (WIN32)
	# Исторически сложившаяся практика в ОП КБ ДОРС состоит в том, что Boost размещается
	# не стандартным образом, в не стандартных местах расположения на жёсстком диске и
	# функция find_package найти его не сможет. По этой причине, CMake нуждается в подсказке
	set( BOOST_ROOT c:/boost/ )
	set( BOOST_INCLUDEDIR c:/boost/include/boost-1_76/ )
	set( BOOST_LIBRARYDIR c:/boost/lib )    
endif (WIN32)

# CMake определяет местонахождение библиотек Boost в разных операционных системах, но это происходит 
# только в том случае, если Boost размещён в "стандарных подкаталогах", т.е. был установлен 
# через app-get, или собран из исходников проекта с указанием на необходимость инсталляции библиотек.
# В данном проекте используется стандартные папки для размещения Boost
#
#if (Unix)
#	set( BOOST_ROOT $ENV{HOME}/boost/current/ )
#	set( BOOST_INCLUDEDIR $ENV{HOME}/boost/current/include/ )
#	set( BOOST_LIBRARYDIR $ENV{HOME}/boost/current/lib_x86_64/ )
#endif (Unix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# В Ubuntu 22.04 доступен Boost 1.74, а на Windows-машинах в КБ ДОРС используется Boost: 1.76
find_package(Boost 1.74 REQUIRED COMPONENTS system)

# Указываем файлы, которые входят в состав библиотеки
add_library(${PROJECT_NAME}
    CommportBoost.cpp
    CommportBoost.h
    )

# Указываем подкаталог Boost-а, в котором следует искать его include-файлы
target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# Указываем нужные нам библиотеки Boost. Для работы boost.asio достаточно system
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
