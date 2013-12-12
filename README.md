# Space Commander

## Overview

The commander is a process that receives commands from the ground station and sends replies.

make buildBin

### Command Step 1

echo -n -e \\x01 > Inet-w-com-r 

echo -n -e \\x31 > Dnet-w-com-r 

echo -n -e \\xFF > Inet-w-com-r

### Command Step 2

echo -n -e \\x01 > Inet-w-com-r 

echo -n -e \\x21 > Dnet-w-com-r 

echo -n -e \\xFF > Inet-w-com-r
