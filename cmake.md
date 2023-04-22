# Из чего состоит CMakeLists.txt

Создание файла "CMakeLists.txt" начинается с определения требований к версии CMake:

``` cmake
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
```

Указать имя проекта и используемый для его сборки язык программирования можно директивой **project**:

``` cmake
project(dsliportable LANGUAGES CXX)
```

Настройка ключей компиляции осуществляется директивами **set**:

``` cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

Выбор стандарта C++ может быть важной задачей. Если выбрать C++ 11, то можно использовать специализированные компиляторы, надёжно работающие на устаревающих платформах. Однако, актуальные стандарты C++ предоставляют мощные инструментальные возможности, например, в C++ 17 появилась поддержка типов: std::string_view, std::optional, std::variant.

Указываем список файлов, участвующих в сборке проекта можно используя директиву add_executable:

``` cmake
add_executable(${PROJECT_NAME} 
    main.cpp 
    CommportBoost.h
    CommportBoost.cpp)
```

## Подключение библиотек Boost

Подключение библиотеки Boost не всегда является тривиальной задачей. CMake определяет местонахождение библиотек Boost в разных операционных системах, но это происходит только в том случае, если Boost размещён в "стандарных подкаталогах", т.е. был установлен через app-get, или собран из исходников проекта с указанием на необходимость инсталляции библиотек. 

Однако, при выполнении "стандартной установки" библиотеки Boost в Microsoft Windows, командой `b2 install`, CMake всё равно не может найти необходимые файлы Boost. Также существует проблема сборки приложений, использующих разные версии Boost на одной машине. В обоих случаях, в файле "CMakeLists.txt" требуется внести директивы, подсказывающие, где размещены файлы библиотеки.

Исходя из опыта, для сборки в Windows почти всегда нужно явным образом указывать размещение файлов Boost:

``` cmake
if (WIN32)
    set( BOOST_ROOT c:/boost/ )
    set( BOOST_INCLUDEDIR c:/boost/include/boost-1_76/ )
    set( BOOST_LIBRARYDIR c:/boost/lib )    
endif (WIN32)
```

Для Linux, почти никогда не нужно подсказывать местоположение Boost, хотя иногда это и бывает необходимым:

``` cmake
if (Unix)
    set( BOOST_ROOT $ENV{HOME}/boost/current/ )
    set( BOOST_INCLUDEDIR $ENV{HOME}/boost/current/include/ )
    set( BOOST_LIBRARYDIR $ENV{HOME}/boost/current/lib_x86_64/ )
endif (WIN32)
```

Настройка параметров сборки приложения с использованием Boost осуществляется командной find_package():

``` cmake
find_package(Boost 1.74 REQUIRED COMPONENTS system)
```

Также следует включить в "CMakeLists.txt" инструкцию, включающую ссылку на папку с заголовочными файлами Boost в проект сборки:

``` cmake
target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
```

В проект нужно включить и ссылки на линкуемые библиотеки:

``` cmake
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
```
