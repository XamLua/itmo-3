#include "crc_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define POLY_NUM 2

int main(int argc, char const *argv[])
{
	if (argc < 3)
		return -1;

	struct irr_poly *polys = read_polys();

	if (*argv[1] == 'e')
	{

		//Encoded and decoded with 1101
		int c_s = encode(argv[2], &polys[POLY_NUM], strnlen(argv[2], 100000));

		char *encoded_str = malloc((strnlen(argv[2], 100000) + polys[POLY_NUM].m + 1)*sizeof(char));

		sprintf(encoded_str, "%s%s", argv[2], int_to_binary_str(c_s, polys[POLY_NUM].m));	

		printf("Encoded string: %s\n", encoded_str); //0101110

		decode(encoded_str, &polys[POLY_NUM], (strnlen(argv[2], 100000) + polys[POLY_NUM].m));

	}	
	else if (*argv[1] == 't' && argc > 3)
	{
		char *end;
		int test_count = (int) strtol(argv[2], &end, 10);
		int mes_len = (int) strtol(argv[3], &end, 10);
		for (int i = 0; i < test_count; i++)
		{
			printf("          TEST %d          \n", i+1);
			test_crc(mes_len, polys);
			printf("--------------------\n");
		}
	}
	else
	{
		printf("Unsupported mode\n");
	}

	for (int i = 0; i < 11; ++i)
		free(polys[i].coefs);

	free(polys);

	return 0;
}

int decode(char *str, struct irr_poly *poly, int message_len)
{
	int remainder = 0;

	int orig_info = binary_string_to_int(str, 'r');

	int info = orig_info;

	int pl = binary_string_to_int(poly->coefs, 'n');

	//Calculate the remainder

	for (int i = 0; i < message_len; ++i)
	{
		remainder <<= 1;
		remainder += info % 2;
		info /= 2;
		if ((remainder & (1 << poly->m)) != 0)
			remainder ^= pl;
	}
	remainder &= (1 << (poly->m+1)) - 1;

	//Error correction if needed
	if (remainder != 0)
	{
		printf("Error detected\n");
		info = binary_string_to_int(str, 'n');
		int shift_count = 0, temp_info = 0;
		char *temp_info_str;
		char *bin_rem = int_to_binary_str(remainder, poly->m); 
		while (weight(bin_rem) != 1)	
		{
			//Cycle-shift to left
			info <<= 1;
			if ((info & (1 << message_len)) != 0)
				info++;
			info &= (1 << message_len) - 1;

			temp_info_str = int_to_binary_str(info, message_len);
			temp_info = binary_string_to_int(temp_info_str, 'r');
			remainder = 0;

			//Calculate the new remainder
			for (int i = 0; i < message_len; ++i)
			{
				remainder <<= 1;
				remainder += temp_info % 2;
				temp_info /= 2;
				if ((remainder & (1 << poly->m)) != 0)
					remainder ^= pl;
			}
			remainder &= (1 << (poly->m+1)) - 1;
			shift_count++;
			free(bin_rem);
			free(temp_info_str);
			bin_rem = int_to_binary_str(remainder, poly->m);
			printf("%s\n", bin_rem);
		}

		free(bin_rem);

		//Fix error
		info ^= remainder;

		//Shift back
		for (int i = 0; i < shift_count; ++i)
		{
			if (info % 2 != 0)
				info |= 1 << message_len;
			info >>= 1;
		}
		orig_info = info;
	}
	else
	{
		printf("No errors detected.\n");
		orig_info = binary_string_to_int(str, 'n');
	}

	char *decoded_str = int_to_binary_str(orig_info >> poly->m, message_len-poly->m);
	printf("Decoded string: %s\n", decoded_str);
	free(decoded_str);

	return remainder;
}

int weight(char *binary_str)
{
	int result = 0, i = 0;

	while(binary_str[i] != '\0')
	{
		if (binary_str[i++] == '1')
			result++;
	}

	return result;
}

