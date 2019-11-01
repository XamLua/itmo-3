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

static int done = 0;

static char* alphabet;

int main(int argc, char * const *argv)
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
		int mode = getopt(argc, argv, "usm");

		switch(mode)
		{
			case 'u': 
				use_unnamed_semaphore();
				break;

			case 's':
				use_s5_semaphore();
				break;

			case 'm':
			{
				if (argc != 5)
					printf("Wrong amount of arguments: %d. Expected - 3\n", argc - 2);

				char* end;
				unsigned long time_p = strtoul(argv[2], &end, 10);
				unsigned long time_c = strtoul(argv[3], &end, 10);
				unsigned long time_o = strtoul(argv[4], &end, 10);

				if (time_p == 0 || time_c == 0 || time_o == 0)
				{
					printf("Error: arg is not a acceptable number\n");
					break;
				}

				struct intervals irvs = {.time_p = time_p, .time_c = time_c, .time_o = time_o};
				use_mutex(&irvs);
				break;
			}
			case 'l':
			{
				//O_O
			}

			default:
				printf("Mode not supported\n");
		}
	}
	free(alphabet);
	return 0;
}

void use_mutex(struct intervals *irvs)
{
	pthread_t invert_case_thread, invert_order_thread;

	//Allocate memory for mutex and other data
	pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
	struct m_data *data_c = malloc(sizeof(struct m_data));
	struct m_data *data_o = malloc(sizeof(struct m_data));

	//Initialize mutex
	int check;
	check = pthread_mutex_init(mutex, NULL);

	//Initialize data
	data_c->lock = (void*) mutex;
	data_o->lock = (void*) mutex;
	data_c->interval = irvs->time_c;
	data_o->interval = irvs->time_o;

	if (check < 0)
		crit_err(check);

	//Lock mutex
	pthread_mutex_lock(mutex);

	//Create threads
	if (pthread_create(&invert_case_thread, NULL, invert_case_mutex, (void *) data_c) < 0)
		crit_err(errno);

	if (pthread_create(&invert_order_thread, NULL, invert_order_mutex, (void *) data_o) < 0)
		crit_err(errno);

	//Unlock mutex
	pthread_mutex_unlock(mutex);

	//Main cycle
	while (!done)
	{
		//Lock mutex
		pthread_mutex_lock(mutex);

		//Print alphabet
		printf("%.26s\n", alphabet);
		
		//Unlock mutex
		pthread_mutex_unlock(mutex);

		usleep(irvs->time_p);
	}

	//Finish all child processes
	pthread_join(invert_case_thread, NULL);
	pthread_join(invert_order_thread, NULL);

	//Free resources
	free(mutex);
	free(data_c);
	free(data_o);
}

void *invert_case_mutex(void *s)
{
	struct m_data *data = (struct m_data*) s;

	while (!done)
	{
		//Lock
		pthread_mutex_lock((pthread_mutex_t*) data->lock);

		invert_case(alphabet);

		//Unlock
		pthread_mutex_unlock((pthread_mutex_t*) data->lock);

		usleep(data->interval);
	}

	return NULL;
}

void *invert_order_mutex(void *s)
{
	struct m_data *data = (struct m_data*) s;

	while (!done)
	{
		//Lock
		pthread_mutex_lock((pthread_mutex_t*) data->lock);

		invert_order(alphabet);

		//Unlock
		pthread_mutex_unlock((pthread_mutex_t*) data->lock);

		usleep(data->interval);
	}

	return NULL;
}

