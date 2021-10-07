#pragma once
#include <iostream>
#include "boost/asio.hpp"
#include "boost/asio/error.hpp"
#include <vector>
#include <boost/bind/bind.hpp>
#include <benchmark/benchmark.h>
#include <thread>

#define IN
#define OUT

//typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;


class CCommPort
{
public:
     CCommPort(const std::string strPortName, bool isLow = false);
    ~CCommPort();

    int ConnectToPort(const std::string strPortName, int baud);

    void DisconnetFromPort();

    void AsyncRead();
    void AsyncWrite(const std::vector<unsigned char> &buf);

    int Run(const int millisec=1000);

     boost::system::error_code Execute(const IN std::vector<unsigned char> &command, OUT std::vector<unsigned char> &data, IN int timeout = 2000);



     std::vector<unsigned char> GetData() const;

     int LowRead(void *ptr, int ctr);
     int LowWrite(const void *dptr, int size);
     bool LowConnect(std::string strPortName);
     void LowExecute(const void IN *dptr, IN int size, OUT void *ptr, OUT int ctr);

     // int m_fd;
     // unsigned char *m_Lowdata;

private:
     void readHandler(const boost::system::error_code &err, std::size_t readBytes);
     void writeHandler(const boost::system::error_code &err, std::size_t writeBytes);
private:

    boost::asio::io_service m_ioService;
    //serial_port_ptr m_port;
    boost::asio::serial_port m_port;
    std::vector<unsigned char> m_data;

    boost::system::error_code m_lastErrorCode;

    int m_maxSize;

   int m_lastReadDataSize;


   unsigned int m_Wstart;
   unsigned int m_Wend;
   unsigned int m_Wtime;

   unsigned int m_Rstart;
   unsigned int m_Rend;
   unsigned int m_Rtime;

   std::thread m_readThread;
   static void _fnRead();




};