int encode(char const *str, struct irr_poly *poly, int message_len)
{
	int remainder = 0;

	int info = binary_string_to_int(str, 'r');

	int pl = binary_string_to_int(poly->coefs, 'n');

	//Caluculate the remainder;

	for (int i = 0; i < message_len; ++i)
	{
		remainder <<= 1;
		remainder += info % 2;
		info /= 2;
		if ((remainder & (1 << poly->m)) != 0)
			remainder ^= pl;
	}

	for (int i = 0; i < poly->m; ++i)
	{
		remainder <<=1;
		if ((remainder & (1 << poly->m)) != 0)
			remainder ^= pl;
	}

	remainder &= (1 << (poly->m+1)) - 1;

	return remainder;

}

struct irr_poly * read_polys()
{

	FILE *fp = fopen(IP, "r");

	int temp_m, temp_d, count;
	char temp_str[7];

	//Total number of polynoms
	fscanf(fp, "%d\n", &count);

	//Allocate memory to store polynoms
	struct irr_poly * polys = malloc(sizeof(struct irr_poly)*count);

	//Read polynoms line by line
	for (int i = 0; i < count; ++i)
	{
		fscanf(fp, "%d %s %d\n", &temp_m, temp_str, &temp_d);
		polys[i].m = temp_m;
		polys[i].d = temp_d;

		polys[i].coefs = malloc(sizeof(temp_str));

		strncpy(polys[i].coefs, temp_str, 7);
	}

	fclose(fp);

	return polys;

}

int binary_string_to_int(char const *str, char mode)
{

	int temp = 0, result = 0, power = 1;
	if (mode == 'n')
	{
		while(str[temp] != '\0') temp++;

		for (int i = temp-1; i >= 0; i--)
		{
			result += (str[i] - '0')*power;
			power *= 2;
		}
	}
	else if (mode == 'r')
	{
		for (int i = 0; str[i]!='\0'; i++)
		{
			result += (str[i] - '0')*power;
			power *= 2;
		}
	}
	return result;
}

char * int_to_binary_str(int num, int d_c)
{
	char * result = malloc((d_c+1)*sizeof(char));

	for (int i = d_c-1; i >= 0; i--)
	{
		result[i] = num % 2 + '0';
		num /= 2;
	}

	result[d_c] = '\0';

	return result;
}

void test_crc(int mes_len, struct irr_poly *polys)
{
	mes_len++;
	char *str = malloc(mes_len * sizeof(char));
	const char charset[] = "01";
	//Generate random string
	int done = 0;
	while (!done)
	{
   		if (mes_len) 
   		{
        	mes_len--;
    	    for (int n = 0; n < mes_len; n++) 
    	    {
       	    	int key = rand() % (int) (sizeof charset - 1);
       	    	str[n] = charset[key];
       		}
       		str[mes_len] = '\0';
    	}
    	if (weight(str) <= 3)
    		done = 1;
    	else
    		mes_len++;
    }

    int c_s = encode(str, &polys[POLY_NUM], strnlen(str, 100000));

	char *encoded_str = malloc((strnlen(str, 100000) + polys[POLY_NUM].m + 1)*sizeof(char));

	char *temp_rem_str = int_to_binary_str(c_s, polys[POLY_NUM].m);
	sprintf(encoded_str, "%s%s", str, temp_rem_str);
	free(temp_rem_str);	

	printf("String: %s\n", str);

	printf("Encoded string: %s\n", encoded_str);

	int rnd = rand() % 100 + 1;

    //Random error addition
    if(rnd > 50)
    {
   		int rand_pos = rand() % (mes_len);
    	if(encoded_str[rand_pos] == '0')
			encoded_str[rand_pos] = '1';
		else
			encoded_str[rand_pos] = '0';
    	printf("Added error at place %d\n", rand_pos+1);
		printf("With error: %s\n", encoded_str);
    }
    else
    {
    	printf("No errors added.\n");
    }


	decode(encoded_str, &polys[POLY_NUM], (strnlen(str, 100000) + polys[POLY_NUM].m));

	free(str);
	free(encoded_str);
    
}
