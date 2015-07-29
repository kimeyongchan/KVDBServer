#include "KVDBServer.h"

#include "Network.h"
#include "RequestInfo.h"
#include "RequestHandler.h"
#include "WorkerThread.h"

#include "Log.h"

#define PORT 3307

KVDBServer* KVDBServer::m_instance = NULL;


void* WorkerThreadFunction(void *data)
{
    WorkerThread* wt = (WorkerThread*)data;
    
    wt->Run();
    
    return NULL;
}

bool KVDBServer::Initialize(int workerThreadCount)
{
	m_log = new Log();
	if (m_log->Initialize(".") == false)
	{
		return false;
	}

	RequestHandler* requestHandler = new RequestHandler();
	if (requestHandler->Initialize() == false)
	{
		ErrorLog("request error");
		return false;
	}

	m_network = new Network();
	if (m_network->Initialize(PORT, requestHandler) == false)
	{
		ErrorLog("Network error");
		return false;
	}
    
    m_workerThreadCount = workerThreadCount;

	m_workerThreadArray = new WorkerThread[m_workerThreadCount];

	for (int i = 0; i < m_workerThreadCount; i++)
	{
		if (m_workerThreadArray[i].Initialize() == false)
		{
			ErrorLog("worker thread error");
			return false;
		}
        
		if (pthread_create(m_workerThreadArray[i].GetTid(), NULL, WorkerThreadFunction, (void*)&m_workerThreadArray[i]) != 0)
		{
			ErrorLog("thread create error : ");
			return false;
		}

	}

	return true;
}

void KVDBServer::Run()
{
	while (true)
	{
		m_network->ProcessEvent();
	}
}

void KVDBServer::SendWorkToWorkerThread(RequestInfo* ri)
{
    int firstCnt, secondCnt;
    int fitNum;
    
    firstCnt = m_workerThreadArray[0].GetRequestInfoCount();
    
    if(firstCnt == 0)
    {
        m_workerThreadArray[0].PushRequestInfo(ri);
        return ;
    }
    
    fitNum = 0;
    
    for(int i=1; i < m_workerThreadCount - 1; i++)
    {
        secondCnt = m_workerThreadArray[i].GetRequestInfoCount();
        
        if(secondCnt == 0)
        {
            m_workerThreadArray[i].PushRequestInfo(ri);
            return ;
        }
        
        if(firstCnt > secondCnt)
        {
            secondCnt = firstCnt;
            fitNum = i;
        }
    }
    
    m_workerThreadArray[fitNum].PushRequestInfo(ri);
        
    return ;
}