#ifndef __AICHECK__
#define __AICHECK__

#include "windows.h"
#include "winuser.h"

#define AiCheckClassName L"AiCheck"

namespace AiCheck
{

	HWND CreateAiCheck(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
		HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam);
	void Paint(HWND hWnd, bool hover);
}

#endif