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
