/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Joseph 
*           (inspired by the previous version : getlog-command-cpp-obsolete201404)
*
* TITLE : getlog-command.h 
*
* DESCRIPTION : Retreive a log archive.
*
*              - Format of the GetLogCommand :  
*                   [0]   :   Command number byte.
*                   [1]   :   Option byte - specifies if options are present or not
*                   [2]   :   Subsystem  
*                 [3-6]   :   Size
*                [7-10]   :   Date as a time_t
*
*       Execute() :
*               if there is no option specified     
*                       - Returns the oldest file present in CS1_TGZ
*               if only Subsystem is specified      
*                       - Returns the oldest file belonging to that subsystem
*               if only Size is specified           
*                       -   Returns the floor(Size / CS1_TGZ_MAX) oldest files in CS1_TGZ
*               if only Date is specified
*                       -   Returns the first file that matches this Date  in CS1_TGZ
*
*----------------------------------------------------------------------------*/
#ifndef GETLOG_COMMAND_H
#define GETLOG_COMMAND_H

#include <cstdlib>
#include <string>

#include "SpaceDecl.h"
#include "Date.h"
#include "subsystems.h"
#include "commands.h"
#include "icommand.h"
#include "iinfobytes.h"

using namespace std;

#define GETLOG_CMD_SIZE 11
#define MAX_NUMBER_OF_FILES_PER_CMD 10

#define OPT_NOOPT 0x00
#define OPT_SUB 0x01
#define OPT_SIZE 0x02
#define OPT_DATE 0x04

#define OPT_ISNOOPT(x)  ((x) == OPT_NOOPT || (x) == OPT_SIZE) // ignore OPT_SIZE
#define OPT_ISSUB(x)    (((x) & OPT_SUB) == OPT_SUB)
#define OPT_ISSIZE(x)   (((x) & OPT_SIZE) == OPT_SIZE)
#define OPT_ISDATE(x)   (((x) & OPT_DATE) == OPT_DATE)

#define START 0
#define GETLOG_ENDBYTES_SIZE 2
#define GETLOG_INFO_SIZE 4  /* number of info bytes written before the actual data, 
                             * limit the size of this 
                             */
struct InfoBytes : public IInfoBytes
{
    char getlog_status;
    ino_t inode;
    const char *getlog_message;
    const char *next_file_in_result_buffer;
    int message_bytes_size;

    string* ToString() {
        return new string (1, getlog_status);
    }
};

class GetLogCommand : public ICommand 
{
    private :
        char opt_byte;
        char subsystem;     // OPT_SUB 
        int size;           // OPT_SIZE
        Date date;          // OPT_DATE

        size_t number_of_processed_files;
        unsigned long processed_files[MAX_NUMBER_OF_FILES_PER_CMD];

    public :
        GetLogCommand();
        GetLogCommand(char opt_byte, char subsystem, size_t size, time_t time);
        ~GetLogCommand();
        void* Execute(size_t *pSize);
        
        char* GetCmdStr(char* cmd_buf);
        void* ParseResult(const char *result, const char *filename); // This function SHOULD be private!!!
        void* ParseResult(const char *result); 

        char* GetNextFile(void);
        size_t ReadFile(char *buffer, const char *filename);
        void MarkAsProcessed(const char *filepath);
        bool isFileProcessed(const char *filepath);
        bool isFileProcessed(unsigned long inode);
        char* FindOldestFile(const char* directory_path, const char* pattern);
        InfoBytes* BuildInfoBytesStruct(InfoBytes* pInfo, const char *buffer);


        static const char* HasNextFile(const char* result);
        static char* GetInfoBytes(char *buffer, const char *filepath);
        static int GetEndBytes(char *buffer);
        static size_t ReadFile_FromStartToEnd(char *buffer, const char *filename, size_t start, 
                                                                                    size_t size);
        static time_t GetFileLastModifTimeT(const char *path);
        static bool prefixMatches(const char* filename, const char* pattern);
        static ino_t GetInoT(const char *filepath);

    private :
        static char* Build_GetLogCommand(char command_buf[GETLOG_CMD_SIZE], char opt_byte, 
                                                        char subsystem, size_t size, time_t date);
};
#endif
