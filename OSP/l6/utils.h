#ifndef __UTILS__
#define __UTILS__

struct Node
{
	struct Node *prev;
	struct Node *next;
	void *data;
};

void print_list(struct Node *head);

#endif