#include "ar_c.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <malloc.h>

#define BLOCK_SIZE 8

extern int errno;

int symbols_count = 0;

long double* intervals;

int main(int argc, char const *argv[])
{
	long double* probabilities;

	//Count symbol's probabilities in the given file
	if (argc > 1)
		probabilities = count_probabilities(argv[1]);
	//Error handling
	else
	{
		printf("Not enough arguments.\n");
		return -1;
	}

	//Error handling
	if (probabilities == NULL)
		return -1;

	//Count cumulative probabilities
	struct q* qs = calc_q(probabilities);

	//Error handling
	if(qs == NULL)
		return -1;

	//Encode message
	long double* codes = encode(argv[1], qs);

	//Decode message
	char* decoded_text = decode(qs, codes);


	for (int i = 0; i < symbols_count && *argv[2] != '0'; ++i)
		printf("%c", decoded_text[i]);

	int encoded_bits = 0;
	for (int i = 0; i < symbols_count/BLOCK_SIZE + 1; ++i)
	{
		encoded_bits += ceil(log2l(intervals[i]));
	}

	printf("-------------------\n");

	printf("Sourse file size: %d bit\n", symbols_count*8);

	printf("Encoded file size: %d bit\n", encoded_bits);

	printf("Compression: %f\n", (double) symbols_count*8/encoded_bits);

	free(codes);
	free(decoded_text);
	free(probabilities);
	free(qs);

	return 0;
}

struct q* calc_q(long double const* probabilities)
{
	int errum;

	struct q* qs = (struct q*) calloc(128, sizeof(struct q));
	if (qs == NULL)
	{
		errum = errno;
		printf("Error while allocating memory: %s\n", strerror(errum));
		return NULL;
	}

	qs[0].l = 0;
	qs[0].h = probabilities[0];
	qs[0].r = probabilities[0];

	for (int i = 1; i < 128; ++i)
		{
			qs[i].l = qs[i-1].h;
			qs[i].r = probabilities[i];
			qs[i].h = qs[i].l + qs[i].r;
		}

	return qs;

}

long double* encode(char const* filename, struct q* qs)
{
	long double* result = (long double*) calloc((symbols_count/BLOCK_SIZE + 1), sizeof(long double));
	intervals = (long double*) calloc((symbols_count/BLOCK_SIZE + 1), sizeof(long double));
	struct q result_q;
	result_q.l = 0;
	result_q.h = 1;
	result_q.r = 1;

	char buf[1024];
	int fd = -1;
	int errum, pos_in_buf, bytes_read, file_offset = 0, block_count = 0, symbols_block = 0;

	//Open file for reading
	fd = open(filename, O_RDONLY);

	//Error handling
	if(fd == -1)
	{
		errum = errno;
		printf("Error: cannot open file '%s': %s\n", filename, strerror(errum));
		return NULL;
	}

	//Main cycle
	do
	{
		pos_in_buf = 0;

		//Change position in the file according to a number of already read symbols

		//Error handling
		if(lseek(fd, file_offset, SEEK_SET) == -1)
		{
			errum = errno;
			printf("Error occured during reading '%s': %s\n", filename, strerror(errum));
			return NULL;
		}

		//Read uup to BUFF_SIZE bytes from file
		bytes_read = read(fd, buf, 1024);

		//Error handling
		if(bytes_read == -1)
		{
			errum = errno;
			printf("Error occured during reading '%s': %s\n", filename, strerror(errum));
			return NULL;
		}

		//If there're symbols in buffer and we haven't already read enough symbols for the block
		while((pos_in_buf < bytes_read) && (symbols_block < BLOCK_SIZE))
		{
			//Alphabet
			if ((buf[pos_in_buf] > 31 && buf[pos_in_buf] < 127) || buf[pos_in_buf] == 10)
			{	
				//Encode symbol buf[pos_in_buf]
				result_q.l += qs[(int) buf[pos_in_buf]].l * result_q.r;
				result_q.r *= qs[(int) buf[pos_in_buf]].r;
				result_q.h = result_q.l + result_q.r;
				symbols_block++;
			}
			pos_in_buf++;
		}

		//If we've read enough for the block or there're no more bytes left in file
		if(bytes_read != 0 && (symbols_block == BLOCK_SIZE || pos_in_buf == bytes_read))
		{
			result[block_count++] = (result_q.l+result_q.h)/2;
			intervals[block_count++] = result_q.r;
			result_q.l = 0;
			result_q.h = 1;
			result_q.r = 1;
			symbols_block = 0;
		}

		//Calculate new offset
		file_offset += pos_in_buf;

	} while (bytes_read != 0);

	return result;
}

