CC = gcc
CFLAGS = -Wall -Wextra -g -pthread -lrt -fPIC

# Targets
all: servidor-mq client

servidor-mq: servidor-mq.c claves.c
	$(CC) -o servidor-mq servidor-mq.c claves.c $(CFLAGS)

client: client.c
	$(CC) -o client client.c

clean:
	rm -f servidor-mq client
