/*
 * reversed code, licensed under MIT license.
 *
 * $Id: winwrap.c 1.2 2023/06/15 19:21:05 Exp $
 * $Locker: $
 *
 * Windows app wrapper
 */
#include "brwrap.h"
#include "winincls.h"
#include "buffer.h"
#include <stdlib.h>
#include <commdlg.h>

#define MAX_CMDS 50
#define MAX_ARGV_BUFFER 255 /* example: not reversed */
#define MENU_ID_START 10000

/* common types */
static brw_application* wrapApp_S218856 = NULL;
static int appArgc_S21868 = 0;
static char appArgv_S21869[MAX_CMDS][MAX_ARGV_BUFFER];
static br_pixelmap* appPixelmap_S21860 = NULL;
static br_boolean appIsActive_S21855 = BR_FALSE;
static br_uint_16 eventQualifiers_S21862 = 0;
static HINSTANCE hinstanceApp_S21848 = NULL;
static HWND hwndApp_S21852 = NULL;
static const char* className_S21854 = "BRENDER";
static HDC hdcApp_S21851 = NULL;
static HPALETTE hpaletteApp_S21849 = NULL;
static HCURSOR currentCursorHandle = NULL;
static HPALETTE hpaletteOld_S21850 = NULL;
static br_int_32 clientWidth_S21857 = 450;
static br_int_32 clientHeight_S21858 = 350;
static br_boolean _captured = BR_FALSE;
static br_int_32 _saved_width = 0;
static br_int_32 _saved_height = 0;
static int currentPointer_S21863 = BRW_POINTER_NONE;
static br_boolean hiddenCursor_S21867 = BR_TRUE;
static HMENU hmenuAppMenu_S21853 = NULL;
static const char* wrapperMenuText_S21872 = "Display";

static brw_menu wrapperMenu_S21880[] =
{
	{.type = BRW_MENU_COMMAND, .flags = BRW_MENUF_CHECK, .text = "WinG", .command = 1001 },
	{.type = BRW_MENU_COMMAND, .flags = 0, .text = "DIBSectison", .command = 1002 },
	{.type = BRW_MENU_COMMAND, .flags = 0, .text = "StretchDIBits", .command = 1003 },
	{.type = BRW_MENU_COMMAND, .flags = 0, .text = "Dummy", .command = 1004 },
	{.type = BRW_MENU_SEPARATOR, .flags = 0, .text = NULL, .command = 0 },
	{.type = BRW_MENU_COMMAND, .flags = 0, .text = "&Force All", .command = 1005 },
	{.type = BRW_MENU_END, .flags = 0, .text = NULL, .command = 0 },
};

/* extern? */
HCURSOR cursorHandles[BRW_POINTER_MAX];
HCURSOR sysCursorHandle = NULL;
brwin_buffer* currentBuffer = NULL;
br_int_32 currentBufferType = 0;

/* prototypes */
void ProcessArguments(int argc, char** argv);
br_boolean IdleProcess();
void BufferEnd();
void BufferSet(br_int_32 width, br_int_32 height);
void AppRender();
void AppScreenUpdate(HDC hdc, br_vector4_i* unk);
void MouseCapture(HWND hwnd, br_boolean capture);
HPALETTE PaletteSetup();
void SendMouseEvent(br_uint_16 type, br_uint_32 pos);

