#ifndef ICOMMAND_H
#define ICOMMAND_H

class ICommand 
{
    public:
        virtual ~ICommand() {};
        virtual void* Execute() = 0;

        // Those functions are intended to the GroundCommander
        // They allow to have a queue of ICommand waiting to be sent and to parse the result buffer returned.
        virtual char* GetCmdStr(char *cmd_buf) { return 0; } // will be set to pure virtual when implemented in all cmds TODO
        virtual void* ParseResult(char *result) { return 0; } // Idem
};

#endif





