# Использовать заголовочные файлы из других проектов

Предположим, что в наш проект состоит из множества подпроектов, в частности: библиотеки "somelib" и утилиты для проведения Unit-тестирования "unittest". Пример такого проекта находится в папке "\CMakeExamples\target_include". Предположим, что проект "unittest" зависит от библиотеки "somelib", поскольку он осуществляет тестирование функционала этой библиотеки. В CMakeLists.txt, находящегося в папке "unittest", мы можем добавить зависимость от библиотеки:

``` cmake
add_executable(cpp_test "")
target_link_libraries(cpp_test
    PRIVATE
        somelib
)
```

Для того, чтобы также добавить путь к заголовочным файлам, достаточно добавить следующие директивы:

``` cmake
set(SOMELIB_DIRECTORY "./../somelib")
message(status "Set path to library directory ${SOMELIB_DIRECTORY}")

add_executable(cpp_test "")

target_include_directories(cpp_test PRIVATE ${SOMELIB_DIRECTORY})
```

После этого мы можем использовать заголовочные файлы из папки "somelib" в исходных текстах проекта "unittest":

``` cpp
#include "somelib.h"

int main() {
    SomeLib obj;
    obj.hello();
    return 0;
}
```
