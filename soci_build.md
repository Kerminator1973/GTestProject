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
