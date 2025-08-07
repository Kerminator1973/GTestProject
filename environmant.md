# Работа с переменными окружения в C++

Начиная с C++ 98/03 в стандартной библиотеке был метод std::getenv(), который позволяет работать с переменными окружения. Пример:

```cpp
const char* path = std::getenv("PATH");

if (path != nullptr) {
    std::cout << "PATH environment variable: " << path << std::endl;
} else {
    std::cout << "PATH environment variable not found." << std::endl;
}
```

Для более аккуратной обработки кода возврата std::getenv() может быть использован std::optional<>, который появился в C++17:

```cpp
std::optional<std::string> get_env_var(const std::string& name) {
    if (const char* value = std::getenv(name.c_str())) {
        return std::string(value);
    }
    return std::nullopt;
}

// Пример использования
if (auto path = get_env_var("PATH")) {
    std::cout << "PATH: " << *path << std::endl;
}
```

Также, для получения переменных окружения можно использовать третий параметр функции main() - `char* envp[]`. Например:

```cpp
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
    printf("Arguments:\n");
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    printf("\nEnvironment Variables:\n");
    for (int i = 0; envp[i] != NULL; i++) {
        printf("envp[%d] = %s\n", i, envp[i]);
    }

    return 0;
}
```
