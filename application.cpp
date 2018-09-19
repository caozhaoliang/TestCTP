#include "./include/application.h"
#include <pthread.h>

Application::Application(){
}
Application::Application(const CxmlParse* cfgxml,int nPort)
{
	m_CfgXml = cfgxml;
	m_nPort = nPort;
	Init();
}

bool Application::Init(){
	
	pthread_t pid[4];
	int nPid = pthread_create(&pid[0],NULL,handleCtpConnet,this);
	if(0 != nPid){
		return false;
	}	
}

void* Application::handleCtpConnet(void* arg){
	//建立并维持与CTP trade Md 的链接并且实时获取行情数据
	Application* app = (Application*)arg;
	while(1){
		if(m_mdApi == NULL){
			m_mdApi = new CMDApi(app);
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
