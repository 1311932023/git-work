﻿CC = /usr/bin/gcc
CXX = /usr/bin/g++
CFLAGS = -ggdb -O0 -Wall -fstack-protector-all
CXXFLAGS = -ggdb -O0 -frtti -fexceptions -Wall -fstack-protector-all
INCPATH = #-I/usr/include/mysql
LD = gcc
LFLAGS = #-static
LIBS = -L. -lstdc++

STATIC_LIB = libcommon.a
OBJECTS_LIB = base64.o client.o client2.o daemonize.o log.o parse.o jpacket.o 

DESTDIR_TARGET = test
OBJECTS = test.o $(OBJECTS_LIB)

all: lib

$(DESTDIR_TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

lib: $(STATIC_LIB)

$(STATIC_LIB): $(OBJECTS_LIB)
	ar -r $(STATIC_LIB) $(OBJECTS_LIB)
	mv $(STATIC_LIB) ../lib

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
	rm -rf $(DESTDIR_TARGET) *.o
