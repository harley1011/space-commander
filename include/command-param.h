#ifndef COMMAND_PARAM_H
#define COMMAND_PARAM_H

#include <cstddef>

struct CommandParam {
    char type;
    void** fields;

    ~CommandParam() {
        if (fields != NULL) {
            delete[] fields;
            fields = 0;
        }
    }
};

#endif
