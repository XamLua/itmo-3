#ifndef __SERVER_C__
#define __SERVER_C__

#include <ncurses.h>

#define AW_COUNT 2

WINDOW *create_win(int height, int width, int start_y, int start_x);

struct menus
{
	struct cli_menu *cnt_menu;
	struct cli_menu *thread_menu;
	WINDOW *log_win;
	struct server_stats *srv_stats;
	struct Node *active_window;
};

struct server_stats
{
	WINDOW *win;
};

int connection_routine(struct menus* menus);

struct menus* init_menus();

void input_handler(struct menus* menus);

void delete_win(WINDOW *win);

#endif