# set minimum cmake version
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# project name and language
project(BuildConvertUTF8Test_1.0 LANGUAGES CXX)

# require C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# универсальная библиотека, которая является объектом тестирования
add_library(ConvertUTF8 ConvertUTF8.cpp)

# Формируем промышленное приложение ... добавляя в него Unit-тесты
# add_executable(Convertation test.cpp)
# target_link_libraries(Convertation ConvertUTF8)

# we will use the network to fetch Google Test sources
# make it possible to disable unit tests when not on network
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if(ENABLE_UNIT_TESTS)
  # the following code to fetch googletest
  # is inspired by and adapted after:
  #   - https://cmake.org/cmake/help/v3.11/module/FetchContent.html
  include(FetchContent)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
  )

FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)# Prevent GoogleTest from using PThreads
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)

    # adds the targets: gtest, gtest_main, gmock, gmock_main
    add_subdirectory(
      ${googletest_SOURCE_DIR}
      ${googletest_BINARY_DIR}
      )

    # Silence std::tr1 warning on MSVC
    if(MSVC)
      foreach(_tgt gtest gtest_main gmock gmock_main)
        target_compile_definitions(${_tgt}
          PRIVATE
            "_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING"
          )
      endforeach()
    endif()
  endif()

  add_executable(cpp_test "")

# Тесты состоят из файла "test.cpp"
  target_sources(cpp_test
    PRIVATE
      test.cpp
	  test.h
    )

# Подключается внешняя библиотека gtest_main и библиотека uniqueRnd
  target_link_libraries(cpp_test
    PRIVATE
      ConvertUTF8
      gtest_main
    )

# Активируем функционал Unit-тестирования проекта
enable_testing()

# Добавляем тест - запуск собранного приложения с тестами
  add_test(
    NAME google_test
    COMMAND $<TARGET_FILE:cpp_test>
    )
endif()