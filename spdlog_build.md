# Библиотека логирования для проектов на C++

Одной из популярных библиотек для логирования в C++ проектах является [spdlog](https://github.com/gabime/spdlog). К достоинствам библиотеки можно отнести:

- Есть Rotating-логи, т.е. мы можем сказать, что в конкретном файле будет храниться не более 5 МБ (или другое число) логов, которые будут изменять этот файл циклически
- Запись в файл отложенная и параметр сохранения управляемый из прикладного кода
- Есть возможность писать логи в "дневной" файл
- Декларируется очень высокая скорость работы
- Есть богатые возможность формирования логов, вплоть до "раскраски" сообщений в консоли (на базе библиотеки fmt)
- Очень простое подключение default-ного logger-а (вывод в консоль раскрашенных текстовых сообщений)

## Загрузка и сборка

Клонирование библиотеки может быть осуществлена консольной командой:

```shell
git clone https://github.com/gabime/spdlog.git
```

В процессе экспериментального подтверждения работоспособности, клонирование завершилось ошибкой:

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

## Пример кода

Заголовочные файлы:

```cpp
// Включаемый файлы библиотеки логирования spdlog
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
```

Пример записи сообщения в Log:

```cpp
// Пример использования компонента логирования splog
try
{
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    logger->set_level(spdlog::level::debug);
    logger->info("Used COM-port: {0}", strPortName.c_str());
}
catch (const spdlog::spdlog_ex& ex)
{
    std::cout << "Log init failed: " << ex.what() << std::endl;
}
```

Конструктор basic_logger_mt создаёт экземпляр класса `std::shared_ptr<spdlog::logger>`. Ценность использования умного указателя - logger будет автоматически удалён при выходе за пределы области видимости.

## Сборка под Windows

Для сборки приложения под Windows может потребоваться добавить библиотеку Threads:

```cmake
# Подключаем библиотеку Threads
find_package(Threads REQUIRED)
```

```cmake
# Подключаем библиотеки, из подкаталогов "ccnet" и "dslip". Также подключаем библиотеки Boost и spdlog
target_link_libraries(${PROJECT_NAME} ccnet dslip dxml BvsPackages Threads::Threads spdlog::spdlog ${Boost_LIBRARIES})
```

Скрипт подключения библиотеки ищет доступную библиотеку для обеспечения многопоточности. В приведённом ниже примере найдена библиотека Threads:

```output
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - not found
-- Found Threads: TRUE
```

Однако как только мы добавим заголовочный файл:

```cpp
#include "spdlog/spdlog.h"
```

При сборке приложения может быть найдено множество сообщений, связанных с выбранной кодировкой страницы:

```output
E:\Sources\DSlipPortable\dxml\dxmlDevice.h(1,1): warning C4828: The file contains a character starting at offset 0x467 that is illegal in the current source character set (codepage 65001). [E:\Sources\DSlipPortable\build\dsliportable.vcxproj] (compiling source file '../main.сpp')
```

Это сообщение говорит о том, что компилятор исходит из того, что текущей кодовой страницей является 65001 (UTF-8), но в некоторые заголовочные файлы, используемые в "main.cpp" имеют другую кодировку (CP-1251) и кириллические символы (в частности - текст комментариев) не соответствуют UTF-8. Чтобы решить проблему, достаточно найти такие заголовочные файлы и выполнить их конвертацию из CP-1251 в UTF-8.
