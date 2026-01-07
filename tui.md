# Разработка TUI-приложений

Во времена кризиса производства оперативной памяти, для промышленных приложений может быть разумным использовать Terminal User Interface (TUI). Приложение открывается в окне терминала, обеспечивает визуализацию пользовательского интерфейса в текстовом режиме и, преимущественно, управление с клавиатуры.

Преимущества TUI-приложений:

- малый размер исполняемого файла: ~2МБ для несложного приложения, протип ~150МБ для Avalonia-приложения
- высокая скорость работы
- минимальные требования к объёму ОЗУ. Будет прекрасно работать на машинах с 8ГБ ОЗУ

## Библиотека FTXUI

Официальный репозитарий библиотеки [FTXUI](https://github.com/ArthurSonzogni/FTXUI).

Ключевые особенности библиотеки **ftxui**: декларативное описание пользовательского интерфейса, поддержка стандартов C++17/20, используются widget-ы, большое количество элементов пользовательского интерфейса (включая анимацию) и существенно меньшее количество низко-уровневого кода, по сравнению с другими библиотеками.

Сборка библиотеки под Linux:

```shell
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
sudo cmake --build build --target install
```

Последняя команда запускается с повышенными привелегиями для того, чтобы библиотеки были установлены в стандартные папки `/usr/local`

Пример приложения, которое корректно завершает свою работу, освобождая ресурсы, захватываемые приложением:

```cpp
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

int main() {
    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    // Get the exit closure once at the start
    auto exit = screen.ExitLoopClosure();

    auto quit_button = ftxui::Button("Quit", exit);

    auto component = ftxui::Container::Vertical({
        ftxui::Renderer([] { return ftxui::text("Press the button to exit."); }),
        quit_button,
    });

    // Add keyboard shortcut for quitting (e.g., 'q')
    component |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('q')) {
            exit();
            return true;
        }
        return false;
    });

    screen.Loop(component);

    // Any cleanup code can go here, but FTXUI handles terminal reset automatically.
    return 0;
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

## Более сложное приложение

```cpp
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/captured_mouse.hpp>

using namespace ftxui;

int main() {
    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    // Ключевой момент: при выходе объекта exit из scope, очередь сообщений будет
    // уничтожена, все ресурсы будут освобождены
    auto exit = screen.ExitLoopClosure();

    // Переменные, определяющие состояние приложения (State variables)
    int counter = 0;
    std::string input_text = "";
    int selected_tab = 0;
    bool checkbox_state = false;

    // Создаём объект для ввода текстовых данных
    auto input = Input(&input_text, "Type something...");

    // Создаём кнопки и добавляем для каждой из них обработчик нажатия
    auto button_increment = Button("Increment", [&] { counter++; });
    auto button_decrement = Button("Decrement", [&] { counter--; });
    auto button_reset = Button("Reset", [&] { counter = 0; });

    // Добавляем check-box и привязываем к нему state-переменную
    auto checkbox = Checkbox("Enable feature", &checkbox_state);

    // Основной контейнер пользовательских элементов - горизонтальный
    auto container = Container::Vertical({
        input,
        Container::Horizontal({
            button_increment,
            button_decrement,
            button_reset,
        }),
        checkbox,
    });

    // Определяем композицию пользовательских элементов (Renderer)
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

    // Конфигурируем обработчики очереди сообщений, в которых нас интересует кнопка 'q',
    // при нажатии на которую требуется завершить работу приложения
    renderer |= CatchEvent([&](Event event) {

        if (event == Event::Character('q')) {
            std::exit(0);
            return true;
        }

        return false;
    });

    // Создаём очередь сообщений и запускаем основной цикл для определённых нами элементов
    // пользовательского интерфейса
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

### Решение проблемы с загрузкой репозитария библиотеки с GitHub (костыль)

Обойти проблему с загрузкой репозитария с GitHub можно скачав zip-архив, собрав проект вручную и используя "CMakeLists.txt" с прямыми ссылками:

```cmake
cmake_minimum_required(VERSION 3.11)
project(ftxui_demo)

set(CMAKE_CXX_STANDARD 17)

# Add the FTXUI headers
include_directories(
    "d:/Sources/Playground/ftxui_demo/FTXUI/include"
)

# Locate the library files (adjust the path if they are in a different folder)
set(FTXUI_LIB_DIR "d:/Sources/Playground/ftxui_demo/FTXUI/build/Debug")

# Create imported targets for clarity (optional but recommended)
add_library(ftxui-component STATIC IMPORTED)
set_target_properties(ftxui-component PROPERTIES
    IMPORTED_LOCATION "${FTXUI_LIB_DIR}/ftxui-component.lib"
)

add_library(ftxui-dom STATIC IMPORTED)
set_target_properties(ftxui-dom PROPERTIES
    IMPORTED_LOCATION "${FTXUI_LIB_DIR}/ftxui-dom.lib"
)

add_library(ftxui-screen STATIC IMPORTED)
set_target_properties(ftxui-screen PROPERTIES
    IMPORTED_LOCATION "${FTXUI_LIB_DIR}/ftxui-screen.lib"
)

add_executable(ftxui_demo main.cpp)
target_link_libraries(ftxui_demo
  PRIVATE         
	ftxui-component
    ftxui-dom
    ftxui-screen
)
```

Приложение под Windows прекрасно запускается из графического пользовательского интерфейса.

## Добавление модальных диалогов

Пример реализации модального диалога приведен в примере "\ftxui-src\examples\component\modal_dialog.cpp".

Для добавления модального диалога необходимо добавить состояние (modal_shown), отвечающее за отображение/сокрытие модального диалога, а также необходимые для его использования лямбда-функции:

```cpp
auto screen = ftxui::ScreenInteractive::TerminalOutput();

// Состояние отображения модального диалога
bool modal_shown = false;

// Действия, которые выполняются при управлении состоянием модального диалога
auto show_modal = [&] { modal_shown = true; };
auto hide_modal = [&] { modal_shown = false; };

// Ключевой момент: при выходе объекта exit из scope, очередь сообщений будет
// уничтожена, все ресурсы будут освобождены
auto exit = screen.ExitLoopClosure();

// Загрушки
auto do_nothing = [&] {};
```

Далее, определяем объект-кнопку, при нажатии на которую будет открываться модальный диалог:

```cpp
auto button_modal = Button("Show modal", show_modal);
```

Включаем эту кнопку в один из контейнеров основной формы:

```cpp
    auto container = Container::Vertical({
        input,
        Container::Horizontal({
            button_increment,
            button_decrement,
            button_reset,
			button_modal,   // <--
        }),
        checkbox,
    });
```

Активируем рендеринг в вызове Renderer():

```cpp
hbox({
    button_increment->Render(),
    button_decrement->Render(),
    button_reset->Render(),
    button_modal->Render()  // <-->
}),
```

Добавляем реализацию модального диалога:

```cpp
// Определяем стиль отображения кнопок
auto button_style = ButtonOption::Animated();

Component ModalComponent(std::function<void()> do_nothing,
                         std::function<void()> hide_modal) 
{
    // В модальном диалоге будет две кнопки. Мы указываем стиль кнопки как
    // ButtonOption::Animated(), чтобы убрать рамки вокруг этих кнопок и сделать их
    // более похожим на контекстное меню
    auto component = Container::Vertical({
        Button("Do nothing", do_nothing, button_style),
        Button("Quit modal", hide_modal, button_style),
    });

    // Добавляем правила рендеринга этих кнопок
    component |= Renderer([&](Element inner) {
    return vbox({
                text("Modal component "),
                separator(),
                inner,
            })
            | size(WIDTH, GREATER_THAN, 30)
            | border;
    });
    return component;
}
```

Добавляем модальный диалог к рендеру основного диалога:

```cpp
auto modal_component = ModalComponent(do_nothing, hide_modal);
renderer |= Modal(modal_component, &modal_shown);

screen.Loop(renderer);
```

## Gauge/progress bar

В примерах пользовательского интерфейса есть gauge, также известный как "progress bar":

```cpp
vbox({
    hbox({
        text("one") | border,
        text("two") | border | flex,
        text("three") | border | flex,
    }),

    gauge(0.25) | color(Color::Red),
    gauge(0.50) | color(Color::White),
    gauge(0.75) | color(Color::Blue),
});
```

Его можно использовать для демонстрации прогресса, например, при обновлении прибора BVS.

В нашем примере добавляется Gauge с таймером. Для этого следует выполнить следующие задачи:

- Добавить переменную для отслеживания прогресса
- Использовать screen.Post() для периодического обновления
- Создать таймер в отдельном потоке
- Добавить элемент gauge в рендерер

Переменные для определения состояния можно определить следующим образом:

```cpp
// Переменная для gauge (используем atomic для потокобезопасности)
std::atomic<float> gauge_value = 0.0f;
std::atomic<bool> timer_running = true;
```

Внутри Renderer() следует добавить полосу прокрутки и поле, в котором будет отображаться прогресс операции:

```cpp
hbox({
    text("Progress: "),
    gauge(gauge_value.load()) | flex,
}),
text("  " + std::to_string(int(gauge_value.load() * 100)) + "%") | center,
separator(),
```

Затем нужно добавить поток, который будет каждые 50 миллисекунд изменять значаение прогресса на 1%, а при достижении 100% сбрасывать его. При выходе из основного цикла, необходимо дождаться завершения работы этого потока:

```cpp
std::thread timer_thread([&]() {
    while (timer_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Увеличиваем значение gauge на 1%
        gauge_value = gauge_value.load() + 0.01f;
        
        // Сбрасываем при достижении 100%
        if (gauge_value >= 1.0f) {
            gauge_value = 0.0f;
        }
        
        // Запрашиваем перерисовку экрана
        screen.Post(Event::Custom);
    }
});

screen.Loop(renderer);

// Ожидаем завершения потока таймера
timer_running = false;
if (timer_thread.joinable()) {
    timer_thread.join();
}
```

Побочный эффект - курсор текущей текстовой позиции прыгает по тексту, т.к. экранная форма у нас постоянно перерисовывается.

## Другие органы управления

Для отображения изменяемых данных, вероятно, можно использовать таблицу (Table).

## Другие TUI-библиотеки

Также распространёнными считаются следующие библиотеки:

- ncurses - Full screen handling, panels, forms, menus, color support. Classic terminal apps (editors, file managers)
- termbox‑go - C, C++ wrappers. Minimalist, event‑driven, works on Windows/Linux/macOS. Simple TUIs, games
- curses‑pp - Object‑oriented wrapper around ncurses. Cleaner C++ code, RAII
- libtcod - Tile‑based rendering, keyboard/mouse, used in roguelikes. Game‑oriented terminal graphics
