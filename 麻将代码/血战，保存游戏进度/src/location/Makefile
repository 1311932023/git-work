CC = /usr/bin/gcc
CXX = /usr/bin/g++
CFLAGS = -ggdb -O0 -Wall -fstack-protector-all
CXXFLAGS = -ggdb -O0 -frtti -fexceptions -Wall -fstack-protector-all
INCPATH = -I../libnormalmahjong -I../proto -I../include  -I/home/yangdian/boost/boost_1_67_0
LD = /usr/bin/gcc
LFLAGS = 
LIBS = -L. -lstdc++ \
		#-L/home/yangdian/boost/boost_1_67_0/stage/lib -lboost_serialization


#福州麻将添加宏定义 ,其余几个Makefile文件也需要添加同步修改.
#FZ_DEF = -DMJ_HAND_CARD_17
#CFLAGS += $(FZ_DEF)
#CXXFLAGS += $(FZ_DEF)
#end


STATIC_LIB = liblocation.a
OBJECTS_LIB =  table_changsha_zhuanzhuan.o table_zz_laizi.o table_hong_zhong.o table_xue_zhan.o table_chengdumj.o

all: lib

$(DESTDIR_TARGET): $(OBJECTS)
	$(LD) $(LFLAGS) -o $(DESTDIR_TARGET) $(OBJECTS) $(LIBS)

lib: $(STATIC_LIB)

clean:
	rm -rf $(DESTDIR_TARGET) *.o
	
$(STATIC_LIB): $(OBJECTS_LIB)
	ar -r $(STATIC_LIB) $(OBJECTS_LIB)
	mv $(STATIC_LIB) ../lib
	
rebuild: clean lib
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

