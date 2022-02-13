#target you can change test to what you want

TARGET  := libsuloki.a

#compile and lib parameter

#CROSS_COMPILE := arm-none-linux-gnueabi-

CC      := gcc

#CC    := $(CROSS_COMPILE)$(CC)

CXX    := g++

#CXX  := $(CROSS_COMPILE)$(CXX)

AR      = ar

RANLIB  = ranlib

LIBS    := 

LDFLAGS:= 

DEFINES:= -DSULOKI_64_PLATFORM_SULOKIDEF

INCLUDE:= -I. -I../../depend/loki/ -I../../depend/boost/ -I../../depend/leveldb/include/ -I../../depend/duktape/src/ -I../../depend/duktape/extras/module-duktape/ 

LIBS    := -L../../depend/boost/zbuildres/lib -lboost_system -lboost_thread -lboost_filesystem -L../../depend/leveldb/zbuildres -lleveldb

#CFLAGS  := -g -fPIC -Wall -O3 $(DEFINES) $(INCLUDE)

CFLAGS  := -fPIC -fpermissive -Wall -O3 $(DEFINES) $(INCLUDE)

CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H

#i think you should do anything here

#source file

#SOURCE  := $(wildcard *.c) $(wildcard ../proto/*.cc) $(wildcard *.cpp)
SOURCE  := $(wildcard *.c) $(wildcard *.cpp)

OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

#OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(SOURCE))))



.PHONY : everything objs clean distclean rebuild

everything : $(TARGET)

all : $(TARGET)

objs : $(OBJS)

rebuild: distclean everything



clean :

	rm -fr *.o

	rm -fr $(TARGET)

distclean : clean

	rm -fr $(TARGET)

$(TARGET) : $(OBJS) 

	$(AR) cru $(TARGET) $(OBJS)

	$(RANLIB) $(TARGET)
