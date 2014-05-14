/*******************************************************************************
 * hotkeyman
 ******************************************************************************/

#ifdef _PLAT_WNDS
#include <windows.h>
#else
#include "xhklib.h"
#include "wnds_keydefs.h"
#endif // _PLAT_WNDS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "hotkeyman.h"

#define HK_CONF_FILENAME "hotkeyman.conf"
#define HK_LOG_FILENAME  "hotkeyman.log"


// #############################################################################
// Declarations
// #############################################################################

// Hotkey manager
struct HotkeyManager
{
    hklist* hotkeys;
    const char* config_file_name;
    int current_id;
    
#ifndef _PLAT_WNDS
    xhkConfig* hkconfig;
#endif // not _PLAT_WNDS
    
    bool quit_flag;
    
    int hk_id_quit;
    int hk_id_refresh;
};


// #############################################################################
// Interface functions
// #############################################################################

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
HotkeyManager* hotkeymanager_create()
{
    HotkeyManager* hkman    = (HotkeyManager*) malloc(sizeof(HotkeyManager));
    hkman->hotkeys          = hklist_create(1);
    hkman->config_file_name = HK_CONF_FILENAME;
    hkman->current_id       = 0;
#ifndef _PLAT_WNDS
    hkman->hkconfig         = xhkInit(NULL);
#endif // not _PLAT_WNDS
    hkman->quit_flag        = false;
    hkman->hk_id_quit       = -1;
    hkman->hk_id_refresh    = -1;
    
    return hkman;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
void hotkeymanager_free(HotkeyManager* hkman)
{
#ifndef _PLAT_WNDS
    xhkClose(hkman->hkconfig);
#endif // not _PLAT_WNDS
    hklist_destroy(hkman->hotkeys);
    free(hkman);
}

// -----------------------------------------------------------------------------
// Process hotkeys
// -----------------------------------------------------------------------------
void hotkeymanager_process_hotkeys_internal(HotkeyManager* hkman, int hotkey_id)
{
    int i = 0;
    hklist* current_item = hkman->hotkeys;
    
    hklog("Hotkey triggered (id: %d)\n", hotkey_id);
    // iterate over list of registered hotkeys
    do
    {
        if (hotkey_id == current_item->id)
        {
            hotkeymanager_handle_hotkey(hkman, current_item->id,
                                        current_item->command);
            // appropriate command found -> break
            break;
        }
        // get next item
        current_item = current_item->next;
    } while (current_item);
}

// -----------------------------------------------------------------------------
// Process hotkeys
// -----------------------------------------------------------------------------
void hotkeymanager_process_hotkeys(HotkeyManager* hkman)
{
#ifdef _PLAT_WNDS
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_HOTKEY)
            hotkeymanager_process_hotkeys_internal(hkman, msg.WParam);
        
        // exit hotkeyman if quit flag is true
        if (hkman->quit_flag)
            break;
    }
#else
    while (true)
    {
        xhkPollKeys(hkman->hkconfig, 1);
        if (hkman->quit_flag)
            break;
    }
#endif // _PLAT_WNDS
}

// -----------------------------------------------------------------------------
// Handle hotkey
// -----------------------------------------------------------------------------
bool hotkeymanager_handle_hotkey(HotkeyManager* hkman, int hotkey_id,
                                 char* command)
{
    if (hotkey_id == hkman->hk_id_quit)
    {
        // quit hotkeyman
        hkman->quit_flag = true;
        hklog("Exit hotkeyman ...\n");
    }
    else if (hotkey_id == hkman->hk_id_refresh)
    {
        // refresh hotkeys
        hklog("Refreshing hotkeys ...\n");
        if (!hotkeymanager_refresh_hotkeys(hkman))
        {
            hklog("ERROR: Failed to refresh hotkeys!\n");
            hklog(HK_ERR_TERMINATE);
            hkman->quit_flag = true;
            return false;
        }
    }
    else
    {
        hklog("Running command: '%s'\n", command);
        
#ifdef _PLAT_WNDS
        PROCESS_INFORMATION proc_info;
        STARTUPINFO startup_info;
        // initialize struct with 0
        memset(&startup_info, 0, sizeof(STARTUPINFO));
        // run command as new process
        if (!CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL,
                           &startup_info, &proc_info))
            hklog("ERROR: Creating process failed! (code: %d)\n", GetLastError());
#else
        int exit_code = system(command);
        if (exit_code != 0)
            hklog("ERROR: Creating process failed! (code: %d)\n", exit_code);
#endif // _PLAT_WNDS
    }
    
    return true;
}

#ifndef _PLAT_WNDS
// -----------------------------------------------------------------------------
// Callback: Handle keypress in X11
// -----------------------------------------------------------------------------
void hotkeymanager_handle_hotkey_cb(xhkEvent event, void* arg1, void* arg2,
                                    void* arg3)
{
    HotkeyManager* hkman = (HotkeyManager*) arg1;
    int id               = (int) arg2;
    
    hotkeymanager_process_hotkeys_internal(hkman, id);
}
#endif // not _PLAT_WNDS

