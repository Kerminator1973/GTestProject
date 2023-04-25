# Сборка демонстрационного проекта

Рекомендуется создать в папке с проектом "CMakeExamples\BuildWithGoogleTest" подпапку с именем "build" и перейти в неё:

``` shell
mkdir -p build
cd build
```

Запустить процесс поиска компиляторов и генерацию скрипта сборки для конкретной машины можно выполнить командой:

``` shell
cmake ..
```

Поскольку мы находимся внутри папки **build** две точки указывают, что сборку нужно начать в папке на уровень выше. Смысл в создании папки build и генерации скриптов сборки в неё состоит в простой процедуре удаляения всех временных файлов и папок - вполне достаточно удалить папку **build** со всем, что в ней находится.

Сборка проекта найденными компиляторами осуществляется командой:

``` shell
cmake --build .
```

Запустить тесты на выполнение можно командой:

``` shell
ctest -C Debug
```

В данном случае, используется конфигурация **Debug**, но может быть использованы и другие конфигурации, например, **Release**.

## Как работает CMakeLists.txt (из папки BuildWithGoogleTest)

Файл "sum_integers.cpp" используется для сборки библиотеки sum_integers (sum_integers.dll):

``` cmake
add_library(sum_integers sum_integers.cpp)
```

Исполняемый файл sum_up (sum_up.exe) собирается из файла "main.cpp":

``` cmake
add_executable(sum_up main.cpp)
target_link_libraries(sum_up sum_integers)
```

Приложение sum_up, в нашем примере, как-будто передаётся клиентам. Оно умеет суммировать все числа, которые будут переданы через командную строку, например:

``` console
sum_up 3 4 5
```

Результат вызова утилиты будет: 12.

Если мы тестируем библиотеку и нам не нужно собирать приложение, то добавлять исполняемый файл вызывая add_executable() не нужно.

Далее в скрипте, мы определяем настройку, которая активирует, или отключает Unit-тестирование. По умолчанию, настройка включена:

``` cmake
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")
```

Если Unit-тестирование включено, то подключается дополнительная команда **FetchContent*, которая позволяет выкачать исходные тексты Google Test с указанного репозитария, с указанием конкретного Tag-а. 

``` cmake
if(ENABLE_UNIT_TESTS)
  include(FetchContent)
```

