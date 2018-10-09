#include <pthread.h>

#include "./include/test.h"


CTestTradeSpi::CTestTradeSpi(CThostFtdcTraderApi *pClientApi){
	
}
CTestTradeSpi::~CTestTradeSpi(){
	
}
CTestTradeSpi::OnFrontConnected(){
	 g_ev.signal();
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
	pTradeApi->RegisterSpi(pTradeSpi);
	pTradeApi->SubscribePublicTopic(2);
	pTradeApi->SubscribePrivateTopic(2);

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
	//¶©ÔÄÐÐÇé
	while(1){
		

	}
}