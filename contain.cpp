#include "./include/contain.h"

CBuffer::CBuffer()
{
    m_nBufferSize = 0;
    m_nDataSize = 0;
    m_pBase = NULL;
    //��ʼ��������
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
// ��������:    GetBufferSize
//
// ����˵��:    ��ȡ�ֽڻ������Ĵ�С
//
// ����˵��:
//
// �� �� ֵ:    �ֽڻ������Ĵ�С
//
// ��Ҫ˵��:
//
////////////////////////////////////////////////////////////////////////////////
int  CBuffer::GetBufferSize()
{
    return m_nBufferSize;
}

////////////////////////////////////////////////////////////////////////////////
//
// ��������:    GetDataLength
//
// ����˵��:    ��ȡ���������������ݳ���
//
// ����˵��:
//
// �� �� ֵ:    ���ݳ���
//
// ��Ҫ˵��:
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
// ��������:    ClearBuffer
//
// ����˵��:    ������ݣ��ͷŻ������ռ�
//
// ����˵��:
//
// �� �� ֵ:    ��
//
// ��Ҫ˵��:
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
// ��������:    ClearData
//
// ����˵��:    ������ݣ������ͷŻ������ռ�
//
// ����˵��:
//
// �� �� ֵ:    ��
//
// ��Ҫ˵��:
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
// ��������:    GetBuffer
//
// ����˵��:    ��ȡ������ָ��
//
// ����˵��:
//
// �� �� ֵ:    ��ȡ�Ļ�����ָ��
//
// ��Ҫ˵��:
//
////////////////////////////////////////////////////////////////////////////////
void*  CBuffer::GetBuffer()
{
   return m_pBase;
}


////////////////////////////////////////////////////////////////////////////////
//
// ��������:    Delete
//
// ����˵��:    �ӻ�����ͷ��ɾ�����ݣ��������ռ䲻��
//
// ����˵��:    nLength       ��Ҫɾ���ĳ��ȣ����ܴ������ݳ��ȣ�����������
//                            ���ȣ���ɾ��ʧ��
//
// �� �� ֵ:    true: �ɹ�    false: ʧ��
//
// ��Ҫ˵��:
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
// ��������:    DeleteTail
//
// ����˵��:    �ӻ�����β��ɾ�����ݣ��������ռ䲻��
//
// ����˵��:    nLength       ��Ҫɾ���ĳ��ȣ����ܴ������ݳ��ȣ�����������
//                            ���ȣ���ɾ��ʧ��
//
// �� �� ֵ:    true: �ɹ�    false: ʧ��
//
// ��Ҫ˵��:
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
// ��������:    Read
//
// ����˵��:    �ӻ���ͷ����������,�����������Ӧ����
//
// ����˵��:    pData          ��Ŷ�ȡ���ݵĻ�����ָ��
//              nSize          ����������
//
// �� �� ֵ:    ʵ�ʶ�ȡ�������ݳ��ȣ����������ʵ�ʱ�������ݳ���
//
// ��Ҫ˵��:
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
// ��������:    ReadLine
//
// ����˵��:    �ӻ���ͷ����ʼ����һ������
//
// ����˵��:
//
// �� �� ֵ:    ��ȡ��һ�����ݣ���δ��ȡ�����򷵻�NULL
//
// ��Ҫ˵��:    ���ص��������¿��ٵĿռ䣬��Ҫ�Լ���������������а�������
//              �н�����
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

    if((lineBuf = (char*)malloc(i+2)) == NULL)  //����ռ�ʧ��
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
// ��������:    Insert
//
// ����˵��:    �ӻ���ͷ����������
//
// ����˵��:    pData          �����������ݵ�ָ��
//              nSize          ��������ݳ���
//
// �� �� ֵ:    true   ����ɹ�     false   ����ʧ��
//
// ��Ҫ˵��:
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
// ��������:    Write
//
// ����˵��:    �ӻ���β��д������
//
// ����˵��:    pData          �����д�����ݵ�ָ��
//              nSize          ��д�����ݳ���
//
// �� �� ֵ:    true   д��ɹ�     false   д��ʧ��
//
// ��Ҫ˵��:
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
// ��������:    Write
//
// ����˵��:    �ӻ���β��д������
//
// ����˵��:    pBuffer        �������ݵĻ������
//
// �� �� ֵ:    true   д��ɹ�     false   д��ʧ��
//
// ��Ҫ˵��:
//
////////////////////////////////////////////////////////////////////////////////
bool  CBuffer::Write(CBuffer *pBuffer)
{
     return Write(pBuffer->GetBuffer(), pBuffer->GetDataLength());
}



////////////////////////////////////////////////////////////////////////////////
//
// ��������:    Compress
//
// ����˵��:    ѹ���������ռ䵽���ݳ��ȴ�С
//
// ����˵��:
//
// �� �� ֵ:    true   ѹ���ɹ�     false   ѹ��ʧ��
//
// ��Ҫ˵��:
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
// ��������:    Copy
//
// ����˵��:    ��һ���������������ݵ���һ��������
//
// ����˵��:
//
// �� �� ֵ:    true   �����ɹ�     false   ����ʧ��
//
// ��Ҫ˵��:
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
        m_pBase = calloc(nSrcBufferSize,sizeof(char));    //�ռ��Զ���ʼ��Ϊ0
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
// ��������:    Find
//
// ����˵��:    �ڻ������в���һ���ִ�
//
// ����˵��:    key          ��Ҫ���ҵ��ִ�
//              nPos         ���ҿ�ʼλ��
//
// �� �� ֵ:    ��δ���ҵ����򷵻�-1�����򷵻��ִ���λ��ƫ�ƣ���λ��ƫ���Ѿ�����
//              ���ִ��ĳ���
//
// ��Ҫ˵��:
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
    if (nRequestedSize < m_nBufferSize) //���������󣬻�����Ҫ���ӻ�����
    {
        return true;
    }

    int nNewSize = (int) ceil(nRequestedSize / (BUFFER_STEP_LENGTH * 1.0)) * BUFFER_STEP_LENGTH;

    //�����µĻ�����
    char *pTemp = (char*)calloc(nNewSize,sizeof(char));
    if(pTemp != NULL)
    {
        memcpy(pTemp, (char*)m_pBase, m_nDataSize);  //�Ѿ����ݿ������»�����

        free(m_pBase);  //�����ǰ������
        m_nBufferSize = nNewSize;
        m_pBase = pTemp;

        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
//
// ��������:    WriteToFile
//
// ����˵��:    �ѻ���������д���
//
// ����˵��:
//
// �� �� ֵ:    true   д��ɹ�     false   д��ʧ��
//
// ��Ҫ˵��:
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

//��������û�����ݣ���ȴ��������ݲŷ���,���Ҳ���ɾ��������Ԫ��
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

