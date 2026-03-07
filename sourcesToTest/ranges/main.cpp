#include <iostream>
#include <vector>
#include <ranges>
#include <string>

int main() {
    // Контейнер с исходными данными
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Составной view: фильтруем чётные числа, затем возводим в квадрат
    auto result = numbers
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::transform([](int n) { return n * n; });

    std::cout << "Чётные числа, возведённые в квадрат:\n";
    for (int value : result) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    // Ещё один пример: строки → фильтр по длине → перевод в верхний регистр
    std::vector<std::string> words = {
        "cat", "elephant", "dog", "rhinoceros", "ox", "hippopotamus"
    };

    auto long_words = words
        | std::views::filter([](const std::string& s) { return s.size() > 4; })
        | std::views::transform([](std::string s) {
            for (char& c : s) c = static_cast<char>(std::toupper(c));
            return s;
          });

    std::cout << "\nСлова длиннее 4 символов в верхнем регистре:\n";
    for (const auto& word : long_words) {
        std::cout << word << " ";
    }
    std::cout << "\n";

    return 0;
}