void use_s5_semaphore()
{
	pthread_t invert_case_thread, invert_order_thread;

	int semid_c, semid_o;

	//Get semaphore set for each thread
	if ((semid_c = semget(SEM_ID_C, 2, PERMISSIONS | IPC_CREAT)) < 0)
		crit_err(errno);
	if ((semid_o = semget(SEM_ID_O, 2, PERMISSIONS | IPC_CREAT)) < 0)
		crit_err(errno);

	//Init semaphores values (blocked)
	semctl(semid_c, 0, SETALL, 0);
	semctl(semid_o, 0, SETALL, 0);

	//Prepare buffer structs for operaions
	struct sembuf sembuf_c = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
	struct sembuf sembuf_o = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};

	//Create threads
	if (pthread_create(&invert_case_thread, NULL, invert_case_s5, NULL) < 0)
		crit_err(errno);

	if (pthread_create(&invert_order_thread, NULL, invert_order_s5, NULL) < 0)
		crit_err(errno);

	//Main cycle
	while(!done)
	{
		sleep(1);

		//Wake up the 1'st thread
		sembuf_c.sem_num = 0;		
		sembuf_c.sem_op = 1;

		if(semop(semid_c, &sembuf_c, 1) < 0)
			crit_err(errno);

		//Wait until it's finished
		sembuf_c.sem_num = 1;
		sembuf_c.sem_op = -1;

		if(semop(semid_c, &sembuf_c, 1) < 0)
			crit_err(errno);		

		//Print alphabet
		printf("%.26s\n", alphabet);

		sleep(1);

		//Wake up the 2'nd thread
		sembuf_o.sem_num = 0;		
		sembuf_o.sem_op = 1;

		if(semop(semid_o, &sembuf_o, 1) < 0)
			crit_err(errno);

		//Wait until it's finished
		sembuf_o.sem_num = 1;
		sembuf_o.sem_op = -1;
		if(semop(semid_o, &sembuf_o, 1) < 0)
			crit_err(errno);

		//Print alphabet
		printf("%.26s\n", alphabet);
	}

	//Finish child threads
	sembuf_c.sem_num = 0;		
	sembuf_c.sem_op = 1;

	if(semop(semid_c, &sembuf_c, 1) < 0)
		crit_err(errno);
	
	sembuf_o.sem_num = 0;		
	sembuf_o.sem_op = 1;

	if(semop(semid_o, &sembuf_o, 1) < 0)
		crit_err(errno);

	pthread_join(invert_case_thread, NULL);
	pthread_join(invert_order_thread, NULL);

	//Free resources
	if (semctl(semid_c, 0, IPC_RMID, 0) < 0)		
		crit_err(errno);
	if (semctl(semid_o, 0, IPC_RMID, 0) < 0)		
		crit_err(errno);
}

void *invert_case_s5()
{
	int semid_c;
	//Get semaphore set
	if ( (semid_c = semget(SEM_ID_C, 2, PERMISSIONS)) < 0)
		crit_err(errno);

	//Prepare buffer for operations
	struct sembuf sembuf_c = {.sem_num = 0, .sem_op = 0, .sem_flg = 0};

	//Main cycle
	while(!done)
	{
		//Wait for semaphore
		sembuf_c.sem_num = 0;
		sembuf_c.sem_op = -1;

		if(semop(semid_c, &sembuf_c, 1) < 0)
			crit_err(errno);

		invert_case(alphabet);

		//Wake up the main thread
		sembuf_c.sem_num = 1;
		sembuf_c.sem_op = 1;
		
		if(semop(semid_c, &sembuf_c, 1) < 0)
			crit_err(errno);
	}

	return NULL;
}

void *invert_order_s5()
{
	int semid_o;
	//Get semaphore set
	if ( (semid_o = semget(SEM_ID_O, 2, PERMISSIONS)) < 0)
		crit_err(errno);

	//Prepare buffer for operations
	struct sembuf sembuf_o = {.sem_num = 0, .sem_op = 0, .sem_flg = 0};

	//Main cycle
	while(!done)
	{
		//Wait for semaphore
		sembuf_o.sem_num = 0;
		sembuf_o.sem_op = -1;

		if(semop(semid_o, &sembuf_o, 1) < 0)
			crit_err(errno);

		invert_order(alphabet);

		//Wake up the main thread
		sembuf_o.sem_num = 1;
		sembuf_o.sem_op = 1;
		if(semop(semid_o, &sembuf_o, 1) < 0)
			crit_err(errno);
	}

	return NULL;
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
		if(sem_post(&sem_c[0]) < 0)
		{
			printf("suka\n");
			crit_err(errno);
		}

		//Wait until it's finished
		if(sem_wait(&sem_c[1]) < 0)
			crit_err(errno);

		//Print alphabet
		printf("%.26s\n", alphabet);

		sleep(1);

		//Wake up the 2'nd thread
		if(sem_post(&sem_o[0]) < 0)
			crit_err(errno);

		//Wait until it's finished
		if(sem_wait(&sem_o[1]) < 0)
			crit_err(errno);

		//Print alphabet
		printf("%.26s\n", alphabet);
	}

	//Finish child threads
	if(sem_post(&sem_c[0]) < 0)
		crit_err(errno);

	if(sem_post(&sem_o[0]) < 0)
		crit_err(errno);

	pthread_join(invert_case_thread, NULL);
	pthread_join(invert_order_thread, NULL);

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