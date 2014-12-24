#ifndef COMMAND_PARAM_H
#define COMMAND_PARAM_H

#include <cstddef>
#include <vector>
#include <string>

using namespace std;


class Field {
public:
    void SetUChar(unsigned char value) {
        type = 0;
        data.UChar = value;
    }

    void SetInt(int value) {
        type = 1;
        data.Int = value;
    }
    
    void SetString(string* value) {
        type = 2;
        data.String = value;
    }

    char GetType()           { return type; }
    unsigned char GetUChar() { return data.UChar; }
    int GetInt()             { return data.Int; }
    string* GetString()      { return data.String; }

private:
    char type;
    union  {
        unsigned char UChar;
        int Int;
        string* String;
    } data;
};

struct CommandParam {
    char type;
    vector<Field> fields;

    ~CommandParam() {
        for (unsigned int i = 0; i != fields.size(); i++) {
            if (fields[i].GetType() == 2) { 
                if (fields[i].GetString() != NULL) {
                    delete fields[i].GetString();
                    fields[i].SetString(NULL);
                }
            }
        }
    }
};

#endif
