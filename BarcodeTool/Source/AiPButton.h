#ifndef __AIPBUTTON__
#define __AIPBUTTON__

#include "windows.h"
#include "winuser.h"

#define AiPButtonClassName L"AiPButton"

namespace AiPButton
{

	HWND CreateAiPButton(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
		HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam);

}

#endif