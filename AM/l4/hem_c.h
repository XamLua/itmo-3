#ifndef __HEM_C__
#define __HEM_C__

char * encode(int d, int n_u, char const * const message);

char * decode(int d, int n_k, char const *message);

void test_hem(int mes_len);

#endif