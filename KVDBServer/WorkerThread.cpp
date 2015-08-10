#include "WorkerThread.h"
#include "Network.h"
#include "Log.h"
#include <stdlib.h>

WorkerThread::WorkerThread()
{
}


WorkerThread::~WorkerThread()
{
	pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
	//ToDo. heap memory (in Queue) destory
}

bool WorkerThread::Initialize()
{
	pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    if(dataPacketQueue.empty() == false)
        dataPacketQueue.clear();
    
	return true;
}


void WorkerThread::Run()
{
	while (true)
	{
		DataPacket* dp = PopDataPacket();
		if (dp != NULL)
		{
            receiveData(dp->connectInfo, dp->data, dp->dataSize);
            free(dp->data);
            delete dp;
		}
	}
}


void WorkerThread::PushDataPacket(DataPacket* dataPacket)
{
    Lock();
    
    dataPacketQueue.push_back(dataPacket);
    
    if(dataPacketQueue.size() == 1)
        pthread_cond_signal(&cond);
    
    UnLock();
}


DataPacket* WorkerThread::PopDataPacket()
{
    Lock();
    
    DataPacket* dp = NULL;
    if (dataPacketQueue.empty() == false)
    {
        dp = dataPacketQueue.front();
        dataPacketQueue.pop_front();
        UnLock();
    }
    else
    {
        UnLockAndWait();
    }
    
    return dp;
}


int WorkerThread::getDataPacketCount()
{
    Lock();
    int cnt = (int)dataPacketQueue.size();
    UnLock();
    return cnt;
}
