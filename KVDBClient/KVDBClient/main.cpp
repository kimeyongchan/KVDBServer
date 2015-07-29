//
//  main.cpp
//  testClient
//
//  Created by kimyongchan on 2015. 7. 27..
//  Copyright (c) 2015년 kimyongchan. All rights reserved.
//


#include "KVDBClient.h"

const char* IP_ADDRESS = "127.0.0.1";
const char* PORT = "3307";
const char* ID = "root";
const char* PASSWORD = "1234";

int main(int argc, char *argv[])
{
    char query[1024] = "insert(\"key1\",\"value1\");";

    KVDB conn; // 연결하기 위한 객체 생성
    
    KVDB_init(&conn); // 객체 초기화
    
    if (KVDB_connect(&conn, IP_ADDRESS, PORT, ID, PASSWORD) < 0) // 디비와 연결
    {
        printf("connect error");
        return -1;
    }

    KVDB_sendQuery(&conn, query); // 쿼리 보내기
    
    KVDB_close(&conn); // 디비와 연결 끊기
    
    return 0;
}