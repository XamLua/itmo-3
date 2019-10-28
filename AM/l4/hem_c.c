#include "hem_c.h"
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

int main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		printf("not enough args\n");
		return -1;
	}

	//Encoding mode
	if (*argv[1] == 'e')
	{
		char *end;
		int d = (int) strtol(argv[2], &end, 10);

		int n_u = strnlen(argv[3], 100000);

		char *code = encode(d, n_u, argv[3]);

		if (code != NULL)
		{
			printf("%s\n", code);
			free(code);
		}
		else
			return -1;

		return 0;
	}
	//Decoding mode
	else if(*argv[1] == 'd')
	{
		char *end;
		int d = (int) strtol(argv[2], &end, 10);
		int n_k = (int) strtol(argv[3], &end, 10);
		char *message = decode(d, n_k, argv[4]);

		if (message != NULL)
		{
			printf("%s\n", message);
			free(message);
		}
		else
			return -1;
	}
	//Test mode
	else if(*argv[1] == 't')
	{
		char *end;
		int test_count = (int) strtol(argv[2], &end, 10);
		int mes_len = (int) strtol(argv[3], &end, 10);
		for (int i = 0; i < test_count; ++i)
		{
			printf("          TEST %d          \n", i+1);
			test_hem(mes_len);
			printf("--------------------\n");
		}
	}

	return 0;
}

char * decode(int d, int n_k, char const *message)
{

	int error_place = 0;
	int temp = 1;
	int m_count = 0;
	int p_b = 0;

	int n = strnlen(message, 100000) - (d == 4 ? 1 : 0);

	char *new_code = malloc((n-n_k+1)*sizeof(char));
	new_code[n-n_k] = '\0';

	//Extract info symbols
	for (int i = 1; i <= n; ++i)
	{
		if (temp == i)
			temp*=2;
		else
			new_code[m_count++] = message[i-1];
		p_b += message[i-1] - '0';
	}

	if (d == 4)
		p_b += message[n] - '0';

	//Encode extracted i.s.
	char * check_message = encode(d, n-n_k, new_code);

	temp = 1;

	//Check syndrom
	while(temp < n)
	{
		if (message[temp-1] != check_message[temp-1])
			error_place += temp;

		temp*=2;
	}

	//Special case for the single error in last symbol
	if (d == 4 && message[n] != check_message[n] && error_place == 0)
		error_place = n + 1;

	//Info about errors
	if(d == 4 && p_b % 2 == 0 && error_place != 0)
		printf("double error in message, cannot fix\n");
	else if(error_place != 0)
	{
		printf("single error in place %d\n", error_place);
		//Fixing single errors
		temp = 1;
		int counter = 0;
		while (temp < error_place)
		{
			counter++;
			temp*=2;
		}	

		if (temp != error_place && !(d == 4 && error_place == (n+1)))
		{
			if(new_code[error_place - counter - 1] == '0')
				new_code[error_place - counter - 1] = '1';
			else
				new_code[error_place - counter - 1] = '0';
		}
	}
	else
		printf("no errors detected\n");

	free(check_message);

	return new_code;
}

char * encode(int d, int n_u, char const * const message)
{
	//Only d == 3|4
	if (d > 4)
	{
		printf("Not supported\n");
		return NULL;
	}
	//n_k = ]lb(n_u+1 + ]lb(n_u+1)[)[
	int n_k = (int) ceil( log( (double) n_u + 1.0 + ceil( log( (double) n_u + 1.0) / log(2.0)))/log(2.0));

	int n = n_u + n_k;
	
	//Encoding process info
	printf("d = %d, n_u = %d, n_k = %d\n", d, n_u, n_k);

	char *code = malloc((n+1 + (d == 4 ? 1 : 0))*sizeof(char));
	code[n] = '\0';

	int temp = 1;
	int m_count = 0;
	int res = 0;

	//Copy i.s. to encoded string
	for (int i = 1; i <= n; ++i)
	{
		if (temp == i)
		{
			code[i-1] = '0';
			temp*=2;
		}
		else
			code[i-1] = message[m_count++];

	}

	temp = 1;

	//Calculate control symbols
	for (int i = 0; i < n_k; i++)
	{
		res = 0;
		for (int j = temp; j <= n; j+=temp)
		{
			for (int k = 1; k <= temp && j<=n; k++)
				res += code[j++ - 1] - '0';
		}
		code[temp-1] = (res % 2 == 0 ? '0' : '1');
		temp *=2;
	}

	code[n] = '\0';

	//Calculate total parity bit if necessary
	if (d == 4)
	{
		res = 0;
		for (int i = 0; i < n; ++i)
			res += code[i] - '0';

		code[n] = (res % 2 == 0 ? '0' : '1');
		code[n+1] = '\0';
	}

	return code;
}

void test_hem(int mes_len)
{
	int n_u = mes_len;
	mes_len++;
	char *str = malloc(mes_len * sizeof(char));
	const char charset[] = "01";
	//Generate random string
    if (mes_len) {
        mes_len--;
        for (int n = 0; n < mes_len; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[mes_len] = '\0';
    }
    
    printf("test string: %s\n", str);
    //Test for both d
    for(int d = 3; d <= 4; d++)
    {
    	char *ans = encode(d, n_u, str);
    	int n_k = (int) ceil( log( (double) n_u + 1.0 + ceil( log( (double) n_u + 1.0) / log(2.0)))/log(2.0));
    	
    	printf("encoded with d = %d: %s\n", d, ans);
    	
    	int rnd = rand() % 100 + 1;

    	//Random error addition
    	if(rnd > 66)
    	{
   		int rand_pos = rand() % (n_u + n_k);
    		if(ans[rand_pos] == '0')
				ans[rand_pos] = '1';
			else
				ans[rand_pos] = '0';
    		printf("Added error at place %d\n", rand_pos+1);
    	}
    	else if (rnd < 33 && d == 4)
    	{
    		int rand_pos_1 = rand() % (n_u + n_k);
    		int rand_pos_2 = rand() % (n_u + n_k);
    		while (rand_pos_1 == rand_pos_2)
    			rand_pos_2 = rand() % (n_u + n_k);
    		
    		if(ans[rand_pos_1] == '0')
				ans[rand_pos_1] = '1';
			else
				ans[rand_pos_1] = '0';
    		
			if(ans[rand_pos_2] == '0')
				ans[rand_pos_2] = '1';
			else
				ans[rand_pos_2] = '0';
    		
    		printf("Added double error\n");
    	}
    	else
    	{	
    		printf("No errors added\n");
    	}
	
		char *decoded_ans = decode(d, n_k, ans);
    	printf("decoded with d = %d: %s\n", d, decoded_ans);

    	free(ans);
    	free(decoded_ans);
    }
    free(str);
}