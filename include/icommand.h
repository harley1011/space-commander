#ifndef ICOMMAND_H
#define ICOMMAND_H


// [0] - CMD_ID
// [1] - STATUS
#define CMD_HEAD_SIZE 2
#define CMD_ID 0
#define CMD_STS 1

#include <string.h>
#include "SpaceDecl.h"

class ICommand 
{
    protected :
        char* log_buffer;

    public :
        ICommand()
        {
            this->log_buffer = new char[CS1_MAX_LOG_ENTRY];
            memset(this->log_buffer, 0, CS1_MAX_LOG_ENTRY);
        }

        virtual ~ICommand() 
        {
            if (this->log_buffer) 
            {
                delete[] this->log_buffer;
                this->log_buffer = 0;
            }
        };

        virtual void* Execute(size_t* size){ return this->Execute(); } // The other Execute will be removed as soon as ALL commands 
                                                                       // comply to the new signature Execute(size_t* size) TODO
        virtual void* Execute() = 0;

        // Intended to the GroundCommander
        // The GroundCommander can use the Command's contructor to build a Command and then
        // call GetCmdStr to build the command buffer to be sent to the satellite. The idea is that 
        // the Commands themselve should have the knowledge of how to build the command buffer.
        // Also, they allow to have a queue of ICommand waiting to be sent and to parse the result buffer returned.
        virtual char* GetCmdStr(char *cmd_buf) { return 0; } // will be set to pure virtual when implemented in all cmds TODO
     //   virtual void* ParseResult(const char *result) { return 0; } // Idem
};

#endif





