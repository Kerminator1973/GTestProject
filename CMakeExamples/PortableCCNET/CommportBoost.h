#pragma once
#include <boost/asio.hpp>
#include <vector>

class CCommPortBoost
{
public:
    enum eResult
    {
        OK,             // Успешное завершение операции
        CONNECT_ERR,    // Не удалось подключиться к прибору
        WRITE_ERR,      // Ошибка при записи данных в порт
        READ_ERR        // Ошибка при чтении данных из порта
    };

    CCommPortBoost(const std::string &strPortName, const int msTimeout = 2000);
    ~CCommPortBoost();

    eResult Execute(const std::vector<uint8_t> &command);

    // TODO: возможно, следует использовать std::range
    std::vector<uint8_t> GetResult();

    inline std::string GetErrorMessage()
    {
        return m_errorMessage;
    }

private:
    // Обработчики результатов выполненной асинхронной команды
    void _writeHandler(const boost::system::error_code& err, std::size_t writeBytes);
    void _readHandler(const boost::system::error_code& err, std::size_t readBytes);

    // Сервис, управляющий выполнением асинхронных задач
    boost::asio::io_service m_ioService;

    // Коммуникационный порт, через который осуществляется взаимодействие с прибором BVS
    boost::asio::serial_port m_port;

    // Тайм-аут на получение данных из порта
    int m_msTimeout;

    // Имя порта, к которому подключен прибор BVS. Используется для при отложенной инициализации
    std::string m_strPortName;

    // Буфер чтения данных из прибора BVS
    std::vector<uint8_t> m_inputData;

    // Количество фактически полученных данных от прибора
    std::size_t m_nReceived = 0;

    // Результат выполнения операции
    eResult m_Result = CONNECT_ERR;

    // Текст сообщения об ошибке
    std::string m_errorMessage;
};
