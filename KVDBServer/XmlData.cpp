//
//  XmlData.cpp
//  ServerModule
//
//  Created by kimyongchan on 2015. 8. 4..
//  Copyright (c) 2015년 kimyongchan. All rights reserved.
//

#include "XmlData.h"

#include <string.h>

#include "Network.h"

XmlData::XmlData()
{
    
}

bool XmlData::initialize()
{
    if(loadServerInfoList() == false)
        return false;
    
    return true;
}

bool XmlData::loadServerInfoList()
{
    serverInfoCount = 2;
    serverInfoList = new ServerInfo[serverInfoCount];
    memset(serverInfoList, 0, sizeof(ServerInfo) * serverInfoCount);
    
    serverInfoList[0].serverType = SERVER_TYPE_CLIENT;
    serverInfoList[0].serverModule = SERVER_MODULE_MASTER;
    memcpy(serverInfoList[0].ip, "127.0.0.1", 9);
    serverInfoList[0].port = 10000;
    /*
    serverInfoList[1].serverType = SERVER_TYPE_SERVER;
    serverInfoList[1].serverModule = SERVER_MODULE_CLIENT;
    memcpy(serverInfoList[1].ip, "127.0.0.1", 9);
    serverInfoList[1].port = 3308;
*/
    return true;
}