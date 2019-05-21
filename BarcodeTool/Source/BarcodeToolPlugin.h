//========================================================================================
//  
//  $File: //ai/mainline/devtech/sdk/public/samplecode/BarcodeTool/source/BarcodeToolPlugin.h $
//
//  $Revision: #2 $
//
//  Copyright 1987 Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//========================================================================================

#ifndef __BarcodeToolPlugin_H__
#define __BarcodeToolPlugin_H__

#include "Plugin.hpp"
#include "AIPanel.h"
//#include "AIFont.h"
#include "SDKDef.h"
#include "SDKAboutPluginsHelper.h"
#include "BarcodeToolPluginSuites.h"
#include <AIUITheme.h>
#include "windows.h"
#include "globvar.h"

	// Panel
	#define ID_CodeEdit 			1
	#define ID_YEdit				2
	#define ID_ResultEdit			3
	#define ID_ButtonAdd			4
	#define ID_ButtonShow			5
	#define ID_ButtonCreate			6
	#define ID_BarType				7
	#define ID_ButtonRotation_0		8
	#define ID_ButtonRotation_90	9
	#define ID_ButtonRotation_180	10
	#define ID_ButtonRotation_270	11
	#define ID_OcrbFont				12
	#define ID_OutlineText			13
	#define ID_WhiteBox				14

using namespace ATE;
using namespace globalvar;

Plugin *AllocatePlugin(SPPluginRef pluginRef);
void FixupReload(Plugin *plugin);

enum ItemType {PANEL};

class BarcodeToolPlugin : public Plugin
{
public:

	BarcodeToolPlugin(SPPluginRef pluginRef);
	virtual ~BarcodeToolPlugin() {}
	
	virtual ASErr StartupPlugin(SPInterfaceMessage *message); 
	virtual ASErr ShutdownPlugin(SPInterfaceMessage *message); 

	void UpdateMenu(AIBoolean isVisible, ItemType item);
	ASErr GoMenuItem(AIMenuMessage *message);
	ASErr Notify(AINotifierMessage *message);


	FIXUP_VTABLE_EX(BarcodeToolPlugin, Plugin);

protected:

	virtual ASErr TrackToolCursor(AIToolMessage* message);

	///**	Sets fStartingPoint to location of pointer during mouse down event.
	//@param message IN pointer to plugin and call information.
	//@return kNoErr on success, other ASErr otherwise.
	//*/
	virtual ASErr ToolMouseUp(AIToolMessage* message);

	/**	Deactivates annotator during mouse up event.
	@param message IN pointer to plugin and call information.
	@return kNoErr on success, other ASErr otherwise.
	*/
private:

	/**	Handle for the About SDK Plug-ins menu item.
	*/
	AIMenuItemHandle fAboutPluginMenu;

	/** Handle for menu item in Window > Third Party Panel menu item.
	*/
	AIMenuItemHandle fBarcodeToolPanelMenuItemHandle;

	/** Pointer to instance of Panel.
	*/
	AIPanelRef fPanel;

	AIPanelFlyoutMenuRef fPanelFlyoutMenu;

	ASErr AddTools(SPInterfaceMessage* message);

	AIMenuItemHandle fBarcodeToolControlBarMenuItemHandle;

	ASErr GetPrefs();

	ASErr SetPrefs();
	
	ASErr AddFont(HWND hwndCombo);

	ASErr AddFontStyle(HWND hwndCombo);

	ASErr AddCodeType(HWND hwndCombo);

	ASErr DrawBarcode(AIReal x, AIReal y);

	ASErr GetSizeFromPanel(float &barSizeH, float &barSizeW);

	AIReal BarcodeToolPlugin::GetMaxCharBottomZize(TCHAR *txt);

	ASErr GetCharSize(TCHAR *txt, AIRealRect &myBounds);

	ASErr GetArtSize(AIReal &x, AIReal &y,float &barSizeH, float &barSizeW, AIArtHandle **store, ai::int32 count);

	ASErr sDraw(TCHAR *XStr, AIReal x, AIReal y, AIReal barSizeH, AIReal barSizeW, AIReal rotation);

	ASErr rotate (AIArtHandle ah);

