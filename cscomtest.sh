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
MULTIPLE_RUN=0
GROUP_LIST=(getlog deletelog net2com commander)

usage()
{
    echo "usage : cscomtest.sh  [-u] [-g testGroup] [-n testName] [-m numberOfRuns][-v]"
    echo "          -m numberOfRuns : run the specified tests 'numberOfRuns' times and stop if error" 
    echo "          -v verbose : to get all DEBUG info (N.B. DEBUG info can be turned on/off in the makefile ... -DDEBUG)"
    echo "          -u usage"
    printf "%s" "          -g group   : one of those -> " 
    for gr in ${GROUP_LIST[@]}; do
        printf "%s " $gr
    done
    echo
}


#
# Parses command line arguments
#
argType=""
while getopts "g:n:uvm:" opt; do
    case "$opt" in
        g) GROUP=$OPTARG;;
        m) MULTIPLE_RUN=$OPTARG;;
        n) SINGLE_TEST="-n $OPTARG" ;;
        u)
            usage
            exit 0;
        ;;
        v) TODEVNULL=0 ;;

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
        'commander')    ARGUMENTS="-g CommanderTestGroup";;
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
    echo -e "\e[31m Build failed\e[0m"
    exit -1
else
    echo -e "\e[32m Build success!\e[0m"
fi

echo ""
echo "=== Run tests ==="

counter=0

while [ $counter -lt $MULTIPLE_RUN ]; do
    if [ $TODEVNULL -ne 0 ]; then
        echo $ALLTESTS $ARGUMENTS 2>/dev/null
        $ALLTESTS $ARGUMENTS  2>/dev/null
    else
        echo $ALLTESTS $ARGUMENTS 
        $ALLTESTS $ARGUMENTS  
    fi

    if [ $? -ne 0 ]; then
        echo -e "\e[31m UTest Failure!\e[0m"
        break;
    else 
        echo -e "\e[32m UTest Success!\e[0m"
    fi

    counter=$(($counter+1))
done

