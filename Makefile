CC=g++
MICROCC=microblazeel-xilinx-linux-gnu-g++
BEAGLECC=arm-linux-gnueabi-g++

#
# Paths
#
SPACE_LIB = ../space-lib
SPACE_UPTDATER = ../space-updater
CPPUTEST_HOME = ../space-commander
SPACE_SCRIPT = ../space-script

#
# Flags
#
CFLAGS += -Wall
CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
MEM_LEAK_MACRO = -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb

#
# includes
#
INCPATH = -I./include/ -I$(SPACE_LIB)/shakespeare/inc -I$(SPACE_SCRIPT)/include -I$(SPACE_UPTDATER)/include
INCTESTPATH = -I./tests/unit/stubs/ -I./tests/helpers/include/

#
# Libraries
#
LIBPATH=-L./lib/  -L$(SPACE_LIB)/shakespeare/lib
LIBS=-lCppUTest -lCppUTestExt -lshakespeare

#
# The test builds have their own main provided by CppUTest so we need to exclude commander.cpp
#
DEBUG_SRC_FILES =`find src/ ! -name 'space-commander-main.cpp' -name '*.cpp'`

buildBin:
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) $(DEBUGFLAGS) src/*.cpp -o bin/space-commander
buildQ6:
	$(MICROCC) $(MICROCFLAGS) $(INCPATH) src/*.cpp -o bin/space-commanderQ6
buildBB:
	$(BEAGLECC) $(INCPATH) $(DEBUGFLAGS) src/*.cpp -o bin/space-commanderBB

buildAllTests: buildUnitTests
buildUnitTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) src/*.cpp tests/unit/*.cpp -o bin/AllUnitTests $(LIBS)


%.o: %.cpp
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) -c $^ -o $@

%.a: %.o
	ar -cvq $@ $^

staticlibs.tar: src/NamedPipe.a src/Net2Com.a
	mv $^ ./
	tar -cf $@ include/NamedPipe.h include/Net2Com.h NamedPipe.a Net2Com.a
	rm *.a

src/NamedPipeQ6.o: src/NamedPipe.cpp
	$(MICROCC) $(MICROCFLAGS) $(INCPATH) -c $^ -o $@

src/Net2ComQ6.o : src/Net2Com.cpp
	$(MICROCC) $(MICROCFLAGS) $(INCPATH) -c $^ -o $@

src/NamedPipe-mbcc.a: src/NamedPipeQ6.o
	ar -cvq $@ $^

src/Net2Com-mbcc.a: src/Net2ComQ6.o
	ar -cvq $@ $^

src/NamedPipeBB.o: src/NamedPipe.cpp
	$(BEAGLECC) $(MICROFLAGS) $(INCPATH) -c $^ -o $@

src/Net2ComBB.o: src/Net2Com.cpp
	$(BEAGLECC) $(MICROFLAGS) $(INCPATH) -c $^ -o $@

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
OBJECTS = bin/Net2Com.o bin/NamedPipe.o bin/Date.o bin/command-factory.o bin/deletelog-command.o  bin/decode-command.o bin/getlog-command.o bin/gettime-command.o bin/reboot-command.o bin/settime-command.o bin/update-command.o bin/base64.o bin/subsystems.o bin/SpaceString.o
#
# CppUTest files, no wildcard, add files explicitly!
#
UNIT_TEST = tests/unit/Net2Com-test.cpp tests/unit/Utl-test.cpp tests/unit/deletelog-command-test.cpp  tests/unit/getlog-command-test.cpp
#
# ENV : either CS1_UTEST for test environment or empty for PROD, perform a 'make clean' when changing this parameter
#
ENV = -DCS1_UTEST

all: bin/space-commander

test: AllTests

bin/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCPATH) -c $< -o $@ $(ENV)
	
bin/fileIO.o: $(SPACE_UPTDATER)/src/fileIO.cpp $(SPACE_UPTDATER)/include/fileIO.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCPATH) -c $< -o $@ $(ENV)

AllTests: tests/unit/AllTests.cpp  $(UNIT_TEST) $(OBJECTS) bin/fileIO.o	
	$(CC) $(CFLAGS) $(MEM_LEAK_MACRO) $(CPPFLAGS) $(CXXFLAGS) $(INCPATH) $(LIBPATH) -o $@ $^ $(LIBS) $(ENV)

bin/space-commander: src/space-commander-main.cpp $(OBJECTS)
	$(CC) $(CFLAGS) $(MEM_LEAK_MACRO) $(CPPFLAGS) $(CXXFLAGS) $(INCPATH) $(LIBPATH) -o $@ $^ $(LIBS) $(ENV)
#
#
# Cleanup
#
clean:
	rm -f *.o *~ ./bin/* AllTests
