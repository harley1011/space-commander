#include "parser.h"
#include <cstddef>

bool Parser::IsValidInput(char* input) {
    if (input == NULL) { return false; }

    if (input[0] < 0 || input[0] > 3) { return false; }

    return true;
}

CommandParam* Parser::ParseBytes(char* input) {
    if ( !IsValidInput(input) ) { return NULL; }

    CommandParam* result = new CommandParam();
    result->type = input[0];
    
    if (input[0] == 2) {
        result->fields    = new void*[1];
        result->fields[0] = (void*)input[1];
    }

    return result;

}
