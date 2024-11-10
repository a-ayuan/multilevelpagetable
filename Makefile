CC = gcc
CFLAGS = -Wall -g -O2
LDFLAGS = -L.

all: libmlpt.a

libmlpt.a: mlpt.o
	ar rcs libmlpt.a mlpt.o

mlpt.o: mlpt.c mlpt.h config.h
	$(CC) $(CFLAGS) -c mlpt.c

clean:
	rm -f *.o libmlpt.a
