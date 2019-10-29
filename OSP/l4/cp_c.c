#include "cp_c.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <limits.h>

#define BUFF_SIZE 1024
#define MESS_BUFF_SIZE 8096

extern int errno;

char message[MESS_BUFF_SIZE];

int main(int argc, char const* argv[])
{
	struct parse_data p_data;
	if(parse_args(argc, argv, &p_data) == -1)
		return -1;

	if(p_data.src_count == -1)
		return -1;

	if (p_data.src_count == 0)
	{
		p_data.src = malloc(sizeof(char*));
		if (p_data.src == NULL)
			malloc_error(errno);

		p_data.src[0] = malloc(2*sizeof(char));
		if (p_data.src[0] == NULL)
			malloc_error(errno);

		strncpy(p_data.src[0], "-", 2);
		p_data.src_count++;
	}

	for (int i = 0; i < p_data.src_count; ++i)
	{
		if (p_data.src_count > 1 && p_data.is_omitted != 1)
		{
			strncat(message, "==> ", 4);
			strncat(message, p_data.src[i], strnlen(p_data.src[i], 4000));
			strncat(message, " <==\n", 5);
			write(1, message, strnlen(message, BUFF_SIZE));
			message[0] = '\0';
		}
		print_lines(p_data.src[i], p_data.line_count);
		if (p_data.src_count > 1 && p_data.is_omitted != 1 && i != (p_data.src_count - 1))
			write(1, "\n", 1);
		free(p_data.src[i]);
	}
	free(p_data.src);
	return 0;
}

int parse_args(int argc, char const* argv[], struct parse_data* p_data)
{
	//parse data contains {source files names, number of source files, number of lines, headers omittion}
	p_data->src = NULL;
	p_data->src_count = 0;
	p_data->line_count = 10;
	p_data->is_omitted = 0;
	int i = 1;

	//Find keys
	for (; (i < argc) && argv[i][0] == '-' && strnlen(argv[i], 4000) > 1; ++i)
	{
		//Change number of lines
		if (strcmp(argv[i], "-n") == 0)
		{
			//Check if option has an argument
			if (++i == argc)
			{
				strncat(message, "option requires an argument -- 'n'\n", 36);
				write(1, message, strnlen(message, BUFF_SIZE));
				message[0] = '\0';
				return -1;
			}

			int* result = str_to_int(argv[i]);

			if (result == NULL)
				return -1;

			p_data->line_count = *result;
			free(result);
		}

		//Omit headers in case of multiple source files
		else if (strcmp(argv[i], "-q") == 0)
			p_data->is_omitted = 1;
		//Change line number or unknown option
		else
		{
			if (argv[i][1] < 48 || argv[i][1] > 57)
			{
				strncat(message, "invalid option -- '", 19);
				strncat(message, argv[i], strnlen(argv[i], 4000));
				strncat(message, "'\n", 3);
				write(1, message, strnlen(message, BUFF_SIZE));
				message[0] = '\0';
				return -1;
			}

			int* result = str_to_int(argv[i]);

			if (result == NULL)
				return -1;

			p_data->line_count = -(*result);
			free(result);
		}
	}

	//Find sources
	if (i < argc)
	{
		p_data->src = malloc((argc-i)*sizeof(char*));
		if (p_data->src == NULL)
		{
			malloc_error(errno);
			return -1;
		}

		p_data->src_count = argc-i;
	}

	for (int j = i; j < argc; j++)
	{
		p_data->src[j-i] = malloc((strnlen(argv[j], 4000)+1)*sizeof(char));
		if (p_data->src[j-i] == NULL)
		{
			malloc_error(errno);
			return -1;
		}

		strncpy(p_data->src[j-i], argv[j], strnlen(argv[j], 5000));
	}

	return 0;

}

