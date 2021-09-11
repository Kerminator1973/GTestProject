# Сборка демонстрационного проекта

Рекомендуется создать в папке с проектом "CMakeExamples\BuildWithGoogleTest" подпапку с именем "build" и перейти в неё:

```
mkdir -p build
cd build
```

Запустить процесс поиска компиляторов и генерацию скрипта сборки для конкретной машины можно выполнить командой: `cmake ..`. Поскольку мы находимся внутри папки **build** две точки указывают, что сборку нужно начать в папке на уровень выше. Смысл в созании папки build и генерации скриптов сборки в неё состоит в простой процедуре удаляения всех временных файлов и папок - вполне достаточно удалить папку **build** со всем, что в ней находится.

Сборка проекта найденными компиляторами осуществляется командой: `cmake --build .`

Запустить тесты на выполнение можно командой: `ctest -C Debug`. В данном случае, используется конфигурация **Debug**, но может быть использованы и другие конфигурации, например, **Release**.

# Как работает CMakeLists.txt (из папки BuildWithGoogleTest)

Файл "sum_integers.cpp" используется для сборки библиотеки sum_integers (sum_integers.dll):

``` cmake
add_library(sum_integers sum_integers.cpp)
```

Исполняемый файл sum_up (sum_up.exe) собирается из файла "main.cpp":

```
add_executable(sum_up main.cpp)
target_link_libraries(sum_up sum_integers)
```

Приложение sum_up, в нашем примере, как-будто передаётся клиентам. Оно умеет суммировать все числа, которые будут переданы через командную строку, например:

```
sum_up 3 4 5
```

Результат вызова утилиты будет: 12.

Далее в скрипте, мы определяем настройку, которая активирует, или отключает Unit-тестирование. По умолчанию, настройка включена:

```
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")
```

Если Unit-тестирование включено, то подключается дополнительная команда **FetchContent*, которая позволяет выкачать исходные тексты Google Test с указанного репозитария, с указанием конкретного Tag-а. 

```
if(ENABLE_UNIT_TESTS)
  include(FetchContent)
```


Рекомендуется использовать актуальные выпуски библиотеки, ознакомившись с содержимым [репозитария на GitHub](https://github.com/google/googletest). Настройка параметров репозитария осуществляется так:

```
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
  )
```

Получение дополнительных параметров из загруженной библиотеки выполняется командой:

```
  FetchContent_GetProperties(googletest)
```

Непосредственная загрузка данных из репозитария осуществляется командой:

```
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
```

В скрипте есть специализированные команды, устраняющие проблемы совместимости с определёнными компиляторами - эту часть скрипта можно пока опустить.

Важным является включение в проект подкаталогов из загруженного репозитария:

```
    add_subdirectory(
      ${googletest_SOURCE_DIR}
      ${googletest_BINARY_DIR}
      )
```

Ещё один исполняемый файл будет называться **cpp_test** и именно он будет содержать Unit-тесты для библиотеки **sum_integers**:

```
  add_executable(cpp_test "")

  target_sources(cpp_test
    PRIVATE
      test.cpp
    )

  target_link_libraries(cpp_test
    PRIVATE
      sum_integers
      gtest_main
    )
```

Финальная часть скрипта - запуск на исполнения файла "cpp_test" с целью выполнения Unit-тестов и сборки статистики выполнения:

```
  enable_testing()

  add_test(
    NAME google_test
    COMMAND $<TARGET_FILE:cpp_test>
    )
endif()
```

Тесты считаются выполненными, если "cpp_test" завершается с кодом 0. Любой другой код сигнализирует об ошибке выполнения Unit-тестов.
