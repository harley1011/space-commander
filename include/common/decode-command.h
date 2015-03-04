#ifndef DECODE_COMMAND_H
#define DECODE_COMMAND_H

#include <string>
#include "icommand.h"
#include "infobytes.h"
#include <cstdlib>

using namespace std;

class InfoBytesDecode : public InfoBytes
{
    public:
        char decode_status;
        string* ToString() {
        string* infoStatus = new string (1, decode_status);
        return infoStatus;
    };
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
    InfoBytes* ParseResult(char *result);
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
