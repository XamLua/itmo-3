#ifndef __CLIENT__
#define __CLIENT__

#include "info.h"

void crit_err(int errnum);

void print_server_data(struct s_data* data);

int use_shared_memory();

int use_message_queue();

int use_posix_smo();

#endif
