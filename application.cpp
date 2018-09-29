#include "./include/application.h"
#include <pthread.h>


/*
bool		m_bCodeUpdate;
	bool		m_bCommRateUpdate;
	bool		m_bTradeDayChange;
	bool		m_bMDSubcribe;
	bool		m_bMDNeedReSubcr;
	bool		m_bServerIniting;

*/
Application::Application(){
}
Application::Application(const CxmlParse* cfgxml,int nPort)
{
	m_bCodeUpdate = false;
	m_bCommRateUpdate = false;
	m_bTradeDayChange = true;
	m_bMDSubcribe = false;
	m_bMDNeedReSubcr = true;
	m_bServerIniting = true;
	m_CfgXml = cfgxml;
	m_nPort = nPort;
	m_pMDApi = NULL;
	m_pTradeApi = NULL;
	Init();
}
Application::~Application(){


	if(m_pTradeApi != NULL){
		delete m_pTradeApi;
		m_pTradeApi = NULL;
	}
}

bool Application::Init(){
	
	pthread_t pid[4];
	int nPid = pthread_create(&pid[0],NULL,handleCtpConnet,this);
	if(0 != nPid){
		return false;
	}	
}

bool Application::InitTradeClient(){
	bool bRet = false;
	std::string strPassword = getXmlConfig()->getConfig("CTPCfg.Password");
	std::string strUserID = getXmlConfig()->getConfig("CTPCfg.UserID");
	std::string strBrokerID = getXmlConfig()->getConfig("CTPCfg.BrokerID");
	std::string strFrontTradeAddr = getXmlConfig()->getConfig("CTPCfg.TradeAddr");
	// "tcp://192.168.111.222:8080" - "tcp://1.1.1.1:90"
	if(strFrontTradeAddr.empty() || strFrontTradeAddr.length() <16 ){
	//print strFrontTradeAddr
		delete m_pTradeApi;
		m_pTradeApi = NULL;
		return false;
	}
	std::vector<std::string> vAddr;
	vAddr.push_back(strFrontTradeAddr);
	if(m_pTradeApi == NULL){
		m_pTradeApi = new CTradeApi(this);
		CThostFtdcReqUserLoginField fldReqLogin;
		memset(&fldReqLogin,0,sizeof(CThostFtdcReqUserLoginField));
		strncpy(fldReqLogin.BrokerID,strBrokerID.c_str(),sizeof(fldReqLogin.BrokerID));
		strncpy(fldReqLogin.UserID,strUserID.c_str(),sizeof(fldReqLogin.UserID));
		strncpy(fldReqLogin.Password,strPassword.c_str(),sizeof(fldReqLogin.Password));
		m_pTradeApi->setReqLoginParam(&fldReqLogin);

		CThostFtdcUserLogoutField fldReqLogout;
		memset(&fldReqLogout,0,sizeof(CThostFtdcUserLogoutField));
		strncpy(fldReqLogout.UserID,strUserID.c_str(),sizeof(fldReqLogout.UserID));
		strncpy(fldReqLogout.BrokerID,strBrokerID.c_str(),sizeof(fldReqLogout.BrokerID));
		m_pTradeApi->setReqLogoutParam(fldReqLogout);

		CThostFtdcQryInstrumentField fldReqCode;
		memset(&fldReqCode,0,sizeof(CThostFtdcQryInstrumentField));
		m_pTradeApi->setQryCodeParam(fldReqCode);

		CThostFtdcQryDepthMarketDataField fldReqMd;
		memset(&fldReqMd,0,sizeof(CThostFtdcQryDepthMarketDataField));
		m_pTradeApi->setQryMarketDataParam(fldReqMd);

		CThostFtdcQryInstrumentCommissionRateField fldReqComm;
		memset(&fldReqComm,0,sizeof(CThostFtdcQryInstrumentCommissionRateField));
		strncpy(fldReqComm.BrokerID,strBrokerID.c_str(),sizeof(fldReqComm.BrokerID));
		strncpy(fldReqComm.InvestorID,strUserID.c_str(),sizeof(fldReqComm.InvestorID));
		m_pTradeApi->setQryInsCommRateParam(fldReqComm);		
	}
	if(!m_pTradeApi->getCTPConnStatus()){
		if(!m_pTradeApi->ConnCTPServer(vAddr)){
			//Error 连接失败
			delete m_pTradeApi;
			m_pTradeApi = NULL;
			return false;
		}
		m_pTradeApi->setCTPConnStatus(true);
		bRet = true;
	}
	if(bRet){
		if(!m_pTradeApi->getCTPLoginStatus()){
			if(0 != m_pTradeApi->CacheLogin()){
				//Error Login 
				m_pTradeApi->setCTPLoginStatus(false);
				bRet = false;
			}else{
				m_pTradeApi->setCTPLoginStatus(true);
				if(m_nTradeDate<m_pTradeApi->getTradeDay()){
					m_nTradeDate = m_pTradeApi->getTradeDay();
					m_bTradeDayChange = true;
					m_bCodeUpdate = false;
					m_bCommRateUpdate = false;
				}else{
					m_bTradeDayChange = false;
				}
			}
			
		}
	}
	return bRet;
}
bool Application::qryInstrumentList(){
	bool bRet = false;
	if(!InitTradeClient()){
		//LOG init trade api failed 
		return bRet;
	}
	if(m_bTradeDayChange){
		if(0 == m_pTradeApi->CacheQryCode()){
			//LOG qryInstrumentList success 
			bRet = true;
		}else{
			//LOG failed
		}
	}else{
		//LOG needn't update 
		bRet = true;
	}
	return bRet;
}

