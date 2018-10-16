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
	LOG(INFO) << "连接CTP期货行情服务器成功";
	m_ctpmdclient->SetMdConnStatus(true);
	m_ev.signal();
}

void CTPMdSpi::OnFrontDisconnected(int nReason){
	LOG(WARNING) << "CTP期货行情服务器断开连接";
	m_ctpmdclient->SetMdConnStatus(false);
	Application* pApp = m_ctpmdclient->GetApp();
	if(NULL != pApp){
		pApp->m_bMDNeedReSubcr = true;
	}
}

void CTPMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
							  CThostFtdcRspInfoField *pRspInfo,	
							  int	nRequestID,	
							  bool	bIsLast){
	if(pRspInfo != NULL && pRspInfo->ErrorID==0){
		m_ctpmdclient->SetMdLoginStatus(true);
		m_ev.signal();
	}
	return;
}

///登出请求响应
void CTPMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast){
	if(pRspInfo != NULL && pRspInfo->ErrorID == 0){
		m_ctpmdclient->SetMdLoginStatus(false);
	}
	return;
}

void CTPMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	if (NULL == pDepthMarketData){
		LOG(WARNING) << "OnRtnDepthMarketData CTP行情应答数据为空";
		return ;
	}
	//在订阅行情之前已经在内存中保存了一份行情
	Application	*pApp = m_ctpmdclient->GetApp();	//APP对象
	if(NULL != pApp){
		pApp->updateLastMd(pDepthMarketData,1);		//更新行情数据
	}
}

void CTPMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
								  CThostFtdcRspInfoField *pRspInfo, 
								  int nRequestID, 
								  bool bIsLast){
	if(NULL == pSpecificInstrument){
		LOG(WARNING) << "OnRspSubMarketData Return NULL";
		m_ev.signal();
		return;
	}
	if(pRspInfo != NULL && pRspInfo->ErrorID != 0){
		LOG(WARNING) << "OnRspSubMarketData was error:"<<pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg;
		m_ev.signal();
		return;
	}
	LOG(INFO) << "subscribe:"<<pSpecificInstrument->InstrumentID; 
	//pSpecificInstrument->InstrumentID;
	if(bIsLast){
		m_ev.signal();
	}
}

void CTPMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if(pRspInfo != NULL && pRspInfo->ErrorID != 0){
		LOG(WARNING) << "OnRsp Error:"<<pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg;
	}
	return;
}

void CTPMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
									CThostFtdcRspInfoField *pRspInfo, 
									int nRequestID, 
									bool bIsLast){
	if(pSpecificInstrument == NULL){
		LOG(WARNING) << "OnRspUnSubMarketData return NULL";
		m_ev.signal();
		return ;
	}
	if(IsErrorRspInfo(pRspInfo)){
		LOG(WARNING) << "OnRspUnSubMarketData getError:"<<pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg;
		m_ev.signal();
		return;
	}
	//退订成功
	//pSpecificInstrument->InstrumentID;
	if(bIsLast ){
		m_ev.signal();
	}
	return;
}

void CTPMdSpi::OnHeartBeatWarning(int nTimeLapse){
	LOG(WARNING)<< "OnHeartBeatWarning 收到CTP行情服务器心跳警告";
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
bool CTPTradeSpi::IsErrorRspInfo(CThostFtdcRspInfoField * pRspInfo){
	return ((NULL != pRspInfo) && (pRspInfo->ErrorID != 0));
}
void CTPTradeSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData,
		CThostFtdcRspInfoField * pRspInfo,int nRequestID,bool bIsLast){
	if(NULL == pDepthMarketData){
		LOG(WARNING) << "OnRspQryDepthMarketData returns NULL";
		m_ev.signal();
		return ;
	}
	if(IsErrorRspInfo(pRspInfo)){
		LOG(WARNING) << "OnRspQryDepthMarketData getError:"<<pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg;
		m_ev.signal();
		return ;
	}
	Application* pApp = m_ctpTradeClient->getApp();
	if(NULL == pApp){
		LOG(ERROR) << "APP is NULL";
		return;
	}
	pApp->updateLastMd(pDepthMarketData);
	if(bIsLast){
		m_ev.signal();
	}
}
void CTPTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin,
									CThostFtdcRspInfoField * pRspInfo,
									int nRequestID,bool bIsLast)
{
	if(NULL == pRspUserLogin){
		LOG(WARNING) << "OnRspUserLogin returns NULL";
		m_ev.signal();
		return ;
	}
	if(IsErrorRspInfo(pRspInfo)){
		LOG(WARNING) << "OnRspUserLogin getError:"<<pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg;
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
		LOG(WARNING) << "OnRspQryInstrument returns NULL";
		m_ev.signal();
		return ;
	}
	if(IsErrorRspInfo(pRspInfo)){
		LOG(WARNING) << "OnRspQryInstrument getError:"<<pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg;
		m_ev.signal();
		return ;
	}
	Application* app = m_ctpTradeClient->getApp();
	if(NULL == app){
		//
		m_ev.signal();
		return;
	}
	std::string strInstrumentID = pInstrument->InstrumentID;
	InstrumentBaseInfo	data;
	memset(&data,0,sizeof(InstrumentBaseInfo));
	stdBase2UserBase(pInstrument,&data);
	app->addInstrumentInfo(strInstrumentID,data);
	if(bIsLast){
		m_ev.signal();
	}
}

