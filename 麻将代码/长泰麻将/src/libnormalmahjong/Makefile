CC = gcc
CXX = g++
CFLAGS = -g -O0 -Wall
CXXFLAGS = -g -O0 -frtti -fexceptions -Wall
INCPATH = #-I/usr/include/mysql
LD = gcc
LFLAGS = #-static
LIBS = -L. -lstdc++


#福州麻将添加宏定义 ,其余几个Makefile文件也需要添加同步修改.
#FZ_DEF = -DMJ_HAND_CARD_17
#CFLAGS += $(FZ_DEF)
#CXXFLAGS += $(FZ_DEF)
#end


STATIC_LIB = libnormalmahjong.a
OBJECTS_LIB = common_mahjong_logic.o game_logic.o game_logicfantype.o mahjong_logic2.o to_char.o mj_common3.o mj_common4.o game_logic17.o

#DESTDIR_TARGET = test
#OBJECTS = test.o $(OBJECTS_LIB)

#all: $(DESTDIR_TARGET)

#$(DESTDIR_TARGET): $(OBJECTS)
#	$(LD) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

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
