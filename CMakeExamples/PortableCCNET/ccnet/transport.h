#pragma once
#include <boost/asio.hpp>
#include <vector>

class CCNetTransport
{
public:
    enum eResult
    {
        OK,             // Успешное завершение операции
        CONNECT_ERR,    // Не удалось подключиться к прибору
        WRITE_ERR,      // Ошибка при записи данных в порт
        READ_ERR,       // Ошибка при чтении данных из порта
        WRONG_CRC       // Не совпала CRC в ответе на запрос
    };

    CCNetTransport(const std::string &strPortName, const int msTimeout = 2000);
	~CCNetTransport();

    // Метод получает команду в виде массива байт. Эта команда должна содержать поля
    // CRC16 (последние два байта), но вычисляется CRC16 внутри метода Execute().
    // Примечание: передача вектора вместе с полями CRC16 обусловлена минимизацией
    // операций выделения памяти на Heap-е
    eResult Execute(std::vector<uint8_t> &command);

    // TODO: возможно, следует использовать std::range
    std::vector<uint8_t> GetResult();

    // Метод возвращает сообщение об ошибке в коде
    inline std::string GetErrorMessage()
    {
        return m_errorMessage;
    }

    // Метод вычисления CRC16 предназначен для выполнения Unit-тестирования
    static const uint16_t CalcCRC(const std::vector<uint8_t>& data);

private:
    // Обработчики результатов выполненной асинхронной команды
    void _writeHandler(const boost::system::error_code& err, std::size_t writeBytes);
    void _readHandler(const boost::system::error_code& err, std::size_t readBytes);

    // Метод разбивает data на два блока: команда и контрольная сумма (последние два 
    // байта). По байтам команды вычисляется CRC, которое затем копируется в поле CRC
    void _updateCRC(std::vector<uint8_t>& data);

    // Метод проверяет CRC полученного блока данных. Проверка CRC осуществляется
    // для данных, полученных из буфера ответа прибора
    bool _checkCRC();

    // Метод осуществляет вычисления CRC по указанному блоку данных. Последние
    // два байта не учитываются. Параметр count добавлен для того, чтобы
    // избежать избыточного копирования данных из буфера ответа данных (с фиксированным
    // размером) в вектор, содержащий только ответ прибора.
    // 
    // В С++ 20 можно было бы использовать m_inputData и readBytes для создания "обёртки"
    // над контейнером, решающей данную задачу. См.: std::span, Ranges Library.
    // Также можно посмотреть: https://github.com/tcbrindle/span
    static uint16_t _calcCRC(const std::vector<uint8_t>& data, size_t count);

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
