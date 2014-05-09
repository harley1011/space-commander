/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Joseph 
*           (inspired by the previous version : getlog-command-cpp-obsolete201404)
*
* TITLE : getlog-command.cpp
*
*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
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
    this->number_of_processed_files = 0;
}

GetLogCommand::GetLogCommand(char opt_byte, char subsystem, size_t size, time_t time)
{
    this->opt_byte = opt_byte;
    this->subsystem = subsystem;
    this->size = size;
    this->date = Date(time);
    this->number_of_processed_files = 0;
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
    /* TODO IN PROGRESS : add [INFO] and [END] bytes before and after EACH file read into result buffer.
    * result : [INFO] + [TGZ DATA] + [END]
    * INFO : use inode instead of filename to limit the size
    */
    
    char *result = 0;

    char filepath[CS1_PATH_MAX] = {'\0'};
    char buffer[CS1_TGZ_MAX] = {'\0'};
    char *file_to_retreive = 0;
    size_t bytes = 0;
    size_t number_of_files_to_retreive = 1;         // defaults to 1

    if (OPT_ISSIZE(this->opt_byte)) { 
        number_of_files_to_retreive = this->size / CS1_TGZ_MAX;
    }

    while (number_of_files_to_retreive) { 
        file_to_retreive = this->GetNextFile();
#ifdef DEBUG
        fprintf(stderr, "[DEBUG] %s() - file_to_retreive : %s\n", __func__, file_to_retreive);
#endif
        GetLogCommand::BuildPath(filepath, CS1_TGZ, file_to_retreive);
        bytes += GetLogCommand::ReadFile(buffer + bytes, filepath); 

        number_of_files_to_retreive--; 
        this->MarkAsProcessed(filepath); /* 'filepath' is considered as processed for this instance of the GetLogCommand
                                         *  if you send a new GetLogCommand with the same parameters, 'filepath' will not
                                         *  be considered as processed. i.e. the processed_files array belongs to this 
                                         *  instance only
                                         */
    }

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
*           N.B. max size is CS1_TGZ_MAX
*
*-----------------------------------------------------------------------------*/
size_t GetLogCommand::ReadFile(char *buffer, const char *filename)
{
    return GetLogCommand::ReadFile_FromStartToEnd(buffer, filename, START, CS1_TGZ_MAX);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : ReadFile_FromStartToEnd
* 
* PURPOSE : Reads 'filename' starting from byte 'start' to byte 'start + size'
*           into 'buffer', use this function if the hability to retreive
*           partial files is needed. 
*
* RETURN : The number of bytes read into buffer.
*
*-----------------------------------------------------------------------------*/
size_t GetLogCommand::ReadFile_FromStartToEnd(char *buffer, const char *filename, size_t start, size_t size)
{
    FILE *pFile = fopen(filename, "rb");
    size_t bytes = 0;

    if (!buffer) {
        fprintf(stderr, "[ERROR] %s:%d - The output buffer is NULL.\n", __func__, __LINE__);
        return bytes;
    }

    if (!pFile) {
        fprintf(stderr, "[ERROR] %s:%d - Cannot fopen the file.\n", __func__, __LINE__);
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
        // Assuming there is only one file with this SUB and this DATE <- NOT TRUE!

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
        if (dir_entry->d_type == DT_REG // is a regular file
                && !this->isFileProcessed(dir_entry->d_ino) // is NOT processed
                    && GetLogCommand::prefixMatches(dir_entry->d_name, pattern)) 
        { 
            GetLogCommand::BuildPath(buffer, directory_path, dir_entry->d_name);

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
* NAME : MarkAsProcessed 
* 
* PURPOSE : Add the inode of a file to the processed_files array
*
*-----------------------------------------------------------------------------*/
void GetLogCommand::MarkAsProcessed(const char *filepath) 
{
    struct stat attr;
    stat(filepath, &attr);

#ifdef DEBUG
    fprintf(stderr, "[DEBUG] %s() - inode %d\n", __func__, attr.st_ino);
#endif
    this->processed_files[this->number_of_processed_files] = attr.st_ino;
    this->number_of_processed_files++;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : isFileProcessed 
* 
* PURPOSE : Checks if a file has been processed
*
*-----------------------------------------------------------------------------*/
bool GetLogCommand::isFileProcessed(unsigned long inode) 
{
    for (size_t i = 0; i < this->number_of_processed_files; i++) {
        if (inode == this->processed_files[i]) {
#ifdef DEBUG
            fprintf(stderr, "[DEBUG] %s() - inode %d\n", __func__, inode);
#endif
            return true;
        }
    }
    
    return false;
}

/* Jacket over the preceding function, to accept the file path */
bool GetLogCommand::isFileProcessed(const char *filepath)
{
    struct stat attr;
    stat(filepath, &attr);

    return this->isFileProcessed(attr.st_ino);
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
* PURPOSE : Builds a GetLogCommand and saves it into 'command_buf'
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

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : BuildPath 
* 
* PURPOSE : Builds a path 'dir/file' and saves it into 'path_buf', the caller
*           has to make sure path_buf is large enough.
*
*-----------------------------------------------------------------------------*/
char* GetLogCommand::BuildPath(char *path_buf, const char *dir, const char *file)
{
    assert(strlen(dir) + strlen(file) + 1 < CS1_PATH_MAX);

    strcpy(path_buf, dir);
    strcat(path_buf, "/");
    strcat(path_buf, file);

    return path_buf;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetInfoBytes 
* 
* PURPOSE : Builds and saves the info bytes for the file 'filepath' at the 
*           location pointed to by 'buffer' 
*
* DESCRIPTION : [ino_t] - inode off the file, to uniquely identify
*                          it and be able to call the DeleteLogCommand with it.
*               [checksum] - TODO
*
*-----------------------------------------------------------------------------*/
char* GetLogCommand::GetInfoBytes(char *buffer, const char *filepath) 
{

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* NAME : GetInoT
* 
* PURPOSE : Returns the ino_t associated with the file at 'filepath'
*
*-----------------------------------------------------------------------------*/
ino_t GetLogCommand::GetInoT(const char *filepath)
{
    struct stat attr;
    stat(filepath, &attr);

    return attr.st_ino;
}
