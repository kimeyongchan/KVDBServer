#ifndef __I_O_MANAGER_H__
#define __I_O_MANAGER_H__

#include "WorkerThread.h"


class IOManager : public WorkerThread
{
public:
    IOManager();
    ~IOManager();

    ////////////virtual method///////////////
    void enterConnect(const ConnectInfo* connectInfo);
    void leaveConnect(const ConnectInfo* connectInfo);
    void receiveData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    
    
    ////////////////////////////////////////
    void receiveClientData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    void receiveSlaveData(const ConnectInfo* connectInfo, const char* data, int dataSize);
    
};

#endif // __I_O_MANAGER_H__