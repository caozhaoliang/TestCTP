#include "../include/xmlParse.h"
#include <iostream>
#include <cstring> 
#include <iostream>  
#include "./include/global.h"

using namespace std;

CxmlParse::CxmlParse(){
	m_mapConfig.clear();
}

int CxmlParse::Load(const char* cfgPath){
	if(strlen(cfgPath)==0){
		std::cerr<< "config file length is zero"<<std::endl;
		return -1;
	}
	if(access(cfgPath,F_OK) != 0){
		std::cerr<<"file:"<<cfgPath<<" is not a file"<<std::endl;
		return -2;
	}
	TiXmlDocument doc;
	if(!doc.LoadFile(cfgPath)){
		std::cerr<<"TiXmlDocument LoadFile failed"<<std::endl;
		doc.Clear();
        return -3;
	}
	TiXmlElement* root = doc.FirstChildElement("Node");	//<Node>
	if(NULL == root){
		std::cerr<<"childElement(node) not exist"<<std::endl;
	    doc.Clear();
		return -4;
	}
	TiXmlElement* chNode=root->FirstChildElement();		
	while(chNode){
		const char* szNodeName = chNode->Attribute("Name");//ParentKey
		TiXmlElement* childNode=chNode->FirstChildElement();
		while(childNode){
			const char* szChildNodeName = childNode->Attribute("Name");//childKey
			const char* szChildNodeValue = childNode->Attribute("Value");//childValue
			if(szChildNodeName && szChildNodeValue){
				std::string comKey=std::string(szNodeName)+"."+std::string(szChildNodeName);
				m_mapConfig[comKey]=szChildNodeValue;
			}
			childNode = childNode->NextSiblingElement();
		}
		chNode = chNode->NextSiblingElement();
	}
	doc.Clear();
	dealUpdatePeroid();
	return 0;
}

void CxmlParse::dealUpdatePeroid(){
	//日市定时更新时间段(24小时制,缺省为08:30-08:50)
	std::string strDayUpdateTime = m_mapConfig["Server.DayUpdateTime"];
	strDayUpdateTime = (strDayUpdateTime.empty()) ? "08:30-08:50" : strDayUpdateTime;
	std::vector<std::string> vtmp = str::split(strDayUpdateTime,"-");
	if (vtmp.size() == 2){
		std::vector<std::string> v = str::split(vtmp[0],":");
		if (v.size() == 2){
			m_DayUpdatePeroid.first = atoi(v[0].c_str())*60 + atoi(v[1].c_str());
		}else{
			m_DayUpdatePeroid.first = 8*60 + 30;//08:30
		}
		v.clear();
		v = str::split(vtmp[1],":");
		if (v.size() == 2){
			m_DayUpdatePeroid.second = atoi(v[0].c_str())*60 + atoi(v[1].c_str());
		}else{
			m_DayUpdatePeroid.second = 8*60 + 50;//08:50
		}
	}else{
		m_DayUpdatePeroid.first = 8*60 + 30;//08:30
		m_DayUpdatePeroid.second = 8*60 + 50;//08:50
	}
	if (m_DayUpdatePeroid.first > m_DayUpdatePeroid.second){
		m_DayUpdatePeroid.first = 8*60 + 30;//08:30
		m_DayUpdatePeroid.second = 8*60 + 50;//08:50
	}
	vtmp.clear();

	//夜市定时更新时间点(24小时制,缺省为20:30)
	std::string strNightUpdateTime = m_mapConfig["Server.NightUpdateTime"];
	strNightUpdateTime = (strNightUpdateTime.empty()) ? "20:30-20:50" : strNightUpdateTime;
	vtmp = str::split(strNightUpdateTime,"-");
	if (vtmp.size() == 2){
		std::vector<std::string> v = str::split(vtmp[0],":");
		if (v.size() == 2){
			m_NightUpdatePeroid.first = atoi(v[0].c_str())*60 + atoi(v[1].c_str());
		}else{
			m_NightUpdatePeroid.first = 8*60 + 30;//08:30
		}
		v.clear();
		v = str::split(vtmp[1],":");
		if (v.size() == 2){
			m_NightUpdatePeroid.second = atoi(v[0].c_str())*60 + atoi(v[1].c_str());
		}else{
			m_NightUpdatePeroid.second = 8*60 + 50;//08:50
		}
	}else{
		m_NightUpdatePeroid.first =20*60 + 30;//08:30
		m_NightUpdatePeroid.second = 20*60 + 50;//08:50
	}
	if (m_NightUpdatePeroid.first > m_NightUpdatePeroid.second){
		m_NightUpdatePeroid.first =20*60 + 30;//08:30
		m_NightUpdatePeroid.second = 20*60 + 50;//08:50
	}
}
bool CxmlParse::CfgNeedUpdate(int curMin){
	if (curMin>= m_DayUpdatePeroid.first && curMin<= m_DayUpdatePeroid.second ||
		curMin >= m_NightUpdatePeroid.first && curMin <= m_NightUpdatePeroid.second){
			return true;
	}else{
		return false;
	}
}




