//
//  main.cpp
//  testClient
//
//  Created by kimyongchan on 2015. 7. 27..
//  Copyright (c) 2015년 kimyongchan. All rights reserved.
//


#include "KVDBClient.h"

#include <string>

const char* IP_ADDRESS = "127.0.0.1";
const char* PORT = "3307";
const char* ID = "root";
const char* PASSWORD = "1234";


void printResult(const KVDB* conn)
{
    int resultType = conn->result->resultType;
    
    if(resultType < 0)
    {
        printf("not arrive result");
        return ;
    }
    
    switch (resultType) {
        case ClientServerPacket::RESULT_TYPE_INSERT_FILE:
        {
            ClientServerPacket::InsertFileRes* res = (ClientServerPacket::InsertFileRes*)conn->result;
            if(res->resultCode == ClientServerPacket::INSERT_FILE_RESULT_CODE_SUCCESS)
            {
                printf("success insert file");
            }
            else
            {
                printf("fail insert file");
            }
            
            break;
        }
        case ClientServerPacket::RESULT_TYPE_INSERT_DIRECTORY:
        {
            ClientServerPacket::InsertDirectoryRes* res = (ClientServerPacket::InsertDirectoryRes*)conn->result;
            if(res->resultCode == ClientServerPacket::INSERT_DIRECTORY_RESULT_CODE_SUCCESS)
            {
                printf("success insert directory");
            }
            else
            {
                printf("fail insert directory");
            }
            
            break;
        }
        case ClientServerPacket::RESULT_TYPE_FIND_FILE:
        {
            ClientServerPacket::FindFileRes* res = (ClientServerPacket::FindFileRes*)conn->result;
            if(res->resultCode == ClientServerPacket::FIND_FILE_RESULT_CODE_SUCCESS)
            {
                std::string str(res->value, res->valueLen);
                printf("success find file value - %s", str.c_str());
            }
            else
            {
                printf("fail find file");
            }
            
            break;
        }
        case ClientServerPacket::RESULT_TYPE_FIND_DIRECTORY:
        {
            ClientServerPacket::FindDirectoryRes* res = (ClientServerPacket::FindDirectoryRes*)conn->result;
            if(res->resultCode == ClientServerPacket::FIND_DIRECTORY_RESULT_CODE_SUCCESS)
            {
                printf("success find directory ");
            }
            else
            {
                printf("fail find directory");
            }
            
            break;
        }
        case ClientServerPacket::RESULT_TYPE_DELETE_FILE:
        {
            ClientServerPacket::DeleteFileRes* res = (ClientServerPacket::DeleteFileRes*)conn->result;
            if(res->resultCode == ClientServerPacket::DELETE_FILE_RESULT_CODE_SUCCESS)
            {
                printf("success delete file");
            }
            else
            {
                printf("fail delete file");
            }
            
            break;
        }
        case ClientServerPacket::RESULT_TYPE_DELETE_DIRECTORY:
        {
            ClientServerPacket::DeleteDirectoryRes* res = (ClientServerPacket::DeleteDirectoryRes*)conn->result;
            if(res->resultCode == ClientServerPacket::DELETE_DIRECTORY_RESULT_CODE_SUCCESS)
            {
                printf("success delete directory");
            }
            else
            {
                printf("fail delete directory");
            }

            break;
        }
        default:
            printf("invalid type - %d", resultType);
            break;
    }
    printf("\n");
}



int main(int argc, char *argv[])
{
//    char query1[1024] = "insert(\"a\",\"value1\");";
    char query1[1024] = "insert(\"b\");";

    char query5[1024] = "find(\"b\");";
    
//    char query3[1024] = "insert(\"a/b\",\"value1\");";
    //    char query2[1024] = "delete(\"a/b\");";
    char query4[1024] = "delete(\"b\");";
    

    KVDB conn; // 연결하기 위한 객체 생성
    
    KVDB_init(&conn); // 객체 초기화
    
    if (KVDB_connect(&conn, IP_ADDRESS, PORT, ID, PASSWORD) < 0) // 디비와 연결
    {
        printf("connect error");
        return -1;
    }
    
    KVDB_sendQuery(&conn, query1, (int)strlen(query1)); // 쿼리 보내기
    printResult(&conn);
    
//    KVDB_sendQuery(&conn, query3, (int)strlen(query3)); // 쿼리 보내기
//    printResult(&conn);

    
//    KVDB_sendQuery(&conn, query2, (int)strlen(query2)); // 쿼리 보내기
//    printResult(&conn);

    KVDB_sendQuery(&conn, query5, (int)strlen(query5)); // 쿼리 보내기
            printResult(&conn);

    
    KVDB_sendQuery(&conn, query4, (int)strlen(query4)); // 쿼리 보내기
    printResult(&conn);
    
    KVDB_close(&conn); // 디비와 연결 끊기
    
    return 0;
}