#include <ncurses.h>
#include "cli_menu.h"
#include "utils.h"
#include <malloc.h>
#include <stdio.h>

int add (void* v_menu, void* data)
{
	struct cli_menu* menu = (struct cli_menu*) v_menu;
	//Init new item
	struct Node *item = malloc(sizeof(struct Node));
	item->data = data;
	item->next = NULL;
	item->prev = NULL;

	//If no items exist
	if (menu->node_list == NULL)
	{
		menu->node_list = item;
		menu->highlighted_node = item;
	}
	else
	{
		struct Node *temp_item = menu->node_list;
		//Find last item
		while(temp_item->next != NULL)
			temp_item = temp_item->next;

		item->prev = temp_item;
		temp_item->next = item;
	}

	menu->highlight(menu, MENU_CURRENT);

	return 0;

}

int delete (void* v_menu)
{
	struct cli_menu* menu = (struct cli_menu*) v_menu;
	//Delete item from linked list

	//If list is empty
	if(menu->node_list != NULL)
	{
		//If there is only one item
		if(menu->highlighted_node->prev == NULL && menu->highlighted_node->next == NULL)
		{
			free(menu->highlighted_node);
			menu->highlighted_node = NULL;
			menu->node_list = NULL;
		}
		//Deleting head
		else if (menu->highlighted_node->prev == NULL)
		{
			menu->node_list = menu->highlighted_node->next;
			free(menu->highlighted_node);
			menu->node_list->prev = NULL;
			menu->highlighted_node = menu->node_list;
		}
		//Deleting tail
		else if(menu->highlighted_node->next == NULL)
		{
			menu->highlighted_node = menu->highlighted_node->prev;
			free(menu->highlighted_node->next);
			menu->highlighted_node->next = NULL;
		}
		//Otherwise
		else
		{
			struct Node *temp = menu->highlighted_node;
			menu->highlighted_node->next->prev = menu->highlighted_node->prev;
			menu->highlighted_node->prev->next = menu->highlighted_node->next;
			menu->highlighted_node = menu->highlighted_node->prev;
			free(temp);
		}
	}
	menu->highlight(menu, MENU_CURRENT);

	return 0;
}
