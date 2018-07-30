CC = gcc
CXX = g++
CFLAGS = -O0 -Wall
# CXXFLAGS = -O0 -frtti -fexceptions -Wall
CXXFLAGS = -g -O0 -fno-strict-aliasing -Wno-write-strings -frtti -fexceptions -Wall -DDEBUG
INCPATH = -I./libphz -I./zlib -I./include -I./libcommon

LD = gcc
LFLAGS = #-static
LIBS =  -L./libphz -L./zlib -L./lib -L./libcommon -lphz -lzlib -lcommon -lprotobuf -lstdc++ -lev -ljsoncpp -lhiredis 

DESTDIR_TARGET = pingxiangmajiang
OBJECTS = phz.o table_cardpoor.o table_operate.o table_recover.o table_common.o table_create.o seat.o table.o ./location/table_pingxiangmj.o

all: $(DESTDIR_TARGET)

$(DESTDIR_TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

####### Implicit rules ######

.SUFFIXES: .cpp .cc .cxx .c
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

clean:
	rm -rf $(DESTDIR_TARGET) *.o ./location/*.o
