# Из чего состоит CMakeLists.txt

>28 марта 2025 года официально [выпущен CMake 4.0.0](https://www.kitware.com/cmake-4-0-0-available-for-download/).

Создание файла "CMakeLists.txt" начинается с определения требований к версии CMake:

``` cmake
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
```

Указание корректной версии CMake может быть критичным, т.к. добавляется поддержка новых языков программирования и директив (так, например, в 3.26 появилась поддержка Microsoft ASM), появляются новые команды и переменные окружения. Чем сложнее скрипт сборки тем более чувствительным он является к версии CMake.

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

Или можно установить свойства групповой операцией, например:

``` cmake
set_target_properties(dsliportable PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS NO
)
```

Выбор стандарта C++ может быть важной задачей. Если выбрать C++ 11, то можно использовать специализированные компиляторы, надёжно работающие на устаревающих платформах. Однако, актуальные стандарты C++ предоставляют мощные инструментальные возможности, например, в C++ 17 появилась поддержка типов: std::string_view, std::optional, std::variant.

Указываем список файлов, участвующих в сборке проекта можно используя директиву add_executable:

``` cmake
add_executable(${PROJECT_NAME} 
    main.cpp 
    CommportBoost.h
    CommportBoost.cpp)
```

## Недостатки CMake

CMake не корректирует структуру проектов автоматически, например, при добавлении разработчиком новых файлов. Если программист добавил что-то, необходимо перегенерировать проекты для сборки кода, что может занимать значительное количество времени.

## Альтернативы CMake

В качестве альтернативы CMake рассматривается проект Google под названием [Bazel](https://bazel.build/). Для установки инструмента может потребоваться система установки под названием **Bazelisk**. Инструмент может быть использован в разных операционных системах и для разных языков программирования (из технологического стека Google): C++, Go, Java. Одна из главных feature Bazel - интеграция в другими системами сборки, включая CMake. Цель - для генерации скрипта сборки проекта использовать скрипты от CMake, если отсутствует готовый скрипт для Bazel.

Новичок среди систем сборки - [Meson](https://mesonbuild.com/)

## Переменные CMake, связанные с директориями проекта

В CMakeLists.txt часто используются следующие переменные:

- PROJECT_SOURCE_DIR - переменная CMake, которая хранит путь к директории с исходным кодом текущего проекта, то есть к той папке, где в текущей области видимости (или в одном из родительских уровней) был вызван макрос project()
- PROJECT_BINARY_DIR - путь к директории сборки (бинарников) для текущего проекта — того, чей project() был вызван последним в текущей области видимости
- CMAKE_SOURCE_DIR - корень исходного дерева всего проекта — директория, где лежит самый верхний (корневой) CMakeLists.txt, с которого начали конфигурировать сборку. Особенность: не меняется внутри подпроектов. Это "абсолютный" корень исходников
- CMAKE_BINARY_DIR - директория сборки самого верхнего уровня — куда CMake пишет всё для всей сборки
- CMAKE_CURRENT_SOURCE_DIR - директория, в которой сейчас обрабатывается "CMakeLists.txt". Ключевое отличие: не зависит от project(), а зависит от того, какой файл CMake читает в данный момент
- CMAKE_CURRENT_BINARY_DIR - директория сборки, соответствующая текущей директории исходников
- CMAKE_CURRENT_LIST_DIR -  директория файла CMake, который прямо сейчас выполняется (включая include() файлы)

Важное отличие от CMAKE_CURRENT_SOURCE_DIR: если вы сделали include(some_file.cmake), то:

- CMAKE_CURRENT_SOURCE_DIR остаётся тем же (директория текущего проекта/уровня)
- CMAKE_CURRENT_LIST_DIR становится директорией файла "some_file.cmake"

Пример трассировки:

``` cmake
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
```

Пример использования PROJECT_SOURCE_DIR — подключаем ресурсы из корня проекта:

``` cmake
include_directories("${PROJECT_SOURCE_DIR}/include")
```

### Ключевые слова PRIVATE, PUBLIC и INTERFACE

Ключевые слова PRIVATE, PUBLIC и INTERFACE в CMake управляют тем, как свойства (например, пути к заголовочным файлам, библиотеки для линковки, флаги компиляции) распространяются между целями (targets) при использовании команд target_link_libraries, target_include_directories, target_compile_definitions и подобных.

**PRIVATE**: свойство используется только самой целью и не передаётся тем, кто ссылается на неё:

```cmake
add_library(utils STATIC utils.cpp)

target_include_directories(utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/internal)
```

Цель utils будет видеть заголовки из `internal/`, но любой исполняемый файл или библиотека, которые подключат utils, — не будут. Это инкапсуляция: внутренние детали реализации скрыты от потребителей.

**PUBLIC**: свойство используется и самой целью, и всеми, кто на неё ссылается. Применяется, когда заголовки библиотеки в своём публичном API включают файлы из каких-то директорий или требуют определённых флагов.

``` cmake
add_library(math STATIC math.cpp)

target_include_directories(math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(math PUBLIC boost::system)
```

Здесь и сама math, и любой потребитель (target_link_libraries(my_app math)) получат путь `include/`, а также получать указание на необходимость выполнить подключение к каждому из них библиотеки boost::system.

**INTERFACE**: свойство не используется самой целью, но передаётся всем, кто на неё ссылается. Это классический случай для header-only библиотек (у которых нет своих единиц компиляции) или для целей-интерфейсов, задающих требования.

``` cmake
add_library(header_only INTERFACE)

target_include_directories(header_only INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(header_only INTERFACE USE_FANCY_MACRO=1)
```

Сама header_only ничего не компилирует, но любой, кто сделает target_link_libraries(my_app header_only), получит и путь к заголовкам, и определение макроса USE_FANCY_MACRO.

## Подключение библиотек Boost

Подключение библиотеки Boost не всегда является тривиальной задачей. CMake определяет местонахождение библиотек Boost в разных операционных системах, но это происходит только в том случае, если Boost размещён в "стандарных подкаталогах", т.е. был установлен через app-get, или собран из исходников проекта с указанием на необходимость инсталляции библиотек.

Однако, при выполнении "стандартной установки" библиотеки Boost в Microsoft Windows, командой `b2 install`, CMake всё равно не может найти необходимые файлы Boost. Также существует проблема сборки приложений, использующих разные версии Boost на одной машине. В обоих случаях, в файле "CMakeLists.txt" требуется внести директивы, подсказывающие, где размещены файлы библиотеки.

Для сборки в Windows может потребоваться явным образом указывать размещение файлов Boost (если не используется vcpkg):

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

>Начиная с версии 3.30, правило CMP0167 определяет каким образом будет осуществляться поиск библиотеки Boost. В старом подходе используется модуль CMake FindBoost, а в новом подходе используется скрипт BoostConfig.cmake. Если правило не установлен, CMake использует старое поведение и предупреждает о том, что операция может быть не успешной. Установка переменной осуществляется следующием образом: `cmake_policy(SET CMP0167 NEW)`.
>
>Скрипт BoostConfig.cmake появился в The Boost Library начиная с версии 1.70.
>
>В частности, без установки правила CMP0167 в значение NEW, CMake может не обнаружить Boost, загруженный менеджером пакетов **vcpkg**.

После ключевого слова COMPONENTS перечисляются необходимые в проекте библиотеки, например:

``` cmake
find_package(Boost 1.74 REQUIRED COMPONENTS system locale filesystem thread)
```

Если библиотеки будут найдены, то CMake создаст переменную `Boost_LIBRARIES`, которую можно будет использовать, например, в команде **target_link_libraries**, которая включает в генерируемый проект ссылки на линкуемые библиотеки:

``` cmake
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
```

Также следует включить в "CMakeLists.txt" инструкцию, включающую ссылку на папку с заголовочными файлами Boost в проект сборки:

``` cmake
target_include_directories( ${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
```

## Использование plug-ins CMake для Visual Studio Code

Наиболее популярные plug-ins разработаны Microsoft (**CMake Tools**), **twxs** и **Jose Torres** и содержат инструменты IntelliSense для написания скриптов сборки CMakeLists.txt.

Одна из наиболее полезных функций - выбор Kit-а для сборки исходников. Сделать это можно средствами CMake Tools, через "Command Palette..." и команду "CMake: Select a Kit".

## Адаптация CMakeLists.txt под особенности платформы сборки

Бывают ситауции, когда некоторые библиотеки доступны только на конкретной аппаратной платформе. В подобных ситуациях может потребоваться вводить в файл "CMakeLists.txt" конструкции, учитывающие особенности таких платформ. Например:

```cmake
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
	message(STATUS "Cross-compiling for aarch64 (ARM64)")

	# Подключаем библиотеку wiringPi
	target_link_libraries(${PROJECT_NAME} wiringPi)
endif ()
```

Заметим, что для 32-ых битных архитектур процессоров ARM используется строка поиска "^ARM", а для 64-ех битных - "^aarch64".
