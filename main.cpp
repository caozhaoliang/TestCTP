#include <iostream>
#include "./includ/xmlParse.h"
#include "./include/global.h"
#include "./include/application.h"

using namespace std;

class CxmlParse;


int main(int argc, char**argv)
{
	CxmlParse cfgXml;
	int nRet = cfgXml.Load("./bin/config/config.xml");
	if(0 != nRet){
		cout <<"配置文件加载错误!"<<nRet<<endl;
		return -1;
	}
	int nPort = cfgXml.getConfigAsInt("Server.ListenPort");
	if(nPort <= 0){
		nPort = 10086;
	}
	if(isPortUsed(nPort)){
		return -1;
	}
	if(daemon(1,0)){		//转入后台运行
		return -1;
	}
	Application* app = new Application(cfgXml,nPort);
	app->Start();

	return;
}
