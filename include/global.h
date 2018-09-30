#ifndef __GLOBAL_H_1_
#define __GLOBAL_H_1_
#include <utility>   
#include <net/inet.h>

#include <asm/ioctls.h>
#include <assert.h>
#include <arpa/inet.h>
#include <bits/signum.h>
#include <ctype.h>
#include <dirent.h> 
#include <errno.h>
#include <float.h>
#include <fcntl.h>
#include <math.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>             /* ISO C variable arguments */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <sys/shm.h> 
#include <sys/sem.h> 
#include <time.h>
#include <unistd.h>
#include <zlib.h>

#include <typeinfo>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <bitset>
#include <memory>
#include <iostream>
#include <fstream>
//CTP���ͷ�ļ�
#include "./include/ThostFtdcMdApi.h"
#include "./include/ThostFtdcTraderApi.h"
#include "./include/ThostFtdcUserApiDataType.h"
#include "./include/ThostFtdcUserApiStruct.h"
//Json ͷ�ļ�
#include "../lib/json.h"

//glog ͷ�ļ�
#include "./glog/logging.h"

using namespace std;


typedef struct tagInstrumentBaseInfo{
	char	InstrumentID[31];				//��Լ����
	char	InstrumentAliasID[31];			//��Լ����ID
	char	UnitName[31];					//���׵�λ����
	char	PYName[50];						//ƴ����������ĸ
	char	FullPYName[50];					//ƴ������ȫƴ
	char	TypeID[11];						//Ʒ��ID
	char	ExchangeID[9];					//����������
	char	InstrumentName[21];				//��Լ����
	char	ExchangeInstID[31];				//��Լ�ڽ������Ĵ���
	char	ProductID[31];					//��Ʒ����
	char	ProductClass;					//��Ʒ����
	int		DeliveryYear;					//�������
	int		DeliveryMonth;					//������
	int		MaxMarketOrderVolume;			//�м۵�����µ���
	int		MinMarketOrderVolume;			//�м۵���С�µ���
	int		MaxLimitOrderVolume;			//�޼۵�����µ���
	int		MinLimitOrderVolume;			//�޼۵���С�µ���
	int		VolumeMultiple;					//��Լ��������
	double	PriceTick;						//��С�䶯��λ
	char	CreateDate[9];					//������
	char	OpenDate[9];					//������
	char	ExpireDate[9];					//������
	char	StartDelivDate[9];				//��ʼ������
	char	EndDelivDate[9];				//����������
	char	InstLifePhase;					//��Լ��������״̬
	int		IsTrading;						//��ǰ�Ƿ���
	char	PositionType;					//�ֲ�����
	char	PositionDateType;				//�ֲ���������
	double	LongMarginRatio;				//��ͷ��֤����
	double	ShortMarginRatio;				//��ͷ��֤����
	char	MaxMarginSideAlgorithm;			//�Ƿ�ʹ�ô��߱�֤���㷨
	char	UnderlyingInstrID[31];			//������Ʒ����
	double	StrikePrice;					//ִ�м�
	char	OptionsType;					//��Ȩ����
	double	UnderlyingMultiple;				//��Լ������Ʒ����
	char	CombinationType;				//�������
}InstrumentBaseInfo;

//����������Ϣ
typedef struct tagCommissionChargeInfo{
	char	InstrumentID[31];			//��ԼID
	char 	InvestorRange;				//Ͷ���߷�Χ
	char	BrokerID[11];				//���͹�˾����
	char	InvestorID[13];				//Ͷ���ߴ���
	double	OpenRatioByMoney;			//������������
	double	OpenRatioByVolume;			//����������
	double	CloseRatioByMoney;			//ƽ����������
	double	CloseRatioByVolume;			//ƽ��������
	double	CloseTodayRatioByMoney;		//ƽ����������
	double	CloseTodayRatioByVolume;	//ƽ��������
}CommissionChargeInfo;


typedef struct ctpTask{
	int								nSockFd;
	int								nFuncID;
	std::string 					strUserID;
	std::string 					strBrokerID;
	std::string 					strPassword;
	std::string						strNewPassword;
	std::string						strOldPassword;
	std::string						strInstrumentID;

} Task;

bool dirExist(const std::string& path){
	return access(path.c_str(),F_OK)==0;
}

bool makedirs(const std::string& path){
	return system("mkdir  -p "+path);
}

