#ifndef _NANO_MSG_NET_COMM_H
#define _NANO_MSG_NET_COMM_H

#include <time.h>
#include <assert.h>
#include <nanomsg/nn.h>
#include <nanomsg/bus.h>
#include <nanomsg/pair.h>
#include <nanomsg/reqrep.h>
#include <nanomsg/pubsub.h>
#include <nanomsg/survey.h>
#include <nanomsg/pipeline.h>

#include <nanomsg/inproc.h>
#include <nanomsg/ipc.h>
#include <nanomsg/tcp.h>
#include <nanomsg/ws.h>

#define INVALID_SOCKET			-1
#define INVALID_ENDPOINTID		-1

class BasicNanoMsg
{
protected:
	int			mSocket;

	bool	CreateSocket(int domain,int protocol)
	{
		mSocket = nn_socket(domain,protocol);
		return INVALID_SOCKET != mSocket;
	};
	void	CloseSocket(void)
	{
		nn_close(mSocket);
	};
public:
	BasicNanoMsg(int domain,int protocol)
	{
		assert(true == CreateSocket(domain,protocol));
	};
	virtual ~BasicNanoMsg(void)
	{
		CloseSocket();
	};

	virtual int SendDatum(void* buf,int length)
	{
		return nn_send(mSocket,buf,length,0);
	};
	virtual int SendDatumWithNonBlock(void* buf,int length)
	{
		return nn_send(mSocket,buf,length,NN_DONTWAIT);
	};
	virtual int RecvDatum(void* buf,int length)
	{
		return nn_recv(mSocket,buf,length,0);
	};
	virtual int RecvDatumWithNonBlock(void* buf,int length)
	{
		return nn_recv(mSocket,buf,length,NN_DONTWAIT);
	};

	void DumpStatistic(void)
	{
		printf("ESTABLISHED_CONNECTIONS= %llu",nn_get_statistic(mSocket,NN_STAT_ESTABLISHED_CONNECTIONS));
		printf("ACCEPTED_CONNECTIONS   = %llu",nn_get_statistic(mSocket,NN_STAT_ACCEPTED_CONNECTIONS));
		printf("DROPPED_CONNECTIONS    = %llu",nn_get_statistic(mSocket,NN_STAT_DROPPED_CONNECTIONS));
		printf("BROKEN_CONNECTIONS     = %llu",nn_get_statistic(mSocket,NN_STAT_BROKEN_CONNECTIONS));
		printf("CONNECT_ERRORS         = %llu",nn_get_statistic(mSocket,NN_STAT_CONNECT_ERRORS));
		printf("BIND_ERRORS            = %llu",nn_get_statistic(mSocket,NN_STAT_BIND_ERRORS));
		printf("ACCEPT_ERRORS          = %llu",nn_get_statistic(mSocket,NN_STAT_ACCEPT_ERRORS));
		printf("CURRENT_CONNECTIONS    = %llu",nn_get_statistic(mSocket,NN_STAT_CURRENT_CONNECTIONS));
		printf("MESSAGES_SENT          = %llu",nn_get_statistic(mSocket,NN_STAT_MESSAGES_SENT));
		printf("MESSAGES_RECEIVED      = %llu",nn_get_statistic(mSocket,NN_STAT_MESSAGES_RECEIVED));
		printf("BYTES_SENT             = %llu",nn_get_statistic(mSocket,NN_STAT_BYTES_SENT));
		printf("BYTES_RECEIVED         = %llu",nn_get_statistic(mSocket,NN_STAT_BYTES_RECEIVED));
	};
	int	 GetLastErrorCode(void)
	{
		return nn_errno();
	};
	const char* GetLastErrorMsg(void)
	{
		return nn_strerror(nn_errno());
	}

