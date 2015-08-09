#ifndef __NETWORK_H__
#define __NETWORK_H__


#include <stdInt.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <deque>

#include "BasicDefines.h"

#define MAX_IP_LEN 15
#define EVENT_BUFFER_SIZE 50
#define MAX_CONNECT_SIZE 100
#define RECV_BUF 5000

class WorkerThread;

typedef uint64_t dataSize_t;

struct TempBufferInfo
{
    int tempBufferSize;
    char* tempBuffer;
};


struct ConnectInfo
{
    int fd;
    int serverModule;
    char flags;
    TempBufferInfo tempBufferInfo;
    std::deque<TempBufferInfo*> tempDataQueue;
};


struct DataPacket
{
    ConnectInfo* connectInfo;
    char* data;
    int dataSize;
};

enum SERVER_TYPE
{
    SERVER_TYPE_SERVER = 0,
    SERVER_TYPE_CLIENT,
};

enum IS_PROCESSING
{
    NOT_PROCESSING = 0,
    PROCESSING,
};


struct NetworkInfo
{
    int module;
    int type;
    char ip[MAX_IP_LEN + 1];
    int port;
};


class Network
{
public:
	Network();
	~Network();
	bool Initialize(const NetworkInfo* _networkInfoList, int _networkInfoCount, int _workThreadCount, WorkerThread* _workerThreadArray);
    bool AddNetworkInfo(const NetworkInfo* _networkInfo);
	void ProcessEvent();
    void SendData(const ConnectInfo* connectInfo, const char* data, int dataSize);

private:
	int CreateTCPServerSocket(const char* ip, unsigned short port);
    int CreateTCPClientSocket(const char* ip, unsigned short port);
    bool AddServerTypeNetworkInfo(const NetworkInfo* _networkInfo);
    bool AddClientTypeNetworkInfo(const NetworkInfo* _networkInfo);
    
    
	ConnectInfo* AddClientPool(int fd);
	bool DelClientPool(int fd);
	bool GetClientFd(int fd);
    
    void sendDataToWorkerThread(ConnectInfo* const _connectInfo, const char* _data, int _dataSize);

private:
	int eventFd;
	int clntFd;
	struct sockaddr_in clntaddr;
	int clntaddrLen;
    int listenSocketCount;
	
    ConnectInfo* serverConnectInfoList;
	ConnectInfo connectInfoList[MAX_CONNECT_SIZE];

	char recvBuffer[RECV_BUF];
    char tempRecvBuffer[RECV_BUF];
    
    int workerThreadCount;
    WorkerThread* workerThreadArray;

    
#if OS_PLATFORM == PLATFORM_LINUX
    
    struct epoll_event* event;
    struct epoll_event connectEvent;
    
#elif OS_PLATFORM == PLATFORM_MAC
    
    struct kevent* event;
    struct kevent connectEvent;
#else
    
#endif
    
};

#endif //_NETWORK_H__