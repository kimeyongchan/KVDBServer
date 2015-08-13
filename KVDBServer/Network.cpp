
#include "Network.h"

#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>
#include <arpa/inet.h>

#include "workerThread.h"
#include "Log.h"

#define FLAG_PROCESSING     0x01 // 00000001
#define FLAG_DISCONNECTED   0x02 // 00000010


void* WorkerThreadFunction(void *data)
{
    WorkerThread* wt = (WorkerThread*)data;
    
    wt->Run();
    
    return NULL;
}

long Network::getCustomCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);    // milliseconds 단위로 변환
}

Network::Network()
{
}


Network::~Network()
{
}

//////////////////////////////////////////
#if OS_PLATFORM == PLATFORM_MAC
//////////////////////////////////////////

bool Network::Initialize(const NetworkInfo* _networkInfo, int _networkInfoCount, int _workerThreadCount, WorkerThread* _workerThreadArray, long _sendPingInterval, int _disconnectPingCount)
{
    
    memset(connectInfoList, 0, sizeof(ConnectInfo) * MAX_CONNECT_SIZE);
    
    for(int i = 0; i < MAX_CONNECT_SIZE; i++)
    {
        connectInfoList[i].fd = 0;
        connectInfoList[i].flags = 0;
        connectInfoList[i].serverModule = -1;
        connectInfoList[i].lastPingTime = 0;
        connectInfoList[i].userData = NULL;
        connectInfoList[i].tempDataQueue.clear();
        memset(&connectInfoList[i].tempBufferInfo, 0, sizeof(TempBufferInfo));
        
    }
    
    
    eventFd = kqueue();
    if (eventFd < 0) {
        ErrorLog("kqueue create error.");
        return false;
    }
    
    event = (struct kevent*)malloc(sizeof(struct kevent)*EVENT_BUFFER_SIZE);
    
 
    listenSocketCount = 0;
    
    for (int i = 0; i < _networkInfoCount; i++)
        if(_networkInfo[i].type == SERVER_TYPE_SERVER)
            listenSocketCount++;

    serverConnectInfoList = new ConnectInfo[listenSocketCount]; // (ConnectInfo*)malloc(sizeof(ConnectInfo) * listenSocketCount);
    
    listenSocketCount = 0;

    for(int i=0; i < _networkInfoCount; i++)
    {
        int type = _networkInfo[i].type;
        
        if(type == SERVER_TYPE_SERVER)
        {
            if(AddServerTypeNetworkInfo(&_networkInfo[i]) == false)
            {
                ErrorLog("AddServerType NetworkInfo error");
                return false;
            }
        }
        else if(type == SERVER_TYPE_CLIENT)
        {
            if(AddClientTypeNetworkInfo(&_networkInfo[i]) == false)
            {
                ErrorLog("AddClientType NetworkInfo error");
                return false;
            }
        }
        else
        {
            ErrorLog("type error - %d", type);
            return false;
        }
    }
    
    memset(recvBuffer, 0, RECV_BUF);

    clntaddrLen = sizeof(clntaddr);
    
    
    workerThreadCount = _workerThreadCount;
    
    workerThreadArray = _workerThreadArray;
    
    for (int i = 0; i < workerThreadCount; i++)
    {
        if (workerThreadArray[i].Initialize() == false)
        {
            ErrorLog("worker thread error");
            return false;
        }
        
        if (pthread_create(workerThreadArray[i].GetTid(), NULL, WorkerThreadFunction, (void*)&workerThreadArray[i]) != 0)
        {
            ErrorLog("thread create error : ");
            return false;
        }
        
        workerThreadArray[i].tfd = i+10;
        
        EV_SET(&connectEvent, i+10, EVFILT_USER, EV_ADD | EV_ENABLE, 0, 0, NULL);
        
        if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
        {
            ErrorLog("kevent init error");
            break;
        }
    }

    
    sendPingInterval = _sendPingInterval;
    disconnectPingCount = _disconnectPingCount;

    
    wait.tv_sec = sendPingInterval / 2 / 1000;
    wait.tv_nsec = ((sendPingInterval / 2) % 1000) * 1000000;
    
    lastPingCheckTime = getCustomCurrentTime();
    
	return true;
}

