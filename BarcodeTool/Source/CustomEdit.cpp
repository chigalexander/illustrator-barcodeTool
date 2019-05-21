#include "CustomEdit.h"

namespace
{

	/* Initialize static variables */
	static WNDPROC defWndProc = NULL;
	static bool	registered = false;

	/* Declaration */

	LRESULT CALLBACK CustomEditWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void RegisterWithOS(HINSTANCE inAppInstance)
	{
		if(!registered)
		{
			WNDCLASSEX customEditClass;
			customEditClass.cbSize = sizeof(customEditClass);

			::GetClassInfoEx(NULL, L"EDIT", &customEditClass);

			customEditClass.hInstance = inAppInstance;
			customEditClass.lpszClassName = CustomEditClassName;
			customEditClass.hbrBackground = NULL;
			defWndProc = customEditClass.lpfnWndProc;
			customEditClass.lpfnWndProc = CustomEditWndProc;

			::RegisterClassEx(&customEditClass);

			registered = true;
		}
	}


	LRESULT CALLBACK CustomEditWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		//HDC dc;
		//PAINTSTRUCT ps;

		switch (msg)
		{
			//case WM_NCPAINT:
			//{

			//	////HBRUSH hBrush = CreateSolidBrush(RGB(150, 150, 150));
			//	//RECT rect;
			//	//GetWindowRect(hWnd, &rect);
			//	//HDC dc = GetWindowDC(hWnd);
			//	////HDC dc = GetDCEx(hWnd, (HRGN)wParam, DCX_WINDOW | DCX_CACHE | DCX_INTERSECTRGN | DCX_LOCKWINDOWUPDATE);
			//	//HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0,));
			//	//
			//	//HBRUSH hBrush = CreateSolidBrush(RGB(150, 150, 150));
			//	////FillRect(dc, &rect, hBrush);
			//	//SelectObject(dc, hBrush);
			//	////SetBkMode(dc, TRANSPARENT);
			//	//SelectObject(dc, hPen);
			//	//Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
			//	//DeleteObject(hPen);
			//	//ReleaseDC(hWnd, dc);
			//	return 0;
			//}
			//break;
			case WM_KEYUP:
			{
				/* Process only TAB and SHIFT + TAB */
				if (wParam == VK_TAB)
				{
					HWND parent = ::GetParent(hWnd);
					/* Poor implementation - Assumes that parent of edit knows about next focusable control */
					HWND nextFocusableItem = ::GetNextDlgTabItem(parent, hWnd, (::GetKeyState(VK_SHIFT) & 0x8000) != 0 ? true : false);
					if (nextFocusableItem)	::SetFocus(nextFocusableItem);
					return 0;
				}

				else if (wParam == VK_BACK)
				{
					TCHAR XStr[255];
					DWORD f, e;
					SendMessage(hWnd, (UINT)WM_GETTEXT, 255, (LPARAM)XStr);
					XStr[11] = '\0';
					SendMessage(hWnd, EM_GETSEL, (WPARAM)&f, (LPARAM)&e);
					SendMessage(hWnd, (UINT)WM_SETTEXT, 255, (LPARAM)XStr);
					SendMessage(hWnd, EM_SETSEL, (WPARAM)f, (LPARAM)e);
					return 0;
				}
				else if (wParam == VK_DELETE)
				{
					TCHAR XStr[255];
					DWORD f, e;
					SendMessage(hWnd, EM_GETSEL, (WPARAM)&f, (LPARAM)&e);
					if (e == 12)
					{
						CustomEdit::LastDigit(hWnd);
						return 0;
					}
					SendMessage(hWnd, (UINT)WM_GETTEXT, 255, (LPARAM)XStr);
					XStr[11] = '\0';
					SendMessage(hWnd, (UINT)WM_SETTEXT, 255, (LPARAM)XStr);
					SendMessage(hWnd, EM_SETSEL, (WPARAM)f, (LPARAM)e);
					return 0;
				}
				else
				{
					CustomEdit::LastDigit(hWnd);
					return 0;
				}
			}
		}

		/* For all other messages call default window proc */
		return CallWindowProc(defWndProc, hWnd, msg, wParam, lParam);
	}

} /*  Anonymous namespace */

void CustomEdit::LastDigit(HWND hWnd)
{
	TCHAR XStr[255];
	DWORD f, e;
	int lastDigit = 0;
	SendMessage(hWnd, (UINT)WM_GETTEXT, 255, (LPARAM)XStr);
	if (XStr[11] > 0x2F)
	{
		if (XStr[12] == '\0')
		{
			XStr[13] = '\0';
		}
		for (int c = 0; c < 11; c = c + 2)
		{
			lastDigit = lastDigit + (XStr[c] - '0') + (XStr[c + 1] - '0') * 3;
		}
		lastDigit = (10 - lastDigit % 10) % 10;
		XStr[12] = lastDigit + '0';
		SendMessage(hWnd, EM_GETSEL, (WPARAM)&f, (LPARAM)&e);
		if (e > 12)
		{
			e = 12;
			f = e;
		}
		SendMessage(hWnd, (UINT)WM_SETTEXT, 255, (LPARAM)XStr);
		SendMessage(hWnd, EM_SETSEL, (WPARAM)f, (LPARAM)e);
	}
}

HWND CustomEdit::CreateCustomEdit(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight, 
									HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam)
{
	if(!registered)
	{
		RegisterWithOS(inHinstance);
	}

	return ::CreateWindowEx(	inExStyle
						,CustomEditClassName
						,inWindowName
						,inStyle
						,x
						,y
						,inWidth
						,inHeight
						,inParent
						,inMenu
						,inHinstance
						,inLpParam
						);
	
}
