#ifndef PARSER_H
#define PARSER_H

class Parser {
public:
    static void* ParseBytes(char*);
private:
    static bool IsValidInput(char *);
};
#endif
