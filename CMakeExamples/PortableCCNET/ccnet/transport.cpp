#include <iostream> // std::cout
#include <boost/bind/bind.hpp>
#include "transport.h"


CCNetTransport::CCNetTransport(const std::string &strPortName, const int msTimeout)
    : m_port(m_ioService)
    , m_msTimeout(msTimeout)
    , m_strPortName(strPortName)
{
    // Устанавливаем максимальный размер буфера чтения в 1024. Это ограничение
    // взято из документа "Эксплуатационная документация. D210BA"
    m_inputData.resize(1024);
}

CCNetTransport::~CCNetTransport()
{
    if (m_port.is_open())
    {
        try {
            m_port.close();
        }
        catch (...)
        {
            // При закрытии порта может возникнуть исключение. Давим его
        }
    }
}

std::vector<unsigned char> CCNetTransport::GetResult()
{
    std::vector<uint8_t> result;

    if (m_Result == CCNetTransport::OK) {
        copy(m_inputData.begin(), m_inputData.begin() + m_nReceived,
            back_inserter(result));
    }

    return result;
}

CCNetTransport::eResult CCNetTransport::Execute(std::vector<uint8_t> &command)
{
    // Если коммуникационный порт не открыт, то пытаемся его открыть
    if (!m_port.is_open())
    {
        try {
            m_Result = CCNetTransport::CONNECT_ERR;
            m_errorMessage.clear();

            m_port.open(m_strPortName);

            // Настраиваем параметры подключения
            m_port.set_option(boost::asio::serial_port_base::baud_rate(921600));
            m_port.set_option(boost::asio::serial_port_base::character_size(8));
            m_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
            m_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
            m_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        }
        catch(boost::system::system_error& ex) {
            m_errorMessage = ex.what();
            return CONNECT_ERR;
        }
    }

    // Пересчиытваем CRC для отправляемой команды
    _updateCRC(command);

    // Очищаем счётчик полученных данных
    m_nReceived = 0;

    // Помещаем асинхронную команду в буфер для передачи в прибор BVS
    m_port.async_write_some(
        boost::asio::buffer(command.data(), command.size()),
        boost::bind(&CCNetTransport::_writeHandler, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

    // Поскольку мы запустили асинхронную задачу, для контроля её выполнения мы
    // можем запустить соответствующий контролирующий сервис (boost::asio::io_service)
    m_ioService.restart();
    std::chrono::milliseconds ms{ m_msTimeout };
    auto nHandlerCount = m_ioService.run_for(ms);

    return m_Result;
}

void CCNetTransport::_writeHandler(const boost::system::error_code &err, std::size_t writeBytes)
{
    if (!err && writeBytes > 0)
    {
        // Запускаем следующую асинхронную задачу - чтение данных из буфера
        m_port.async_read_some(
            boost::asio::buffer(m_inputData, m_inputData.size()),
            boost::bind(&CCNetTransport::_readHandler, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        m_Result = CCNetTransport::WRITE_ERR;
        m_errorMessage = err.message();
    }
}

void CCNetTransport::_readHandler(const boost::system::error_code &err, std::size_t readBytes)
{
    if (!err)
    {
        m_nReceived = readBytes;
        m_Result = CCNetTransport::OK;

        // Проверяем CRC полученного ответа на запрос
        if (!_checkCRC()) {
            m_Result = CCNetTransport::WRONG_CRC;
        }
    }
    else
    {
        m_Result = CCNetTransport::READ_ERR;
        m_errorMessage = err.message();
    }
}

const uint16_t CCNetTransport::CalcCRC(const std::vector<uint8_t>& data)
{
    return _calcCRC(data, data.size());
}

void CCNetTransport::_updateCRC(std::vector<uint8_t>& data)
{
    auto count = data.size();
    if (count > 2) {
        const auto crc16 = _calcCRC(data, count);
        data[count - 2] = crc16 & 0x00FF;
        data[count - 1] = (crc16 & 0xFF00) >> 8;
    }
}

bool CCNetTransport::_checkCRC()
{
    if (m_nReceived > 2) {
        const auto crc16 = _calcCRC(m_inputData, m_nReceived);
        auto loBy = uint8_t(crc16 & 0x00FF);
        auto hiBy = uint8_t((crc16 & 0xFF00) >> 8);

        if (m_inputData[m_nReceived - 2] == loBy && m_inputData[m_nReceived - 1] == hiBy) {
            return true;
        }
    }

    return false;
}

// Применяемый алгоритм вычисления контрольной суммы называется:
// CRC16-CCITT (Kermit) 0x8408 Polynomial.
// Пример кода на языке Си описан в "Эксплуатационная документация. D210BA"
uint16_t CCNetTransport::_calcCRC(const std::vector<uint8_t>& data, size_t count)
{
    uint16_t acc = 0;
    for (int i = 0; i < count - 2; i++)
    {
        acc = acc ^ data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (acc & 0x0001)
            {
                acc >>= 1;
                acc ^= 0x08408;
            }
            else acc >>= 1;
        }
    }

    return acc;
}
