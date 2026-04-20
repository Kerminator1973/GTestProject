# GodBolt.org - оптимизация кода

Интернет ресурс [GodBolt.org](https://godbolt.org/) реализует облачную систему компиляции кода из C++ в ассемблер. Цель - используя конструкции на C++ увидеть, насколько эффективно они будут скомпилированы конкретным компилятором. Поддерживается множество компиляторов под разные аппаратные платформы.

Например, можно указать следующий код (он уже [доступен по ссылке](https://godbolt.org/z/PYMTYz)):

```cpp
#include <algorithm>
#include <array>

struct Merchant {
  int id;
};

bool has_merchant(const Merchant &selected) {
  auto merchants = std::array{Merchant{1}, Merchant{2}, Merchant{3},
                              Merchant{4}, Merchant{5}};
  return std::binary_search(merchants.begin(), merchants.end(), selected,
                            [](auto a, auto b) { return a.id < b.id; });
}

int main() { return has_merchant({4}); }
```

То компилятор сгенерирует целую кучу ассемблерных инструкций.

А вот если заменить возвращаемый тип на `constexpr auto`:

```cpp
constexpr auto has_merchant(const Merchant& selected) {
```

А также указать ключи компиляции `-O3 --std=c++2a`, а в качестве компилятора использования x86-gcc (trunk), то компилятор сгенерирует очень компактный код:

```
"main":
        mov     eax, 1
        ret
```
