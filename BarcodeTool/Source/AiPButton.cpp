#include "AiPButton.h"
#include "BarcodeToolPlugin.h"

namespace
{
	/* Initialize static variables */
	static WNDPROC defWndProc = NULL;
	static bool	registered = false;

	/* Declaration */

	LRESULT CALLBACK AiPButtonWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void RegisterWithOS(HINSTANCE inAppInstance)
	{
		if (!registered)
		{
			WNDCLASSEX AiPButtonClass;
			AiPButtonClass.cbSize = sizeof(AiPButtonClass);
			::GetClassInfoEx(NULL, L"BUTTON", &AiPButtonClass);
			AiPButtonClass.hInstance = inAppInstance;
			AiPButtonClass.lpszClassName = AiPButtonClassName;
			defWndProc = AiPButtonClass.lpfnWndProc;
			AiPButtonClass.lpfnWndProc = AiPButtonWndProc;
			AiPButtonClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
			::RegisterClassEx(&AiPButtonClass);
			registered = true;
		}
	}

	THEMCOLOR *ic;

	LRESULT CALLBACK AiPButtonWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		HDC dc;
		PAINTSTRUCT ps;
		RECT rect;
		HPEN hPen;

		switch (msg)
		{
		case WM_PAINT:
		{
			HBRUSH hBrush;
			HFONT  hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft Sans Serif");

			LRESULT chkState = SendMessage((HWND)hWnd, BM_GETSTATE, 0, 0);

			dc = BeginPaint(hWnd, &ps);
			hBrush = CreateSolidBrush(ic->ICBackgroundColor);
			FillRect(dc, &ps.rcPaint, hBrush);

			if (chkState & BST_PUSHED) {
				hBrush = CreateSolidBrush(ic->ICBtnDownColor);}
			else {
				hBrush = CreateSolidBrush(ic->ICBtnUpColor);}
				
			hPen = CreatePen(PS_INSIDEFRAME, 1, ic->ICBtnFrameColor);
			SelectObject(dc, hPen);
			SelectObject(dc, hBrush);
			RoundRect(dc, ps.rcPaint.left , ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom, 0, 0);

			SelectObject(dc, hFont);
			SetBkMode(dc, TRANSPARENT);
			SetTextColor(dc, ic->ICTextColor);
			SetTextAlign(dc, TA_CENTER);

			GetClientRect(hWnd, &rect);
			TCHAR HStr[255];
			SendMessage(hWnd, (UINT)WM_GETTEXT, 255, (LPARAM)HStr);
			int c = 0;
			while ((HStr[c] != '\0')) c++;
			TextOut(dc, (rect.right - rect.left) / 2, rect.top + 6, HStr, c);

			EndPaint(hWnd, &ps);
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hFont);

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

HWND AiPButton::CreateAiPButton(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
	HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam)
{
	if (!registered)
	{
		RegisterWithOS(inHinstance);
	}

	return ::CreateWindowEx(inExStyle
		, AiPButtonClassName
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