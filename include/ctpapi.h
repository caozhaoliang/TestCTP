#ifndef __CTPAPI_H__
#define __CTPAPI_H__
#include "global.h"
#include "application.h"
#include "ctpspi.h"

class Application;
class CTPMdSpi;

class CMDApi{
public:
	CMDApi(Application* app);
	~CMDApi();
	bool Connect2MdServer(const std::vector<std::string>&);
	bool LoginMdServer();
	bool LogoutMdServer();
	bool SubscriberMd(std::vector<std::string>& vCode);

	inline Application* GetApp(){
		return m_App;
	}
	inline void SetMdConnStatus(bool status){
		m_bConnStatus = status;
	}
	inline bool GetMdConnStatus(){
		return m_bConnStatus;
	}
	inline void SetMdLoginStatus(bool status){
		m_bLoginStatus = status;
	}
	inline bool GetMdLoginStatus(){
		return m_bLoginStatus;
	}

private:
	Application* 			m_App;
	CThostFtdcMdApi*		m_mdApi;
	CTPMdSpi*				m_mdSpi;
	SyncEvent				m_ev;
	bool 					m_bConnStatus;
	bool					m_bLoginStatus;
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
	inline void setTradeDay(int nTradeDate){
		m_nTradeDate = nTradeDate;
	}
	inline int getTradeDay(){
		return m_nTradeDate;
	}
	inline Application* getApp(){
		return m_App;
	}
public:
	const char* getVersion();
	int CacheLogin();
	int CacheQryCode();
	int CacheQryMd();
    bool ConnCTPServer(std::vector<std::string>& vAddr);
private:
	Application* 					m_App;
	CThostFtdcTraderApi*			m_pTraderApi;
	CTPTradeSpi*					m_pTraderSpi;
	SyncEvent						m_ev;
	std::string 					m_strFlowPath;
	std::string						m_strUserID;
	int								m_nTradeDate;
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
