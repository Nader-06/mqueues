CC = gcc
CFLAGS = -Wall -Wextra -pthread -lrt
LDFLAGS = -lrt -pthread

# Files
SRCS = claves.c servidor-mq.c proxy-mq.c app-cliente.c
OBJS = claves.o servidor-mq.o proxy-mq.o app-cliente.o
LIBRARY = libclaves.so

# Build server executable
servidor-mq: servidor-mq.o claves.o
	$(CC) -o servidor-mq servidor-mq.o claves.o $(LDFLAGS)

# Build proxy shared library (Ensure Position Independent Code)
$(LIBRARY): proxy-mq.o
	$(CC) -shared -o $(LIBRARY) proxy-mq.o $(LDFLAGS) -fPIC

# Build client executable
app-cliente: app-cliente.o $(LIBRARY)
	$(CC) -o app-cliente app-cliente.o -L. -lclaves $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Clean up compiled files
clean:
	rm -f *.o servidor-mq app-cliente $(LIBRARY)

.PHONY: clean