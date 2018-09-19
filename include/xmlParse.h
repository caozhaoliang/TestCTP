#ifndef __XML_PARSE_H__
#define __XML_PARSE_H__
#include <iostream>
#include <string>
#include <map>
#include "../utill/tinyxml.h"
#include "../utill/tinystr.h"
using namespace std;

class CxmlParse{
public:
	CxmlParse();
	int Load(const char* cfgPath);
	inline std::string getConfig(const std::string& Key){
		return m_mapConfig[Key];
	}
	inline int getConfigAsInt(const std::string& Key){
		return atoi(m_mapConfig[Key].c_str());
	}
	inline double getConfigAsDouble(const std::string& Key){
		return atof(m_mapConfig[Key].c_str());
	}

private:
	std::map<string,string>		m_mapConfig;
};


#endif
