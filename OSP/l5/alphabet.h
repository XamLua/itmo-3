#ifndef __ALPH__
#define __ALPH__

#define ALPH_BUF_SIZE 26

#define SEM_ID_C 313
#define SEM_ID_O 626

#define PERMISSIONS 0666

#include <pthread.h>

struct intervals
{
	unsigned long time_p;
	unsigned long time_c;
	unsigned long time_o;
	unsigned long time_count;
};

struct m_data
{
	void* *lock;
	unsigned long interval;
};

void handle_sigint(int sig);

void crit_err(int errnum);

void invert_case(char* alphabet);

void invert_order(char* alphabet);

void use_unnamed_semaphore();

void *invert_case_unnamed(void *sem);

void *invert_order_unnamed(void *sem);

void use_s5_semaphore();

void *invert_case_s5();

void *invert_order_s5();

void use_mutex(struct intervals *irvs);

void *invert_case_mutex(void *s);

void *invert_order_mutex(void *s);

void use_rwlock(struct intervals *irvs);

void *invert_case_rwlock(void *s);

void *invert_order_rwlock(void *s);

void *count_upcl(void *s);

#endif
