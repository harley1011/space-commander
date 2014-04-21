#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>
#include <string.h>
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
* NAME : GetFileLastModifTimeT 
* 
* PURPOSE : return the time_t corresponding to the last modification date of 
*           a specified file.
*
*-----------------------------------------------------------------------------*/
time_t GetLogCommand::GetFileLastModifTimeT(const char *path) {
    struct stat attr;
    stat(path, &attr);
    return attr.st_mtime;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : FindOldestFile 
* 
* PURPOSE : Returns the name of the oldest file present in the specified 
*           directory
*           N.B. returns a newly allocated char*  FREE IT
*
*-----------------------------------------------------------------------------*/
char* GetLogCommand::FindOldestFile(const char* directory_path){
    struct dirent* dir_entry = 0;
    DIR* dir = 0;
    time_t oldest_timeT = INT_MAX - 1;
    time_t current_timeT = 0;
    char* oldest_filename = (char*)malloc(sizeof(char) * NAME_MAX);
    memset(oldest_filename, '\0', NAME_MAX * sizeof(char));
    char buffer[CS1_MAX_PATH_LENGTH] = {'\0'};
    
    dir = opendir(directory_path);

    while ((dir_entry = readdir(dir))) { 
        if (dir_entry->d_type == DT_REG) { 
            strncpy(buffer, directory_path, strlen(directory_path) + 1);
            strncat(buffer, "/", strlen("/"));
            strncat(buffer, dir_entry->d_name, strlen(dir_entry->d_name)); 


            current_timeT = GetLogCommand::GetFileLastModifTimeT(buffer); 

            if (current_timeT < oldest_timeT) {
                fprintf(stderr, "path : %s\n", buffer);
                oldest_timeT = current_timeT;
                strncpy(oldest_filename, dir_entry->d_name, strlen(dir_entry->d_name) + 1);
            }
        }
    }
    
    // Have to close the DIR!!! closedir()   TODO

    return oldest_filename; 
}
