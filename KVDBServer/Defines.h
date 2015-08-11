//
//  Defines.h
//  KVDBServer
//
//  Created by kimyongchan on 2015. 7. 30..
//  Copyright (c) 2015년 kvdb. All rights reserved.
//

#ifndef __KVDBServer__Defines__
#define __KVDBServer__Defines__

#include <stdInt.h>
#include <limits.h>

static const uint16_t BLOCK_SIZE = 8192; // 8K
static const uint64_t DISK_SIZE = 1024 * 1024 * 1024; // 1G

#define MAX_KEY_LEN UINT8_MAX;
#define MAX_VALUE_LEN UINT32_MAX;

typedef uint8_t KeyLen_t;
typedef uint32_t ValueLen_t;

static const char* KVDB_NAME = "kvdb.bin";
static const char* KVDB_LOG_NAME = "kvdb_log.bin";


#endif /* defined(__KVDBServer__Defines__) */
