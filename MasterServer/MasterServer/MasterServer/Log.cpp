#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "Log.h"

FILE* Log::_fp = NULL;
time_t Log::_lastUpdateTime = 0;

static const int FILE_NAME_LEN = 128;
static char _logPath[FILE_NAME_LEN];


Log::Log()
{
    _fp = NULL;
    _lastUpdateTime = 0;
}

Log::~Log()
{
    if (_fp)
    {
        fclose(_fp);
    }
}

bool Log::Initialize(const char* logPath)
{
    memset(_logPath, 0, sizeof(_logPath));
    memcpy(_logPath, logPath, strlen(logPath));
    
    return CreateFile();
}

bool Log::CreateFile()
{
    if (_fp)
    {
        fclose(_fp);
    }
    
    time_t curTime;
    struct tm* timeInfo;
    
    curTime = time(NULL);
    timeInfo = localtime(&curTime);
    
    _lastUpdateTime = curTime;
    
    char fullFileName[FILE_NAME_LEN] = { 0, };
    
    sprintf(fullFileName, "%s%d%02d%02d_%02d%02d%02d%s",
            _logPath, timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday, timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, ".txt");
    
    if ((_fp = fopen(fullFileName, "w")) == NULL)
    {
        ErrorLog("log file open error");
        return false;
    }
    
    return true;
}

void Log::Print(LogType type, const char* fmt, ...)
{
    struct tm* prevTimeInfo = localtime(&_lastUpdateTime);
    int createdDay = prevTimeInfo->tm_mday;
    int createdHour = prevTimeInfo->tm_hour;
 
    time_t curTime;
    struct tm* timeInfo;

    curTime = time(NULL);
    timeInfo = localtime(&curTime);
    
    if (createdDay != timeInfo->tm_mday ||
        createdHour != timeInfo->tm_hour)
    {
        if (!CreateFile())
            return;
    }
        
    static char logBuf[7000];
    static char buf[7000];
    
    memset(logBuf, 0, sizeof(logBuf));
    memset(buf, 0, sizeof(buf));
    
    va_list list;
    
    va_start(list, fmt);
    vsprintf(logBuf, fmt, list);
    va_end(list);
    
    sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d,%s", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday, timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, logBuf);
    
    puts(buf);
    
    if (_fp)
    {
        fprintf(_fp, "%s\n", buf);
        fflush(_fp);
    }
}
