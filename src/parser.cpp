#include "parser.h"
#include <cstddef>
#include <string>
#include <vector>

using namespace std;

bool Parser::IsValidInput(char* input) {
    if (input == NULL) { return false; }

    if (input[0] < 0 || input[0] > 3) { return false; }

    return true;
}


CommandParam* Parser::ParseBytes(char* input) {
    if ( !IsValidInput(input) ) { return NULL; }

    CommandParam* result = new CommandParam();
    result->type = input[0];
    
    Field f;
    switch(input[0]) {
        case 1: {
            string* s = new string((input+2), input[1]);        
            f.SetString(s);
            result->fields.push_back(f);
        }

    }
    if (input[0] == 2) {
        f.SetUChar(input[1]);
        result->fields.push_back(f);
    }

    return result;

}

