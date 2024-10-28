# Makefile

CC = gcc
CFLAGS = -Wall -g

all: signal timer

signal: signal.c
	$(CC) $(CFLAGS) -o signal signal.c

timer: timer.c
	$(CC) $(CFLAGS) -o timer timer.c

clean:
	rm -f signal timer