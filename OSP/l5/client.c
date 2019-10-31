#include "client.h"
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

int inrpt = 0;

int main(int argc, char const *argv[])
{
	signal(SIGINT, handle_sigint);
	if (argc < 2)
		use_shared_memory();
	else
	{
		int mode = (int) *argv[1];

		switch(mode)
		{
			case 's': 
				use_shared_memory();
				break;

			case 'q':
				use_message_queue();
				break;

			case 'm':
				use_posix_smo();
				break;

			default:
				printf("Mode not supported\n");
				return -1;
		}
	}

	return 0;
}

int use_shared_memory()
{
	int shmid;
	int semid;

	struct s_data *data;

	//Get shared memory segment
	if ( (shmid = shmget(SHM_ID, sizeof(struct s_data), PERMISSIONS)) < 0)
		crit_err(errno);

	//Create semaphore for shared memory
	if ( (semid = semget(SEM_ID, 1, PERMISSIONS)) < 0)
		crit_err(errno);

	//Attach shared memory to data
	if ( (data = (struct s_data*) shmat(shmid, 0, 0)) == NULL)
		crit_err(errno);

	//Wait for lock
	while(!inrpt && semctl(semid, 0, GETVAL, 0))

	if (inrpt)
		return 0;

	//Block from writing
	semctl(semid, 0, GETVAL, 1);

	print_server_data(data);

	//Unblock from writing
	semctl(semid, 0, GETVAL, 1);

	//Free resources
	if (shmdt(data) < 0)
		crit_err(errno);

	return 1;
}

int use_message_queue()
{
	int mqid;

	struct mq_data *m_data = malloc(sizeof(struct mq_data));

	//struct s_data *data = malloc(sizeof(struct s_data));

	//Get message queue
	if ( (mqid = msgget(MQ_ID, PERMISSIONS)) < 0)
		crit_err(errno);

	//Recieve message
	msgrcv(mqid, m_data, sizeof(struct mq_data), MSG_TYPE_SERVER_INFO, 0);

	print_server_data(&(m_data->msg_data));

	return 1;
}

int use_posix_smo()
{
	sem_t *sem;
	//Get posix shared object
	int fd = shm_open(SMO, O_RDONLY, S_IRUSR);

	if (fd < 1)
		crit_err(errno);

	//Use mmap to map pso to memory
	struct s_data *data = mmap(NULL, sizeof(struct s_data), PROT_READ,
								MAP_SHARED, fd, 0);

	if (data == MAP_FAILED)
		crit_err(errno);

	//Create posix semaphore to sync things up
	if ( (sem = sem_open(SMO_SEM, O_CREAT, PERMISSIONS, 0)) == SEM_FAILED)
		crit_err(errno);

	//Wait for resource to be free
	if (sem_wait(sem) < 0)
		crit_err(errno);

	print_server_data(data);

	//Unblock semaphore
	if (sem_post(sem) < 0)
		crit_err(errno);

	//Free resources
	if(munmap(data, sizeof(struct s_data)) < 0)
		crit_err(errno);

	if(close(fd) < 0)
		crit_err(errno);

	if(sem_close(sem) < 0)
		crit_err(errno);

	return 1;
}

void handle_sigint(int sig)
{
	inrpt = 1;
	printf("Client interrupted with %s\n", sys_siglist[sig]);
}

void crit_err(int errnum)
{
	printf("Critical error: %s", strerror(errnum));
	exit(EXIT_FAILURE);
}

void print_server_data(struct s_data* data)
{
	printf("Server stats:\n");

	printf("Server process id: %d\n", data->pid);
	printf("Server user id: %d\n", data->uid);
	printf("Server group id: %d\n", data->gid);
	printf("Seconds from start: %ld\n", data->t_work);
	printf("Average Load for 1 minute: %f\n", data->load[0]);
	printf("Average Load for 5 minute: %f\n", data->load[1]);
	printf("Average Load for 15 minute: %f\n", data->load[2]);
}