void CTPTradeSpi::stdBase2UserBase(CThostFtdcInstrumentField* pSrc,InstrumentBaseInfo* pDst){
	if(NULL == pSrc || NULL == pDst){
		LOG(WARNING)<< "stdBase2UserBase Input Paramete NULL";
		return;
	}
	strncpy(pDst->InstrumentID,pSrc->InstrumentID,sizeof(pDst->InstrumentID));
	char szTypeID[3] = {0};
	strncpy(szTypeID,pDst->InstrumentID,2);
	if(szTypeID[1] >='0' && szTypeID[1] <= '9'){
		szTypeID[1] = 0;
	}
	strncpy(pDst->TypeID,szTypeID,sizeof(pDst->TypeID));
	strncpy(pDst->InstrumentAliasID,pSrc->InstrumentID,sizeof(pDst->InstrumentAliasID));
	strncpy(pDst->ExchangeInstID,pSrc->ExchangeInstID,sizeof(pDst->ExchangeInstID));
	strncpy(pDst->ProductID,pSrc->ProductID,sizeof(pDst->ProductID));
	pDst->ProductClass = pSrc->ProductClass;
	pDst->DeliveryYear = pSrc->DeliveryYear;		//交割年份
	pDst->DeliveryMonth = pSrc->DeliveryMonth;					//交割月
	pDst->MaxMarketOrderVolume = pSrc->MaxMarketOrderVolume;	//市价单最大下单量
	pDst->MinMarketOrderVolume = pSrc->MinMarketOrderVolume;	//市价单最小下单量
	pDst->MaxLimitOrderVolume = pSrc->MaxLimitOrderVolume;		//限价单最大下单量
	pDst->MinLimitOrderVolume = pSrc->MinLimitOrderVolume;		//限价单最小下单量
	pDst->VolumeMultiple = pSrc->VolumeMultiple;		//合约数量乘数
	pDst->PriceTick = pSrc->PriceTick;		//最小变动价位
	strncpy(pDst->CreateDate,pSrc->CreateDate,sizeof(pDst->CreateDate));		//创建日
	strncpy(pDst->OpenDate,pSrc->OpenDate,sizeof(pDst->OpenDate));		//上市日
	strncpy(pDst->ExpireDate,pSrc->ExpireDate,sizeof(pDst->ExpireDate));		//到期日
	strncpy(pDst->StartDelivDate,pSrc->StartDelivDate,sizeof(pDst->StartDelivDate));		//开始交割日
	strncpy(pDst->EndDelivDate,pSrc->EndDelivDate,sizeof(pDst->EndDelivDate));		//结束交割日
	pDst->InstLifePhase = pSrc->InstLifePhase;		//合约生命周期状态
	pDst->IsTrading = pSrc->IsTrading;		//当前是否交易
	pDst->PositionType = pSrc->PositionType;		//持仓类型
	pDst->PositionDateType = pSrc->PositionDateType;		//持仓日期类型
	pDst->LongMarginRatio = pSrc->LongMarginRatio;		//多头保证金率
	pDst->ShortMarginRatio = pSrc->ShortMarginRatio;		//空头保证金率
	pDst->MaxMarginSideAlgorithm = pSrc->MaxMarginSideAlgorithm;		//是否使用大额单边保证金算法
	strncpy(pDst->UnderlyingInstrID,pSrc->UnderlyingInstrID,sizeof(pDst->UnderlyingInstrID));		//基础商品代码
	pDst->StrikePrice = pSrc->StrikePrice;		//执行价
	pDst->OptionsType = pSrc->OptionsType;		//期权类型
	pDst->UnderlyingMultiple = pSrc->UnderlyingMultiple;		//合约基础商品乘数
	pDst->CombinationType = pSrc->CombinationType;		//组合类型
}
