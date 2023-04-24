#pragma once
#include <boost/asio.hpp>
#include <vector>

class CCommPortBoost
{
public:
    // TODO: выполнить рефакторинг комментариев
    enum eResult
    {
        OK,           //Удачно
        CONNECT_ERR,  //Ошибка при подключении к порту
        WRITE_ERR,    //Ошибка при записи данных
        READ_ERR,     //Ошибка при чтении данных
        TIMEOUT_ERR,  //Время вышло
        ERR,          //Общая ошибка
    };

    CCommPortBoost(const std::string &strPortName, const int msTimeout = 2000);
    ~CCommPortBoost();

    eResult Execute(const std::vector<uint8_t> &command);

    // TODO: возможно, следует использовать std::range
    std::vector<uint8_t> GetResult();

private:
    // Обработчики результатов выполненной асинхронной команды
    void _writeHandler(const boost::system::error_code& err, std::size_t writeBytes);
    void _readHandler(const boost::system::error_code& err, std::size_t readBytes);

    // Сервис, управляющий выполнением асинхронных зададач
    boost::asio::io_service m_ioService;

    // Коммуникационный порт, через который осуществляется взаимодействие с прибором BVS
    boost::asio::serial_port m_port;

    // Тайм-аут на получение данных из порта
    int m_msTimeout;

    // Имя порта, к которому подключен прибор BVS. Используется для при отложенной инициализации
    std::string m_strPortName;

    // Буфер чтения данных из прибора BVS
    std::vector<uint8_t> m_inputData;

    // Количесто фактически полученных данных от прибора
    std::size_t m_nReceived = 0;
};
