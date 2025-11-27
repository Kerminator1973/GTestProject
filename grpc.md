# Использование gRPC

gRPC применяется чаще всего как инструмент взаимодействия между микросервисами в соответсвующей архитектуре. gRPC часто противопоставляют JSON.

Недостатки JSON:

- динамическая схема, т.е. структура документа может меняться произвольным образом
- parsing документа медленный
- отсутствие типизации
- относительно большой размер данных

Protobuf, являющийся одним из уровней gRPC пытается компенсировать эти недостатки, включая строгую схему данных.

Ориентировочно, пакет protobuf в четыре раза меньше, чем JSON.

## Типы данных Protobuf

Основные типы данных Protobuf:

- double/float
- int32/int64
- uint32/uint64
- sint32/sint64 - оптимизация для преимущественно отрицательных чисел
- fixed32/fixed64 - оптимизация для больших чисел, с большим количеством нулей в конце (10 в степени N)
- bool, string, bytes
- google nullable type - особый тип, который может быть nullable

Так же при определении типа может использоваться модификатор repeated, который означает динамический массив. Для понимания: list, vector, slice.

Пример сообщения:

```proto
message InfoResponse {
    int32 id = 1;
    int32 moderator_id = 2;
    repeated int64 task_ids = 3;
}
```

Числа с правой части определения - это порядковый номер поля в сообщении.

Сообщения могут быть вложенными:

```proto
message CreateNoteRequest {
    NoteContent content = 1;
}
```

В определении сообщений могут быть использованы enum-ы:

```proto
enum ContentType {
    POST = 1;
    COMMENT = 1;
    reserved 2 to 7;
    STREAM = 8;
}
```

Хорошей практикой является определение значения с индексом 0, например: `UNKNOWN=0`.

Ключевое слово reserved позволяет обеспечивать обратную совместимость - добавлять новые поля, не разрушая совместимости с уже существующим кодом.

По структуре protobuf похож на TLV, но он чуть похитрее - в нём явно передаются тип и порядковый номер поля.

## Генерация кода

Работа начинается с декларации. Пример декларации:

```proto
syntax = "proto3";

package example;

// Пользовательская информация
message User {
  // Уникальный идентификатор пользователя
  int64 id = 1;

  // Имя пользователя
  string name = 2;

  // Электронная почта (опционально)
  string email = 3;

  // Список ролей пользователя
  repeated Role roles = 4;
}

// Возможные роли пользователя
enum Role {
  UNKNOWN = 0;
  ADMIN   = 1;
  EDITOR  = 2;
  VIEWER  = 3;
}

// Запрос на получение пользователя по ID
message GetUserRequest {
  int64 user_id = 1;
}

// Ответ с данными пользователя
message GetUserResponse {
  User user = 1;
}

// Сервис, предоставляющий операции над пользователями
service UserService {
  // Получить информацию о пользователе
  rpc GetUser (GetUserRequest) returns (GetUserResponse);
}
```

После того, как осуществлена разработка декларации осуществляется генерация кода для клиента и сервера, для конкретных языков программирования. Google поддерживает генерацию для нескольких основных языков, включая: Java, Go и C++. Существуют custom-ные генераторы для других языков программирования.

Основной модуль генерации кода protobuf называется **protoc**, но для своей работы он может требовать и другие исполняемый файлы, для генерации кода на целевых языках программирования.

Часто для генерации используют make-файл, который описывает вызов protoc (с параметрами), копирование результатов генерации в целевые папки и удаление артефактов генерации.

Хорошей структурой папок для хранения деклараций (proto) является папка "api", внутри которой описываются декларации для конкретных API, например - "user_v1".

## Чем хорош gRPC

В качестве надостатков Http обычно рассматривают:

- Для каждого нового запроса создаётся новое соединение (медленно и дорого)
- Head of Blocking (слабое распараллеливание запросов)
- Не сжимает заголовки (они могут быть большими)
- Нет встроенных механизмов server push (однако есть расширенные механизмы: WebSockets, SSE)

