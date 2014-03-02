CC=g++
INCLUDE=./..
LIBRARY=mysqlclient

testmysql: test.cpp
	$(CC) -std=c++0x -o $@ $^ -I$(INCLUDE) -l$(LIBRARY)
