# Советы по организации кода

Передача внешнего объёкта:

```cpp
void A(Resource*);
void B(Resource&);
void C(std::unique_ptr<Resource>);
void D(std::unique_ptr<Resource>&);
void E(std::shared_ptr<Resource>);
void F(std::shared_ptr<Resource>&);
```

Общие рекомендации: 

- если не может быть нулевым, то передавать по ссылке
- если может быть нулевым, передавать по значению
- если нужно передать владение - то `std::unique_ptr<>`
- если может быть несколько ссылок на объект, то `std::shared_ptr<>`
- `std::shared_ptr<>` более дорогой, т.к. работает со счётчиком копий
