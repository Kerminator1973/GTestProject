#include <iostream> // cout
#include <boost/bind/bind.hpp>
#include "CommportBoost.h"


CCommPortBoost::CCommPortBoost(const std::string &strPortName, const int msTimeout)
    : m_port(m_ioService)
    , m_msTimeout(msTimeout)
    , m_strPortName(strPortName)
{
    // Устанавливаем максимальный размер буфера чтения в 4096
    m_inputData.resize(4096);
}

CCommPortBoost::~CCommPortBoost()
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

std::vector<unsigned char> CCommPortBoost::GetResult()
{
    std::vector<uint8_t> result;
    copy(m_inputData.begin(), m_inputData.begin() + m_nReceived, 
        back_inserter(result));
    return result;
}

CCommPortBoost::eResult CCommPortBoost::Execute(const std::vector<uint8_t> &command)
{
    // Если коммуникационный порт не открыт, то пытаемся его открыть
    if (!m_port.is_open())
    {
        try {
            m_port.open(m_strPortName);

            // Настраиваем параметры подключения
            m_port.set_option(boost::asio::serial_port_base::baud_rate(921600));
            m_port.set_option(boost::asio::serial_port_base::character_size(8));
            m_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
            m_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
            m_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        }
        catch(boost::system::system_error&) {
            return CONNECT_ERR;
        }
    }

    // Очищаем счётчик полученных данных
    m_nReceived = 0;

    // Помещаем асинхронную команду в буфер для передачи в прибор BVS
    m_port.async_write_some(
        boost::asio::buffer(command.data(), command.size()),
        boost::bind(&CCommPortBoost::_writeHandler, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

    // Поскольку мы запустили асинхронную задачу, для контроля её выполнения мы
    // можем запустить соответствующий контролирующий сервис (boost::asio::io_service)
    m_ioService.restart();
    std::chrono::milliseconds ms{ m_msTimeout };
    auto nHandlerCount = m_ioService.run_for(ms);
    
    // TODO: переделать! Сёма предположил, что можно ориентироваться на количество 
    // callback-вызовов. В действительности нужно явным образом обрабатывать результат
    // на каждом этапе выполнения всей операции
    if (nHandlerCount < 2) {
        return ERR;
    }

    return OK;
}

void CCommPortBoost::_writeHandler(const boost::system::error_code &err, std::size_t writeBytes)
{
    if (!err)
    {
        if (writeBytes > 0)
        {
            // Запускаем следующую асинхронную задачу - чтение данных из буфера
            m_port.async_read_some(
                boost::asio::buffer(m_inputData, m_inputData.size()),
                boost::bind(&CCommPortBoost::_readHandler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

        }
    }
    else
    {
        // Something went wrong
        std::cout << err.message() << "\n";
    }
}

void CCommPortBoost::_readHandler(const boost::system::error_code &err, std::size_t readBytes)
{
    // TODO: здесь нужно обработать код ошибки и проверить, а являются ли полученные данные полными
    if (!err)
    {
        m_nReceived = readBytes;
    }
    else
    {
        // Something went wrong
        std::cout << err.message() << "\n";
    }
}
