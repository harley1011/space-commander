CC=g++
MBCC=microblazeel-xilinx-linux-gnu-g++
BEAGLECC=arm-linux-gnueabi-g++

#
#++++++++++++++++++++
# Paths
#--------------------
SPACE_LIB = ../space-lib
SPACE_UTLS = $(SPACE_LIB)/utls
SPACE_UPTDATER = ../space-updater
CPPUTEST_HOME = ../CppUTest
SPACE_SCRIPT = ../space-script

#
#++++++++++++++++++++
# Flags
#--------------------
CFLAGS += -Wall
CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
MEM_LEAK_MACRO = -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h  -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb

#
#++++++++++++++++++++
# includes
#--------------------
INCLUDES = -I./include/ -I$(SPACE_LIB)/shakespeare/inc -I$(SPACE_LIB)/include -I$(SPACE_UPTDATER)/include -I$(SPACE_SCRIPT)/tgz-wizard/include -I$(SPACE_UTLS)/include
INCTESTPATH = -I./tests/unit/stubs/ -I./tests/helpers/include/

#
#++++++++++++++++++++
# Libraries
#--------------------
LIBPATH=-L./lib/  -L$(SPACE_LIB)/shakespeare/lib -L$(CPPUTEST_HOME)/lib -L$(SPACE_UTLS)/lib

make_dir:
	mkdir -p bin && mkdir -p lib

#
#++++++++++++++++++++
# 	CppUTest / PC
#--------------------
LIBS=-lshakespeare -lcs1_utls  
CPPUTEST_LIBS=-lCppUTest -lCppUTestExt 

#
# All Object files, do not use wildcard, add the ones you need explicitly!
#
OBJECTS = bin/Net2Com.o bin/NamedPipe.o bin/command-factory.o bin/deletelog-command.o  bin/decode-command.o bin/getlog-command.o bin/gettime-command.o bin/reboot-command.o bin/settime-command.o bin/update-command.o bin/base64.o bin/subsystems.o 
#
# CppUTest files, no wildcard, add files explicitly!
#

UNIT_TEST = tests/unit/Net2Com-test.cpp  tests/unit/deletelog-command-test.cpp  tests/unit/getlog-command-test.cpp tests/unit/commander-test.cpp
CS1_UTEST_DIR="cs1_utest" # as defined in SpaceDecl.h

#
# ENV : either CS1_UTEST for test environment or empty for PROD, perform a 'make clean' when changing this parameter
#
UTEST_ENV=-DCS1_UTEST $(MEM_LEAK_MACRO) $(CPPUTEST_LIBS) 
ENV = -DCS1_DEBUG  $(UTEST_ENV)  #-DPRESERVE

#
#++++++++++++++++++++
# CppUTest / PC Rules
#--------------------
buildBin: make_dir bin/space-commander

bin/%.o: src/%.cpp include/%.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) -c $< -o $@ $(ENV) 

bin/space-commander: src/space-commander-main.cpp $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(LIBPATH) -o $@ $^ $(LIBS) $(ENV)

test: make_dir bin/AllTests bin/space-commander
	mkdir -p $(CS1_UTEST_DIR)

bin/AllTests: tests/unit/AllTests.cpp  $(UNIT_TEST) $(OBJECTS) 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(LIBPATH) -o $@ $^ $(LIBS) $(ENV)

#
#++++++++++++++++++++
#  MicroBlaze 
#--------------------
LIBS_Q6= -lshakespeare-mbcc -lcs1_utlsQ6

OBJECTS_Q6 = bin/Net2ComQ6.o bin/NamedPipeQ6.o bin/command-factoryQ6.o bin/deletelog-commandQ6.o  bin/decode-commandQ6.o bin/getlog-commandQ6.o bin/gettime-commandQ6.o bin/reboot-commandQ6.o bin/settime-commandQ6.o bin/update-commandQ6.o bin/base64Q6.o bin/subsystemsQ6.o 

#
#++++++++++++++++++++
# MicroBlaze Rules 
#--------------------
buildQ6:  make_dir bin/space-commanderQ6
	
bin/%Q6.o: src/%.cpp include/%.h
	$(MBCC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) -c $< -o $@

bin/space-commanderQ6: src/space-commander-main.cpp $(OBJECTS_Q6)
	$(MBCC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(LIBPATH) -o $@ $^ $(LIBS_Q6)


#
#
#++++++++++++++++++++
# Cleanup
#--------------------
clean:
	rm -fr ./bin ./lib ./cs1_utest 




#++++++++++++++++++++
# buildLib 
#--------------------
staticlibs.tar: make_dir lib/NamedPipe.a lib/Net2Com.a
	tar -cf $@ include/NamedPipe.h include/Net2Com.h lib/NamedPipe.a lib/Net2Com.a

staticlibsQ6.tar: lib/NamedPipe-mbcc.a lib/Net2Com-mbcc.a
	tar -cf $@ include/NamedPipe.h include/Net2Com.h lib/NamedPipe-mbcc.a lib/Net2Com-mbcc.a

lib/NamedPipe.a: bin/NamedPipe.o
	ar -cvq $@ $^

lib/Net2Com.a: bin/Net2Com.o
	ar -cvq $@ $^

lib/NamedPipe-mbcc.a: bin/NamedPipeQ6.o
	ar -cvq $@ $^

lib/Net2Com-mbcc.a: bin/Net2ComQ6.o
	ar -cvq $@ $^




#
#
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# TODO cleanup what is below
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
#

buildBB:
	$(BEAGLECC) $(INCLUDES) $(DEBUGFLAGS) src/*.cpp -o bin/space-commanderBB

%.a: %.o
	ar -cvq $@ $^

bin/NamedPipeBB.o: src/NamedPipe.cpp
	$(BEAGLECC) $(MICROFLAGS) $(INCLUDES) -c $^ -o $@

bin/Net2ComBB.o: src/Net2Com.cpp
	$(BEAGLECC) $(MICROFLAGS) $(INCLUDES) -c $^ -o $@

NamedPipe-BB.a: bin/NamedPipeBB.o
	ar -cvq $@ $^

Net2Com-BB.a: src/Net2ComBB.o
	ar -cvq $@ $^
