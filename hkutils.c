/*******************************************************************************
 * Hotkey utilities
 ******************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _PLAT_WNDS
#include <X11/Xlib.h>
#include "wnds_keydefs.h"
#endif // not _PLAT_WNDS


// #############################################################################
// Declarations
// #############################################################################

static const char* HK_LOG_FILENAME = "hotkeyman.log";


// #############################################################################
// Interface functions
// #############################################################################

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

#ifndef _PLAT_WNDS
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
#endif // not _PLAT_WNDS
