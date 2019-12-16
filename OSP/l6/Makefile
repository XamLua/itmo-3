CC=gcc
CFLAGS=-std=c99 -Wall -Wextra --std=gnu99 -pedantic -Werror
LDFLASG=-lrt -pthread -lncurses

SERVER_SOURCES=server.c client_menu.c cli_menu.c thread_menu.c server_socket.c
CLIENT_SOURCES=client_socket.c

SERVER_EXEC=server
CLIENT_EXEC=client

all: $(SERVER_EXEC) $(CLIENT_EXEC)
 
$(SERVER_EXEC): $(SERVER_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

$(CLIENT_EXEC): $(CLIENT_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

clean:
	rm -rf *.o $(SERVER_EXEC) $(CLIENT_EXEC)