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