CC=gcc                              
CFLAGS=-std=c99 -Wall -Wextra
sps: sps.c
	$(CC) $(CFLAGS) sps.c -o sps
array: array.c
	$(CC) $(CFLAGS) array.c -o array