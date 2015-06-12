/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2015
*
* TITLE : icommand.h 
*
* NOTES :
*       CMD_HEAD_SIZE => size of the common header for sent commands
*       [0] - CMD_ID
*       [?] - CMD_CID // TODO
*
*       CMD_RES_HEAD_SIZE  => size of the common header for result buffers
*       [1] - CMD_STS (for result buffer only only)
*       [?] - CMD_CID TODO
*
*----------------------------------------------------------------------------*/
#ifndef ICOMMAND_H
#define ICOMMAND_H


#define CMD_HEAD_SIZE 1
#define CMD_RES_HEAD_SIZE 2
#define CMD_ID 0
#define CMD_STS 1

#include <string.h>
#include "SpaceDecl.h"
#include "infobytes.h"

class ICommand {
    protected :
        char* log_buffer;

    public :
        ICommand() {
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

        virtual void* Execute(size_t* size){return 0;} 

        // Intended to the GroundCommander
        // The GroundCommander can use the Command's contructor to build a Command and then
        // call GetCmdStr to build the command buffer to be sent to the satellite. The idea is that 
        // the Commands themselve should have the knowledge of how to build the command buffer.
        // Also, they allow to have a queue of ICommand waiting to be sent and to parse the result buffer returned.
        virtual char* GetCmdStr(char *cmd_buf) { return 0;}; // will be set to pure virtual when implemented in all cmds TODO
        virtual InfoBytes* ParseResult(char *result) = 0; // Idem
};

#endif





