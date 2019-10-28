CC=gcc
CFLAGS= -Wall -g -Wextra -pedantic -Werror
SOURCES=ar_c.c
EXECUTABLE=ar_c

all: $(EXECUTABLE)

$(EXECUTABLE): 
	$(CC) $(CFLAGS) $(SOURCES) -o $@

clean:
	rm -rf $(EXECUTABLE)