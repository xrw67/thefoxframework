all:libthefox_log.a

CC = g++
CFLAGS = -Wall -std=c++0x -fPIC -shared
INCLUDE = ..

libthefox_log.a: logging.o log_file.o log_syslog.o
	ar cr libthefox_log.a $@

logging.o: logging.cc logging.h
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $<

log_file.o: log_file.cc log_file.h
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $<

log_syslog.o: log_syslog.cc log_syslog.h
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $<

clean:
	rm *.o *.a