bool Network::AddNetworkInfo(const NetworkInfo* _networkInfo)
{
    if(_networkInfo->type == SERVER_TYPE_SERVER)
    {
        serverConnectInfoList = (ConnectInfo*)realloc(serverConnectInfoList, sizeof(ConnectInfo) * (listenSocketCount + 1));
        if(AddServerTypeNetworkInfo(_networkInfo) == false)
        {
            ErrorLog("AddServerType NetworkInfo error");
            serverConnectInfoList = (ConnectInfo*)realloc(serverConnectInfoList, sizeof(ConnectInfo) * (listenSocketCount));
            return false;
        }
    }
    else
    {
        if(AddClientTypeNetworkInfo(_networkInfo) == false)
        {
            ErrorLog("AddClientType NetworkInfo error");
            return false;
        }
    }
    
    return true;
}

bool Network::AddServerTypeNetworkInfo(const NetworkInfo* _networkInfo)
{
    serverConnectInfoList[listenSocketCount].fd = CreateTCPServerSocket(_networkInfo->ip, _networkInfo->port);
    if (serverConnectInfoList[listenSocketCount].fd < 0)
    {
        ErrorLog("create sock error");
        return false;
    }
    
    serverConnectInfoList[listenSocketCount].serverModule = _networkInfo->module;
    
    EV_SET(&connectEvent, serverConnectInfoList[listenSocketCount].fd, EVFILT_READ, EV_ADD, 0, 0, (void*)&serverConnectInfoList[listenSocketCount]);
    
    if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
    {
        ErrorLog("kevent init error");
//        break;
    }
    
    listenSocketCount++;
    
    return true;
}

bool Network::AddClientTypeNetworkInfo(const NetworkInfo* _networkInfo)
{
    int fd = CreateTCPClientSocket(_networkInfo->ip, _networkInfo->port);
    if(fd < 0)
    {
        ErrorLog("create sock error");
        return false;
    }
    
    ConnectInfo* connectInfo = AddClientPool(fd);
    if(connectInfo == NULL)
    {
        ErrorLog("add pool error");
        return false;
    }
    
    connectInfoList->serverModule = _networkInfo->module;
    
//    _networkInfo->receiveHandler->IsConnected(connectInfoList);
    
    EV_SET(&connectEvent, connectInfoList->fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void*)connectInfoList);
    
    if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
    {
        ErrorLog("kevent init error");
        //        break;
    }
    
    return true;
}


