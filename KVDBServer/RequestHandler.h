#ifndef __REQUEST_HANDLER__
#define __REQUEST_HANDLER__

#define BUF_SIZE 256

#include <string>
#include <map>

#include "Network.h"

class RequestInfo;

class RequestHandler : public ReceiveHandler
{
public:
	RequestHandler();
	~RequestHandler();
	bool Initialize();
	void Parse(int fd, const char* query, int queryLength);
    
private:
    bool ParsingQueryToRequestInfo(const char* query, const int queryLength, RequestInfo** ri);

private:
	char m_recvBuffer[BUF_SIZE];
    std::map<int, std::string> m_cutOffQueryMap;
    std::map<int, std::string>::iterator itr;
};

#endif //__REQUEST_HANDLER__