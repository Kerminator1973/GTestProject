# Возвращаемые значения в С++

Традиционных подход для Си и ранних версий С++ состоит в том, что некоторая функция возвращает флаг успешности операции, а результат значение - через ссылочный параметр, или указатель. Этот подход далёк от математически корректно и приводит к увеличению когнитивной сложности кода.

Долгое время казалось, что решением проблемы является создание и обработка исключений, но со временем стало понятно, что обработка исключений это дорогой (с точки зрения расхода вычислительных ресурсов), а также потенциально, ещё более усложняющий сопровождение кода подход.

Среди рассматриваемых альтернатив - возвращать структуру (хм...), или tuple. Тоже варианты не совсем уж типовые и бесплатные.

В языке Rust появились революционные типы Result и Option (см. также `expect()`), которые позволили писать легко читаемый код, который работает также быстро, как и код на C++ без использования исключений.

Начиная с C++ 17 стали появляться конструкции, которые позволяются сделать тоже самое, что и Rust с типами Result и Option.

## std::expected в C++ 23

В C++ 23 появилась возможность возвращать из метода как сообщение об ошибке, так и результат выполнения. Пример кода:

```cpp
#include <iostream>
#include <expected>
#include <string>

// Using std::expected with a custom error type
std::expected<double, std::string> safe_divide(double numerator, double denominator) {
    if (denominator == 0) {
        return std::unexpected("Division by zero error");
    }
    return numerator / denominator;  // Return the result wrapped in std::expected
}

int main() {
    double numerator = 10.0;
    
    // Attempting a safe division
    auto result = safe_divide(numerator, 0);
    
    // Check if the result is valid
    if (result.has_value()) {
        std::cout << "Result: " << result.value() << '\n';  // Accessing the successful result
    } else {
        std::cout << "Error: " << result.error() << '\n';   // Accessing the error message
    }

    // Trying a valid division
    result = safe_divide(numerator, 2);
    
    // Check if the result is valid
    if (result.has_value()) {
        std::cout << "Result: " << result.value() << '\n';
    } else {
        std::cout << "Error: " << result.error() << '\n';
    }

    return 0;
}
```

В чём преимущество std::expected над std::variant:

- хранит только два типа: значение и ошибка
- делает код интуитивно понятнее, те.е. для создания ошибки следует использовать std::unexpected
- предоставляет простой и лаконичный базовый интерфейс: has_value(), value() и error()

## std::optional в C++ 17

Ту же самую задачу можно решить используя std::optional, который появился в C++ 17:

```cpp
#include <iostream>
#include <optional>
#include <string>

// Функция safe_divide возвращает std::optional
std::optional<double> safe_divide(double numerator, double denominator) {
    if (denominator == 0) {
        return std::nullopt; // Возвращаем std::nullopt при делении на ноль
    }
    return numerator / denominator; // Возвращаем результат
}

int main() {
    double numerator = 10.0;

    // Пробуем деление на ноль
    auto result = safe_divide(numerator, 0);

    // Проверяем, было ли значение возвращено
    if (result) {
        std::cout << "Result: " << *result << '\n'; // Доступ к успешному результату
    } else {
        std::cout << "Error: Division by zero\n";   // Сообщение об ошибке
    }

    // Пробуем корректное деление
    result = safe_divide(numerator, 2);

    // Проверяем, было ли значение возвращено
    if (result) {
        std::cout << "Result: " << *result << '\n';
    } else {
        std::cout << "Error: Division by zero\n";
    }

    return 0;
}
```

std::optional лучше, чем громозкий std::variant, но гораздно хуже, чем std::expected, т.к. для индикации ошибки используется std::nullopt, который не предполагает, что будет возвращена информация о типе ошибки. Мы просто возвращаем флаг - если ли ошибка, или её нет.

## std::variant - C++ 17

Ту же самую задачу можно решать используя std::variant из стандарта C++ 17:

```cpp
#include <iostream>
#include <variant>
#include <string>

// Объявляем тип, который будет либо результатом, либо ошибкой
using Result = std::variant<double, std::string>;

Result safe_divide(double numerator, double denominator) {
    if (denominator == 0) {
        return "Error: Division by zero"; // Возвращаем строку при делении на ноль
    }
    return numerator / denominator; // Возвращаем результат
}

int main() {
    double numerator = 10.0;

    // Пробуем деление на ноль
    auto result = safe_divide(numerator, 0);

    // Проверяем, какой тип содержится в variant
    if (std::holds_alternative<double>(result)) {
        std::cout << "Result: " << std::get<double>(result) << '\n'; // Доступ к успешному результату
    } else {
        std::cout << std::get<std::string>(result) << '\n'; // Сообщение об ошибке
    }

    // Пробуем корректное деление
    result = safe_divide(numerator, 2);

    // Проверяем, какой тип содержится в variant
    if (std::holds_alternative<double>(result)) {
        std::cout << "Result: " << std::get<double>(result) << '\n';
    } else {
        std::cout << std::get<std::string>(result) << '\n';
    }

    return 0;
}
```
