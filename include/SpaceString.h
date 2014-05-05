/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* AUTHORS : Space Concordia 2014, Joseph
*
* TITLE : SpaceString.h
*
* DESCRIPTION : Contains static function related to the conversion from 
*               raw bytes to Type.
*
*----------------------------------------------------------------------------*/
#ifndef SPACE_STRING_H
#define SPACE_STRING_H
#include <string.h>
class SpaceString{

    private :
        union Converter_uint_4char {
            unsigned int uint;
            char cstr[4];
        };

    public:
        static char* get4Char(unsigned int uint, char char_buf[4]);
        static size_t getUInt(char cstr[4]);
};
#endif
