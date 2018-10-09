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
		//返回年份格式的字串(如:2012)
		static std::string GetShortYearStr();
        //返回短时间格式的字串(如:2012-01-01)
        static std::string GetShortTimeStr();
		//返回短时间格式的字串(如:20120101)
		static std::string GetCurDateStr();
        //返回长时间格式的字串(如:2012-01-01 01:01:01)
        static std::string GetLongTimeStr();
		//获取当前时间(如:01:01:01)
		static std::string GetCurShortTimeStr();
        //返回当前时间(从1970.01.01 00:00:00以来的秒数)
        static long GetCurTime();
        //返回当前时间(从1970.01.01 00:00:00以来的毫秒数) 
        static long GetTotalMilliseconds();
        //返回当前时间(从1970.01.01 00:00:00以来的微秒数) 
        static long GetTotalMicroseconds();
		//返回当前分钟数(如:01:01分钟数为1*60+1)
		static int	GetCurIntMinute();

		//传入数字，获取前一天
		static int GetYesterday(int nDate);
		//传入数字，获取后一天
		static int GetTomorrow(int nDate);

		static int GetCurWeek();
		static int GetCurDay();
		static int GetCurMonth();
		static int GetCurYear();
		static bool IsWeekend();	//判断是否为周末

		//add by czl 判断当前时间减去两个半小时是否为周末
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

