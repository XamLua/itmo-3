#include "unnamed_ch.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

extern int errno;

int main(int argc, char const *argv[])
{
	if(argc != 2)
		return -1;

	char const *filepath = argv[1];
	//Prepare pipe
	int fds[2];
	if(pipe(fds) < 0)
		crit_err(errno);

	int fid = fork();

	if (fid < 0)
		crit_err(errno);

	//Child
	if (fid == 0)
	{
		//Close write fd
		close(fds[1]);

		//Substitute STDIN
		if (dup2(fds[0], 0) < 0)
			crit_err(errno);

		//Execute wc
		if(execl("/usr/bin/wc", "wc", NULL) < 0)
			crit_err(errno);

		close(fds[0]);
	}
	//Parent
	else
	{
		//Close read fd
		close(fds[0]);

		char buf[BUFF_SIZE] = {0};
		int bytes_read = 0;
		int fd;
		int pos_in_buf = 0;
		int file_offset = 0;
		
		//Open file
		fd = open(filepath, O_RDONLY);

		if (fd < 0)
			crit_err(errno);

		//Read from file
		do
		{
			pos_in_buf = 0;

			if(lseek(fd, file_offset, SEEK_SET) == -1)
				crit_err(errno);

			bytes_read = read(fd, buf, BUFF_SIZE);

			if (bytes_read == -1)
				crit_err(errno);

			while(pos_in_buf < bytes_read)
			{
				write(fds[1], &buf[pos_in_buf], sizeof(char));
				pos_in_buf+=2;
			}

			file_offset += bytes_read+1;

		} while(bytes_read != 0);

		close(fds[1]);
	}
	return 0;
}

void crit_err(int errnum)
{
	printf("Critical error: %s", strerror(errnum));
	exit(EXIT_FAILURE);
}
