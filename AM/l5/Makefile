CC=gcc
CFLAGS=-std=c99 -g -Wall -Wextra --std=gnu99 -pedantic -Werror

SOURCES=crc_c.c

EXEC=crc_c

all: $(EXEC)

$(EXEC): $(SOURCES:.c=.o)
	$(CC) -o $@ $^

clean:
	rm -rf *.o $(EXEC)