# Class Template Argument Deduction

Начиная с C++17 появилась возможность инициализировать некоторый класс массивом переменной длины с элементами разных типов. Называется это свойство языка **Class Template Argument Deduction** (CTAD).

Пример реализации:

```cpp
#include <iostream>
#include <array>
#include <cstddef>
#include <algorithm>
#include <type_traits>

template<class T, std::size_t N>
struct Box {
    std::array<T, N> data;

    // Этот конструктор разрешает использование конструкицй вида `Box b(arr);`
    constexpr Box(const T(&arr)[N]) {
        std::copy_n(arr, N, data.begin());
    }

    constexpr std::size_t size() const noexcept { return N; }
};

int main() {
    int raw1[] = { 1, 2, 3, 4 };
    Box b1(raw1);          // deduces Box<int, 4>

    Box b2("hello");       // string literal is const char[6] -> Box<const char, 6>

    static_assert(std::is_same_v<decltype(b1), Box<int, 4>>);
    static_assert(std::is_same_v<decltype(b2), Box<char, 6>>);

    std::cout << b1.size() << "\n";  // 4
    std::cout << b2.size() << "\n";  // 6 (includes '\0')
}
```

Замечу, что в Visual Studio 2022 для C++ проектов, по умолчанию устанавливается C++14 и код не собирается.

В книге "Software Architecture with C++" by Adrian Ostrowski, Piotr Gaczkowski приводится следующий пример:

```cpp
template<class T, class... U>
    array(T, U...) -> array<T, 1 + sizeof...(U)>;
```

Возможно использование конструкторов разных видов, как C-style, так и гораздо более лаконичного _Variadic constructor_:

```cpp
// Constructor from C-style array
ArrayWrapper(const T(&arr)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        data[i] = arr[i];
    }
}

// Variadic constructor
template<typename... Args>
ArrayWrapper(Args&&... args) : data{ std::forward<Args>(args)... } {
    static_assert(sizeof...(args) == N, "Number of arguments must match array size");
}

// Default constructor
ArrayWrapper() = default;
```
