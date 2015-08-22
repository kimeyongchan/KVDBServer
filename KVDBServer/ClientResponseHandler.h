//
//  ClientResponseHandler.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 19..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef __KVDBServer__ClientResponseHandler__
#define __KVDBServer__ClientResponseHandler__

#include "ClientServerPacket.h"

#define MAX_SEND_BUF_LEN 5000

class ConnectInfo;

class ClientResponseHandler
{
public:
    void InsertFileRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::InsertFileRes* insertFileRes);
    void InsertDirectoryRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::InsertDirectoryRes* insertDirectoryRes);
    void FindFileRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::FindFileRes* findFileRes);
    void FindRirectoryRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::FindDirectoryRes* findDirectoryRes);
    void DeleteFileRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::DeleteFileRes* deleteFileRes);
    void DeleteDirectoryRes(int tfd, const ConnectInfo* connectInfo, ClientServerPacket::DeleteDirectoryRes* deleteDirectoryRes);
    
private:
    char sendBuffer[MAX_SEND_BUF_LEN];
};

#endif /* defined(__KVDBServer__ClientResponseHandler__) */
