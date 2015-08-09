#ifndef __CLIENT_REQUEST_HANDLER_H__
#define __CLIENT_REQUEST_HANDLER_H__

#define BUF_SIZE 256

#include <string>
#include <map>


#include "Network.h"

class RequestInfo;

class ClientRequestHandler
{
public:
    ClientRequestHandler();
    ~ClientRequestHandler();
    bool Initialize();
    void Parse(const ConnectInfo* connectInfo, const char* query, int queryLength);
    void IsConnected(const ConnectInfo* connectInfo);
    void IsDisconnected(const ConnectInfo* connectInfo);
    
private:
    bool ParsingQueryToRequestInfo(const char* query, const int queryLength, RequestInfo** ri);
    
private:
    char m_recvBuffer[BUF_SIZE];
    std::map<int, std::string> m_cutOffQueryMap;
    std::map<int, std::string>::iterator itr;
};

#endif //__CLIENT_REQUEST_HANDLER_H__