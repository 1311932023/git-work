CC = /usr/bin/gcc
CXX = /usr/bin/g++
CFLAGS = -g -O0 -Wall -fstack-protector-all
CXXFLAGS = -g -O0 -frtti -fexceptions -Wall -fstack-protector-all
INCPATH = -I./libnormalmahjong -I./include -I./include/curl -I/usr/local/include -I./proto -I/home/yangdian/boost/boost_1_67_0
LD = /usr/bin/gcc
LFLAGS = #-static
LIBS = -lstdc++ -L./lib -L/home/yangdian/boost/boost_1_67_0/stage/lib -llocation -lnormalmahjong -lcommon -lcomproto -lprotobuf\
		-ljsoncpp -lev -lcurl -lcrypto -lhiredis -lboost_system -lboost_serialization -Wl,-rpath,/home/yangdian/boost/boost_1_67_0/stage/lib


#福州麻将添加宏定义 ,其余几个Makefile文件也需要添加同步修改.
#FZ_DEF = -DMJ_HAND_CARD_17
#CFLAGS += $(FZ_DEF)
#CXXFLAGS += $(FZ_DEF)
#end


DESTDIR_TARGET = xuezhanmatch
OBJECTS = game.o game2.o sd_match.o rr_waiwei_match.o rj_match.o rr_match.o match.o match2.o\
message_word.o match_ext.o redis_client.o table.o table2.o table3.o table4.o table5.o table_helper.o\
table_helper2.o player.o zjh.o table_disband.o table_gamestart.o table_mp.o\
trustee/CTrustee.o trustee/CXueZhanTrustee.o trustee/CardsCounts.o trustee/TrusteeFunc.o

all: $(DESTDIR_TARGET)

$(DESTDIR_TARGET): $(OBJECTS)	
	cd location && make
	$(CXX) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS) 

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
	rm -rf trustee/*.o
	rm -rf location/*.o
