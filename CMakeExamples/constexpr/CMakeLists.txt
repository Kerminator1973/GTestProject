cmake_minimum_required(VERSION 3.20)
project(CRC8Example LANGUAGES CXX)

# Требуем минимум C++20 для constexpr‑циклов
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(crc8_example crc8_table.cpp)

# При желании можно включить предупрежения
if (MSVC)
    target_compile_options(crc8_example PRIVATE /W4)
else()
    target_compile_options(crc8_example PRIVATE -Wall -Wextra -Wpedantic)
endif()
