CC = gcc

CFLAGS = -O2 -Wall

.PHONY: all

all: qimg_merge

qimg_merge:
	$(CC) $(CFLAGS) -Iinclude -I/usr/include/libxml2 $@.c -o $@ -lxml2

clean:
	[ -f "qimg_merge" ] && rm -f "qimg_merge"