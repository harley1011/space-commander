#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "common/commands.h"
#include "shakespeare.h"
#include "SpaceDecl.h"
#include "common/deletelog-command.h"
#include "common/subsystems.h"
#define FILENAME_TMP "filename.tmp" 

extern const char* s_cs1_subsystems[];

/* TODO  issue if you have two instances of space-commander running! (which should not happen)

                                    *        add timestamp...?
                                    */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogCommand
*
* ARGUMENTS : filename  : input - null terminated string
* 
*-----------------------------------------------------------------------------*/
DeleteLogCommand::DeleteLogCommand(const char* filename) 
{
    memset(this->filename, '\0', CS1_PATH_MAX);

    if (filename) {
        snprintf(this->filename, strlen(filename) + 1, "%s", filename); // Make a copy!
        this->FindType();
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : DeleteLogCommand
*
* ARGUMENTS : inode - inode of a file to delete
* 
*-----------------------------------------------------------------------------*/
DeleteLogCommand::DeleteLogCommand(ino_t inode)
{
    this->SaveFilename(inode);
    this->ExtractFilenameFromFile();
    this->FindType();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ~DeleteLogCommand
* 
*-----------------------------------------------------------------------------*/
DeleteLogCommand::~DeleteLogCommand() 
{
    //    
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
void* DeleteLogCommand::Execute(size_t* pSize) 
{
    char buffer[CS1_PATH_MAX] = {'\0'};
    const char* folder = 0;

    *pSize = strlen(this->filename) + CMD_HEAD_SIZE + 1; // 1 for NULL terminator
    
    switch(this->type){
        case LOG : folder = CS1_LOGS;
            break;
        case TGZ : folder = CS1_TGZ;
            break;
    }

    snprintf(buffer, CS1_PATH_MAX, "%s/%s", folder, this->filename);

    #ifdef CS1_DEBUG
        fprintf(stderr, "[DEBUG] %s():%d - %s/%s\n", __func__, __LINE__, folder, this->filename);
    #endif

    char* result = (char*)malloc(sizeof(char) * *pSize);
    if (remove(buffer) == 0) {
        snprintf(result, *pSize, "%c%c%s", DELETELOG_CMD, CS1_SUCCESS, this->filename);
    } else {   
        snprintf(result, *pSize, "%c%c%s", DELETELOG_CMD, CS1_FAILURE, this->filename);
    }

    return (void*)result;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : FindType
*
* PURPOSE : Determines if the file is under the CS1_LOGS or CS1_TGZ directory
*           by checking its extension.
* 
*-----------------------------------------------------------------------------*/
char DeleteLogCommand::FindType()
{
    this->type = TGZ;

    if (strstr(this->filename, "log")) {
        this->type = LOG;
    }

    return this->type;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : SaveFilename 
*
* PURPOSE : Saves the filename corresponding to inode in file FILENAME_TMP
* 
*-----------------------------------------------------------------------------*/
void DeleteLogCommand::SaveFilename(ino_t inode)
{
    const char* command_prefix = "find "CS1_TGZ"/ -inum ";
    const char* command_suffix = " > "CS1_TMP"/"FILENAME_TMP;
    char inode_str[15] = {'\0'};
    sprintf(inode_str, "%d", (unsigned int)inode);
    const size_t COMMAND_BUF_SIZE = strlen(command_prefix) + strlen(command_suffix) + strlen(inode_str) + 1;

    char* command_buf = (char*)malloc(sizeof(char) * COMMAND_BUF_SIZE);

    if (command_buf) {
        sprintf(command_buf, "%s%d%s", command_prefix, (unsigned int)inode, command_suffix);

        #ifdef CS1_DEBUG
            fprintf(stderr, "[DEBUG] %s:%d executing \'%s\'\n", __func__, __LINE__, command_buf);
        #endif

        system(command_buf);

        free(command_buf);
        command_buf = 0;
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ExtractFilenameFromFile 
*
* PURPOSE : Extract the filename from FILENAME_TMP and removes the file.
*
* RETURN : a pointer to static memory
* 
*-----------------------------------------------------------------------------*/
char* DeleteLogCommand::ExtractFilenameFromFile()
{
    const char* tmp = CS1_TMP"/"FILENAME_TMP;
    FILE* file = fopen(tmp, "r");
    char filestr[CS1_NAME_MAX] = {'\0'};

    if (file) {
        fscanf(file, "%s", filestr);
        fclose(file);
        file = 0;
    }

    if (strrchr(filestr, '/')) {                                 // We keep what is after the last '/'
        strcpy(filestr, strrchr(filestr,'/') + 1);                // +1 to get rid of the '/'
    }

    remove(tmp);

    strncpy(this->filename, filestr, CS1_PATH_MAX);

    return this->filename;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ParseResult 
*
* PURPOSE : Parses the result buffer returned by the execute function
*
* ARGUMENTS : result    : pointer to the result buffer
*
* RETURN : A InfoBytes containing delete_status, and filename
* 
*-----------------------------------------------------------------------------*/
void* DeleteLogCommand::ParseResult(const char *result)
{
    static struct InfoBytesDeleteLog info_bytes;

    if (!result || result[CMD_ID] != DELETELOG_CMD) {
        Shakespeare::log(Shakespeare::NOTICE, cs1_systems[CS1_COMMANDER],
                                        "DeleteLog failure: Can't parse result");
        return (void*)0;
    }

    info_bytes.delete_status = result[CMD_STS];
    info_bytes.filename = result + CMD_HEAD_SIZE;
    
    if(info_bytes.delete_status == CS1_SUCCESS)
    {
        snprintf(this->log_buffer, CS1_MAX_LOG_ENTRY, 
                                "DeleteLog success: File %s deleted",
                                info_bytes.filename);
    }
    else
    {
        snprintf(this->log_buffer, CS1_MAX_LOG_ENTRY, 
                                "DeleteLog failure: File %s not deleted",
                                info_bytes.filename);
    }

    Shakespeare::log(Shakespeare::NOTICE, cs1_systems[CS1_COMMANDER], this->log_buffer);

    return (void*)&info_bytes;
}
