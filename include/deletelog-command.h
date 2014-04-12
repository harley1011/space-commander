/*=============================================================================
*
*   AUTHOR      : Space Concordia 2014  -  Joseph
*
*   PURPOSE     : The DeleteLogCommand deletes the specified file  located in
*                 CS1_LOGS or CS1_TGZ directory.
*
*============================================================================*/
#ifndef DELETELOG_COMMAND_H
#define DELETELOG_COMMAND_H

#include "icommand.h"

#define LOG 0x0
#define TGZ 0x1

class DeleteLogCommand : public ICommand {
    private :
        char* filename;
        char type;


    public :

        DeleteLogCommand(const char* filename);
        virtual void* Execute();
        virtual ~DeleteLogCommand();
        char FindType();
};

#endif
