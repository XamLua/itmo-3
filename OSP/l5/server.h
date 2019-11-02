#ifndef __SERVER_C__
#define __SERVER_C__

void crit_err(int errnum);

int use_shared_memory();

int use_message_queue();

int use_posix_smo();

#endif 
