cmake_minimum_required(VERSION 3.10)

project(TestBoostAsIO)

add_subdirectory(CommPort)

# Hints: для того, чтобы find_package смог найти Boost, который находится по не стандартному пути
set( BOOST_ROOT $ENV{HOME}/boost/current/ )			# Preferred installation prefix.
set( BOOST_INCLUDEDIR $ENV{HOME}/boost/current/include/ )	# Preferred include directory e.g. <prefix>/include.
set( BOOST_LIBRARYDIR $ENV{HOME}/boost/current/lib_x86_64/ )	# Preferred library directory e.g. <prefix>/lib.


# После того, как мы подсказали где искать Boost, модуль find_package должен
# выполнить необходимые настройки, в том числе, установить необходимые переменные окружения
find_package(Boost 1.69 REQUIRED COMPONENTS program_options thread)

find_package(benchmark)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lbenchmark -std=c++11")

add_executable(${PROJECT_NAME}
    "main.cpp")

# Трассируем результаты поиска в консоль
message("Boost include directory: " ${Boost_INCLUDE_DIRS})
message("Boost library directory: " ${Boost_LIBRARY_DIRS})

# Указываем подкаталог с Boost-ом, в котором нужно искать его include-файлы
target_include_directories( ${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

# Указываем нужные нам библиотеки Boost, которые были перечислены в find_package()
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

target_link_libraries(${PROJECT_NAME} CommPort)
target_link_libraries(${PROJECT_NAME} benchmark)

