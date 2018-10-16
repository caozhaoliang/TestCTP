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
	//���ͻ����뽻���й�ϵͳ������ͨ�����ӣ��ͻ�����Ҫ���е�¼
void CTestMarketSpi::OnFrontConnected(){

}

	//���ͻ����뽻���й�ϵͳͨ�����ӶϿ�ʱ���÷���������
void CTestMarketSpi::OnFrontDisconnected(int	nReason){

}


int test_conn_trade(const std::string strAddr){
	if(strAddr.length() <= 0 ){
		LOG(ERROR) << "Test Connect Trade Empty Addr";
		return -1;
	}
	std::string strPath = "./ConnTestTrade/";
	CThostFtdcTraderApi* pTradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi(strPath.c_str());
	if(NULL == pTradeApi ){
		LOG(ERROR) <<"Test Connect Trade CreateApi failed";
		return -2;
	}
	CTestTradeSpi* pTradeSpi = new CTestTradeSpi(pTradeApi);
	if(NULL == pTradeSpi){
		LOG(ERROR)<< "new CTPTradeSpi NULL";
		return -3;
	}
	THOST_TE_RESUME_TYPE type= THOST_TERT_QUICK;
	pTradeApi->RegisterSpi(pTradeSpi);
	pTradeApi->SubscribePublicTopic(type);
	pTradeApi->SubscribePrivateTopic(type);

	pTradeApi->RegisterFront((char*)strAddr.c_str());
	pTradeApi->Init();
	if(!g_ev.timed_wait(6000)){
		LOG(ERROR) <<"Test Connect Trade Init() timeout";
		return -4;
	}
	return 0;
}

int test_conn_marketdata(const std::string strAddr){
	if(strAddr.length() <= 0 ){
		LOG(ERROR) << "Test Connect MD Empty Addr";
		return -1;
	}
	std::string strPath = "./ConnTestMD/";
	CThostFtdcMdApi* pMdApi = CThostFtdcMdApi::CreateFtdcMdApi(strPath.c_str());
	if(NULL == pMdApi){
		LOG(ERROR) <<"Test Connect MD CreateApi failed";
		return -2;
	}
	CTestMarketSpi* pMdSpi = new CTestMarketSpi(pMdApi);
	if(NULL == pMdSpi){
		LOG(ERROR)<< "new CTPMDSpi NULL";
		return -3;
	}
	pMdApi->RegisterSpi(pMdSpi);
	pMdApi->RegisterFront((char*)strAddr.c_str());
	pMdApi->Init();

	if(!g_ev.timed_wait(6000)){
		LOG(ERROR) <<"Test Connect MD Init() timeout";
		return -4;
	}
	return 0;
}

int test_subscrib(){
	//��������
	while(1){
		

	}
}