# Scope Exit

Весьма распространённая практика для освобождения ресурсов, использующая RAII.

Простой пример использования **BOOST_SCOPE_EXIT**:

```cpp
#include <boost/scope_exit.hpp>
#include <iostream>

int main() {
    int resource = 123;
    BOOST_SCOPE_EXIT() {
        std::cout << "Освобождаем ресурс: " << resource << "\n";
    } BOOST_SCOPE_EXIT_END

    std::cout << "Работаем с ресурсом...\n";
    // При выходе из main выполнится блок выше
    return 0;
}
```

На практике, основной негативный эффект - использование нескольких BOOST_SCOPE_EXIT() в одной длинной функции усложняют верификацию кода.

Тот же самый эффект достигается при использовании **Guidelines Support Library** (GSL), см. gsl::finally():

```cpp
#include <gsl/gsl>
#include <fstream>
#include <mutex>

void use_file_and_lock() {
    std::ofstream file("data.txt");
    std::mutex mtx;
    std::lock_guard<std::mutex> lg(mtx); // блокировка мьютекса

    // Гарантируем закрытие файла при выходе из scope
    auto close_file = gsl::finally([&file] {
        file.close();
        std::cout << "Файл закрыт\n";
    });

    // Работа с файлом и мьютексом...
    // Даже если здесь будет исключение — файл закроется
}
```
