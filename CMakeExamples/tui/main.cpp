#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/captured_mouse.hpp>

using namespace ftxui;

auto button_style = ButtonOption::Animated();

// Definition of the modal component. The details are not important.
Component ModalComponent(std::function<void()> do_nothing,
                         std::function<void()> hide_modal) {
  auto component = Container::Vertical({
      Button("Do nothing", do_nothing, button_style),
      Button("Quit modal", hide_modal, button_style),
  });
  // Polish how the two buttons are rendered:
  component |= Renderer([&](Element inner) {
    return vbox({
               text("Modal component "),
               separator(),
               inner,
           })                               //
           | size(WIDTH, GREATER_THAN, 30)  //
           | border;                        //
  });
  return component;
}

int main() {
    auto screen = ftxui::ScreenInteractive::TerminalOutput();

	// State of the application:
	bool modal_shown = false;

	// Some actions modifying the state:
	auto show_modal = [&] { modal_shown = true; };
	auto hide_modal = [&] { modal_shown = false; };

    // Ключевой момент: при выходе объекта exit из scope, очередь сообщений будет
    // уничтожена, все ресурсы будут освобождены
    auto exit = screen.ExitLoopClosure();
	
	auto do_nothing = [&] {};
	

    // Переменные, определяющие состояние приложения (State variables)
    int counter = 0;
    std::string input_text = "";
    int selected_tab = 0;
    bool checkbox_state = false;

    // Переменная для gauge (используем atomic для потокобезопасности)
    std::atomic<float> gauge_value = 0.0f;
    std::atomic<bool> timer_running = true;

    // Создаём объект для ввода текстовых данных
    auto input = Input(&input_text, "Type something...");

    // Создаём кнопки и добавляем для каждой из них обработчик нажатия
    auto button_increment = Button("Increment", [&] { counter++; });
    auto button_decrement = Button("Decrement", [&] { counter--; });
    auto button_reset = Button("Reset", [&] { counter = 0; });
	auto button_modal = Button("Show modal", show_modal);

    // Добавляем check-box и привязываем к нему state-переменную
    auto checkbox = Checkbox("Enable feature", &checkbox_state);

    // Основной контейнер пользовательских элементов - горизонтальный
    auto container = Container::Vertical({
        input,
        Container::Horizontal({
            button_increment,
            button_decrement,
            button_reset,
			button_modal,
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
				button_modal->Render()
            }),
            separator(),
            checkbox->Render(),
            separator(),
            text("Status: " + std::string(checkbox_state ? "Enabled" : "Disabled")),
            separator(),
            // Добавляем gauge
            hbox({
                text("Progress: "),
                gauge(gauge_value.load()) | flex,
            }),
            text("  " + std::to_string(int(gauge_value.load() * 100)) + "%") | center,
            separator(),
            text("Press 'q' to quit") | dim,
                }) | border;
        });

    // Конфигурируем обработчики очереди сообщений, в которых нас интересует кнопка 'q',
    // при при нажатии на которую требуется завершить работу приложения
    renderer |= CatchEvent([&](Event event) {

        if (event == Event::Character('q')) {
            std::exit(0);
            return true;
        }

        return false;
    });

	// Добавляем модальный диалог к основному диалогу
	auto modal_component = ModalComponent(do_nothing, hide_modal);
	
	// Use the `Modal` function to use together the main component and its modal
	// window. The |modal_shown| boolean controls whether the modal is shown or
	// not.
	renderer |= Modal(modal_component, &modal_shown);
	
    // Запускаем таймер в отдельном потоке
    std::thread timer_thread([&]() {
        while (timer_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // Увеличиваем значение gauge
            gauge_value = gauge_value.load() + 0.01f;

            // Сбрасываем при достижении 1.0
            if (gauge_value >= 1.0f) {
                gauge_value = 0.0f;
            }

            // Запрашиваем перерисовку экрана
            screen.Post(Event::Custom);
        }
    });

    // Создаём очередь сообщений и запускаем основной цикл для определённых нами элементов
    // пользовательского интерфейса
    screen.Loop(renderer);

    // Ожидаем завершения потока таймера
    timer_running = false;
    if (timer_thread.joinable()) {
        timer_thread.join();
    }

    return 0;
}
