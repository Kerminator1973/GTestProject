# Минимальной версий CMake является 3.22. В действительности, никакая специфика
# версий CMake во внимание, пока, не принимается и могли бы быть использованы более
# ранние версии. В Ubuntu 22.04 устанавливается CMake 3.22, а на Windows-машинах
# КБ ДОРС используются CMake 3.24
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

if (WIN32)
	# Исторически сложившаяся практика в ОП КБ ДОРС состоит в том, что Boost размещается
	# не стандартным образом, в не стандартных местах расположения на жёсстком диске и
	# функция find_package найти его не сможет. По этой причине, CMake нуждается в подсказке
	set( BOOST_ROOT c:/boost/ )
	set( BOOST_INCLUDEDIR c:/boost/include/boost-1_82/ )
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

# Имя проекта PortableDSlip, или dsliportable одной строкой
project(dsliportable LANGUAGES CXX)

# В C++ 17 очень много вкусных возможностей: std::string_view, std::optional, std::variant
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# В Ubuntu 22.04 доступен Boost 1.74, а на Windows-машинах в КБ ДОРС используется Boost: 1.76
find_package(Boost 1.74 REQUIRED COMPONENTS system)

# Указываем список файлов, участвующих в сборке проекта
add_executable(${PROJECT_NAME} 
	main.cpp 
	CommportBoost.h
	CommportBoost.cpp)

# Указываем подкаталог с Boost-ом, в котором нужно искать его include-файлы
target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# Указываем нужные нам библиотеки Boost. Для работы boost.asio достаточно system
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