	bool SetOptSendBuf(int bufSize)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_SNDBUF,&bufSize,sizeof(int));
	};
	bool SetOptRecvBuf(int bufSize)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_RCVBUF,&bufSize,sizeof(int));
	};
	bool SetOptRecvBufMax(int bufSize)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_RCVMAXSIZE,&bufSize,sizeof(int));
	};
	bool SetOptSendTimeOut(int timeout)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_SNDTIMEO,&timeout,sizeof(int));
	};
	bool SetOptRecvTimeOut(int timeout)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_RCVTIMEO,&timeout,sizeof(int));
	};
	bool SetOptReconnIVL(int ivl)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_RECONNECT_IVL,&ivl,sizeof(int));
	};
	bool SetOptReconnIVLMax(int ivl)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_RECONNECT_IVL_MAX,&ivl,sizeof(int));
	};
	bool SetOptSendPrio(int prio)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_SNDPRIO,&prio,sizeof(int));
	};
	bool SetOptRecvPrio(int prio)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_RCVPRIO,&prio,sizeof(int));
	};
	bool SetOptIPV4Only(bool bIPV4Only)
	{
		int tmpVal = bIPV4Only;
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_IPV4ONLY,&tmpVal,sizeof(int));
	};
	bool SetOptSocketName(const char* name)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_SOCKET_NAME,name,strlen(name));
	};
	bool SetOptMaxTTL(int ttl)
	{
		return 0 == nn_setsockopt(mSocket,NN_SOL_SOCKET,NN_MAXTTL,&ttl,sizeof(int));
	};

	bool GetOptDomain(int& domain)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_DOMAIN,&domain,&sz);
	};
	bool GetOptProtocol(int& protocol)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_PROTOCOL,&protocol,&sz);
	};
	bool GetOptLinger(int& duration)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_LINGER,&duration,&sz);
	};
	bool GetOptSendBuf(int& bufSize)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_SNDBUF,&bufSize,&sz);
	};
	bool GetOptRecvBuf(int& bufSize)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_RCVBUF,&bufSize,&sz);
	};
	bool GetOptRecvBufMax(int& bufSize)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_RCVMAXSIZE,&bufSize,&sz);
	};
	bool GetOptSendTimeOut(int& timeout)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_SNDTIMEO,&timeout,&sz);
	};
	bool GetOptRecvTimeOut(int& timeout)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_RCVTIMEO,&timeout,&sz);
	};
	bool GetOptReconnIVL(int& ivl)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_RECONNECT_IVL,&ivl,&sz);
	};
	bool GetOptReconnIVLMax(int& ivl)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_RECONNECT_IVL_MAX,&ivl,&sz);
	};
	bool GetOptSendPrio(int& prio)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_SNDPRIO,&prio,&sz);
	};
	bool GetOptRecvPrio(int& prio)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_RCVPRIO,&prio,&sz);
	};
	bool GetOptIPV4Only(bool& bIPV4Only)
	{
		int tmpVal = 0;
		size_t sz = sizeof(bool);
		int rtnCode = nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_IPV4ONLY,&tmpVal,&sz);

		bIPV4Only = (tmpVal == 1);
		return rtnCode;
	};
	bool GetOptSendFD(int& fd)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_SNDFD,&fd,&sz);
	};
	bool GetOptRecvFD(int& fd)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_RCVFD,&fd,&sz);
	};
	bool GetOptMaxTTL(int& ttl)
	{
		size_t sz = sizeof(int);
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_MAXTTL,&ttl,&sz);
	};
	bool GetOptSocketName(char* name,int strlen)
	{
		size_t sz = strlen;
		return 0 == nn_getsockopt(mSocket,NN_SOL_SOCKET,NN_SOCKET_NAME,name,&sz);
	};
};

class BasicNanoMsgServer : public BasicNanoMsg
{
private:
	int		mEndPointID;
protected:
	bool Bind(const char* addr)
	{
		mEndPointID	=  nn_bind(mSocket,addr);

		return (0 <= mEndPointID);
	};
public:
	BasicNanoMsgServer(int domain,int protocol,const char* addr)
		:BasicNanoMsg(domain,protocol)
	{
		assert(true == Bind(addr));
	};

	virtual ~BasicNanoMsgServer(void)
	{
		nn_shutdown(mSocket,mEndPointID);
	};
};

class BasicNanoMsgClient : public BasicNanoMsg
{
protected:
	bool Connect(const char* addr)
	{
		if( 0 > nn_connect(mSocket,addr))
		{
			return false;
		}

		sleep(3);

		return true;
	};
public:
	BasicNanoMsgClient(int domain,int protocol,const char* addr)
		:BasicNanoMsg(domain,protocol)
	{
		assert(true == Connect(addr));
	};
};

