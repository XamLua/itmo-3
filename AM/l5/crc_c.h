#ifndef __CRC_C__
#define __CRC_C__

#define IP "irreducible_polynomials.txt"

struct irr_poly
{
	int m;
	char* coefs;
	int d;
};

struct irr_poly * read_polys();

int encode(char const *str, struct irr_poly *poly, int message_len);

int decode(char *str, struct irr_poly *poly, int message_len);

int binary_string_to_int(char const *str, char mode);

char * int_to_binary_str(int num, int d_c);

void test_crc(int mes_len, struct irr_poly *polys);

int weight(char *binary_str);

#endif