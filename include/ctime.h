#ifndef TIME_H_
#define TIME_H_
#include <iostream>
#include "global.h"
#include <time.h>
#include <sys/time.h>
using namespace std;

class CTime
{
public:        
		//������ݸ�ʽ���ִ�(��:2012)
		static std::string GetShortYearStr();
        //���ض�ʱ���ʽ���ִ�(��:2012-01-01)
        static std::string GetShortTimeStr();
		//���ض�ʱ���ʽ���ִ�(��:20120101)
		static std::string GetCurDateStr();
        //���س�ʱ���ʽ���ִ�(��:2012-01-01 01:01:01)
        static std::string GetLongTimeStr();
		//��ȡ��ǰʱ��(��:01:01:01)
		static std::string GetCurShortTimeStr();
        //���ص�ǰʱ��(��1970.01.01 00:00:00����������)
        static long GetCurTime();
        //���ص�ǰʱ��(��1970.01.01 00:00:00�����ĺ�����) 
        static long GetTotalMilliseconds();
        //���ص�ǰʱ��(��1970.01.01 00:00:00������΢����) 
        static long GetTotalMicroseconds();
		//���ص�ǰ������(��:01:01������Ϊ1*60+1)
		static int	GetCurIntMinute();

		//�������֣���ȡǰһ��
		static int GetYesterday(int nDate);
		//�������֣���ȡ��һ��
		static int GetTomorrow(int nDate);

		static int GetCurWeek();
		static int GetCurDay();
		static int GetCurMonth();
		static int GetCurYear();
		static bool IsWeekend();	//�ж��Ƿ�Ϊ��ĩ

		//add by czl �жϵ�ǰʱ���ȥ������Сʱ�Ƿ�Ϊ��ĩ
		static bool IsWeekEnd(int nSec);
		int GetWeek(int year,int month,int day);
		bool IsWeekend(int year,int month,int day);
protected:
       CTime() {}
       ~CTime() { }
};
namespace sys{
	void msleep(unsigned int ms);
	
	/*
	 * sleep for @us microseconds
	 */
	void usleep(unsigned int us);
	
	/*
	 * sleep for @sec seconds
	 */
	inline void sleep(unsigned int sec) {
		sys::msleep(sec * 1000);
	}

}
#endif /* TIME_H_ */

