#include "./include/contain.h"

CBuffer::CBuffer()
{
    m_nBufferSize = 0;
    m_nDataSize = 0;
    m_pBase = NULL;
    //初始化互斥锁
    pthread_mutex_init(&m_mutex,NULL);
}

CBuffer::~CBuffer()
{
    pthread_mutex_destroy(&m_mutex);
    if (NULL != m_pBase && m_nBufferSize > 0)
    {
        free(m_pBase);
        m_pBase = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    GetBufferSize
//
// 函数说明:    获取字节缓冲区的大小
//
// 参数说明:
//
// 返 回 值:    字节缓冲区的大小
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
int  CBuffer::GetBufferSize()
{
    return m_nBufferSize;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    GetDataLength
//
// 函数说明:    获取缓冲区中填充的数据长度
//
// 参数说明:
//
// 返 回 值:    数据长度
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
int  CBuffer::GetDataLength()
{
    if (m_pBase == NULL)
    {
        return 0;
    }

    return m_nDataSize;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    ClearBuffer
//
// 函数说明:    清除数据，释放缓冲区空间
//
// 参数说明:
//
// 返 回 值:    无
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
void  CBuffer::ClearBuffer()
{
    pthread_mutex_lock(&m_mutex);
    if(NULL != m_pBase && m_nBufferSize > 0)
    {
        free(m_pBase);
        m_pBase = NULL;
    }

    m_nBufferSize = 0;
    m_nDataSize = 0;
    pthread_mutex_unlock(&m_mutex);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    ClearData
//
// 函数说明:    清除数据，但不释放缓冲区空间
//
// 参数说明:
//
// 返 回 值:    无
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
void  CBuffer::ClearData()
{
    pthread_mutex_lock(&m_mutex);
    if(NULL != m_pBase)
    {
        memset(m_pBase,0,m_nDataSize);
        m_nDataSize = 0;
    }
    pthread_mutex_unlock(&m_mutex);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    GetBuffer
//
// 函数说明:    获取缓冲区指针
//
// 参数说明:
//
// 返 回 值:    获取的缓冲区指针
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
void*  CBuffer::GetBuffer()
{
   return m_pBase;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Delete
//
// 函数说明:    从缓冲区头部删除数据，缓冲区空间不变
//
// 参数说明:    nLength       需要删除的长度，不能大于数据长度，若大于数据
//                            长度，则删除失败
//
// 返 回 值:    true: 成功    false: 失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
bool  CBuffer::Delete(int nLength)
{
    pthread_mutex_lock(&m_mutex);
    if(nLength > m_nDataSize)
    {
        pthread_mutex_unlock(&m_mutex);
        return false;
    }
    if(nLength > 0)
    {
        memmove(m_pBase,(char*)m_pBase + nLength,m_nBufferSize - nLength);
        m_nDataSize = m_nDataSize - nLength;
        memset((char*)m_pBase + m_nDataSize,0,nLength);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }
    pthread_mutex_unlock(&m_mutex);
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    DeleteTail
//
// 函数说明:    从缓冲区尾部删除数据，缓冲区空间不变
//
// 参数说明:    nLength       需要删除的长度，不能大于数据长度，若大于数据
//                            长度，则删除失败
//
// 返 回 值:    true: 成功    false: 失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
bool  CBuffer::DeleteTail(int nLength)
{
    pthread_mutex_lock(&m_mutex);
    if(nLength > m_nDataSize)
    {
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    if(nLength > 0)
    {
        m_nDataSize = m_nDataSize - nLength;
        memset((char*)m_pBase + m_nDataSize,0,nLength);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }
    pthread_mutex_unlock(&m_mutex);
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Read
//
// 函数说明:    从缓冲头部读出数据,缓冲区清除相应数据
//
// 参数说明:    pData          存放读取数据的缓冲区指针
//              nSize          缓冲区长度
//
// 返 回 值:    实际读取到的数据长度，不会大于其实际保存的数据长度
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
int  CBuffer::Read(void *pData, int nSize)
{
    pthread_mutex_lock(&m_mutex);
    if(nSize > m_nDataSize)
    {
        nSize = m_nDataSize;
    }

    if(nSize > 0)
    {
        memcpy((char*)pData,(char*)m_pBase,nSize);
        memmove(m_pBase,(char*)m_pBase + nSize,m_nBufferSize - nSize);
        m_nDataSize = m_nDataSize - nSize;
        memset((char*)m_pBase + m_nDataSize,0,nSize);
    }
    pthread_mutex_unlock(&m_mutex);
    return nSize;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    ReadLine
//
// 函数说明:    从缓冲头部开始读出一行数据
//
// 参数说明:
//
// 返 回 值:    读取的一行数据，若未读取到，则返回NULL
//
// 重要说明:    返回的数据是新开辟的空间，需要自己清除，返回数据中包含最后的
//              行结束符
//
////////////////////////////////////////////////////////////////////////////////
char*  CBuffer::ReadLine()
{
    char *lineBuf = NULL;
    int i = 0;

    for(i = 0; i< m_nDataSize; i++ )
    {
        char *pTemp = (char*)m_pBase + i;
        if((*pTemp) == '\n')
        {
            break;
        }
    }

    if(i == m_nDataSize)
    {
         return NULL;
    }

    if((lineBuf = (char*)malloc(i+2)) == NULL)  //分配空间失败
    {
        return NULL;
    }

    memcpy(lineBuf,m_pBase,i+1);
    lineBuf[i+1] = '\0';

    Delete(i+1);

    return lineBuf;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Insert
//
// 函数说明:    从缓冲头部插入数据
//
// 参数说明:    pData          存放需插入数据的指针
//              nSize          需插入数据长度
//
// 返 回 值:    true   插入成功     false   插入失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
bool  CBuffer::Insert(void* pData,int nSize)
{
    pthread_mutex_lock(&m_mutex);
    if(ReAllocateBuffer(nSize + m_nDataSize))
    {
       memmove((char*)m_pBase + nSize,(char*)m_pBase,m_nBufferSize - nSize);
       memcpy((char*)m_pBase, (char*)pData, nSize);
       m_nDataSize += nSize;
	   pthread_mutex_unlock(&m_mutex);
       return true;
    }
    pthread_mutex_unlock(&m_mutex);
    return false;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Write
//
// 函数说明:    从缓冲尾部写入数据
//
// 参数说明:    pData          存放需写入数据的指针
//              nSize          需写入数据长度
//
// 返 回 值:    true   写入成功     false   写入失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
bool  CBuffer::Write(void* pData, int nSize)
{
    pthread_mutex_lock(&m_mutex);
    if (ReAllocateBuffer(m_nDataSize + nSize))
    {
        memcpy((char*)m_pBase + m_nDataSize, (char*)pData, nSize);
        m_nDataSize += nSize;
        pthread_mutex_unlock(&m_mutex);
        return true;
    }
    pthread_mutex_unlock(&m_mutex);
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Write
//
// 函数说明:    从缓冲尾部写入数据
//
// 参数说明:    pBuffer        保存数据的缓冲对象
//
// 返 回 值:    true   写入成功     false   写入失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
bool  CBuffer::Write(CBuffer *pBuffer)
{
     return Write(pBuffer->GetBuffer(), pBuffer->GetDataLength());
}



////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Compress
//
// 函数说明:    压缩缓冲区空间到数据长度大小
//
// 参数说明:
//
// 返 回 值:    true   压缩成功     false   压缩失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
bool  CBuffer::Compress()
{
    pthread_mutex_lock(&m_mutex);
    if(NULL != m_pBase)
    {
        m_nBufferSize = m_nDataSize + 1;
        m_pBase = realloc(m_pBase,m_nBufferSize);
        memset((char*)m_pBase + m_nDataSize,0,m_nBufferSize - m_nDataSize);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }
    pthread_mutex_unlock(&m_mutex);
    return false;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Copy
//
// 函数说明:    从一个缓冲区拷贝数据到另一个缓冲区
//
// 参数说明:
//
// 返 回 值:    true   拷贝成功     false   拷贝失败
//
// 重要说明:
//
//
////////////////////////////////////////////////////////////////////////////////
bool  CBuffer::Copy(CBuffer& buffer)
{
	pthread_mutex_lock(&m_mutex);
    int nSrcBufferSize = buffer.GetBufferSize();
    if(nSrcBufferSize)
    {
        ClearBuffer();
        m_pBase = calloc(nSrcBufferSize,sizeof(char));    //空间自动初始化为0
        if(NULL != m_pBase)
        {
            memcpy(m_pBase,buffer.GetBuffer(),buffer.GetDataLength());
            m_nBufferSize = nSrcBufferSize;
            m_nDataSize = buffer.GetDataLength();
            memset((char*)m_pBase + m_nDataSize, 0, m_nBufferSize - m_nDataSize);
            pthread_mutex_unlock(&m_mutex);
            return true;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Find
//
// 函数说明:    在缓冲区中查找一个字串
//
// 参数说明:    key          需要查找的字串
//              nPos         查找开始位置
//
// 返 回 值:    若未查找到，则返回-1，否则返回字串的位置偏移，此位置偏移已经包含
//              了字串的长度
//
// 重要说明:
//
//
////////////////////////////////////////////////////////////////////////////////
int CBuffer::Find(char *key, int nPos)
{
    if (nPos > m_nDataSize)
    {
        return -1;
    }

    char *pStr = (char*) strstr((char*)m_pBase + nPos, key);

    int nOffset = 0;
    if (pStr)
    {
        nOffset = (pStr - (char*)m_pBase) + strlen(key);
    }

    return nOffset;
}

bool  CBuffer::ReAllocateBuffer(int nRequestedSize)
{
    if (nRequestedSize < m_nBufferSize) //缓冲区够大，还不需要增加缓冲区
    {
        return true;
    }

    int nNewSize = (int) ceil(nRequestedSize / (BUFFER_STEP_LENGTH * 1.0)) * BUFFER_STEP_LENGTH;

    //分配新的缓冲区
    char *pTemp = (char*)calloc(nNewSize,sizeof(char));
    if(pTemp != NULL)
    {
        memcpy(pTemp, (char*)m_pBase, m_nDataSize);  //把旧数据拷贝到新缓冲区

        free(m_pBase);  //清除以前的数据
        m_nBufferSize = nNewSize;
        m_pBase = pTemp;

        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    WriteToFile
//
// 函数说明:    把缓冲区数据写入件
//
// 参数说明:
//
// 返 回 值:    true   写入成功     false   写入失败
//
// 重要说明:
//
//
////////////////////////////////////////////////////////////////////////////////
bool CBuffer::WriteToFile(const char* strFileName)
{
    FILE*  pFile = NULL;

    pFile = fopen(strFileName,"wb");
    if(pFile != NULL)
    {
        fwrite(m_pBase,1,m_nBufferSize,pFile);
        fflush(pFile);
        fclose(pFile);

        return true;
    }

    return false;
}

CPtrCircleQueue::CPtrCircleQueue()
{
    mMaxCount = 8;
    mEntries = (void**) malloc(sizeof(void*) * mMaxCount);

    mHead = mTail = mCount = 0;
}

CPtrCircleQueue::~CPtrCircleQueue()
{
    free(mEntries);
    mEntries = NULL;
}

void  CPtrCircleQueue::Push(void* item)
{
    if (mCount >= mMaxCount)
    {
        mMaxCount = (mMaxCount * 3) / 2 + 1;
        void ** newEntries = (void**) malloc(sizeof(void *) * mMaxCount);

        unsigned int headLen = 0, tailLen = 0;
        if (mHead < mTail)
        {
            headLen = mTail - mHead;
        }
        else
        {
            headLen = mCount - mTail;
            tailLen = mTail;
        }

        memcpy(newEntries, &(mEntries[mHead]), sizeof(void *) * headLen);
        if (tailLen)
        {
            memcpy(&(newEntries[headLen]), mEntries, sizeof(void *) * tailLen);
        }

        mHead = 0;
        mTail = headLen + tailLen;

        free(mEntries);
        mEntries = newEntries;
    }

    mEntries[mTail++] = item;
    mTail = mTail % mMaxCount;
    mCount++;
}

void*  CPtrCircleQueue::Pop()
{
    void * ret = NULL;

    if (mCount > 0)
    {
        ret = mEntries[mHead++];
        mHead = mHead % mMaxCount;
        mCount--;
    }

    return ret;
}

void*  CPtrCircleQueue::Front()
{
	void * ret = NULL;

	if (mCount > 0)
	{
		ret = mEntries[mHead];
	}

	return ret;
}

void*  CPtrCircleQueue::Top()
{
    return mCount > 0 ? mEntries[mHead] : NULL;
}

int  CPtrCircleQueue::GetLength()
{
    return mCount;
}


/*******************************************************************************************/
CBlockingQueue::CBlockingQueue()
{
    mQueue = new CPtrCircleQueue();
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mCond, NULL);
}

CBlockingQueue::~CBlockingQueue()
{
    delete mQueue;
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);
}

void  CBlockingQueue::Push(void * item)
{
    pthread_mutex_lock(&mMutex);

    mQueue->Push(item);

    pthread_cond_signal(&mCond);

    pthread_mutex_unlock(&mMutex);
}

void*  CBlockingQueue::Pop()
{
    void * ret = NULL;

    pthread_mutex_lock(&mMutex);

    if(mQueue->GetLength() == 0)
    {
        pthread_cond_wait(&mCond, &mMutex);
    }

    ret = mQueue->Pop();

    pthread_mutex_unlock(&mMutex);

    return ret;
}

//阻塞，若没有数据，则等待，有数据才返回,并且不会删除队列中元素
void* CBlockingQueue::Front() {
	void * ret = NULL;

	pthread_mutex_lock(&mMutex);

	if (mQueue->GetLength() == 0)
	{
		pthread_cond_wait(&mCond, &mMutex);
	}

	ret = mQueue->Front();

	pthread_mutex_unlock(&mMutex);

	return ret;
}

void*  CBlockingQueue::Top()
{
    void * ret = NULL;

    pthread_mutex_lock(&mMutex);

    ret = mQueue->Top();

    pthread_mutex_unlock(&mMutex);

    return ret;
}

int  CBlockingQueue::GetLength()
{
    int len = 0;

    pthread_mutex_lock(&mMutex);

    len = mQueue->GetLength();

    pthread_mutex_unlock(&mMutex);

    return len;
}

