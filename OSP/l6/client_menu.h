#ifndef __CLIENT_MENU__
#define __CLIENT_MENU__

struct client_data
{
	char* socket;
	int pid;
	int count;
	char* last;
};

int highlight_client (void* v_menu, int mode);
int print_client_header (void* v_menu);
int print_client_footer (void* v_menu);

#endif