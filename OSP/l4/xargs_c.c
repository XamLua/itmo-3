#include "xargs_c.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFF_SIZE 1024

extern int errno;

char buf[BUFF_SIZE] = {0};

char result[50000] = {0};

char prev_char = '\0';

char message[5000];

int errnum;

int is_escaped = 0;

int is_prev_char_escaped = 0;

int is_dq = 0;

int count = 0;

int main()
{
	if (print_lines("-") != -1 && write(1, result, count) == -1)
		{
			errnum = errno;
			strcat(message, "Error occure during writing to output: ");
			strcat(message, strerror(errnum));
			strcat(message, "\n");
			write(1, message, strlen(message));
			message[0] = '\0';
			return -1;
		}

	return 0;
}

int print_lines(const char* filepath){

	int bytes_read;

	int fd = 0;

	int pos_in_buf = 0;

	do
	{
		pos_in_buf = 0;

		bytes_read = read(fd, buf, BUFF_SIZE);

		if (bytes_read == -1)
		{
			errnum = errno;
			strcat(message, "Error occured during reading '");
			strcat(message, filepath);
			strcat(message, "': ");
			strcat(message, strerror(errnum));
			strcat(message, "\n");
			write(1, message, strlen(message));
			message[0] = '\0';
			return -1;
		}

		while(pos_in_buf < bytes_read)
		{
			switch(buf[pos_in_buf])
			{
				case '"':
					if (is_escaped)
					{
						result[count++] = '"'; 
						is_escaped = 0;
						is_prev_char_escaped = 1;
					}
					else if (is_dq)
					{
						is_dq = 0;
						is_prev_char_escaped = 0;
					}
					else
					{
						is_dq = 1;
						is_prev_char_escaped = 0;
					}
					prev_char = '"';
					break;	

				case '\n':
					if (is_dq)
					{
						strcat(message, "unmatched double quote; by default quotes are special to xargs\n");
						write(1, message, strlen(message));
						message[0] = '\0';
						return -1;
					}
					if (is_escaped)
					{
						result[count++] = '\n'; 
						is_escaped = 0;
						is_prev_char_escaped = 1;
					}
					else if ((prev_char != '\n' && prev_char != ' ') || is_prev_char_escaped != 0)
					{
						result[count++] = ' ';
						is_prev_char_escaped = 0;
					}
					prev_char = '\n';
					break;

				case ' ':
					if (is_dq)
					{
						result[count++] = ' ';
						is_prev_char_escaped = 0;
					}
					else if (is_escaped)
					{
						result[count++] = ' ';
						is_escaped = 0;
						is_prev_char_escaped = 1;
					}
					else if ((prev_char != '\n' && prev_char != ' ') || is_prev_char_escaped != 0)
					{
						result[count++] = ' ';
						is_prev_char_escaped = 0;
					}
					prev_char = ' ';
					break;

				case '\\':
					if (is_dq)
						result[count++] = '\\';
					else if (is_escaped)
					{
						result[count++] = '\\';
						is_escaped = 0;
						is_prev_char_escaped = 1;
					}
					else
						is_escaped = 1;

					prev_char = '\\';
					break;

				case '\0':
					if (is_escaped)
					{
						result[count++] = '\0';
						is_escaped = 0;
						is_prev_char_escaped = 1;
					}
					else
					{
						result[count++] = '\n';
						return 0;
					}
					break;

				default:
					if (is_escaped)
					{
						result[count++] = buf[pos_in_buf];
						is_escaped = 0;
						is_prev_char_escaped = 1;
					}
					else
						result[count++] = buf[pos_in_buf];

					prev_char = buf[pos_in_buf];
					break;


			}
			pos_in_buf++;
		}

		//printf("bytes_read = %d, pos_in_buf = %d, offset = %d,  count = %d\n", bytes_read, pos_in_buf, file_offset, count);

	} while (bytes_read != 0);

	result[count++] = '\n';

	return count;
}