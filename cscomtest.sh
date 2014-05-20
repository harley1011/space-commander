#!/bin/sh
#**********************************************************************************************************************
#
# AUTHORS : Space Concordia 2014, Joseph
# 
# PURPOSE : Conveniently runs the tests.
#
#**********************************************************************************************************************

ALLTESTS="./bin/AllTests"
ARGUMENTS=""
GROUP=""
TODEVNULL="1"

usage()
{
    echo "usage : TODO"
}


#
# Parses command line arguments
#
argType=""

for arg in "$@"; do
    case $argType in
        -g) GROUP=$arg ;;
        -v) TODEVNULL="0" ;;
    esac
    
    argType=""    

    case $arg in
        -u)
            usage
            exit 0;
        ;;
        *)
            argType=$arg
        ;;
    esac
done 

#
# GROUP
#
if [ "$GROUP" != "" ]; then
    case $GROUP in 
        'getlog') ARGUMENTS="-g GetLogTestGroup" ;;
        'deletelog') echo "TODO";;
    esac
fi


#
# Execute
#
if [ $TODEVNULL -ne 0 ]; then
    echo $ALLTESTS $ARGUMENTS 2>/dev/null
    $ALLTESTS $ARGUMENTS  2>/dev/null
else
    echo $ALLTESTS $ARGUMENTS 
    $ALLTESTS $ARGUMENTS  
fi
