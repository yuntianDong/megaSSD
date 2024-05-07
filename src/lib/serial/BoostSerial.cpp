/*
 * file : BoostSerial.cpp
 */
#include <string>

#include <boost/locale.hpp>
#include "serial/BoostSerial.h"
#include "Logging.h"

using namespace std;
using namespace boost::asio;

#define BYTES_DEBUG_DUMP(d,l)	\
	if(true)	\
	{	\
		printf("\n");	\
		for(int idx=0;idx<(l);idx++)	\
		{	\
			printf("%02X ",(d)[idx]);	\
		}	\
		printf("\n");	\
	}	\

BoostSerial::BoostSerial(const char* port)
{
	mpSerial	= serial_port_ptr(new serial_port(mIOSrv));
	try
	{
		mpSerial->open(port);
	}catch(exception ex)
	{
		LOGERROR("%s",ex.what());
	}
}

BoostSerial::~BoostSerial()
{
	mpSerial->close();
	mIOSrv.stop();

	mpSerial.reset();
}

bool BoostSerial::Configure(uint32_t baud,enFlowCtrl flowCtrl,enDataParity parity,
		enStopBit stopBit,uint8_t charSize)
{
	if(false == mpSerial->is_open())
	{
		LOGERROR("COM Port is not Opened!");
		return false;
	}

	serial_port::flow_control::type	fcParam = serial_port::flow_control::none;
	serial_port::parity::type dpParam = serial_port::parity::none;
	serial_port::stop_bits::type sbParam = serial_port::stop_bits::one;

	if(FC_SW == flowCtrl)
	{
		fcParam = serial_port::flow_control::software;
	}
	else if(FC_HW == flowCtrl)
	{
		fcParam = serial_port::flow_control::hardware;
	}

	if(DP_ODD == parity)
	{
		dpParam = serial_port::parity::odd;
	}
	else if(DP_EVEN == parity)
	{
		dpParam = serial_port::parity::even;
	}

	if(SB_1_5 == stopBit)
	{
		sbParam = serial_port::stop_bits::onepointfive;
	}
	else if(SB_2_0 == stopBit)
	{
		sbParam = serial_port::stop_bits::two;
	}

	mpSerial->set_option(serial_port::baud_rate(baud));
	mpSerial->set_option(serial_port::flow_control(fcParam));
	mpSerial->set_option(serial_port::parity(dpParam));
	mpSerial->set_option(serial_port::stop_bits(sbParam));
	mpSerial->set_option(serial_port::character_size(charSize));

	return true;
}

bool BoostSerial::Write(void* data,uint32_t length)
{
	if(false == mpSerial->is_open())
	{
		LOGERROR("COM Port is not Opened!");
		return false;
	}

	boost::system::error_code err;
	uint32_t nWrite = mpSerial->write_some(buffer(data,length),err);
	if(err)
	{
		LOGERROR("%s",err.message().c_str());
		return false;
	}

	return nWrite == length;
}

bool BoostSerial::Read(void* data,uint32_t length)
{
	if(false == mpSerial->is_open())
	{
		LOGERROR("COM Port is not Opened!");
		return false;
	}

	boost::system::error_code err;
	mpSerial->read_some(buffer(data,length),err);
	if(err)
	{
		LOGERROR("%s",err.message().c_str());
		return false;
	}

	return true;
}

bool BoostSerial::Read(void* data,uint32_t length,BSReadCompCondition* pCond)
{
	if(false == mpSerial->is_open())
	{
		LOGERROR("COM Port is not Opened!");
		return false;
	}

	mutable_buffer buf = buffer(data,length);

	boost::system::error_code err;
	int nRead = 0;
	while(0 == nRead && !err)
	{
		nRead = read(*mpSerial,buf,*pCond,err);
		if(err)
		{
			LOGERROR("%s",err.message().c_str());
			return false;
		}
	}

	return true;
}

bool BoostSerial::Write(const char* data,const char* encoding)
{
	if(false == mpSerial->is_open())
	{
		LOGERROR("COM Port is not Opened!");
		return false;
	}

	string wData(data);
	if(NULL != encoding)
	{
		wData	= boost::locale::conv::between(wData,"ASCII",encoding);
	}

	boost::system::error_code err;
	int nWrite = mpSerial->write_some(buffer(wData),err);
	if(err)
	{
		LOGERROR("%s",err.message().c_str());
		return false;
	}

	return true;
}

bool BoostSerial::Read(char* data,const char* encoding)
{
	if(false == mpSerial->is_open())
	{
		LOGERROR("COM Port is not Opened!");
		return false;
	}

	string wData(data);

	boost::system::error_code err;
	mpSerial->read_some(buffer(wData),err);
	if(err)
	{
		LOGERROR("%s",err.message().c_str());
		return false;
	}

	if(NULL != encoding)
	{
		wData	= boost::locale::conv::between(wData,encoding,"ASCII");
	}

	return true;
}

bool BoostSerial::ReadUtil(char* data,int maxLen,const char* delims,const char* encoding)
{
	if(false == mpSerial->is_open())
	{
		LOGERROR("COM Port is not Opened!");
		return false;
	}

	string wData;
	boost::system::error_code err;

	read_until(*mpSerial,dynamic_buffer(wData),delims,err);
	if(err)
	{
		LOGERROR("%s",err.message().c_str());
		return false;
	}

	if(NULL != encoding)
	{
		wData	= boost::locale::conv::between(wData,encoding,"ASCII");
	}

	int length	= wData.length() + 1;
	length	= (maxLen >= length)? length : maxLen;
	memcpy(data,wData.c_str(),length);

	return true;
}

