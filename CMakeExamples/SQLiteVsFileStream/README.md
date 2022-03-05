# Выполнение тестов производительности SQLite в разных режимах

Перед началом работы необходимо установить на машине [CMake](https://cmake.org/).

После того, как Google Benchmark будет загружен, собран (`cmake --build . --config Release`) и проинсталлирован (команда `cmake --install .`)

На машине с Windows следует выполнить сборку проекта, так же указав режим сборки - Release:

``` shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
cmake --install .
```
