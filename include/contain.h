#ifndef _CONTAIN_H_
#define _CONTAIN_H_

#include <iostream>
#include <string>
#include "global.h"
#include <pthread.h>


using namespace std;
class CBuffer
{
public:
    CBuffer();
    virtual ~CBuffer();
    //��ȡ���������������ݳ���
    int   GetDataLength();
    //��ȡ�ֽڻ������Ĵ�С
    int   GetBufferSize();
    //������ݣ��ͷŻ������ռ�
    void    ClearBuffer();
    //������ݣ������ͷŻ������ռ�
    void    ClearData();
    //��ȡ������ָ��
    void*   GetBuffer();

    //�ӻ�����ͷ��ɾ�����ݣ��������ռ䲻��
    bool    Delete(int nLength);
    //�ӻ�����β��ɾ�����ݣ��������ռ䲻��
    bool    DeleteTail(int nLength);
    //�ӻ���ͷ����������,�����������Ӧ����
    int   Read(void *pData, int nSize);
    //��ȡ��һ�����ݣ���δ��ȡ�����򷵻�NULL
    char*   ReadLine();
    //�ӻ���β��д������
    bool    Write(void* pData, int nSize);
    //�ӻ���β��д������
    bool    Write(CBuffer *pBuffer);
    //�ӻ���ͷ����������
    bool    Insert(void* pData,int nSize);
    //�ڻ������в���һ���ִ�
    int   Find(char* key, int nPos);

    //ѹ���������ռ䵽���ݳ��ȴ�С
    bool    Compress();
    //��һ���������������ݵ���һ��������
    bool    Copy(CBuffer& buffer);

public:
    //������������д���ļ�(��������)
    bool    WriteToFile(const char* strFileName);

private:
    bool    ReAllocateBuffer(int nRequestedSize);

    CBuffer(CBuffer &);
    CBuffer& operator=(CBuffer &);
    pthread_mutex_t  m_mutex;     //Buffer��

private:
    void*   m_pBase;               //�ֽڻ�����ָ��
    int   m_nDataSize;           //��������С
    int   m_nBufferSize;         //��������С
};


class CPtrCircleQueue
{
public:
    CPtrCircleQueue();
    virtual  ~CPtrCircleQueue();

    void  Push(void *item);
	void*  Pop();
	void*  Front();
    void*  Top();
    int  GetLength();

private:
    void ** mEntries;
    unsigned int mHead;
    unsigned int mTail;
    unsigned int mCount;
    unsigned int mMaxCount;
};


class CBlockingQueue
{
public:
    CBlockingQueue();
    virtual ~CBlockingQueue();

    //��������������ݽ�����
    void Push(void *item);

    //��������û�����ݣ���ȴ��������ݲŷ���
    void* Pop();

	//��������û�����ݣ���ȴ��������ݲŷ���,���Ҳ���ɾ��������Ԫ��
	void* Front();

    //����������û�����ݣ���ֱ�ӷ���NULL
    void* Top();

    //�����������ض��г���
    int GetLength();

private:
    CPtrCircleQueue * mQueue;
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
};


#endif
