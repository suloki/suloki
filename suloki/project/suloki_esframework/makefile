#source file
SOURCE  := $(wildcard ../../../depend/duktape/src/*.c) $(wildcard *.c) $(wildcard ../../library/*.cpp) $(wildcard *.cpp)
#SOURCE  := $(wildcard *.c) $(wildcard *.cpp)
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
#target you can change test to what you want
TARGET  := suloki_esframework
#compile and lib parameter
#CROSS_COMPILE := arm-none-linux-gnueabi-
CC      := gcc
#CC    := $(CROSS_COMPILE)$(CC)
CXX    := g++
#CXX  := $(CROSS_COMPILE)$(CXX)
#LIBS    := -L../../depend/boost/build/lib -lboost_system -lboost_thread -lpthread
#LIBS    := -lpthread -L../../depend/boost/build/lib -lboost_system -lboost_thread -L../ -lsuloki
#LIBS    := -L../../../depend/boost/zbuildres/lib -lboost_system -lboost_thread -lboost_filesystem -L../ -lsuloki -L../../../depend/leveldb/zbuildres -lleveldb -lpthread
LIBS    := -L../../../depend/boost/zbuildres/lib -lboost_system -lboost_thread -lboost_filesystem -L../../../depend/leveldb/zbuildres -lleveldb -lpthread -ldl
LDFLAGS:= 
DEFINES:= -DSULOKI_64_PLATFORM_SULOKIDEF
INCLUDE:= -I. -I../../../depend/loki/ -I../../../depend/boost/ -I../../../depend/leveldb/include/ -I../../../depend/duktape/src/ -I../../../depend/duktape/extras/module-duktape/ -I../../library/ 
#CFLAGS  := -g -Wall -O3 $(DEFINES) $(INCLUDE)
CFLAGS  := -fpermissive -Wall -O3 $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H

#i think you should do anything here
.PHONY : everything objs clean distclean rebuild
everything : $(TARGET)
all : $(TARGET)
objs : $(OBJS)
rebuild: distclean everything

clean :
#	rm -fr ../*.o
	rm -fr *.o
distclean : clean
	rm -fr $(TARGET)
$(TARGET) : $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