void Network::ProcessEvent()
{
	int eventCnt;
    long currentTime;
	while(true)
	{
        eventCnt = kevent(eventFd, NULL, 0, event, EVENT_BUFFER_SIZE, &wait);
        
        currentTime = getCustomCurrentTime();
        
		if (eventCnt < 0)
		{
			ErrorLog("kevent wait fail.");
			continue;
		}
        else if(eventCnt == 0)
        {
            if(currentTime - lastPingCheckTime > sendPingInterval)
            {
                pingCheck();
            }
        }

		for (int i = 0; i < eventCnt; i++)
        {
			if (event[i].filter == EVFILT_READ)
            {
                bool isServer = false;
                
                for(int i = 0; i < listenSocketCount; i++)
                {
                    if(event[i].ident == serverConnectInfoList[i].fd) // Server sock
                    {
                        isServer = true;
                        
                        clntFd = accept(serverConnectInfoList[i].fd, (struct sockaddr *)&clntaddr, (socklen_t*)&clntaddrLen);
                        if (clntFd < 0)
                        {
                            ErrorLog("accept fail.");
                            break;
                        }
                        
                        ConnectInfo* connectInfo = AddClientPool(clntFd);
                        
                        if (connectInfo == NULL)
                        {
                            ErrorLog("AddClientPool Error");
                            close(clntFd);
                            break;
                        }
                        
                        connectInfo->serverModule = serverConnectInfoList[i].serverModule;
                        
                        
                        EV_SET(&connectEvent, clntFd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, (void*)connectInfo);
                    
                        if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
                        {
                            ErrorLog("kevent init error");
                            break;
                        }
                        
//                        receiveHandlerPointerList[connectInfo->serverModule]->IsConnected(connectInfo);
                        
                        break;
                    }
                }
                
                
                if(isServer) continue;
                
                
                
                clntFd = (int)event[i].ident;
                
                ConnectInfo* connectInfo = (ConnectInfo*)event[i].udata;
                
                
                int readCnt = 0;
                
                readCnt = (int)recv(clntFd, recvBuffer, RECV_BUF - 1, 0);
                
                
                if (readCnt > 0)
                {
                    char* wholeRecvBuffer = recvBuffer;
                    int wholeRecvBufferSize = readCnt;
                    int recvCnt = readCnt;
                    char* allocateRecvBuffer = NULL;
                    
                    
                    TempBufferInfo* tempBufferInfo = &connectInfo->tempBufferInfo;
                    
                    int tempBufferSize = tempBufferInfo->tempBufferSize;
                    
               /*     bool isTempBufferInfoEmpty;
                    if(tempBufferSize == 0)
                        isTempBufferInfoEmpty = true;
                    else
                        isTempBufferInfoEmpty = false;
               */
                    
                    if(tempBufferSize != 0)
                    {
                        wholeRecvBuffer = (char*)malloc(sizeof(char) * (recvCnt + tempBufferSize));
                        
                        memcpy(wholeRecvBuffer, tempBufferInfo->tempBuffer, tempBufferSize);
                        memcpy(wholeRecvBuffer + tempBufferSize, recvBuffer, recvCnt);
                        wholeRecvBufferSize += tempBufferInfo->tempBufferSize;
                        
                        connectInfo->tempBufferInfo.tempBufferSize = 0;
                        free(connectInfo->tempBufferInfo.tempBuffer);
                        connectInfo->tempBufferInfo.tempBuffer = NULL;
                        
                        allocateRecvBuffer = wholeRecvBuffer;
                    }
                    
                    char* pWholeRecvBuffer = wholeRecvBuffer;
                    int pWholeRecvBufferSize = wholeRecvBufferSize;
                    
                    while(true)
                    {
                        if(pWholeRecvBufferSize < sizeof(dataType_t))
                        {
                            connectInfo->tempBufferInfo.tempBufferSize = wholeRecvBufferSize;
                            connectInfo->tempBufferInfo.tempBuffer = (char*)malloc(sizeof(char) * wholeRecvBufferSize);
                            memcpy(connectInfo->tempBufferInfo.tempBuffer, wholeRecvBuffer, wholeRecvBufferSize);
                            
                            //memset(recvBuffer, 0, readCnt);
                            
                            break;
                        }
                        
                        ////////////////////////// data type
                        dataType_t flags;
                        memcpy(&flags, pWholeRecvBuffer, sizeof(flags));
                        pWholeRecvBuffer += sizeof(flags);
                        pWholeRecvBufferSize -= sizeof(flags);
                        
                        if(flags == DATA_TYPE_PING_NOTIFY)
                        {
                            DebugLog("ping noti");
                            dataType_t dataTypeToSend = DATA_TYPE_PING_OK;
                            sendData(connectInfo, (char*)&dataTypeToSend, sizeof(dataTypeToSend));
                            
                            if(pWholeRecvBufferSize == 0)
                            {
                                break;
                            }
                            else if(pWholeRecvBufferSize > 0) // 남은 데이터들 존재함
                            {
                                wholeRecvBufferSize = pWholeRecvBufferSize;
                                wholeRecvBuffer = pWholeRecvBuffer;
                                
                                continue;
                            }
                            else
                            {
                                ErrorLog("pWholeRecvBufferSize - %d", pWholeRecvBufferSize);
                                //return ;
                            }
                            
                        }
                        else if (flags == DATA_TYPE_PING_OK)
                        {
                            connectInfo->sendPingCount = 0;
                            
                            if(pWholeRecvBufferSize == 0)
                            {
                                break;
                            }
                            else if(pWholeRecvBufferSize > 0) // 남은 데이터들 존재함
                            {
                                wholeRecvBufferSize = pWholeRecvBufferSize;
                                wholeRecvBuffer = pWholeRecvBuffer;
                                
                                continue;
                            }
                            else
                            {
                                ErrorLog("pWholeRecvBufferSize - %d", pWholeRecvBufferSize);
                                //return ;
                            }
                        }
                        else if(flags == DATA_TYPE_REQ)
                        {
                            if(pWholeRecvBufferSize < sizeof(dataSize_t))
                            {
                                connectInfo->tempBufferInfo.tempBufferSize = wholeRecvBufferSize;
                                connectInfo->tempBufferInfo.tempBuffer = (char*)malloc(wholeRecvBufferSize);
                                memcpy(connectInfo->tempBufferInfo.tempBuffer, wholeRecvBuffer, wholeRecvBufferSize);
                                
                                break;
                            }
                            
                            dataSize_t dataSize;
                            memcpy(&dataSize, pWholeRecvBuffer, sizeof(dataSize));
                            pWholeRecvBuffer += sizeof(dataSize);
                            pWholeRecvBufferSize -= sizeof(dataSize);
                        
                            //int recvDataSize = pWholeRecvBufferSize - sizeof(dataSize);
                        
                            if(dataSize <= pWholeRecvBufferSize)
                            {
                                
                                if((connectInfo->flags & FLAG_PROCESSING) != 0)
                                {
                                    TempBufferInfo* saveData = new TempBufferInfo();
                                    saveData->tempBufferSize = (int)dataSize;
                                    saveData->tempBuffer = (char*)malloc((int)dataSize);
                                    memcpy(saveData->tempBuffer, pWholeRecvBuffer, (int)dataSize);
                                    connectInfo->tempDataQueue.push_back(saveData);
                                }
                                else
                                {
                                    connectInfo->flags |= FLAG_PROCESSING;
                                    sendDataToWorkerThread(connectInfo, pWholeRecvBuffer, (int)dataSize);
                                }
                                
                                pWholeRecvBuffer += dataSize;
                                pWholeRecvBufferSize -= dataSize;
                                
                                if(pWholeRecvBufferSize == 0)
                                {
                                    break;
                                }
                                else if(pWholeRecvBufferSize > 0)
                                {
                                    wholeRecvBufferSize = pWholeRecvBufferSize;
                                    wholeRecvBuffer = pWholeRecvBuffer;
                                    
                                    continue;
                                }
                                else
                                {
                                    ErrorLog("pWholeRecvBufferSize - %d", pWholeRecvBufferSize);
                                }
                            }
                            else
                            {
                                connectInfo->tempBufferInfo.tempBufferSize = wholeRecvBufferSize;
                                connectInfo->tempBufferInfo.tempBuffer = (char*)malloc(wholeRecvBufferSize);
                                memcpy(connectInfo->tempBufferInfo.tempBuffer, wholeRecvBuffer, wholeRecvBufferSize);
                                
                                break;
                            }
                        }
                    
                    }
                    
                    if(allocateRecvBuffer != NULL)
                    {
                        free(allocateRecvBuffer);
                    }
                }
                else if (readCnt == 0)
                {
                    EV_SET(&connectEvent, clntFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    
                    
                    if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
                    {
                        ErrorLog("kevent init error");
                    }
                    
                    if((connectInfo->flags & FLAG_PROCESSING) != 0)
                    {
                        connectInfo->flags |= FLAG_DISCONNECTED;
                    }
                    else
                    {
                        if(DelClientPool(clntFd) == false)
                        {
                            ErrorLog("DelClient error %d", clntaddr);
                        }
                    }
                }
                else
                {
                    EV_SET(&connectEvent, clntFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    
                    if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
                    {
                        ErrorLog("kevent init error");
                    }
                    
                    if((connectInfo->flags & FLAG_PROCESSING) != 0)
                    {
                        connectInfo->flags |= FLAG_DISCONNECTED;
                    }
                    else
                    {
                        if(DelClientPool(clntFd) == false)
                        {
                            ErrorLog("DelClient error %d", clntaddr);
                        }
                    }
                }
            }
            else if(event[i].filter == EVFILT_USER)
            {
                EV_SET(&connectEvent, event[i].ident, EVFILT_USER, EV_DISABLE, EV_CLEAR, NOTE_TRIGGER, NULL);
                if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
                {
                    ErrorLog("kevent init error");
                }
                
                ConnectInfo* connectInfo = (ConnectInfo*)event[i].udata;
                
                if((connectInfo->flags & FLAG_DISCONNECTED) != 0)
                {
                    DelClientPool(connectInfo->fd);
                    continue ;
                }
                
                if(connectInfo->tempDataQueue.empty() == false)
                {
                    TempBufferInfo* tbi = connectInfo->tempDataQueue.front();
                    connectInfo->tempDataQueue.pop_front();
                    sendDataToWorkerThread(connectInfo, tbi->tempBuffer, tbi->tempBufferSize);
                    
                    delete tbi;
                }
                else
                {
                    connectInfo->flags = 0;
                }
            }
        }
        
        if(currentTime - lastPingCheckTime > sendPingInterval)
        {
            pingCheck();
        }
    }
}

void Network::sendDataToWorkerThread(ConnectInfo* const _connectInfo, const char* _data, int _dataSize)
{
    DataPacket* dp = new DataPacket();
    dp->connectInfo = _connectInfo;
    dp->dataSize = _dataSize;
    dp->data = (char*)malloc(dp->dataSize);
    memcpy(dp->data, _data, dp->dataSize);
    
    
    int firstCnt, secondCnt;
    int fitNum;
    
    firstCnt = INT_MAX;
    secondCnt = 0;
    fitNum = 0;
    
    for(int i = 0; i < workerThreadCount; i++)
    {
        secondCnt = workerThreadArray[i].getDataPacketCount();
        
        if(firstCnt > secondCnt)
        {
            secondCnt = firstCnt;
            fitNum = i;
        }
    }
    
    workerThreadArray[fitNum].PushDataPacket(dp);
    
    return ;
}

void Network::sendData(const ConnectInfo* connectInfo, const char* data, int dataSize)
{
    ssize_t sendCnt = send(connectInfo->fd, (void*)data, dataSize, NULL);
    if(sendCnt < 0)
    {
        DebugLog("%d, %d", connectInfo->fd, connectInfo->flags);
    }
}

void Network::sendData(int threadId, const ConnectInfo* connectInfo, const char* data, int dataSize)
{
    DataHeader dataHeader;
    dataHeader.dataType = DATA_TYPE_REQ;
    dataHeader.dataSize = dataSize;
    char sendDataArray[dataSize + sizeof(dataHeader)];
    memcpy(sendDataArray, &dataHeader, sizeof(dataHeader));
    memcpy(sendDataArray + sizeof(dataHeader), data, dataSize);
    
    sendData(connectInfo, sendDataArray, sizeof(dataHeader) + dataSize);
    
    EV_SET(&connectEvent, threadId, EVFILT_USER, EV_ENABLE, NOTE_TRIGGER, 0, (void*)connectInfo);

    if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
    {
        ErrorLog("kevent init error");
        return ;
    }
}


void Network::pingCheck()
{
//    DebugLog("ping check");
    long currentTime = getCustomCurrentTime();
    
    dataType_t dataTypeToSend = DATA_TYPE_PING_NOTIFY;
    
    for(int j = 0; j < listenSocketCount; j++)
    {
        for (int i = 0; i < MAX_CONNECT_SIZE; i++) {
            if (connectInfoList[i].fd != 0 && connectInfoList[i].serverModule == serverConnectInfoList[j].serverModule && (connectInfoList[i].flags & FLAG_DISCONNECTED) == 0)
            {
                if(connectInfoList[i].sendPingCount >= disconnectPingCount)
                {
                    EV_SET(&connectEvent, connectInfoList[i].fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    
                    if (kevent(eventFd, &connectEvent, 1, NULL, 0, NULL) == -1)
                    {
                        ErrorLog("kevent init error");
                    }
                    
                    if((connectInfoList[i].flags & FLAG_PROCESSING) != 0)
                    {
                        connectInfoList[i].flags |= FLAG_DISCONNECTED;
                    }
                    else
                    {
                        if(DelClientPool(clntFd) == false)
                        {
                            ErrorLog("DelClient error %d", clntaddr);
                        }
                    }
                }
                else
                {
                    sendData(&connectInfoList[i], (char*)&dataTypeToSend, sizeof(dataTypeToSend));
                    
                }
            }
        }
    }
    
    lastPingCheckTime = currentTime;
}


int Network::CreateTCPServerSocket(const char* ip, unsigned short port)
{
	int sock;
	struct sockaddr_in servaddr;

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("socket() failed.\n");
		return -1;
	}

	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in)) < 0)
	{
		printf("Bind() failed.\n");
		return -2;
	}

	if (listen(sock, SOMAXCONN)) {
		printf("Listen() failed.\n");
		return -3;
	}

	return sock;
}

