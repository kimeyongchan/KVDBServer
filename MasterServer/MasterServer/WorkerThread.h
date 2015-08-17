#ifndef __WORKER_THREAD_H__
#define __WORKER_THREAD_H__

#include <pthread.h>
#include <deque>

class ConnectInfo;
class DataPacket;

class WorkerThread
{
public:
	WorkerThread();
	~WorkerThread();
	bool Initialize();
	void Run();

    void PushDataPacket(DataPacket* dataPacket);
    DataPacket* PopDataPacket();
    
    virtual void connected(const ConnectInfo* connectInfo) = 0;
    virtual void disconnected(const ConnectInfo* connectInfo) = 0;
    virtual void receiveData(const ConnectInfo* connectInfo, const char* data, int dataSize) = 0;
    
    pthread_t* GetTid() { return &tid; }
    pthread_cond_t* GetCond() { return &cond; }
    int getDataPacketCount();
    
    int tfd;

protected:
	void Lock() { pthread_mutex_lock(&mutex); }
	void UnLock() { pthread_mutex_unlock(&mutex); }
    void UnLockAndWait() { pthread_cond_wait(&cond, &mutex); pthread_mutex_unlock(&mutex); }

protected:
    pthread_t tid;
	pthread_mutex_t mutex;
    pthread_cond_t cond;
    std::deque<DataPacket*>* dataPacketQueue;
};

#endif //__WORKER_THREAD_H__