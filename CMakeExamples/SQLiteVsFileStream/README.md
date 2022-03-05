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

## Сборка проекта

После выполнения команды `cmake --install .` в скрипте "CMakeLists.txt" можно использовать команду `find_package(benchmark REQUIRED)`, которая устанавливает путь к lib-файлам. Однако, путь к include-файлу следует включить в проект следующей командой:

``` cmake
# Указываем подкаталог Google Benchmark, в котором нужно искать его include-файлы
target_include_directories( ${PROJECT_NAME} PRIVATE ${benchmark_INCLUDE_DIRS})
```

При сборке проекта нужно также указывать, что должна быть указана конфигурация Release:

``` shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Для сборки benchmark достаточно просто запустить exe-файл.
