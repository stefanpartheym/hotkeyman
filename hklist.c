/*******************************************************************************
 * hklist -- simple hotkey-list implementation in C
 * 
 *  Author: Stefan Partheymüller
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "hklist.h"

// -----------------------------------------------------------------------------
// create list 'head'-item
// -----------------------------------------------------------------------------
hklist* hklist_create(unsigned char ishead)
{
	hklist* item	= malloc(sizeof(hklist));
	item->ishead	= ishead;
	item->next		= NULL;
	item->id		= -1;
	item->command	= NULL;
	item->mod		= 0;
	item->vk		= 0;
	
	return item;
}

// -----------------------------------------------------------------------------
// append item at the end of the list
// -----------------------------------------------------------------------------
hklist* hklist_append(hklist* head)
{
	hklist* current_item = head;
	// get the last item in the list
	while (current_item->next)
		current_item = current_item->next;
	
	current_item->next = hklist_create(0);
	return current_item->next;
}

// -----------------------------------------------------------------------------
// set item attributes
// -----------------------------------------------------------------------------
void hklist_set_item(	hklist* item, int id, char* command, unsigned int mod,
						unsigned int vk)
{
	// copy command string
	char* command_str = malloc( sizeof(char) * (strlen(command) + 1) );
	command_str[0]	  = '\0';
	strcpy(command_str, command);
	// set attributes
	item->command = command_str;
	item->id	  = id;
	item->mod	  = mod;
	item->vk	  = vk;
}

// -----------------------------------------------------------------------------
// remove 'next'-member
// -----------------------------------------------------------------------------
hklist* hklist_remove_next_item(hklist* parent)
{
	hklist* temp = parent->next;
	parent->next = parent->next->next;
	if (temp->command != NULL)
		free(temp->command);
	free(temp);
	return parent;
}

// -----------------------------------------------------------------------------
// destroy list with all its items
// -----------------------------------------------------------------------------
void hklist_destroy(hklist* head)
{
	// free all items
	while (head->next != NULL)
		hklist_remove_next_item(head);
	// free head item
	if (head->command != NULL)
		free(head->command);
	free(head);
}
