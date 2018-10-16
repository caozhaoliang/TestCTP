#ifndef _TEST_H__
#define _TEST_H__
#include <iostream>
#include <pthread.h>
#include "global.h"

class CTestTradeSpi: public CThostFtdcTraderSpi{
public:
	CTestTradeSpi(CThostFtdcTraderApi *pClientApi);
	~CTestTradeSpi();
	//当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	//当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(int nReason);

	//心跳超时警告。当长时间未收到报文时，该方法被调用。
	virtual void OnHeartBeatWarning(int nTimeLapse);

	//登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	CThostFtdcTraderApi *	pTApi;
};
class CTestMarketSpi: public CThostFtdcMdSpi{
public:
    CTestMarketSpi(CThostFtdcMdApi* pClientApi);
	~CTestMarketSpi();
	//当客户端与交易托管系统建立起通信连接，客户端需要进行登录
	virtual	void OnFrontConnected();

	//当客户端与交易托管系统通信连接断开时，该方法被调用
	virtual	void OnFrontDisconnected(int	nReason);

private:
	CThostFtdcMdApi* 		pMarketApi;
	
};
#endif

