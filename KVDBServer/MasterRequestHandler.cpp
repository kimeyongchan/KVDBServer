#include "MasterRequestHandler.h"

#include <string.h>

#include "Network.h"
#include "Log.h"
#include <unistd.h>

MasterRequestHandler::MasterRequestHandler()
{
	memset(recvBuffer, 0, BUF_SIZE);
}


MasterRequestHandler::~MasterRequestHandler()
{

}


bool MasterRequestHandler::Initialize()
{
	return true;
}


void MasterRequestHandler::Parse(const ConnectInfo* connectInfo, const char* data, int dataSize)
{
    
}

void MasterRequestHandler::IsConnected(const ConnectInfo* connectInfo)
{
    DebugLog("connect %d", connectInfo->fd);
}

void MasterRequestHandler::IsDisconnected(const ConnectInfo* connectInfo)
{
    DebugLog("disconnect %d", connectInfo->fd);
    sleep(3);
}