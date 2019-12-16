#ifndef __CLI__MENU__
#define __CLI__MENU__ 

#include <ncurses.h>

#define MENU_UP -1
#define MENU_DOWN 1
#define MENU_CURRENT 0
#define MENU_NONE 2

struct cli_menu
{
	WINDOW *win;
	int is_active;
	int (*add) (void*, void*);
	int (*delete) (void*);
	int (*highlight) (void*, int);
	int (*print_header) (void*);
	int (*print_footer) (void*);
	struct Node *node_list;
	struct Node *highlighted_node;
};

int add(void* v_menu, void* data);
int delete(void* v_menu);

#endif