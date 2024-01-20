/*
 *
 * $Id: brkeycodes.h 1.0 2023/17/08 12:40:06 chry Exp $
 * $Locker: $
 *
 * Key code definitions (brender fork)
 */
#ifndef _BRKEYCODES_H_
#define _BRKEYCODES_H_

enum br_mouse_codes
{
	BR_MOUSE_BUTTON_LEFT,
	BR_MOUSE_BUTTON_RIGHT,
	BR_MOUSE_BUTTON_MIDDLE,
	BR_MOUSE_BUTTON_X1,
	BR_MOUSE_BUTTON_X2
};

enum br_key_codes
{
	BR_KEY_NONE, // null key

	// standard
	BR_KEY_BACKSPACE = '\b',
	BR_KEY_TAB = '\t',
	BR_KEY_SPACEBAR = ' ',
	BR_KEY_ENTER = '\n',

	// numbers
	BR_KEY_0 = '0',
	BR_KEY_1 = '1',
	BR_KEY_2 = '2',
	BR_KEY_3 = '3',
	BR_KEY_4 = '4',
	BR_KEY_5 = '5',
	BR_KEY_6 = '6',
	BR_KEY_7 = '7',
	BR_KEY_8 = '8',
	BR_KEY_9 = '9',

	// chars

	// special
	BR_KEY_CLEAR = 0x100,
	BR_KEY_PAUSE,
	BR_KEY_CANC,
	BR_KEY_CAPSLOCK,
	BR_KEY_ESC,
	BR_KEY_END,
	BR_KEY_SELECT,
	BR_KEY_PRINT,
	BR_KEY_EXECUTE,
	BR_KEY_INSERT,
	BR_KEY_DELETE,
	BR_KEY_PRINTSCR,
	BR_KEY_NUMLOCK,
	BR_KEY_SCROLLLOCK,
	BR_KEY_HOME,
	BR_KEY_HELP,
	BR_KEY_PG_UP,
	BR_KEY_PG_DOWN,
	BR_KEY_UP,
	BR_KEY_DOWN,
	BR_KEY_LEFT,
	BR_KEY_RIGHT,

	// IME
	BR_KEY_IME_KANA = 0x200,
	BR_KEY_IME_HANGUL,
	BR_KEY_IME_ON,
	BR_KEY_IME_JUNJA,
	BR_KEY_IME_FINAL,
	BR_KEY_IME_HANJA,
	BR_KEY_IME_OFF,
	BR_KEY_IME_ACCEPT,
	BR_KEY_IME_MODECHANGE,
};

enum br_mod_codes
{
	BR_MOD_LSHIFT = 1 << 0,
	BR_MOD_RSHIFT = 1 << 1,
	BR_MOD_LCTRL = 1 << 2,
	BR_MOD_RCTRL = 1 << 3,
	BR_MOD_LALT = 1 << 4,
	BR_MOD_RALT = 1 << 5,
};

#endif
