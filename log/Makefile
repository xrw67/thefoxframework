CC = g++
CFLAGS = -Wall -std=c++0x -fPIC -shared
INCLUDE = ..

logging.o: logging.cc
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $^

log_file.o: log_file.cc
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $^

log_syslog.o: log_syslog.cc
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $^

clean:
	rm -rf *.o *.a
