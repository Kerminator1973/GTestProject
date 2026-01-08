# Опыт использования Koda

[Koda](https://kodacode.ru/) - это внутренний AI-помощник для разработчика,с поддержкой русского языка, позволяющий пользоваться западными ИИ без использования прокси.

Установить Koda можно как Plug-in для Visual Studio Code.

Чтобы активировать Koda, необходимо пройти аутентификацию на GitHub и предоставить результат аутентификации plug-in-у.

## Попытка генерации TUI-приложения

В chat-е был задан следующий запрос: "_Я хочу создать проект на C++ в котором описание системы сборки будет сделано на CMake. Компилятор и CMake уже установлены на моей системе. Это приложение должно реализовывать TUI-интерфейс с использованием библиотеки ftxui. Сгенерируй мне файл CMakeLists.txt для загрузки ftxui из репозиатрия на github и простое приложение в котором будет добавлена одна экранная кнопка, а также будет осуществляться выход из приложения при нажатии на кнопку 'q'_".

Агент предложил следующий вариант файла "CMakeLists.txt":

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

Koda предложил следующий вариант реализации файла "main.cpp":

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

Что очень удобно:

- Агент предлагает самостоятельно создать файл на диске, без необходимости выполнения операций copy&paste вручную
- Агент может копировать команды в Terminal (Shell)

Что сразу было сделано неправильно:

- Koda создала некорректную структуру проекта, добавив лишную промежуточную папку
- Сгенерированный код не соответствует требованиям и содержит ошибку - не добавлен код, который освобождает ресурсы при выходе из приложения, что приведёт к тому, что работоспособность терминала будет нарушена и его придётся закрывать вручную. Визуально это выглядит как появление координат курсора мыши в человеко-читаемой формате с блокированием возможности реальной работы
- Скрипты были сгенерированы под Linux, хотя операционная система - Windows. Вот эта команда выполнилась с ошибкой: `mkdir build && cd build`
- собранный файл оказался не там, где предполагался кода в своих инструкциях

Результат: после исправления ошибок, связанных с структурой проекта и shell-командами, приложение успешно собралось и заработало, но с указанными ранее ошибками.

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
