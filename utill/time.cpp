#include "../include/time.h"

static void split_token(const std::string& str, std::vector<std::string>& list, const char *token){
	list.clear();
	const char* begin = str.c_str();
	const char* end;
	while (*begin){
		while (*begin && strchr(token, *begin)){
			++begin;
		}
		end = begin;
		while (*end && strchr(token, *end) == NULL){
			++end;
		}
		if (*begin){
			list.push_back(std::string(begin, end - begin));
		}
		begin = end;
	}
}

std::string CTime::GetShortYearStr(){
	struct timeval tv;
	char buffer[20] = {0};

	gettimeofday(&tv,NULL);
	strftime(buffer,sizeof(buffer),"%Y",localtime(&tv.tv_sec));

	return std::string(buffer);
}
std::string  CTime::GetShortTimeStr(){
    struct timeval tv;
    char buffer[20] = {0};

    gettimeofday(&tv,NULL);
    strftime(buffer,sizeof(buffer),"%Y-%m-%d",localtime(&tv.tv_sec));

    return std::string(buffer);
}


//返回短时间格式的字串(如:20120101)
std::string CTime::GetCurDateStr(){
	struct timeval tv;
	char buffer[20] = {0};

	gettimeofday(&tv,NULL);
	strftime(buffer,sizeof(buffer),"%Y%m%d",localtime(&tv.tv_sec));

	return std::string(buffer);
}

std::string  CTime::GetLongTimeStr(){
    struct timeval tv;
    char buffer[20] = {0};

    gettimeofday(&tv,NULL);
    strftime(buffer,sizeof(buffer),"%Y-%m-%d %T",localtime(&tv.tv_sec));

    return std::string(buffer);
}

//获取当前时间(如:01:01:01)
std::string CTime::GetCurShortTimeStr(){
	struct timeval tv;
    char buffer[20] = {0};

    gettimeofday(&tv,NULL);
    strftime(buffer,sizeof(buffer),"%T",localtime(&tv.tv_sec));

    return std::string(buffer);
}

long  CTime::GetCurTime(){
    return time(NULL);
}

long  CTime::GetTotalMilliseconds(){
    timeval tv_cur;
    gettimeofday(&tv_cur, NULL);
    return (1000000 * tv_cur.tv_sec + tv_cur.tv_usec) / 1000;
}

long  CTime::GetTotalMicroseconds(){
    timeval tv_cur;
    gettimeofday(&tv_cur, NULL);
    return 1000000 * tv_cur.tv_sec + tv_cur.tv_usec;
}

int CTime::GetCurIntMinute(){
	struct timeval tv;
    char buffer[20] = {0};

    gettimeofday(&tv,NULL);
    strftime(buffer,sizeof(buffer),"%H:%M",localtime(&tv.tv_sec));

	std::string strResult = buffer;
	
	std::vector<std::string> vectorTime;
	vectorTime.clear();

	split_token(strResult,vectorTime,":");

	int nValue = 0;
	
	switch(vectorTime.size()){
	case 1:
		nValue = atoi(vectorTime[0].c_str())*60;
		break;
	case 2:
    	nValue = atoi(vectorTime[0].c_str())*60+atoi(vectorTime[1].c_str());
		break;
	}
    return nValue;
}

int CTime::GetYesterday( int nDate ){
	int nYear = nDate/10000;
	int nMonth = nDate/100%100;
	int nday = nDate%100;
	int nHour = 0;
	int nMin = 0;
	int nSecond = 0;
	struct tm ptm1;

	ptm1.tm_year = nYear-1900;
	ptm1.tm_mon = nMonth-1;
	ptm1.tm_mday = nday ;
	ptm1.tm_hour = nHour;
	ptm1.tm_min = nMin;
	ptm1.tm_sec = nSecond;

	time_t lt =  mktime(&ptm1);
	lt -= 24*60*60;
	struct tm* ptm2 = localtime(&lt);
	nYear = ptm2->tm_year+1900;
	nMonth = ptm2->tm_mon+1;
	nday = ptm2->tm_mday;
	return nYear*10000+nMonth*100+nday;
}


//传入数字，获取后一天
int CTime::GetTomorrow(int nDate){
	int nYear = nDate/10000;
	int nMonth = nDate/100%100;
	int nday = nDate%100;
	int nHour = 0;
	int nMin = 0;
	int nSecond = 0;
	struct tm ptm1;

	ptm1.tm_year = nYear-1900;
	ptm1.tm_mon = nMonth-1;
	ptm1.tm_mday = nday ;
	ptm1.tm_hour = nHour;
	ptm1.tm_min = nMin;
	ptm1.tm_sec = nSecond;

	time_t lt =  mktime(&ptm1);
	lt += 24*60*60;
	struct tm* ptm2 = localtime(&lt);
	nYear = ptm2->tm_year+1900;
	nMonth = ptm2->tm_mon+1;
	nday = ptm2->tm_mday;
	return nYear*10000+nMonth*100+nday;
}

int CTime::GetCurYear(){
	int year;
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);
	year = timeinfo->tm_year + 1900;

	return year;
}

int CTime::GetCurMonth(){
	int month;
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);
	month = timeinfo->tm_mon + 1;

	return month;
}

int CTime::GetCurDay(){
	int day;
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);
	day = timeinfo->tm_mday;

	return day;
}

int CTime::GetCurWeek(){
	int wday;
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);
	wday = timeinfo->tm_wday;

	return wday;
}

//判断是否为周末
bool CTime::IsWeekend(){
	int nWeek = GetCurWeek();
	if (6 == nWeek || 0 == nWeek){
		return true;
	}else{
		return false;
	}
}
//判断当前时间的前X秒是否为周末
bool CTime::IsWeekEnd(int nSec){
	int wDay,wDay2;
	time_t	tCur,tInput;
	time(&tCur);
	tInput = tCur-nSec;

	struct tm* timeinfo;
	timeinfo=localtime(&tInput);
	wDay = timeinfo->tm_wday;		//x秒之前是否为周六
	wDay2 = GetCurWeek();
	if(wDay2==6 && wDay==5){
		return false;
	}
	if(wDay2 == 0 || wDay2 == 6){
		return true;
	}
	return false;
}

//利用泰勒公式计算星期几
//w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1 
int CTime::GetWeek(int year,int month,int day){
	int w;    //星期
	int c;    //世纪-1 YYYY的头两位
	int y;    //年份   YYYY的后两位
	int m;    //月份 >=3 1月 2月看成上年的13月 14月
	int d=day;    //日
	if(month>=3) {
		c=year / 100;
		y=year % 100;
		m=month;
	}else{
		m=month+12;
		y=(year-1) % 100;
		c=(year-1) / 100;
	}
	w=y+y/4+c/4-2*c+(26*(m+1))/10+d-1;
	w=(w+700) %7;
	return w;
}

//判断是否为周末
bool CTime::IsWeekend(int year,int month,int day){
	int nWeekDay = GetWeek(year,month,day);
	if(6 == nWeekDay || 7 == nWeekDay){
		return true;
	}else{
		return false;
	}
}
namespace sys{
	void msleep(unsigned int ms) {
		struct timespec ts;
		ts.tv_sec = ms / 1000;
		ts.tv_nsec = ms % 1000 * 1000000;
	
		while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
	}	
	void usleep(unsigned int us) {
		struct timespec ts;
		ts.tv_sec = us / 1000000;
		ts.tv_nsec = us % 1000000 * 1000;
	
		while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
	}
}
