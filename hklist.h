/*******************************************************************************
 * hklist -- simple hotkey-list implementation in C
 * 
 *	USAGE:
 *	-	Use hklist_create() to create the 'head' (first) item of the list.
 *	-	Use hklist_append() to append an item at the end of a list.
 *	-	Use hklist_set_item() to set attributes, since it provides "memory-safe"
 *		assignment of the 'command'-attribute.
 *	-	Use hklist_destroy() to destroy the complete list with all its items.
 *		Unlike hklist_append(), for this function it is important to pass the
 *		'head'-item of the list to remove ALL items from memory. Otherwise, only
 *		items beginning at the right from the passed item will be removed.
 * 
 *  Author: Stefan Partheymüller
 ******************************************************************************/

#ifndef HKLIST_H
#define HKLIST_H

typedef struct hklist_t
{
	unsigned char ishead;
	int id;
	char* command;
	unsigned int mod;
	unsigned int vk;
	struct hklist_t* next;
} hklist;

// create new hotkey list item
hklist* hklist_create(unsigned char ishead);
// append item to a list
hklist* hklist_append(hklist* head);
// set item attributes
void hklist_set_item(hklist* item, int id, char* command, unsigned int mod,
					 unsigned int vk);
// remove item on the right
hklist* hklist_remove_next_item(hklist* parent);
// destroy list
void hklist_destroy(hklist* head);

#endif // HKLIST_H
