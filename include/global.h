#ifndef __GLOBAL_H_1_
#define __GLOBAL_H_1_
#include <assert.h>
#include <arpa/inet.h>
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
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/epoll.h>
//#include <time.h>
#include <unistd.h>
#include <zlib.h>

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <memory>
#include <iostream>
#include <fstream>
//CTP相关头文件
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"	//ThostFtdcUserApiStruct.h
//Json 头文件
#include "../lib/json.h"

//glog 头文件
#include "glog/logging.h"
#include "glog/raw_logging.h"

using namespace std;


typedef struct tagInstrumentBaseInfo{
	char	InstrumentID[31];				//合约代码
	char	InstrumentAliasID[31];			//合约别名ID
	char	UnitName[31];					//交易单位名称
	char	PYName[50];						//拼音名称首字母
	char	FullPYName[50];					//拼音名称全拼
	char	TypeID[11];						//品种ID
	char	ExchangeID[9];					//交易所代码
	char	InstrumentName[21];				//合约名称
	char	ExchangeInstID[31];				//合约在交易所的代码
	char	ProductID[31];					//产品代码
	char	ProductClass;					//产品类型
	int		DeliveryYear;					//交割年份
	int		DeliveryMonth;					//交割月
	int		MaxMarketOrderVolume;			//市价单最大下单量
	int		MinMarketOrderVolume;			//市价单最小下单量
	int		MaxLimitOrderVolume;			//限价单最大下单量
	int		MinLimitOrderVolume;			//限价单最小下单量
	int		VolumeMultiple;					//合约数量乘数
	double	PriceTick;						//最小变动价位
	char	CreateDate[9];					//创建日
	char	OpenDate[9];					//上市日
	char	ExpireDate[9];					//到期日
	char	StartDelivDate[9];				//开始交割日
	char	EndDelivDate[9];				//结束交割日
	char	InstLifePhase;					//合约生命周期状态
	int		IsTrading;						//当前是否交易
	char	PositionType;					//持仓类型
	char	PositionDateType;				//持仓日期类型
	double	LongMarginRatio;				//多头保证金率
	double	ShortMarginRatio;				//空头保证金率
	char	MaxMarginSideAlgorithm;			//是否使用大额单边保证金算法
	char	UnderlyingInstrID[31];			//基础商品代码
	double	StrikePrice;					//执行价
	char	OptionsType;					//期权类型
	double	UnderlyingMultiple;				//合约基础商品乘数
	char	CombinationType;				//组合类型
}InstrumentBaseInfo;

//手续费率信息
typedef struct tagCommissionChargeInfo{
	char	InstrumentID[31];			//合约ID
	char 	InvestorRange;				//投资者范围
	char	BrokerID[11];				//经纪公司代码
	char	InvestorID[13];				//投资者代码
	double	OpenRatioByMoney;			//开仓手续费率
	double	OpenRatioByVolume;			//开仓手续费
	double	CloseRatioByMoney;			//平仓手续费率
	double	CloseRatioByVolume;			//平仓手续费
	double	CloseTodayRatioByMoney;		//平今手续费率
	double	CloseTodayRatioByVolume;	//平今手续费
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

bool dirExist(const std::string& path);

bool makedirs(const std::string& path);

bool rmdirs(const std::string& path);

std::string get_version();

bool isPortUsed(int nPort);

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
			if(!m_manual_reset)	m_signaled = false;//(如果为自动重置，则将信号重置)
		}

		bool timed_wait(unsigned int ms){
			struct timeval now;
            struct timespec timeout;
            int retcode;
 			MutexGuard g(m_mutex);
            gettimeofday(&now,NULL);
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

		bool 	m_manual_reset;
		bool	m_signaled;
		SyncEvent(const SyncEvent& ){}
		void operator=(const SyncEvent& ){}
		//DISALLOW_COPY_AND_ASSIGN(SyncEvent);
	};

namespace str{
	std::vector<std::string> split(const std::string s,const std::string sep,int max=-1);
};
#endif 