>Важно заметить, что в определённом смысле "картинка" Http упрощена. Так, например, в Go можно было бы использовать WebSockets, но имеющаяся реализация - устаревшая, базирующаяст на socket.io (возможно, порт с JavaScript). Т.е. в рамках эко-системы Google удобнее использовать именно gRPC, а не, например, WebSockets

gRPC решает указанные выше проблемы Http, используя "под капотом" HTTP/2.

## Как осуществляется генерация кода

В рамках gRPC различаются API клиента и сервера. Одним вызовом make, обычно, выполняется генерация и клиента, и сервера для одного языка программирования.

Утилита protoc генерирует код в двух режимах: только структуры (generate-structs) и структуры + stub-ы для кода (generate).

Чтобы protoc сгенерировал stub-ы для кода, необходимо в файле-декларации (proto) описать сервис, например:

```grpc
service UserService {
  // Получить информацию о пользователе
  rpc GetUser (GetUserRequest) returns (GetUserResponse);
}
```

По договорённостям gRPC, сервисная функция имеет один вход (структуру) и один выход (структуру).

## Postman

В актуальных версиях Postman есть поддержка gRPC. Нужно выбрать в меню "My Workspace" нажать кнопку "new" и в появившемся списке выбрать gRPC. Далее нужно указать Endpoint (например, `localhost:50501`) и Postman предложит обратиться к серверу и запросить у него список доступных функций. Выбрав функцию из полученного списка можно попросить Postman сгенерировать пример запроса. Далее можно выполнить запрос и он, с высокой вероятностью, корректно выполнится.

Удобство подхода состоит в том, что можно сначала разработать только сервер, а когда станет ясно, что он успешно разработан, то тогда уже можно будет потратить время и на клиента.

## Попытка сборки gRPC сервера на C++ под Linux (Ubuntu 24.04)

В качестве стартовой инструкции (не оптимальной), рассматривают текст сгенерированный GPT-OSS 120B.

Установка компонентов:

```shell
sudo apt-get update
sudo apt-get install -y build-essential cmake protobuf-compiler libprotobuf-dev libgrpc++-dev grpc-proto
```

В моём случае, apt-get не нашёл grpc-proto в репозитарии пакетов. Осуществляю попытку собрать plugin из исходных текстов:

```shell
# Install build prerequisites
sudo apt-get update
sudo apt-get install -y build-essential cmake git libssl-dev libprotobuf-dev protobuf-compiler

# Clone gRPC (includes submodule protobuf)
git clone --recurse-submodules -b v1.64.0 https://github.com/grpc/grpc.git
cd grpc

# Build and install
mkdir -p cmake/build && cd cmake/build
cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF ../..
make -j$(nproc)
sudo make install
```

В приведённом выше примере указывается конкретная версия продукта - v1.64.0. Скорее всего, эта сборка уже не актуальная сейчас.

Репозитарий проекта весьма большой - 864723 объекта. Требуется выполнить операцию Resolving Deltas, т.е. при сборке в Windows могут потребоваться дополнительные настройки git-клиента. Кажется разумным использовать vcpkg для сборки сложных приложений. Загрузка - десятки минут. Сборка также около десяти минут.

Проверка доступности plug-in-а:

```shell
which grpc_cpp_plugin
# → /usr/bin/grpc_cpp_plugin   (or /usr/local/bin/grpc_cpp_plugin)

grpc_cpp_plugin --version   # should print the version number
```

При попытке запросить версию plugin-а получил сообщение об ошибке:

```shell
developer@VirtPlatform:~/grpc/cmake/build$ grpc_cpp_plugin --version
grpc_cpp_plugin: Unknown option: --version
```

Декларация протокола "proto/helloworld.proto":

```proto
syntax = "proto3";

package helloworld;

// The greeting service definition.
service Greeter {
  // Sends a greeting
  rpc SayHello (HelloRequest) returns (HelloReply) {}
}

// The request message containing the user's name.
message HelloRequest {
  string name = 1;
}

// The response message containing the greeting.
message HelloReply {
  string message = 1;
}
```

Для генерации C++ кода может быть использована следующая команда:

