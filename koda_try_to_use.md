# Опыт использования Koda

[Koda](https://kodacode.ru/) - это внутренний AI-помощник для разработчика,с поддержкой русского языка, позволяющий пользоваться западными ИИ без использования прокси. Базируется на проекте [Continue](https://github.com/continuedev/continue).

Установить Koda можно как Plug-in для Visual Studio Code.

Чтобы активировать Koda, необходимо пройти аутентификацию на GitHub и предоставить результат аутентификации plug-in-у.

## Попытка генерации TUI-приложения

Далее описана вторая попытка вайб-кодинга. Стоит заметить, что я уже решил данную задачу; решение описано описано в [tui](./tui.md)

>Я хочу создать проект на C++ для операционной системы Windows в котором описание системы сборки будет сделано на CMake. Компилятор и CMake уже установлены на моей системе. Это приложение должно реализовывать TUI-интерфейс с использованием библиотеки ftxui. Сгенерируй мне файл CMakeLists.txt для загрузки ftxui из репозитария на github и простое приложение в котором будет добавлена одна экранная кнопка, а также будет осуществляться выход из приложения при нажатии на кнопку 'q' на клавиатуре

Koda предложил следующий вариант файла "CMakeLists.txt":

```
cmake_minimum_required(VERSION 3.16)
project(ftxui_button_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение FetchContent для загрузки ftxui из GitHub
include(FetchContent)

# Загрузка ftxui из репозитория GitHub
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui.git
    GIT_TAG v5.0.0  # Можно указать конкретную версию или ветку
)

# Сборка и подключение ftxui
FetchContent_MakeAvailable(ftxui)

# Создание исполняемого файла
add_executable(ftxui_button_app src/main.cpp)

# Подключение ftxui как библиотеки
target_link_libraries(ftxui_button_app PRIVATE ftxui::screen ftxui::dom ftxui::component)
```

В первой попытке Koda предложил следующую реализацию "main.cpp":

```cpp
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>

int main() {
    using namespace ftxui;

    // Создание кнопки с обработчиком выхода
    auto button_exit = Button("Выход", []() {
        exit(0);
    });

    // Создание контейнера с кнопкой
    auto component = Container::Vertical({button_exit});

    // Создание экрана
    auto screen = ScreenInteractive::TerminalOutput();

    // Основной рендер с обработкой клавиши 'q' для выхода
    screen.Loop(Container::Vertical({
        component
    }) | border | center);

    return 0;
}
```

Вторую попытку не привожу - она получилось значительно более глючной и неработоспособной. Однако во второй попытке Koda догадался, что нужно обрабатывать кнопку 'q':

```cpp
// Обработка ввода с клавиатуры
auto full_renderer = CatchEvent(renderer, [&](Event event) {
    if (event == Event::Character('q') || event == Event::Escape) {
        exit = true;
        return true;
    }
    return false;
});
```

К сожалению, завершать приложение второй пример разучился.

Что сразу было сделано неправильно:

- Koda создала некорректную структуру проекта, добавив лишную промежуточную папку (первая попытка генерации)
- Koda предложила выполнить в терминале неполную команду `cd путь/к/project` (вторая попытка генерации)
- Сгенерированный код не соответствует требованиям и содержит ошибку - не добавлен код, который освобождает ресурсы при выходе из приложения, что приводит к тому, что работоспособность терминала нарушается и его приходится закрывать вручную. Визуально это выглядит как появление координат курсора мыши в человеко-читаемой формате с блокированием возможности реальной работы (первая попытка генерации)
- собранный файл оказался не там, где предполагался. Инструкция по запуску была некорректной
- во второй попытке, сборка оказалась неуспешной:

```
D:\Sources\Playground\kodaTUI\src\main.cpp(4,10): error C1083: Cannot open include file: 'ftxui/component/input.hpp': No such file or directory [D:\Sour
ces\Playground\kodaTUI\build\ftxui_app.vcxproj]
```

Ошибка была связана с тем, что Koda добавил не существующие заголовочные файлы:

```cpp
#include <ftxui/component/input.hpp>
#include <ftxui/component/button.hpp>
```
В действительности, описание Button находится в другом файле - "ftxui/component/component.hpp", а Input был вообще не нужен в данной реализации.

После исправления ошибок, связанных с структурой проекта и shell-командами, приложение успешно собралось и заработало, но с указанными ранее ошибками.

## Vibe-coding

Уточнение: "Код приложения содержит ошибку - он не освобождает ресурсы при завершении работы".

Предложенные ИИ правки некорректны и приводят к ошибкам сборки:

```
D:\Sources\Playground\kodaTUI\src\main.cpp(15,5): error C2280: 'ftxui::ScreenInteractive::ScreenInteractive(const ftxui::ScreenInteractive &)': attempti
ng to reference a deleted function [D:\Sources\Playground\kodaTUI\build\ftxui_button_app.vcxproj]
      D:\Sources\Playground\kodaTUI\build\_deps\ftxui-src\include\ftxui\component\screen_interactive.hpp(123,1):
      compiler has generated 'ftxui::ScreenInteractive::ScreenInteractive' here
      D:\Sources\Playground\kodaTUI\build\_deps\ftxui-src\include\ftxui\component\screen_interactive.hpp(123,1):
      'ftxui::ScreenInteractive::ScreenInteractive(const ftxui::ScreenInteractive &)': function was implicitly deleted because a data member invokes a d
  eleted or inaccessible function 'std::unique_ptr<ftxui::SenderImpl<ftxui::Task>,std::default_delete<ftxui::SenderImpl<ftxui::Task>>>::unique_ptr(const
   std::unique_ptr<ftxui::SenderImpl<ftxui::Task>,std::default_delete<ftxui::SenderImpl<ftxui::Task>>> &)'
      C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include\memory(3475,5):
      'std::unique_ptr<ftxui::SenderImpl<ftxui::Task>,std::default_delete<ftxui::SenderImpl<ftxui::Task>>>::unique_ptr(const std::unique_ptr<ftxui::Send
  erImpl<ftxui::Task>,std::default_delete<ftxui::SenderImpl<ftxui::Task>>> &)': function was explicitly deleted
```

Если скормить текст ошибки ИИ, то он начинает галюцинировать:

```cpp
// Структура-обёртка для безопасного выхода из экрана
struct ScreenExit {
    std::function<void()> exit_fn;
    void request_exit() { 
        if (exit_fn) exit_fn(); 
    }
};
```

Глюки не решают, а только усугубляют проблемы.

ИИ не справился с решением задачи.

## Что мешает выполнению работы

- Code Complete навязчивый в режиме работы с Markdown и почти никогда не даёт разумных предложений
- Генерация занимает значительное количество времени. Вместо работы программист сидит и ждёт пока закончится генерация кода. Генерация может продолжаться несколько минут!
- Компьютер постоянно находится в состоянии нагрузки, т.е. он почти всегда что-то делает
- В некоторых случаях ИИ успешно исправляет свои же ошибки, но это происходит не регулярно и требует значительных усилий и времени

## Что понравилось

- Агент предлагает самостоятельно создать файл на диске, без необходимости выполнения операций copy&paste вручную
- Агент может копировать команды в Terminal (Shell)
- Koda предлагает множество ИИ, включая: Minimax-m2.1, Gemini 2.5 Flash, DeepSeek V3.2, GIM 4.7, QWEN3, QWEN Coder, Kimi K2 Thinking. По большей части, модели устаревшие
- Есть "Koda Agent", который умеет работать по протоколу MCP

## Третья попытка создания приложения с использоанием Kimi K2

При использовании Kimi K2 расходуются токены. Тестовое количество токенов - 1000.

>Изучи документ по ссылке: https://github.com/Kerminator1973/GTestProject/blob/main/tui.md
Создай проект на C++ для операционной системы Windows. В проекте должна использоваться система сборки CMake. Компилятор и CMake уже установлены в системе. Приложение должно реализовывать TUI-интерфейс с использованием библиотеки ftxui. Сгенерируй мне файл CMakeLists.txt для загрузки ftxui из репозитария на github и простое приложение в котором будет добавлена одна экранная кнопка, а также будет осуществляться выход из приложения при нажатии на кнопку 'q' на клавиатуре

Kimi K2 создал полноценно работающее приложение.

Содержимое файла "CMakeLists.txt":

```
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
```

Скрипт CMake типовой, версия CMake, кажется, выбрана случайно - 3.14.

Программный код:

```cpp
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::TerminalOutput();

    // Создание кнопки
    auto button = Button("Нажми меня!", []{});

    // Основной контейнер с кнопкой и текстом
    auto component = Renderer(button, [&] {
        return vbox({
            text("Приложение с кнопкой"),
            separator(),
            button->Render(),
            separator(),
            text("Нажмите 'q' для выхода")
        }) | border;
    });

    // Обработчик событий клавиатуры
    auto event_handler = CatchEvent(component, [&](Event event) {
        if (event.character() == "q" || event.character() == "Q") {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    // Запуск главного цикла
    screen.Loop(event_handler);

    return 0;
}
```

Код очень эффектный. Генератор учёл необходимость вызова ExitLoopClosure() при выходе из приложения - именно это было основной ошибкой двух предыдущих экспериментов.

Объяснение про `screen.ExitLoopClosure()();` - Это не опечатка, а специальный паттерн в FTXUI:

- screen.ExitLoopClosure() возвращает функцию (замыкание типа `std::function<void()>`)
- Вторая пара скобок () вызывает эту возвращённую функцию

Такой подход позволяет передавать "инструкцию выхода" в другие части кода, не предоставляя им прямой доступ к объекту screen. Это безопасный механизм обратного вызова.

>Замечу! Kimi K2 здесь меня обыграла, я этого паттерна не знал. Kimi K2 сделала всё абсолютно корректно и красиво.

Инструкции по сборке простая, рациональная и правильная:

```shell
mkdir build
cd build
cmake ..
cmake --build .
```

### Добавление динамического поведения

>Добавь в приложение ещё одну кнопку, при нажатии на которую будет динамически добавляться gauge, отображающий прогресс некоторой задачи, исполняемой в течение 20 секунд. Как только gauge дойдёт до 100% он должен быть удалён из пользовательского интерфейса приложения.

К сожалению, на этом запросе Kimi 2 сгенерировал две ошибки. Первая ошибка - некорректный захват переменной screen в лямбде:

```cpp
std::thread([&, screen]{  // ❌ ОШИБКА: screen копируется
```

Простое правильное решение:

```cpp
std::thread([&]{  // ✅ ПРАВИЛЬНО: все переменные по ссылке
```

Kimi K2 сам исправил эту ошибку.

Вторая ошибка - ошибка компиляции:

```output
D:\Sources\KodaTUI\src\main.cpp(83,17): error C2039: 'Add': is not a member of 'ftxui::Node' [D:\Sources\KodaTUI\build\tui_app.vcxproj]   
      D:\Sources\KodaTUI\build\_deps\ftxui-src\include\ftxui\dom\node.hpp(22,7):
      see declaration of 'ftxui::Node'
```

Её Kimi K2 так же успешно исправил.

После небольших правок код, получившийся результат выглядит следующим образом:

```cpp
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <chrono>

using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::TerminalOutput();

    // Состояние прогресса задачи
    struct {
        int percentage = 0;
        bool active = false;
    } progress;

    // Вторая кнопка - запуск задачи
    auto buttonRunGauge = Button("Запустить задачу", [&]{
        if (!progress.active) {
            progress.active = true;
            progress.percentage = 0;

            // Запускаем задачу в отдельном потоке
            std::thread([&]{
                for (int i = 0; i <= 100; i++) {
                    // Обновляем прогресс в UI-потоке (i по значению!)
                    screen.Post([&, i]{
                        progress.percentage = i;
                        if (i == 100) {
                            progress.active = false;
                        }
                    });
                    
                    // Триггерим перерисовку
                    screen.Post(Event::Custom);
                    
                    // 20 секунд / 100 шагов = 200 мс на шаг
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }).detach();
        }
    });

    // Компонент gauge (рендерится только при активной задаче)
    auto gauge_component = Renderer([&]{
        if (!progress.active) {
            return text(""); // Не рендерим ничего
        }
        return vbox({
            text("Прогресс задачи:"),
            gauge(progress.percentage),
            text(std::to_string(progress.percentage) + "%")
        });
    });

    // Основной контейнер с кнопкой запуска приложения
    auto container = Container::Vertical({
        buttonRunGauge
    });

    // Рендерер всего UI
    auto component = Renderer(container, [&]{
        std::vector<Element> elements = {
            text("Приложение с индикатором прогресса"),
            separator(),
            buttonRunGauge->Render(),
            separator(),
        };
        
        // Динамически добавляем gauge, если задача активна
        if (progress.active) {
            elements.push_back(gauge_component->Render());
            elements.push_back(separator());
        }

        elements.push_back(text("Нажмите 'q' для выхода"));
        
        return vbox(elements) | border;
    });

    // Обработчик выхода по клавише 'q'
    auto event_handler = CatchEvent(component, [&](Event event) {
        if (event.character() == "q" || event.character() == "Q") {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(event_handler);

    return 0;
}
```

Приложение работает корректно. Качество исполнения вызывает восхищение - код простой и очень эффективный.

## Результат

Plug-in Koda отключен, но я буду продолжать эксперименты с Koda используя мощные модели, в частности - Kimi K2.
