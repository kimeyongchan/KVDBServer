#ifndef __MASTER_REQUEST_HANDLER__
#define __MASTER_REQUEST_HANDLER__

#define BUF_SIZE 256

class ConnectInfo;

class MasterRequestHandler
{
public:
	MasterRequestHandler();
	~MasterRequestHandler();
	bool Initialize();
	void Parse(const ConnectInfo* connectInfo, const char* data, int dataSize);
    void IsConnected(const ConnectInfo* connectInfo);
    void IsDisconnected(const ConnectInfo* connectInfo);
    
private:
	char recvBuffer[BUF_SIZE];
};

#endif //__MASTER_REQUEST_HANDLER__