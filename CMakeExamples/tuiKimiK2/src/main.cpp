#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>

using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::TerminalOutput();

    // Структура для отслеживания задачи
    struct ProgressTask {
        int id;
        int percentage = 0;
        bool active = true;
    };

    // Вектор активных задач и мьютекс для потокобезопасности
    std::vector<std::shared_ptr<ProgressTask>> tasks;
    std::mutex tasks_mutex;
    std::atomic<int> next_task_id{1};

    // Кнопка запуска новой задачи
    auto buttonRunGauge = Button("Запустить задачу", [&]{
        // Создаём новую задачу через shared_ptr
        auto new_task = std::make_shared<ProgressTask>();
        new_task->id = next_task_id++;
        
        // Добавляем задачу в вектор (потокобезопасно)
        {
            std::lock_guard<std::mutex> lock(tasks_mutex);
            tasks.push_back(new_task);
        }

        // Запускаем задачу в отдельном потоке
        std::thread([&, task = new_task]{
            for (int i = 0; i <= 100; i++) {
                // Обновляем прогресс в UI-потоке
                screen.Post([&, task, i]{
                    task->percentage = i;
                    if (i == 100) {
                        task->active = false;
                    }
                });
                
                // Триггерим перерисовку
                screen.Post(Event::Custom);
                
                // 20 секунд / 100 шагов = 200 мс на шаг
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }).detach();
    });

    // Компонент для рендеринга всех gauge
    auto gauge_component = Renderer([&]{
        std::lock_guard<std::mutex> lock(tasks_mutex);
        
        // Удаляем неактивные задачи
        tasks.erase(
            std::remove_if(tasks.begin(), tasks.end(), 
                [](const std::shared_ptr<ProgressTask>& task) { 
                    return !task->active; 
                }),
            tasks.end()
        );
        
        // Если нет задач, возвращаем пустой элемент
        if (tasks.empty()) {
            return text("");
        }
        
        // Создаём вектор элементов для всех активных задач
        std::vector<Element> gauge_elements;
        for (const auto& task : tasks) {
            gauge_elements.push_back(
                vbox({
                    text("Задача #" + std::to_string(task->id) + ":"),
                    gauge((float(task->percentage)) / 100),
                    text(std::to_string(task->percentage) + "%")
                })
            );
            gauge_elements.push_back(separator());
        }
        
        // Удаляем последний лишний separator
        gauge_elements.pop_back();
        
        return vbox(gauge_elements);
    });

    // Основной контейнер с кнопкой
    auto container = Container::Vertical({
        buttonRunGauge
    });

    // Рендерер всего UI
    auto component = Renderer(container, [&]{
        std::vector<Element> elements = {
            text("Приложение с множественными задачами"),
            separator(),
            buttonRunGauge->Render(),
            separator(),
        };
        
        // ВСЕГДА добавляем gauge_component, он сам захватит mutex внутри
        elements.push_back(gauge_component->Render());
        
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