class NanoMsgPipelineReceiver : public BasicNanoMsgServer
{
public:
	NanoMsgPipelineReceiver(const char* addr)
		:BasicNanoMsgServer(AF_SP, NN_PULL,addr)
	{};
};

class NanoMsgPipelineSender : public BasicNanoMsgClient
{
public:
	NanoMsgPipelineSender(const char* addr)
		:BasicNanoMsgClient(AF_SP, NN_PUSH,addr)
	{};
};

class NanoMsgRPCServer : public BasicNanoMsgServer
{
public:
	NanoMsgRPCServer(const char* addr)
		:BasicNanoMsgServer(AF_SP, NN_REP,addr)
	{}
};

class NanoMsgRPCClient : public BasicNanoMsgClient
{
public:
	NanoMsgRPCClient(const char* addr)
		:BasicNanoMsgClient(AF_SP, NN_REQ,addr)
	{}

	bool SetOptResendIVL(int ivl)
	{
		return 0 == nn_setsockopt(mSocket,NN_REQ,NN_REQ_RESEND_IVL,&ivl,sizeof(int));
	};
};

class NanoMsgPairServer : public BasicNanoMsgServer
{
public:
	NanoMsgPairServer(const char* addr)
		:BasicNanoMsgServer(AF_SP, NN_PAIR,addr)
	{};
};

class NanoMsgPairClient : public BasicNanoMsgClient
{
public:
	NanoMsgPairClient(const char* addr)
		:BasicNanoMsgClient(AF_SP, NN_PAIR,addr)
	{};
};

class NanoMsgPubSubServer : public BasicNanoMsg
{
private:
	int		mEndPointID;
public:
	NanoMsgPubSubServer(void)
		:BasicNanoMsg(AF_SP, NN_PUB),mEndPointID(INVALID_ENDPOINTID)
	{};
	virtual ~NanoMsgPubSubServer(void)
	{
		if(INVALID_ENDPOINTID != mEndPointID)
		{
			nn_shutdown(mSocket,mEndPointID);
		}
	};

	bool Bind(const char* addr)
	{
		mEndPointID	=  nn_bind(mSocket,addr);

		return (0 <= mEndPointID);
	};
};

class NanoMsgPubSubClient : public BasicNanoMsg
{
public:
	NanoMsgPubSubClient()
		:BasicNanoMsg(AF_SP, NN_SUB)
	{};

	bool Bind(const char* addr)
	{
		return INVALID_ENDPOINTID != nn_connect(mSocket,addr);
	};

	bool SetOptSubScribe(const char* sub)
	{
		return 0 == nn_setsockopt(mSocket,NN_SUB,NN_SUB_SUBSCRIBE,sub,strlen(sub));
	};
	bool SetOptUnSubScribe(const char* sub)
	{
		return 0 == nn_setsockopt(mSocket,NN_SUB,NN_SUB_UNSUBSCRIBE,sub,strlen(sub));
	};
};

class NanoMsgSurveyServer : public BasicNanoMsgServer
{
public:
	NanoMsgSurveyServer(const char* addr)
		:BasicNanoMsgServer(AF_SP, NN_SURVEYOR,addr)
	{};

	bool SetOptDeadline(int msTimeout)
	{
		return 0 == nn_setsockopt(mSocket,NN_SURVEYOR,NN_SURVEYOR_DEADLINE,&msTimeout,sizeof(int));
	};
};

class NanoMsgSurveyClient : public BasicNanoMsgClient
{
public:
	NanoMsgSurveyClient(const char* addr)
		:BasicNanoMsgClient(AF_SP, NN_RESPONDENT,addr)
	{};
};

class NanoMsgBusNode : public BasicNanoMsgServer
{
public:
	NanoMsgBusNode(const char* addr)
		:BasicNanoMsgServer(AF_SP, NN_BUS,addr)
	{};

	bool Connect(const char* slaver)
	{
		return 0 < nn_connect(mSocket,slaver);
	};
};

#endif
