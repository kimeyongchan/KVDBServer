#include "WorkerThread.h"

#include <unistd.h>

#include "Network.h"
#include "RequestInfo.h"
#include "IOManager.h"
#include "Log.h"

WorkerThread::WorkerThread()
{
}


WorkerThread::~WorkerThread()
{
	pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
	//ToDo. heap memory (in Queue) destory
}

bool WorkerThread::Initialize()
{
	pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
    dataPacketQueue.clear();

    //m_ioMgr = new IOManager();
    
	return true;
}

void WorkerThread::Run()
{
	while (true)
	{
		DataPacket* dp = PopDataPacket();
		if (dp != NULL)
		{
            /*
            switch (requestInfo->type) {
                case INSERT_REQUEST:
                {
                    InsertRequestInfo * iri = (InsertRequestInfo*)requestInfo;
                    m_ioMgr->processInsert(iri);
                    delete iri;
                    break;
                }
                case INSERT_DIRECTORY_REQUEST:
                {
                    InsertDirectoryRequestInfo * idri = (InsertDirectoryRequestInfo*)requestInfo;
                    m_ioMgr->processInsert(idri);
                    delete idri;
                    break;
                }
                case FIND_REQUEST:
                {
                    FindRequestInfo * fri = (FindRequestInfo*)requestInfo;
                    m_ioMgr->processFind(fri);
                    delete fri;
                    break;
                }
                case DELETE_REQUEST:
                {
                    DeleteRequestInfo * dri = (DeleteRequestInfo*)requestInfo;
                    m_ioMgr->processDelete(dri);
                    delete dri;
                    break;
                }
                default:
                {
                    ErrorLog("type - %d", requestInfo->type);
                    break;
                }
            }*/
            
            receiveData(dp->connectInfo, dp->data, dp->dataSize);
            
            
		}
	}
}

/*
void WorkerThread::PushRequestInfo(RequestInfo* requestInfo)
{
	Lock();

	m_requestInfoQueue.push_back(requestInfo);
    
    if(m_requestInfoQueue.size() == 1)
        pthread_cond_signal(&m_cond);

	UnLock();
}

RequestInfo* WorkerThread::PopRequestInfo()
{
	Lock();

	RequestInfo* ri = m_requestInfoQueue.front();
	if (ri != NULL)
    {
		m_requestInfoQueue.pop_front();
        UnLock();
    }
    else
    {
        UnLockAndWait();
    }

	return ri;
}
*/

void WorkerThread::PushDataPacket(DataPacket* dataPacket)
{
    Lock();
    
    dataPacketQueue.push_back(dataPacket);
    
    if(dataPacketQueue.size() == 1)
        pthread_cond_signal(&m_cond);
    
    UnLock();
}


DataPacket* WorkerThread::PopDataPacket()
{
    Lock();
    
    DataPacket* dp = dataPacketQueue.front();
    if (dp != NULL)
    {
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


/*int WorkerThread::GetRequestInfoCount()
{
    Lock();
    int cnt = (int)m_requestInfoQueue.size();
    UnLock();
    return cnt;
}*/