int Network::CreateTCPClientSocket(const char* ip, unsigned short port)
{
    int sock;
    struct sockaddr_in addr;
    
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("socket() failed.\n");
        return -1;
    }
    
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("can't connect.\n");
        return -2;
    }
    
    return sock;
}

ConnectInfo* Network::AddClientPool(int fd)
{
	for (int i = 0; i < MAX_CONNECT_SIZE; i++) {
		if (connectInfoList[i].fd == 0) {
			connectInfoList[i].fd = fd;
			return &connectInfoList[i];
		}
	}

	return NULL;
}

bool Network::DelClientPool(int fd)
{
	for (int i = 0; i < MAX_CONNECT_SIZE; i++) {
		if (connectInfoList[i].fd == fd) {
			connectInfoList[i].fd = 0;
            connectInfoList[i].flags = 0;
            connectInfoList[i].serverModule = -1;
            connectInfoList[i].sendPingCount = 0;
            connectInfoList[i].lastPingTime = 0;
            connectInfoList[i].userData = NULL;
            //ToDo. memory delete queue
            connectInfoList[i].tempDataQueue.clear();
            if(connectInfoList[i].tempBufferInfo.tempBufferSize != 0)
            {
                connectInfoList[i].tempBufferInfo.tempBufferSize = 0;
                free(connectInfoList[i].tempBufferInfo.tempBuffer);
            }
            memset(&connectInfoList[i].tempBufferInfo, 0, sizeof(TempBufferInfo));
			return true;
		}
	}

	return false;
}

