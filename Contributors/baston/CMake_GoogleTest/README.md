# Сборка и выполнение тестов средствами Google Test

## Команды сборки

``` bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Команды выполнения теста

```
ctest -C Debug
```

В случае запуска тестов, в появляется подкаталог Testing/Temporary/, а в нём файл "LastTest.log". Пример содержимого:

```
Start testing: Nov 15 10:42 RTZ 2 (ceia)
----------------------------------------------------------
1/1 Testing: google_test
1/1 Test: google_test
Command: "K:/Playground/test/build/Debug/cpp_test.exe"
Directory: K:/Playground/test/build
"google_test" start time: Nov 15 10:42 RTZ 2 (ceia)
Output:
----------------------------------------------------------
<end of output>
Test time =   0.02 sec
----------------------------------------------------------
Test Passed.
"google_test" end time: Nov 15 10:42 RTZ 2 (ceia)
"google_test" time elapsed: 00:00:00
----------------------------------------------------------

End testing: Nov 15 10:42 RTZ 2 (ceia)
```
