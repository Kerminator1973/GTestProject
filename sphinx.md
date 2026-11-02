# Установка Sphinx в уже существующий репозитарий

Предположим, что в репозитарии, размещённом на локальном диске, есть файлы из которых можно собрать HTML-документацию. Например, проект документации может находится в папке `d:\Sources\FS365\docs\HTML_consumer_ProATM`. Следует обратить внимание, что в этой папке находится файл `requirements_sphinx.txt`, в котором хранятся зависимости Sphinx.

Пример файла:

```txt
alabaster==1.0.0
babel==2.18.0
certifi==2026.6.17
charset_normalizer==3.4.7
colorama==0.4.6
docutils==0.22.4
idna==3.18
imagesize==2.0.0
jinja2==3.1.6
markupsafe==3.0.3
packaging==26.2
pygments==2.20.0
requests==2.34.2
roman_numerals==4.1.0
snowballstemmer==3.1.1
sphinx_rtd_theme==3.1.0
sphinx==9.1.0
sphinxcontrib_applehelp==2.0.0
sphinxcontrib_devhelp==2.0.0
sphinxcontrib_htmlhelp==2.1.0
sphinxcontrib_jquery==4.1
sphinxcontrib_jsmath==1.0.1
sphinxcontrib_qthelp==2.0.0
sphinxcontrib_serializinghtml==2.0.0
urllib3==2.7.0
```

Установить зависимости лучше в отдельное окружение, чтобы не засорять базовое окружение:

```shell
cd d:/Sources/FS365/docs/HTML_consumer_ProATM
python -m venv .venv
.venv\Scripts\activate
pip install -r requirements_sphinx.txt
```

Команда сборки документации выглядит следующим образом:

```shell
sphinx-build -b html source _build
```

Альтернативная команда: `python -m sphinx -b html source _build`

Результат команды будет сформирован в папке "_build".
