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

Вопрос к KODA: почему был выбран TAG v5.0.0 от 19 августа 2023 года, тогда как есть более актуальные TAG-и, в частности: v6.1.9 от 7 мая 2025 года?

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
- У Koda, фактически, некорректно работает merge, т.е. код, который вносит человек не сохраняется в контексте и удаляется при последующих генерациях кода ИИ
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

При использовании [Kimi K2](https://www.kimi.com/) расходуются токены. Тестовое количество токенов - 1000.

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

Компилятор сообщает о предупреждении:

```
D:\Sources\KodaTUI\src\main.cpp(52,27): warning C4244: 'argument': conversion from 'int' to 'float', possible loss of data [D:\Sources\KodaTUI\build\tui_app.vcxproj]
```

Причина замечания - в структуре данных progress предполагается, что значение прогресса целочисленное. Это предположение кажется разумным, поскольку в текстовом режиме в одной строке вряд ли будет отображено более 100 символов.

Однако в библиотеке ftxui `gauge()` принимает вещественное число типа _float_, что более универсально.

Другими словами, сгенерированное Kimi K2 приложение изменяет значения полосы прокрутки в диапазоне от 0 до 100, а библиотека ftxui ожидает изменения в диапазоне от 0 до 1. Соответственно, полоса прогресса всегда отображается заполненной.

Для устранения ошибки мне было достаточно вручную изменить код:

```cpp
gauge_elements.push_back(
    vbox({
        text("Задача #" + std::to_string(task.id) + ":"),
        gauge((float(task.percentage)) / 100),              // <-- Здесь я добавил деление на 100
        text(std::to_string(task.percentage) + "%")
    })
);
gauge_elements.push_back(separator());
```

После этой модификации код заработал корректно.

Исходные тексты приложения приведены в файле "\CMakeExamples\tuiKimiK2\src\main.cpp".

### Отображение нескольких gauge

>При нажатии кнопки buttonRunGauge необходимо добавлять новый gauge в пользовательский интерфейс. Цель - возможность запуска нескольких задач пользователя одновременно.

По мере усложнения задачи энтропия стала возрастать и степень серьёзности ошибок увеличилась.

Сгенерированный Kimi K2 код не учёл внесённый мной в код изменений, сохранив прежний контекст выполнения. Код скомпилировался, но при попытке запуска задачи возникла критичная ошибка.

Поскольку в качестве системы сборки испольузется CMake, был сгенерирован solution для Visual Studio 2022, т.е приложение можно запустить под отладчиком и увидеть место в коде, в котором возникло не обработанное исключение.

Ошибка возникла вот здесь:

```cpp
// Компонент для рендеринга всех gauge
auto gauge_component = Renderer([&]{
    std::lock_guard<std::mutex> lock(tasks_mutex);
```

Анализ сбоя позволяет утверждать, что возникает Deadlock, см. код:

```cpp
void lock() {
    if (_Mtx_lock(_Mymtx()) != _Thrd_result::_Success) {
        // undefined behavior, only occurs for plain mutexes (N4950 [thread.mutex.requirements.mutex.general]/6)
        _STD _Throw_Cpp_error(_RESOURCE_DEADLOCK_WOULD_OCCUR);
    }
```

>Сгенерированный код вызывает Deadlock при попытке захвата mutex-а в строке:
>
>```cpp
>std::lock_guard<std::mutex> lock(tasks_mutex);
>```
>
>Исправь возникшую ошибку.

Kimi K2 действительно исправил ошибку и приложения стало запускать несколько полос прогресса на экране для разных задач.

Однако gauge всегда отображается заполненным.

## Серьёзная проблема с производительностью кода

В действительности, в коде сгенерированном Kimi K2 есть значимые дефекты. Содержатся дефекты в следующем коде:

```cpp
// Запускаем задачу в отдельном потоке
std::thread([&, task_id = new_task.id]{
    for (int i = 0; i <= 100; i++) {
        // Обновляем прогресс в UI-потоке
        screen.Post([&, task_id, i]{
            std::lock_guard<std::mutex> lock(tasks_mutex);
            for (auto& task : tasks) {
                if (task.id == task_id) {
                    task.percentage = i;
                    if (i == 100) {
                        task.active = false;
                    }
                    break;
                }
            }
        });
        
        // Триггерим перерисовку
        screen.Post(Event::Custom);
        
        // 20 секунд / 100 шагов = 200 мс на шаг
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}).detach();
```

Предположим, что у нас работает несколько gauges, которые интенсивно двигают каждый свою полосу прогресса. При каждом шаге, каждый gauge вызывает `screen.Post(Event::Custom);`. В приведённом выше примере, если у нас будет пять gauges, то перерисовка пользовательского интерфейса будет осуществляться 200 мс / 5 = 40 мс, т.к. каждые 40 мс. Другими словами, паразитная нагрузка очень высокая и её нужно было бы уменьшить. Как вариант, сделать отдельный поток, код которого фиксирует изменения gauges, но перерисовывает пользовательский интерфейс не чаще, чем раз в 200 мс.

Второй дефект состоит в том, что код на каждой итерации ищет свою задачу по идентификатору в контейнере задач - `for (auto& task : tasks) {`. Поскольку task - это объект, мы могли бы сохранить ссылку на этот объект при запуске потока и избежать цикла. Здесть есть ещё один сомнительный момент - код ищет задачу в цикле. Предположим, что это делается для надёжности - вдруг в контейнере tasks не будет этой задачи! А что произойдёт, если задачи там действительно не окажется? Цикл будет продолжаться ещё до 20 секунд, пользовательский интерфейс будет перерисовываться - т.е. ресурсы будут бесполезно расходоваться.

Замечу, что на эти замечания можно закрыть глаза, т.к. производительность современных процессоров очень высокая. Однако существует накопительный эффект, при котором в некоторый момент накопленные дефекты в совокупности будут приводить к критичному падению производительности системы.

### Следующая итерация улучшений

>При каждой итерации цикла, осуществляется поиск task в контейнере. Мне кажется, что поиск task в каждом цикле является избыточным и ссылку на него можно было бы найти один раз, при входе в цикл. Оптимизируй свой код.

```cpp
for (int i = 0; i <= 100; i++) {
    // Обновляем прогресс в UI-потоке
    screen.Post([&, task_id, i]{
        std::lock_guard<std::mutex> lock(tasks_mutex);
        for (auto& task : tasks) {
            if (task.id == task_id) {
                task.percentage = i;
                if (i == 100) {
                    task.active = false;
                }
                break;
            }
        }
    });
    //...
}
```

Kimi K2: "_Правильно замечено! Поиск задачи на каждой итерации цикла (100 раз) избыточен. Оптимизация возможна: найти задачу один раз до цикла и работать с ней напрямую через указатель._"

Kimi K2 предложила следующий вариант решения проблемы:

```cpp
// Запускаем задачу в отдельном потоке
std::thread([&, task_id = new_task.id]{
    // === ОПТИМИЗАЦИЯ: Находим задачу один раз ===
    ProgressTask* task_ptr = nullptr;
    {
        std::lock_guard<std::mutex> lock(tasks_mutex);
        for (auto& task : tasks) {
            if (task.id == task_id) {
                task_ptr = &task;
                break;
            }
        }
    }
    
    // Если задача не найдена, выходим
    if (!task_ptr) {
        return;
    }
    
    // Обновляем прогресс напрямую, без поиска
    for (int i = 0; i <= 100; i++) {
        screen.Post([&, i]{
            task_ptr->percentage = i;
            if (i == 100) {
                task_ptr->active = false;
            }
        });
        
        screen.Post(Event::Custom);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}).detach();
```

Однако этот вариант останавливает визуализацию прогресса всех раннее зарущенных задачи при добавлении новой.

Кроме этого, вариант Kimi K2 переходит к низкоуровневому программированию, в котором используются указатели. Мне бы хотелось избежать смешения низкоуровневого и высокоуровневого стилей программирования.

Почему код перестал работать?

Конкретно вызов `screen.Post(Event::Custom)` имитирует нажатие кнопки на клавиатуре. Вызов `screen.Post()` осуществляет потокобезопасное обновление UI из фонового потока. Если мы помещает в Post() лямбду, то поток пользовательского интерфейса выполнит её, когда будет готов. Переменная i захватывается по значению, чтобы избежать гонки данных.

Разъяснение от Kimi K2:

> Вы абсолютно правы, проблема в невалидности указателя task_ptr. Когда вы добавляете новую задачу в вектор `tasks.push_back(...)`, вектор может перераспределить память, и все ранее полученные указатели на элементы становятся невалидными. Это приводит к тому, что старые потоки пишут в освобождённую память, и UI не видит обновлений.
>
>Нужно использовать std::shared_ptr, чтобы задача жила ровно столько, сколько нужно всем потокам, которые с ней работают.

Разъяснение от Grok 4:

> Да, в C++ операция push_back() на контейнере std::vector (если tasks является экземпляром std::vector) может привести к изменению адресов элементов. Это происходит из-за возможной реаллокации памяти: когда вектор заполняется и его текущая ёмкость (capacity) недостаточна для добавления нового элемента, вектор выделяет новый блок памяти большего размера, копирует или перемещает существующие элементы в него, а затем освобождает старый блок. В результате указатели, итераторы и ссылки на элементы становятся недействительными.

И это действительно так, `std::vector<>` именно так и работает. Ключевое изменение, которые сделал Kimi K2 - реализовал tasks не как вектор ProgressTask, а как вектор `std::shared_ptr<ProgressTask>`. Было:

```cpp
std::vector<ProgressTask> tasks;
```

Стало:

```cpp
std::vector<std::shared_ptr<ProgressTask>> tasks;
```

Это действительно очень умный ход, поскольку в фоновых поток можно захватывать указатель на конкретную задачу `auto new_task = std::make_shared<ProgressTask>();`, избежать использования дорогостоящего mutex-а, а обновление пользовательского интерфейса безопасно выполняется через очередь изменений.

Проблема с избыточной блокировкой была успешно решена.

### Борьба с избыточным re-rendering-ом

Сформировал следующий запрос:

>В текущей реализации каждый из фоновых потоков изменения полосы прогресса (gauge) каждые 200 миллисекунд вызывает screen.Post(Event::Custom) для рендеринга пользовательского интерфейса. Если у нас запущено 5 фоновых потоков, то повторные рендеринг выполняется каждые 40 миллисекунд. Это создаёт высокую вычислительную нагрузку. Оптимизируй код таким образом, чтобы уменьшить количество повторных рендерингов.

Ответ Kimi K2:

>Верно замечено! Текущая реализация создает избыточную нагрузку. Оптимизируем код, введя глобальный таймер рендеринга с фиксированной частотой (30 FPS ~ 33 мс), независимо от количества фоновых задач.

Далее LLM вошла в режим рассуждений рассмотрев пять разных вариантов решения проблемы, из которых отвергла 4. Режим рассуждений - очень интересный, т.к. эти варианты можно проанализировать в дальнейшем и, возможно, какой-то из отвергнутых режимов окажется вполне работоспособным.

Вместе с тем сгенерированный Kimi K2 привер кода не заработал, т.к. LLM придумала не существующий метод PostDelayed():

```
D:\Sources\KodaTUI\src\main.cpp(65,28): error C2039: 'PostDelayed': is not a member of 'ftxui::ScreenInteractive' [D:\Sources\KodaTUI\build\tui_app.vcxproj]
```

Bug-fix также оказался не работоспособным:

```cpp
D:\Sources\KodaTUI\src\main.cpp(55,24): error C3493: 'active_tasks_count' cannot be implicitly captured because no default capture mode has been specified [D:\Sources\KodaTUI\build\tui_app.vcxproj]
D:\Sources\KodaTUI\src\main.cpp(56,25): error C3493: 'needs_render' cannot be implicitly captured because no default capture mode has been specified [D:\Sources\KodaTUI\build\tui_app.vcxproj]
D:\Sources\KodaTUI\src\main.cpp(60,49): error C3493: 'min_render_interval' cannot be implicitly captured because no default captu re mode has been specified [D:\Sources\KodaTUI\build\tui_app.vcxproj]
D:\Sources\KodaTUI\src\main.cpp(62,17): error C3493: 'render_timer_running' cannot be implicitly captured because no default capture mode has been specified [D:\Sources\KodaTUI\build\tui_app.vcxproj]
```

Можно констатировать, что на задаче оптимизации управления рендерингом LLM сломалась.

На уровне идей, она сгенерировала следующие:

- рекурсивный вызов функции генерации ре-рендеринга на каждой итерации цикла прогресса в каждом потоке. Это очень переусложнённый подход и Kimi K2 не смогла написать код с правильно реализованным замыканием
- throttling (притормаживание) в UI-потоке. Вариант не плохой, но throttling происходит для любых изменений, а не только при движении полос прогресса. Это тоже переусложнение кода и не факт, что этот подход работоспособен
- отдельный фоновый поток для повторного рендеринга. Не плохая идея, но создаёт дополнительный поток, что является ресурсоёмким решением
- вернулся к варианту с рекурсией во вспомогательном функциональном объекте (указателе на функцию) render_timer. Этот вариант Kimi K2 посчитал хорошим, но не смог написать компилируемый код

Kimi K2 снова вернулся к варианту с дополнительным фоновым потоком, который снова не собрался.

Как человек-программист, я бы рассмотрел вариант изменить шаг итерации до 2, или до 3. Также я бы попробовал выполнять изменения всех полос прогресса в одном потоке, а не создавать отдельный поток для каждого из gauges. Однако это меняет подход к реализации кода принципиально и требует полной переработки кода.

## Результат

Модель Kimi K2 продемонстрировала впечатляющие результаты. Возникшие критические ошибки удалось исправить, приложение заработало именно так, как и ожидалось. На некоторые замечания можно закрыть глаза. Долгое время работы модели - это значимый фактор риска.

Вайб-кодинг оптимизирует операцию кодирования в цикле:

- текстовое описание задачи
- КОДИРОВАНИЕ
- проверка результата

Рассуждающие модели выглядят очень интересно, т.к. позволяют сопоставлять разные варианты решения проблемы.

Риски, связанные с Vibe-coding:

- ухудшение навыков кодирования при злоупотреблении vide-кодингом
- при кодировании человеком, автоматически проверяются гипотезы, заложенные в алгоритм, т.е. фаза проверки результата осуществляется на фазе кодирования, а не потом
- LLM может не предложить разумного решения. Следует использовать мощные, "рассуждающие" LLM
- Затраты на анализ результата работы LLM могут быть соизмеримы, или даже больше, чем при ручном кодировании, но результат такой совместной работы может быть даже лучше, чем при ручном кодировании

Plug-in Koda отключен, но я буду продолжать эксперименты с Koda используя мощные модели, в частности - Kimi K2.

### Что пока не удалось попробовать

Возможности Koda существенно шире, чем были рассмотрены в рамках генерации TUI-приложения.

В следующих итерациях необходимо рассмотреть инструменты Koda, доступные при вводе символов `/` и `@` в строке промпта. В частности:

- read_file: читает содержимое файла в рабочей области (@Files)
- view_diff: показывает текущий git diff незакоммиченных изменений
- codebase_tool: семантический поиск по кодовой базе на естественном языке (@Codebase)

Также следует разобраться с командами:

- /kodamd - генерирует или обновляет файл KODA.md в корневой директории проекта. Этот файл содержит правила проекта, которые Agent использует для понимания предпочтений и ограничений при выполнении задач
- /summarize - сжимает историю текущего разговора, чтобы предотвратить переполнение контекстного окна модели
