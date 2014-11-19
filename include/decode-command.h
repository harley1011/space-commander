#ifndef DECODE_COMMAND_H
#define DECODE_COMMAND_H

#include <string>
#include "icommand.h"
#include <cstdlib>

using namespace std;

struct InfoBytesDecode
{
    char decode_status;

};
class DecodeCommand : public ICommand {
public:
    DecodeCommand(char* destPath, char* srcPath, int isExecutable, int totalSize) {
        this->destPath     = destPath;
        this->srcPath      = srcPath;
        this->isExecutable = isExecutable;
        this->totalSize    = totalSize;
    }
    
    ~DecodeCommand(){ 
        if (srcPath  != NULL) { delete srcPath;  }
        if (destPath != NULL) { delete destPath; }
    }

    void* Execute();       
    void* ParseResult(const char *result);
    char* GetDestPath() { return destPath; }
    char* GetSrcPath()  { return srcPath; }
    int IsExecutable()  { return isExecutable; }
    int GetTotalSize()  { return totalSize; }
private:
    char* destPath;
    char* srcPath;
    int isExecutable;
    int totalSize;
};
#endif
