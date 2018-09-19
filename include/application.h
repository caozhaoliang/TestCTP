#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include "./global.h"
#include <iostream>
#include "xmlParse.h"
#include "./include/ctpapi.h"


class CxmlParse;
using namespace std;

class Application{
public:
	Application(CxmlParse* cfg,int nPort);
	bool Init();
	void Start();
	inline CxmlParse* getXmlConfig(){
		return m_CfgXml;
	}
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
	inline int closeClient(CsockClient* pClient){
		if(NULL == pClient){
			return -1;
		}
		int nClientFd = pClient->GetSockFd();
		if(RemoveEpollFd(nClientFd) < 0){
			//error
			return -1;
		}
		std::map<int,CsockClient*>::iterator it = m_clientMap.find(nClientFd);
		if(it == m_clientMap.end()){
			close(nClientFd);
			return 0;	//
		}
		m_clientMap.erase(nClientFd);
		delete pClient;
		pClient=NULL;
		return 0;
	}
	int analyseRequest(int fd, CsockClient* pClient);
	
	Application();
	static void* handleCtpConnet(void*);
	void readCallBack(int fd);
	void writeCallBack(int fd);
	void errorCallBack(int fd);
private:
	int 		m_epollfd;
	int			m_listenfd;
	CxmlParse* 	m_CfgXml;
	int			m_nPort;
	CMDApi*		m_mdApi;
	CTradeApi*	m_TradApi;
	std::map<int,	CsockClient*>		m_clientMap;
	CBlockingQueue						m_TaskQueue;
};

#endif
