CC = gcc
CXX = g++
CFLAGS = -ggdb -O0 -Wall
# CXXFLAGS = -O0 -frtti -fexceptions -Wall
CXXFLAGS = -ggdb -O0 -fno-strict-aliasing -Wno-write-strings -frtti -fexceptions -Wall -DDEBUG
INCPATH = -I./libnormalmahjong -I./protocol -I./include -I./include/curl -I/usr/local/include -I./proto

#福州麻将添加宏定义 ,其余几个Makefile文件也需要添加同步修改.
#FZ_DEF = -DMJ_HAND_CARD_17
#CFLAGS += $(FZ_DEF)
#CXXFLAGS += $(FZ_DEF)
#end

LD = gcc
LFLAGS = #-static
LIBS =  -L./lib -lnormalmahjong -lacp -lstdc++ -L./lib -lcommon -lcomproto -lprotobuf -ljsoncpp -lev -lcurl -lcrypto -lhiredis

DESTDIR_TARGET = yunxiaomajiang
OBJECTS = zjh.o game.o table.o screenword.o date.o  game_account.o redis_cache.o redis_client.o\
 robot_manager.o table_helper.o table_helper2.o table_helper3.o table2.o table3.o table4.o table_disband.o message_word.o\
 CardPool_ReadJson.o location/table_yunxiaomajiang.o

 
all: $(DESTDIR_TARGET)

$(DESTDIR_TARGET): $(OBJECTS)	
	$(LD) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

rebuild: clean $(DESTDIR_TARGET)
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

.pb.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

clean: 
	rm -rf $(DESTDIR_TARGET) *.o
	rm -rf location/*.o	

