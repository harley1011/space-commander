CC=g++
MICROCC=microblazeel-xilinx-linux-gnu-g++
BEAGLECC=arm-linux-gnueabi-g++

#
# Paths
#
SPACE_LIB = ../space-lib
SPACE_UTLS = $(SPACE_LIB)/utls
SPACE_UPTDATER = ../space-updater
CPPUTEST_HOME = ../CppUTest
SPACE_SCRIPT = ../space-script

#
# Flags
#
CFLAGS += -Wall
CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
MEM_LEAK_MACRO = -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h  -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb

#
# includes
#
INCLUDES = -I./include/ -I$(SPACE_LIB)/shakespeare/inc -I$(SPACE_LIB)/include -I$(SPACE_UPTDATER)/include -I$(SPACE_SCRIPT)/tgz-wizard/include -I$(SPACE_UTLS)/include
INCTESTPATH = -I./tests/unit/stubs/ -I./tests/helpers/include/

#
# Libraries
#
LIBPATH=-L./lib/  -L$(SPACE_LIB)/shakespeare/lib -L$(CPPUTEST_HOME)/lib -L$(SPACE_UTLS)/lib
LIBS=-lshakespeare -lcs1_utls 
CPPUTEST_LIBS=-lCppUTest -lCppUTestExt 

buildQ6:
	$(MICROCC) $(MICROCFLAGS) $(INCLUDES) src/*.cpp -o bin/space-commanderQ6
buildBB:
	$(BEAGLECC) $(INCLUDES) $(DEBUGFLAGS) src/*.cpp -o bin/space-commanderBB

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBPATH) -c $^ -o $@

%.a: %.o
	ar -cvq $@ $^

staticlibs.tar: src/NamedPipe.a src/Net2Com.a
	mv $^ ./
	tar -cf $@ include/NamedPipe.h include/Net2Com.h NamedPipe.a Net2Com.a
	rm *.a

src/NamedPipeQ6.o: src/NamedPipe.cpp
	$(MICROCC) $(MICROCFLAGS) $(INCLUDES) -c $^ -o $@

src/Net2ComQ6.o : src/Net2Com.cpp
	$(MICROCC) $(MICROCFLAGS) $(INCLUDES) -c $^ -o $@

src/NamedPipe-mbcc.a: src/NamedPipeQ6.o
	ar -cvq $@ $^

src/Net2Com-mbcc.a: src/Net2ComQ6.o
	ar -cvq $@ $^

src/NamedPipeBB.o: src/NamedPipe.cpp
	$(BEAGLECC) $(MICROFLAGS) $(INCLUDES) -c $^ -o $@

src/Net2ComBB.o: src/Net2Com.cpp
	$(BEAGLECC) $(MICROFLAGS) $(INCLUDES) -c $^ -o $@

NamedPipe-BB.a: src/NamedPipeBB.o
	ar -cvq $@ $^

Net2Com-BB.a: src/Net2ComBB.o
	ar -cvq $@ $^

staticlibsQ6.tar: src/NamedPipe-mbcc.a src/Net2Com-mbcc.a
	mv $^ ./
	tar -cf $@ include/NamedPipe.h include/Net2Com.h NamedPipe-mbcc.a Net2Com-mbcc.a


#
#++++++++++++++++++++
# 	CppUTest / PC
#--------------------
#
# All Object files, do not use wildcard, add the ones you need explicitly!
#
OBJECTS = bin/Net2Com.o bin/NamedPipe.o bin/command-factory.o bin/deletelog-command.o  bin/decode-command.o bin/getlog-command.o bin/gettime-command.o bin/reboot-command.o bin/settime-command.o bin/update-command.o bin/base64.o bin/subsystems.o 
#
# CppUTest files, no wildcard, add files explicitly!
#
UNIT_TEST = tests/unit/Net2Com-test.cpp  tests/unit/deletelog-command-test.cpp  tests/unit/getlog-command-test.cpp tests/unit/commander-test.cpp tests/unit/settime-command-test.cpp
CS1_UTEST_DIR="cs1_utest" # as defined in SpaceDecl.h
#
# ENV : either CS1_UTEST for test environment or empty for PROD, perform a 'make clean' when changing this parameter
#
UTEST_ENV=-DCS1_UTEST $(MEM_LEAK_MACRO) $(CPPUTEST_LIBS) 
ENV = -DDEBUG  $(UTEST_ENV)  #-DPRESERVE

buildBin: bin/space-commander

test: bin/AllTests bin/space-commander
	mkdir -p $(CS1_UTEST_DIR)

bin/%.o: src/%.cpp include/%.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) -c $< -o $@ $(ENV) 
	
bin/fileIO.o: $(SPACE_UPTDATER)/src/fileIO.cpp $(SPACE_UPTDATER)/include/fileIO.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) -c $< -o $@ $(ENV)

bin/dirUtl.o: $(SPACE_SCRIPT)/tgz-wizard/src/dirUtl.cpp $(SPACE_SCRIPT)/tgz-wizard/include/dirUtl.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) -c $< -o $@ $(ENV)

bin/AllTests: tests/unit/AllTests.cpp  $(UNIT_TEST) $(OBJECTS) bin/fileIO.o	 bin/dirUtl.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(LIBPATH) -o $@ $^ $(LIBS) $(ENV)

bin/space-commander: src/space-commander-main.cpp $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(LIBPATH) -o $@ $^ $(LIBS) $(ENV)
#
#
#++++++++++++++++++++
# Cleanup
#--------------------
clean:
	rm -f *.o *~ ./bin/* AllTests  && rm -fr ./cs1_utest
