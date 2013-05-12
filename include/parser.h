#ifndef PARSER_H
#define PARSER_H

#include "command-param.h"
class Parser {
public:
    static CommandParam* ParseBytes(char*);
private:
    static bool IsValidInput(char *);
};
#endif
