#ifndef _TEST_H__
#define _TEST_H__
#include <iostream>
#include <pthread.h>
#include "global.h"

class CTestTradeSpi: public CThostFtdcTraderSpi{
public:
	CTestTradeSpi(CThostFtdcTraderApi *pClientApi);
	~CTestTradeSpi();
	//���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	//���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);

	//������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse);

	//��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	CThostFtdcTraderApi *	pTApi;
};
class CTestMarketSpi: public CThostFtdcMdSpi{
public:
    CTestMarketSpi(CThostFtdcMdApi* pClientApi);
	~CTestMarketSpi();
	//���ͻ����뽻���й�ϵͳ������ͨ�����ӣ��ͻ�����Ҫ���е�¼
	virtual	void OnFrontConnected();

	//���ͻ����뽻���й�ϵͳͨ�����ӶϿ�ʱ���÷���������
	virtual	void OnFrontDisconnected(int	nReason);

private:
	CThostFtdcMdApi* 		pMarketApi;
	
};
#endif

