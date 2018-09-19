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
		return -1;
	}
	if(access(cfgPath,F_OK) != 0){
		return -2;
	}
	TiXmlDocument doc;
	if(!doc.LoadFile(cfgPath)){
		return -3;
	}
	TiXmlElement* root = doc.FirstChildElement("Node");	//<Node>
	if(NULL == root){
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
				string comKey=string(szNodeName)+"."+string(szChildNodeName);
				m_mapConfig[comKey]=szChildNodeValue;
			}
			childNode = childNode->NextSiblingElement();
		}
		chNode = chNode->NextSiblingElement();
	}
	doc.close();
	return 0;
}






