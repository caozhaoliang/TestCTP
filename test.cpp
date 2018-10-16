#include <pthread.h>

#include "./include/test.h"
extern SyncEvent g_ev;

CTestTradeSpi::CTestTradeSpi(CThostFtdcTraderApi *pClientApi){
	
}
CTestTradeSpi::~CTestTradeSpi(){
	
}
void CTestTradeSpi::OnFrontConnected(){
	 g_ev.signal();
}
void CTestTradeSpi::OnFrontDisconnected(int nReason){
	
}
void CTestTradeSpi::OnHeartBeatWarning(int nTimeLapse){

}

void CTestTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){

}
CTestMarketSpi::CTestMarketSpi(CThostFtdcMdApi* pClientApi){

}
CTestMarketSpi::~CTestMarketSpi(){

}
	//当客户端与交易托管系统建立起通信连接，客户端需要进行登录
void CTestMarketSpi::OnFrontConnected(){

}

	//当客户端与交易托管系统通信连接断开时，该方法被调用
void CTestMarketSpi::OnFrontDisconnected(int	nReason){

}


int test_conn_trade(const std::string strAddr){
	if(strAddr.length() <= 0 ){
		return -1;
	}
	std::string strPath = "./";
	CThostFtdcTraderApi* pTradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi(strPath.c_str());
	if(NULL == pTradeApi ){
		return -2;
	}
	CTestTradeSpi* pTradeSpi = new CTestTradeSpi(pTradeApi);
	if(NULL == pTradeSpi){
		return -3;
	}
	THOST_TE_RESUME_TYPE type= THOST_TERT_QUICK;
	pTradeApi->RegisterSpi(pTradeSpi);
	pTradeApi->SubscribePublicTopic(type);
	pTradeApi->SubscribePrivateTopic(type);

	pTradeApi->RegisterFront((char*)strAddr.c_str());
	pTradeApi->Init();
	if(!g_ev.timed_wait(6000)){
		return -4;
	}
	return 0;
}

int test_conn_marketdata(const std::string strAddr){
	if(strAddr.length() <= 0 ){
		return -1;
	}
	std::string strPath = "./";
	CThostFtdcMdApi* pMdApi = CThostFtdcMdApi::CreateFtdcMdApi(strPath.c_str());
	if(NULL == pMdApi){
		return -2;
	}
	CTestMarketSpi* pMdSpi = new CTestMarketSpi(pMdApi);
	if(NULL == pMdSpi){
		return -3;
	}
	pMdApi->RegisterSpi(pMdSpi);
	pMdApi->RegisterFront((char*)strAddr.c_str());
	pMdApi->Init();

	if(!g_ev.timed_wait(6000)){
		return -4;
	}
	return 0;
}

int test_subscrib(){
	//订阅行情
	while(1){
		

	}
}