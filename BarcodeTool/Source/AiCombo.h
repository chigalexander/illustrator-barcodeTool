#ifndef __AICOMBO__
#define __AICOMBO__

#include "windows.h"
#include "winuser.h"

#define AiComboClassName L"AiCombo"
#define ClassStyle WS_CHILD | WS_VISIBLE// | WS_POPUP

namespace AiCombo
{

	HWND CreateAiCombo(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
		HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam, HFONT hFont);

}

#endif