bool Network::GetClientFd(int fd)
{
	int	i;
	for (i = 0; i < MAX_CONNECT_SIZE; i++) {
		if (connectInfoList[i].fd == fd) {
			return true;
		}
	}

	return false;
}








//////////////////////////////////////////
#elif OS_PLATFORM == PLATFORM_LINUX
//////////////////////////////////////////

bool Network::Initialize(int port, ReceiveHandler* recvHandler)
{
    m_port = port;
    
    m_sockFd = CreateTCPServerSocket(port);
    if (m_sockFd < 0)
    {
        ErrorLog("create sock error");
        return false;
    }
    
    m_eventFd = epoll_create(EPOLL_SIZE);
    if (m_eventFd < 0) {
        perror("epoll create error.");
        exit(2);
    }
    
    m_event = (struct epoll_event*)malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
    
    memset(&m_connectEvent, 0, sizeof(struct epoll_event));
    
    m_connectEvent.events = EPOLLIN;
    m_connectEvent.data.fd = m_sockFd;
    epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_sockFd, &m_connectEvent);
    
    m_clntaddrLen = sizeof(m_clntaddr);
    
    memset(m_connectInfo, 0, sizeof(ConnectInfo) * MAX_CONNECT_SIZE);
    memset(m_recvBuffer, 0, RECV_BUF);
    
    m_recvHandler = recvHandler;
    
    return true;
}

