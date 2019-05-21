#include "AiButton.h"
#include "..\Resource\Win\resource.h"
#include "BarcodeToolPlugin.h"

namespace
{
	/* Initialize static variables */
	static WNDPROC defWndProc = NULL;
	static bool	registered = false;

	/* Declaration */

	LRESULT CALLBACK AiButtonWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void RegisterWithOS(HINSTANCE inAppInstance)
	{
		if (!registered)
		{
			WNDCLASSEX AiButtonClass;
			AiButtonClass.cbSize = sizeof(AiButtonClass);
			::GetClassInfoEx(NULL, L"BUTTON", &AiButtonClass);
			AiButtonClass.hInstance = inAppInstance;
			AiButtonClass.lpszClassName = AiButtonClassName;
			defWndProc = AiButtonClass.lpfnWndProc;
			AiButtonClass.lpfnWndProc = AiButtonWndProc;
			AiButtonClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
			::RegisterClassEx(&AiButtonClass);
			registered = true;
		}
	}

	THEMCOLOR *ic;

	LRESULT CALLBACK AiButtonWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		switch (msg)
		{
		case WM_PAINT:
		{
			HDC dc;
			PAINTSTRUCT ps;
			RECT rect;
			HPEN hPen;
			HBRUSH hBrush;
			LRESULT chk = SendMessage((HWND)hWnd, BM_GETCHECK, 0, 0);
			LRESULT chkState = SendMessage((HWND)hWnd, BM_GETSTATE, 0, 0);

			dc = BeginPaint(hWnd, &ps);

			rect.top = (ps.rcPaint.bottom - ps.rcPaint.top - 16) / 2;
			rect.bottom = rect.top + 16;
			rect.left = rect.top;
			rect.right = rect.bottom;

			LRESULT bIcon = SendMessage((HWND)hWnd, BM_GETIMAGE,IMAGE_ICON, 0);

			if (GetRValue(ic->ICBackgroundColor) < 90)
			{
				HDC hdcMem = CreateCompatibleDC(dc);
				HBITMAP hBmp = CreateCompatibleBitmap(dc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
				SelectObject(hdcMem, hBmp);

				hBrush = CreateSolidBrush(RGB(~GetRValue(ic->ICBackgroundColor), ~GetGValue(ic->ICBackgroundColor), ~GetBValue(ic->ICBackgroundColor)));
				FillRect(hdcMem, &ps.rcPaint, hBrush);

				if ((chk == BST_CHECKED)| (chkState & BST_PUSHED)) {
					hBrush = CreateSolidBrush(RGB(~GetRValue(ic->ICBtnDownColor), ~GetGValue(ic->ICBtnDownColor), ~GetBValue(ic->ICBtnDownColor)));
				}
				else {
					hBrush = CreateSolidBrush(RGB(~GetRValue(ic->ICBtnUpColor), ~GetGValue(ic->ICBtnUpColor), ~GetBValue(ic->ICBtnUpColor)));
				}
				
				hPen = CreatePen(PS_INSIDEFRAME, 1, RGB(~GetRValue(ic->ICBtnFrameColor), ~GetGValue(ic->ICBtnFrameColor), ~GetBValue(ic->ICBtnFrameColor)));

				SelectObject(hdcMem, hPen);
				SelectObject(hdcMem, hBrush);
				RoundRect(hdcMem, ps.rcPaint.left , ps.rcPaint.top , ps.rcPaint.right , ps.rcPaint.bottom , 0, 0);
				DrawIconEx(hdcMem, rect.left, rect.top, (HICON)bIcon, 16, 16, 0, hBrush, DI_NORMAL);
				BitBlt(dc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, hdcMem, 0, 0, NOTSRCCOPY);
				DeleteObject(hBmp);
				DeleteDC(hdcMem);
			}
			else
			{
				hBrush = CreateSolidBrush(ic->ICBackgroundColor);
				FillRect(dc, &ps.rcPaint, hBrush);

				if ((chk == BST_CHECKED) | (chkState & BST_PUSHED)) {
					hBrush = CreateSolidBrush(ic->ICBtnDownColor);}
				else {
					hBrush = CreateSolidBrush(ic->ICBtnUpColor);}
				
				hPen = CreatePen(PS_INSIDEFRAME, 1, ic->ICBtnFrameColor);
				SelectObject(dc, hPen);
				SelectObject(dc, hBrush);
				RoundRect(dc, ps.rcPaint.left , ps.rcPaint.top , ps.rcPaint.right , ps.rcPaint.bottom , 0, 0);
				DrawIconEx(dc, rect.left, rect.top, (HICON)bIcon, 16, 16, 0, hBrush, DI_NORMAL);
			}

			EndPaint(hWnd, &ps);
			DeleteObject(hBrush);
			DeleteObject(hPen);

			if (chkState & BST_PUSHED)
				SendMessage(GetParent(hWnd), AIB_PUSHED, 0, 0);

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

HWND AiButton::CreateAiButton(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
	HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam)
{
	if (!registered)
	{
		RegisterWithOS(inHinstance);
	}

	return ::CreateWindowEx(inExStyle
		, AiButtonClassName
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