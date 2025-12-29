# Разработка TUI-приложений

Во времена кризиса производства оперативной памяти, для промышленных приложений может быть разумным использовать Terminal User Interface (TUI). Приложение открывается в окне терминала, обеспечивает визуализацию пользовательского интерфейса в текстовом режиме и, преимущественно, управление с клавиатуры.

Преимущества TUI-приложений:

- малый размер исполняемого файла
- высокая скорость работы
- минимальные требования к объёму ОЗУ

## Библиотека FTXUI

Официальный репозитарий библиотеки [FTXUI](https://github.com/ArthurSonzogni/FTXUI).

Сборка библиотеки под Linux:

```shell
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
sudo cmake --build build --target install
```

Последняя команда запускается с повышенными привелегиями для того, чтобы библиоеки были установлены в стандартные папки `/usr/local`

Пример приложения:

```cpp
// hello_ftxui.cpp
#include <ftxui/component/component.hpp>   // Component utilities
#include <ftxui/component/screen_interactive.hpp> // Main loop
#include <ftxui/dom/elements.hpp>          // UI elements

using namespace ftxui;

int main() {
    // --- State ---------------------------------------------------------
    int counter = 0;

    // --- UI elements ----------------------------------------------------
    auto counter_text = Renderer([&] {
        return text("Counter: " + std::to_string(counter)) | bold;
    });

    auto inc_button = Button("Increase", [&] { ++counter; });
    auto dec_button = Button("Decrease", [&] { --counter; });
    auto quit_button = Button("Quit", [] { std::exit(0); });

    // Layout: vertical box with spacing
    auto layout = Container::Vertical({
        counter_text,
        inc_button,
        dec_button,
        quit_button,
    });

    // --- Main loop ------------------------------------------------------
    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(layout);
}
```

Команда сборки приложения:

```shell
g++ hello_ftxui.cpp -std=c++20 -lftxui-component -lftxui-dom -lftxui-screen -pthread -o hello_ftxui
```

Запуск приложения в консоли:

```shell
./hello_ftxui
```

Приложение работает, причём даже с мышкой, но при выходе ломает пользовательскую консоль. Также у приложения есть сложности с запуском из графической среды.

Вернуть курсор можно активировав его вручную:

```cpp
auto quit_button = Button("Quit", [] { 
    std::printf("\033[?25h");   // ANSI escape to show cursor
    std::fflush(stdout);
    std::exit(0); 
});
```

Также решается shell-командой `reset`:

```shell
./ftxui_demo
reset
```

### Чуть более сложное приложение

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;

int main() {
    // State variables
    int counter = 0;
    std::string input_text = "";
    int selected_tab = 0;
    bool checkbox_state = false;

    // Create input component
    auto input = Input(&input_text, "Type something...");

    // Create buttons
    auto button_increment = Button("Increment", [&] { counter++; });
    auto button_decrement = Button("Decrement", [&] { counter--; });
    auto button_reset = Button("Reset", [&] { counter = 0; });

    // Create checkbox
    auto checkbox = Checkbox("Enable feature", &checkbox_state);

    // Create main container
    auto container = Container::Vertical({
        input,
        Container::Horizontal({
        button_increment,
        button_decrement,
        button_reset,
        }),
        checkbox,
    });

    // Create renderer
    auto renderer = Renderer(container, [&] {
        return vbox({
        text("FTXUI Demo Application") | bold | center,
        separator(),
        hbox({
            text("Counter: "),
            text(std::to_string(counter)) | color(Color::Green) | bold,
        }),
        separator(),
        hbox({
            text("Input: "),
            input->Render(),
        }),
        separator(),
        hbox({
            button_increment->Render(),
            button_decrement->Render(),
            button_reset->Render(),
        }),
        separator(),
        checkbox->Render(),
        separator(),
        text("Status: " + std::string(checkbox_state ? "Enabled" : "Disabled")),
        separator(),
        text("Press 'q' to quit") | dim,
        }) | border;
    });

    // Handle quit key
    renderer |= CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
        exit(0);
        return true;
        }
        return false;
    });

    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(renderer);

    return 0;
}
```

Скрипт сборки приложения:

```cmake
cmake_minimum_required(VERSION 3.11)
project(ftxui_demo)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(ftxui_demo main.cpp)
target_link_libraries(ftxui_demo
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
```

Команды сборки приложения:

```shell
mkdir build && cd build
cmake ..
cmake --build .
```