#include <ncurses.h>
#include "server.h"
#include "utils.h"
#include "cli_menu.h"
#include "client_menu.h"
#include "thread_menu.h"
#include "server_socket.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h> 
#include <menu.h>

int main(/*int argc, char const *argv[]*/)
{
	initscr();	
	cbreak();			
	keypad(stdscr, TRUE);
    refresh();
    noecho();
    curs_set(0);

    struct menus* menus = init_menus();
    connection_routine(menus);
    printw("%d\n", init_socket());
    input_handler(menus);

    //wrefresh(menus->cnt_menu->win);

    getch();
    endwin();
	return 0;
}


int connection_routine(struct menus* menus)
{
	struct client_data *c_data = malloc(sizeof(struct client_data));
	c_data->socket = "socket1";
	c_data->pid = 111111;
	c_data->count = 1;
	c_data->last = "command1";
	menus->cnt_menu->add(menus->cnt_menu, (void*) c_data);

	c_data = malloc(sizeof(struct client_data));
	c_data->socket = "socket2";
	c_data->pid = 222222;
	c_data->count = 2;
	c_data->last = "command2";
	menus->cnt_menu->add(menus->cnt_menu, (void*) c_data);

	c_data = malloc(sizeof(struct client_data));
	c_data->socket = "socket3";
	c_data->pid = 333333;
	c_data->count = 3;
	c_data->last = "command3";
	menus->cnt_menu->add(menus->cnt_menu, (void*) c_data);

	struct thread_data *t_data = malloc(sizeof(struct thread_data));
	t_data->pid = 22222222;
	t_data->ppid = 22222222;
	t_data->nice = 22222222;
	menus->thread_menu->add(menus->thread_menu, (void*) t_data);

	t_data = malloc(sizeof(struct thread_data));
	t_data->pid = 33333333;
	t_data->ppid = 33333333;
	t_data->nice = 33333333;
	menus->thread_menu->add(menus->thread_menu, (void*) t_data);

	t_data = malloc(sizeof(struct thread_data));
	t_data->pid = 11111111;
	t_data->ppid = 11111111;
	t_data->nice = 11111111;
	menus->thread_menu->add(menus->thread_menu, (void*) t_data);
	return 0;
}

struct menus* init_menus()
{
	//Alloc memory for result
	struct menus *result = malloc(sizeof(struct menus));

	//Get info about window
	int row, col;

    getmaxyx(stdscr, row, col); 

    //Create and init client menu
	struct cli_menu *cnt_menu = malloc(sizeof(struct cli_menu));
	cnt_menu->win = create_win(row/2 - 4, col/2, 4, col - col/2);
	wrefresh(cnt_menu->win);
	cnt_menu->add = add;
	cnt_menu->delete = delete;
	cnt_menu->highlight = highlight_client;
	cnt_menu->print_header = print_client_header;
	cnt_menu->highlighted_node = NULL;
	cnt_menu->node_list = NULL;
	cnt_menu->is_active = 1;
	//Create and init thread menu
	struct cli_menu *thread_menu = malloc(sizeof(struct cli_menu));
	thread_menu->win = create_win(row/2 - 4, col/2, 4, 0);
	wrefresh(thread_menu->win);
	thread_menu->add = add;
	thread_menu->delete = delete;
	thread_menu->highlight = highlight_thread;
	thread_menu->print_header = print_thread_header;
	thread_menu->highlighted_node = NULL;
	thread_menu->node_list = NULL;
	thread_menu->is_active = 0;

	//Create and init server stats
	struct server_stats* srv_stats = malloc(sizeof(struct server_stats));
	srv_stats->win = create_win(4, col, 0, 0);
	mvwprintw(srv_stats->win, 1, 1, "tut servera stati");
	wrefresh(srv_stats->win);

	//Create and init active window list
	struct Node *wds[AW_COUNT];

	for (int i = 0; i < AW_COUNT; ++i)
		wds[i] = malloc(sizeof(struct Node));

	for (int i = 0; i < AW_COUNT; ++i)
	{
		wds[i]->next = wds[(i+1) % AW_COUNT];
		wds[i]->prev = wds[(i+(AW_COUNT-1)) % AW_COUNT];
	}

	wds[0]->data = (void*) cnt_menu;
	wds[1]->data = (void*) thread_menu;

	result->cnt_menu = cnt_menu;
	result->thread_menu = thread_menu;
	result->srv_stats = srv_stats;
	result->active_window = wds[0];

	return result;
}

void input_handler(struct menus* menus)
{
	struct cli_menu *clients = menus->cnt_menu;
	struct cli_menu *active_menu = (struct cli_menu*) menus->active_window->data;

	int ch;
	struct client_data *c_data;
	while((ch = getch()) != KEY_F(1))
	{	
		switch(ch)
		{
			case KEY_UP:
				active_menu->highlight((void*) active_menu,  MENU_UP);
				break;
			case KEY_DOWN:
				active_menu->highlight((void*) active_menu, MENU_DOWN);
				break;
			case KEY_LEFT:
				c_data = malloc(sizeof(struct client_data));
				c_data->socket = "socket";
				c_data->pid = 000000;
				c_data->count = 0;
				c_data->last = "command";
				clients->add(clients, c_data);
				break;
			//Delete key - delete entry in menu
			case 330:
				active_menu->delete((void*) active_menu);
				break;
			//Tab key - switch betweeb menus
			case 9:
				//Unhighlight old menu
				active_menu->is_active = 0;
				active_menu->highlight((void*) active_menu, MENU_CURRENT);

				//Switch to the next menu in cycle
				menus->active_window = menus->active_window->next;
				active_menu = (struct cli_menu*) menus->active_window->data;
				
				//Highlight new menu
				active_menu->is_active = 1;
				active_menu->highlight((void*) active_menu, MENU_CURRENT);
				break;
			default:
				printw("%d", ch);
		}
	}
}


WINDOW *create_win(int height, int width, int start_y, int start_x)
{
	WINDOW *win; 

	win = newwin(height, width, start_y, start_x);
	box(win, 0, 0);

	wrefresh(win);

	return win;
}

void delete_win(WINDOW *win)
{
	wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');

	wrefresh(win);

	delwin(win);
}

//Debug shit
void print_list(struct Node* head)
{

	if (head == NULL)
	{
		printw("Empty\n");
		return;
	}

	if(head->prev == NULL)
	{
		printw("null->");
	}
	else
	{
		printw("u head'a prev ne null\n");
		return;
	}

	while(head != NULL)
	{
		printw("%s->", head->data);
		head = head->next;
	}

	if (head == NULL)
	{
		printw("null\n");
	}
	else
		printw("u teila next ne null\n");

	return;
}