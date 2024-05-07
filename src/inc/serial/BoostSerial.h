#ifndef _BOOST_SERIAL_H
#define _BOOST_SERIAL_H

#include <boost/asio.hpp>

typedef enum _enFlowCtrl
{
	FC_NONE,
	FC_SW,
	FC_HW,
}enFlowCtrl;

typedef enum _enDataParity
{
	DP_NONE,
	DP_ODD,
	DP_EVEN,
}enDataParity;

typedef enum _enStopBit
{
	SB_1_0,
	SB_1_5,
	SB_2_0,
}enStopBit;

#define DEF_SERIAL_BAUDRATE			115200
#define DEF_SERIAL_FLOWCTRL			FC_NONE
#define DEF_SERIAL_PARITY			DP_NONE
#define DEF_SERIAL_STOPBIT			SB_1_0
#define DEF_SERIAL_CHARSIZE			8

#ifndef UNUSED
#define UNUSED(expr) do { (void)(expr); } while (0)
#endif

class BSReadCompCondition
{
protected:
	boost::asio::mutable_buffer		mDataBuf;

	virtual std::size_t completion_check(
			const boost::system::error_code& error,
			std::size_t bytes_transferred)
	{
		UNUSED(bytes_transferred);
		return 0;
	};
public:
	BSReadCompCondition(boost::asio::mutable_buffer buf)
		:mDataBuf(buf)
	{};
	virtual ~BSReadCompCondition(void)
	{};

	std::size_t operator()(
			const boost::system::error_code& error,
			std::size_t bytes_transferred)
	{
		return completion_check(error,bytes_transferred);
	};
};

class BoostSerial
{
protected:
	typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;

	serial_port_ptr					mpSerial;
	boost::asio::io_service 		mIOSrv;
public:
	BoostSerial(const char* port);
	virtual ~BoostSerial();

	bool Configure(uint32_t baud = DEF_SERIAL_BAUDRATE,
			enFlowCtrl flowCtrl	= DEF_SERIAL_FLOWCTRL,
			enDataParity parity	= DEF_SERIAL_PARITY,
			enStopBit stopBit	= DEF_SERIAL_STOPBIT,
			uint8_t charSize	= DEF_SERIAL_CHARSIZE);

	bool Write(void* data,uint32_t length);
	bool Read(void* data,uint32_t length);
	bool Read(void* data,uint32_t length,BSReadCompCondition* pCond);
	bool Write(const char* data,const char* encoding=NULL);
	bool Read(char* data,const char* encoding=NULL);
	bool ReadUtil(char* data,int len,const char* delims,const char* encoding=NULL);
};

#endif
