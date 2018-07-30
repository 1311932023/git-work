CC = /usr/bin/gcc
CXX = /usr/bin/g++
CFLAGS = -ggdb -O0 -Wall
CXXFLAGS = -ggdb -O0 -fno-strict-aliasing -Wno-write-strings -frtti -fexceptions -Wall
INCPATH = -I../include
LD = /usr/bin/gcc
LFLAGS = 
LIBS = -L../lib  -lprotobuf -lstdc++
	
STATIC_LIB = libcomproto.a
OBJECTS_LIB = gameproto.pb.o hainangameproto.pb.o hunangameproto.pb.o sichuangameproto.pb.o jiangxigameproto.pb.o guizhougameproto.pb.o\
 gdgameproto.pb.o henangameproto.pb.o pinghugameproto.pb.o hubeigameproto.pb.o\
 shanxigameproto.pb.o jinhuagameproto.pb.o loginproto.pb.o fuzhoumahjonggame.pb.o dgkgameproto.pb.o nanninggameproto.pb.o\
 nanpingmahjonggame.pb.o  laibingameproto.pb.o longyanmj.pb.o yuanjianggameproto.pb.o ganzhoucg.pb.o xiangyanggameproto.pb.o \
 zhenjiang.pb.o xixiahuashuigameproto.pb.o jingshangameproto.pb.o qionglaigameproto.pb.o anqinggameproto.pb.o nanchang.pb.o chongqinggameproto.pb.o \
 jiujiang.pb.o ningdegameproto.pb.o baohu.pb.o dongxiang.pb.o antu.pb.o daozhou.pb.o xuezhan.pb.o yudu.pb.o\
 loginproto_match.pb.o chengdugameproto.pb.o


all: $(STATIC_LIB)

$(STATIC_LIB): $(OBJECTS_LIB)	
	ar -r $(STATIC_LIB) $(OBJECTS_LIB)
	mv $(STATIC_LIB) ../lib
	
#makeproto:  
#	protoc --cpp_out=./ *.proto
	
.SUFFIXES: .cpp .cc .cxx .c
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

.pb.cc.o:
	$(CXX) -c $(CFLAGS) $(INCPATH) -o $@ $<
	
clean:
	rm -rf ./*.pb.*
