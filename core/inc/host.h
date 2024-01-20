/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: host.h 1.1 1997/12/10 16:41:13 jon Exp $
 * $Locker: $
 */
#ifndef _HOST_H_
#define _HOST_H_

#ifndef _BRENDER_H_
#include "brender.h"
#endif

/*
 * Structure used to describe host information
 */
typedef struct host_info {
	br_uint_32 size;
	char identifier[40];
	br_uint_32 capabilities;
	br_token processor_family;
	br_token processor_type;
} host_info;

#define HOST_CAPS_REAL_MEMORY			0x00000001	/* Can allocate/read/write real-mode mem.	*/
#define HOST_CAPS_REAL_INT_CALL			0x00000002	/* Can invoke real mode interrupts			*/
#define HOST_CAPS_REAL_INT_HOOK			0x00000004	/* Can hook real mode interrupts			*/
#define HOST_CAPS_PROTECTED_INT_CALL	0x00000008	/* Can invoke protected mode interrupts		*/
#define HOST_CAPS_PROTECTED_INT_HOOK	0x00000010	/* Can hook prot. mode interrupts			*/
#define HOST_CAPS_ALLOC_SELECTORS		0x00000020	/* Can allocate new selectors				*/
#define HOST_CAPS_PHYSICAL_MAP			0x00000040	/* Can map physical memory -> linear		*/
#define HOST_CAPS_EXCEPTION_HOOK		0x00000080	/* Can hook exceptions						*/
#define HOST_CAPS_BASE_SELECTORS_WRITE	0x00000100	/* Can modify base/limit of cs,ds,es,ss selectors	*/
#define HOST_CAPS_PORTS					0x00000200	/* Can use IO ports							*/
#define HOST_CAPS_MMX					0x00000400	/* Has MMX extensions						*/
#define HOST_CAPS_FPU					0x00000800	/* Has hardware FPU							*/
#define HOST_CAPS_CMOV                  0x00001000  /* Has CMOV extensions */


/*
 * Aggregate used to represent a block of real-mode memory
 */
typedef struct host_real_memory {
	br_uint_32 pm_off;
	br_uint_16 pm_seg;

	br_uint_16 _reserved;

	br_uint_16 rm_off;
	br_uint_16 rm_seg;
} host_real_memory;

/*
 * Structure for passing register sets around - 'happens' to be
 * the same as DPMI
 */
typedef union host_regs {
	struct {
		br_uint_32 edi;
		br_uint_32 esi;
		br_uint_32 ebp;
		br_uint_32 _res;
		br_uint_32 ebx;
		br_uint_32 edx;
		br_uint_32 ecx;
		br_uint_32 eax;

		br_uint_16 flags;
		br_uint_16 es;
		br_uint_16 ds;
		br_uint_16 fs;
		br_uint_16 gs;
		br_uint_16 ip;
		br_uint_16 cs;
		br_uint_16 sp;
		br_uint_16 ss;
	} x;
	struct {
		br_uint_16 di, _pad0;
		br_uint_16 si, _pad1;
		br_uint_16 bp, _pad2;
		br_uint_16 _res,_pad3;
		br_uint_16 bx, _pad4;
		br_uint_16 dx, _pad5;
		br_uint_16 cx, _pad6;
		br_uint_16 ax, _pad7;

		br_uint_16 flags;
		br_uint_16 es;
		br_uint_16 ds;
		br_uint_16 fs;
		br_uint_16 gs;
		br_uint_16 ip;
		br_uint_16 cs;
		br_uint_16 sp;
		br_uint_16 ss;
	} w;
	struct {
		br_uint_32 _pad0[4];
		br_uint_8 bl, bh, _pad1, _pad2;
		br_uint_8 dl, dh, _pad3, _pad4;
		br_uint_8 cl, ch, _pad5, _pad6;
		br_uint_8 al, ah, _pad7, _pad8;
  } h;
} host_regs;

enum host_flags {
	HOST_FLAG_CARRY		=0x0001,
	HOST_FLAG_PARITY	=0x0004,
	HOST_FLAG_AUX_CARRY	=0x0010,
	HOST_FLAG_ZERO		=0x0040,
	HOST_FLAG_SIGN		=0x0080
};

/*
 * Structures use to hook interrupts and exceptions
 */
typedef struct host_interrupt_hook {
		br_uint_32	old_offset;
		br_boolean	active;
		br_uint_8	vector;
		br_uint_16	old_sel;
} host_interrupt_hook ;

typedef struct host_exception_hook {
		br_uint_8	exception;
		br_boolean	active;

		br_uint_32	old_offset;
		br_uint_16	old_sel;

		br_uint_8	scratch[256];
} host_exception_hook ;

/*
 * Types of config. string supported
 */
enum config_types {
		HOST_CS_NONE,
		HOST_CS_DRIVERS,
		HOST_CS_OUTPUT_TYPE,
		HOST_CS_SEARCH_PATH
};

