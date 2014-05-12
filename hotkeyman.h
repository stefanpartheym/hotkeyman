/*******************************************************************************
 * hotkeyman -- managing hotkeys in windows
 ******************************************************************************/

#ifndef HOTKEYMAN_H
#define HOTKEYMAN_H

#include <stdbool.h>
#include "hklist.h"

#define HK_ERR_TERMINATE "-> Terminating hotkeymanager ...\n"


typedef struct HotkeyManager HotkeyManager;

HotkeyManager* hotkeymanager_create();
void hotkeymanager_free(HotkeyManager* hkman);
void hotkeymanager_process_hotkeys(HotkeyManager* hkman);
bool hotkeymanager_handle_hotkey(HotkeyManager* hkman, int hotkey_id,
                                 char* command);

bool hotkeymanager_register_hotkeys(HotkeyManager* hkman);
void hotkeymanager_append_default_hotkeys(HotkeyManager* hkman);
bool hotkeymanager_unregister_hotkeys(HotkeyManager* hkman);
bool hotkeymanager_refresh_hotkeys(HotkeyManager* hkman);
bool hotkeymanager_read_hotkeys_form_file(HotkeyManager* hkman);

void hklog(const char* format, ...);

#endif // HOTKEYMAN_H
