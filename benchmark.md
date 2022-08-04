# Google Benchmark

В современном программировании на C++ бывает достатчно сложно дать точную оценку того, как быстро будет исполняться некоторый код. Существует множество факторов, которые влияют на производительность и развитие языковых средств оптимизирующих шаблонный код ещё больше осложняет "ручную" оценку эффективности кода. Решением проблемы может быть [Google Benchmark library](https://github.com/google/benchmark), специализированная высокоточная библиотека для анализа времени выполнения кода.

## Настройка проекта

Пример измеряемого кода, файл "benchtest.cpp":

``` cpp
#include <benchmark/benchmark.h>

static void BM_StringCreation(benchmark::State& state) {
	for (auto _ : state)
		std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State& state) {
	std::string x = "hello";
	for (auto _ : state)
		std::string copy(x);
}
BENCHMARK(BM_StringCopy);

BENCHMARK_MAIN();
```

Собрать тест из командной строки можно так:

```
g++ ./benchtest.cpp -std=c++11 -isystem benchmark/include -L~/benchmark/build/src -lbenchmark -lpthread -o benchtest
```

Для использования совместно с CMake, должен быть установлен инструмент [Google Test](https://github.com/google/googletest). 

Google Test и Google Benchmark загружаются в исходных текстах и их необходимо собрать. Типовая строка сборки в Release-варианте выглядит следующим образом:

``` shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
cmake --install .
```

Типовой скрипт CMake для сборки проекта (под Linux) с использованием Google Benchmark, файл "CMakeLists.txt":

``` cmake
cmake_minimum_required(VERSION 3.10)

project(benchtest)

# Hints: для того, чтобы find_package смог найти Boost, который находится по не стандартному пути
set( BOOST_ROOT $ENV{HOME}/boost/current/ )			# Preferred installation prefix.
set( BOOST_INCLUDEDIR $ENV{HOME}/boost/current/include/ )	# Preferred include directory e.g. <prefix>/include.
set( BOOST_LIBRARYDIR $ENV{HOME}/boost/current/lib_x86_64/ )	# Preferred library directory e.g. <prefix>/lib.

# После того, как мы подсказали где искать Boost, модуль find_package должен
# выполнить необходимые настройки, в том числе, установить необходимые переменные окружения
find_package(Boost 1.69 REQUIRED COMPONENTS program_options thread)

# Ищем установленную копию Google Benchmark
find_package(benchmark)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lbenchmark -std=c++11")

add_executable(${PROJECT_NAME}
    "benchtest.cpp")

# Трассируем результаты поиска в консоль
message("Boost include directory: " ${Boost_INCLUDE_DIRS})
message("Boost library directory: " ${Boost_LIBRARY_DIRS})

# Указываем подкаталог с Boost-ом, в котором нужно искать его include-файлы
target_include_directories( ${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

# Указываем нужные нам библиотеки Boost, которые были перечислены в find_package()
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

# Подключаем к проекту найденную библиотеку Google Benchmark
target_link_libraries(${PROJECT_NAME} benchmark)
```

Сборка и запуск на исполнение из подкаталога с файлами "CMakeLists.txt" и "benchtest.cpp":

```
mkdir build && cd build
cmake ..
cmake --build .
./benchtest
```

## Объяснение метрик

По результатам выполнения тестов Google Benchmark выдает следующие данные:

```
Run on (4 X 3192.64 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 0.19, 0.10, 0.09
***WARNING*** Library was built as DEBUG. Timings may be affected.
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
BM_StringCreation       5.59 ns         5.59 ns     90980605
BM_StringCopy           23.3 ns         23.3 ns     30095881
```

Колонка **Benchmark** указывается имя теста. Часто один и тот же алгоритм кодируют разными способами, а Google Benchmart позволяет установить, какой из вариантов работает более эффективно. Под _эффективностью_ подразумевается две метрики: время фактической загрузки процессора выполняемой работой и общее время выполнения задачи, включая время, в котором алгоритм простаивал, например, ожидал результата выполнения синхронной функции операционной системы.

Колонка **Time** это т.н. _The wall clock time_, буквально, "_время по настенным часам_", т.е. время с момента начала выполнения теста и до его завершения. Если замеры выполняются несколькими процессорными ядрами, Time может быть пропорционально меньше, чем при проведении однопоточного тестирования.

Колонка **CPU** означает "_The user-cpu time and system-cpu time_", т.е. время непосредственно затраченное на выполнение кода теста основным потоком исполнения.

**Iterations** - число итераций, которые необходимо выполнить для того, чтобы получить статистически достоверные результаты.

## Дополнительные возможности

**Benchmark** позволяет выполнять функцию в нескольких потоках, например:

``` cpp
BENCHMARK(BM_Empty)->Threads(4);
...
BENCHMARK(BM_Empty)->Threads(1)->UseRealTime();
```

Подобные тесты позволяют оценить падение эффективности из-за межпоточной синхронизации.

```
Benchmark Time CPU Iterations
----------------------------------------------------------------------------
BM_Empty/threads:1 2 ns 2 ns 418132308
BM_Empty/threads:2 1 ns 2 ns 418901068
BM_Empty/threads:4 0 ns 2 ns 394432372
BM_Empty/real_time/threads:1 2 ns 2 ns 418057380
BM_Empty/real_time/threads:2 1 ns 2 ns 836633562
BM_Empty/real_time/threads:4 0 ns 2 ns 1621747004
```

Продолжительность выполнения кода является суммой по всем потоках.
