#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#include <assert.h>

#include "SpaceString.h"
#include "subsystems.h"
#include "commands.h"
#include "getlog-command.h"

extern const char* s_cs1_subsystems[];  // defined in sybsystems.cpp

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetLogCommand
*
*-----------------------------------------------------------------------------*/
GetLogCommand::GetLogCommand()
{
    this->opt_byte = 0x0;
    this->size = CS1_MAX_FRAME_SIZE;        // Max number of bytes to retreive.  size / tgz-part-size = number of frames
    this->date = Date();                    // Default to oldest possible log file.
    this->subsystem = 0x0;
}

GetLogCommand::GetLogCommand(char opt_byte, char subsystem, size_t size, time_t time)
{
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
GetLogCommand::~GetLogCommand()
{

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : Execute 
* 
* PURPOSE : Executes the GetLogCommand.
*           - if no OPT_SIZE is not specified, retreives one tgz
*           - if OPT_SIZE is specified, retreives floor(SIZE / CS1_TGZ_MAX) tgzs
*
*-----------------------------------------------------------------------------*/
void* GetLogCommand::Execute()
{
    /* TODO IN PROGRESS
    * result : [INFO] + [TGZ DATA] + [END]
    * INFO : use inode instead of filename to limit the size
    */
    
    char *result = 0;

    char filepath[CS1_PATH_MAX] = {'\0'};
    char *file_to_retreive = 0;
    char buffer[CS1_TGZ_MAX] = {0};
    size_t bytes = 0;

    file_to_retreive = this->GetNextFile();

    strcpy(filepath, CS1_TGZ);
    strcat(filepath, "/");
    strcat(filepath, file_to_retreive);

    bytes = GetLogCommand::ReadFile(buffer, filepath); 

    // allocate the result buffer
    result = (char*)malloc(sizeof(char) * bytes);

    // 1. get the GETLOG result info header

    // 2. saves the tgz data in th result buffer
    memcpy(result, buffer, bytes);

    // 3. add END byte
    
    return (void*)result;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ReadFile
* 
* PURPOSE : Reads the specefied file into the specified buffer.
*
*-----------------------------------------------------------------------------*/
size_t GetLogCommand::ReadFile(char *buffer, const char *filename)
{
    return GetLogCommand::ReadFile_FromStartToEnd(buffer, filename, START, CS1_TGZ_MAX - GETLOG_INFO_SIZE);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ReadFile_FromStartToEnd
* 
* PURPOSE : 
*
*-----------------------------------------------------------------------------*/
size_t GetLogCommand::ReadFile_FromStartToEnd(char *buffer, const char *filename, size_t start, size_t size)
{
    FILE *pFile = fopen(filename, "rb");
    size_t bytes = 0;

    if (!buffer) {
        fprintf(stderr, "[ERROR] GetLogCommand::ReadFile_FromStartToEnd - The output buffer is NULL.\n");
        return bytes;
    }

    if (!pFile) {
        fprintf(stderr, "[ERROR] GetLogCommand::ReadFile_FromStartToEnd - Cannot fopen the file.\n");
        return bytes;
    }

    fseek(pFile, start, 0);
    bytes = fread(buffer, 1, size, pFile);

    // Cleanup
    if (pFile) {
        fclose(pFile);
        pFile = 0;
    }

    return bytes;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetNextFile 
* 
* PURPOSE : Returns the name of the next file to retreive according to the 
*           opt_byte
*           
* RETURN  : char* to STATIC memory! A second call to GetNextFile will overwrite
*           the buffer.
*
*-----------------------------------------------------------------------------*/
char* GetLogCommand::GetNextFile(void) 
{
    static char filename[CS1_NAME_MAX] = {'\0'};
    char* buf = 0;

    if (OPT_ISNOOPT(this->opt_byte)) 
    { 
        // 1. No Options are specified, retreive the oldest package.
        fprintf(stderr, "[INFO] Execute GetLogCommand with OPT_NOOPT : Finding oldest tgz...\n");
        buf = GetLogCommand::FindOldestFile(CS1_TGZ, NULL);     // Pass NULL to match ANY Sub
    } 
    else if (OPT_ISSUB(this->opt_byte) && !OPT_ISDATE(this->opt_byte)) 
    {
        // 2. The Subsystem is defined, retreive the oldest package that belongs to that subsystem.
        fprintf(stderr, "[INFO] Execute GetLogCommand with OPT_SUB : Finding oldest tgz that matches SUB...\n");
        buf = GetLogCommand::FindOldestFile(CS1_TGZ, s_cs1_subsystems[(size_t)this->subsystem]);
    } 
    else if (OPT_ISSUB(this->opt_byte) && OPT_ISDATE(this->opt_byte)) 
    {

    }
    
    if (buf) { 
        assert(strlen(buf) < CS1_NAME_MAX);
        strcpy(filename, buf);

        free (buf);
        buf = 0;
    }

    return filename;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : FindOldestFile 
* 
* PURPOSE : Returns the name of the oldest file present in the specified 
*           directory and that matches 'pattern' (if not NULL)
*           N.B. returns a newly allocated char*  FREE IT
*
*-----------------------------------------------------------------------------*/
char* GetLogCommand::FindOldestFile(const char* directory_path, const char* pattern) 
{
    struct dirent* dir_entry = 0;
    DIR* dir = 0;
    time_t oldest_timeT = INT_MAX - 1;
    time_t current_timeT = 0;
    char buffer[CS1_PATH_MAX] = {'\0'};
    char* oldest_filename = (char*)malloc(sizeof(char) * CS1_NAME_MAX);
    memset(oldest_filename, '\0', CS1_NAME_MAX * sizeof(char));
    
    dir = opendir(directory_path);

    while ((dir_entry = readdir(dir))) { 
        if (dir_entry->d_type == DT_REG && GetLogCommand::prefixMatches(dir_entry->d_name, pattern)) { 
            GetLogCommand::GetPath(directory_path, dir_entry->d_name, buffer);

            current_timeT = GetLogCommand::GetFileLastModifTimeT(buffer); 

            if (current_timeT < oldest_timeT) {
                oldest_timeT = current_timeT;
                strncpy(oldest_filename, dir_entry->d_name, strlen(dir_entry->d_name) + 1);
            }
        }
    }
    
    if (dir) {
        closedir(dir);
        dir = NULL;
    }

    return oldest_filename; 
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetFileLastModifTimeT 
* 
* PURPOSE : return the time_t corresponding to the last modification date of 
*           a specified file.
*
*-----------------------------------------------------------------------------*/
time_t GetLogCommand::GetFileLastModifTimeT(const char *path) 
{
    struct stat attr;
    stat(path, &attr);
    return attr.st_mtime;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetPath 
* 
* PURPOSE : Builds the path dir/file and saves it into buffer. 
*           N.B. Make sure buffer is large enough (CS1_PATH_MAX)
*
*-----------------------------------------------------------------------------*/
char* GetLogCommand::GetPath(const char* dir, const char* file, char* buffer) 
{
    strncpy(buffer, dir, strlen(dir) + 1);
    strncat(buffer, "/", strlen("/"));
    strncat(buffer, file, strlen(file)); 

    return buffer;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : prefixMatches 
* 
* PURPOSE : Returns true if the 'filename' constains 'pattern'
*
*-----------------------------------------------------------------------------*/
bool GetLogCommand::prefixMatches(const char* filename, const char* pattern) 
{
    if (filename == NULL || pattern == NULL) {
        return true;
    }

    if (strstr(filename, pattern)) {
	    return true;
    } 

    return false;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : Build_GetLogCommand 
* 
* PURPOSE : 
*
*-----------------------------------------------------------------------------*/
char* GetLogCommand::Build_GetLogCommand(char command_buf[GETLOG_CMD_SIZE], char opt_byte, char subsystem, size_t size, time_t date) 
{
   command_buf[0] = GETLOG_CMD;
   command_buf[1] = opt_byte;
   command_buf[2] = subsystem;
   SpaceString::get4Char(command_buf + 3, size);
   SpaceString::get4Char(command_buf + 7, date);

   return command_buf;
}
