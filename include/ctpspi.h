#ifndef _FTD_CTPSPI_H_
#define _FTD_CTPSPI_H_
#include "./global.h"
class CMDApi;
class CTradeApi;

class CTPMdSpi:public CThostFtdcMdSpi{
	public:
		CTPMdSpi(CMDApi *pParent,CThostFtdcMdApi *pUserApi,SyncEvent &ev);
		~CTPMdSpi();
	
		//���ͻ����뽻���й�ϵͳ������ͨ�����ӣ��ͻ�����Ҫ���е�¼
		virtual void OnFrontConnected();
	
		//���ͻ����뽻���й�ϵͳͨ�����ӶϿ�ʱ���÷���������
		virtual void OnFrontDisconnected(int	nReason);
	
		//���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
		virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
			CThostFtdcRspInfoField *pRspInfo,int	nRequestID,bool bIsLast);
	
		//���ͻ��˷�����¼����֮�󣬸÷����ᱻ���ã�֪ͨ�ͻ��˵�¼�Ƿ�ɹ�
		virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast);
	
		//����Ӧ��
		virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	
		//��������Ӧ��
		virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
			CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);
	
		//��Կͻ�����ĳ���֪ͨ
		virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast);
	
		virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
			CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast);
	
		virtual void OnHeartBeatWarning(int nTimeLapse);
	
		//�����ж�
		bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	
	private:
		CMDApi		 		*m_ctpmdclient;
		CThostFtdcMdApi 	*m_mdapi;
		SyncEvent 			&m_ev;

};

class CTPTradeSpi:public CThostFtdcTraderSpi{
public:
	CTPTradeSpi(CThostFtdcTraderApi* pClientApi,CTradeApi* pClient,SyncEvent& ev);
	~CTPTradeSpi();
	bool IsErrorRspInfo(CThostFtdcRspInfoField * pRspInfo);
protected:
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField * pRspUserLogin,CThostFtdcRspInfoField * pRspInfo,int nRequestID,bool bIsLast);
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField * pInstrument,CThostFtdcRspInfoField * pRspInfo,int nRequestID,bool bIsLast);
	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField * pDepthMarketData,CThostFtdcRspInfoField * pRspInfo,int nRequestID,bool bIsLast);

	
private:
	CThostFtdcTraderApi*	m_traderApi;
	CTradeApi* 				m_ctpTradeClient;
	SyncEvent&				m_ev;
};


#endif
