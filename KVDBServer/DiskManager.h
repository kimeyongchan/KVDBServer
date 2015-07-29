#ifndef __K_V_D_B_SERVER_H__
#define __K_V_D_B_SERVER_H__

class SuperBlock;

class DiskManager
{

public:
    DiskManager();
    ~DiskManager();
    
 //   bool initialize();
    
    
private:
    int fd;
    SuperBlock* superBlock;
    
};

#endif // __K_V_D_B_SERVER_H__
