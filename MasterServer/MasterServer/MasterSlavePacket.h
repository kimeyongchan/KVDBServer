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

class MasterSlavePacket
{
    struct OpenClientPortReqPacket
    {
        uint16_t port;
    };
    
    struct OpenClientPortResPacket
    {
        char ip
    };
};


#endif
