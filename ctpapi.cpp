#include "./include/ctpapi.h"
#include "./include/xmlParse.h"

#define CACHE_MD_PATH "./MdFlow/"
class CxmlParse;

CMDApi::CMDApi(Application* app){
	m_App = app;
	
	std::string strMdAddr = app->getXmlConfig().getConfig("Server.MdAddr");
	std::vector<std::string> vMdAddr;
	vMdAddr.push_back(strMdAddr);
	Connect2MdServer(vMdAddr);
}
CMDApi::~CMDApi(){
	if(NULL != m_mdApi){
		m_mdApi->RegisterSpi(NULL);
		m_mdApi->Release();
		m_mdApi=NULL;
	}
	if(NULL != m_mdSpi){
		delete m_mdSpi;
		m_mdSpi = NULL;
	}
	if(!m_strFlowPath.empty()){
		rmdirs(m_strFlowPath);
	}
}
bool CMDApi::Connect2MdServer(const std::vector<std::string>& vAddr){
	if(vAddr.empty()){
		return false;
	}
	std::string strFlowPath = CACHE_MD_PATH;
	m_strFlowPath = strFlowPath;
	if(!dirExist(strFlowPath)){
		makedirs(strFlowPath);
	}else{
		if(!rmdirs(strFlowPath)){
			//ÒÆ³ýÊ§°Ü
		}else{
			makedirs(strFlowPath);
		}
	}
	m_mdApi = CThostFtdcMdApi::CreateFtdcMdApi(strFlowPath.c_str());
	if(NULL == m_mdApi){
		return false;
	}
	m_mdSpi = new CTPMdSpi(this,m_mdApi,m_ev);
	m_mdApi->RegisterSpi(m_mdSpi);
	for(std::vector<std::string>::const_iterator it = vAddr.begin();
			it != vAddr.end();++it){
		char szAddr[32]={0};
		snprintf(szAddr,sizeof(szAddr),"%s",it->c_str());
		m_mdApi->RegisterFront(szAddr);
	}
	m_mdApi->Init();
	if(!m_ev.timed_wait(60000)){
		return false;
	}
	return true;			
}

CTradeApi::CTradeApi(Application * pApp):m_App(pApp)
{
	m_bConnStatus = false;
	m_bLoginStatus = false;
	m_pTraderApi = NULL;
	m_pTraderSpi = NULL;
}
CTradeApi::~CTradeApi(){
	m_bConnStatus=false;
	m_bLoginStatus = false;
	if(m_pTraderApi != NULL){
		m_pTraderApi->RegisterSpi(NULL);
		m_pTraderApi->Release();
		m_pTraderApi = NULL;
	}
	if(m_pTraderSpi != NULL){
		delete m_pTraderSpi;
		m_pTraderSpi = NULL;
	}
	if(m_strFlowPath.length() > 0){
		rmdirs(m_strFlowPath);
	}
}
int CTradeApi::CacheLogin(){
	if(NULL == m_pTraderApi){
		return -1;
	}
	int nRet = m_pTraderApi->ReqUserLogin(&m_ReqLogin,10001);
	if(0 == nRet){
		if(!m_ev.timed_wait(5000)){
			//Error Timeout
			return -1;
		}
		//success
	}else{
		//LOG  Error with nRet 
	}
	return nRet;
}
bool CTradeApi::ConnCTPServer(std::vector<std::string>& vAddr){
	if(vAddr.empty()){
		return false;
	}
	m_strFlowPath = "./CacheTrade/";
	if(!dirExist(m_strFlowPath)){
		makedirs(m_strFlowPath);
	}else{
		if(!rmdirs(m_strFlowPath)){
			//Error
		}else{
			makedirs(m_strFlowPath);
		}
	}
	m_pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(m_strFlowPath.c_str());
	if(m_pTraderApi == NULL){
		//Error
		return false;
	}
	m_pTraderSpi = new CTPTradeSpi(m_pTraderApi,this,m_ev);
	if(NULL == m_pTraderSpi){
		//Error
		return false;
	}
	m_pTraderApi->RegisterSpi(m_pTraderSpi);
	m_pTraderApi->SubscribePublicTopic(2);
	m_pTraderApi->SubscribePrivateTopic(2);
	for(std::vector<std::string>::iterator it = vAddr.begin();it!=vAddr.end();++it){
		m_pTraderApi->RegisterFront((char*)it->c_str());
	}
	m_pTraderApi->Init();

	if(!m_ev.timed_wait(5000)){
		//Error
		return false;
	}
	return true;
}