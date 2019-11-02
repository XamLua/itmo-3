#ifndef __INFO_H__
#define __INFO_H__

#define SMO "/SMO"
#define SMO_SEM "/SMO_SEM"

#define MSG_BUFF_SIZE 1000
#define MSG_TYPE_SERVER_INFO 1

#define SHM_ID 313
#define SEM_ID 314
#define MQ_ID 313


#define PERMISSIONS 0666

#define BUF_SIZE 1000

#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

struct s_data
{
	pid_t pid;
	uid_t uid;
	gid_t gid;
	time_t t_start;
	time_t t_work;
	double load[3];
};

struct mq_data
{
	long msg_type;
	struct s_data msg_data;
};

void handle_sigint(int sig);

#endif
