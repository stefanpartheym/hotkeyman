/*******************************************************************************
 * hotkeyman -- managing hotkeys in windows
 * 
 *  Author: Stefan Partheymüller
 ******************************************************************************/

#ifndef HOTKEYMAN_H
#define HOTKEYMAN_H

#include "hklist.h"

bool register_hotkeys(hklist* list);
int set_default_hotkeys(hklist* item);
bool unregister_hotkeys(hklist* list);
hklist* refresh_hotkeys(hklist* list);
int read_hotkeys_form_file(hklist* head, const char* file_name, int last_hkid);
void hklog(const char* format, ...);

#endif // HOTKEYMAN_H
