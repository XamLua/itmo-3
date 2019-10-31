#include "alphabet.h"
#include <stdarg.h>
#include "info.h"
#include <stdio.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h> 
#include <sys/mman.h> 
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int done = 0;

int main(int argc, char const *argv[])
{
	
	return 0;
}

void crit_err(int errnum)
{
	printf("Critical error: %s", strerror(errnum));
	exit(EXIT_FAILURE);
}

void handle_sigint(int sig)
{
	done = 1;
	printf("Programm interrupted with '%s' signal\n", sys_siglist[sig]);
}