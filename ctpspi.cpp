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
	LOG(INFO) << "����CTP�ڻ�����������ɹ�";
	m_ctpmdclient->SetMdConnStatus(true);
	m_ev.signal();
}

void CTPMdSpi::OnFrontDisconnected(int nReason){
	LOG(WARNING) << "CTP�ڻ�����������Ͽ�����";
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

///�ǳ�������Ӧ
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
		LOG(WARNING) << "OnRtnDepthMarketData CTP����Ӧ������Ϊ��";
		return ;
	}
	//�ڶ�������֮ǰ�Ѿ����ڴ��б�����һ������
	Application	*pApp = m_ctpmdclient->GetApp();	//APP����
	if(NULL != pApp){
		pApp->updateLastMd(pDepthMarketData,1);		//������������
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
	//�˶��ɹ�
	//pSpecificInstrument->InstrumentID;
	if(bIsLast ){
		m_ev.signal();
	}
	return;
}

void CTPMdSpi::OnHeartBeatWarning(int nTimeLapse){
	LOG(WARNING)<< "OnHeartBeatWarning �յ�CTP�����������������";
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
	pDst->DeliveryYear = pSrc->DeliveryYear;		//�������
	pDst->DeliveryMonth = pSrc->DeliveryMonth;					//������
	pDst->MaxMarketOrderVolume = pSrc->MaxMarketOrderVolume;	//�м۵�����µ���
	pDst->MinMarketOrderVolume = pSrc->MinMarketOrderVolume;	//�м۵���С�µ���
	pDst->MaxLimitOrderVolume = pSrc->MaxLimitOrderVolume;		//�޼۵�����µ���
	pDst->MinLimitOrderVolume = pSrc->MinLimitOrderVolume;		//�޼۵���С�µ���
	pDst->VolumeMultiple = pSrc->VolumeMultiple;		//��Լ��������
	pDst->PriceTick = pSrc->PriceTick;		//��С�䶯��λ
	strncpy(pDst->CreateDate,pSrc->CreateDate,sizeof(pDst->CreateDate));		//������
	strncpy(pDst->OpenDate,pSrc->OpenDate,sizeof(pDst->OpenDate));		//������
	strncpy(pDst->ExpireDate,pSrc->ExpireDate,sizeof(pDst->ExpireDate));		//������
	strncpy(pDst->StartDelivDate,pSrc->StartDelivDate,sizeof(pDst->StartDelivDate));		//��ʼ������
	strncpy(pDst->EndDelivDate,pSrc->EndDelivDate,sizeof(pDst->EndDelivDate));		//����������
	pDst->InstLifePhase = pSrc->InstLifePhase;		//��Լ��������״̬
	pDst->IsTrading = pSrc->IsTrading;		//��ǰ�Ƿ���
	pDst->PositionType = pSrc->PositionType;		//�ֲ�����
	pDst->PositionDateType = pSrc->PositionDateType;		//�ֲ���������
	pDst->LongMarginRatio = pSrc->LongMarginRatio;		//��ͷ��֤����
	pDst->ShortMarginRatio = pSrc->ShortMarginRatio;		//��ͷ��֤����
	pDst->MaxMarginSideAlgorithm = pSrc->MaxMarginSideAlgorithm;		//�Ƿ�ʹ�ô��߱�֤���㷨
	strncpy(pDst->UnderlyingInstrID,pSrc->UnderlyingInstrID,sizeof(pDst->UnderlyingInstrID));		//������Ʒ����
	pDst->StrikePrice = pSrc->StrikePrice;		//ִ�м�
	pDst->OptionsType = pSrc->OptionsType;		//��Ȩ����
	pDst->UnderlyingMultiple = pSrc->UnderlyingMultiple;		//��Լ������Ʒ����
	pDst->CombinationType = pSrc->CombinationType;		//�������
}
