/*
 *
 * $Id: hmouse.c 1.1 2023/14/08 17:08:50 chry Exp $
 * $Locker: $
 *
 * Mouse and poineter setup (brender fork)
 */
#include "host.h"

#ifdef __WIN_32__
#include <Windows.h>

 // custom id of the window status
#define GWLP_WINDOWSTATUS 0

 // System mouse cursor show count
br_uint_8 show_count = 0;

// Copy of Windows mouse hide/show state
br_uint_8 cursor_state = 0;

static HCURSOR cursor_shapes[BR_POINTER_MAX] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL/*, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL*/
};

void BR_RESIDENT_ENTRY HostMouseShow(void)
{
	show_count++;

	if (show_count == 1 && !cursor_state)
	{
		cursor_state = 1;
		ShowCursor(TRUE);
	}
}

void BR_RESIDENT_ENTRY HostMouseHide(void)
{
	show_count--;

	if (show_count == 0 && cursor_state)
	{
		cursor_state = 0;
		ShowCursor(FALSE);
	}
}

void BR_RESIDENT_ENTRY HostMouseConstrain(br_window* w)
{
	host_window_status* status = GetWindowLongPtr((HWND)w, GWLP_WINDOWSTATUS);
	status->constrain_state = 1;

	if (status->current_display_mode == BR_MODE_WINDOW)
	{
		status->constrain_request = 1;
	}
}
void BR_RESIDENT_ENTRY HostMouseUnconstrain(br_window* w)
{
	host_window_status* status = GetWindowLongPtr((HWND)w, GWLP_WINDOWSTATUS);
	status->constrain_state = 0;

	if (status->current_display_mode == BR_MODE_WINDOW)
	{
		ClipCursor(&status->unconstrained_rect);
		status->constrain_request = 0;
	}
}

void BR_RESIDENT_ENTRY HostMouseGetPosition(br_int_32* x, br_int_32* y)
{
	POINT point;
	if (GetCursorPos(&point))
	{
		*x = point.x;
		*y = point.y;
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

void BR_RESIDENT_ENTRY HostMouseSetPosition(br_int_32 x, br_int_32 y)
{
	SetCursorPos(x, y);
}

// Cursor service (brender fork)

void BR_RESIDENT_ENTRY HostPointerSetShape(br_uint_32 id)
{
	if (id < BR_POINTER_MAX && cursor_shapes[id])
		SetCursor(cursor_shapes[id]);
	else
		SetCursor(NULL);
}

void BR_RESIDENT_ENTRY HostPointerLoadShapes(void)
{
	cursor_shapes[BR_POINTER_ARROW] = LoadCursor(NULL, IDC_ARROW);
	cursor_shapes[BR_POINTER_HAND] = LoadCursor(NULL, IDC_HAND);
	cursor_shapes[BR_POINTER_WAIT] = LoadCursor(NULL, IDC_WAIT);
	cursor_shapes[BR_POINTER_CROSS] = LoadCursor(NULL, IDC_CROSS);
	//cursor_shapes[BR_POINTER_SQUARE] = LoadCursor(NULL, IDC_SQUARE);
}

#endif
