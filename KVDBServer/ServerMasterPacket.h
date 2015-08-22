//
//  ClientServerPacket.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 19..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef KVDBServer_ServerMasterPacket_h
#define KVDBServer_ServerMasterPacket_h

#include <stdint.h>

#pragma pack(push, 1)

class ServerMasterPacket
{
public:
    
    enum COMMAND_TYPE
    {
        COMMAND_TYPE_OPEN_CLIENT_SOCK_REQ = 1,
        COMMAND_TYPE_OPEN_CLIENT_SOCK_RES,
        
    };
    
    
    struct BasicPacket
    {
        uint8_t commandType;
    };
    
    struct OpenClientSockReq : public BasicPacket
    {
        OpenClientSockReq() { commandType = COMMAND_TYPE_OPEN_CLIENT_SOCK_REQ; }
        
        uint16_t port;
    };
    
    struct OpenClientSockRes : public BasicPacket
    {
        OpenClientSockRes() { commandType = COMMAND_TYPE_OPEN_CLIENT_SOCK_RES; }
        
        uint16_t port;
        char ip[16];
    };
    
};

#pragma pack(pop)


#endif
