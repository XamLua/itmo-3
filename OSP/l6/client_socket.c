#include "client_socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(/*int argc, char const *argv[]*/)
{
	
	char buf[CLIENT_BUF_LEN];
	int c_sock;
	struct sockaddr_in addr;
	int bytes_read;

	//Init socket
	c_sock = socket(AF_INET, SOCK_STREAM, 0);

	//Init socket address
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_IP, &addr.sin_addr);

	//Connect socket to address
	connect(c_sock, (struct sockaddr *) &addr, sizeof(addr));

	send(c_sock, "Hello there", 12, 0);
	bytes_read = recv(c_sock, buf, 1024, 0);

	buf[bytes_read] = '\0';
	printf("%s\n", buf);

	close(c_sock);

	return 0;
}