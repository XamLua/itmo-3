CC=gcc
CFLAGS=-std=c99 -Wall -Wextra --std=gnu99 -pedantic -Werror
LDFLASG=-lrt -pthread

SERVER_SOURCES=server.c
CLIENT_SOURCES=client.c
ALPH_SOURCES=alphabet.c

SERVER_EXEC=server
CLIENT_EXEC=client
ALPH_EXEC=alphabet

all: $(SERVER_EXEC) $(CLIENT_EXEC) $(ALPH_EXEC)
 
$(ALPH_EXEC): $(ALPH_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

$(SERVER_EXEC): $(SERVER_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

$(CLIENT_EXEC): $(CLIENT_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

clean:
	rm -rf *.o $(SERVER_EXEC) $(CLIENT_EXEC) $(ALPH_EXEC)