## Аттрибутированный код

В современном C++ может быть использован атрибутированный код, который можно распознать по конструкции:

```cpp
[[attribute_name]]
```

Примеры использования:

```cpp
[[nodiscard]] int compute();
[[deprecated("use newFunc() instead")]] void oldFunc();
[[maybe_unused]] int temp = 0;
```

## Стандартные атрибуты C++20

- [[nodiscard]]	- предупреждать, если возвращаемое значение игнорируется
- [[maybe_unused]] - задавить предупреждение "unused variable/function"
- [[deprecated]] - помечает устаревшую декларацию. Будет выдаваться дополнительное сообщение при сборке
- [[fallthrough]] - прячет сообщение о том, что в switch/case есть провалы (в case отсутствует break)
- [[likely]] / [[unlikely]] - подсказка оптимизирующему компилятору
- [[no_unique_address]]

В конкретных компиляторах могут быть свои собственные наборы атрибутов, например:

```cpp
[[gnu::always_inline]]
[[clang::no_sanitize("address")]]
[[msvc::noinline]]
```

## Где атрибуты могут быть размещены

По материалам Telegram-канала `@grokaemcpp`.

Есть 3 места для навешивания атрибутов на функцию.

Перед типом возвращаемого значения:

```cpp
[[deprecated]] int foo() { return 42; }
```

Тогда он работает при непосредственном использовании функции.

После имени функции:

```cpp
int foo [[deprecated]] () { return 42; }
```

В таком виде атрибут тоже применяется к самой функции.

После параметров:

```cpp
int foo() [[gnu::deprecated]] { return 42; }
```

Тогда атрибут применяется к типу функции, а не к самой функции. Разница вот в чем:

```cpp
int foo() [[gnu::deprecated]] { return 42; }

int main() {
  foo(); // no warnings
  using FuncType = decltype(foo); // use of type is deprecated
}
```

Обычный вызов функции прекрасно компилируется. Но вот использование типа функции через decltype помечается как устаревшее.

В лямбде в тех же местах можно ставить атрибуты:

```cpp
auto complicated_compute = [] [[nodiscard]] () [[gnu::deprecated]] {
  return 2 * 2;
};
```
