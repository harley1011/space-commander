/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Joseph
*
* TITLE :  class GetLogCommand
*
* DESCRIPTION : Retreive a log archive.
*
*----------------------------------------------------------------------------*/
#ifndef GETLOG_COMMAND_H
#define GETLOG_COMMAND_H

#include <cstdlib>
#include <string>

#include "SpaceDecl.h"
#include "Date.h"
#include "subsystem.h"
#include "icommand.h"

using namespace std;

#define OPT_NOOPT 0x00
#define OPT_SUB 0x01
#define OPT_SIZE 0x02
#define OPT_DATE 0x04

#define OPT_ISNOOPT(x)  ((x) == OPT_NOOPT)
#define OPT_ISSUB(x)    (((x) & OPT_SUB) == OPT_SUB)
#define OPT_ISSIZE(x)   (((x) & OPT_SIZE) == OPT_SIZE)
#define OPT_ISDATE(x)   (((x) & OPT_DATE) == OPT_DATE)


class GetLogCommand : public ICommand {
    private :
        char opt_byte;
        char subsystem;     // OPT_SUB 
        int size;           // OPT_SIZE
        Date date;          // OPT_DATE


    public :
        GetLogCommand();
        GetLogCommand(char opt_byte, char subsystem, size_t size, time_t time);
        ~GetLogCommand();
        void* Execute();

        static time_t GetFileLastModifTimeT(const char *path);
        static char* FindOldestFile(const char* directory_path);


};
#endif
