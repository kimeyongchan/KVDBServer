#ifndef __I_O_MANAGER_H__
#define __I_O_MANAGER_H__

#include "WorkerThread.h"


class IOManager : public WorkerThread
{
public:
    IOManager();
    ~IOManager();

    ////////////virtual method///////////////
    void connected(const ConnectInfo* connectInfo);
    void disconnected(const ConnectInfo* connectInfo);
    void receiveData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    
    
    ////////////////////////////////////////
    void receiveClientData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    void receiveSlaveData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    
};

#endif // __I_O_MANAGER_H__