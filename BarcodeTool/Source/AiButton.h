#ifndef __AIBUTTON__
#define __AIBUTTON__

#include "windows.h"
#include "winuser.h"

#define AiButtonClassName L"AiButton"

namespace AiButton
{

	HWND CreateAiButton(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
		HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam);

}

#endif