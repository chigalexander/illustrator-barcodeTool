#ifndef __RBUTTON__
#define __RBUTTON__

#include "windows.h"
#include "winuser.h"

#define AiStaticClassName L"AiStatic"

namespace AiStatic
{
	HWND CreateAiStatic(LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
		HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam);
	
	
}

#endif