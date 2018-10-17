#include "./include/application.h"
#include <pthread.h>
#include "./include/ctime.h"

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
Application::Application(CxmlParse* cfgxml,int nPort)
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
		LOG(INFO) << "pthread_create failed nRet:"<<nPid;
		return false;
	}	
	return true;
}

bool Application::InitTradeClient(){
	bool bRet = false;
	std::string strPassword = getXmlConfig()->getConfig("CTPCfg.Password");
	std::string strUserID = getXmlConfig()->getConfig("CTPCfg.UserID");
	std::string strBrokerID = getXmlConfig()->getConfig("CTPCfg.BrokerID");
	std::string strFrontTradeAddr = getXmlConfig()->getConfig("CTPCfg.TradeAddr");
	// "tcp://192.168.111.222:8080" - "tcp://1.1.1.1:90"
	LOG(INFO) << strUserID<<":"<<strPassword<<":"<<strBrokerID<<":"<<strFrontTradeAddr;
    if(strFrontTradeAddr.empty() || strFrontTradeAddr.length() <16 ){
	//print strFrontTradeAddr
		LOG(ERROR) <<"config.xml TradeAddr is empty";
	//	delete m_pTradeApi;
	//	m_pTradeApi = NULL;
	//	return false;
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
		m_pTradeApi->setReqLogoutParam(&fldReqLogout);

		CThostFtdcQryInstrumentField fldReqCode;
		memset(&fldReqCode,0,sizeof(CThostFtdcQryInstrumentField));
		m_pTradeApi->setQryCodeParam(&fldReqCode);

		CThostFtdcQryDepthMarketDataField fldReqMd;
		memset(&fldReqMd,0,sizeof(CThostFtdcQryDepthMarketDataField));
		m_pTradeApi->setQryMarketDataParam(&fldReqMd);

		CThostFtdcQryInstrumentCommissionRateField fldReqComm;
		memset(&fldReqComm,0,sizeof(CThostFtdcQryInstrumentCommissionRateField));
		strncpy(fldReqComm.BrokerID,strBrokerID.c_str(),sizeof(fldReqComm.BrokerID));
		strncpy(fldReqComm.InvestorID,strUserID.c_str(),sizeof(fldReqComm.InvestorID));
		m_pTradeApi->setQryInsCommRateParam(&fldReqComm);		
	}
	if(!m_pTradeApi->getCTPConnStatus()){
		if(!m_pTradeApi->ConnCTPServer(vAddr)){
			//Error 连接失败
			LOG(ERROR) <<"Connect to trade front server failed";
			delete m_pTradeApi;
			m_pTradeApi = NULL;
			return false;
		}
		m_pTradeApi->setCTPConnStatus(true);
		bRet = true;
	}
	if(m_pTradeApi->getCTPConnStatus()){
		if(!m_pTradeApi->getCTPLoginStatus()){
			if(0 != m_pTradeApi->CacheLogin()){
				//Error Login 
				LOG(ERROR) << "Login to trade front failed";
				m_pTradeApi->setCTPLoginStatus(false);
				bRet = false;
			}else{
				LOG(INFO) << "Login to trade front success";
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
			
		}else{
			bRet = true;//已经登录了
		}
	}
	return bRet;
}
bool Application::qryInstrumentList(){
	bool bRet = false;
	if(!InitTradeClient()){
		LOG(ERROR) << "init trade api failed"; 
		return bRet;
	}
	if(m_bTradeDayChange){
		if(0 == m_pTradeApi->CacheQryCode()){
			LOG(INFO)<<" qryInstrumentList success"; 
			bRet = true;
		}else{
			LOG(ERROR)<<"qryInstrumentList failed";
			//LOG failed
		}
	}else{
		LOG(INFO) <<"tradeday not change. needn't update";
		//LOG needn't update 
		bRet = true;
	}
	return bRet;
}
bool Application::subscribeMd(bool& bCodeEmpty){
	if(NULL == m_pMDApi){
		m_pMDApi = new CMDApi(this);
	}else{
		m_pMDApi->LogoutMdServer();
		delete m_pMDApi;
		m_pMDApi = NULL;
		m_pMDApi = new CMDApi(this);
	}
	bool bRet = true;
	if(!m_pMDApi->GetMdConnStatus()){
		std::vector<std::string> vMdAddr;
		std::string strMdAddr = m_CfgXml->getConfig("CTPCfg.MdAddr");
		vMdAddr.push_back(strMdAddr);
		if(!m_pMDApi->Connect2MdServer(vMdAddr)){
			LOG(WARNING) << "connect to Martet Front server failed";
			bRet =  false;
		}else{
			bRet = true;
		}
	}
	if(bRet && m_pMDApi->GetMdConnStatus()&& !m_pMDApi->GetMdLoginStatus()){
		if(!m_pMDApi->LoginMdServer()){
			LOG(WARNING)<<"LoginMdServer failed";
			bRet = false;
		}else{
			bRet = true;
		}
	}
	if(bRet){
		std::vector<std::string> vNeedSubcriCode;
		std::map<std::string,InstrumentBaseInfo>::iterator it = m_InstrumentInfo.begin();
		for(;it != m_InstrumentInfo.end(); ++it){
			char chProductClass = it->second.ProductClass;
			if(chProductClass == '1' || chProductClass == '2' || chProductClass == '6'){
				vNeedSubcriCode.push_back(it->first);
			}
		}
		bCodeEmpty = (vNeedSubcriCode.size() <= 0);
		if(!m_pMDApi->SubscriberMd(vNeedSubcriCode)){
			bRet = false;
			LOG(WARNING) <<"SubscriberMd failed";
		}
	}
	if(!bRet){
		m_pMDApi->LogoutMdServer();
		delete m_pMDApi;
		m_pMDApi = NULL;
	}
	return bRet;
}

bool Application::qryDeepMarketData(){
	bool bRet = false;
	if(NULL != m_pMDApi){
		//m_pMDApi->LogoutMdServer();
		LOG(ERROR) << "when aryDeepMarketData CTP Md API was empty";
		delete m_pMDApi;
		m_pMDApi = NULL;
	}
	if(!InitTradeClient()){
		LOG(ERROR) << "InitTradeClient failed";
		return false;
	}
	if(0 == m_pTradeApi->CacheQryMd()){
		LOG(INFO) << "CacheQryMd success";
		return true;
	}
	LOG(WARNING) << "CacheQryMd failed";
	return false;
}

bool Application::qryCommissionRate(){
	bool bRet = false;
	
}

bool Application::mdConnstatus(){
	if(NULL == m_pMDApi){
		return false;
	}
	if(!m_pMDApi->GetMdConnStatus()){
		return false;
	}
	if(!m_pMDApi->GetMdLoginStatus()){
		return false;
	}
	return true;
}
void* handleCtpConnet(void* arg){
	//建立并维持与CTP trade Md 的链接并且实时获取行情数据
	Application* app = (Application*)arg;
	//do not qry CommRate
	app->m_bCommRateUpdate = true;
	while(1){
		if(app->m_bCodeUpdate && app->m_bCommRateUpdate && CTime::IsWeekEnd(2.5*60*60)){
			LOG(INFO) <<"weekend not querry Market Data.";
			sys::sleep(60);
			continue;
		}
		if(app->m_bTradeDayChange || !app->m_bCodeUpdate){
			app->m_bServerIniting = true;
			while(1){
				if((app->m_bCodeUpdate = app->qryInstrumentList())==true){
					LOG(INFO) << "query instrument list finished";
					break;
				}
				sys::sleep(1);
			}
			//拉取码表完成开始查询行情快照
			LOG(INFO) << "[初始化合约列表] 拉取合约列表完成";
			sys::sleep(1);
			if(!app->qryDeepMarketData()){
				LOG(ERROR)<<" get deep Market Data failed";
				continue;
			}
			app->m_bServerIniting = false;
		}
		sys::sleep(1);
		bool bMdConnStatus = app->mdConnstatus();
		if(!bMdConnStatus || !app->m_bMDSubcribe ||app->m_bTradeDayChange || app->m_bMDNeedReSubcr){
			app->m_bServerIniting = true;
			bool bCodeEmpty = false;
			for(int i =0; i <3;){
				if((app->m_bMDSubcribe = app->subscribeMd(bCodeEmpty)) == true){
					LOG(INFO) << "subscibe success";
					app->m_bServerIniting = false;
					app->m_bMDNeedReSubcr = false;
					break;
				}
				if(3 == ++i){
					app->resetTradeClient();
				}
				
			}
			if(bCodeEmpty){
				LOG(WARNING) << "Subscribe Market data failed";
				app->m_bCodeUpdate = false;
				continue;
			}
			sys::sleep(1);
		}
		if(app->m_bMDSubcribe){
			LOG(INFO) << "[初始化订阅行情] 行情订阅成功";
		}
		/*
		sys::sleep(1);
		if(app->m_bTradeDayChange || !app->m_bCommRateUpdate){
			if((app->m_bCommRateUpdate = app->qryCommissionRate()) == true){
				// LOG  手续费率查询完
			}else{
				//LOG
				sys::sleep(1);
				continue;
			}
		}
		*/
		if(app->m_bTradeDayChange ){
			app->m_bTradeDayChange = false;
			//app->loadRiskWarn();
		}
		if(app->NeedUpdate()){
			app->resetTradeClient();
			sys::sleep(300);	// 5min
			app->InitTradeClient();
		}else{
			app->resetTradeClient();
			sys::sleep(1);
		}
	}
	return NULL;
}

bool Application::NeedUpdate(){
	int curMin = CTime::GetCurIntMinute();
	return getXmlConfig()->CfgNeedUpdate(curMin);
}

void Application::Start(){
	while(1){
		pause();
	}
	
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
				client->SetConnTime(CTime::GetCurTime());
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
int Application::closeClient(CsockClient * pClient){
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
	m_clientMap.erase(it);
	delete pClient;
	pClient=NULL;
	return 0;
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
				if(strUserID.empty() || strBrokerID.empty()|| strPassword.empty()){
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
				if(strUserID.empty() || strBrokerID.empty()|| strNewPassword.empty()|| strOldPassword.empty()){
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
				if(strUserID.empty() || strBrokerID.empty()){
					return -1;
				}
				
			}

		default:
			break;
	}
}


void Application::updateLastMd(CThostFtdcDepthMarketDataField* pData, int _type){
	CThostFtdcDepthMarketDataField data;
	memset(&data,0,sizeof(CThostFtdcDepthMarketDataField));
	memcpy(&data,pData,sizeof(CThostFtdcDepthMarketDataField));
	std::string strInstrumentID = pData->InstrumentID;
	LOG_IF(INFO,_type==1) << "MDPUSH "<<pData->InstrumentID<<"|lastprice:"<<pData->LastPrice<<"|PreSettl:"
		<<pData->PreSettlementPrice<<"|updatetime:"<<pData->UpdateTime;
	LOG_IF(INFO,_type==0) << "QUERRY "<<pData->InstrumentID<<"|lastprice:"<<pData->LastPrice<<"|PreSettl:"
		<<pData->PreSettlementPrice<<"|updatetime:"<<pData->UpdateTime;
	m_LastMd.insert(std::make_pair(strInstrumentID,data));
}
