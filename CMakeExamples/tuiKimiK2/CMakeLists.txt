cmake_minimum_required(VERSION 3.14)
project(tui_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Загрузка ftxui из GitHub
include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v5.0.0
)

FetchContent_MakeAvailable(ftxui)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} src/main.cpp)

# Линковка с библиотеками ftxui
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    ftxui::dom
    ftxui::component
    ftxui::screen
)