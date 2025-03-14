# vcpkg для установки библиотек

[vcpkg](https://vcpkg.io/en/) - свободно распространяемый менеджер проектов для приложений на C++. Развитием vcpkg занимается Microsoft C++ Team и сообщество разработчиков приложений. Более 2200 библиотек доступно через данных репозитарий. Для установки библиотек используются скрипты проекта и CMake. Решение работает под Windows, Linux и macOS.

Базовая статья о vcpkg находится [здесь](https://learn.microsoft.com/en-us/vcpkg/get_started/overview).

vcpkg это весьма большой репозитарий, который обновляется почти ежечасно и содержит инструкции сборки для ~2300 библиотек на С++. Важно понимать, что значительная часть ревизий в репозитарии являются не стабильными, т.к. крайне сложно гарантировать совместимость всех 2300 библиотек. В репозитарии систематически создаются т.н. Releases - выпуски, которые считаются стабильными. При выборе конкретной ревизии репозитария следует ориентироваться именно на релизы.

Соответственно, для загрузки и подготовки нужных библиотек нужно либо выбрать "правильный" релиз репозитария vcpkg, либо использовать т.н. "manifest mode" - разработать специальный файл, в котором следует указать, какие версии каждой из библиотек следует использовать.

Использование vcpkg может быть отличным решением даже для сборки проекта, который использует только одну библиотеку The Boost Library. Причина - для сборки библиотеки могут потребоваться внешние библиотеки gzip и openssl, т.е. объём работы выполняемой вручную, без использования package manager-а, чаще всего кратно больше, чем с использованием vcpkg.

Крайне важным понятием в vcpkg является triplets - совокупность описания операционной системы, архитектуры процессора, компилятора и runtime. Всего в vcpkg определено более 70 triplets и при необходимости можно добавить свои собственные. Этот подход позволяет выполнять кросс-компиляцию. Например, на машине с архитектурой x86-64 можно собирать приложения с triplets **arm64-linux**, т.е. собирать приложения для 64-битных процессоров ARM.

Следует заметить, что vcpkg интегрирован с CMake и ninja. vcpkg может выступать для CMake в качестве toolchain-а.

Таким образом vcpkg снимает с разработчика задачи установки необходимых библиотек и настройки процесса кросс-компиляции.

## Предварительные условия для начала работы с vcpkg

Предварительные условия работы с vcpkg - установка клиента git, CMake и средств компиляции:

```shell
sudo apt install git
```

```shell
sudo apt update && sudo apt install build-essential
```

Установка ускорителя сборки - Ninja:

```shell
sudo apt install ninja-build
```

Если планируется использовать кросс-компиляцию, необходимо установить дополнительный кросс-компилятор. Сделать это можно разными способами, один из вариантов такой:

```shell
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

Также может потребоваться установка пакета pkg-config, который позволяет управлять ключами компиляции и флагами сборки (линковки). Установить его можно так:

```shell
sudo apt-get install pkg-config
```

>Чтобы получить ключи компиляции некоторого package, можно использовать параметр `--cflags`. Для получения ключей линковки, следует использовать параметр `--libs`:
>
>```shell
>pkg-config --cflags glib-2.0
>pkg-config --libs glib-2.0
>```

Для работы с репозитарием библиотек следует клонировать из GitHub проект vcpkg:

```shell
git clone https://github.com/microsoft/vcpkg.git
```

> При загрузке репозитария могут возникнуть проблемы, в частности, может не хватать буфера для загружаемых данных. Решить эту проблему можно командой:
>
>```shell
>git config --global http.postBuffer 524288000
>```
>
> Для работы через корпоративный прокси-сервер может потребоваться его настройка. Для этого можно добавить в файл `/etc/environment`следующие строки:
>
>``` shell
>http_proxy="http://192.168.100.200:3128"
>https_proxy="http://192.168.100.200:3128"
>```

Для сборки vcpkg в Ubuntu/Debian, дополнительно может потребоваться установить следующие зависимости:

```shell
sudo apt-get install curl zip unzip tar
```

Далее необходимо запустить bootstrap для системы на которой планируется использовать репозитарий (sh/bat). Для Linux это следующая команда: `./bootstrap-vcpkg.sh`

В Microsoft Windows скрипт компиляции называется `e:\SourcesFromGit\vcpkg\bootstrap-vcpkg.bat`

Если каких-то зависимостей не будет, скрипт выведет на экране вспомогательные команды, которые помогут установить необходимые компоненты.

В случае успешной сборки, будут загружены дополнительные библиотеки и появится файл vcpkg.

Пример использования vcpkg для установки библиотеки soci:

```shell
vcpkg install soci
```

При попытке загрузки библиотеки, может оказаться, что установленная версия CMake в системе отсутствует и это может привести к тому, что vcpkg загрузит актуальные исходники CMake и соберёт продукт из них.

Как результат, в папках vcpkg появляются заголовочные файлы и lib-ы библиотеки soci. Их можно подключать в проект вручную, или через CMake.

## Решение проблем

Некоторые библиотеки могут включать большое количество компонентов, установка всех компонентов может быть избыточной. Более того, некоторые из компонентов могут не собираться, что приведёт к ошибкам установки всего пакета. Пример - компонент поддержки Python в The Boost Library.

> Для решения проблем с запуском приложений собираемых для других аппаратных архитектур может быть использован эмулятор QEMU. Подключить эмулятор QEMU можно используя переменную `CMAKE_CROSSCOMPILING_EMULATOR`, например:
>
>```cmake
>set(CMAKE_CROSSCOMPILING_EMULATOR qemu-aarch64-static -L ${CMAKE_SYSROOT})
>```

Посмотреть, какие компоненты доступны для библиотеки можно командой:

```shell
./vcpkg search boost
```

Пример вывода (частичный):

```output
asio[coroutine]                           Boost.Coroutine (optional) if you use spawn() to launch coroutines
asio[regex]                               Boost.Regex (optional) if you use any of the read_until() or async_read_un...
async-mqtt               9.0.1            Header-only Asynchronous MQTT communication library for C++17 based on Boo...
async-mqtt[tls]                           Enable TLS support
autobahn                 20.8.1#2         WAMP for C++ in Boost/Asio
azmq                     2023-03-23       Boost Asio style bindings for ZeroMQ
beast                    0#2              HTTP/1 and WebSocket, header-only using Boost.Asio and C++11
bext-wintls              0.9.8            Native Windows TLS stream wrapper for use with boost::asio
boost                    1.85.0#1         Peer-reviewed portable C++ source libraries
boost[mpi]                                Build with MPI support
boost-accumulators       1.85.0#1         Boost accumulators module
boost-algorithm          1.85.0#1         Boost algorithm module
boost-align              1.85.0#1         Boost align module
boost-any                1.85.0#1         Boost any module
boost-array              1.85.0#1         Boost array module
boost-asio               1.85.0#2         Boost asio module
boost-asio[ssl]                           Build with SSL support
boost-assert             1.85.0#1         Boost assert module
boost-assign             1.85.0#1         Boost assign module
boost-atomic             1.85.0#1         Boost atomic module
boost-beast              1.85.0#2         Boost beast module
```

Чтобы исключить загрузку лишних компонентов можно явно указать только необходимые нам библиотеки. Например:

```shell
./vcpkg install boost-core:x64-linux
./vcpkg install boost-filesystem:x64-linux
./vcpkg install boost-system:x64-linux
./vcpkg install boost-program-options:x64-linux
./vcpkg install boost-thread:x64-linux
./vcpkg install c-ares:x64-linux
./vcpkg install protobuf:x64-linux
./vcpkg install grpc:x64-linux
./vcpkg install spdlog:x64-linux
./vcpkg install gtest:x64-linux
```

Важно заметить, что если в имени заголовочного файла библиотеки используется символ подчеркивания (program_option), в имени пакета загружаемого посредством vcpkg следует использовать минус/тире (program-option).

## Сборка проекта с использованием манифеста пакетов (vcpkg.json)

Сборка проекта посредством cmake возможна и без явной загрузки зависимостей (отдельными командами `vcpkg install`). Для этого в папке с файлом сборки "CMakeLists.txt" должен быть указан файл со списком необходимых библиотек. Файл называется "vcpkg.json". Пример файла:

```json
{
  "name": "adm-demo",
  "version": "0.0.1",
  "dependencies": [
    "spdlog",
    "boost-program-options",
    "boost-locale",
    "boost-filesystem",
    "boost-uuid",
    "boost-beast",
    "boost-json",
    "boost-signals2",
    "boost-scope-exit",
    "gtest",
    "protobuf",
    "grpc",
    {
      "name": "grpc",
      "features": [ "codegen" ]
    },
    "ms-gsl",
    "nlohmann-json",
    "rapidjson"
  ],
  "builtin-baseline": "c43dbaeca0f96d90da29a151eb2ee14cad5f0aeb",
  "overrides": [
    {
      "name": "spdlog",
      "version": "1.11.0"
    },
    {
      "name": "fmt",
      "version": "9.1.0"
    },
    {
      "name": "protobuf",
      "version": "3.21.12"
    },
    {
      "name": "grpc",
      "version": "1.51.1"
    }
  ]
}
```

Ключевым параметром в файле является "builtin-baseline" - он определяет базовую ревизию vcpkg, в которой находятся скрипты сборки зависимостей, у которых явно не указана версия библиотеки. Т.е. в приведённом выше примере, скрипты сборки библиотек boost, gtest, ms-gsl, nlohmann-json и rapidjson будут взяты из ревизии "bae8f8c7...", а скрипты сборки библиотек spdlog, fmt, protobuf и grpc будут взяты из других ревизий vcpkg, которые будут найдены по указанным номерам версий библиотек.

Для интеграции **vcpkg** с **CMake** используется механизм использования Toolchain-ов CMake. Обычно этот механизм используется для того, чтобы указать список кросс-компиляторов и используемых библиотек целевой платформы. В случае интеграции с vcpkg, таким toolchain-ом является непосредственно сам vcpkg, через который подключаются необходимые кросс-компиляторы и библиотеки, используя указанный триплет.

Сгенерировать скрипты сборки можно следующей командой:

```shell
cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/developer/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux
```

**Triplet** объединяет окружение сборки (процессор, операционная система, компилятор, runtime, и т.д) в одно простое и удобное имя. В приведённом выше примере используется Triplet `x64-linux`, что означает - следует использовать библиотеку для Linux, для процессоров x86-64.

Для формирования зависимостей используется понятие **port**, которое описывает способ сборки пакета. В папке ports хранятся скрипты CMake и json-файлы с описанием зависимостей библиотек.

В случае, если собираемое приложение не поддерживает режим манифеста (пример манифеста приведён выше), может потребоваться использовать специализированную ревизию vcpkg, в которой настроено использование конкретных версий пакетов. Для переключения репозитария vcpkg на конкретную ревизию, может быть использована команда checkout (является устаревшей):

```shell
git checkout [хэш-код коммита]
```

Либо команда switch:

```shell
git switch [имя ветки]
```

## Примеры популярных пакетов

- c-ares - A C library for asynchronous DNS requests
- protobuf - Protocol Buffers - Google's data interchange format
- grpc - An RPC library and framework
- spdlog - Very fast, header only, C++ logging library
- gtest - GoogleTest and GoogleMock testing frameworks

Установить их из vcpkg можно командой:

```shell
./vcpkg install c-ares protobuf grpc spdlog gtest
```

Для успешной сборки библиотек может потребоваться доустановить дополнительные пакеты, например:

```shell
sudo apt-get install pkg-config
```

К сожалению, [vcpkg не позволяет устанавливать](https://github.com/microsoft/vcpkg/issues/21357) специфическую версию библиотеки в _classic mode_. Vcpkg позволяет указать специфическую версию пакета только в _"manifest" mode_.
