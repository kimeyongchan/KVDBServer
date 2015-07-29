#include "Network.h"

#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Log.h"


Network::Network()
{
}


Network::~Network()
{
}

//////////////////////////////////////////
#if OS_PLATFORM == PLATFORM_MAC
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

	m_eventFd = kqueue();
	if (m_eventFd < 0) {
		ErrorLog("kqueue create error.");
        return false;
	}

	m_event = (struct kevent*)malloc(sizeof(struct kevent)*EVENT_BUFFER_SIZE);
    
    EV_SET(&m_connectEvent, m_sockFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    
	m_clntaddrLen = sizeof(m_clntaddr);

	memset(m_connectInfo, 0, sizeof(ConnectInfo) * MAX_CONNECT_SIZE);
	memset(m_recvBuffer, 0, RECV_BUF);

	m_recvHandler = recvHandler;
    
    if (kevent(m_eventFd, &m_connectEvent, 1, NULL, 0, NULL) == -1)
    {
        ErrorLog("kevent init error");
        return false;
    }
    
	return true;
}

void Network::ProcessEvent()
{
	int eventCnt;
	while(true)
	{
        eventCnt = kevent(m_eventFd, NULL, 0, m_event, EVENT_BUFFER_SIZE, NULL);
		if (eventCnt <= 0)
		{
			ErrorLog("kevent wait fail.");
			continue;
		}

		for (int i = 0; i < eventCnt; i++)
        {
			if (m_event[i].filter == EVFILT_READ)
            {
                if(m_event[i].ident == m_sockFd) // Server sock
                {
                    m_clntFd = accept(m_sockFd, (struct sockaddr *)&m_clntaddr, (socklen_t*)&m_clntaddrLen);
                    if (m_clntFd < 0)
                    {
                        ErrorLog("accept fail.");
                        continue;
                    }
                    else
                    {
                        if (AddClientPool(m_clntFd) == false)
                        {
                            ErrorLog("AddClientPool Error");
                            close(m_clntFd);
                            continue;
                        }
                        
                        EV_SET(&m_connectEvent, m_clntFd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, NULL);
                        
                        if (kevent(m_eventFd, &m_connectEvent, 1, NULL, 0, NULL) == -1)
                        {
                            ErrorLog("kevent init error");
                            continue ;
                        }
                    }
                }
                else { // Client sock
                    
                    m_clntFd = (int)m_event[i].ident;
                    
                    if (GetClientFd(m_clntFd) == false)
                    {
                        ErrorLog("not exist client fd : %d", m_clntFd);
                        continue;
                    }

                    int readCnt = 0;
                
                    readCnt = (int)recv(m_clntFd, m_recvBuffer, RECV_BUF - 1, 0);
                    
                    if (readCnt > 0)
                    {
                        m_recvHandler->Parse(m_clntFd, m_recvBuffer, readCnt);
                        memset(m_recvBuffer, 0, readCnt);
                        
                    }
                    else if (readCnt == 0)
                    {
                        EV_SET(&m_connectEvent, m_clntFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                        
                        if(DelClientPool(m_clntFd) == false)
                        {
                            ErrorLog("DelClient error %d", m_clntaddr);
                        }
                        
                        if (kevent(m_eventFd, &m_connectEvent, 1, NULL, 0, NULL) == -1)
                        {
                            ErrorLog("kevent init error");
                        }
                    }
                    else
                    {
                        ErrorLog("ERR : read fail (%d)\n", readCnt);
                        
                        EV_SET(&m_connectEvent, m_clntFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                        
                        if(DelClientPool(m_clntFd) == false)
                        {
                            ErrorLog("DelClient error %d", m_clntaddr);
                        }
                        if (kevent(m_eventFd, &m_connectEvent, 1, NULL, 0, NULL) == -1)
                        {
                            ErrorLog("kevent init error");
                        }
                    }
                }
			}
		}
	}
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


int Network::CreateTCPServerSocket(unsigned short port)
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

bool Network::AddClientPool(int fd)
{
	int	i;
	for (i = 0; i < MAX_CONNECT_SIZE; i++) {
		if (m_connectInfo[i].used == UNUSED) {
			m_connectInfo[i].fd = fd;
			m_connectInfo[i].used = USED;
			return true;
		}
	}

	return false;
}

bool Network::DelClientPool(int fd)
{
	int	i;
	for (i = 0; i < MAX_CONNECT_SIZE; i++) {
		if (m_connectInfo[i].fd == fd) {
			m_connectInfo[i].fd = 0;
			m_connectInfo[i].used = UNUSED;
			return true;
		}
	}

	return false;
}

bool Network::GetClientFd(int fd)
{
	int	i;
	for (i = 0; i < MAX_CONNECT_SIZE; i++) {
		if (m_connectInfo[i].fd == fd) {
			return true;
		}
	}

	return false;
}