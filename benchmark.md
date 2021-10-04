# Google Benchmark

В современном программировании на C++ бывает достатчно сложно дать точную оценку того, как быстро будет исполняться некоторый код. Существует множество факторов, которые влияют на производительность и развитие языковых средств оптимизирующих шаблонный код ещё больше осложняет "ручную" оценку эффективности кода. Решением проблемы может быть [Google Benchmark library](https://github.com/google/benchmark), специализированная высокоточная библиотека для анализа времени выполнения кода.

## Настройка проекта

Пример измеряемого кода:

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

Для использования совместно с CMake, должен быть установлен инструмент [Google Test](https://github.com/google/googletest). Типовой скрипт CMake для сборки проекта (под Linux) с использование Google Benchmark:

``` cmake
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

# Ищем установленную копию Google Benchmark
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

# Подключаем к проекту найденную библиотеку Google Benchmark
target_link_libraries(${PROJECT_NAME} CommPort)
target_link_libraries(${PROJECT_NAME} benchmark)
```

Сборка и запуск на исполнение:

```
mkdir build && cd build
cmake ../
make
make test
```

## Объяснение метрик

По результатам выполнения тестов Google Benchmark выдает следующие данные:

```
---------------------------------------------------------
Benchmark                  Time           CPU Iterations
---------------------------------------------------------
BM_fastFuncReturn      16327 ns      16392 ns      44800
BM_slowFuncReturn      17499 ns      16881 ns      40727
```

Колонка **Benchmark** указывается имя теста. Часто один и тот же алгоритм кодируют разными способами, а Google Benchmart позволяет установить, какой из вариантов работает более эффективно.

Колонка **Time** это т.н. _The wall clock time_, буквально, "_время по настенным часам_", т.е. время с момента начала выполнения тестов и до его завершения. Если замеры выполняются несколькими процессорными ядрами, Time может быть пропорционально меньше, чем при проведении однопоточного тестирования.

Колонка **CPU** означает "_The user-cpu time and system-cpu time_", т.е. время непосредственно затраченное на выполнение кода тестов.

**Iterations** - общее количество выполненных итераций.
