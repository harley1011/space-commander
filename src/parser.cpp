#include "parser.h"
#include <cstddef>

bool Parser::IsValidInput(char* input) {
    if (input == NULL) { return false; }

    if (input[0] < 0 || input[0] > 3) { return false; }

    return true;
}

void* Parser::ParseBytes(char* input) {
    if ( !IsValidInput(input) ) { return NULL; }
}
