/*
 *
 * $Id: hstvk.c 1.0 1997/12/11 14:35:02 chry Exp $
 * $Locker: $
 *
 * Virtual keys conversion (brender fork)
 */
#include "host.h"
#include "hstvk.h"

#ifdef __WIN_32__

// map of BR virtual keys from Windows VKs
// see https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
static br_uint_16 WindowsVKs[0xFF] = {
	BR_KEY_NONE,
	BR_KEY_NONE, // mouse-l
	BR_KEY_NONE, // mouse-r
	BR_KEY_NONE, // ctrl+c
};

br_uint_16 BR_FORCEINLINE HostVirtualKeyToBrKey(br_int_32 vk);
br_uint_8 BR_FORCEINLINE HostVirtualKeyToBrMod(br_int_32 vk);

#endif
