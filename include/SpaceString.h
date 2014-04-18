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

    public:

    static size_t getUInt(char cstr[4]){
        union SizeUnion {
            unsigned int size;
            char cstr[4];
        };

        SizeUnion converter = {0};

        memcpy(converter.cstr, cstr, 4);

        return converter.size;
    }
};
#endif
