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
	//LOG << "����CTP�ڻ�����������ɹ�";
	m_ctpmdclient->SetMdConnStatus(true);
	m_ev.signal();
}

void CTPMdSpi::OnFrontDisconnected(int nReason){
	//LOG << "<<<<<<<<<<<<<<<<<<<<CTP�ڻ�����������Ͽ�����>>>>>>>>>>>>>>>>>>>>";
	m_ctpmdclient->SetMdLink(FALSE);
}

void CTPMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
							  CThostFtdcRspInfoField *pRspInfo,	
							  int	nRequestID,	
							  bool	bIsLast){
	if (NULL == pRspInfo) {
		//LOG << "�������ӵ�¼��Ӧ����Ϊ��";
	}else {
		if (0 == pRspInfo->ErrorID) {
			//LOG << "<<<<<<<<<<<<<<��¼CTP����������ɹ�>>>>>>>>>>>>>>>>";
			m_ctpmdclient->SetMdLogin(TRUE);
			m_ev.signal();
		}else {
			//LOG << "��¼CTP���������ʧ��,������Ϣ[" << pRspInfo->ErrorID
			//	<< ":" << pRspInfo->ErrorMsg << "]";
		}
	}
}

///�ǳ�������Ӧ
void CTPMdSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout, 
	CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, 
	bool bIsLast){
	if (NULL == pRspInfo) {
		//LOG << "�ǳ������������Ӧ����Ϊ��";
	}else {
		if (0 == pRspInfo->ErrorID){
			//LOG << "�ǳ�����������ɹ�";
			m_ctpmdclient->SetMdLogin(FALSE);
		}else {
			//LOG << "�ǳ�CTP����ǰ�÷�����ʧ��,������Ϣ[" << pRspInfo->ErrorID
				//<< ":" << pRspInfo->ErrorMsg << "]";
		}
	}
}

void CTPMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	if (NULL == pDepthMarketData){
		//LOG << "CTP����Ӧ������Ϊ��";
		return ;
	}else {
		//�ڶ�������֮ǰ�Ѿ����ڴ��б�����һ������
		Application	*pApp = m_ctpmdclient->GetApp();		//APP����
		pApp->update_last_md(pDepthMarketData);		//������������
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
	//LOG << "�յ�CTP�����������������,�����ϴ�����ʱ��[" << nTimeLapse << "]";
}

bool CTPMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo){
	return ((NULL != pRspInfo) && (pRspInfo->ErrorID != 0));
}

