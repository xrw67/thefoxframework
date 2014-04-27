CC=g++

CFLAGS= -g
#DFLAGS=-fPIC -shared
INCLUDE=./.. -I/usr/local/include/

LIBRARY=netsnmp

all: snmp

snmp:	test.cpp
	$(CC) $(CFLAGS) $(DFLAGS) -std=c++0x -o $@ $^ -I$(INCLUDE) -l$(LIBRARY) 
	
msg:
	protoc --cpp_out=. ./NmsMsg.proto
	
clean:
	rm -rf *.o snmp