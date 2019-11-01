#include "client_socket.h"
#include "info.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

int main()
{
	signal(SIGINT, handle_sigint);
	void* mes_buf = malloc(sizeof(struct s_data));

	//Create socket
	int sid = socket(AF_UNIX, SOCK_STREAM, 0);

	if(sid < 0)
		crit_err(errno);

	//Create and init sockaddr struct
	struct sockaddr_un server_addr;

	strlcpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path));

	server_addr.sun_family = AF_UNIX;

	//Connect to server socket
	if(connect(sid, (struct sockaddr*) &server_addr, 
		    strlen(server_addr.sun_path) + sizeof(server_addr.sun_family)) < 0)
		crit_err(errno);


	//Read data
	memset(mes_buf, 0, BUF_SIZE);

	if(recv(sid, mes_buf, sizeof(struct s_data), 0) != sizeof(struct s_data))
	{
		printf("Data transfer error\n");
		crit_err(errno);
	}
	else
	{
		struct s_data* data = (struct s_data*) mes_buf;
		//Print data
		printf("Server stats:\n");
	
		printf("Server process id: %lu\n", (unsigned long) data->pid);
		printf("Server user id: %lu\n", (unsigned long) data->uid);
		printf("Server group id: %lu\n", (unsigned long) data->gid);
		printf("Seconds from start: %ld\n", data->t_work);
		printf("Average Load for 1 minute: %f\n", data->load[0]);
		printf("Average Load for 5 minute: %f\n", data->load[1]);
		printf("Average Load for 15 minute: %f\n", data->load[2]);
	}
	//Free resources
	free(mes_buf);
	return 0;
}

void crit_err(int errnum)
{
	printf("Critical error: %s", strerror(errnum));
	exit(EXIT_FAILURE);
}

void handle_sigint(int sig)
{
	printf("Client interrupted with '%s' signal\n", strsignal(sig));
	exit(EXIT_FAILURE);
}
