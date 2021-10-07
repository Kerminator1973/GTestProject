#include "CommPort.h"
#include <errno.h>
#include <chrono>
#include <boost/asio/read.hpp>
#include <boost/thread.hpp>
#include <mutex>

std::mutex mtx;
static volatile bool g_isLowWrited = false;
static volatile bool g_isLowReaded = false;

int m_fd = -1;
unsigned char *m_Lowdata = NULL;


CCommPort::CCommPort(const std::string strPortName, bool isLow)
    : m_maxSize(512)
    , m_port(m_ioService)
    , m_lastReadDataSize(0)
    , m_readThread(CCommPort::_fnRead)
{
    if(!isLow)
    {
    ConnectToPort(strPortName,921600);
    m_data.resize(m_maxSize);
    }
    else
    {
        if(LowConnect(strPortName))
        {
            std::cout<<__FUNCTION__<<" good"<<std::endl;
        }
    }
}

CCommPort::~CCommPort()
{
    DisconnetFromPort();
    if(m_readThread.joinable())
     {
         g_isLowWrited = false;
         m_readThread.detach();
     }
}

int CCommPort::ConnectToPort(const std::string strPortName, int baud)
{
    int iResult = -1;
    if(m_port.is_open())
    {
        std::cout<<__FUNCTION__<<"(): порт уже открыт"<<std::endl;
        return iResult;
    }

    m_port.open(strPortName);
    if(m_port.is_open())
    {
        iResult = 0;

        m_port.set_option(boost::asio::serial_port_base::baud_rate(baud));
        m_port.set_option(boost::asio::serial_port_base::character_size(8));
        m_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
        m_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        m_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    }
    return iResult;
}

void CCommPort::DisconnetFromPort()
{
    if(m_port.is_open())
    {
        m_port.close();
    }
}

void CCommPort::AsyncRead()
{
   // m_Rstart = clock();
    m_port.async_read_some(
                boost::asio::buffer(m_data,m_maxSize),
                boost::bind(&CCommPort::readHandler,this,
                     boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred));


//    boost::asio::async_read(m_port,
//                            boost::asio::buffer(m_data.data(),m_maxSize),
//                            boost::bind(&CCommPort::readHandler,
//                                   this,
//                                   boost::asio::placeholders::error,
//                                   boost::asio::placeholders::bytes_transferred));
}

void CCommPort::AsyncWrite(const std::vector<unsigned char> &buf)
{
   // m_Wstart = clock();
    m_port.async_write_some(
                boost::asio::buffer(buf.data(),buf.size()),
                boost::bind(&CCommPort::writeHandler,this,
                     boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
}

int CCommPort::Run(const int millisec)
{
    m_ioService.restart();
    std::chrono::milliseconds ms{millisec};
    size_t sHendlerCount =  m_ioService.run_for(ms);
    return sHendlerCount;
}

boost::system::error_code CCommPort::Execute(const IN std::vector<unsigned char> &command, OUT std::vector<unsigned char> &data, IN int timeout)
{
   // m_data.clear();

   // AsyncWrite(command);
  //  Run(timeout);
//    data = m_data;
    AsyncWrite(command);
    Run(timeout);

    data = m_data;
  //  data.insert(data.begin(),m_data,m_data+m_lastReadDataSize);


    return m_lastErrorCode;
}

void CCommPort::readHandler(const boost::system::error_code &err, std::size_t readBytes)
{
    m_lastErrorCode = err;
    m_lastReadDataSize = readBytes;
    m_data.resize(readBytes);
}

void CCommPort::writeHandler(const boost::system::error_code &err, std::size_t writeBytes)
{
    m_lastErrorCode = err;
    AsyncRead();
}

void CCommPort::_fnRead()
{
    for(;;)
    {
        if(!g_isLowWrited)
        {
            continue;
        }

        if(m_fd < 0)
        {
            std::cout<<__FUNCTION__<<"(): the port descriptor is illegal"<<std::endl;
            sleep(1);
        }
        struct pollfd fds;
        fds.fd=m_fd;
        fds.events = POLLIN;
        poll(&fds, 1, 1000); //ожидаем 1000 мс

        int size = -1;
        int realSize= 0;
        unsigned char data[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        if(fds.revents & POLLIN)
        {
            while(realSize < 47)
            {
                size = read(m_fd, data+realSize, 6);
                realSize +=size;
            }
        }
        if(realSize >= 47)
        {
            g_isLowReaded = true;
            g_isLowWrited = false;
            m_Lowdata = data;
            ::memcpy(m_Lowdata,data,47);
        }
    }
}

std::vector<unsigned char> CCommPort::GetData() const
{
    return m_data;
}

int CCommPort::LowRead(void* ptr, int ctr)
{
    if(m_fd < 0)
    {
        std::cout<<__FUNCTION__<<"(): the port descriptor is illegal"<<std::endl;
        sleep(1);
    }
    struct pollfd fds;
    fds.fd=m_fd;
    fds.events = POLLIN;
    poll(&fds, 1, 1000); //ожидаем 1000 мс

    int size = -1;
    if(fds.revents & POLLIN)
    {
        size = read(m_fd, ptr, ctr);
    }
    return size;
}

int CCommPort::LowWrite(const void *dptr, int size)
{
    tcflush(m_fd,TCIOFLUSH);
    int n = write(m_fd,dptr,size);

    if(n == -1)
    {
        std::cout<<__FUNCTION__<<"(): Failed to write data to the port"<<std::endl;
    }
    else
    {
        g_isLowWrited = true;
    }
    return n;
}

bool CCommPort::LowConnect(std::string strPortName)
{
    m_fd = ::open(strPortName.data(),O_RDWR | O_NOCTTY ,S_IRWXU);

    if(m_fd >0)
        std::cout<<"The port is opened successfully"<<std::endl;
    else
        std::cout<<"The port can’t be opened"<<std::endl;

    termios config;

    // Get the current configuration of the serial interface
    if( tcgetattr( m_fd, &config ) < 0 ) {
        return false;
    }

    config.c_iflag = 0;
    config.c_oflag = 0;
    config.c_lflag = 0;

    // Настраиваем параметры COM-порта
    // Статья: http://www.ccfit.nsu.ru/~deviv/courses/unix/unix/ng6b870.html
    // CS8 -> BYTE=8
    // PARENB -> Разрешить контроль четности. У нас отсутствует, т.е. контроль чётности 'N'
    // CSTOPB -> Посылать два стоповых бита, иначе один. У нас отсутствует, т.е. один бит
    config.c_cflag = B921600 | CS8 | CREAD | CLOCAL;
    config.c_cc[ VMIN ]  = 0;
    config.c_cc[ VTIME ] = 10;  // Таймаут чтения, децисекунды (это linux, детка)

    // Communication speed (simple version, using the predefined constants)
    if( cfsetispeed( &config, B921600 ) < 0 || cfsetospeed( &config, B921600 ) < 0 ) {
        return false;
    }

    // Finally, apply the configuration
    if( tcsetattr( m_fd, TCSANOW, &config ) < 0) {
        return false;
    }
    tcflush(m_fd, TCOFLUSH);
    tcflush(m_fd, TCIFLUSH);

    return true;
}

void CCommPort::LowExecute(const void *dptr, int size, void *ptr, int ctr)
{
    int wSieze = LowWrite(dptr,size);
    if(wSieze > 0)
    {
        for(;;)
        {
            usleep(10000);
            if(g_isLowReaded)
            {
                g_isLowReaded = false;
              //  ::memcpy(ptr,m_Lowdata,47);
                ctr = 47;
                break;
            }
        }
    }
}
