CC = gcc
CFLAGS = -Wall -Wextra -pthread -lrt

all: servidor-mq app-cliente

servidor-mq: servidor-mq.c claves.c
	$(CC) $(CFLAGS) servidor-mq.c claves.c -o servidor-mq

app-cliente: app-cliente.c proxy-mq.c
	$(CC) $(CFLAGS) app-cliente.c proxy-mq.c -o app-cliente

clean:
	rm -f servidor-mq app-cliente