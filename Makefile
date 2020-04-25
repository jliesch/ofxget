CC_SRCS = $(wildcard *.cc) \
          $(wildcard clap/src/*.cc) 
CC_SRCS := $(filter-out ofxget_main.cc, $(CC_SRCS))
CC_SRCS := $(filter-out ofxhome_main.cc, $(CC_SRCS))
CC_SRCS := $(filter-out ofxhome_test.cc, $(CC_SRCS))

CPP_SRCS = $(wildcard pugixml/*.cpp)

OBJS = $(CC_SRCS:.cc=.o) $(CPP_SRCS:.cpp=.o)
INCLUDES=-I/usr/include -Iclap/include
CLFAGS = -Wall
LDFLAGS = -lcurl

%.o: %.cc
	g++ -std=c++11 -g -c -o $@ $< $(INCLUDES) $(CFLAGS)
%.o: %.cpp
	g++ -std=c++11 -g -c -o $@ $< $(INCLUDES) $(CFLAGS)

all: ofxget ofxhome ofxhome_test

ofxget: $(OBJS) ofxget_main.o
	g++ -std=c++11 -o $@ $^ $(LDFLAGS)

ofxhome: $(OBJS) ofxhome_main.o
	g++ -std=c++11 -o $@ $^ $(LDFLAGS)

ofxhome_test: $(OBJS) ofxhome_test.o
	g++ -std=c++11 -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS)
	rm -f ofxget ofxhome ofxget_main.o ofxhome_main.o
