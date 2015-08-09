#ifndef __WORKER_THREAD_H__
#define __WORKER_THREAD_H__

#include <pthread.h>
#include <list>
#include <deque>

class RequestInfo;
class IOManager;
class ConnectInfo;

#include "Network.h"

class WorkerThread
{
public:
	WorkerThread();
	~WorkerThread();
	bool Initialize();
	void Run();

    void PushDataPacket(DataPacket* dataPacket);
    DataPacket* PopDataPacket();
    
    virtual void receiveData(const ConnectInfo* connectInfo, const char* data, int dataSize) = 0;
    
//	void PushRequestInfo(RequestInfo* requestInfo);
//	RequestInfo* PopRequestInfo();
    
    pthread_t* GetTid() { return &m_tid; }
    pthread_cond_t* GetCond() { return &m_cond; }
//    int GetRequestInfoCount();
    int getDataPacketCount();

private:
	void Lock() { pthread_mutex_lock(&m_mutex); }
	void UnLock() { pthread_mutex_unlock(&m_mutex); }
    void UnLockAndWait() { pthread_cond_wait(&m_cond, &m_mutex); pthread_mutex_unlock(&m_mutex); }

private:
    pthread_t m_tid;
	pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
//    std::deque<RequestInfo*> m_requestInfoQueue;
    std::list<DataPacket*> dataPacketQueue;
    
//    IOManager* m_ioMgr;
};

#endif //__WORKER_THREAD_H__