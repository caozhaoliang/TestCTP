#include "./include/sockClient.h"
using namespace std;


/*
char 	m_szIP[32];
	int		m_nPort;
	int		m_sockfd;
	int		m_connTime;
	int		m_lastTouchTime;
	bool	m_bIsLogin;
	std::string		m_strUserID;
	std::string		m_strBrokerID;
	std::string		m_strPassword;
	
	CBuffer		m_ReadBuff;
	CBuffer		m_WriteBuff;

*/

CsockClient::CsockClient(){
	memset(m_szIP,0,sizeof(m_szIP));
	m_sockfd =0;
	m_connTime=0;
	m_nPort=0;
	m_lastTouchTime=0;
	m_bIsLogin=false;
	
}

CsockClient::~CsockClient(){
	if(m_sockfd>0){
		close(m_sockfd);
		m_sockfd=0;
	}
	m_ReadBuff.ClearBuffer();
	m_WriteBuff.ClearBuffer();
}



