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
