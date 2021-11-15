# Сборка и выполнение тестов средствами Google Test

Сборка осуществлялась в операционной системе Windows 10. В системе был установлен компилятор С++ из состава **Visual Studio 16** (2019).

Разработка скрипта велась с использованием рекомендаций из книги Rodovan Bast и Roberto Di Remigio «CMake Cookbook: Building, testing, and packaging modular software with modern CMake».

## Команды сборки

Перед началом сборки следует переместиться в подкаталог "\Contributors\baston\CMake_GoogleTest".

Для сборки выполняются следующие команды:

```
mkdir build
cd build
cmake ..
cmake --build .
```

По умолчанию, выполняется сборка исполняемых файлов в Debug-конфигурации. Ожидаемый результат - в папке "\Contributors\baston\CMake_GoogleTest\Debug" должны будут появиться файлы "sum_up.exe" и "cpp_test.exe".

## Команды выполнения теста

Запустить тесты на выполнение можно из папки "build", указав имя конфигурации (Debug) в командной строке:

```
ctest -C Debug
```

Приложение ctest завершится с нулевым кодов, а в консоль будут выведены следующие данные:

```
Test project K:/Playground/test2/build
    Start 1: google_test
1/1 Test #1: google_test ......................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.03 sec
```

Для детального разбора проблем в работе тестов можно использовать файлы из подкаталога "Testing/Temporary/", в частности - файл "LastTest.log". 

Чтобы убедиться, что тесты работают, можно внести изменение в файл "test.cpp". Заменим следующую строку:

```
if (sum_integers(integers) == 15) {
```

на:

```
if (sum_integers(integers) == 208) {
```

Результат выполнения теста будет негативный - тесты не выполнены успешно. Код завершения команды `ctest -C Debug` будет отличаться от нулевого, а в консоль будет выведена информация об ошибке тестирования:

```
Test project K:/Playground/test2/build
    Start 1: google_test
1/1 Test #1: google_test ......................***Failed    0.04 sec

0% tests passed, 1 tests failed out of 1

Total Test time (real) =   0.04 sec

The following tests FAILED:
          1 - google_test (Failed)
Errors while running CTest
```

## Результат выполнения задачи

Можно с уверенностью утверждать, что Google Test может быть использован для проведения Unit-тестирования программного кода, написанного на C++.

Утилита `ctest` может быть использована в система CI/CD, в частности, как задача [Jenkins](https://www.jenkins.io/).
