#include <ncurses.h>
#include "thread_menu.h"
#include "cli_menu.h"
#include "utils.h"
#include <malloc.h>
#include <stdio.h>

int highlight_thread (void* v_menu, int mode)
{
	struct cli_menu* menu = (struct cli_menu*) v_menu;
	int temp = 0;

	struct Node *temp_node = menu->node_list;

	//Clear window
	werase(menu->win);
	box(menu->win, 0, 0);
    int skip_rows = menu->print_header(v_menu);
	wrefresh(menu->win);
	
	//Check if menu is empty
	if (temp_node == NULL)
		return 0;

	//Go down menu
	if (mode == MENU_DOWN)
		menu->highlighted_node = menu->highlighted_node->next == NULL ? menu->node_list : menu->highlighted_node->next;
	//Go up menu
	else if (mode == MENU_UP)
	{
		menu->highlighted_node = menu->highlighted_node->prev;
		//If highlighted item is head
		if (menu->highlighted_node == NULL)
		{
			while(temp_node->next != NULL)
				temp_node = temp_node->next;
			menu->highlighted_node = temp_node;
		}
	}

	//Get info about window
	int col = getmaxx(menu->win);

	//Redraw new menu
	temp_node = menu->node_list;
	struct thread_data* t_data;
	while(temp_node != NULL)
	{
		t_data = (struct thread_data*) temp_node->data;
		if (temp_node == menu->highlighted_node && menu->is_active)
			wattron(menu->win, A_STANDOUT);

		//Print node data
		for (int i = 1; i < col-1; ++i)
			mvwprintw(menu->win, temp+skip_rows, i, "%s", " ");
   		
   		mvwprintw(menu->win, temp+skip_rows, 1, "%d", t_data->pid);
    	mvwprintw(menu->win, temp+skip_rows, 1 + col/3, "%d", t_data->ppid);
   		mvwprintw(menu->win, temp+skip_rows, 1 + 2*col/3, "%d", t_data->nice);

		if (temp_node == menu->highlighted_node&& menu->is_active)
			wattroff(menu->win, A_STANDOUT);
		
		temp_node=temp_node->next;
		temp++;
	}

	wrefresh(menu->win);
	//print_list(menu->node_list, menu->highlighted_node);

	return 0;
}

int print_thread_header (void* v_menu)
{
	struct cli_menu* menu = (struct cli_menu*) v_menu;

	//Get info about window
	int col = getmaxx(menu->win);

	//Print header
	mvwprintw(menu->win, 0, 1 + col/2 - 3, "%s", "THREADS");

    //Print header columns
    wattron(menu->win, A_BOLD);

    mvwprintw(menu->win, 1, 1, "%s", "pid");
    mvwprintw(menu->win, 1, 1 + col/3, "%s", "ppid");
    mvwprintw(menu->win, 1, 1 + 2*col/3, "%s", "nice");

    wattroff(menu->win, A_BOLD);

    return 2;
}

int print_thread_footer (void* v_menu)
{
	if (v_menu == NULL)
	{
		return 313;
	}
	return 131;
}