static void* handleCtpConnet(void* arg){
	//建立并维持与CTP trade Md 的链接并且实时获取行情数据
	Application* app = (Application*)arg;
	while(1){
		if(app->m_bCodeUpdate && app->m_bCommRateUpdate && CTime::IsWeekEnd(2.5*60*60)){
			// 
			sys::sleep(60);
			continue;
		}
		if(app->m_bTradeDayChange || !app->m_bCodeUpdate){
			app->m_bServerIniting = true;
			while(1){
				if((app->m_bCodeUpdate = app->qryInstrumentList())==true){
					break;
				}
				sys::sleep(1);
			}
			//拉取码表完成开始查询行情快照
			sys::sleep(1);
			
		}
	
	}
	return NULL;
}

void Application::Start(){
	// 1 创建epoll侦听端口9091 
	m_epollfd = epoll_create(1024);
	if(m_epollfd <0){
		// error 
		return ;
	}
	m_listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(m_listenfd == -1){
		// errno strerror(errno)
		return ;
	}
	int nLen =1;
	setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,(char*)&nLen,sizeof(nLen));
	setsockopt(m_listenfd,IPPROTO_TCP,TCP_NODELAY,(char*)&nLen,sizeof(nLen));

	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port =htons(m_nPort);
	addr.sin_addr.s_addr = INADDR_ANY;	//本机上所有IP
	if(bind(m_listenfd,(struct sockaddr*)&addr,sizeof(addr))<0){
		//error 
		close(m_listenfd);
		m_listenfd = -1;
		return;
	}
	if(listen(m_listenfd,1024)<0){
		//error
		close(m_listenfd);
		return;
	}
	epoll_event ev;
	epoll_event events[1024];
	ev.data.fd = m_listenfd;
	ev.events = EPOLLIN;	//EPOLLLT 水平触发 默认
	epoll_ctl(m_epollfd,EPOLL_CTL_ADD,m_listenfd,&ev);
	while(1){
		int nCount = epoll_wait(m_epollfd,events,1024,100);
		for(int i=0;i<nCount;++i){
			if(events[i].data.fd == m_listenfd){
				struct sockaddr_in clientAddr;
				socklen_t nLenth = sizeof(struct sockaddr);
				int clientfd=accept(m_listenfd,(struct sockaddr*)&clientAddr,&nLenth);
				if(clientfd < 0){
					//error should I juage errno is EINTR
					return;
				}
				CsockClient* client=new CsockClient();
				char* szAddr=inet_ntoa(clientAddr.sin_addr);
				int nPort =ntohs(clientAddr.sin_port);
				client->SetAddr(szAddr,nPort);
				client->SetConnTime(time(NULL));
				client->SetSockFd(clientfd);
				m_clientMap.insert(make_pair(clientfd,client));
				
				ev.data.fd = clientfd;
				ev.events=EPOLLIN;
				epoll_ctl(m_epollfd,EPOLL_CTL_ADD,clientfd,&ev);
			}else if(events[i].events& EPOLLIN){
				readCallBack(events[i].data.fd);			
			}else if(events[i].events&EPOLLOUT){
				writeCallBack(events[i].data.fd);
			}else if(events[i].events & EPOLLERR || events[i].events & EPOLLHUP){
				errorCallBack(events[i].data.fd);
			}
		}

	}
	
	
}
void Application::readCallBack(int fd){
	char buffer[1024*2] = {0};
	int nCount=0;
	std::map<int,CsockClient*>::iterator it = m_clientMap.find(fd);
	if(it == m_clientMap.end()){
		//error 
		return ;
	}
	CsockClient* pClient = it->second;
	char* pClientIP = pClient->GetClientIP();
	int nPort = pClient->GetClientPort();
	nCount = recv(fd,buffer,1024*2,0);
	if(nCount < 0){
		if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR){
			RegistReadEvent(fd);
		}else{
			//error 
			closeClient(pClient);
		}
	}else if(nCount == 0){
		// close by peer 
		closeClient(pClient);
	}else{
		CBuffer* pInBuff = pClient->rBuffer();
		pInBuff->Write(buffer,nCount);
		analyseRequest(fd,pClient);
	}
	return;
}
void Application::writeCallBack(int fd){
	int nCount=0;
	std::map<int,CsockClient*>::iterator it = m_clientMap.find(fd);
	if(it == m_clientMap.end()){
		//error 
		return ;
	}
	CsockClient* pClient = it->second;
	//char* pClientIP = pClient->GetClientIP();
	//int nPort = pClient->GetClientPort();
	if(NULL == pClient){
		//error
		m_clientMap.erase(it);
		return;
	}
	CBuffer* pOutBuf = pClient->wBuffer();
	if(pOutBuf->GetDataLength() > 0){
		nCount = ::send(fd,(char*)pOutBuf->GetBuffer(),pOutBuf->GetDataLength(),0);
		if(nCount < 0){
			if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR){
				RegistWriteEvent(fd);
			}else{
				//error 
				closeClient(pClient);
			}
		}else if(nCount == 0){
			// close by peer 
			closeClient(pClient);
			return ;
		}else{
			pOutBuf->Delete(nCount);
		}
	}
	if(pOutBuf->GetDataLength() == 0){
		pOutBuf->Compress();
	}else{
		RegistWriteEvent(fd);
	}
	return;
}
int Application::analyseRequest(int fd, CsockClient* pClient){
	Json::Value jsonObj;
	Json::Reader jsonReader;
	std::string szbuff = pClient->rBuffer()->ReadLine();	//读取一行 '\n'结束
	if(!jsonReader.parse(szbuff,jsonObj)){
		//收到的数据不是一个Json数据
		return  -1;
	}
	int nFuncId = atoi(jsonObj["FuncID"].asString().c_str());
	switch(nFuncId){
		case 10003:		//登录
			{
				if(pClient->GetLoginStatus()){
					// 已经登录了不需要再次登录
					return -1;
				}
				std::string strUserID = jsonObj["UserID"].asString();
				std::string strBrokerID = jsonObj["BrokerID"].asString();
				std::string strPassword = jsonObj["Password"].asString();
				if(strUserID.empty || strBrokerID.empty()|| strPassword.empty()){
					return -1;
				}
				Task* t = new Task;
				t->nSockFd = pClient->GetSockFd();
				t->nFuncID =  10003;
				t->strUserID = strUserID;
				t->strBrokerID=strBrokerID;
				t->strPassword=strPassword;
				m_TaskQueue.Push(t);
				
			}
			break;
		case 10004:			//修改密码
			{
				//必须要登录成功才能修改密码
				std::string strUserID = jsonObj["UserID"].asString();
				std::string strBrokerID = jsonObj["BrokerID"].asString();
				std::string strNewPassword = jsonObj["NewPassword"].asString();
				std::string strOldPassword = jsonObj["OldPassword"].asString();
				if(strUserID.empty || strBrokerID.empty()|| strNewPassword.empty()|| strOldPassword.empty()){
					return -1;
				}
				Task* t = new Task;
				t->nSockFd=pClient->GetSockFd();
				t->nFuncID=10004;
				t->strUserID = strUserID;
				t->strBrokerID=strBrokerID;
				t->strNewPassword=strNewPassword;
				t->strOldPassword=strOldPassword;
				m_TaskQueue.Push(t);
			}
			break;
		case 10005:			//登出
			{
				//必须要登录 成功才能登出
				std::string strUserID = jsonObj["UserID"].asString();
				std::string strBrokerID = jsonObj["BrokerID"].asString();
				if(strUserID.empty || strBrokerID.empty()){
					return -1;
				}
				
			}

		default:
			break;
	}
}
