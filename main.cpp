#include <iostream>

#include "./includ/xmlParse.h"
#include "./include/global.h"
#include "./include/application.h"
#include "./include/test.h"
using namespace std;

class CxmlParse;
SyncEvent g_ev;

int main(int argc, char**argv)
{
	CxmlParse cfgXml;
	int nRet = cfgXml.Load("./bin/config/config.xml");
	if(0 != nRet){
		cout <<"配置文件加载错误!"<<nRet<<endl;
		return -1;
	}
	int nRet = 0;
	#ifdef _TEST_CONN_
	std::string strAddr = cfgXml.getConfig("CTPCfg.TradeAddr");
	nRet = test_conn_trade(strAddr);
	printf("connection trade nRet:%d\n",nRet);
	strAddr = cfgXml.getConfig("CTPCfg.MdAddr");
	nRet = test_conn_marketdata(strAddr);
	printf("connection marketdata nRet:%d\n",nRet);
	#elif _TEST_SUBSCRIB_
	nRet = test_subscrib();
	printf("subscrib nRet:%d\n",nRet);
	#endif
	/*
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
	*/
	return;
}