```shell
protoc -I=proto --grpc_out=generated --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` --cpp_out=generated proto/helloworld.proto
```

Однако перед выполнением команды следует создать папку "generated" на том же уровне, что и "proto". Вызывать скрипт следует из родительской папки, в которой находятся папки "proto" и "generated".

Как результат, были созданы следующие файлы: "helloworld.grpc.pb.cc", "helloworld.grpc.pb.h", "helloworld.pb.cc" и "helloworld.pb.h". Первые два файла - это stub-ы для gRPC, а последние два - описание структур protobuf.

Далее необходимо добавить исходные тексты сервера в родительском подкаталоге. Имя: "server.cpp"

```cpp
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "generated/helloworld.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

class GreeterServiceImpl final : public Greeter::Service {
public:
  Status SayHello(ServerContext* /*context*/,
                  const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix = "Hello ";
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
};

int main(int argc, char** argv) {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
  return 0;
}
```

И "CMakeLists.txt" - скрипт сборки для CMake:

```cmake
cmake_minimum_required(VERSION 3.14)
project(gRPCServerExample LANGUAGES CXX)

# Find gRPC and Protobuf packages
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Proto files
set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

file(MAKE_DIRECTORY ${GENERATED_DIR})

# Generate C++ code from .proto
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/helloworld.proto)
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_DIR}/helloworld.proto)

# Move generated files to a known location
set(GENERATED_SRCS ${PROTO_SRCS} ${GRPC_SRCS})
set(GENERATED_HDRS ${PROTO_HDRS} ${GRPC_HDRS})

# Server target
add_executable(server server.cpp ${GENERATED_SRCS})
target_include_directories(server PRIVATE ${GENERATED_DIR})
target_link_libraries(server PRIVATE gRPC::grpc++ protobuf::libprotobuf)

# Optional: set C++17
target_compile_features(server PRIVATE cxx_std_17)
```

Генерация скрипта сборки осуществляется командами:

```shell
mkdir -p build && cd build
cmake ..
```

При генерации скрипта сборки, CMake выдал ошибку:

```cpp
CMake Error at CMakeLists.txt:15 (protobuf_generate_cpp):
  Unknown CMake command "protobuf_generate_cpp".
```

В действительности, эти действия уже были выполнены мною вручную ранее. Следующие команды можно закомментировать:

```cmake
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/helloworld.proto)
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_DIR}/helloworld.proto)
```

Эти команды могут быть важны для файла "CMakeLists.txt", который осуществляет полную, чистую сборку из исходных текстов.

Если скрипты сборки были сгенерированы корректно, то можно попробовать собрать проект:

```shell
cmake --build .
```

В моё случае, исходники скомпилировали, но проект не собрался, т.к. не были найдены подключаемые файлы с исходниками stub-ов gRPC и структурами protobuf. Проблема вот в этих строчках:

```cmake
set(GENERATED_SRCS ${PROTO_SRCS} ${GRPC_SRCS})
set(GENERATED_HDRS ${PROTO_HDRS} ${GRPC_HDRS})
```

Самое простое решение - скопировать файлы в ту же папку, где находится файл "server.cpp" и явным образом указать их в списке собираемых:

```cmake
# Server target
add_executable(server 
	server.cpp
	helloworld.grpc.pb.cc 
	helloworld.pb.cc
	helloworld.grpc.pb.h 
	helloworld.pb.h
)
```

В таком варианте приложение успешно компилируется и запускается. По умолчанию, сервер слушает порт 50051.

К сожалению, подключиться через Postman на автомате не получилось - Postman сообщил, что "_Details: 12 UNIMPLEMENTED:_". Однако можно вручную скопировать схему (файл с расширением ".proto") и указать его Postman. Далее, в строке следующей за адресом и портом, в которой есть выпадающий список, можно выбрать имя отправляемого сообщения и вызвать удалённую процедуру, используя кнопку "Invoke".

В моём случае, удалось успешно вызвать функцию сервиса из Postman (основная машина на Microsoft Windows), gRPC Server запущенный на виртуальной машине Ubuntu 24.04.
