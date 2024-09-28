# Code Checker - статический анализатор кода

Базовая статья - [CodeChecker: анализируем большой проект на С++ быстро, эффективно и бесплатно](https://habr.com/ru/companies/yadro/articles/838878/) by YADRO.

Официальная документация по [продукту](https://codechecker.readthedocs.io/en/latest/).

Позволяет запускать различные анализаторы кода C/C++ под Linux и macOS: Clang-Tidy, Clang Static Analyzer, Cppcheck и GCC Static Analyzer — в любой комбинации. На официальном сайте есть ссылка на 

Code Checker - это Python-пакет.

Варианты установки:

```shell
pip3 install codechecker
```

Можно установить пакет в виртуальном окружении или воспользоваться pipx, который самостоятельно создаст venv: 

```shell
pipx install CodeChecker
```

Также возможна установка через пакетный менеджер Snap:

```shell
sudo snap install codechecker --classic
```

Для передачи исходников на анализ, потребуется создать файл "compile_commands.json". Этот файл содержит _compilation database_ проекта, который используют IDE (например: VSCode, CLion), средства разработки (например: Ninja, Bazel) и анализаторы кода (например: SonarLint).

Структура файла (JSON) состоит мз массива элементов, в каждом из которых есть:

- directory: папка из которой выполняется команда
- command: команда сборки файла
- file: имя файла

Если в проекта используется CMake, то в каталоге проекта можно запустить команду:

```shell
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Либо в папке `build`, используя чуть более длинную команду:

```shell
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

Альтернативный вариант - использовать утилиту Bear, которая работает с clang.

Я устанавливал утилиту через snap и в моём случае она называлась `codechecker`.

Команда запуска статических анализаторов:

```shell
codechecker analyze ./compile_commands.json --enable=sensitive --output ./reports
```

В приведённой выше статье команда чуть другая:

```shell
CodeChecker analyze ./compile_commands.json --analyzers clangsa --enable=alpha --enable=sensitive --output ./reports
```

В моём случае, на машине с Ubuntu 24.04 автоматически применились анализаторы clang-tidy и clangsa.

Для того, чтобы поработать с результатами, требуется сконвертировать результаты обработки в статический web-сайт:

```shell
codechecker parse --export html --output ./reports_html ./reports
```

Однако, в моём случае, вывод был абсолютно беполезным, т.к. система былол найдено несколько тысяч замечаний к The Boost Library, а не к моему коду.

Команда `--exclude */boost/*` не была принята утилитой. Запрос `codechecker analyze --help`, к сожалению, не дал четких подсказок, как исключить из анализа the Boost library.

Однако в папке экспорта лежали файлы со сложными именами, в которых было существенно меньше замечаний к коду. И вот в этих файлах были замечания непосредственно к коду моего приложения. И пару замечаний оказались очень полезными.

Чтобы исключить файлы Boost из анализа, вероятнее всего, нужно настроить конфигурационные файлы Clang-Tidy (.clang-tidy) и Clang Static Analyzer (clangsa).

Пример конфигурационного файла ".clang-tidy":

```yaml
Checks:          '-*,<your_current_checks>'
HeaderFilterRegex: ''
FormatStyle:     none

# Add exclusions here
Exclude:         exclude_dir/**/*.cpp
```

## Альтенативы

Разработка Facebook - [Infer](https://fbinfer.com/) является статическим анализатором кода не только C/C++, но и Objective-C с Java.
