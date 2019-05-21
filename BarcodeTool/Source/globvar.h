#ifndef __GLOBALVAR__
#define __GLOBALVAR__

#include "windows.h"

#define AIC_OPENLIST			WM_USER + 1
#define AIC_ITEMSELECT			WM_USER + 3
#define AIC_SETTEXT				WM_USER + 5
#define AIB_PUSHED				WM_USER + 2

#define K_p_to_mm				0.3527778

namespace globalvar
{
	struct THEMCOLOR
	{
		COLORREF ICBackgroundColor;
		COLORREF ICTextColor;
		COLORREF ICTextEditBackgroundColor;
		COLORREF ICTextEditColor;
		COLORREF ICFocusRingColor;
		COLORREF ICHoverColor;
		COLORREF ICCheckColor;
		COLORREF ICBtnUpColor;
		COLORREF ICBtnDownColor;
		COLORREF ICBtnFrameColor;
		HBRUSH ICTextEditBackgroundBrush;
	};
}
#endif