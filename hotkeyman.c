/*******************************************************************************
 * hotkeyman -- managing hotkeys in windows
 ******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hotkeyman.h"

#define HK_ID_QUIT 1
#define HK_ID_REFRESH 2
#define HK_CONF_FILENAME "hotkeyman.conf"
#define HK_LOG_FILENAME "hotkeyman.log"

#define HK_ERR_EXIT "error occurred\n-> terminating program ...\n"

int main(int argc, char* argv[])
{
	bool quit = false;
	// create hotkey list
	hklist* head = hklist_create(1);
	int last_hkid = set_default_hotkeys(head);
	// read hotkeys
	last_hkid = read_hotkeys_form_file(head, HK_CONF_FILENAME, last_hkid);
	// check for errors
	if (last_hkid == -1)
	{
		hklist_destroy(head);
		hklog(HK_ERR_EXIT);
		return 1;
	}
	// register hotkeys
	register_hotkeys(head);
	
	// wait for hotkeys
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
		if (msg.message == WM_HOTKEY)
		{
			int i = 0;
			hklist* current_item = head;
			
			hklog("hotkey triggered (id: %d)\n", msg.wParam);
			// iterate over list of registered hotkeys
			do
			{
				if (msg.wParam == current_item->id)
				{
					if (current_item->id == HK_ID_QUIT)
					{
						// quit hotkeyman
						quit = true;
						hklog("exit hotkeyman ...\n");
					}
					else if (current_item->id == HK_ID_REFRESH)
					{
						// refresh hotkeys
						hklog("refreshing hotkeys ...\n");
						head = refresh_hotkeys(head);
						// check for errors
						if (!head)
						{
							hklog(HK_ERR_EXIT);
							quit = true;
							break;
						}
					}
					else
					{
						// run command
						system(current_item->command);
						hklog("running command: '%s'\n", current_item->command);
					}
					// appropriate command found -> break
					break;
				}
				// get next item
				current_item = current_item->next;
			} while (current_item);
		}
		// exit hotkeyman when quit flag is true
		if (quit)
			break;
    }
	// unregister hotkeys
	unregister_hotkeys(head);
	// destroy list
	hklist_destroy(head);
	
	return 0;
}

// -----------------------------------------------------------------------------
// register hotkeys
// -----------------------------------------------------------------------------
bool register_hotkeys(hklist* list)
{
	hklist* item = list;
	do
	{
		// ignore hotkeys with id = -1
		if (item->id != -1)
		{
			// register hotkey
			if (RegisterHotKey(NULL, item->id, item->mod, item->vk))
				hklog("successfully registered hotkey (id: %d)\n", item->id);
			else
			{
				hklog("WARNING: unable to register hotkey (id: %d)\n", item->id);
				return false;
			}
		}
		// get next item
		item = item->next;
	} while (item);
	
	return true;
}

// -----------------------------------------------------------------------------
// set default hotkeys
// -----------------------------------------------------------------------------
int set_default_hotkeys(hklist* item)
{
	hklist_set_item(item, HK_ID_QUIT, "", MOD_ALT | MOD_CONTROL, 'Q');
	item = hklist_append(item);
	hklist_set_item(item, HK_ID_REFRESH, "", MOD_ALT | MOD_CONTROL, 'R');
	
	return HK_ID_REFRESH;
}

// -----------------------------------------------------------------------------
// unregister hotkeys
// -----------------------------------------------------------------------------
bool unregister_hotkeys(hklist* list)
{
	hklist* item = list;
	do
	{
		if (!UnregisterHotKey(NULL, item->id))
			return false;
		// get next item
		item = item->next;
	} while (item);
	
	return true;
}

// -----------------------------------------------------------------------------
// refresh hotkeys
// -----------------------------------------------------------------------------
hklist* refresh_hotkeys(hklist* list)
{
	unregister_hotkeys(list);
	hklist_destroy(list);
	list = hklist_create(1);
	int last_id = set_default_hotkeys(list);
	if (read_hotkeys_form_file(list, HK_CONF_FILENAME, last_id) == -1)
		// error occurred
		return NULL;
	
	register_hotkeys(list);
	
	return list;
}

// -----------------------------------------------------------------------------
// read hotkeys from hotkeyman configuration file.
// default: hotkeyman.conf
// -----------------------------------------------------------------------------
int read_hotkeys_form_file(hklist* head, const char* file_name, int last_hkid)
{
	FILE* hk_file	= fopen(file_name, "r");
	char* key		= malloc(sizeof(char) * 512);
	char* value		= malloc(sizeof(char) * 512);
	hklist* item	= NULL;
	bool new_stmt	= true;
		
	while (fscanf(hk_file, " %[^= ] = \"%[^\"]\" ", key, value) == 2)
	{
		// check for new statement
		if (new_stmt)
		{
			// new statement appeard -> append new hotkey-list item
			item = hklist_append(head);
			// current statement is active -> no new statement is expected
			new_stmt = false;
			last_hkid++;
			item->id = last_hkid;
		}
		
		if (strcmp("cmd", key) == 0)
			// set command
			hklist_set_item_command_attrib(item, value);
		else if (strcmp("keys", key) == 0)
		{
			unsigned int mod1 = 0;
			unsigned int mod2 = 0;
			unsigned int mod3 = 0;
			char vk = -1;
			// parse value
			if (sscanf(value, "%x %x %x %c", &mod1, &mod2, &mod3, &vk) == 4)
			{
				// set keys
				item->mod = mod1 | mod2 | mod3;
				item->vk = vk;
			}
			else
			{
				hklog(	"ERROR: value of key '%s' does not fit the specified"\
						" format!\n");
				return -1;
			}
		}
		else
			hklog("WARNING: the key '%s' is not specified!\n", key);
		
		// remember current position
		fpos_t temp;
		fgetpos(hk_file, &temp);
		if (fgetc(hk_file) == ';')
			// end of statement reached -> new statement is expected
			new_stmt = true;
		else
			// restore position
			fsetpos(hk_file, &temp);
	}
	
	free(key);
	free(value);
	fclose(hk_file);
	
	return last_hkid;
}

// -----------------------------------------------------------------------------
// write log message
// -----------------------------------------------------------------------------
void hklog(const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
#ifdef DEBUG
	// actual print
	vprintf(format, arglist);
#endif
	// log to file
	FILE* log_file = fopen(HK_LOG_FILENAME, "a+");
	vfprintf(log_file, format, arglist);
	fclose(log_file);
	
	va_end(arglist);
}
