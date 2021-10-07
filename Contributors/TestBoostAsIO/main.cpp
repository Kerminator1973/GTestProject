#include <iostream>
#include "CommPort/CommPort.h"
#include <algorithm>
#include <boost/system/error_code.hpp>
//#include <benchmark/benchmark.h>

using namespace std;

unsigned short GetCrc(unsigned char *pbData, unsigned short wDataSize)
{
    unsigned short TmpCRC=0, i;
    unsigned char j;
    for(i=0; i < wDataSize; i++)
    {
        TmpCRC = TmpCRC ^ pbData[i];
        for(j=0; j < 8; j++)
        {
            if(TmpCRC & 0x0001)
            {
                TmpCRC >>= 1;
                TmpCRC ^= 0x08408;
            }
            else TmpCRC >>= 1;
        }
    }
    return TmpCRC;
}

std::vector<unsigned char> AddCrc(unsigned char *pbData, unsigned short wDataSize)
{
    std::vector<unsigned char> data;
    unsigned short wCrc = GetCrc( pbData, wDataSize - 2 );
    //перед возвратом меняем местами старший и младший байт результата,
    //в таком виде по спецификации CCNET используются 2 байта CRC
    ::memcpy( (void*)( pbData + wDataSize - 2 ), &wCrc, 2 );
    data.insert(data.begin(),pbData,pbData+wDataSize);
    return data;
}

void Show(std::vector<unsigned char> v)
{
    if(v.size() == 0)
        return;
    for(auto i: v)
    {
        std::cout<<(int)i<<" ";
    }
    std::cout<<std::endl;
}

static bool isOpen = false;
CCommPort port("/dev/ttyUSB1",true);
std::vector<unsigned char> vData = {0x02,0x03,0x06,0x37,0xfe,0xc7};
//vData = AddCrc(vData.data(),vData.size());
void pollF()
{
  // std::vector<unsigned char> vResult;
   // boost::system::error_code eResult = port.Execute(vData,vResult,3000);
    unsigned char data[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   // port.LowWrite(vData.data(),6);
   // port.LowRead(data,47);
    port.LowExecute(vData.data(),6,data,47);
//    std::cout<<(int)data[0]<<" "<<(int)data[1]<<" "<<(int)data[2]<<std::endl;
}

static void BM_poll(benchmark::State& state)
{
    for(auto _ : state)
        pollF();
}


//int main()
//{
//    //данные устройства crc xfr/xc7
//    //перезагрузка crc x41/xb3
//    // CCommPort port("/dev/pts/3");
//    CCommPort port("/dev/ttyUSB0",false);
//    std::vector<unsigned char> vData = {0x02,0x03,0x06,0x37,0xfe,0xc7};
//    vData = AddCrc(vData.data(),vData.size());

//    std::vector<unsigned char> vResult;

//    int sum = 0;
//    for(int i=0;i<100;i++)
//    {
//       unsigned int start = clock();
//       boost::system::error_code eResult = port.Execute(vData,vResult,3000);
//       unsigned int end = clock();
//       unsigned int t = end - start;
//       sum+=t;
//       std::cout<<"time: "<<t<<std::endl;
//       Show(vResult);
//     //  sleep(1);
//    }
//    std::cout<<sum/100;

//   boost::system::error_code eResult = port.Execute(vData,vResult);

////    unsigned char data[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
////    int sum = 0;
////    for(int i=0;i<100;i++)
////    {
////        unsigned int start = clock();
////        port.LowWrite(vData.data(),6);
////        port.LowRead(data,47);
////        unsigned int end = clock();
////        unsigned int t = end - start;
////        std::cout<<"time: "<<t<<std::endl;
////        sum+=t;
////        for(int j=0;j<47;j++)
////            std::cout<<data[j]<<" ";
////        std::cout<<std::endl;
////    }
////    std::cout<<sum/100;


//    return 0;
//}




BENCHMARK(BM_poll);
BENCHMARK_MAIN();


