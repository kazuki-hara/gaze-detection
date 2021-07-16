CC=gcc
CCFLAGS=-Wall -g
CXX=g++
CXXFLAGS=-std=c++11 -Wall -g -I../../thirdparty/boostlibs
LDFLAGS=-Wall -g -lGL -lGLU -lglut -ltbb -lpthread -lrt 
LDFLAGS2=`pkg-config --cflags opencv` `pkg-config --libs opencv`
LDFLAGS3=`pkg-config --cflags freetype2` `pkg-config --libs freetype2`

SRCS := main.cpp DistortionCorrection2.cpp Velocity.cpp OpenGL.cpp Gaze.cpp Filter.cpp Camcontrol.cpp
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

TARGET = main

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LDFLAGS2) $(LDFLAGS3)

clean:
	$(RM) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

