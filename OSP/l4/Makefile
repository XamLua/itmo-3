CC=gcc
CFLAGS=-std=c99 -Wall -Wextra --std=gnu99 -pedantic -Werror
HEAD_SOURCES=cp_c.c
HEAD_EXECUTABLE=head_c
XARGS_SOURCES=xargs_c.c
XARGS_EXECUTABLE=xargs_c

all: head xargs 

head: $(HEAD_EXECUTABLE)

xargs: $(XARGS_EXECUTABLE)

$(HEAD_EXECUTABLE): $(HEAD_SOURCES:.c=.o)
	$(CC) -o $@ $^

$(XARGS_EXECUTABLE): $(XARGS_SOURCES:.c=.o)
	$(CC) -o $@ $^

clean:
	rm -rf $(XARGS_EXECUTABLE) $(HEAD_EXECUTABLE)