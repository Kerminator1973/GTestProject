# Автоматическая генерация документации по тексту

Стандартом де-факто для генерации документации по C++ коду является [Doxygen](https://www.doxygen.org/).

К недостаткам Doxygen можно отнести отсутствие полнотекстового поиска, однако добавить эту функцию достаточно легко, т.е. существует интеграция Doxygen с системой управления документированием [Sphinx](https://www.sphinx-doc.org/en/master/). Система интеграции называется [Breathe](https://github.com/breathe-doc/breathe).

## В ПРОЦЕССЕ РАЗРАБОТКИ

При использовании CMake, обработку исходных текстов и генерацию документации можно осуществлять автоматически. Для этого необходимо включить в проект папки с исходными текстами и документацией:

```cmake
cmake_minimum_required(VESRION 3.10)

project("Breathe Demo" VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
add_subdirectory(src)
add_subdirectory(doc)
```

Интеграцию Sphinx рекоменжуется делать через вспомогательный скриптовый файл "FindSphinx.cmake", который может выглядеть так:

```cmake
find_program(
    SPHINX_EXECUTABLE
    NAMES sphinx-build
    DOC "Path to sphinx-build executable"
)

# Необходимо обработать аргументы REQUIRED и QUIET, а также установить переменную SPHINX_FOUND
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Sphinx "Unable to locate sphinx-build executable" SPHINX_EXECUTABLE
)
```

Обычно основная документация находится в header-файлах, в которых используются специальные тэги внутри комментариев: `@file`, `@brief`, `@param`.

В папке "doc" предлагается добавить свой скрипт сборки "CMakeLists.txt", который настраивает Doxygen:

```cmake
find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
    return()
endif()
```

Если Doxygen найден, то настраиваем специализированные переменные:

```cmake
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)
```

Далее может потребоваться добавить относительные пути:

```cmake
set(DOXIGEN_STRIP_FROM_PATH ${PROJECTS_SOURCE_DIR}/include)
```

Генерация документации выполняется следующей строкой:

```cmake
doxygen_add_docs(doxygen-doc ${PROJECT_SOURCE_DIR}/include COMMENT "Generating API documentation with Doxygen")
```

После того, как документация средствами Doxygen реализована, то нам следует использовать Breate, для интеграции с Sphinx:

```cmake
find_package(Sphinx REQUIRED)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)
add_custom_target(
    sphinx-doc ALL
    COMMAND ${SPHINX_EXECUTABLE} -b html -c ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generation API documentaion with Sphinx"
        VERBATIM
)
```

Чтобы каждый раз генерировать документацию средствами Doxygen, необходимо добавить вызов:

```cmake
add_dependencies(sphinx-doc doxygen-doc)
```

Внутри файла "conf.py.in" находятся настройки, связанные с формируемым документом.
