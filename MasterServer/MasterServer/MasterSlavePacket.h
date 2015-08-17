//
//  MasterSlavePacket.h
//  MasterServer
//
//  Created by kimyongchan on 2015. 8. 13..
//  Copyright (c) 2015년 io. All rights reserved.
//

#ifndef MasterServer_MasterSlavePacket_h
#define MasterServer_MasterSlavePacket_h

#include "CommonPacket.h"

#pragma pack(push, 1)
class MasterSlavePacket
{
public:
    
    struct OpenClientPortReqPacket
    {
        uint16_t port;
    };
    
    struct OpenClientPortResPacket
    {
        char ip[CommonPacket::MAX_IP_ADDRESS_LEN];
    };
};
#pragma pack(pop)

#endif