void Network::ProcessEvent()
{
    int eventCnt;
    while(true)
    {
        eventCnt = epoll_wait(m_epollFd, m_event, EPOLL_SIZE, -1);
        if (eventCnt <= 0)
        {
            perror("epoll_wait fail.");
            continue;
        }
        
        for (int i = 0; i < eventCnt; i++) {
            if (m_event[i].data.fd == m_sockFd) {		// Server sock
                
                m_clntFd = accept(m_sockFd, (struct sockaddr *)&m_clntaddr, &m_clntaddrLen);
                if (m_clntFd < 0)
                {
                    perror("accept fail.");
                }
                else
                {
                    result = AddClientPool(m_clntFd);
                    if (result < 0)
                    {
                        perror("AddClientPool Error");
                        close(m_clntFd);
                    }
                    else
                    {
                        m_connectEvent.events = EPOLLIN;
                        m_connectEvent.data.fd = m_clntFd;
                        result = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_clntFd, &m_connectEvent);
                    }
                }
            }
            else {
                m_clntFd = m_event[i].data.fd;
                
                bool isExist = GetClientFd(m_clntFd);
                if (isExist == false)
                {
                    exit(0);
                }
                int wholeQueryLen = 0;
                int readCnt = 0;
                while (true)
                {
                    readCnt = read(m_clntFd, m_recvBuffer + wholeQueryLen, RECV_BUF);
                    if (readCnt < 0)
                    {
                        printf("ERR : read fail (%d)\n", readCnt);
                        epoll_ctl(m_epollFd, EPOLL_CTL_DEL, m_clntFd, m_event);
                        DelClientPool(m_clntFd);
                        exit(0);
                    }
                    else if (readCnt == 0)
                    {
                        //	printf("ERR : client disconnect idx(%d)\n", idx);
                        //	epoll_ctl(m_epollFd, EPOLL_CTL_DEL, m_clntFd, m_event);
                        //	DelClientPool(m_clntFd);
                        break;
                    }
                    else
                    {
                        wholeQueryLen += readCnt;
                        ///¥˙ø‘¿ªºˆµµ ¿÷¿∏¥œ ø©∑Øπ¯ ¿–æÓº≠ «œ¥¬ ∞˙¡§ ºˆ«‡«ÿæﬂµ»¥Ÿ.
                    }
                }
                
                m_recvHandler->Parse(&m_clntFd, m_recvBuffer, wholeQueryLen);
            }
        }
    }
}

#else
#endif
