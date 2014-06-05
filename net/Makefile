CC = g++
CFLAGS = -std=c++0x -g -fPIC -shared
INCLUDE = -I..
LIBRARY = -lpthread

all:libthefox.so


libthefox.so: acceptor.o epoll_event.o event_loop.o socket.o tcp_client.o tcp_connection.o tcp_server.o
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)

acceptor.o: acceptor.cc
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)

epoll_event.o: epoll_event.cc
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)
event_loop.o: event_loop.cc
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)
socket.o: socket.cc
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)
tcp_connection.o: tcp_connection.cc
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)
tcp_client.o: tcp_client.cc
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)
tcp_server.o: tcp_server.cc
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LIBRARY)

clean:
	rm -rf *.o *.so