Рекомендуется использовать актуальные выпуски библиотеки, ознакомившись с содержимым [репозитария на GitHub](https://github.com/google/googletest). Настройка параметров репозитария осуществляется так:

``` cmake
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
  )
```

Получение дополнительных параметров из загруженной библиотеки выполняется командой:

``` cmake
  FetchContent_GetProperties(googletest)
```

Непосредственная загрузка данных из репозитария осуществляется командой:

``` cmake
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
```

В скрипте есть специализированные команды, устраняющие проблемы совместимости с определёнными компиляторами - эту часть скрипта можно пока опустить.

Важным является включение в проект подкаталогов из загруженного репозитария:

``` cmake
    add_subdirectory(
      ${googletest_SOURCE_DIR}
      ${googletest_BINARY_DIR}
      )
```

Ещё один исполняемый файл будет называться **cpp_test** и именно он будет содержать Unit-тесты для библиотеки **sum_integers**:

``` cmake
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

``` cmake
  enable_testing()

  add_test(
    NAME google_test
    COMMAND $<TARGET_FILE:cpp_test>
    )
endif()
```

Тесты считаются выполненными, если "cpp_test" завершается с кодом 0. Любой другой код сигнализирует об ошибке выполнения Unit-тестов.

Дополнительная статья [Building GoogleTest and GoogleMock directly in a CMake project](https://crascit.com/2015/07/25/cmake-gtest/).

[Пример](.\Contributors\baston\CMake_GoogleTest\README.md) выполнения задачи разработчиком baston доступен в данном репозитарии.

## Автономный вызов приложения с тестами

Когда мы используем команду выполнения тестов CTest (`ctest -C Debug`) диагностический вывод будет ограниченным. По сути, эта команда позволяет проверить, являются ли тесты успешно пройденными. Вместе с тем, мы можем немного доработать приложение, сделав вывод гораздо более информативным. Предположим, что у нас есть тестируемый класс TestClass, с определением в файле "test.h":

``` cpp
class TestClass {
public:
  TestClass();
  int ReturnZero();
  int ReturnOne();
};
```

Для простоты, поместим реализацию класса и тесты в один файл - "test.cpp":

``` cpp
#include "gtest/gtest.h"
#include "test.h"

TestClass::TestClass() {
}

int TestClass::ReturnZero()
{
  return 1;	// Имитация ошибки
}

int TestClass::ReturnOne()
{
  return 1;	// Имитация успешного выполнения
}

namespace {
	TEST(TestClass, ReturnZero) {
		TestClass tc;
		EXPECT_EQ(0, tc.ReturnZero());
	}

	TEST(TestClass, ReturnOne) {
		TestClass tc;
		EXPECT_EQ(1, tc.ReturnOne());
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
```

Метод ReturnZero() будет возвращать ошибочное значение - это нужно, чтобы тест провалился и мы это увидели.

В приведённом выше примере новым являются код функции main() и применение макросов Google Test. В функции main() добавлен вызов InitGoogleTest(), который позволяет обрабатывать дополнительные параметры, определяющие стратегию проведения тестов. А макрос RUN_ALL_TESTS() позволяет выполнить все тесты определённые в приложении:

``` cpp
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```

Встраивание обработки параметров вызова приложения для тестирования добавляет функции управления набором и порядком выполнения тестов. Наиболее интресными флагами являются:

`--gtest_list_tests`: выводит полный список всех доступных тестов

`--gtest_filter=pattern`: запускает только тесты, имя которых соответствует шаблону. Например: `--gtest_filter="TibcoLayerTest.*"`

`--gtest_shuffle`: запускает тесты в произвольном порядке

Запуск тестов в произвольном порядке позволяет обеспечить предотвратить ошибки в тестах (или в тестируемых функциях), связанные с фиксированной последовательностью вызова.

Для описания теста используется макроc TEST() и макрос условия выполнения - EXPECT_EQ():

``` cpp
TEST(TestClass, ReturnOne) {
	TestClass tc;
	EXPECT_EQ(1, tc.ReturnOne());
}
```

[Параметрами вызова](https://google.github.io/googletest/reference/testing.html) TEST() являются имя тестируемого класса (TestClass) и тестируемый метод (ReturnOne).

Условие EXPECT_EQ() указывает на ожидаемый результат - если фактический и ожидаемый результаты совпадут, тест будет выполнен успешно, а если не совпадут - будет зафиксирована ошибка выполнения теста.

Если мы запустим исполняемый файл выполняющий тесты, то получим подробный лог выполнения тестов, с подробным логом выполнения тестов:

``` output
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from TestClass
[ RUN      ] TestClass.ReturnZero
K:\Sources\Playground\uniqueRnd\test.cpp(22): error: Expected equality of these values:
  0
  tc.ReturnZero()
    Which is: 1
[  FAILED  ] TestClass.ReturnZero (2 ms)
[ RUN      ] TestClass.ReturnOne
[       OK ] TestClass.ReturnOne (0 ms)
[----------] 2 tests from TestClass (3 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (5 ms total)
[  PASSED  ] 1 test.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] TestClass.ReturnZero

 1 FAILED TEST
 ```

## Sanitizers

В современных компиляторах (clang и gcc) существует прекрасная функциональная особенность, позволяющая подключать дополнительные инструментальные средства, в частности – санитайзеры, инструменты автоматического поиска ошибок в коде в процессе исполнения. Ссылка на библиотеку: https://github.com/google/sanitizers

Примеры CMake для активации Sanitizers: https://github.com/dev-cafe/cmake-cookbook/tree/master/chapter-05/recipe-07

Достаточно просто перекомпилировать исходники с определённым набором флагов (включая параметры linker-а) и вы можете исследовать и отлаживать проблемы с кодом. Основные типы санитайзеров:

* Memory errors (address sanitizer)
* Uninitialized reads (memory sanitizer)
* Thread safety (thread sanitizer)
* Undefined behavior (undefined behavior sanitizer)

По сравнению с другими подходами (внешние инструменты, такие как [Valgrid](https://valgrind.org/)), санитайзеры обычно гораздо экономнее используют вычислительные ресурсы и дают больше полезной информации о найденных проблемах. Единственный _drawback_ – ваши исходные тексты и библиотеки должны быть пересобраны с дополнительными флагами.

Sanitizers уже давно доступны для Clang и GCC.

Модуль, который поддерживает работу sanitizer-ов с CMake доступен по ссылке: https://github.com/arsenm/sanitizers-cmake

Напоминание: в CMake элементы списка разделяются символом semicolon (точка с запятой).

Выполнил сборку проекта командами:

``` shell
cmake .
cmake --build .
```

Типовая условная сборка отдельного санитайзера:

``` cmake
set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_REQUIRED_FLAGS ${ASAN_FLAGS})
check_cxx_compiler_flag(${ASAN_FLAGS} asan_works)
unset(CMAKE_REQUIRED_FLAGS)
if(asan_works)
	string(REPLACE " " ";" _asan_flags ${ASAN_FLAGS})
	add_executable(asan-example asan-example.cpp)
	target_compile_options(asan-example
		PUBLIC
			${CXX_BASIC_FLAGS}
			${_asan_flags}
		)
	target_link_libraries(asan-example
		PUBLIC
			${_asan_flags}
		)
endif()
```

Скрипт сборки создал makefiles только для трёх санитайзеров: asan-example, tsan-example и ubsan-example. Приложения запускаются и действительно выдают полезную информацию.

Пример приложения:

``` cpp
int main(int argc, char **argv) {
	int stack_array[100];
	stack_array[1] = 0;
	return stack_array[argc + 100]; // BOOM
}
```

Ошибка состоит в том, что argc – количество параметров больше нуля и индекс возвращаемого значения (stack_array) выходит за пределы буфера. Запускаем приложение:
`./asan-example`

Получаем:

``` output
==3888==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7fffd9423594 at pc 0x555c5612eaff bp 0x7fffd94233d0 sp 0x7fffd94233c0
…
    #0 0x555c5612e989 in main /home/developer/cmake-cookbook/chapter-05/recipe-07/cxx-example/asan-example.cpp:1
This frame has 1 object(s):
    [32, 432) 'stack_array' <== Memory access at offset 436 overflows this variable
…
SUMMARY: AddressSanitizer: stack-buffer-overflow /home/developer/cmake-cookbook/chapter-05/recipe-07/cxx-example/asan-example.cpp:4 in main
Shadow bytes around the buggy address: …
```

Санитайзер выводит тип ошибки, имя буфера, который не корректно используется, место, где это буфер был создан и shadow bytes – дамп окружающих байт, которые используются для проверки переполнения. Инструмент кажется очень полезным.

Причина, по которой не был собран msan-example – у компиляторов GNU 7.5 немного другой набор имен поддерживаемых санитайзеров: https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html

На RPi 3 установлен gcc 8.3, но проекты tsan и msan не собрались. Исполняемый модуль asan-example собрался, но не заработал из-за отсутствия необходимой библиотеки. После того, как был добавлен ключ сборки -static-libasan приложение прекрасно отработало:

``` cmake
set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer -static-libasan")
```

Ошибка в коде была успешно обнаружена.
