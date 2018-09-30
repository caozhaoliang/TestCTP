#ifndef _FTD_CTPSPI_H_
#define _FTD_CTPSPI_H_
#include "./global.h"
class CMDApi;
class CTradeApi;

class CTPMdSpi:public CThostFtdcMdSpi{
	public:
		CTPMdSpi(CMDApi *pParent,CThostFtdcMdApi *pUserApi,SyncEvent &ev);
		~CTPMdSpi();
	
		//当客户端与交易托管系统建立起通信连接，客户端需要进行登录
		virtual void OnFrontConnected();
	
		//当客户端与交易托管系统通信连接断开时，该方法被调用
		virtual void OnFrontDisconnected(int	nReason);
	
		//当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
		virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
			CThostFtdcRspInfoField *pRspInfo,int	nRequestID,bool bIsLast);
	
		//当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
		virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast);
	
		//行情应答
		virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	
		//订阅行情应答
		virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
			CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);
	
		//针对客户请求的出错通知
		virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast);
	
		virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
			CThostFtdcRspInfoField *pRspInfo, int nRequestID,bool bIsLast);
	
		virtual void OnHeartBeatWarning(int nTimeLapse);
	
		//错误判断
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
