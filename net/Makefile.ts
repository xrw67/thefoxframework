CC = g++
CFLAGS = -std=c++0x -g
INCLUDE = -I..
LIBRARY = -lpthread

all:server_test


server_test: acceptor.cc epoll_event.cc event_loop.cc socket.cc tcp_connection.cc tcp_server.cc tcp_server_test.cc ../log/logging.cc ../log/log_file.cc
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)

clean:
	rm -rf *.o *.so server_test

