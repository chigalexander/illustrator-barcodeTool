#include "AiListbox.h"
//#define WM_LBTRACKPOINT 0x0131
#include "BarcodeToolPlugin.h"

namespace
{

	/* Initialize static variables */
	static WNDPROC defWndProc = NULL;
	static bool	registered = false;

	/* Declaration */

	LRESULT CALLBACK AiListboxWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void RegisterWithOS(HINSTANCE inAppInstance)
	{
		if(!registered)
		{
			WNDCLASSEX AiListboxClass;
			AiListboxClass.cbSize = sizeof(AiListboxClass);

			::GetClassInfoEx(NULL, L"LISTBOX", &AiListboxClass);

			AiListboxClass.hInstance = inAppInstance;
			AiListboxClass.lpszClassName = AiListboxClassName;
			AiListboxClass.hbrBackground = NULL;
			defWndProc = AiListboxClass.lpfnWndProc;
			AiListboxClass.lpfnWndProc = AiListboxWndProc;

			::RegisterClassEx(&AiListboxClass);

			registered = true;
		}
	}
	HWND hParent;
	LRESULT CALLBACK AiListboxWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		switch (msg)
		{
			case WM_MOUSEMOVE:
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				int yPos = HIWORD(lParam);
				int xPos = LOWORD(lParam);
				if (xPos >= rect.left && xPos <= rect.right && yPos >= rect.top && yPos <= rect.bottom)
				{
					int idx = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);
					int h = SendMessage(hWnd, LB_GETITEMHEIGHT, 0, 0); 
					SendMessage(hWnd, LB_SETCURSEL, idx + yPos / h, 0);
				}
				else if (xPos >= rect.left && xPos <= (rect.right + 20) && yPos >= rect.top && yPos <= rect.bottom) //скроллбар
				{
					ReleaseCapture();
				}
				return 0;
			}
				break;
			case WM_NCMOUSELEAVE:
			{
				SetCapture(hWnd);
			}
			break;
			case WM_KILLFOCUS:
			{
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				return 0;
			}
			break;
			case WM_LBUTTONDOWN:
			{
				RECT rect;
				GetClientRect(hWnd, &rect);
				int yPos = HIWORD(lParam);
				int xPos = LOWORD(lParam);
				if (xPos >= rect.left && xPos <= rect.right && yPos >= rect.top && yPos <= rect.bottom)//список WM_SYSCOMMAND
				{
					int idx = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);
					int h = SendMessage(hWnd, LB_GETITEMHEIGHT, 0, 0); 
					SendMessage(hParent, LB_SETCURSEL, idx + yPos / h, 0); 
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
				else 
				{
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
				return 0;
			}
			break;
			case WM_USER + 4:
			{
				hParent = (HWND) wParam;
			}
			break;
		}

		/* For all other messages call default window proc */
		return CallWindowProc(defWndProc, hWnd, msg, wParam, lParam);
	}

} /*  Anonymous namespace */

HWND AiListbox::CreateAiListbox(DWORD inExStyle, LPCTSTR inWindowName, DWORD inStyle, int x, int y, int inWidth, int inHeight,
									HWND inParent, HMENU inMenu, HINSTANCE inHinstance, LPVOID inLpParam)
{
	if(!registered)
	{
		RegisterWithOS(inHinstance);
	}

	return ::CreateWindowEx(	inExStyle
						, AiListboxClassName
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
