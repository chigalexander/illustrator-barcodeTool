#ifndef __AILISTBOX__
#define __AILISTBOX__

#include "windows.h"
#include "winuser.h"

#define AiListboxClassName L"AiListbox"

namespace AiListbox
{
	HWND CreateAiListbox(DWORD inExStyle,  LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
									HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam);

}

#endif