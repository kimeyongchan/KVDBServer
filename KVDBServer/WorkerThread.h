#ifndef __WORKER_THREAD_H__
#define __WORKER_THREAD_H__

#include <pthread.h>
#include <list>

class RequestInfo;
class IOManager;

class WorkerThread
{
public:
	WorkerThread();
	~WorkerThread();
	bool Initialize();
	void Run();

	void PushRequestInfo(RequestInfo* requestInfo);
	RequestInfo* PopRequestInfo();
    
    pthread_t* GetTid() { return &m_tid; }
    pthread_cond_t* GetCond() { return &m_cond; }
    int GetRequestInfoCount();

private:
	void Lock() { pthread_mutex_lock(&m_mutex); }
	void UnLock() { pthread_mutex_unlock(&m_mutex); }
    void UnLockAndWait() { pthread_cond_wait(&m_cond, &m_mutex); pthread_mutex_unlock(&m_mutex); }

private:
    pthread_t m_tid;
	pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
	std::list<RequestInfo*> m_requestInfoQueue;
    
    IOManager* m_ioMgr;
};

#endif //__WORKER_THREAD_H__