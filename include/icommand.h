#ifndef ICOMMAND_H
#define ICOMMAND_H

class ICommand 
{
    public:
        virtual ~ICommand() {};
        virtual void* Execute() = 0;

        // Intended to the GroundCommander
        // The GroundCommander can use the Command's contructor to build a Command and then
        // call GetCmdStr to build the command buffer to be sent to the satellite. The idea is that 
        // the Commands themselve should have the knowledge of how to build the command buffer.
        // Also, they allow to have a queue of ICommand waiting to be sent and to parse the result buffer returned.
        virtual char* GetCmdStr(char *cmd_buf) { return 0; } // will be set to pure virtual when implemented in all cmds TODO
        virtual void* ParseResult(const char *result) { return 0; } // Idem
};

#endif





