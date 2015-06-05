# Space Commander

The Space Commander is a process that receives commands from the ground station and sends replies. It communicates with the Network Layer (Netman) via Named Pipes for IPC. The info pipe is a control channel that tells the Commander when there is a command and or data to process in the data pipe. 

Net2Com - when the Commander starts, it opens an instance of the Net2Com class, which makes sure the pipes are opened with the right settings, and provides functionality for reading to and writing from the pipes. 

The space-commander main function has the Commander reading at a set frequency from the Info Pipe. When bytes are written there, the bytes are analysed to match the control sequence to tell the Commander there is a command and/or associated data in the data pipe. The data pipe is then scanned, and a command buffer is created with the command ID and any associated data. This command buffer is sent to the CommandFactory, which creates an instance of the appropriate command. That command's Execute() function is called, which returns a result buffer containing the response of the command. The response buffer may contain some pertinent data, or a SUCCESS/ERROR message. 

Each command has functions to both create its own command buffer, as well as parse its own result buffer. 
e.g.
GetLogCommand::Build_GetLogCommand
GetLogCommand::ParseResult

The Build_Command function returns a pointer to the command buffer array
The ParseResult returns a pointer to an InfoBytes struct, containing the command ID, the command status (0 for success, 1-255 for various statuses), and some complimentary data (like log data, the time on the satellite, etc.)

## Ground/Flight Context
Ground Commander and Space Commander are structured as follows:

/common <-- files common to both

/ground-commander

/space-commander

The Ground Commander reads commands from a file, and implements all the commands in order to maintain consistency with the Space Commander, and be provided the command-specific ParseResult function. 

make buildBin

## Unit tests

./cscomtest.sh  [-g Group][-n testName][-v][-u]

## Net2Com

Both the Netman AND the Commander have to have their instance of the Net2Com BEFORE using the pipes!


### Command Step 1

echo -n -e \\x01 > Inet-w-com-r 

echo -n -e \\x31 > Dnet-w-com-r 

echo -n -e \\xFF > Inet-w-com-r

### Command Step 2

echo -n -e \\x01 > Inet-w-com-r 

echo -n -e \\x21 > Dnet-w-com-r 

echo -n -e \\xFF > Inet-w-com-r
