# Сборка приложения с использованием библиотеки Soci

Библиотека [SOCI](https://github.com/SOCI/soci) предоставляет возможность добавить в приложения на C++ возможность доступа к различным СУБД. Поддерживаются как Native-интерфейсы, так и ODBC.

В данной инструкции описыается сборка динамической библиотеки SOCI (в Debug-режиме) для разработки и отладки приложения для ОС Microsoft Windows. В качестве целевой СУБД рассматривается Postgres.

## Предварительные условия

Для возможности использования Native-адаптера Postgres, необходимо указать путь к заголовочным файлам и библиотекам Postgres. Самый простой способ сделать это - установить Postgres, перейдя [со страницы загрузки официального сайта](https://www.postgresql.org/download/windows/) на партнёрский сайт EDB.

При установке Postgres, следует иметь в виду, что **Stack Builder** - это вспомогательный инструмент для установки расширений Postgres. Для загрузки заголовочных файлов и библиотеки в нём нет необходимости. Это опциональный инструмент.

Контрольная процедура: [установить pgAdmin 4](https://www.pgadmin.org/download/pgadmin-4-windows/) и подключиться к локальной базе данных (localhost) с указанными при установке Postgres логином и паролем.

## Сборка библиотеки

Загрузить исходные тексты библиотеки SOCI можно из [репозитария GitHub](https://github.com/SOCI/soci).

[Инструкция по сборке](https://soci.sourceforge.net/doc/master/installation/) содержит описание ключевых параметров запуска CMake для включения Native-адаптера Postgres.

Папки с заголовочными файлами и библиотеками на типовой Windows-машине вот такие:

- "c:\Program Files\PostgreSQL\16\include"
- "c:\Program Files\PostgreSQL\16\lib"

Для сборки Debug-версии библиотеки предлагается использовать следующие команды:

```shell
mkdir Debug
cd Debug
cmake  -DWITH_POSTGRESQL=ON -DPOSTGRESQL_INCLUDE_DIR="c:/Program Files/PostgreSQL/16/include" -DPOSTGRESQL_LIBRARY="c:/Program Files/PostgreSQL/16/lib/libpq.lib" ..
```

Указать тип сборки можно специализированной командой: `-DCMAKE_BUILD_TYPE=RELEASE`

Если не планируется использование библиотеки Boost, то следует добавить ключ генерации скриптов: `-DWITH_BOOST=OFF`

Если в логах есть следующее сообщение, это означает, что сборка для Postgres была неуспешной:

```output
-- PostgreSQL:
-- WARNING: PostgreSQL libraries not found, some features will be disabled.
```

Если генерация скрипта завершилась успешно, то в логах генерации должен быть следующий текст:

```output
-- PostgreSQL:
-- POSTGRESQL_INCLUDE_DIRS                  = C:/Program Files/PostgreSQL/16/include
-- POSTGRESQL_LIBRARIES                     = C:/Program Files/PostgreSQL/16/lib/libpq.lib
-- POSTGRESQL_VERSION                       = 16.3
```

В процессе генерации скрипта сборки будет создан файл, который следует включить в проект приложения, работающего с Postgres: `.\Debug\include\soci\soci-config.h`. Этот файл содержит директивы препроцессора, которые указывают на доступные адаптеры библиотеки. 

Для контроля: в файле `soci-config.h` должна быть строка:

```cpp
// POSTGRESQL backend
#define SOCI_HAVE_POSTGRESQL
```

Непосредственная сборка приложения может быть выполнена командой:

```shell
cmake --build . --config Debug
```

Результирующие артефакты сборки:

- Динамические библиотеки: `\soci\Debug\bin\Debug`
- Статические библиотеки: `\soci\Debug\lib\Debug`
- Заголовочные файлы: `\soci\Debug\include` и `\soci\include`

### Проверочные действия

В папке `\soci\Debug\bin\Debug` должны находится исполняемые файлы (dll и exe).

Для проверки работоспособности собранных библиотек следует выполнить тестовый файл: `soci_postgresql_test.exe`

В случае, если не хватает некоторых динамических библиотек, на экран будет выведено системное сообщение.

По факту, необходимо скопировать в папку шесть файлов из состава PostgreSQL, либо следует указать папке с lib-файлами PostgreSQL в качестве папки для поиска dll-файлов по умолчанию. Для PostgresSQL 16, в числе этих файлов должны быть: libpq.dll, libcrypto-3-x64.dll, libssl-3-x64.dll, libiconv-2.dll, libintl-9.dll.

Если какие-то файлы будут отсутствовать, то в консоль будет выведено сообщение:

```output
Error: Failed to find shared library for backend postgresql
```

Проверить путь размещения файлов, можно есть начать отладку кода библиотеки. Нужно зайти в конструктор объекта session, затем в `lastConnectParameters_(backendName, connectString)`, далее в `factory_(&dynamic_backends::get(backendName))`, потом в `do_register_backend(name, std::string());`. Нас интересует код:

```cpp
std::string const fullFileName(search_paths_[i] + "/" + LIBNAME(name));
h = DLOPEN(fullFileName.c_str());
```

В переменной fullFileName будет сохранено имя динамической библиотеки, которую планируется загрузить и использовать в качестве провайдера для доступа к данным в СУБД.

### Состав сборки

По умолчанию, скрипт сборки собирает и динамические и статические библиотеки.

Библиотеки со статической линковкой: libsoci_core_4_1.lib, libsoci_empty_4_1.lib, libsoci_odbc_4_1.lib, libsoci_postgresql_4_1.lib.

Библиотеки с динамической линковкой (lib): soci_core_4_1.lib, soci_empty_4_1.lib, soci_odbc_4_1.lib, soci_postgresql_4_1.lib

Реализация динамических библиотек (dll) находятся в папке "bin/Debug": soci_core_4_1.dll, soci_empty_4_1.dll, soci_odbc_4_1.dll, soci_postgresql_4_1.dll

## Сборка примера кода

Базовый пример приложения размещён [здесь](https://soci.sourceforge.net/doc/release/3.1/)

Мой вариант приложения:

```cpp
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <exception>
#include "soci/soci.h"

using namespace soci;
using namespace std;

int main()
{
    try
    {
        session sql("postgresql", "dbname=postgres user=postgres password=38Gjgeuftd");

        int count;
        sql << "select count(*) from phonebook", into(count);

        cout << "We have " << count << " entries in the phonebook.\n";
    }
    catch (exception const& e)
    {
        cerr << "Error: " << e.what() << '\n';
    }
}
```

В базе данных "postgres" была создана таблица "phonebook", в которую были добавлены две записи. Как результат, приложение вывело сообщение:

```output
We have 2 entries in the phonebook.
```

## TODO - что ещё имеет смысл сделать

- Создать консольное приложение с использованием CMake
- Описать процедуру сборки в Linux
- Расписать оптимизацию сборки (что можно отключить)