static WINAPI AppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		eventQualifiers_S21862 |= BRW_QUAL_POINTER_1;
		if (!appIsActive_S21855)
			break;

		MouseCapture(hwnd, BR_TRUE);
		SendMouseEvent(BRW_EVENT_POINTER1_DOWN, (br_uint_32)lParam);
		return 0;
	case WM_LBUTTONUP:
		eventQualifiers_S21862 &= ~BRW_QUAL_POINTER_1;
		SendMouseEvent(BRW_EVENT_POINTER1_UP, (br_uint_32)lParam);
		MouseCapture(hwnd, BR_FALSE);
		return 0;

	case WM_RBUTTONDOWN:
		eventQualifiers_S21862 |= BRW_QUAL_POINTER_3;
		if (!appIsActive_S21855)
			break;

		MouseCapture(hwnd, BR_TRUE);
		SendMouseEvent(BRW_EVENT_POINTER3_DOWN, (br_uint_32)lParam);
		return 0;

	case WM_RBUTTONUP:
		eventQualifiers_S21862 &= ~BRW_QUAL_POINTER_3;
		SendMouseEvent(BRW_EVENT_POINTER3_UP, (br_uint_32)lParam);
		MouseCapture(hwnd, BR_FALSE);
		return 0;

	case WM_MBUTTONDOWN:
		eventQualifiers_S21862 |= BRW_QUAL_POINTER_2;
		if (!appIsActive_S21855)
			break;

		MouseCapture(hwnd, BR_TRUE);
		SendMouseEvent(BRW_EVENT_POINTER2_DOWN, (br_uint_32)lParam);
		return 0;

	case WM_MBUTTONUP:
		eventQualifiers_S21862 &= ~BRW_QUAL_POINTER_2;
		SendMouseEvent(BRW_EVENT_POINTER2_UP, (br_uint_32)lParam);
		MouseCapture(hwnd, BR_FALSE);
		return 0;

	case WM_QUERYNEWPALETTE:
		if (hpaletteApp_S21849)
		{
			SelectPalette(hdcApp_S21851, (HPALETTE)hpaletteApp_S21849, FALSE);
			RealizePalette(hdcApp_S21851);
		}
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	case WM_MOUSEFIRST:
		SetCursor(currentCursorHandle);
		if (eventQualifiers_S21862 & (BRW_QUAL_POINTER_1 | BRW_QUAL_POINTER_2 | BRW_QUAL_POINTER_3))
			SendMouseEvent(BRW_EVENT_KEY_DOWN, (br_uint_32)lParam);

		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) > MENU_ID_START)
		{
			brw_event evt;
			evt.qualifiers = eventQualifiers_S21862;
			evt.type = BRW_EVENT_COMMAND;
			evt.value_1 = LOWORD(wParam) - MENU_ID_START;
			evt.value_2 = 0;
			wrapApp_S218856->event(wrapApp_S218856, &evt);
		}
		else if (wParam == IDCANCEL)
			PostMessage(hwnd, WM_CLOSE, NULL, NULL);

		return 0;

	case WM_ACTIVATEAPP:
		appIsActive_S21855 = wParam == TRUE;
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		AppScreenUpdate(hdc, 0);
		EndPaint(hwnd, &ps);
		return 0;
	}

