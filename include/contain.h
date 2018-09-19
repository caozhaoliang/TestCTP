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
    //获取缓冲区中填充的数据长度
    int   GetDataLength();
    //获取字节缓冲区的大小
    int   GetBufferSize();
    //清除数据，释放缓冲区空间
    void    ClearBuffer();
    //清除数据，但不释放缓冲区空间
    void    ClearData();
    //获取缓冲区指针
    void*   GetBuffer();

    //从缓冲区头部删除数据，缓冲区空间不变
    bool    Delete(int nLength);
    //从缓冲区尾部删除数据，缓冲区空间不变
    bool    DeleteTail(int nLength);
    //从缓冲头部读出数据,缓冲区清除相应数据
    int   Read(void *pData, int nSize);
    //读取的一行数据，若未读取到，则返回NULL
    char*   ReadLine();
    //从缓冲尾部写入数据
    bool    Write(void* pData, int nSize);
    //从缓冲尾部写入数据
    bool    Write(CBuffer *pBuffer);
    //从缓冲头部插入数据
    bool    Insert(void* pData,int nSize);
    //在缓冲区中查找一个字串
    int   Find(char* key, int nPos);

    //压缩缓冲区空间到数据长度大小
    bool    Compress();
    //从一个缓冲区拷贝数据到另一个缓冲区
    bool    Copy(CBuffer& buffer);

public:
    //把整个缓冲区写入文件(包含数据)
    bool    WriteToFile(const char* strFileName);

private:
    bool    ReAllocateBuffer(int nRequestedSize);

    CBuffer(CBuffer &);
    CBuffer& operator=(CBuffer &);
    pthread_mutex_t  m_mutex;     //Buffer锁

private:
    void*   m_pBase;               //字节缓冲区指针
    int   m_nDataSize;           //数据区大小
    int   m_nBufferSize;         //缓冲区大小
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

    //非阻塞，添加数据进队列
    void Push(void *item);

    //阻塞，若没有数据，则等待，有数据才返回
    void* Pop();

	//阻塞，若没有数据，则等待，有数据才返回,并且不会删除队列中元素
	void* Front();

    //非阻塞，若没有数据，则直接返回NULL
    void* Top();

    //非阻塞，返回队列长度
    int GetLength();

private:
    CPtrCircleQueue * mQueue;
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
};


#endif
