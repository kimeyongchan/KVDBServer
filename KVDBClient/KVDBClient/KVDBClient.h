//
//  KVDBClient.h
//  testClient
//
//  Created by kimyongchan on 2015. 7. 27..
//  Copyright (c) 2015년 kimyongchan. All rights reserved.
//

#ifndef __K_V_D_B_CLIENT_H__
#define __K_V_D_B_CLIENT_H__


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_LEN 1023


struct KVDB
{
    int s;
    struct sockaddr_in addr;
};

void KVDB_init(KVDB* conn)
{
    conn->s = 0;
    memset(&conn->addr, 0, sizeof(conn->addr));
}
int KVDB_connect(KVDB* conn, const char* ip, const char* port, const char* id, const char* password)
{
    if((conn->s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("can't create socket\n");
        return -1;
    }
    
    memset(&conn->addr, 0, sizeof(conn->addr));

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_addr.s_addr = inet_addr(ip);
    conn->addr.sin_port = htons(atoi(port));
    
    if(connect(conn->s, (struct sockaddr *)&conn->addr, sizeof(conn->addr)) < 0)
    {
        printf("can't connect.\n");
        return -1;
    }
    
    return 1;
}

void KVDB_close(KVDB* conn)
{
    close(conn->s);
}

void KVDB_sendQuery(KVDB* conn, const char* query)
{
    send(conn->s, query, strlen(query), NULL);
}



#endif // __K_V_D_B_CLIENT_H__