#include "AiCombo.h"
#include "AiButton.h"
#include "AiListbox.h"
#include "..\Resource\Win\resource.h"
#include "BarcodeToolPlugin.h"

namespace
{
	/* Initialize static variables */
	static WNDPROC defWndProc = NULL;
	static bool	registered = false;

	/* Declaration */

	LRESULT CALLBACK AiComboWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void RegisterWithOS(HINSTANCE inAppInstance)
	{
		if (!registered)
		{
			WNDCLASSEX AiComboClass;
			AiComboClass.cbSize = sizeof(AiComboClass);
			::GetClassInfoEx(NULL, L"STATIC", &AiComboClass);
			AiComboClass.hInstance = inAppInstance;
			AiComboClass.lpszClassName = AiComboClassName;
			defWndProc = AiComboClass.lpfnWndProc;
			AiComboClass.lpfnWndProc = AiComboWndProc;
			AiComboClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
			::RegisterClassEx(&AiComboClass);
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
	HFONT myFont;
	HWND fHwndListBox;
	HWND fHwndEdit;

	LRESULT CALLBACK AiComboWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HPEN hPen;
		RECT rect;

		switch (msg)
		{
		case WM_CREATE:
		{
			GetClientRect(hWnd, &rect);

			fHwndEdit = CreateWindowEx(
				WS_EX_TRANSPARENT, L"EDIT",   // Predefined class; Unicode assumed. 
				L"",		//  
				WS_VISIBLE | WS_CHILD | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL,   // Styles. 
				3,         // x position. 
				(rect.bottom - 13) /2,         // y position. 
				rect.right - rect.left-4-20,        // width.
				rect.bottom - rect.top,        // height.
				hWnd,       // Parent window.
				NULL,
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
				NULL);      // Pointer not needed.
			SendMessage(fHwndEdit, WM_SETFONT, WPARAM(myFont), TRUE);

			HWND HwndButton = AiButton::CreateAiButton(
				0, //L"BUTTON",   // Predefined class; Unicode assumed. 
				L"",		//  
				WS_VISIBLE | WS_CHILD | ES_LEFT,  // Styles. 
				rect.right - 20,         // x position. 
				0,         // y position. 
				20,        // width.
				rect.bottom - rect.top,        // height.
				hWnd,       // Parent window.
				(HMENU)1,
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
				NULL);      // Pointer not needed.
			SendMessage(HwndButton, WM_USER, 0, (LPARAM)&ic);
			HBITMAP hBitmap = (HBITMAP)LoadImage(GetCurrentModule(), MAKEINTRESOURCE(IDI_ICON7), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT);
			SendMessage(HwndButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hBitmap);
			return 0;
		}
		break;
		case WM_PAINT:
		{
			HBRUSH hBrush = CreateSolidBrush(ic->ICTextEditBackgroundColor);
			HDC dc;
			dc = BeginPaint(hWnd, &ps);
			hPen = CreatePen(PS_SOLID, 1, ic->ICBtnFrameColor);
			SelectObject(dc, hPen);
			SelectObject(dc, hBrush);
			RoundRect(dc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - 19, ps.rcPaint.bottom, 0, 0);
			EndPaint(hWnd, &ps);
			DeleteObject(hBrush);
			DeleteObject(hPen);
			return 0;
		}
		break;
		case WM_CTLCOLOREDIT:
		{
			SetTextColor((HDC)wParam, ic->ICTextEditColor);
			SetBkColor((HDC)wParam, ic->ICTextEditBackgroundColor);
			return 0;
		}
		break;
		case WM_CTLCOLORLISTBOX:
		{
				SetBkMode((HDC)wParam, OPAQUE);
				SetTextColor((HDC)wParam, ic->ICTextEditColor);
				SetBkColor((HDC)wParam, ic->ICTextEditBackgroundColor);
				return (INT_PTR)ic->ICTextEditBackgroundBrush;
			}
		break;
		case AIB_PUSHED:
		{
				if (!IsWindow((HWND)fHwndListBox)) {
					GetWindowRect(hWnd, &rect);
					fHwndListBox = AiListbox::CreateAiListbox(
						0,// L"LISTBOX",   // Predefined class; Unicode assumed. 
						L"Font",		//  
						WS_POPUP | WS_VSCROLL | WS_BORDER,   // Styles. 
						0, 0, 0, 0, 
						hWnd,       // Parent window.
						NULL,
						(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
						NULL);      // Pointer not needed.

					SendMessage(fHwndListBox, WM_SETFONT, WPARAM(myFont), TRUE);
					SendMessage(GetParent(hWnd), AIC_OPENLIST, (WPARAM)hWnd, (LPARAM)fHwndListBox);  //add fontlist
					SendMessage(fHwndListBox, WM_USER + 4, (WPARAM)hWnd, 0); //hwnd parent

					int itemCount = SendMessage(fHwndListBox, LB_GETCOUNT, 0, 0);
					int itemHeight = SendMessage(fHwndListBox, LB_GETITEMHEIGHT, 0, 0);

					HDC hDCScreen = GetDC(NULL);
					int vert = GetDeviceCaps(hDCScreen, VERTRES);
					ReleaseDC(NULL, hDCScreen);

					int myX = rect.left;
					int myY = rect.bottom;
					int myWidth = rect.right - rect.left;
					int myHeght = itemHeight * 35 + 2;

					int height = (itemCount + 1) * itemHeight;
					
					if (height > myHeght)
					{
						height = myHeght;
					}

					if ((myY + height) > vert)
					{
						myY = rect.top - height;
					}
					SetWindowPos(fHwndListBox, 0, myX, myY, myWidth, height, 0);

					ShowWindow(fHwndListBox, SW_SHOW);
					SetCapture(fHwndListBox);
				}
				else
				{
					SendMessage(fHwndListBox, WM_CLOSE, 0, 0);
				}
			return 0;
		}
		break;
		case LB_SETCURSEL:
		{
			TCHAR buf[255];
			SendMessage(fHwndListBox, LB_GETTEXT, wParam, (LPARAM)buf);
			SendMessage(GetWindow(hWnd, GW_CHILD), WM_SETTEXT, 0, (LPARAM)buf);
			SendMessage(GetParent(hWnd), AIC_ITEMSELECT, (WPARAM)hWnd, (LPARAM)SendMessage(fHwndListBox, LB_GETITEMDATA, wParam,0));
			return 0;
		}
		break;
		case AIC_SETTEXT:
		{
			SendMessageA(GetWindow(hWnd, GW_CHILD), WM_SETTEXT, 0, (LPARAM)lParam);
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

HWND AiCombo::CreateAiCombo(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
	HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam, HFONT hFont)
{
	if (!registered)
	{
		RegisterWithOS(inHinstance);
	}
	myFont = hFont;
	return ::CreateWindowEx(inExStyle
		, AiComboClassName
		, inWindowName
		, ClassStyle
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