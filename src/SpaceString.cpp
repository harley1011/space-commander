
#include "SpaceString.h"

size_t SpaceString::getUInt(char cstr[4])
{

    Converter_uint_4char converter = {0};

    if (cstr) {
        memcpy(converter.cstr, cstr, 4);
    }

    return converter.uint;
}

char* SpaceString::get4Char(unsigned int uint, char char_buf[4])
{
    Converter_uint_4char converter = {0};    
    
    if (char_buf) {
        converter.uint = uint;
    }

    memcpy(char_buf, converter.cstr, 4);

    return char_buf;
}

