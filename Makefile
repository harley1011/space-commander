CC=g++
MICROCC=microblazeel-xilinx-linux-gnu-g++
CFLAGS=-Wall
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb
INCPATH=-I./include/
INCTESTPATH=-I./tests/unit/stubs/ -I./tests/helpers/include/
LIBPATH=-L./lib/
LIBS=-lCppUTest -lCppUTestExt
#The test builds have their own main provided by CppUTest so we need to exclude commander.cpp
DEBUG_SRC_FILES =`find src/ ! -name 'space-commander-main.cpp' -name '*.cpp'`

buildBin:
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) -DPC src/*.cpp -o bin/space-commander
buildQ6:
	$(MICROCC) $(MICROCFLAGS) $(INCPATH) src/*.cpp -o bin/space-commanderQ6

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
