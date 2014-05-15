/*******************************************************************************
 * Hotkey utilities
 ******************************************************************************/

#ifndef HKUTILS_H
#define HKUTILS_H


void hklog(const char* format, ...);
#ifndef _PLAT_WNDS
unsigned int hkconvert(unsigned int modifiers);
#endif // not _PLAT_WNDS


#endif // HKUTILS_H
