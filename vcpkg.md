# vcpkg для установки библиотек

[vcpkg](https://vcpkg.io/en/) - свободно распространяемый менеджер проектов для приложений на C++. Развитием vcpkg занимаетс Microsoft C++ Team и сообщество разработчиков приложений. Более 2200 библиотек доступно через данных репозитарий. Для установки библиотек используются скрипты проекта и CMake. Решение работает под Windows, Linux и macOS.

Предварительные условия работы с vcpkg - установка клиента git, CMake и средств компиляции:

```shell
sudo apt install git
```

```shell
sudo apt update && sudo apt install build-essential
```

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

Посмотреть, какие компоненты доступны для библиотеки можно командой:

```shell
./vcpkg search boost
```

Пример вывода:

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
boost-bimap              1.85.0#2         Boost bimap module
boost-bind               1.85.0#1         Boost bind module
boost-build              1.85.0#2         Boost.Build
boost-callable-traits    1.85.0#1         Boost callable_traits module
boost-charconv           1.85.0#1         Boost charconv module
boost-chrono             1.85.0#1         Boost chrono module
boost-circular-buffer    1.85.0#1         Boost circular_buffer module
boost-cmake              1.85.0#2         Boost CMake support infrastructure
boost-cobalt             1.85.0#2         Boost cobalt module
boost-compat             1.85.0#2         Boost compat module
boost-compatibility      1.85.0#1         Boost compatibility module
boost-compute            1.85.0#1         Boost compute module
boost-concept-check      1.85.0#1         Boost concept_check module
boost-config             1.85.0#1         Boost config module
boost-container          1.85.0#1         Boost container module
boost-container-hash     1.85.0#1         Boost container_hash module
boost-context            1.85.0#1         Boost context module
boost-contract           1.85.0#1         Boost contract module
boost-conversion         1.85.0#1         Boost conversion module
boost-convert            1.85.0#1         Boost convert module
boost-core               1.85.0#2         Boost core module
boost-coroutine          1.85.0#1         Boost coroutine module
boost-coroutine2         1.85.0#1         Boost coroutine2 module
boost-crc                1.85.0#1         Boost crc module
boost-date-time          1.85.0#1         Boost date_time module
boost-describe           1.85.0#2         Boost describe module
boost-detail             1.85.0#1         Boost detail module
boost-dll                1.85.0#1         Boost dll module
boost-dynamic-bitset     1.85.0#1         Boost dynamic_bitset module
boost-endian             1.85.0#1         Boost endian module
boost-exception          1.85.0#1         Boost exception module
boost-fiber              1.85.0#1         Boost fiber module
boost-fiber[numa]                         Enable NUMA support
boost-filesystem         1.85.0#2         Boost filesystem module
boost-flyweight          1.85.0#1         Boost flyweight module
boost-foreach            1.85.0#1         Boost foreach module
boost-format             1.85.0#1         Boost format module
boost-function           1.85.0#1         Boost function module
boost-function-types     1.85.0#1         Boost function_types module
boost-functional         1.85.0#1         Boost functional module
boost-fusion             1.85.0#1         Boost fusion module
boost-geometry           1.85.0#2         Boost geometry module
boost-gil                1.85.0#1         Boost gil module
boost-graph              1.85.0#1         Boost graph module
boost-graph-parallel     1.85.0#1         Boost graph_parallel module
boost-hana               1.85.0#1         Boost hana module
boost-headers            1.85.0#1         Boost headers module
boost-heap               1.85.0#1         Boost heap module
boost-histogram          1.85.0#1         Boost histogram module
boost-hof                1.85.0#1         Boost hof module
boost-icl                1.85.0#1         Boost icl module
boost-integer            1.85.0#1         Boost integer module
boost-interprocess       1.85.0#1         Boost interprocess module
boost-interval           1.85.0#1         Boost interval module
boost-intrusive          1.85.0#2         Boost intrusive module
boost-io                 1.85.0#1         Boost io module
boost-iostreams          1.85.0#1         Boost iostreams module
boost-iostreams[bzip2]                    Support bzip2 filters
boost-iostreams[lzma]                     Support LZMA/xz filters
boost-iostreams[zlib]                     Support zlib filters
boost-iostreams[zstd]                     Support zstd filters
boost-iterator           1.85.0#1         Boost iterator module
boost-json               1.85.0#2         Boost json module
boost-lambda             1.85.0#1         Boost lambda module
boost-lambda2            1.85.0#1         Boost lambda2 module
boost-leaf               1.85.0#1         Boost leaf module
boost-lexical-cast       1.85.0#1         Boost lexical_cast module
boost-local-function     1.85.0#1         Boost local_function module
boost-locale             1.85.0#2         Boost locale module
boost-locale[icu]                         ICU backend for Boost.Locale
boost-lockfree           1.85.0#1         Boost lockfree module
boost-log                1.85.0#2         Boost log module
boost-logic              1.85.0#1         Boost logic module
boost-math               1.85.0#2         Boost math module
boost-math[legacy]                        Build the legacy C99 and TR1 libraries
boost-metaparse          1.85.0#1         Boost metaparse module
boost-move               1.85.0#1         Boost move module
boost-mp11               1.85.0#1         Boost mp11 module
boost-mpi                1.85.0#2         Boost mpi module
boost-mpi[python3]                        Build Python3 bindings
boost-mpl                1.85.0#1         Boost mpl module
boost-msm                1.85.0#2         Boost msm module
boost-multi-array        1.85.0#1         Boost multi_array module
boost-multi-index        1.85.0#2         Boost multi_index module
boost-multiprecision     1.85.0#2         Boost multiprecision module
boost-mysql              1.85.0#2         Boost mysql module
boost-nowide             1.85.0#2         Boost nowide module
boost-numeric-conversion 1.85.0#1         Boost numeric_conversion module
boost-odeint             1.85.0#1         Boost odeint module
boost-odeint[mpi]                         Support parallelization with MPI
boost-optional           1.85.0#1         Boost optional module
boost-outcome            1.85.0#2         Boost outcome module
boost-parameter          1.85.0#1         Boost parameter module
boost-parameter-python   1.85.0#1         Boost parameter_python module
boost-pfr                1.85.0#2         Boost pfr module
boost-phoenix            1.85.0#1         Boost phoenix module
boost-poly-collection    1.85.0#1         Boost poly_collection module
boost-polygon            1.85.0#1         Boost polygon module
boost-pool               1.85.0#1         Boost pool module
boost-predef             1.85.0#1         Boost predef module
boost-preprocessor       1.85.0#1         Boost preprocessor module
boost-process            1.85.0#2         Boost process module
boost-program-options    1.85.0#2         Boost program_options module
boost-property-map       1.85.0#1         Boost property_map module
boost-property-map-parallel 1.85.0#1      Boost property_map_parallel module
boost-property-tree      1.85.0#1         Boost property_tree module
boost-proto              1.85.0#1         Boost proto module
boost-ptr-container      1.85.0#1         Boost ptr_container module
boost-python             1.85.0#2         Boost python module
boost-qvm                1.85.0#1         Boost qvm module
boost-random             1.85.0#1         Boost random module
boost-range              1.85.0#1         Boost range module
boost-ratio              1.85.0#1         Boost ratio module
boost-rational           1.85.0#1         Boost rational module
boost-redis              1.85.0#2         Boost redis module
boost-regex              1.85.0#1         Boost regex module
boost-regex[icu]                          ICU backend for Boost.Regex
boost-safe-numerics      1.85.0#1         Boost safe_numerics module
boost-scope              1.85.0#1         Boost scope module
boost-scope-exit         1.85.0#1         Boost scope_exit module
boost-serialization      1.85.0#1         Boost serialization module
boost-signals2           1.85.0#1         Boost signals2 module
boost-smart-ptr          1.85.0#1         Boost smart_ptr module
boost-sort               1.85.0#1         Boost sort module
boost-spirit             1.85.0#1         Boost spirit module
boost-stacktrace         1.85.0#4         Boost stacktrace module
boost-stacktrace[backtrace]               Use boost_stacktrace_backtrace
boost-stacktrace[windbg]                  Use boost_stacktrace_windbg
boost-statechart         1.85.0#1         Boost statechart module
boost-static-assert      1.85.0#1         Boost static_assert module
boost-static-string      1.85.0#1         Boost static_string module
boost-stl-interfaces     1.85.0#1         Boost stl_interfaces module
boost-system             1.85.0#1         Boost system module
boost-test               1.85.0#1         Boost test module
boost-thread             1.85.0#1         Boost thread module
boost-throw-exception    1.85.0#1         Boost throw_exception module
boost-timer              1.85.0#2         Boost timer module
boost-tokenizer          1.85.0#1         Boost tokenizer module
boost-tti                1.85.0#1         Boost tti module
boost-tuple              1.85.0#1         Boost tuple module
boost-type-erasure       1.85.0#1         Boost type_erasure module
boost-type-index         1.85.0#1         Boost type_index module
boost-type-traits        1.85.0#1         Boost type_traits module
boost-typeof             1.85.0#1         Boost typeof module
boost-ublas              1.85.0#1         Boost ublas module
boost-uninstall          1.85.0#1         Internal vcpkg port used to uninstall Boost
boost-units              1.85.0#1         Boost units module
boost-unordered          1.85.0#2         Boost unordered module
boost-url                1.85.0#1         Boost url module
boost-utility            1.85.0#1         Boost utility module
boost-uuid               1.85.0#1         Boost uuid module
boost-variant            1.85.0#1         Boost variant module
boost-variant2           1.85.0#2         Boost variant2 module
boost-vmd                1.85.0#1         Boost vmd module
boost-wave               1.85.0#2         Boost wave module
boost-winapi             1.85.0#1         Boost winapi module
boost-xpressive          1.85.0#1         Boost xpressive module
boost-yap                1.85.0#1         Boost yap module
highfive[boost]                           Enables Boost support
influxdb-cxx[boost]                       Enables UDP and Unix sockets as Transport Layer
json-spirit              4.1.0#5          A C++ JSON parser/generator implemented with Boost Spirit
leaf                     0.2.2#2          Deprecated boost-leaf port.
libodb-boost             2.4.0#7          Description: Boost support for the ODB ORM library
lightgbm                 4.4.0#1          A fast, distributed, high performance gradient boosting (GBT, GBDT, GBRT, ...
lightgbm[gpu]                             GPU support using Boost.Compute
lightgbm[openmp]                          Support for multi-threading using OpenMP
magic-get                2019-09-02#3     Deprecated boost-pfr port.
mailio                   0.23.0           mailio is a cross platform C++ library for MIME format and SMTP, POP3 and ...
mongo-cxx-driver[boost]                   Enables the Boost C++17 polyfill for bsoncxx.
mqtt-cpp                 13.2.1           Header-only MQTT client/server for C++14 based on Boost.Asio.
msgpack[boost]                            Build msgpack using Boost
nghttp2-asio             2022-08-11#1     High level abstraction API to build HTTP/2 applications with nghttp2 and b...
nowide                   11.3.0           Boost nowide module (standalone)
numcpp[boost]                             Enable use boost
outcome                  2.2.9            Provides very lightweight outcome<T> and result<T> (non-Boost edition)
outcome[polyfill-cxx17]                   Polyfill C++17 entities
outcome[polyfill-cxx20]                   Polyfill C++20 entities
outcome[run-tests]                        Build and run the dependency validation tests
restc-cpp[boost-log]                      Use boost-log for logging.
soci[boost]                               Integration with Boost
spirit-po                1.1.2#4          A header-only C++ library for localization using GNU gettext po files, bas...
stlab[cpp17shims]                         Support C++11/14 by polyfilling stlab's use of `optional` and `variant` wi...
strtk[boost]                              Request boost libraries
threadpool               0.2.5#3          threadpool is a cross-platform C++ thread pool library. It provides a conv...
triton[boost]                             Use Boost as multiprecision library
trompeloeil              47               A thread-safe header-only mocking framework for C++11/14 using the Boost S...
vcpkg-boost              2024-05-15       
vit-vit-ctpl[lockfree]                    Depends on Boost Lockfree Queue library
```

Чтобы исключить загрузку лишних компонентов можно указать конкретный набор необходимых нам библиотек. Пример:

```shell
./vcpkg install boost[core,filesystem,system,program_options,thread]:x64-linux
```

> Хм. Столкнулся на практике, что vcpkg не позволяет использовать нижнее подчёркивание у "program_options". Вероятно, его нужно заменить на "program-options". По факту, попробовал выполнить следующую команду:
>
>```shell
>./vcpkg install boost-core boost-filesystem boost-system boost-program-options boost-thread:x64-linux
>```
>
> Получил ошибку сборки program_options. Если убрать program_options, то ПО устанавливается:
>
>```shell
> ./vcpkg install boost-core boost-filesystem boost-system boost-thread:x64-linux
>```
>
> Доставить program_options можно командой:
>
>```shell
> ./vcpkg install boost-program-options:x64-linux
>```

## Сборка проекта с использованием пакетов vcpkg

Сборка проекта посредством cmake возможна и без явной загрузки зависимостей. Для этого в папке с файлом сборке "CMakeLists.txt" должен быть указан файл со списком необходимых библиотек. Файл называется "vcpkg.json". Пример файла:

```json
{
  "name": "atms-ihd",
  "version": "0.0.1",
  "dependencies": [
    "spdlog",
    "boost",
    "gtest",
    "protobuf",
    "grpc",
    "ms-gsl",
    "nlohmann-json",
    "rapidjson"
  ],
  "builtin-baseline": "bae8f8c7d837c631ca72daec4b14e243824135a5",
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

Сгенерировать скрипты сборки можно следующей командой:

```shell
cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/developer/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux
```

В этой команде нужно необходимо явно указать скрипт загрузки зависимостей посредством **vcpkg**.

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
