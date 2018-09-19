#ifndef __SOCK_CLIENT_H__
#define __SOCK_CLIENT_H__
#include <iostream>
#include "global.h"
#include "contain.h"


class CsockClient{
public:
	CsockClient();
	~CsockClient();
	inline void SetAddr(const char* ip,int port){
		strncpy(m_szIP,ip,strlen(ip));
		m_nPort = port;
	}
	inline char* GetClientIP(){
		return m_szIP;
	}
	inline int GetClientPort(){
		return m_nPort;
	}
	inline void SetSockFd(int fd){
		m_sockfd =fd;
	}
	inline int GetSockFd(){
		return m_sockfd;
	}
	inline void SetConnTime(int conntime){
		m_connTime = conntime;
	}
	inline int GetConnTime(){
		return m_connTime;
	}
	inline void SetLastTouchTime(int lasttime){
		m_lastTouchTime = lasttime;
	}
	inline int GetLastTouchTime(){
		return m_lastTouchTime;
	}
	inline void SetLoginStatus(bool status){
		m_bIsLogin = status;
	}
	inline bool GetLoginStatus(){
		return m_bIsLogin;
	}
	inline CBuffer* rBuffer(){
		return  &m_ReadBuff;
	}
	inline CBuffer* wBuffer(){
		return &m_WriteBuff;
	}
	inline void SetUserID(const std::string& userId){
		m_strUserID=  userId;
	}
	inline std::string GetUserID(){
		return m_strUserID;
	}
	inline void SetPassword(const std::string& password){
		m_strPassword = password;
	}
	inline std::string GetPassword(){
		return m_strPassword;
	}
	
	inline void SetBrokerID(const std::string& brokerid){
		m_strBrokerID = brokerid;
	}
	inline std::string GetBrokerID(){
		return m_strBrokerID;
	}


private:
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
};



#endif
