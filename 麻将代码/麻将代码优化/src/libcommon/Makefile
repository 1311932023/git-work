CC = gcc
CXX = g++
CFLAGS = -g -O0 -Wall
# CXXFLAGS = -O0 -frtti -fexceptions -Wall
CXXFLAGS = -g -O0 -fno-strict-aliasing -Wno-write-strings -frtti -fexceptions -Wall -DDEBUG
INCPATH = -I../libphz -I../zlib -I../include

LD = gcc
LFLAGS = #-static
LIBS =  -L. -lstdc++

STATIC_LIB = libcommon.a
OBJECTS_LIB = daemonize.o log.o redis_client.o client.o game.o jpacket.o \
				player.o base64.o ppacket.o gameproto.pb.o loginproto.pb.o pingxianggameproto.pb.o

DESTDIR_TARGET = test
OBJECTS = test.o $(OBJECTS_LIB)

all: $(DESTDIR_TARGET)

$(DESTDIR_TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

lib: $(STATIC_LIB)

$(STATIC_LIB): $(OBJECTS_LIB)
	ar -r $(STATIC_LIB) $(OBJECTS_LIB)

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