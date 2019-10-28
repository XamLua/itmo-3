CC=gcc
CFLAGS= -Wall -g -Wextra -pedantic -Werror
SOURCES=ar_c.c
EXECUTABLE=ar_c

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES:.c=.o)
	$(CC) -o $@ $^

clean:
	rm -rf $(EXECUTABLE) *.o