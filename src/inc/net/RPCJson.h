#ifndef _RPC_JSON_H
#define _RPC_JSON_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define	RPCJSON_JSONRPC				"jsonrpc"
#define RPCJSON_ID					"id"

#define RPCJSON_REQ_METHOD			"method"
#define RPCJSON_REQ_PARAMS			"params"

#define RPCJSON_RESP_RESULT			"result"
#define RPCJSON_RESP_ERROR			"error"
#define RPCJSON_ERR_CODE			"code"
#define RPCJSON_ERR_MESSAGE			"message"
#define RPCJSON_ERR_DATA			"data"

#define RPCJSON_VERSION				"2.0"

#define RPCJSON_EC_PARSE_ERROR			-32700
#define RPCJSON_EM_PARSE_ERROR			"Parse Error"
#define RPCJSON_EC_INVALID_REQUEST		-32600
#define RPCJSON_EM_INVALID_REQUEST		"Invalid Request"
#define RPCJSON_EC_METHOD_NOT_FOUND		-32601
#define RPCJSON_EM_METHOD_NOT_FOUND		"Method Not Found"
#define RPCJSON_EC_INVALID_PARAMS		-32602
#define RPCJSON_EM_INVALID_PARAMS		"Invalid Params"
#define RPCJSON_EC_INTERNAL_ERROR		-32603
#define RPCJSON_EM_INTERNAL_ERROR		"Internal Error"

class BasicRPCJson
{
protected:
	boost::property_tree::ptree		mRoot;

public:
	BasicRPCJson(int id)
	{
		mRoot.clear();
		mRoot.put(RPCJSON_JSONRPC,RPCJSON_VERSION);
		mRoot.put(RPCJSON_ID,id);
	}

	BasicRPCJson(std::string jsonStr)
	{
		std::stringstream sstream(jsonStr);
		boost::property_tree::json_parser::read_json(sstream, mRoot);
	}

	int GetID(void)
	{
		return mRoot.get<int>(RPCJSON_ID);
	}

	const char* GetVersion(void)
	{
		return mRoot.get<std::string>(RPCJSON_JSONRPC).c_str();
	}

	const char* operator ()(char* buf=NULL,int length=0)
	{
		std::stringstream ss;
		ss.clear();
		ss.str("");
		boost::property_tree::write_json(ss, mRoot);

		if(NULL != buf && 0 != length)
		{
			strncpy(buf,ss.str().c_str(),length);
		}

		return ss.str().c_str();
	};
};

class RPCJsonReq : public BasicRPCJson
{
protected:
	boost::property_tree::ptree		mParams;
public:
	RPCJsonReq(int id,const char* method)
		:BasicRPCJson(id)
	{
		mRoot.put(RPCJSON_REQ_METHOD,method);
		mParams.clear();
	};

	RPCJsonReq(std::string jsonStr)
		:BasicRPCJson(jsonStr)
	{
		mParams = mRoot.get_child(RPCJSON_REQ_PARAMS);
	}

	const char* GetMethod(void)
	{
		return mRoot.get<std::string>(RPCJSON_REQ_METHOD).c_str();
	}

	template <typename T>
	void AddParam(const char* key,T val)
	{
		mParams.put(key,val);
	};

	template <typename T>
	T GetParam(const char* key)
	{
		return mParams.get<T>(key);
	}

	const char* operator ()(char* buf=NULL,int length=0)
	{
		mRoot.push_back(make_pair(RPCJSON_REQ_PARAMS,mParams));

		std::stringstream ss;
		ss.clear();
		ss.str("");
		boost::property_tree::write_json(ss, mRoot);

		if(NULL != buf && 0 != length)
		{
			strncpy(buf,ss.str().c_str(),length);
		}

		return ss.str().c_str();
	};
};

class RPCJsonRespWithSuccess : public BasicRPCJson
{
protected:
	boost::property_tree::ptree		mResult;
public:
	RPCJsonRespWithSuccess(int id)
		:BasicRPCJson(id)
	{
		mResult.clear();
	};

