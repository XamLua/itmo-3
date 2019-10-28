#ifndef __AR_C__
#define __AR_C__

struct q
{
	long double l;
	long double h;
	long double r;
};

long double* count_probabilities(char const* filename);

struct q* calc_q(long double const* probabilities);

long double* encode(char const* filename, struct q* qs);

char* decode(struct q* qs, long double* codes);

#endif