int print_lines(const char* filepath, const int line_count){

	int errnum;

	char buf[BUFF_SIZE] = {0};

	struct stat path_stat;

	int bytes_read = 0;

	int fd;

	int pos_in_buf = 0;

	int file_offset = 0;

	int count = 0;


	if(	(stat(filepath, &path_stat) == -1 || S_ISDIR(path_stat.st_mode) != 0) && strcmp(filepath, "-") != 0)
	{
		errnum = errno;
		if (errnum == 0){
			strncat(message, "Error reading '", 15);
			strncat(message, filepath, strnlen(filepath, 5000));
			strncat(message, "': Is a directory\n", 19);
			write(1, message, strnlen(message, BUFF_SIZE));
			message[0] = '\0';
		}
		else{
			strncat(message, "Cannot open '", 13);
			strncat(message, filepath, strnlen(filepath, 5000));
			strncat(message, "' for reading: ", 15);
			strncat(message, strerror(errnum), 100);
			strncat(message, "\n", 2);
			write(1, message, strnlen(message, BUFF_SIZE));
			message[0] = '\0';
		}
		return -1;
	}

	if (strncmp(filepath, "-", 2) == 0)
		fd = 0;
	else
		fd = open(filepath, O_RDONLY);

	if (fd == -1)
	{
		errnum = errno;
		strncat(message, "Cannot open '", 13);
		strncat(message, filepath, strnlen(filepath, 5000));
		strncat(message, "' for reading: ", 15);
		strncat(message, strerror(errnum), 100);
		strncat(message, "\n", 2);
		write(1, message, strnlen(message, BUFF_SIZE));
		message[0] = '\0';
		return -1;
	}

	do
	{
		pos_in_buf = 0;

		if(lseek(fd, file_offset, SEEK_SET) == -1 && strcmp(filepath, "-") != 0)
		{
			errnum = errno;
			strncat(message, "Error occured during reading '", 32);
			strncat(message, filepath, strnlen(filepath, 5000));
			strncat(message, "': ", 5);
			strncat(message, strerror(errnum), 100);
			strncat(message, "\n", 2);
			write(1, message, strnlen(message, BUFF_SIZE));
			message[0] = '\0';
			return -1;
		}


		bytes_read = read(fd, buf, BUFF_SIZE);

		if (bytes_read == -1)
		{
			errnum = errno;
			strncat(message, "Error occured during reading '", 32);
			strncat(message, filepath, strnlen(filepath, 5000));
			strncat(message, "': ", 5);
			strncat(message, strerror(errnum), 100);
			strncat(message, "\n", 2);
			write(1, message, strnlen(message, BUFF_SIZE));
			message[0] = '\0';
			return -1;
		}

		while(pos_in_buf < bytes_read && buf[pos_in_buf] != '\n')
			pos_in_buf++;
	
		if (write(1, buf, pos_in_buf+1) == -1)
		{
			errnum = errno;
			strncat(message, "Error occure during writing to output: ", 42);
			strncat(message, strerror(errnum), 100);
			strncat(message, "\n", 2);
			write(1, message, strnlen(message, BUFF_SIZE));
			message[0] = '\0';
			return -1;
		}

		if (pos_in_buf != bytes_read)
			count++;

		file_offset += pos_in_buf+1;

		//printf("bytes_read = %d, pos_in_buf = %d, offset = %d,  count = %d\n", bytes_read, pos_in_buf, file_offset, count);

	} while (count < line_count && bytes_read != 0);

	close(fd);
	return count;
}

int* str_to_int(const char* number)
{
	int* conversion_result = malloc(sizeof(int));

	*conversion_result = 0;

	size_t i = 0;
	int digit;

	//Skip '+' or '-'
	if (number[0] == '-' || number[0] == '+')
		i++;

	do
	{
		//Check if char is a digit
		if (number[i] < 48 || number[i] > 57)
		{
			//Given string isn't a number
			strncat(message, "invalid number of lines: '", 26);
			strncat(message, number, strnlen(number, 12));
			strncat(message, "'\n", 3);
			write(1, message, strnlen(message, BUFF_SIZE));
			message[0] = '\0';
			return NULL;
		}
		
		//Integer value of current digit
		digit = number[i] - '0';

		//Check for overflow
		if ((*conversion_result >= INT_MAX/10) && (*conversion_result >= INT_MAX/10 ||  digit > INT_MAX%10))
		{
			//Value of given number is too large	
			strncat(message, "invalid number of lines: '", 26);
			strncat(message, number, strnlen(number, 12));
			strncat(message, "': Value too large for defined data type\n", 42);
			write(1, message, strnlen(message, BUFF_SIZE));
			message[0] = '\0';
			return NULL;
		}

		*conversion_result = *conversion_result*10 + digit;

		i++;

	} while (i < strnlen(number, 30));

	if (number[0] == '-')
		*conversion_result = -(*conversion_result);

	return conversion_result;
}

void malloc_error(int errnum)
{
	strncat(message, "malloc allocation error: ", 26);
	strncat(message, strerror(errnum), strnlen(strerror(errnum), 100));
	strncat(message, "\n", 2);
	write(1, message, strnlen(message, BUFF_SIZE));
	message[0] = '\0';
}