#ifndef __CTPAPI_H__
#define __CTPAPI_H__
#include "./global.h"
#include "./application.h"
#include "./ctpmdspi.h"

class Application;
class CTPMdSpi;

class CMDApi{
public:
	CMDApi(Application* app);
	~CMDApi();
	bool Connect2MdServer(const std::vector<std::string>&);
	bool LoginMdServer();
	int LogoutMdServer();
	bool SubscriberMd(std::map<std::string,InstrumentBaseInfo>&);
	inline void SetMdConnStatus(bool status){
		m_bConnStatus = status;
	}
	inline bool GetMdConnStatus(){
		return m_bConnStatus;
	}

private:
	Application* 			m_App;
	CThostFtdcMdApi*		m_mdApi;
	CTPMdSpi*				m_mdSpi;
	SyncEvent				m_ev;
	bool 					m_bConnStatus;
	
	std::string				m_strFlowPath;
};

class CTradeApi{
public:
	explicit CTradeApi(Application* pApp);
	~CTradeApi();
	inline bool getCTPConnStatus(){
		return m_bConnStatus;
	}
	inline bool setCTPConnStatus(bool bStatus){
		m_bConnStatus = bStatus;
	}
	inline bool getCTPLoginStatus(){
		return m_bLoginStatus;
	}
	inline bool setCTPLoginStatus(bool bStatus){
		m_bLoginStatus= bStatus;
	}
	inline void setReqLoginParam(CThostFtdcReqUserLoginField* fldReqLogin){
		memmove(&m_ReqLogin,fldReqLogin,sizeof(CThostFtdcReqUserLoginField));
		m_strUserID = m_ReqLogin.UserID;
	}
	inline void setQryCodeParam(CThostFtdcQryInstrumentField* fldQryCode){
		memmove(&m_ReqCode,fldQryCode,sizeof(CThostFtdcQryInstrumentField));
	}
	inline void setQryMarketDataParam(CThostFtdcQryDepthMarketDataField* fldQryMD){
		memmove(&m_ReqMd,fldQryMD,sizeof(CThostFtdcQryDepthMarketDataField));
	}
	inline void setQryInsCommRateParam(CThostFtdcQryInstrumentCommissionRateField* fldQryComm){
		memmove(&m_ReqCommission,fldQryComm,sizeof(CThostFtdcQryInstrumentCommissionRateField));
	}
	inline void setReqLogoutParam(CThostFtdcUserLogoutField* fldReqLogout){
		memmove(&m_ReqLogout,fldReqLogout,sizeof(CThostFtdcUserLogoutField));
	}
public:
	int CacheLogin();
private:
	Application* 					m_App;
	CThostFtdcTraderApi*			m_pTraderApi;
	CTPTradeSpi*					m_pTraderSpi;
	SyncEvent						m_ev;
	std::string 					m_strFlowPath;
	std::string						m_strUserID;
	bool							m_bConnStatus;
	bool							m_bLoginStatus;
	//CThostFtdcReqAuthenticateField					m_Authen;				//请求认证参数
	CThostFtdcReqUserLoginField						m_ReqLogin;				//请求登录参数
	CThostFtdcQryInstrumentField					m_ReqCode;				//请求查询合约列表
	CThostFtdcQryDepthMarketDataField				m_ReqMd;				//请求查询合约行情
	CThostFtdcQryInstrumentCommissionRateField		m_ReqCommission;	//请求查询合约手续费
	CThostFtdcUserLogoutField						m_ReqLogout;
};

#endif
