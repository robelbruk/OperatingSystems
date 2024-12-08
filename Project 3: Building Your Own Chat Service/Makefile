CC = gcc
CFLAGS = -Wall -pthread
TARGET = server

SRCS = server.c server_client.c list.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)
