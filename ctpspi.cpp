#include "./include/global.h"
#include "./include/application.h"
#include "./include/ctpapi.h"
#include "./include/ctpspi.h"


//static const int kMarketLen = sizeof(CThostFtdcDepthMarketDataField);

CTPMdSpi::CTPMdSpi(CMDApi* pParent,
				   CThostFtdcMdApi *pUserApi,
				   SyncEvent& ev):
					m_ctpmdclient(pParent),
				    m_mdapi(pUserApi),
				    m_ev(ev){
}

CTPMdSpi::~CTPMdSpi(){
	m_ctpmdclient = NULL;
	m_mdapi = NULL;
}

void CTPMdSpi::OnFrontConnected(){
	//LOG << "连接CTP期货行情服务器成功";
	m_ctpmdclient->SetMdConnStatus(true);
	m_ev.signal();
}

void CTPMdSpi::OnFrontDisconnected(int nReason){
	//LOG << "<<<<<<<<<<<<<<<<<<<<CTP期货行情服务器断开连接>>>>>>>>>>>>>>>>>>>>";
	m_ctpmdclient->SetMdLink(FALSE);
}

void CTPMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
							  CThostFtdcRspInfoField *pRspInfo,	
							  int	nRequestID,	
							  bool	bIsLast){
	if (NULL == pRspInfo) {
		//LOG << "行情连接登录响应数据为空";
	}else {
		if (0 == pRspInfo->ErrorID) {
			//LOG << "<<<<<<<<<<<<<<登录CTP行情服务器成功>>>>>>>>>>>>>>>>";
			m_ctpmdclient->SetMdLogin(TRUE);
			m_ev.signal();
		}else {
			//LOG << "登录CTP行情服务器失败,错误信息[" << pRspInfo->ErrorID
			//	<< ":" << pRspInfo->ErrorMsg << "]";
		}
	}
}

///登出请求响应
void CTPMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast){
	if (NULL == pRspInfo) {
		//LOG << "登出行情服务器响应数据为空";
	}else {
		if (0 == pRspInfo->ErrorID){
			//LOG << "登出行情服务器成功";
			m_ctpmdclient->SetMdLogin(FALSE);
		}else {
			//LOG << "登出CTP行情前置服务器失败,错误信息[" << pRspInfo->ErrorID
				//<< ":" << pRspInfo->ErrorMsg << "]";
		}
	}
}

void CTPMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	if (NULL == pDepthMarketData){
		//LOG << "CTP行情应答数据为空";
		return ;
	}else {
		//在订阅行情之前已经在内存中保存了一份行情
		Application	*pApp = m_ctpmdclient->GetApp();		//APP对象
		pApp->update_last_md(pDepthMarketData);		//更新行情数据
		m_ev.signal();
	}
}

void CTPMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
								  CThostFtdcRspInfoField *pRspInfo, 
								  int nRequestID, 
								  bool bIsLast){
}

void CTPMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void CTPMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
									CThostFtdcRspInfoField *pRspInfo, 
									int nRequestID, 
									bool bIsLast){
}

void CTPMdSpi::OnHeartBeatWarning(int nTimeLapse){
	//LOG << "收到CTP行情服务器心跳警告,距离上次心跳时间[" << nTimeLapse << "]";
}

bool CTPMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo){
	return ((NULL != pRspInfo) && (pRspInfo->ErrorID != 0));
}


CTPTradeSpi::CTPTradeSpi(CThostFtdcTraderApi * pClientApi,CTradeApi * pClient,SyncEvent & ev):
		m_ctpTradeClient(pClient),m_traderApi(pClientApi),m_ev(ev)
{
}

CTPTradeSpi::~CTPTradeSpi(){
	if(NULL != m_ctpTradeClient){
		m_ctpTradeClient=NULL;
	}
}
CTPTradeSpi::IsErrorRspInfo(CThostFtdcRspInfoField * pRspInfo){
	return ((NULL != pRspInfo) && (pRspInfo->ErrorID != 0));
}
void CTPTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin,
									CThostFtdcRspInfoField * pRspInfo,
									int nRequestID,bool bIsLast)
{
	if(NULL == pRspUserLogin){
		//LOG error for login
		m_ev.signal();
		return ;
	}
	if(IsErrorRspInfo(pRspInfo)){
		//LOG error for login
		m_ev.signal();
		return ;
	}
	if(strlen(pRspUserLogin->TradingDay) != 0){
		m_ctpTradeClient->setTradeDay(atoi(pRspUserLogin->TradingDay));
	}
	m_ctpTradeClient->setCTPLoginStatus(true);
	m_ev.signal();
	return ;
}
void CTPTradeSpi::OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument,
									CThostFtdcRspInfoField * pRspInfo,
									int nRequestID,bool bIsLast)
{
	if(NULL == pInstrument){
		//
		m_ev.signal();
		return ;
	}
	if(IsErrorRspInfo(pRspInfo)){
		//
		m_ev.signal();
		return ;
	}
	
}