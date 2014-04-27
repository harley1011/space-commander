#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SpaceDecl.h"
#include "deletelog-command.h"


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogCommand
*
* ARGUMENTS : filename  : input - null terminated string
* 
*-----------------------------------------------------------------------------*/
DeleteLogCommand::DeleteLogCommand(const char* filename) {
    int filename_len = strlen(filename) + 1;
    
    this->filename = (char*)malloc(sizeof(char) * filename_len);

    if (this->filename != NULL) {
        snprintf(this->filename, filename_len, "%s", filename); // Make a copy!
    }

    this->FindType();
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
* RETURNS : a newly allocated buffer, free it!
*           
*-----------------------------------------------------------------------------*/
void* DeleteLogCommand::Execute() {
    char buffer[CS1_PATH_MAX] = {'\0'};
    const char* good_str = "0 DeleteLogCommand : removed ";
    const char* bad_str = "1 DeleteLogCommand : failed "; 
    const char* folder = 0;

    int size =  strlen(this->filename) + 1;

    switch(this->type){
        case LOG : folder = CS1_LOGS;
            break;
        case TGZ : folder = CS1_TGZ;
            break;
    }

    snprintf(buffer, CS1_PATH_MAX, "%s/%s", folder, this->filename);

    if (remove(buffer) == 0){
        size += strlen(good_str) + 1;
        sprintf(buffer, "%s", good_str);
    }else{
        size += strlen(bad_str) + 1;
        sprintf(buffer, "%s",  bad_str);
    }
    
    strncat(buffer, this->filename, CS1_PATH_MAX);
    char* result = (char*)malloc(sizeof(char) * size);
    snprintf(result, size, "%s", buffer); 
    return result;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : FindType
*
* PURPOSE : Determines if the file is under the CS1_LOGS or CS1_TGZ directory
*           by checking its extension.
* 
*-----------------------------------------------------------------------------*/
char DeleteLogCommand::FindType(){
    this->type = LOG;

    if (strstr(this->filename, ".tgz")){
        this->type = TGZ;
    }

    return this->type;
}
