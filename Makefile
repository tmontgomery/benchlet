
SRCS=   
OBJS=   bench1.o bench2.o benchlet-main.o
CC=     gcc
CXX=    g++
LINK=   gcc
PLAT=   $(shell uname -s)
CFLAGS= -Wall
CXXFLAGS= -Wall
LD_LIBS=

ifeq ($(PLAT),Darwin)
        CFLAGS += -DDarwin
        CXXFLAGS += -DDarwin
else ifeq ($(PLAT),Linux)
        CFLAGS += -D_GNU_SOURCE
        CXXFLAGS += -D_GNU_SOURCE
endif

CXXFLAGS += -I..
CFLAGS += -I..
LD_LIBS += -lstdc++

all:	benchlet-runner

benchlet-runner:	$(OBJS)
					$(LINK) $(OBJS) -o benchlet-runner $(LD_LIBS)

clean:
		$(RM) -rf $(OBJS)