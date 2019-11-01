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
#include <pthread.h>


static int done = 0;

static char* alphabet;

int main(int argc, char const *argv[])
{
	signal(SIGINT, handle_sigint);

	//Init the alphabet
	alphabet = malloc(sizeof(char) * BUF_SIZE);
	for (int i = 0; i < BUF_SIZE; ++i)
		alphabet[i] = 97 + i;

	if (argc < 2)
		use_unnamed_semaphore();
	else
	{
		int mode = (int) *argv[1];

		switch(mode)
		{
			case 'u': 
				use_unnamed_semaphore();
				break;

			case 'n':
				break;

			case 'm':
				break;

			default:
				printf("Mode not supported\n");
				return -1;
		}
	}
	return 0;
}

void use_unnamed_semaphore()
{
	pthread_t invert_case_thread, invert_order_thread;

	//Allocate memory for semaphores
	sem_t *sem_c = malloc(2*sizeof(sem_t));
	sem_t *sem_o = malloc(2*sizeof(sem_t));

	//Initialize semaphores
	if(sem_init(&sem_c[0], 0, 0) < 0)
		crit_err(errno);

	if(sem_init(&sem_o[0], 0, 0) < 0)
		crit_err(errno);

	if(sem_init(&sem_c[1], 0, 0) < 0)
		crit_err(errno);

	if(sem_init(&sem_o[1], 0, 0) < 0)
		crit_err(errno);

	//Create threads
	if (pthread_create(&invert_order_thread, NULL, invert_case_unnamed, sem_c) < 0)
		crit_err(errno);

	if (pthread_create(&invert_case_thread, NULL, invert_order_unnamed, sem_o) < 0)
		crit_err(errno);

	//Main cycle
	while(!done)
	{
		sleep(1);

		//Wake up the 1'st thread
		sem_post(&sem_c[0]);

		//Wait until it's finished
		sem_wait(&sem_c[1]);

		//Print alphabet
		printf("%.26s\n", alphabet);

		sleep(1);

		//Wake up the 2'nd thread
		sem_post(&sem_o[0]);

		//Wait until it's finished
		sem_wait(&sem_o[1]);

		//Print alphabet
		printf("%.26s\n", alphabet);
	}

	//Free resources
	sem_destroy(&sem_c[0]);
	sem_destroy(&sem_o[0]);
	sem_destroy(&sem_c[1]);
	sem_destroy(&sem_o[1]);
	free(sem_c);
	free(sem_o);

	return;
}

void *invert_case_unnamed(void *sem)
{
	sem_t *sem_c = (sem_t *) sem;

	while(!done)
	{
		//Wait for semaphore
		if(sem_wait(&sem_c[0]) < 0)
			crit_err(errno);

		invert_case(alphabet);

		//Unlock semaphore for main thread
		if(sem_post(&sem_c[1]) < 0)
			crit_err(errno);
	}

	return NULL;
}

void *invert_order_unnamed(void *sem)
{
	sem_t *sem_o = (sem_t *) sem;

	while(!done)
	{
		//Wait for semaphore
		if(sem_wait(&sem_o[0]) < 0)
			crit_err(errno);

		invert_order(alphabet);

		//Unlock semaphore for main thread
		if(sem_post(&sem_o[1]) < 0)
			crit_err(errno);
	}

	return NULL;
}

void invert_case(char* alphabet)
{
	for (int i = 0; i < BUF_SIZE; ++i)
	{
		if (alphabet[i] < 96)
			alphabet[i] += 32;
		else
			alphabet[i] -= 32; 
	}
	return;
}

void invert_order(char* alphabet)
{
	char temp;

	for (int i = 0; i < BUF_SIZE/2; ++i)
	{
		temp = alphabet[i];
		alphabet[i] = alphabet[BUF_SIZE-1 - i];
		alphabet[BUF_SIZE-1 - i] = temp;	
	}

	return;
}

void crit_err(int errnum)
{
	printf("Critical error: %s", strerror(errnum));
	exit(EXIT_FAILURE);
}

void handle_sigint(int sig)
{
	done = 1;
	printf("Program interrupted with '%s' signal\n", sys_siglist[sig]);
}