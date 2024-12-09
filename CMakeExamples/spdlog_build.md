# Библиотека логирования для проектов на C++

Одной из популярных библиотек для логирования в C++ проектах является [spdlog](https://github.com/gabime/spdlog). К достоинствам библиотеки можно отнести:

- Есть Rotating-логи, т.е. мы можем сказать, что в конкретном файле будет храниться не более 5 МБ (или другое число) логов, которые будут изменять этот файл циклически
- Запись в файл отложенная и параметр сохранения управляемый из прикладного кода
- Есть возможность писать логи в "дневной" файл
- Декларируется очень высокая скорость работы
- Есть богатые возможность формирования логов, вплоть до "раскраски" сообщений в консоли

## Загрузка и сборка

Клонирование библиотеки может быть осуществлена консольной командой:

```shell
git clone https://github.com/gabime/spdlog.git
```

В процессе экспериментального подтверждения клонирование завершилось ошибкой:

```output
Cloning into 'spdlog'...
remote: Enumerating objects: 30456, done.
remote: Counting objects: 100% (1156/1156), done.
remote: Compressing objects: 100% (443/443), done.
error: RPC failed; curl 92 HTTP/2 stream 5 was not closed cleanly: CANCEL (err 8)
error: 2727 bytes of body are still expected
fetch-pack: unexpected disconnect while reading sideband packet
fatal: early EOF
fatal: fetch-pack: invalid index-pack output
```

После изменения размера post-буфера клонирование завершилось успешно. Команда настройки размера буфера:

```shell
git config --global http.postBuffer 524288000
```

Для сборки демонстрационного проекта рекомендуется создать папку "build" в корне проекта и выполнить команды:

```shell
cmake ..
cmake --build .
```

На Windows-машине сборка выполняется успешно - в папке `\build\example\Debug` появляется файл с именем "example.exe". При запуске файла в консоль выводятся тестовые сообщения. Такие же сообщения сохраняются в различные файлы в дочерней папке "logs".

## Включение команд клонирования spdlog в CMakeLists.txt

Пример включения команды клонирования репозитария

```cmake
cmake_minimum_required(VERSION 3.11)
project(YourProjectName)

# Модуль FetchContent будет использоваться для загрузки репозитария
include(FetchContent)

# Загружаем репозитарий spdlog с GitHub
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.0  # Указывам тэг репозитария
)

# Проверяем, что библиотека spdlog была успешно загружена
FetchContent_MakeAvailable(spdlog)

# Включаем библиотеку spdlog в собираемый проект
add_executable(YourExecutableName main.cpp)
target_link_libraries(YourExecutableName PRIVATE spdlog::spdlog)
```

В дочерних подпроектах, можно уже не загружать репозитарий, а использовать определённую на верхнем уровне библиотеку. Пример:

```cmake
add_executable(YourChildExecutableName main.cpp)

target_link_libraries(YourChildExecutableName PRIVATE spdlog::spdlog)
```
