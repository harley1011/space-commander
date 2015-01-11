#ifndef IINFOBYTES_H 
#define IINFOBYTES_H
#include <string.h>

struct IInfoBytes{
    public:
        virtual std::string* ToString() = 0; 
};

#endif
