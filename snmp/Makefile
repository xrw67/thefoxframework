CC=g++

CFLAGS= -g
DFLAGS=-fPIC -shared
INCLUDE=./..

LIBRARY=libnetsnmp

all: testsnmp

testsnmp: test.cpp
	$(CC) $(CFLAGS) $(DFLAGS) -std=c++0x -o $@ $^ -I$(INCLUDE) -L$(LIBRARY)


