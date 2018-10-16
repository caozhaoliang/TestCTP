#include <iostream>

#include "./include/xmlParse.h"
#include "./include/global.h"
#include "./include/application.h"
#include "./include/test.h"
#include "./include/gflags/gflags.h"
using namespace std;

class CxmlParse;
SyncEvent g_ev;
bool g_SubscribStatus;

int main(int argc, char**argv)
{
	//���������ļ�
	CxmlParse cfgXml;
	int nRet = cfgXml.Load("./bin/config/config.xml");
	if(0 != nRet){
		cout <<"�����ļ����ش���!"<<nRet<<endl;
		return -1;
	}
	//��ʼ����־����
	std::string strPath = "./log/";
	google::InitGoogleLogging("TCTP");
	google::SetLogDestination(google::INFO,"./log/CTPDemo.log");
	if(!dirExist(strPath)){
		makedirs(strPath);
	}
	//int nRet = 0;
	#ifdef _TEST_CONN_
	std::string strAddr = cfgXml.getConfig("CTPCfg.TradeAddr");
	nRet = test_conn_trade(strAddr);
	printf("connection trade nRet:%d\n",nRet);
	strAddr = cfgXml.getConfig("CTPCfg.MdAddr");
	nRet = test_conn_marketdata(strAddr);
	printf("connection marketdata nRet:%d\n",nRet);
	
	#endif
	/*
	int nPort = cfgXml.getConfigAsInt("Server.ListenPort");
	if(nPort <= 0){
		nPort = 10086;
	}
	if(isPortUsed(nPort)){
		return -1;
	}*/
	if(daemon(1,0)){		//ת���̨����
		return -1;
	}
	LOG(INFO) << "ת���̨����...����汾:"<<get_version();
		
	int nPort = 10086;
	Application* app = new Application(&cfgXml,nPort);
	app->Start();
	
	
	//delete app;
	google::ShutdownGoogleLogging();
	return 0;
}
