.PHONY:default all 
BINFILE := CTPDame

LIBS := z pthread jsoncpp thostmduserapi thosttraderapi glog gflags

INCS := ./include ./utill

LIBPATHS := ./lib 

USER_MARCOS := 
test = 0
ifeq ($(test),1)
USER_MARCOS :=$(USER_MARCOS) _TEST_
endif
CFLAGS = -g -Wall #-pg -O2
CC = g++

COMMONSRCS = $(wildcard ./*.cpp ./utill/*.cpp ./utill/xml/*.cpp)

COMMONOBJS = $(patsubst %.cpp,%.o,$(COMMONSRCS))

all:$(BINFILE)

MAIN_VERSION_STRING = "1.0.1"
MINOR_VERSION_STRING:=$(shell LANG=C git log --oneline -1 2>/dev/null | cut -c1-7)
version.h:force
	@echo Generating version.h
	@echo -e "#ifndef __MYPROJECT_VERSION_H__\n"\
	"#define __MYPROJECT_VERSION_H__\n"\
	"#define MAIN_VERSION_STRING \"$(MAIN_VERSION_STRING)\"\n" \
	"#define MINOR_VERSION_STRING \"$(MINOR_VERSION_STRING)\"\n" \
	"#endif" >version.h	
force:

$(BINFILE):version.h $(COMMONOBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(addprefix -L,$(LIBPATHS)) $(addprefix -l,$(LIBS))

%.o:%.cpp
	$(CC) $(FLAGS) $(addprefix -D,$(USER_MARCOS)) $(addprefix -I,$(INCS)) -c $< -o $@

clean :
	rm -rf $(COMMONOBJS) $(BINFILE) version.h
