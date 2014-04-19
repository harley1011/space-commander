#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "getlog-command.h"


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetLogCommand
*
*-----------------------------------------------------------------------------*/
GetLogCommand::GetLogCommand(){
    this->opt_byte = 0x0;
    this->size = CS1_MAX_FRAME_SIZE;        // Max number of bytes to retreive.  size / tgz-part-size = number of frames
    this->date = Date();                    // Default to oldest possible log file.
    this->subsystem = 0x0;
}

GetLogCommand::GetLogCommand(char opt_byte, char subsystem, size_t size, time_t time){
    this->opt_byte = opt_byte;
    this->subsystem = subsystem;
    this->size = size;
    this->date = Date(time);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ~GetLogCommand
* 
*-----------------------------------------------------------------------------*/
GetLogCommand::~GetLogCommand(){

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : Execute 
* 
* PURPOSE : executes the GetLogCommand
*
*-----------------------------------------------------------------------------*/
void* GetLogCommand::Execute(){
    char* result = 0;

    // 1. No Options are specified, retreive the oldest package.
    if (OPT_ISNOOPT(this->opt_byte)){
        fprintf(stdout, "Execute GetLogCommand with OPT_NOOPT : Finding oldest tgz...\n");
        
    }
    


    
    return (void*)result;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : getFileLastModifTimeT 
* 
* PURPOSE : return the time_t corresponding to the last modification date of 
*           a specified file.
*
*-----------------------------------------------------------------------------*/
time_t GetLogCommand::getFileLastModifTimeT(const char *path) {
    struct stat attr;
    stat(path, &attr);
    return attr.st_mtime;
}
