//
//  Disk.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 8. 1..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef __KVDBServer__Disk__
#define __KVDBServer__Disk__

#include <stdInt.h>
#include <unistd.h>

#include "SuperBlock.h"

class Disk
{
public:
    bool initialize(const char* fileName, uint16_t blockSize, uint64_t diskSize);
    int openDisk();
    int readDisk(int fd, void* buf, size_t bufSize);
    int writeDisk(int fd, const void* buf, size_t bufSize);
    void close();
    
private:
    SuperBlock superBlock;
};

#endif /* defined(__KVDBServer__Disk__) */
