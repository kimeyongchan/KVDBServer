//
//  SlaveInfo.h
//  MasterServer
//
//  Created by kimyongchan on 2015. 8. 15..
//  Copyright (c) 2015년 io. All rights reserved.
//

#ifndef MasterServer_SlaveInfo_h
#define MasterServer_SlaveInfo_h

#include <stdint.h>

struct SlaveInfo
{
    int8_t used;
    char ip[16];
    uint16_t port;
    
};

#endif