// -----------------------------------------------------------------------------
// Register hotkeys
// -----------------------------------------------------------------------------
bool hotkeymanager_register_hotkeys(HotkeyManager* hkman)
{
    hklist* item = hkman->hotkeys;
    do
    {
        // ignore hotkeys with id = -1
        if (item->id != -1)
        {
            // register hotkey
#ifdef _PLAT_WNDS
            if (RegisterHotKey(NULL, item->id, item->mod, item->vk))
#else
            if (xhkBindKey(hkman->hkconfig, 0, item->vk, hkconvert(item->mod),
                           xhkKeyPress, &hotkeymanager_handle_hotkey_cb, hkman,
                           (void*) item->id, 0) == 0)
#endif // _PLAT_WNDS
                hklog("Successfully registered hotkey (id: %d)\n", item->id);
            else
            {
                hklog("WARNING: Unable to register hotkey (id: %d)\n", item->id);
                return false;
            }
        }
        // get next item
        item = item->next;
    } while (item);
    
    return true;
}

// -----------------------------------------------------------------------------
// Set default hotkeys
// -----------------------------------------------------------------------------
void hotkeymanager_append_default_hotkeys(HotkeyManager* hkman)
{
    int new_id   = hkman->current_id++;
    hklist* item = hklist_append(hkman->hotkeys);
    hklist_set_item(item, new_id, "", MOD_ALT | MOD_SHIFT | MOD_CONTROL, 'Q');
    hkman->hk_id_quit = new_id;
    
    new_id = hkman->current_id++;
    item   = hklist_append(item);
    hklist_set_item(item, new_id, "", MOD_ALT | MOD_CONTROL, 'R');
    hkman->hk_id_refresh = new_id;
}

// -----------------------------------------------------------------------------
// Unregister hotkeys
// -----------------------------------------------------------------------------
bool hotkeymanager_unregister_hotkeys(HotkeyManager* hkman)
{
    hklist* item = hkman->hotkeys->next; // get first item
    while (item)
    {
#ifdef _PLAT_WNDS
        if (!UnregisterHotKey(NULL, item->id))
#else
        if (xhkUnBindKey(hkman->hkconfig, 0, item->vk, hkconvert(item->mod),
                         xhkKeyPress) == -1)
#endif // _PLAT_WNDS
            return false;
        else
            hklog("Successfully unregistered hotkey (id: %d)\n", item->id);
        // get next item
        item = item->next;
    }
    
    return true;
}

// -----------------------------------------------------------------------------
// Refresh hotkeys
// -----------------------------------------------------------------------------
bool hotkeymanager_refresh_hotkeys(HotkeyManager* hkman)
{
    if (!hotkeymanager_unregister_hotkeys(hkman))
    {
        hklog("ERROR: Failed to unregister hotkeys!\n");
        return false;
    }
    
    hklist_destroy(hkman->hotkeys);
    hkman->hotkeys       = hklist_create(1);
    hkman->current_id    = 0;
    hkman->hk_id_quit    = -1;
    hkman->hk_id_refresh = -1;
    
    hotkeymanager_append_default_hotkeys(hkman);
    
    if (hotkeymanager_read_hotkeys_form_file(hkman) &&
        hotkeymanager_register_hotkeys(hkman))
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------------
// Read hotkeys from hotkeyman configuration file.
// Default: hotkeyman.conf
// -----------------------------------------------------------------------------
bool hotkeymanager_read_hotkeys_form_file(HotkeyManager* hkman)
{
    FILE* hk_file = fopen(hkman->config_file_name, "r");
    char* key     = malloc(sizeof(char) * 512);
    char* value   = malloc(sizeof(char) * 512);
    hklist* item  = hkman->hotkeys;
    bool new_stmt = true;
    
    // strip trailing and leading whitespaces ('\n' and ' ')
    while (fscanf(hk_file, " \n %[^= ] = \"%[^\"]\" \n", key, value) == 2)
    {
        // check for new statement
        if (new_stmt)
        {
            // new statement appeard -> append new hotkey-list item
            item = hklist_append(item);
            // current statement is active -> no new statement is expected
            new_stmt = false;
            hkman->current_id++;
            item->id = hkman->current_id;
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
                item->vk  = vk;
            }
            else
            {
                hklog("ERROR: Value of key '%s' does not fit the specified"\
                      " format!\n");
                return false;
            }
        }
        else
            hklog("WARNING: The key '%s' is not specified!\n", key);
        
        // get next char
        char c = fgetc(hk_file);
        if (c == ';')
            // end of statement reached -> new statement is expected
            new_stmt = true;
        else
            // restore char (end of statement not yet reached)
            ungetc(c, hk_file);
    }
    
    free(key);
    free(value);
    fclose(hk_file);
    
    return true;
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

// -----------------------------------------------------------------------------
// Convert windows modifier keys to X11 keys
// -----------------------------------------------------------------------------
unsigned int hkconvert(unsigned int modifiers)
{
    unsigned int result = 0;
    
    if (modifiers & MOD_ALT)
    {
        result    |= Mod1Mask;
        modifiers &= ~MOD_ALT;
    }
    if (modifiers & MOD_CONTROL)
    {
        result    |= ControlMask;
        modifiers &= ~MOD_CONTROL;
    }
    if (modifiers & MOD_SHIFT)
    {
        result    |= ShiftMask;
        modifiers &= ~MOD_SHIFT;
    }
    if (modifiers & MOD_WIN)
    {
        result    |= Mod4Mask;
        modifiers &= ~MOD_WIN;
    }
    
    return result;
}
