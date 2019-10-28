CC=gcc
CFLAGS= -Wall -g -Wextra -pedantic -Werror
SOURCES=hem_c.c
EXECUTABLE=hem_c

all: $(EXECUTABLE)

$(EXECUTABLE): 
	$(CC) $(CFLAGS) $(SOURCES) -o $@ -lm

clean:
	rm -rf $(EXECUTABLE)