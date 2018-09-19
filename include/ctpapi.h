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
	
};

#endif
