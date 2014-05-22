#!/bin/sh
#**********************************************************************************************************************
#
# AUTHORS : Space Concordia 2014, Joseph
#
# FILE : cscomtest.sh
# 
# PURPOSE : Conveniently runs the tests.
#           -g      Group
#           -n      TestName
#           -v      verbose (to get all DEBUG output)
# 
#       ex. ./cscomtest.sh                  =>   run ALL the tests
#           ./cscomtest.sh -g deletelog     =>   run ALL deletelog tests
#           ./cscomtest.sh -n nameOfTheTest
#
#**********************************************************************************************************************

ALLTESTS="./bin/AllTests"
ARGUMENTS=""
GROUP=""
TODEVNULL=1

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
        -n) SINGLE_TEST="-n $arg";;
    esac
    
    argType=""    

    case $arg in
        '-g'|'-n')
            argType=$arg
        ;;
        -u)
            usage
            exit 0;
        ;;
        -v) TODEVNULL=0 ;;
    esac
done 

#
# GROUP
#
if [ "$GROUP" != "" ]; then
    case $GROUP in 
        'getlog')       ARGUMENTS="-g GetLogTestGroup" ;;
        'deletelog')    ARGUMENTS="-g DeleteLogTestGroup" ;;
        'net2com')      ARGUMENTS="-g Net2ComTestGroup" ;;
    esac
fi

ARGUMENTS="$ARGUMENTS $SINGLE_TEST" 

#
# Execute
#
echo ""
echo "=== Build tests ==="
make test

if [ $? -ne 0 ]; then
    echo "[ERROR] build failed"
    exit -1
fi

echo ""
echo "=== Run tests ==="

if [ $TODEVNULL -ne 0 ]; then
    echo $ALLTESTS $ARGUMENTS 2>/dev/null
    $ALLTESTS $ARGUMENTS  2>/dev/null
else
    echo $ALLTESTS $ARGUMENTS 
    $ALLTESTS $ARGUMENTS  
fi
