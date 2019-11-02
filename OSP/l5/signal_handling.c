#include "info.h"
#include "signal_handling.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/loadavg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

static struct s_data data;

int main()
{
	//Initialize data
	data.pid = getpid();
	data.uid = getuid();
	data.gid = getgid();
	data.t_start = time(NULL);
	data.t_work = 0;
	getloadavg(data.load, 3);

	//Add handlers
	struct sigaction hup_action, int_action, term_action, usr1_action, usr2_action;
	hup_action.sa_handler = handle_hup;
	int_action.sa_handler = handle_int;
	term_action.sa_handler = handle_term;
	usr1_action.sa_handler = handle_usr1;
	usr2_action.sa_handler = handle_usr2;

	sigemptyset (&hup_action.sa_mask);
	sigemptyset (&int_action.sa_mask);
	sigemptyset (&term_action.sa_mask);
	sigemptyset (&usr1_action.sa_mask);
	sigemptyset (&usr2_action.sa_mask);

	hup_action.sa_flags = 0;
	int_action.sa_flags = 0;
	term_action.sa_flags = 0;
	usr1_action.sa_flags = 0;
	usr2_action.sa_flags = 0;

	sigaction(SIGINT, &int_action, NULL);
	sigaction(SIGHUP, &hup_action, NULL);
	sigaction(SIGTERM, &term_action, NULL);
	sigaction(SIGUSR1, &usr1_action, NULL);
	sigaction(SIGUSR2, &usr2_action, NULL);

	//Main cycle
	while(1)
	{
		sleep(1);
		data.t_work = time(NULL) - data.t_start;
		getloadavg(data.load, 3);
	}
	return 0;
}

void handle_hup(int num)
{
	printf("Handled %s\n", strsignal(num));
	printf("Server process id: %lu\n", (unsigned long) data.pid);
}
void handle_int(int num)
{
	printf("Handled %s\n", strsignal(num));
	printf("Server user id: %lu\n", (unsigned long) data.uid);
}
void handle_term(int num)
{
	printf("Handled %s\n", strsignal(num));
	printf("Server group id: %lu\n", (unsigned long) data.gid);
}
void handle_usr1(int num)
{
	printf("Handled %s\n", strsignal(num));
	printf("Seconds from start: %ld\n", data.t_work);
}
void handle_usr2(int num)
{
	printf("Handled %s\n", strsignal(num));
	printf("Average Load for 1 minute: %f\n", data.load[0]);
	printf("Average Load for 5 minute: %f\n", data.load[1]);
	printf("Average Load for 15 minute: %f\n", data.load[2]);
}
