#include "server_socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int init_socket()
{
	char buf[SERVER_BUF_LEN];
	int s_sock, c_sock;
	struct sockaddr_in addr;
	int bytes_read;

	//Init server socket
	s_sock = socket(AF_INET, SOCK_STREAM, 0);

	//Init socket address
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_IP, &addr.sin_addr);

	//Bind socket to address
	bind(s_sock, (struct sockaddr *) &addr, sizeof(addr));

	//Open socket for listening
	listen(s_sock, 10);

	//Accept connections
	c_sock = accept(s_sock, 0, 0);
	bytes_read = recv(c_sock, buf, 1024, 0);
	// printf("%s\n", strerror(errno));
	send(c_sock, buf, bytes_read, 0);

	close(s_sock);
	close(c_sock);

	return bytes_read;	
}