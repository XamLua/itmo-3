CC=gcc
CFLAGS=-std=c99 -Wall -Wextra --std=gnu99 -pedantic -Werror
LDFLASG=-lrt -pthreads /usr/lib/libsocket.so

SERVER_SOURCES=server.c
CLIENT_SOURCES=client.c
ALPH_SOURCES=alphabet.c
SERVER_SOCKET_SOURCES=server_socket.c
CLIENT_SOCKET_SOURCES=client_socket.c
SIGNAL_SOURCES=signal_handling.c
UNNAMED_CH_SOURCES=unnamed_ch.c

SERVER_EXEC=server
CLIENT_EXEC=client
SERVER_SOCKET_EXEC=server_socket
CLIENT_SOCKET_EXEC=client_socket
ALPH_EXEC=alphabet
SIGNAL_EXEC=signal_handling
UNNAMED_CH_EXEC=unnamed_ch

all: $(SERVER_EXEC) $(CLIENT_EXEC) $(ALPH_EXEC) $(SERVER_SOCKET_EXEC) $(CLIENT_SOCKET_EXEC) $(SIGNAL_EXEC) $(UNNAMED_CH_EXEC)

$(UNNAMED_CH_EXEC): $(UNNAMED_CH_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)	
 
$(SIGNAL_EXEC): $(SIGNAL_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

$(SERVER_SOCKET_EXEC): $(SERVER_SOCKET_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

$(CLIENT_SOCKET_EXEC): $(CLIENT_SOCKET_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

$(ALPH_EXEC): $(ALPH_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

$(SERVER_EXEC): $(SERVER_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

$(CLIENT_EXEC): $(CLIENT_SOURCES:.c=.o)
	$(CC) -o $@ $^ $(LDFLASG)

clean:
	rm -rf *.o $(UNNAMED_CH_EXEC) $(SERVER_EXEC) $(CLIENT_EXEC) $(ALPH_EXEC) $(SERVER_SOCKET_EXEC) $(CLIENT_SOCKET_EXEC) $(SIGNAL_EXEC)
	