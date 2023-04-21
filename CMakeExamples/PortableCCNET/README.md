# Сборка проекта

Для сборки проекта необходимо установить [CMake](https://cmake.org/) и библиотеки [Boost](https://www.boost.org/).

Сборка проекта осуществляется из главного подкаталога командами:

``` shell
mkdir build
cd build
cmake ..
cmake --build .
```

## Сборка под Windows 7 (устаревший раздел)

Для сборки проекта под Windows 7 перед началом сборки проекта нужно настроить окружение сборки. Для этого нужно запустить командный файл, созданный Visual Studio при установке этой IDE:

``` shell
%comspec% /k "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
```
