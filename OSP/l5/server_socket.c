#include "server_socket.h"
#include "info.h"       
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/loadavg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int errno;

static int done = 0;

int main()
{
	signal(SIGINT, handle_sigint);
	//Server data
	struct s_data data;
	data.pid = getpid();
	data.uid = getuid();
	data.gid = getgid();
	data.t_start = time(NULL);
	data.t_work = 0;
	getloadavg(data.load, 3);

	//Create socket
	int sid = socket(AF_UNIX, SOCK_STREAM, 0);

	if(sid < 0)
		crit_err(errno);

	//Create and init sockaddr struct
	struct sockaddr_un addr;

	strlcpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path));

	addr.sun_family = AF_UNIX;

	//Bind socket to file
	if(bind(sid, (struct sockaddr*) &addr, strlen(addr.sun_path) + sizeof(addr.sun_family)) < 0)
		crit_err(errno);

	//Listen to connections
	if(listen(sid, MAX_QUEUE) < 0)
		crit_err(errno);

	//Create sockaddr for client socket
	struct sockaddr_un client_addr;

	while(!done)
	{
		//Accept connection
		socklen_t len = sizeof(client_addr);
		int client_sid = accept(sid, (struct sockaddr*) &client_addr, &len);

		if(client_sid < 0)
			break;

		//Send data
		data.t_work = time(NULL) - data.t_start;
		getloadavg(data.load, 3);
		if (send(client_sid, (void *) &data, sizeof(data), 0) != sizeof(data))
			printf("Error transfering data\n");

		//Close connection
		if(shutdown(client_sid, SHUT_RDWR) < 0)
			crit_err(errno);

	}

	if(unlink(SOCKET_PATH) < 0)
		crit_err(errno);
	return 0;
}

void crit_err(int errnum)
{
	printf("Critical error: %s", strerror(errnum));
	unlink(SOCKET_PATH);
	exit(EXIT_FAILURE);
}

void handle_sigint(int sig)
{
	done = 1;
	printf("Server interrupted with '%s' signal\n", strsignal(sig));
}
