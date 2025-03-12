# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -g -pthread -lrt -fPIC

# Source Files
LIB_SRCS = proxy-mq.c
SERVER_SRCS = servidor-mq.c claves.c
CLIENT_SRCS = app-cliente.c

# Object Files
LIB_OBJS = $(LIB_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

# Executables
LIBRARY = libclaves.so
SERVER = servidor-mq
CLIENT = client

# Default Target
all: $(LIBRARY) $(SERVER) $(CLIENT)

# Compile Shared Library (libclaves.so)
$(LIBRARY): $(LIB_OBJS)
	$(CC) -shared -o $@ $^ $(CFLAGS)

# Compile Server
$(SERVER): $(SERVER_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

# Compile Client (Links to libclaves.so)
$(CLIENT): $(CLIENT_OBJS)
	$(CC) -o $@ $^ -L. -lclaves $(CFLAGS)

# Object Files Compilation
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean Everything
clean:
	rm -f $(LIBRARY) $(SERVER) $(CLIENT) *.o