	RPCJsonRespWithSuccess(std::string jsonStr)
		:BasicRPCJson(jsonStr)
	{
		mResult = mRoot.get_child(RPCJSON_RESP_RESULT);
	};

	template <typename T>
	void AddResult(const char* key,T val)
	{
		mResult.put(key,val);
	}

	template <typename T>
	T GetResult(const char* key)
	{
		return mResult.get<T>(key);
	}

	const char* operator ()(char* buf=NULL,int length=0)
	{
		mRoot.push_back(make_pair(RPCJSON_RESP_RESULT,mResult));

		std::stringstream ss;
		ss.clear();
		ss.str("");
		boost::property_tree::write_json(ss, mRoot);

		if(NULL != buf && 0 != length)
		{
			strncpy(buf,ss.str().c_str(),length);
		}

		return ss.str().c_str();
	};
};

class RPCJsonRespWithFailure : public BasicRPCJson
{
protected:
	boost::property_tree::ptree		mError;
	boost::property_tree::ptree		mErrData;
public:
	RPCJsonRespWithFailure(int id,int error,const char* message)
		:BasicRPCJson(id)
	{
		mError.clear();
		mErrData.clear();
		mError.put(RPCJSON_ERR_CODE,error);
		mError.put(RPCJSON_ERR_MESSAGE,message);
	}

	RPCJsonRespWithFailure(std::string jsonStr)
		:BasicRPCJson(jsonStr)
	{
		mError = mRoot.get_child(RPCJSON_RESP_ERROR);
		mErrData = mError.get_child(RPCJSON_ERR_DATA);
	};

	int GetErrorCode(void)
	{
		return mError.get<int>(RPCJSON_ERR_CODE);
	}

	const char* GetErrorMessage(void)
	{
		return mError.get<std::string>(RPCJSON_ERR_MESSAGE).c_str();
	}

	template <typename T>
	void AddErrData(const char* key,T val)
	{
		mErrData.put(key,val);
	}

	template <typename T>
	T GetErrData(const char* key)
	{
		return mErrData.get<T>(key);
	}

	const char* operator ()(char* buf=NULL,int length=0)
	{
		mError.push_back(make_pair(RPCJSON_ERR_DATA,mErrData));
		mRoot.push_back(make_pair(RPCJSON_RESP_ERROR,mError));

		std::stringstream ss;
		ss.clear();
		ss.str("");
		boost::property_tree::write_json(ss, mRoot);

		if(NULL != buf && 0 != length)
		{
			strncpy(buf,ss.str().c_str(),length);
		}

		return ss.str().c_str();
	};
};

class RPCJsonErrParseError : public RPCJsonRespWithFailure
{
public:
	RPCJsonErrParseError(int id)
		:RPCJsonRespWithFailure(id,RPCJSON_EC_PARSE_ERROR,RPCJSON_EM_PARSE_ERROR)
	{};
};

class RPCJsonErrInvalidReq : public RPCJsonRespWithFailure
{
public:
	RPCJsonErrInvalidReq(int id)
		:RPCJsonRespWithFailure(id,RPCJSON_EC_INVALID_REQUEST,RPCJSON_EM_INVALID_REQUEST)
	{};
};

class RPCJsonErrMethodNotFound : public RPCJsonRespWithFailure
{
public:
	RPCJsonErrMethodNotFound(int id)
		:RPCJsonRespWithFailure(id,RPCJSON_EC_METHOD_NOT_FOUND,RPCJSON_EM_METHOD_NOT_FOUND)
	{};
};

class RPCJsonErrInvalidParams : public RPCJsonRespWithFailure
{
public:
	RPCJsonErrInvalidParams(int id)
		:RPCJsonRespWithFailure(id,RPCJSON_EC_INVALID_PARAMS,RPCJSON_EM_INVALID_PARAMS)
	{};
};

class RPCJsonErrInternalError : public RPCJsonRespWithFailure
{
public:
	RPCJsonErrInternalError(int id)
		:RPCJsonRespWithFailure(id,RPCJSON_EC_INTERNAL_ERROR,RPCJSON_EM_INTERNAL_ERROR)
	{};
};

#endif
