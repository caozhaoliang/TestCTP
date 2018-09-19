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


