#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include "global.h"
#include <iostream>
#include "xmlParse.h"
#include "ctpapi.h"
#include "sockClient.h"
#include "contain.h"

class CxmlParse;
class CMDApi;
class CTradeApi;
using namespace std;
    
void* handleCtpConnet(void* arg);

class Application{
public:
	Application(CxmlParse* cfg,int nPort);
	~Application();
	bool Init();
	void Start();
	inline CxmlParse* getXmlConfig(){
		return m_CfgXml;
	}
	bool Connect2Trade(CTradeApi* pTradeApi);
    bool InitTradeClient();
	bool qryInstrumentList();
	bool qryDeepMarketData();
	inline void addInstrumentInfo(std::string strInstrumentID,InstrumentBaseInfo baseInfo){
		m_InstrumentInfo.insert(std::make_pair(strInstrumentID,baseInfo));
	}
	void updateLastMd(CThostFtdcDepthMarketDataField* pData,int _type = 0);
	bool loadCommRateFile();
	bool subscribeMd(bool& bCodeEmpty);
	bool qryCommissionRate();	
	bool mdConnstatus();
	inline void resetTradeClient(){
		if(NULL != m_pTradeApi){
			delete m_pTradeApi;
			m_pTradeApi = NULL;
		}
	}
	bool NeedUpdate();
public:
	bool		m_bCodeUpdate;
	bool		m_bCommRateUpdate;
	bool		m_bTradeDayChange;
	bool		m_bMDSubcribe;
	bool		m_bMDNeedReSubcr;
	bool		m_bServerIniting;
	int 		m_nTradeDate;
	
private:
	inline int RegistReadEvent(int fd){
		epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLIN| EPOLLONESHOT;
		return epoll_ctl(m_epollfd,EPOLL_CTL_MOD,fd,&ev);
	}
	inline int RegistWriteEvent(int fd){
		epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLOUT| EPOLLONESHOT;
		return epoll_ctl(m_epollfd,EPOLL_CTL_MOD,fd,&ev);
	}
	inline int RemoveEpollFd(int fd){
		epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLIN;
		return epoll_ctl(m_epollfd,EPOLL_CTL_DEL,fd,&ev);
	}
	int closeClient(CsockClient* pClient);
	int analyseRequest(int fd, CsockClient* pClient);
	
	Application();
	//static void* handleCtpConnet(void*);
	void readCallBack(int fd);
	void writeCallBack(int fd);
	void errorCallBack(int fd);
private:
	int 		m_epollfd;
	int			m_listenfd;
	CxmlParse* 	m_CfgXml;
	int			m_nPort;
	CMDApi*		m_pMDApi;
	CTradeApi*	m_pTradeApi;
	std::map<std::string,InstrumentBaseInfo> 					m_InstrumentInfo;
	std::map<std::string,CThostFtdcDepthMarketDataField> 		m_LastMd;
	std::map<std::string,CommissionChargeInfo>					m_CommRate;
	std::map<int,	CsockClient*>		m_clientMap;
	CBlockingQueue						m_TaskQueue;
};

#endif