#if 0
	if (Msg >= 0x100)
	{
		v13.qualifiers = eventQualifiers_S21862;
		v13.value_1 = 0;
		v13.value_2 = 0;
		switch (wParam)
		{
		case 8:
			v13.value_1 = 30777;
			v13.value_2 = 9;
			break;
		case 9:
			v13.value_1 = 9;
			v13.value_2 = 9;
			break;
		case 13:
			v13.value_1 = 30779;
			break;
		case 16:
			if (Msg == 257)
				eventQualifiers_S21862 &= ~1u;
			else
				eventQualifiers_S21862 |= 1u;
			break;
		case 17:
			if (Msg == 257)
				eventQualifiers_S21862 &= ~2u;
			else
				eventQualifiers_S21862 |= 2u;
			break;
		case 18:
			if (Msg == 257)
				eventQualifiers_S21862 &= ~4u;
			else
				eventQualifiers_S21862 |= 4u;
			break;
		case 27:
			v13.value_1 = 30778;
			break;
		case 32:
			v13.value_1 = 32;
			v13.value_2 = 32;
			break;
		case 33:
		case 188:
			v13.value_1 = 30786;
			break;
		case 34:
		case 190:
			v13.value_1 = 30787;
			break;
		case 35:
			v13.value_1 = 30785;
			break;
		case 36:
			v13.value_1 = 30784;
			break;
		case 37:
			v13.value_1 = 30782;
			break;
		case 38:
			v13.value_1 = 30780;
			break;
		case 39:
			v13.value_1 = 30783;
			break;
		case 40:
			v13.value_1 = 30781;
			break;
		case 112:
			v13.value_1 = 128;
			break;
		case 113:
			v13.value_1 = 129;
			break;
		case 114:
			v13.value_1 = 130;
			break;
		case 115:
			v13.value_1 = 131;
			break;
		case 116:
			v13.value_1 = 132;
			break;
		case 117:
			v13.value_1 = 133;
			break;
		case 118:
			v13.value_1 = 134;
			break;
		case 119:
			v13.value_1 = 135;
			break;
		case 120:
			v13.value_1 = 136;
			break;
		default:
			if (__mb_cur_max <= 1)
				v10 = _pctype[(char)wParam] & 4;
			else
				v10 = _isctype((char)wParam, 4);
			if (v10
				|| (__mb_cur_max <= 1 ? (v11 = _pctype[(char)wParam] & 0x103) : (v11 = _isctype((char)wParam, 259)), v11))
			{
				v13.value_2 = wParam;
				v13.value_1 = wParam;
				if ((eventQualifiers_S21862 & 1) == 0)
					v13.value_2 = tolower(wParam);
			}
			break;
		}
		if (!v13.value_1)
			return DefWindowProcA(hWnd, Msg, wParam, lParam);
		v13.type = Msg == 257;
		(*(void(__cdecl**)(brw_application*, brw_event*))((char*)&off_3C + (_DWORD)wrapApp_S21856))(
			wrapApp_S21856,
			&v13);
		return 0;
#endif

	case WM_CREATE:
	{
		int i;

		BufferClassesInit();

		for (i = 0; i < BUFFER_COUNT; i++)
		{
			if (BufferClasses[i].init())
				break;
		}

		if (i >= BUFFER_COUNT)
		{
			MessageBox(NULL, "No off-screen buffer types available", "ERROR", MB_TASKMODAL);
			PostMessage(hwndApp_S21852, WM_CLOSE, 0, 0);
		}

		currentBufferType = i;

		sysCursorHandle = LoadCursor(NULL, IDC_ARROW);
		currentCursorHandle = sysCursorHandle;
		SetCursor(sysCursorHandle);
		hdcApp_S21851 = GetDC(hwnd);
		BrV1dbBeginWrapper_Fixed();
		wrapApp_S218856->begin(wrapApp_S218856, appArgc_S21868, (char**)appArgv_S21869);
		hpaletteApp_S21849 = PaletteSetup();
		if (hpaletteApp_S21849)
		{
			hpaletteOld_S21850 = SelectPalette(hdcApp_S21851, hpaletteApp_S21849, FALSE);
			RealizePalette(hdcApp_S21851);
		}
		else
		{
			MessageBox(NULL, "Could not load palette", "Logging", MB_TASKMODAL);
		}

		return 0;
	}
	case WM_DESTROY:
		BufferEnd();
		if (hpaletteApp_S21849)
		{
			SelectPalette(hdcApp_S21851, hpaletteOld_S21850, FALSE);
			DeleteObject(hpaletteApp_S21849);
		}

		wrapApp_S218856->end(wrapApp_S218856);
		BrV1dbEndWrapper();
		ReleaseDC(hwndApp_S21852, hdcApp_S21851);
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		clientHeight_S21858 = HIWORD(lParam);
		clientWidth_S21857 = LOWORD(lParam);
		BufferSet(clientWidth_S21857, clientHeight_S21858);
		AppRender();
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL accelerators;

	hinstanceApp_S21848 = hInstance;
	wrapApp_S218856 = AppQuery("WIN32", 0, 0, 3);

	if (!wrapApp_S218856)
	{
		MessageBox(NULL, "AppQuery Failed", "Error", MB_TASKMODAL);
		return 10;
	}

	ProcessArguments(wrapApp_S218856->argc, wrapApp_S218856->argv);
	appArgv_S21869[appArgc_S21868][0] = '\0';

	if (!hPrevInstance)
	{
		WNDCLASS wc;
		memset(&wc, 0, sizeof(wc));
		wc.hInstance = hInstance;
		wc.style = CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = AppWndProc;
		wc.lpszClassName = className_S21854;
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(4));
		wc.hbrBackground = (HBRUSH)(SYSTEM_FIXED_FONT);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);

		if (!RegisterClass(&wc))
		{
			MessageBox(NULL, "Failed to register class", "Error", MB_TASKMODAL);
			return 20;
		}
	}

	hwndApp_S21852 = CreateWindowEx(
		0,
		className_S21854,
		wrapApp_S218856->title,
		0x2CD0000u, // TODO
		0x80000000, // TODO
		0,
		450,
		350,
		NULL,
		NULL,
		hinstanceApp_S21848,
		NULL
	);

	if (!hwndApp_S21852)
	{
		MessageBox(NULL, "Failed to create window", "Error", MB_TASKMODAL);
		return 30;
	}

	ShowWindow(hwndApp_S21852, nCmdShow);

	accelerators = LoadAccelerators(hinstanceApp_S21848, MAKEINTRESOURCE(4));

	while (1)
	{
		while (!PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (IdleProcess())
				WaitMessage();
		}

		if (msg.message == WM_QUIT)
			break;

		if (!TranslateAccelerator(msg.hwnd, accelerators, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

/* common functions */

br_boolean IdleProcess()
{
	int r;

	if (!appPixelmap_S21860)
		return BR_TRUE;

	if (!appIsActive_S21855)
		return BR_TRUE;

	r = wrapApp_S218856->update(wrapApp_S218856);
	AppRender();
#ifdef _WIN32
	AppScreenUpdate(hdcApp_S21851, 0);
#endif
	return r != 0;
}

void ProcessArguments(int argc, char** argv)
{
	int process = argc;

	for (; process > 0; process--)
	{
		BrStrCpy(appArgv_S21869[appArgc_S21868], argv[appArgc_S21868]);
		appArgc_S21868 += 1;
	}
}

void SendMouseEvent(br_uint_16 type, br_uint_32 pos)
{
	brw_event evt;
	evt.type = type;
	evt.qualifiers = eventQualifiers_S21862;
	evt.value_1 = LOWORD(pos); /* x */
	evt.value_2 = HIWORD(pos); /* y */
	wrapApp_S218856->event(wrapApp_S218856, &evt);
}

void MouseCapture(HWND hwnd, br_boolean capture)
{
	if (capture && !_captured)
	{
		_captured = BR_TRUE;
		SetCapture(hwnd);
	}

	if (!capture && _captured)
	{
		_captured = BR_FALSE;
		ReleaseCapture();
	}
}

typedef struct brw_gdi_palette // LOGPALETTE
{
	br_uint_16         palVersion;
	br_uint_16         palNumEntries;
	PALETTEENTRY		palPalEntry[256];
} brw_gdi_palette;

HPALETTE PaletteSetup()
{
	br_pixelmap* palette = NULL;
	HGLOBAL hGlob;
	brw_gdi_palette* newPal;
	HPALETTE creation;
	HDC dc;
	int i;
	br_uint_8* startp;

	if (wrapApp_S218856->palette)
		palette = wrapApp_S218856->palette(wrapApp_S218856, 10, 256);

	if (!palette)
		return NULL;

	if (palette->type != 7 || palette->width != 1 || palette->height != 256)
		BrFailure("incorrect palette");

	hGlob = GlobalAlloc(GHND, sizeof(brw_gdi_palette));
	newPal = GlobalLock(hGlob);

	newPal->palVersion = 768;
	newPal->palNumEntries = 256;

	dc = GetDC(NULL);
	GetSystemPaletteEntries(dc, 0, 256, newPal->palPalEntry);
	ReleaseDC(NULL, dc);

	for (i = 236; i > 0; i--)
	{
		newPal->palPalEntry[10 + i].peFlags = 4;
	}

	for (i = 236; i > 0; i--)
	{
		newPal->palPalEntry[10 + i].peRed = -1;
		newPal->palPalEntry[10 + i].peGreen = 0;
		newPal->palPalEntry[10 + i].peBlue = -1;
	}

	startp = ((br_uint_8*)(palette->pixels)) + 10 * palette->row_bytes;
	for (i = 226; i > 0; i--, startp += 4)
	{
		newPal->palPalEntry[10 + i].peRed = *(startp + 3);
		newPal->palPalEntry[10 + i].peGreen = *(startp + 2);
		newPal->palPalEntry[10 + i].peBlue = *(startp + 1); // either this or blue and alpha are the same...
		newPal->palPalEntry[10 + i].peFlags = *startp;

		startp += palette->row_bytes;
	}

	/* might not be 100% ok ... */

	creation = CreatePalette((LOGPALETTE*)newPal);
	hGlob = GlobalHandle(newPal);
	GlobalUnlock(hGlob);
	hGlob = GlobalHandle(newPal);
	GlobalFree(hGlob);

	return creation;
}

void BufferSet(br_int_32 width, br_int_32 height)
{
	if (_saved_height != height || _saved_width != width)
	{
		_saved_width = width;
		_saved_height = height;

		if (currentBuffer)
			currentBuffer->free(currentBuffer);

		if (appPixelmap_S21860)
			BrPixelmapFree(appPixelmap_S21860);

		currentBuffer = BufferClasses[currentBufferType].allocate(hdcApp_S21851, hpaletteApp_S21849, clientWidth_S21857, clientHeight_S21858);

		appPixelmap_S21860 = BrPixelmapAllocate(BR_PMT_INDEX_8, _saved_width, _saved_height, currentBuffer->bits, BR_PMAF_INVERTED);
		wrapApp_S218856->destination(wrapApp_S218856, appPixelmap_S21860);
	}
}

void BufferEnd()
{
	if (appPixelmap_S21860)
	{
		BrPixelmapFree(appPixelmap_S21860);
		appPixelmap_S21860 = NULL;
	}

	if (currentBuffer)
		currentBuffer->free(currentBuffer);
}

void AppRender()
{
	static br_bounds2i _renderBounds;
	if (appPixelmap_S21860)
		wrapApp_S218856->render(wrapApp_S218856, &_renderBounds);
}

void AppScreenUpdate(HDC hdc, br_vector4_i* rc)
{
	int x = 0, y = 0, width = clientWidth_S21857, height = clientHeight_S21858;

	if (rc) /* this was never properly used... */
	{
		x = rc->v[0] / 0x10000;
		y = rc->v[1] / 0x10000;
		width = rc->v[2] / 0x10000 - x + 1;
		height = rc->v[3] / 0x10000 - y + 1;
	}

	if (currentBuffer)
		currentBuffer->blit(currentBuffer, hdc, x, y, x, y, width, height);
}

void BrwQuitRequest()
{
	PostMessage(hwndApp_S21852, WM_CLOSE, 0, 0);
}

void BrwWinWarning(const char* msg)
{
	MessageBox(NULL, msg, "WARNING", MB_TASKMODAL);
}


void BrwWinError(const char* msg)
{
	MessageBox(NULL, msg, "ERROR", MB_TASKMODAL);
	ExitProcess(10);
}

void BrwTimerStop(int id)
{

}

void BrwTimerStart(int period, int shots, int id)
{

}

int BrwRequestFile(brw_requestfile* frq)
{
	static const char filter_1[] = "All Files (*.*)";
	static char filename_request[512];
	static char filename_title[256];

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = filter_1;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename_request;
	ofn.hwndOwner = hwndApp_S21852;
	ofn.lpstrTitle = frq->title;
	ofn.nMaxFile = 512;
	ofn.lpstrFileTitle = filename_title;
	ofn.nMaxFileTitle = 256;
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;

	if (frq->extension)
	{
		ofn.lpstrDefExt = frq->extension;
		sprintf(filename_request, "*.%s", frq->extension);
	}
	else
		strcpy(filename_request, "*.*");

	if (frq->type & BRW_FRQ_MANY)
		ofn.Flags |= OFN_ALLOWMULTISELECT;

	if (frq->type & BRW_FRQ_EXISTS || frq->type & BRW_FRQ_READ)
		ofn.Flags |= OFN_FILEMUSTEXIST;

	if (frq->type & BRW_FRQ_WRITE)
		ofn.Flags |= OFN_NOREADONLYRETURN;

	if (frq->type & BRW_FRQ_READWRITE)
		ofn.Flags |= OFN_FILEMUSTEXIST | OFN_NOREADONLYRETURN;

	if (!GetOpenFileName(&ofn))
		return 0;

	frq->namec = 0;

	if (!strchr(&ofn.lpstrFile[ofn.nFileOffset], ' '))
	{
		if (frq->buffer_size - 5 >= strlen(ofn.lpstrFile))
		{
			strcpy(frq->buffer, ofn.lpstrFile);
			frq->namev[frq->namec] = frq->buffer;
			frq->namec++;
		}
	}
	else
	{
		if (ofn.nFileOffset)
			ofn.lpstrFile[ofn.nFileOffset - 1] = '\\';

		if (ofn.lpstrFile[ofn.nFileOffset])
		{
			char* buf = &ofn.lpstrFile[ofn.nFileOffset];

			while (*buf)
			{
				int len;
				char* x = strchr(buf, ' ');
				if (x)
				{
					*x = '\0';
					buf = x + 1;
				}

				len = strlen(x) + 1;

				if (ofn.nFileOffset + len > frq->buffer_size)
					break;

				strncpy(frq->buffer, ofn.lpstrFile, ofn.nFileOffset);
				strcpy(&frq->buffer[ofn.nFileOffset], x);

				frq->namev[frq->namec] = x;
				frq->namec++;

				frq->buffer += ofn.nFileOffset + len - 1;

				if (!buf)
					return frq->namec;
			}
		}
	}

	return frq->namec;
}

int BrwRequestList(brw_requestlist* lrq)
{
	return 0;
}

int BrwRequestString(brw_requeststring* srq)
{
	return 0;
}

int BrwRequestMessage(brw_requestmessage* mrq)
{
	return 0;
}

br_errorhandler* BrwErrorHandler(void)
{
	static br_errorhandler BrwPMErrorHandler_S22258 =
	{
		.identifier = "Presentation Manager ErrorHandler",
		.error = BrwWinError,
		.message = BrwWinWarning
	};

	return &BrwPMErrorHandler_S22258;
}

br_filesystem* BrwFilesystem(void)
{
	return NULL;
}

br_allocator* BrwAllocator(void)
{
	return NULL;
}

void BrwPointerBegin(void)
{
	cursorHandles[BRW_POINTER_NONE] = 0;
	cursorHandles[BRW_POINTER_ARROW] = LoadCursor(NULL, IDC_ARROW);
	cursorHandles[BRW_POINTER_WAIT] = LoadCursor(NULL, IDC_WAIT);
	cursorHandles[BRW_POINTER_CROSS] = LoadCursor(hinstanceApp_S21848, MAKEINTRESOURCE(64)); // TODO: what?
	cursorHandles[BRW_POINTER_SQUARE] = LoadCursor(hinstanceApp_S21848, MAKEINTRESOURCE(68));

	currentPointer_S21863 = BRW_POINTER_ARROW;
	hiddenCursor_S21867 = BR_FALSE;
}

void BrwPointerPositionGet(int* xp, int* yp)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(hwndApp_S21852, &point);
	*xp = point.x;
	*yp = point.y;
}

void BrwPointerPositionSet(int x, int y)
{
	POINT point;
	ClientToScreen(hwndApp_S21852, &point);
	SetCursorPos(point.x, point.y);
}

void BrwPointerShapeSet(int pointer_shape)
{
	currentPointer_S21863 = pointer_shape;

	if (cursorHandles[pointer_shape])
	{
		SetCursor(cursorHandles[pointer_shape]);
		currentCursorHandle = cursorHandles[pointer_shape];

		if (hiddenCursor_S21867)
		{
			ShowCursor(TRUE);
			hiddenCursor_S21867 = BR_FALSE;
		}
	}
	else
	{
		if (!hiddenCursor_S21867)
		{
			ShowCursor(FALSE);
			hiddenCursor_S21867 = BR_TRUE;
		}
	}
}

int BrwPointerShapeGet(void)
{
	return currentPointer_S21863;
}

void BrwPointerEnd(void)
{
	SetCursor(sysCursorHandle);
	DestroyCursor(cursorHandles[BRW_POINTER_CROSS]);
	DestroyCursor(cursorHandles[BRW_POINTER_SQUARE]);
}

static int MenuTemplateSize(brw_menu* menus)
{
	int size = 0;

	while (menus->type != BRW_MENU_END)
	{
		switch (menus->type)
		{
		case BRW_MENU_COMMAND:
			size += 2 * MultiByteToWideChar(CP_ACP, 0, menus->text, -1, NULL, 0) + 4; // sizeof(MENUITEMTEMPLATE) - mtString = 4
			break;
		case BRW_MENU_SEPARATOR:
			size += 6;
			break;
		case BRW_MENU_SUBMENU:
			size += 2 * MultiByteToWideChar(CP_ACP, 0, menus->text, -1, NULL, 0) + 2;
			size += MenuTemplateSize((brw_menu*)menus->data);
			break;
		case BRW_MENU_WRAPPER:
			size += 2 * MultiByteToWideChar(CP_ACP, 0, wrapperMenuText_S21872, -1, NULL, 0) + 2;
			size += MenuTemplateSize((brw_menu*)menus->data);
			break;
		default:
			break;
		}

		menus++;
	}

	return size;
}

static MENUITEMTEMPLATE* MenuTemplateBuild(MENUITEMTEMPLATE* itm, brw_menu* menus, int base_id)
{
	do
	{
		itm->mtOption = 0x80;

		if (menus->type == BRW_MENU_END)
		{
			itm->mtOption = 0;
			break;
		}

		switch (menus->type)
		{
		case BRW_MENU_COMMAND:

			if (menus->flags & BRW_MENUF_CHECK)
				itm->mtOption |= MF_CHECKED;

			if (menus->flags & BRW_MENUF_DISABLE)
				itm->mtOption |= MF_GRAYED;

			if (menus->command)
				itm->mtID = menus->command + base_id;
			else
				itm->mtID = 0;

			MultiByteToWideChar(CP_ACP, 0, menus->text, -1, itm->mtString, 128);
			break;
		case BRW_MENU_SEPARATOR:
			itm->mtID = 0;
			itm->mtString[0] = L'\0';
			break;
		case BRW_MENU_SUBMENU:
			itm->mtOption |= MF_POPUP;
			itm->mtID = 0;
			MultiByteToWideChar(CP_ACP, 0, menus->text, -1, itm->mtString, 128);
			itm = MenuTemplateBuild(itm + 1, (brw_menu*)menus->data, base_id);
			break;
		case BRW_MENU_WRAPPER:
			itm->mtOption |= MF_POPUP;
			MultiByteToWideChar(CP_ACP, 0, wrapperMenuText_S21872, -1, itm->mtString, 128);
			itm = MenuTemplateBuild(itm + 1, wrapperMenu_S21880, 0);
			break;

		default:
			break;
		}

		itm++;
		menus++;
	} while (menus->type != BRW_MENU_END);

	return itm;
}

void BrwMenuBegin(brw_menu* menus)
{
	if (!hmenuAppMenu_S21853)
	{
		MENUITEMTEMPLATEHEADER* tmp;
		int sz = MenuTemplateSize(menus);
		tmp = (MENUITEMTEMPLATEHEADER*)malloc(sz + sizeof(MENUITEMTEMPLATEHEADER));
		
		if (!tmp)
			return;

		tmp->versionNumber = 0;
		tmp->offset = 0;
		MenuTemplateBuild((MENUITEMTEMPLATE*)(((br_uint_8*)tmp) + 4), menus, MENU_ID_START);
		hmenuAppMenu_S21853 = LoadMenuIndirect((MENUTEMPLATE*)tmp);
		free(tmp);
		SetMenu(hwndApp_S21852, hmenuAppMenu_S21853);
	}
}

int BrwMenuDisable(int item, int action)
{
	int r;

	if (!hmenuAppMenu_S21853)
		return 0;

	r = GetMenuState(hmenuAppMenu_S21853, item + MENU_ID_START, MF_BYCOMMAND) & MF_GRAYED;

	switch (action)
	{
	case BRW_ACTION_NOP:
		break;

	case BRW_ACTION_CLEAR:
		CheckMenuItem(hmenuAppMenu_S21853, item + MENU_ID_START, 0);
		break;

	case BRW_ACTION_SET:
		CheckMenuItem(hmenuAppMenu_S21853, item + MENU_ID_START, MF_GRAYED);
		break;

	case BRW_ACTION_TOGGLE:
		CheckMenuItem(hmenuAppMenu_S21853, item + MENU_ID_START, r != 0 ? 0 : MF_GRAYED);
		break;
	}

	return r != 0;
}

int BrwMenuCheck(int item, int action)
{
	int r;

	if (!hmenuAppMenu_S21853)
		return 0;

	r = GetMenuState(hmenuAppMenu_S21853, item + MENU_ID_START, MF_BYCOMMAND) & MF_CHECKED;

	switch (action)
	{
	case BRW_ACTION_NOP:
		break;

	case BRW_ACTION_CLEAR:
		CheckMenuItem(hmenuAppMenu_S21853, item + MENU_ID_START, MF_UNCHECKED);
		break;

	case BRW_ACTION_SET:
		CheckMenuItem(hmenuAppMenu_S21853, item + MENU_ID_START, MF_CHECKED);
		break;

	case BRW_ACTION_TOGGLE:
		CheckMenuItem(hmenuAppMenu_S21853, item + MENU_ID_START, r != 0 ? MF_UNCHECKED : MF_CHECKED);
		break;
	}

	return r != 0;
}

void BrwMenuEnd(void)
{
	if (hmenuAppMenu_S21853)
	{
		SetMenu(hwndApp_S21852, NULL);
		DestroyMenu(hmenuAppMenu_S21853);
		hmenuAppMenu_S21853 = NULL;
	}
}
