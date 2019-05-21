#include "AiStatic.h"
#include <tchar.h> 

namespace
{

	/* Initialize static variables */
	static WNDPROC defWndProc = NULL;
	static bool	registered = false;

	/* Declaration */

	LRESULT CALLBACK AiStaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void RegisterWithOS(HINSTANCE inAppInstance)
	{
		if (!registered)
		{
			WNDCLASSEX AiStaticClass;
			AiStaticClass.cbSize = sizeof(AiStaticClass);

			::GetClassInfoEx(NULL, L"STATIC", &AiStaticClass);

			AiStaticClass.hInstance = inAppInstance;
			AiStaticClass.lpszClassName = AiStaticClassName;
			defWndProc = AiStaticClass.lpfnWndProc;
			AiStaticClass.lpfnWndProc = AiStaticWndProc;

			::RegisterClassEx(&AiStaticClass);

			registered = true;
		}
	}


	LRESULT CALLBACK AiStaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		HDC dc;
		PAINTSTRUCT ps;
		HFONT  hFont = CreateFont(13.5, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft Sans Serif");
		

		switch (msg)
		{
		case WM_PAINT:
			{

			TCHAR Str[255];
			SendMessage(hWnd, (UINT)WM_GETTEXT, 255, (LPARAM)Str);
			RECT rect;
			GetWindowRect(hWnd, &rect);
			dc = BeginPaint(hWnd, &ps);
			SetTextAlign(dc, TA_RIGHT);
			SetBkMode(dc, TRANSPARENT);

			SelectObject(ps.hdc, hFont);
			//SetTextColor(dc, RGB(0,0,0));
			TextOut(dc, rect.right - rect.left, 0, Str, _tcsclen(Str));
			SetBkMode(dc, OPAQUE);
			EndPaint(hWnd, &ps);
				
			}
			break;
		}

		/* For all other messages call default window proc */
		return CallWindowProc(defWndProc, hWnd, msg, wParam, lParam);
	}

} /*  Anonymous namespace */

HWND AiStatic::CreateAiStatic(LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
	HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam)
{
	if (!registered)
	{
		RegisterWithOS(inHinstance);
	}

	return ::CreateWindowEx(WS_EX_TRANSPARENT
		, AiStaticClassName
		, inWindowName
		, inStyle
		, x
		, y
		, inWidth
		, inHeight
		, inParent
		, inMenu
		, inHinstance
		, inLpParam
	);

}