bool rmdirs(const std::string& path){
	DIR * pDir = NULL;  
	struct dirent *pDirent = NULL; 

	if((pDir = opendir(path.c_str())) == NULL){
		return FALSE;
	}

	char szBuf[256] = {0};
	while((pDirent = readdir(pDir)) != NULL){
		if(strcmp(pDirent->d_name,".") == 0 
			|| strcmp(pDirent->d_name,"..") == 0){
				continue;
		}

		if(pDirent->d_type == DT_DIR){
			memset(szBuf,0,256);
			snprintf(szBuf,256,"%s/%s",path.c_str(),pDirent->d_name);
			rmdirs(szBuf);
		}else{
			memset(szBuf,0,256);
			snprintf(szBuf,256,"%s/%s",path.c_str(),pDirent->d_name);
			remove(szBuf);//delete file
		}
	}

	closedir(pDir);

	if(0 != remove(path.c_str())){
		return false;
	}

	return true;
}


bool isPortUsed(int nPort){
	int socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(socketfd ==-1){
		return true;	//�׽��ִ���ʧ��?����������
	}
	if(nPort <=0){
		return true;		//true ��ʾ����ʹ�øö˿�
	}
	struct sockaddr_in socketAddr;
	socketAddr.sin_family=AF_INET;
	socketAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	socketAddr.sin_port = htons(nPort);
	int nResult = bind(socketfd,(struct sockaddr*)&socketAddr,sizeof(socketAddr));
	if( nResult != -1){	//bind success port not be used
		close(socketfd);
		return false;
	}
	return true;
}

class Mutex{
	public:
		Mutex(){
			//CHECK(pthread_mutex_init(&_mutex, NULL) == 0);
			pthread_mutex_init(&_mutex, NULL);
		}

		~Mutex() {
			//CHECK(pthread_mutex_destroy(&_mutex) == 0);
			pthread_mutex_destroy(&_mutex);
		}

		void lock() {
			int err = pthread_mutex_lock(&_mutex);
			//CHECK_EQ(err, 0) << ::strerror(err);
		}

		void unlock() {
			int err = pthread_mutex_unlock(&_mutex);
			//CHECK_EQ(err, 0) << ::strerror(err);
		}

		pthread_mutex_t* mutex() {
			return &_mutex;
		}
	private:
		pthread_mutex_t		_mutex;
		Mutex(const Mutex&){}
		void operator=(const Mutex&){}
	};


class MutexGuard{
	public:
		explicit MutexGuard(Mutex &mutex):
		m_Mutex(mutex){
			m_Mutex.lock();
		}

		~MutexGuard(){
			m_Mutex.unlock();
		}
	private:
		MutexGuard(const MutexGuard &);
		void operator=(const MutexGuard &);

		Mutex &m_Mutex;
	};

class SyncEvent{
	public:
		explicit SyncEvent(bool manual_reset=false,bool signaled=false)
			:m_manual_reset(manual_reset),m_signaled(signaled){
			//pthread_condattr_t	attr;
			//CHECK(pthread_condattr_init(&attr) == 0);
			//CHECK(pthread_condattr_setclock(&attr,CLOCK_MONOTONIC) == 0);
			//CHECK(pthread_cond_init(&m_cond,&attr) == 0);
			//CHECK(pthread_condattr_destroy(&attr) == 0);
			pthread_cond_init(&m_cond,NULL);
		}

		~SyncEvent(){
			//CHECK(pthread_cond_destroy(&m_cond) == 0);
			pthread_cond_destroy(&m_cond);
		}

		void signal(){
			MutexGuard g(m_mutex);
			if (!m_signaled){
				m_signaled = true;
				pthread_cond_broadcast(&m_cond);
			}
		}

		void reset(){
			MutexGuard g(m_mutex);
			m_signaled = false;
		}

		bool is_signaled(){
			MutexGuard g(m_mutex);
			return m_signaled;
		}

		void wait(){
			MutexGuard g(m_mutex);
			if(!m_signaled){
				pthread_cond_wait(&m_cond, m_mutex.mutex());
				//CHECK(m_signaled);
			}
			if(!m_manual_reset)	m_signaled = false;//(���Ϊ�Զ����ã����ź�����)
		}

		bool timed_wait(unsigned int ms){
			struct timeval now;
            struct timespec timeout;
            int retcode;
 			MutexGuard g(m_mutex);
            gettimeofday(&now);
            timeout.tv_sec = ms / 1000;
            timeout.tv_nsec = ms % 1000 * 1000000;
            retcode = 0;
            while (retcode != ETIMEDOUT) {
            	retcode = pthread_cond_timedwait(&m_cond, m_mutex.mutex(), &timeout);
           	}
            if (retcode == ETIMEDOUT) {
            	return false;
            }
			if(!m_manual_reset){
				m_signaled = false;
			}
			return true;
		}

	private:
		pthread_cond_t	m_cond;
		Mutex m_mutex;

		const bool m_manual_reset;
		bool	m_signaled;
		SyncEvent(const SyncEvent& ){}
		void operator=(const SyncEvent& ){}
		//DISALLOW_COPY_AND_ASSIGN(SyncEvent);
	};


#endif 


