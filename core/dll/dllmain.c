#if defined(_WIN32) && !defined(BR_STATIC)

#define WIN32_LEAN_AND_MEAN 1
#define STRICT 1
#include <Windows.h>

LRESULT WINAPI DllMain(HINSTANCE hDll, DWORD fdwReason, LPVOID lpReserved)
{
    return TRUE;
}

#endif
