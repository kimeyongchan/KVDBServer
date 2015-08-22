//
//  ClientMasterPacket.h
//  KVDBClient
//
//  Created by kimyongchan on 2015. 8. 21..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef KVDBClient_ClientMasterPacket_h
#define KVDBClient_ClientMasterPacket_h


#include <stdint.h>

#pragma pack(push, 1)

class ClientMasterPacket
{
public:
    
    enum COMMAND_TYPE
    {
        COMMAND_TYPE_SERVER_INFO_NOTIFY = 1,
    };
    
    struct ServerInfo
    {
        char ip[16];
        uint16_t port;
    };
    
    
    struct BasicPacket
    {
        uint8_t commandType;
    };
    
    struct ServerInfoNotify : public BasicPacket
    {
        ServerInfoNotify() { commandType = COMMAND_TYPE_SERVER_INFO_NOTIFY; }
        
        ServerInfo serverInfo[2];
    };
};

#pragma pack(pop)

#endif
