#ifndef __REQUEST_INFO_H__
#define __REQUEST_INFO_H__

#include <string>

enum REQUEST_TYPE
{
	INSERT_REQUEST = 1,
    INSERT_DIRECTORY_REQUEST,
	FIND_REQUEST,
	DELETE_REQUEST,
};

class RequestInfo
{
public:
	int type;
};

class InsertRequestInfo : public RequestInfo
{
public:
    InsertRequestInfo() { type = INSERT_REQUEST; }
    std::string key;
    std::string value;
};

class InsertDirectoryRequestInfo : public RequestInfo
{
public:
    InsertDirectoryRequestInfo() { type = INSERT_DIRECTORY_REQUEST; }
    
    std::string key;
};

class FindRequestInfo : public RequestInfo
{
public:
    FindRequestInfo() { type = FIND_REQUEST; }
    
	std::string key;
};

class DeleteRequestInfo : public RequestInfo
{
public:
    DeleteRequestInfo() { type = DELETE_REQUEST; }
    
	std::string key;
};

#endif //_REQUEST_INFO_H__