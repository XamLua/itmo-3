#ifndef __ALPH__
#define __ALPH__

#define BUF_SIZE 26

void handle_sigint(int sig);

void crit_err(int errnum);

void invert_case(char* alphabet);

void invert_order(char* alphabet);

void use_unnamed_semaphore();

void *invert_case_unnamed(void *sem);

void *invert_order_unnamed(void *sem);

#endif