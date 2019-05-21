#include "AiCheck.h"
#include "..\Resource\Win\resource.h"
#include "BarcodeToolPlugin.h"

namespace
{
	/* Initialize static variables */
	static WNDPROC defWndProc = NULL;
	static bool	registered = false;

	/* Declaration */

	LRESULT CALLBACK AiCheckWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//void Paint(HWND hWnd);

	void RegisterWithOS(HINSTANCE inAppInstance)
	{
		if (!registered)
		{
			WNDCLASSEX AiCheckClass;
			AiCheckClass.cbSize = sizeof(AiCheckClass);
			::GetClassInfoEx(NULL, L"BUTTON", &AiCheckClass);
			AiCheckClass.hInstance = inAppInstance;
			AiCheckClass.lpszClassName = AiCheckClassName;
			defWndProc = AiCheckClass.lpfnWndProc;
			AiCheckClass.lpfnWndProc = AiCheckWndProc;
			AiCheckClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
			::RegisterClassEx(&AiCheckClass);
			registered = true;
		}
	}
	const HMODULE GetCurrentModule()
	{
		HMODULE hModule = NULL;
		// hModule is NULL if GetModuleHandleEx fails.
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
			| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCTSTR)GetCurrentModule, &hModule);
		return hModule;
	}

	THEMCOLOR *ic;

	HBITMAP hBitmapBlack; //черный
	HBITMAP hBitmapWhite; //белый

	LRESULT CALLBACK AiCheckWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CREATE:
		{
			HMODULE HmyModule = GetCurrentModule();
			hBitmapBlack = (HBITMAP)LoadImage(HmyModule, MAKEINTRESOURCE(IDI_ICON5), IMAGE_ICON, 12, 12, LR_LOADTRANSPARENT); //черный
			hBitmapWhite = (HBITMAP)LoadImage(HmyModule, MAKEINTRESOURCE(IDI_ICON6), IMAGE_ICON, 12, 12, LR_LOADTRANSPARENT); //белый
			break;
		}
		case WM_PAINT:
		{
			AiCheck::Paint(hWnd, false);
			return 0;
		}
		break;
		case WM_USER:
		{
			ic =(THEMCOLOR *) lParam;
			return 0;
		}
		break;
		}
		/* For all other messages call default window proc */
		return CallWindowProc(defWndProc, hWnd, msg, wParam, lParam);

	}

} /*  Anonymous namespace */

 void AiCheck::Paint(HWND hWnd, bool hover)
{
	 RECT rect;
	 HPEN hPen;
	 HBRUSH hBrush;
	 HDC dc;
	 PAINTSTRUCT ps;
	 HFONT  hFont;

	 hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft Sans Serif");

	 LRESULT chkState = SendMessage((HWND)hWnd, BM_GETCHECK, 0, 0);
	 dc = BeginPaint(hWnd, &ps);
	 hBrush = CreateSolidBrush(ic->ICBackgroundColor);
	 FillRect(dc, &ps.rcPaint, hBrush);
	 rect.top = (ps.rcPaint.bottom - ps.rcPaint.top - 12) / 2;
	 rect.bottom = rect.top + 12;
	 rect.left = rect.top;
	 rect.right = rect.bottom;

	 if (hover)
	 {
		 hPen = CreatePen(PS_SOLID, 1, ic->ICHoverColor);
		 SelectObject(dc, hPen);
		 SelectObject(dc, hBrush);
		 hBrush = CreateSolidBrush(ic->ICHoverColor);
	 }
	 else
	 {
		 hPen = CreatePen(PS_SOLID, 1, ic->ICCheckColor);
		 SelectObject(dc, hPen);
		 SelectObject(dc, hBrush);
		 hBrush = CreateSolidBrush(ic->ICCheckColor);
	 }

	 SelectObject(dc, hFont);
	 SetBkMode(dc, TRANSPARENT);
	 SetTextColor(dc, ic->ICTextColor);
	 SetTextAlign(dc, TA_LEFT);

	 TCHAR HStr[255];
	 SendMessage(hWnd, (UINT)WM_GETTEXT, 255, (LPARAM)HStr);
	 int c = 0;
	 while ((HStr[c] != '\0')) c++;

	 TextOut(dc, rect.right + 3, rect.top, HStr, c);

	 if (chkState == BST_CHECKED)
	 {
		 FillRect(dc, &rect, hBrush);
		 if (GetRValue(ic->ICBackgroundColor) < 90)
		 {
			 DrawIconEx(dc, rect.left, rect.top, (HICON)hBitmapBlack, 12, 12, 0, hBrush, DI_NORMAL);
		 }
		 else
		 {
			 DrawIconEx(dc, rect.left, rect.top, (HICON)hBitmapWhite, 12, 12, 0, hBrush, DI_NORMAL);
		 }
	 }
	 else
	 {
		 RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, 3, 3);
	 }

	 EndPaint(hWnd, &ps);
	 DeleteObject(hBrush);
	 DeleteObject(hPen);
	 DeleteObject(hFont);
}

HWND AiCheck::CreateAiCheck(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
	HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam)
{
	if (!registered)
	{
		RegisterWithOS(inHinstance);
	}

	return ::CreateWindowEx(inExStyle
		, AiCheckClassName
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