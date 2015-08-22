#ifndef __NETWORK_H__
#define __NETWORK_H__


#include <stdInt.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <deque>

#include "BasicDefines.h"
#include "WorkerThread.h"

#define MAX_IP_LEN 15
#define EVENT_BUFFER_SIZE 50
#define MAX_CONNECT_SIZE 100
#define MAX_SERVER_CONNECT_COUNT 10
#define RECV_BUF 5000

typedef int8_t dataType_t;
typedef uint64_t dataSize_t;

#pragma pack(push, 1)
struct DataHeader
{
    dataType_t dataType;
    dataSize_t dataSize;
};
#pragma pack(pop)

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
    int sendPingCount;
    time_t lastPingTime;
    void* userData;
    TempBufferInfo tempBufferInfo;
    std::deque<TempBufferInfo*> tempDataQueue;
};


struct DataPacket
{
    int receiveType;
    ConnectInfo* connectInfo;
    char* data;
    int dataSize;
};


enum SERVER_TYPE
{
    SERVER_TYPE_SERVER = 0,
    SERVER_TYPE_CLIENT,
};

enum DATA_TYPE
{
    DATA_TYPE_PING_NOTIFY = 0,
    DATA_TYPE_PING_OK,
    DATA_TYPE_REQ,
};

enum RECEIVE_TYPE
{
    RECEIVE_TYPE_RECEIVE = 0,
    RECEIVE_TYPE_CONNECT,
    RECEIVE_TYPE_DISCONNECT,
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
	bool Initialize(const NetworkInfo* _networkInfoList, int _networkInfoCount, int _workThreadCount, WorkerThread** _workerThreadArray, long _sendPingInterval, int _disconnectPingCount);
    bool AddNetworkInfo(const NetworkInfo* _networkInfo);
	void ProcessEvent();
    void sendData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    void sendData(int threadId, const ConnectInfo* connectInfo, const char* data, int dataSize);
    void finishProcessing(int threadId, const ConnectInfo* connectInfo);

private:
	int CreateTCPServerSocket(const char* ip, unsigned short port);
    int CreateTCPClientSocket(const char* ip, unsigned short port);
    bool AddServerTypeNetworkInfo(const NetworkInfo* _networkInfo);
    bool AddClientTypeNetworkInfo(const NetworkInfo* _networkInfo);
    
    
	ConnectInfo* AddClientPool(int fd);
	bool DelClientPool(int fd);
	bool GetClientFd(int fd);
    
    void sendDataToWorkerThread(int receiveType, ConnectInfo* const _connectInfo, const char* _data = NULL, int _dataSize = 0);
    
    long getCustomCurrentTime();
    void pingCheck();
private:
	int eventFd;
	int clntFd;
	struct sockaddr_in clntaddr;
	int clntaddrLen;
    int listenSocketCount;
	
    ConnectInfo serverConnectInfoList[MAX_SERVER_CONNECT_COUNT];
	ConnectInfo connectInfoList[MAX_CONNECT_SIZE];

	char recvBuffer[RECV_BUF];
    char tempRecvBuffer[RECV_BUF];
    
    int workerThreadCount;
    WorkerThread** workerThreadArray;

    struct timespec wait;
    
    long sendPingInterval;
    int disconnectPingCount;
    
    long lastPingCheckTime;
    
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