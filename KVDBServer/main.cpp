#include "KVDBServer.h"
#include "Log.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//int main(int argc, const char * argv[]) {
//    
//    if(KVDBServer::getInstance()->Initialize(4) == false)
//    {
//        ErrorLog("init error");
//        return -1;
//    }
//
//    KVDBServer::getInstance()->Run();
//    
//    
//    
//    return 0;
//}

#include "radix_tree.h"
#include "NamedCache.h"
#include "bufferCache.h"

int main(int argc, const char * argv[]) {
    
    RadixTree root;
    bool b = root.insertData("aaaaaaaa", 0x1111111);
    b = root.insertData("aaaaaaaa", 0x1111111);
    if(b == false)
        cout<<"fail insert" << endl;
    NamedData* d = root.findData("aaaaaaaa");
    
    cout<<"find key in main: " << d->getKey()<< endl;
    
    root.deleteData("aaaaaaaa");
    d = root.findData("aaaaaaaa");
    if(d == NULL)
        cout<<" cannot find data " << endl;
    
    b = root.insertData("aaaaaaaa", 0x1111111);
    if(b == false)
        cout<<"fail insert" << endl;
    else
        d = root.findData("aaaaaaaa");
    
    
    
    
    
    return 0;
}