/*
* Native window type (brender fork)
*/
typedef void* br_window;

/*
* Native instance type (brender fork)
*/
typedef void* br_ninstance;

enum sal_window_mode {
	BR_MODE_FULLSCREEN, // Set fullscreen mode
	BR_MODE_WINDOW, // Set windowed mode
	BR_MODE_DESKTOP, // Use current desktop mode in fullscreen, ignoring X and Y size parameters
};

/*
* Window flags (brender fork)
*/
enum window_flags {
	BR_WINDOW_FLAG_NONE						=0x0000,
	BR_WINDOW_FLAG_RESIZABLE				=0x0002,
	BR_WINDOW_FLAG_MINIMIZABLE				=0x0004,
	BR_WINDOW_FLAG_POPUP					=0x0008,
	BR_WINDOW_FLAG_PREVENT_ALT_MENU_POPUP	=0x0010,
	BR_WINDOW_FLAG_ENABLE_MODE_TOGGLE		=0x0020,
	BR_WINDOW_FLAG_REFRESH_WHILE_SLEEP		=0x0040,
};

/*
* Window states (brender fork)
*/
enum window_state_types {
	BR_WINDOW_STATE_NORMAL,
	BR_WINDOW_STATE_MAXIMIZED,
	BR_WINDOW_STATE_MINIMIZED,
	BR_WINDOW_STATE_HIDDEN,
};

/*
* Pointer types (brender fork)
*/
enum pointer_shape_types {
	/* please note that not all cursors are available on all platforms */

	BR_POINTER_NONE,
	BR_POINTER_ARROW,
	BR_POINTER_HAND,
	BR_POINTER_WAIT,
	BR_POINTER_CROSS,
	BR_POINTER_SQUARE,

	BR_POINTER_CUSTOM = 10,
	BR_POINTER_MAX = 20,
};

typedef struct host_window_callbacks
{
	// called when the window changes the focus
	void (BR_CALLBACK* on_focus)(br_boolean status);

	// called when the window is destroyed
	void (BR_CALLBACK* on_exit)(void);

	// called when the window is being created
	br_boolean (BR_CALLBACK* on_create)(void);

	// called when a key is pressed
	void (BR_CALLBACK* on_key_down)(br_uint_16 k, br_uint_8 mod);

	// called when a key is released
	void (BR_CALLBACK* on_key_up)(br_uint_16 k, br_uint_8 mod);

	// called when there's a text input (UTF-16 codepoint)
	void (BR_CALLBACK* on_text)(br_uint_16 k);

	// called when a mouse button is pressed
	void (BR_CALLBACK* on_mouse_key_down)(br_uint_8 k);

	// called when a mouse button is released
	void (BR_CALLBACK* on_mouse_key_up)(br_uint_8 k);

	// called when the mouse is moved 
	void (BR_CALLBACK* on_mouse_move)(br_int_32 x, br_int_32 y);

	// TODO: joystick callbacks

} host_window_callbacks;

/*
* Window status/config (brender fork)
*/
typedef struct host_window_status
{
	// Default area of mouse constraint
	br_rectangle unconstrained_rect;

	// 1 if mouse limited to window area
	br_boolean constrain_state;

	// 1 if mouse should be constrained at next movement
	br_boolean constrain_request;

	// BR_TRUE if this SAL app has input focus
	br_boolean app_active;

	// TRUE to ignore WM_WINDOWPOSCHANGED messages
	br_boolean WPS_lock;

	// App not minimized by default
	br_boolean app_minimized;

	// BR_TRUE to toggle window/fullscreen
	br_boolean mode_change_request;

	// BR_MODE_FULLSCREEN / BR_MODE_WINDOW / BR_MODE_DESKTOP
	br_uint_8 current_display_mode;

	// Window flags
	br_uint_16 flags;

	// list of callbacks to call when something happens
	host_window_callbacks callbacks;

	// current width
	br_int_32 current_width;

	// current height
	br_int_32 current_height;

#if __WIN_32__
	// Custom window procedure
	void* window_proc;
#endif

} host_window_status ;

/*
* Window initialization info (brender fork)
*/
typedef struct host_window_init_info
{
	// native instnace
	br_ninstance instance;

	// title of the window
	char* window_title;

	// window rect (size+pos)
	br_rectangle rect;

	// window flags, see "window_flags"
	br_uint_16 flags;

	// window mode, see "sal_window_mode"
	br_uint_8 mode;

	// window state, see "window_state_types"
	br_uint_8 state;

	// parent of the window
	br_window parent;

	// menu of the window
	//br_menuw menu;

	// list of callbacks to call when something happens
	host_window_callbacks callbacks;
} host_window_init_info ;

#ifndef _HOST_P_H_
#include "host_p.h"
#endif

#endif