char* decode(struct q* qs, long double* codes)
{
	char* decoded_string = (char*) calloc((symbols_count+1), sizeof(char));

	for (int i = 0; i < symbols_count + 1; ++i)
		decoded_string[i] = '\0';

	struct q curr;
	curr.l = 0;
	curr.h = 1;
	curr.r = 1;

	//Main cycle
	for (int i = 0; i < symbols_count/BLOCK_SIZE+1; i++)
	{
		//Decoding one block
		for (int k = 0; k < BLOCK_SIZE && i*BLOCK_SIZE + k <= symbols_count; k++)
		{
			//Checking all symbols for the appropeiate interval
			for (int j = 0; j < 128; j++)
			{	
				//If the interval is suitable
				if(curr.l+qs[j].l*curr.r <= codes[i] && curr.l+qs[j].h*curr.r >= codes[i] && qs[j].r != 0.0)
				{
					//Update decoded string
					decoded_string[i*BLOCK_SIZE + k] = (char) j;
					curr.l += qs[j].l*curr.r;
					curr.r *= qs[j].r;
					curr.h = curr.l + curr.r;
					break;
				}
			}
		}
		curr.l = 0;
		curr.h = 1;
		curr.r = 1;
	}

	//Null terminate the string
	decoded_string[symbols_count] = '\0';
	return decoded_string;

}

long double* count_probabilities(char const* filename)
{

	long double* const probabilities = (long double*) calloc(128, sizeof(long double));
	for (int i = 0; i < 128; ++i)
		probabilities[i] = 0.0;

	char buf[BLOCK_SIZE];
	int fd = -1;
	int errum, pos_in_buf, bytes_read, file_offset = 0;

	int* const symbols = (int*) calloc(128, sizeof(int));

	//Error handling
	if (symbols == NULL)
	{
		errum = errno;
		printf("Error while allocating memory: %s\n", strerror(errum));
		return NULL;
	}
	//Initialize 
	else
	{
		for (int i = 0; i < 128; ++i)
		{
			symbols[i] = 0;
		}
	}

	//Open file for reading
	fd = open(filename, O_RDONLY);

	//Error handling
	if(fd == -1)
	{
		errum = errno;
		printf("Error: cannot open file '%s': %s\n", filename, strerror(errum));
		return NULL;
	}

	//Main reading cycle
	do
	{
		pos_in_buf = 0;

		//Offset in file

		//Error handling
		if(lseek(fd, file_offset, SEEK_SET) == -1)
		{
			errum = errno;
			printf("Error occured during reading '%s': %s\n", filename, strerror(errum));
			return NULL;
		}

		//Read bytes from file
		bytes_read = read(fd, buf, BLOCK_SIZE);

		//Error handling
		if(bytes_read == -1)
		{
			errum = errno;
			printf("Error occured during reading '%s': %s\n", filename, strerror(errum));
			return NULL;
		}

		//While there're unchecked bytes in buffer
		while(pos_in_buf < bytes_read)
		{
			//Alphabet
			if ((buf[pos_in_buf] > 31 && buf[pos_in_buf] < 127)|| buf[pos_in_buf] == 10)
			{	
				//Increase counter of symbol
				symbols[(int)buf[pos_in_buf]]++;
				symbols_count++;
			}
			pos_in_buf++;
		}

		//Calculate new offset
		file_offset += pos_in_buf;

	} while (bytes_read != 0);


	//Calculate probabilities
	for (int i = 0; i < 128; ++i)
	{
		if(symbols[i] != 0)
			probabilities[i] = (long double)symbols[i]/symbols_count;
	}

	free(symbols);

	return probabilities;
}