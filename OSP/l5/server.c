#include "server.h"
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
#include <sys/loadavg.h>
#include <string.h>

extern int errno;

static int done = 0;

int main(int argc, char const *argv[])
{
	signal(SIGINT, handle_sigint);
	if (argc < 2)
		use_shared_memory();
	else
	{
		int mode = getopt(argc, argv, "sqm");

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

	//Create shared memory segment
	if ( (shmid = shmget(SHM_ID, sizeof(struct s_data), PERMISSIONS | IPC_CREAT)) < 0)
		crit_err(errno);

	//Create semaphore for shared memory
	if ( (semid = semget(SEM_ID, 1, PERMISSIONS | IPC_CREAT)) < 0)
		crit_err(errno);

	//Attach shared memory to data
	if ( (data = (struct s_data*) shmat(shmid, 0, 0)) == NULL)
		crit_err(errno);

	//Init semaphore value (blocked)
	semctl(semid, 0, SETVAL, 1);
	
	data->pid = getpid();
	data->uid = getuid();
	data->gid = getgid();
	data->t_start = time(NULL);
	data->t_work = 0;
	getloadavg(data->load, 3);

	printf("Server start is successful\n");

	//Unlock semaphore
	semctl(semid, 0, SETVAL, 0);
	
	//Main cycle
	while(!done)
	{
		//Wait a second
		sleep(1);
		//Check if blocked
		if (semctl(semid, 0, GETVAL, 0))
			continue;

		//Block resource
		semctl(semid, 0, SETVAL, 1);
		
		//Update values
		data->t_work = time(NULL) - data->t_start;
		getloadavg(data->load, 3);

		//printf("Updated values\n");

		//Unblock resource
		semctl(semid, 0, SETVAL, 0);
	}


	//Free resources
	if (shmdt((void*)data) < 0)
		crit_err(errno);

	if (shmctl(shmid, IPC_RMID, 0) < 0)
		crit_err(errno);

	if (semctl(semid, 0, IPC_RMID, 0) < 0)		
		crit_err(errno);

	return 0;
}

int use_message_queue()
{
	int mqid;

	struct mq_data *m_data = malloc(sizeof(struct mq_data));

	//Create message queue
	if ( (mqid = msgget(MQ_ID, PERMISSIONS | IPC_CREAT)) < 0)
		crit_err(errno);

	//Init the message
	m_data->msg_type = MSG_TYPE_SERVER_INFO;
	m_data->msg_data.pid = getpid();
	m_data->msg_data.uid = getuid();
	m_data->msg_data.gid = getgid();
	m_data->msg_data.t_start = time(NULL);
	m_data->msg_data.t_work = 0;
	getloadavg(m_data->msg_data.load, 3);

	//Send message
	msgsnd(mqid, m_data, sizeof(struct mq_data), 0);

	while(!done)
	{
		sleep(1);

		//Delete last message if exist
		msgrcv(mqid, (void*) m_data, sizeof(struct mq_data), 0, IPC_NOWAIT);

		//Update message info
		m_data->msg_data.t_work = time(NULL) - m_data->msg_data.t_start;
		getloadavg(m_data->msg_data.load, 3);

		//Send new message to queue
		msgsnd(mqid, (void*) m_data, sizeof(struct mq_data), 0);

	}

	//Free resources
	if(msgctl(mqid, IPC_RMID, 0) < 0)
		crit_err(errno);

	free(m_data);

	return 1;
}

int use_posix_smo()
{
	sem_t *sem;
	//Create posix shared object
	int fd = shm_open(SMO, O_RDWR | O_CREAT, S_IRWXU | S_IROTH);

	if (fd < 1)
		crit_err(errno);

	//Set size of pso
	if (ftruncate(fd, sizeof(struct s_data)) < 0)
		crit_err(errno);

	//Use mmap to map pso to memory
	struct s_data *data = (struct s_data*) mmap(NULL, sizeof(struct s_data), PROT_READ | PROT_WRITE,
								MAP_SHARED, fd, 0);

	if (data == MAP_FAILED)
		crit_err(errno);

	//Create posix semaphore to sync things up
	if ( (sem = sem_open(SMO_SEM, O_CREAT, PERMISSIONS, 0)) == SEM_FAILED)
		crit_err(errno);

	//Init mmap'ed memory
	data->pid = getpid();
	data->uid = getuid();
	data->gid = getgid();
	data->t_start = time(NULL);
	data->t_work = 0;
	getloadavg(data->load, 3);

	//Unlock semaphore
	if (sem_post(sem) < 0)
		crit_err(errno);

	while(!done)
	{
		sleep(1);
		//Block until no reads from clients
		if(sem_wait(sem) < 0)
			crit_err(errno);

		//Update info
		data->t_work = time(NULL) - data->t_start;
		getloadavg(data->load, 3);

		//Unblock semaphore
		sem_post(sem);

	}

	//Free resources
	if(munmap((void*)data, sizeof(struct s_data)) < 0)
		crit_err(errno);

	if(close(fd) < 0)
		crit_err(errno);

	if(shm_unlink(SMO) < 0)
		crit_err(errno);

	if(sem_unlink(SMO_SEM) < 0)
		crit_err(errno);

	return 1;
}

void crit_err(int errnum)
{
	printf("Critical error: %s", strerror(errnum));
	exit(EXIT_FAILURE);
}

void handle_sigint(int sig)
{
	done = 1;
	printf("Server interrupted with '%s' signal\n", strsignal(sig));
}
