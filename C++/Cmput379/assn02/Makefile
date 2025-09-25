# Makefile for distpsnotify and remoteagent C++ versions
CXX = g++
CXXFLAGS = -Wall -Wextra -g -std=c++11
LDFLAGS = 

all: distpsnotify remoteagent

distpsnotify: distpsnotify.cc
	$(CXX) $(CXXFLAGS) -o distpsnotify distpsnotify.cc $(LDFLAGS)

remoteagent: remoteagent.cc
	$(CXX) $(CXXFLAGS) -o remoteagent remoteagent.cc $(LDFLAGS)

clean:
	rm -f distpsnotify remoteagent *.o

.PHONY: all clean