	ASErr sBarUnit(AIReal x, AIReal y, AIReal barXPosition, AIReal barSizeW, AIReal barSizeH,  bool key, AIArtHandle artGroup);

	ASErr CreatePointText(TCHAR *txt, AIReal x, AIReal y, int XPosition, AIArtHandle artGroup);

	ASErr GetFont(AIFontKey *fontKey);

	ASErr GetFontStyle(int indx, char *fontName);

	ASErr SetFullFontName(int indxFace, int indxStyle);

	ASErr FillFontCombo();

	int chColor(int col, THEMCOLOR *ic);

	ASErr AddWidgets();

	ASErr PostStartupPlugin();

	AIResourceManagerHandle fResourceManagerHandle;

	AIPanelPlatformWindow hDlg;

	//Panel related
	static LRESULT CALLBACK NewWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool PanelWindowProc(LRESULT& result, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK CallDefaultWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	WNDPROC fDefaultWindProc;

	AIToolHandle fToolHandle;
	AIRealPoint fStartingPoint;

	HWND fHWndCodeEdit;
	HWND fHwndHEdit;
	HWND fHwndWEdit;
	HWND fHwndREdit;
	HWND HwndFontCombo;
	HWND HwndFontStyleCombo;
	HWND HwndCodeTypeCombo;
	HWND HwndButtonShow;
	HWND HwndButtonR0;
	HWND HwndButtonR90;
	HWND HwndButtonR180;
	HWND HwndButtonR270;
	HWND HwndChekBoxBox;
	HWND HwndChekBoxOcrb;
	HWND HwndChekBoxOutline;
	HWND HwndChekOCRB;
	HWND HwndButtonCreate;

	AIReal offset; //смещение для рамки
	AIReal scale;
	AIReal rotateRadiant;
	AIRealPoint center;
	int direction = 0;
	/** Назначеный шрифт*/
	char FullFontName[50];

	AIBoolean chkWhiteBox;
	AIBoolean chkOCRB;
	AIBoolean chkOutlineText;
	THEMCOLOR icThem;
	HBRUSH hBrushBkg;
	HFONT  hFont;


	int LbarCode[10][4] = { 4,2,7,1,
						    3,2,7,1,
						    3,1,6,2,
						    2,4,7,1,
							2,1,6,2,
							2,2,7,1,
							2,1,4,4,
							2,3,6,2,
							2,2,5,3,
							4,1,6,2
	};
	int RbarCode[10][4] = { 1,3,6,1,
							1,2,5,2,
							1,2,4,2,
							1,1,6,1,
							1,1,3,3,
							1,1,4,3,
							1,1,3,1,
							1,1,5,1,
							1,1,4,1,
							1,3,5,1
	};
	int GbarCode[10][4] = { 2,1,5,3,
							2,2,6,2,
							3,2,6,2,
							2,1,7,1,
							3,3,7,1,
							2,3,7,1,
							5,1,7,1,
							3,1,7,1,
							4,1,7,1,
							3,1,5,3
	};
	int FirstDigit[10] = 
	{ 
		0b111111,
		0b110100,
		0b110010,
		0b110001,
		0b101100,
		0b100110,
		0b100011,
		0b101010,
		0b101001,
		0b100101
	};
};

#endif	// __BarcodeToolPlugin_H__


//Первая
//	цифра	Первая(левая)
//	группа из 6 цифр	Вторая(правая)
//	группа из 6 цифр
//	0	111111	RRRRRR
//	1	110100	RRRRRR
//	2	110010	RRRRRR
//	3	110001	RRRRRR
//	4	101100	RRRRRR
//	5	100110	RRRRRR
//	6	100011	RRRRRR
//	7	101010	RRRRRR
//	8	101001	RRRRRR
//	9	100101	RRRRRR

//Кодирование цифр
//	Цифра	L - код	R - код	G - код
//	0	0001101	1110010	0100111
//	1	0011001	1100110	0110011
//	2	0010011	1101100	0011011
//	3	0111101	1000010	0100001
//	4	0100011	1011100	0011101
//	5	0110001	1001110	0111001
//	6	0101111	1010000	0000101
//	7	0111011	1000100	0010001
//	8	0110111	1001000	0001001
//	9	0001011	1110100	0010111
