CC=gcc
CFLAGS=-std=c99 -Wall -Wextra --std=gnu99 -pedantic -Werror
SOURCES=cp_c.c
EXECUTABLE=cp_c

all: $(EXECUTABLE)

$(EXECUTABLE): 
	$(CC) $(CFLAGS) $(SOURCES) -o $@

clean:
	rm -rf $(EXECUTABLE)