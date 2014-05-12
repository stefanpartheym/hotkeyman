#include <stdlib.h>
#include <stdbool.h>
#include "hotkeyman.h"


// #############################################################################
// Declarations
// #############################################################################

const char* error_message;
int error_code;

// -----------------------------------------------------------------------------
// Main function
// -----------------------------------------------------------------------------
void set_error(int code, const char* message)
{
    error_code    = code;
    error_message = message;
}

// -----------------------------------------------------------------------------
// Main function
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    set_error(0, NULL); // initialize error flag
    
	// create hotkey manager
    HotkeyManager* hkman = hotkeymanager_create();
    hotkeymanager_append_default_hotkeys(hkman);
    
	// read hotkeys
	if (!hotkeymanager_read_hotkeys_form_file(hkman))
        set_error(1, "Failed to read hotkey definitions from file!");
    else
    {
        // register hotkeys
        if (!hotkeymanager_register_hotkeys(hkman))
            set_error(1, "Failed to register hotkeys!");
        else
        {
            hotkeymanager_process_hotkeys(hkman); // process hotkeys
            
            // unregister hotkeys
            if (!hotkeymanager_unregister_hotkeys(hkman))
                set_error(1, "Failed to unregister hotkeys!");
        }
    }
    
    // Print errors
    if (error_code != 0)
    {
        hklog("ERROR: %s\n", error_message);
		hklog(HK_ERR_TERMINATE);
    }
    
    // always free hotkey manager
	hotkeymanager_free(hkman);
    
	return error_code;
}
