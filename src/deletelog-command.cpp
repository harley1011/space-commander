#include <stdio.h>
#include <string.h>
#include "SpaceDecl.h"
#include "deletelog-command.h"


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogCommand
* 
*-----------------------------------------------------------------------------*/
DeleteLogCommand::DeleteLogCommand(const char* filename) {
    int filename_len = strlen(filename) + 1;
    
    this->filename = (char*)malloc(sizeof(char) * filename_len);

    if (this->filename != NULL) {
        snprintf(this->filename, filename_len, "%s", filename);
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ~DeleteLogCommand
* 
*-----------------------------------------------------------------------------*/
DeleteLogCommand::~DeleteLogCommand() {
    if (filename != NULL){
        free(filename);
        filename = NULL;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : Execute
* 
* PURPOSE : Deletes 'filename', check in /home/logs and /home/tgz
*           
*-----------------------------------------------------------------------------*/
void* DeleteLogCommand::Execute() {
    char buffer[100] = {'\0'};
    const char* good_str = "1 DeleteLogCommand : removed ";
    const char* bad_str = "0 DeleteLogCommand : failed "; 

    int size =  strlen(this->filename) + 1;

    snprintf(buffer, 100, "%s/%s", CS1_LOGS, this->filename);

    if (remove(buffer) == 0){
        size += strlen(good_str) + 1;
        sprintf(buffer, "%s", good_str);
    }else{
        size += strlen(bad_str) + 1;
        sprintf(buffer, bad_str);
    }
    
    strncat(buffer, this->filename, 100);
    char* result = (char*)malloc(sizeof(char) * size);
    snprintf(result, size, "%s", buffer); 
    return result;
}
