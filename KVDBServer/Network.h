#ifndef __NETWORK_H__
#define __NETWORK_H__


#include <stdInt.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include "BasicDefines.h"


#define EVENT_BUFFER_SIZE 50
#define MAX_CONNECT_SIZE 100
#define RECV_BUF 5000

#define USED 1
#define UNUSED 0

struct ConnectInfo
{
	int fd;
	int8_t used;
};


class ReceiveHandler
{
public:
	virtual void Parse(int fd, const char* query, int queryLength) = 0;

};


class Network
{
public:
	Network();
	~Network();
	bool Initialize(int port, ReceiveHandler* recvHandler);
	void ProcessEvent();

private:
	int CreateTCPServerSocket(unsigned short port);
	bool AddClientPool(int fd);
	bool DelClientPool(int fd);
	bool GetClientFd(int fd);

private:
	int m_sockFd;
	int m_port;
	int m_eventFd;
	int m_clntFd;
	struct sockaddr_in m_clntaddr;
	int m_clntaddrLen;
	
	ConnectInfo m_connectInfo[MAX_CONNECT_SIZE];

	char m_recvBuffer[RECV_BUF];

	ReceiveHandler* m_recvHandler;
    
#if OS_PLATFORM == PLATFORM_LINUX
    
    struct epoll_event* m_event;
    struct epoll_event m_connectEvent;
    
#elif OS_PLATFORM == PLATFORM_MAC
    
    struct kevent* m_event;
    struct kevent m_connectEvent;
#else
    
#endif
    
};

#endif //_NETWORK_H__