#ifndef __THREAD_MENU__
#define __THREAD_MENU__

struct thread_data
{
	int pid;
	int ppid;
	int nice;
};

int highlight_thread (void* v_menu, int mode);
int print_thread_header (void* v_menu);
int print_thread_footer (void* v_menu);


#endif