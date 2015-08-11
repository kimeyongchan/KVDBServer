//
//  XmlData.h
//  ServerModule
//
//  Created by kimyongchan on 2015. 8. 4..
//  Copyright (c) 2015년 kimyongchan. All rights reserved.
//

#ifndef __ServerModule__XmlData__
#define __ServerModule__XmlData__

enum SERVER_MODULE
{
    SERVER_MODULE_SLAVE = 0,
    SERVER_MODULE_CLIENT,
};

struct ServerInfo
{
    int serverType;
    int serverModule;
    char ip[16];
    unsigned short port;
};

class XmlData
{
public:
    XmlData();
    bool initialize();
    
    
private:
    bool loadServerInfoList();
    
public:
    int serverInfoCount;
    ServerInfo* serverInfoList;
};

#endif /* defined(__ServerModule__XmlData__) */
