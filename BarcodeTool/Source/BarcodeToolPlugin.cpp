#include <WindowsX.h>
#include "CustomEdit.h"

#include "IllustratorSDK.h"
#include "BarcodeToolPlugin.h"
#include "BarcodeToolPluginSuites.h"
#include "BarcodeToolID.h"
#include "AppContext.hpp"
#include "SDKErrors.h"
#include "AiCheck.h"
#include "AiButton.h"
#include "AiPButton.h"
#include "AiCombo.h"
#include "AIUITheme.h"
#include "..\Resource\Win\resource.h"




#define bufMax 1024
#define CHECK_ERROR  if (error) {return error;}

extern "C" SPBasicSuite*  sSPBasic;

using namespace ATE;

Plugin *AllocatePlugin(SPPluginRef pluginRef)
{
	return new BarcodeToolPlugin(pluginRef);
}

void FixupReload(Plugin *plugin)
{
	BarcodeToolPlugin::FixupVTable( static_cast<BarcodeToolPlugin*>( plugin ) );
}

BarcodeToolPlugin::BarcodeToolPlugin(SPPluginRef pluginRef)
	: Plugin(pluginRef),
	fResourceManagerHandle(NULL),
	fPanel(NULL),
	hDlg(NULL)
{
	strncpy(fPluginName, kBarcodeToolPluginName, kMaxStringLength);
	fDefaultWindProc = NULL;
}

static void flyoutMenuPreVisFunc(AIPanelRef inPanel)
{
	//sAIUser->MessageAlert(ai::UnicodeString("pre visiblity"));
}

ASErr BarcodeToolPlugin::AddTools(SPInterfaceMessage* message)
{
	AIErr error = kNoErr;
	AIAddToolData toolData;

	ai::int32			options = kToolWantsToTrackCursorOption;

	char firstToolName[256];
	ai::UnicodeString toolTitleStr;
	toolTitleStr = ai::UnicodeString(ZREF("Barcode Tool"));

	ai::uint32 iconName;
	iconName =  IDB_PNG1;

	ai::uint32 darkIconName;
	darkIconName = IDB_PNG2;

	std::string toolTitle = toolTitleStr.as_Platform();
	toolData.title = toolTitle.c_str();
	toolData.tooltip = toolTitle.c_str();
	toolData.darkIconResID = darkIconName;
	toolData.normalIconResID = iconName;

	strcpy(firstToolName, toolData.title);
	// New group on tool palette.
	toolData.sameGroupAs = kNoTool;
	// New toolset in new group.
	toolData.sameToolsetAs = kNoTool;

	error = sAITool->AddTool(message->d.self, toolData.title, &toolData,options, &fToolHandle);			
	return error;
}


void PanelFlyoutMenuProc(AIPanelRef inPanel, ai::uint32 itemID)
{
	AIErr error = kNoErr;
	AIPanelFlyoutMenuRef flyoutMenu = NULL;
	error = sAIPanel->GetFlyoutMenu(inPanel, flyoutMenu);
	AIPanelFlyoutMenuItemMark currentItemMark;
	switch(itemID)
	{
	case 1:
		error = sAIPanelFlyoutMenu->GetItemMark(flyoutMenu, 1 , currentItemMark);
		error = sAIPanelFlyoutMenu->SetItemMark(flyoutMenu, 1 , currentItemMark ? kAIPanelFlyoutMenuItemMark_NONE : kAIPanelFlyoutMenuItemMark_CHECK);
		break;
	case 2:
		error = sAIPanelFlyoutMenu->GetItemMark(flyoutMenu, 2 , currentItemMark);
		error = sAIPanelFlyoutMenu->SetItemMark(flyoutMenu, 2 , currentItemMark ? kAIPanelFlyoutMenuItemMark_NONE : kAIPanelFlyoutMenuItemMark_CHECK);
		break;
	case 3:
		error = sAIPanelFlyoutMenu->GetItemMark(flyoutMenu, 3 , currentItemMark);
		error = sAIPanelFlyoutMenu->SetItemMark(flyoutMenu, 3 , currentItemMark ? kAIPanelFlyoutMenuItemMark_NONE : kAIPanelFlyoutMenuItemMark_CHECK);
		break;
	}
}

void PanelVisibilityChangedNotifyProc(AIPanelRef inPanel, AIBoolean isVisible)
{
	AIPanelUserData ud = NULL;
	sAIPanel->GetUserData(inPanel, ud);
	BarcodeToolPlugin* sPlugin = reinterpret_cast<BarcodeToolPlugin*>(ud);
	sPlugin->UpdateMenu(isVisible, PANEL);
}

void PanelSizeChangedNotifyProc(AIPanelRef inPanel)
{

}	

void PanelStateChangedNotifyProc(AIPanelRef inPanel, ai::int16 newState)
{
	AIErr err = kNoErr;
	AISize prefSize;
	switch(newState)
	{
	case 1:
		prefSize.height = 253;
		prefSize.width = 230;
		break;
	case 2:
		prefSize.height = 253;
		prefSize.width = 230;
		break;
	case 3:
		prefSize.height = 253;
		prefSize.width = 230;
		break;
	}
	
	err = sAIPanel->SetPreferredSizes(inPanel, prefSize);
}

void PanelClosedNotifyProc(AIPanelRef inPanel)
{
	//sAIUser->MessageAlert(ai::UnicodeString("Barcode Tool Closed"));
}

// --------------------------------------------------------------------
// Called when plugin is initially loaded
// --------------------------------------------------------------------
ASErr BarcodeToolPlugin::StartupPlugin(SPInterfaceMessage *message)
{
	AIErr error = kNoErr;
	
	// Call base class routine
	error = Plugin::StartupPlugin(message);
	CHECK_ERROR

	AINotifierHandle appShutDownNotifier;
	error = sAINotifier->AddNotifier(fPluginRef, "AI Application Shutdown Notifier", kAIApplicationShutdownNotifier, &appShutDownNotifier);
	CHECK_ERROR

	AINotifierHandle appInterfaceBrightnessChanged;
	error = sAINotifier->AddNotifier(fPluginRef, "AI Interface Brightness Changed", kAIUIBrightnessChangedNotifier, &appInterfaceBrightnessChanged);
	CHECK_ERROR

	// Add About Plugins menu item for this plug-in.
	SDKAboutPluginsHelper aboutPluginsHelper;
	error = aboutPluginsHelper.AddAboutPluginsMenuItem(message, 
				kSDKDefAboutSDKCompanyPluginsGroupName, 
				ai::UnicodeString("About Barcode Tool"), 
				"Barcode Tool...", 
				&fAboutPluginMenu);

	CHECK_ERROR

	// Add menu item
	error = sAIMenu->AddMenuItemZString(fPluginRef, "Barcode Tool", kOtherPalettesMenuGroup, ZREF("Barcode Tool"),
										kMenuItemNoOptions, &fBarcodeToolPanelMenuItemHandle);
	CHECK_ERROR

	fPanelFlyoutMenu = NULL;
	error = sAIPanelFlyoutMenu->Create(fPanelFlyoutMenu);
	CHECK_ERROR

	error = sAIPanelFlyoutMenu->AppendItem(fPanelFlyoutMenu, 1, ai::UnicodeString("First Item"));
    CHECK_ERROR
	error = sAIPanelFlyoutMenu->AppendItem(fPanelFlyoutMenu, 3, ai::UnicodeString("Third Item"));
    CHECK_ERROR
	error = sAIPanelFlyoutMenu->InsertItem(fPanelFlyoutMenu, 3, 2, ai::UnicodeString("Second Item"));
    CHECK_ERROR
	error = sAIPanelFlyoutMenu->InsertSeparator(fPanelFlyoutMenu, 3, 5);
    CHECK_ERROR
	error = sAIPanelFlyoutMenu->AppendItem(fPanelFlyoutMenu, 4, ai::UnicodeString("Fourth Item"));
    CHECK_ERROR
    
	error = sAIPanelFlyoutMenu->SetItemEnabled(fPanelFlyoutMenu, 4, true);
    CHECK_ERROR
	error = sAIPanelFlyoutMenu->SetItemMark(fPanelFlyoutMenu, 1 , kAIPanelFlyoutMenuItemMark_CHECK);
    CHECK_ERROR
	error = sAIPanelFlyoutMenu->SetItemMark(fPanelFlyoutMenu, 2 , kAIPanelFlyoutMenuItemMark_CHECK);
    CHECK_ERROR
	error = sAIPanelFlyoutMenu->SetItemMark(fPanelFlyoutMenu, 3 , kAIPanelFlyoutMenuItemMark_CHECK);
	CHECK_ERROR
    
	AISize pnSize = { 230, 253};
	error = sAIPanel->Create(fPluginRef, ai::UnicodeString("Barcode Tool"), ai::UnicodeString("Barcode Tool"), 3, pnSize, true, fPanelFlyoutMenu, this, fPanel);
	CHECK_ERROR

	AISize minSize = { 230, 253 };
	AISize maxSize = { 230, 253 };
	AISize prefConstSize = { 230, 253 };
	AISize prefUnconstSize = { 230, 253 };

	error = sAIPanel->SetSizes(fPanel, minSize, prefUnconstSize, prefConstSize, maxSize);
	CHECK_ERROR
	error = sAIPanel->Show(fPanel, true);
      CHECK_ERROR
	error = sAIPanel->SetFlyoutMenuPreVisibilityProc(fPanel, flyoutMenuPreVisFunc);
      CHECK_ERROR
	error = sAIPanel->SetFlyoutMenuProc(fPanel, PanelFlyoutMenuProc);
      CHECK_ERROR
	error = sAIPanel->SetVisibilityChangedNotifyProc(fPanel, PanelVisibilityChangedNotifyProc);
      CHECK_ERROR
	error = sAIPanel->SetSizeChangedNotifyProc(fPanel, PanelSizeChangedNotifyProc);
      CHECK_ERROR
	error = sAIPanel->SetStateChangedNotifyProc(fPanel, PanelStateChangedNotifyProc);
      CHECK_ERROR
	error = sAIPanel->SetClosedNotifyProc(fPanel, PanelClosedNotifyProc);
	CHECK_ERROR
	
	error = AddTools(message);
	CHECK_ERROR

	error = AddWidgets();

	return error;
}

ASErr BarcodeToolPlugin::PostStartupPlugin()
{
	AIErr result = kNoErr;
	result = sAIUser->CreateCursorResourceMgr(fPluginRef, &fResourceManagerHandle);
	return result;
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

AIErr BarcodeToolPlugin::AddWidgets()
{
	AIErr error = kNoErr;
	hDlg = NULL;
	HMODULE HmyModule = GetCurrentModule();
	HBITMAP hBitmap = NULL;
	error = sAIPanel->GetPlatformWindow(fPanel, hDlg);

	hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft Sans Serif");

	HwndCodeTypeCombo = CreateWindowEx(
		NULL, L"COMBOBOX",//L"EDIT",   // Predefined class; Unicode assumed. 
		L"",		//  
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,  // Styles. 
		65,         // x position. 
		5,         // y position. 
		160,        // width.
		20,        // height.
		hDlg,       // Parent window.
		NULL,NULL,NULL);
	SendMessage(HwndCodeTypeCombo, WM_SETFONT, WPARAM(hFont), TRUE);

	//-----------------------Rotation------------------------------

	fHwndREdit = CreateWindowEx(
		WS_EX_TRANSPARENT, L"EDIT",   // Predefined class; Unicode assumed. 
		L"",		//  
		WS_VISIBLE | WS_CHILD | ES_LEFT | WS_TABSTOP | ES_MULTILINE,  // Styles. 
		68,         // x position. 
		38,         // y position. 
		36,        // width.
		20,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_YEdit,
		NULL, NULL);      // Pointer not needed.
	SendMessage(fHwndREdit, WM_SETFONT, WPARAM(hFont), TRUE);

	HwndButtonR0 = AiButton::CreateAiButton(
		0,// L"BUTTON",   // Predefined class; Unicode assumed. 
		L"R0",		//  
		WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_AUTORADIOBUTTON | BS_ICON,  // Styles. 
		124,         // x position. 
		32,         // y position. 
		26,        // width.
		26,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_ButtonRotation_0,
		NULL, NULL);      // Pointer not needed.
	hBitmap = (HBITMAP)LoadImage(HmyModule, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT);
	SendMessage(HwndButtonR0, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hBitmap);

	HwndButtonR90 = AiButton::CreateAiButton(
		0,// L"BUTTON",   // Predefined class; Unicode assumed. 
		L"R90",		//  
		WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_AUTORADIOBUTTON | BS_ICON,  // Styles. 
		149,         // x position. 
		32,         // y position. 
		26,        // width.
		26,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_ButtonRotation_90,
		NULL, NULL);      // Pointer not needed.
	hBitmap = (HBITMAP)LoadImage(HmyModule, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT);
	SendMessage(HwndButtonR90, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hBitmap);

	HwndButtonR180 = AiButton::CreateAiButton(
		0,// L"BUTTON",   // Predefined class; Unicode assumed. 
		L"R180",		//  
		WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_AUTORADIOBUTTON | BS_ICON,  // Styles. 
		174,         // x position. 
		32,         // y position. 
		26,        // width.
		26,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_ButtonRotation_180,
		NULL, NULL);      // Pointer not needed.
	hBitmap = (HBITMAP)LoadImage(HmyModule, MAKEINTRESOURCE(IDI_ICON3), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT);
	SendMessage(HwndButtonR180, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hBitmap);

	HwndButtonR270 = AiButton::CreateAiButton(
		0,// L"BUTTON",   // Predefined class; Unicode assumed. 
		L"R270",		//  
		WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_AUTORADIOBUTTON | BS_ICON,  // Styles. 
		199,         // x position. 
		32,         // y position. 
		26,        // width.
		26,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_ButtonRotation_270,
		NULL, NULL);      // Pointer not needed.
	hBitmap = (HBITMAP)LoadImage(HmyModule, MAKEINTRESOURCE(IDI_ICON4), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT);
	SendMessage(HwndButtonR270, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hBitmap);

	//-----------------------------Code---------------------------------------------

	fHWndCodeEdit = CustomEdit::CreateCustomEdit(
		WS_EX_TRANSPARENT, //L"EDIT", // Predefined class; Unicode assumed. 
		L"",		//  
		WS_VISIBLE | WS_CHILD | ES_LEFT | ES_NUMBER | WS_TABSTOP  | ES_MULTILINE,// | WS_CLIPCHILDREN,  // Styles. 
		68,         // x position. 
		67,         // y position. 
		156,        // width.
		20,         // height.
		hDlg,       // Parent window.
		(HMENU)ID_CodeEdit,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(fHWndCodeEdit, WM_SETFONT, WPARAM(hFont), TRUE);
	Edit_LimitText(fHWndCodeEdit, 13);
	
	//----------------------Dimention----------------------------------

	fHwndWEdit = CreateWindowEx(
		WS_EX_TRANSPARENT, L"EDIT",   // Predefined class; Unicode assumed. 
		L"",		//  
		WS_VISIBLE | WS_CHILD | ES_LEFT | WS_TABSTOP | ES_MULTILINE,  // Styles. 
		68,         // x position. 
		93,         // y position. 
		61,        // width.
		20,        // height.
		hDlg,       // Parent window.
		NULL,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE), 
		NULL);      // Pointer not needed.
	SendMessage(fHwndWEdit, WM_SETFONT, WPARAM(hFont), TRUE);

	fHwndHEdit = CreateWindowEx(
		WS_EX_TRANSPARENT, L"EDIT",   // Predefined class; Unicode assumed. 
		L"",		//  
		WS_VISIBLE | WS_CHILD | ES_LEFT | WS_TABSTOP | ES_MULTILINE,   // Styles. 
		163,         // x position. 
		93,         // y position. 
		61,        // width.
		20,        // height.
		hDlg,       // Parent window.
		NULL,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(fHwndHEdit, WM_SETFONT, WPARAM(hFont), TRUE);

	HwndChekBoxBox = AiCheck::CreateAiCheck(
		0, //L"BUTTON",   // Predefined class; Unicode assumed. 
		L"",		//  
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, // | BS_PUSHLIKE,   // Styles. 
		5,         // x position. 
		116,         // y position. 
		70,        // width.
		15,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_WhiteBox,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(HwndChekBoxBox, WM_SETFONT, WPARAM(hFont), TRUE);
	
//-----------------------Font-------------------------------
	HwndChekBoxOcrb = AiCheck::CreateAiCheck(
		0, //L"BUTTON",   // Predefined class; Unicode assumed. 
		L"",		//  
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, // | BS_PUSHLIKE,   // Styles. 
		5,         // x position. 
		139,         // y position. 
		70,        // width.
		15,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_OcrbFont,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(HwndChekBoxOcrb, WM_SETFONT, WPARAM(hFont), TRUE);

	HwndChekBoxOutline = AiCheck::CreateAiCheck(
		0, //L"BUTTON",   // Predefined class; Unicode assumed. 
		L"",		//  
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles. 
		100,         // x position. 
		139,         // y position. 
		70,        // width.
		15,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_OutlineText,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.
	SendMessage(HwndChekBoxOutline, WM_SETFONT, WPARAM(hFont), TRUE);
	// font name
	HwndFontCombo = AiCombo::CreateAiCombo(
		0, //L"BUTTON",   // Predefined class; Unicode assumed. 
		L"Font Family",		//  
		WS_CHILD,  // Styles. 
		65,         // x position. 
		160,         // y position. 
		160,        // width.
		20,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_ResultEdit,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
		NULL,
		hFont);      // Pointer not needed.
	SendMessage(HwndFontCombo, WM_USER, 0, (LPARAM)&icThem);
	// font style

	HwndFontStyleCombo = AiCombo::CreateAiCombo(
		0, //L"BUTTON",   // Predefined class; Unicode assumed. 
		L"Font Style",		//  
		WS_CHILD,  // Styles. 
		65,         // x position. 
		187,         // y position. 
		160,        // width.
		20,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_ResultEdit,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
		NULL,
		hFont);      // Pointer not needed.
	SendMessage(HwndFontStyleCombo, WM_USER, 0, (LPARAM)&icThem);

	//-----------------------Botton----------------------------------
	HwndButtonCreate = AiPButton::CreateAiPButton(
		0, //L"BUTTON",   // Predefined class; Unicode assumed. 
		L"Replace selection",		//  
		WS_VISIBLE | WS_CHILD | ES_LEFT,  // Styles. 
		30,         // x position. 
		213,         // y position. 
		100,        // width.
		26,        // height.
		hDlg,       // Parent window.
		(HMENU)ID_ButtonCreate,
		(HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

//------------------------------------------------------------------------------------------
	//LPVOID lpMsgBuf;

	//FormatMessage(
	//	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	//	NULL,
	//	GetLastError(),
	//	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	//	(LPTSTR)&lpMsgBuf,
	//	0,
	//	NULL
	//);

	//// Display the string.
	//MessageBox(NULL, (LPTSTR)lpMsgBuf, L"GetLastError", MB_OK | MB_ICONINFORMATION);

	//// Free the buffer.
	//LocalFree(lpMsgBuf);


	SendMessage(HwndButtonR0, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(HwndButtonShow, WM_SETFONT, WPARAM(hFont), TRUE);
	SendMessage(HwndButtonCreate, WM_SETFONT, WPARAM(hFont), TRUE); 
	SendMessage(fHWndCodeEdit, (UINT) WM_SETTEXT, 0, (LPARAM)L"0000000000000");

	SendMessage(fHwndHEdit, (UINT) WM_SETTEXT, 0, (LPARAM)L"25,93 mm");

	SendMessage(fHwndWEdit, (UINT)WM_SETTEXT, 0, (LPARAM)L"37,29 mm");
	SendMessage(fHwndREdit, (UINT)WM_SETTEXT, 0, (LPARAM)L"0°");

	SendMessage(HwndChekBoxBox, WM_SETTEXT, 0, (LPARAM)L"White Box");
	SendMessage(HwndChekBoxOcrb, WM_SETTEXT, 0, (LPARAM)L"OCRB Font");
	SendMessage(HwndChekBoxOutline, WM_SETTEXT, 0, (LPARAM)L"Outline text");

	error = AddCodeType(HwndCodeTypeCombo);

	SendMessage(HwndChekBoxOcrb, WM_USER, 0, (LPARAM)&icThem);
	SendMessage(HwndButtonR0, WM_USER, 0, (LPARAM)&icThem);
	SendMessage(HwndButtonCreate, WM_USER, 0, (LPARAM)&icThem);
	
	::SetPropA(hDlg, "TPNL", this);
	fDefaultWindProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hDlg, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(BarcodeToolPlugin::NewWindowProc)));
	
	error = GetPrefs();

	return error;
}

ASErr BarcodeToolPlugin::GetPrefs()
{
	AIErr error = kNoErr;

	error = sAIPref->GetStringPreference("BarcodeTool", "FontName", FullFontName);
	if (error) return error;

	chkOCRB = true;
	error = sAIPref->GetBooleanPreference("BarcodeTool", "OCRBFont", &chkOCRB);
	SendMessage(HwndChekBoxOcrb, BM_SETCHECK, chkOCRB, 0);
	if (error) return error;

	chkOutlineText = true;
	error = sAIPref->GetBooleanPreference("BarcodeTool", "OutlineText", &chkOutlineText);
	SendMessage(HwndChekBoxOutline, BM_SETCHECK, chkOutlineText, 0);
	if (error) return error;

	chkWhiteBox = true;
	error = sAIPref->GetBooleanPreference("BarcodeTool", "WhiteBox", &chkWhiteBox);
	SendMessage(HwndChekBoxBox, BM_SETCHECK, chkWhiteBox, 0);

	FillFontCombo();
	return error;
}

ASErr BarcodeToolPlugin::SetPrefs()
{
	AIErr error = kNoErr;
	error = sAIPref->PutStringPreference("BarcodeTool", "FontName", FullFontName);
	if (error) return error;
	error = sAIPref->PutBooleanPreference("BarcodeTool", "OCRBFont", chkOCRB);
	if (error) return error;
	error = sAIPref->PutBooleanPreference("BarcodeTool", "WhiteBox", chkWhiteBox);
	if (error) return error;
	error = sAIPref->PutBooleanPreference("BarcodeTool", "OutlineText", chkOutlineText);
	return error;
}

LRESULT CALLBACK BarcodeToolPlugin::NewWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BarcodeToolPlugin* panel = (BarcodeToolPlugin*)GetPropA(hWnd, "TPNL");
	LRESULT result = 0;
	bool handled = panel->PanelWindowProc(result, hWnd, msg, wParam, lParam);
	if(!handled)
		result =  panel->CallDefaultWindowProc(hWnd, msg, wParam, lParam);
	return result;
}

bool BarcodeToolPlugin::PanelWindowProc(LRESULT& result, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	AIErr error = kNoErr;
	bool handled = false;
	HDC dc;
	PAINTSTRUCT ps;
	RECT rect;

	switch (msg) 
	{
	case WM_PAINT:
	{
			HBRUSH hBrushBackground = CreateSolidBrush(icThem.ICBackgroundColor);

			HPEN hPen = CreatePen(PS_SOLID, 1, icThem.ICBtnFrameColor);

			GetClientRect(hWnd, &rect);
			dc = BeginPaint(hWnd, &ps);
			FillRect(dc, &rect, hBrushBackground);

			SelectObject(dc, hPen);
			SelectObject(dc, icThem.ICTextEditBackgroundBrush);

			//Code:
			RoundRect(dc, 65, 64, 65 + 160, 20 + 64, 0, 0);
			//Rotation:
			RoundRect(dc, 65, 35, 40 + 65, 20 + 35, 0, 0);
			//W:
			RoundRect(dc,65,90,65+65,20+90,0,0);
			//H:
			RoundRect(dc, 160, 90, 65 + 160, 20 + 90, 0, 0);

			SelectObject(ps.hdc, hFont);
			SetBkMode(dc, TRANSPARENT);
			SetTextColor(dc, icThem.ICTextColor);
			SetTextAlign(dc, TA_RIGHT);
			TextOut(dc, 60, 9, L"Code Type:", 10);
			TextOut(dc, 60, 37, L"Rotation:", 9);
			TextOut(dc, 60, 67, L"Code:", 5);
			TextOut(dc, 60, 93, L"W:", 2);
			TextOut(dc, 155, 93, L"H:", 2);
			TextOut(dc, 60, 140, L"Font Name:", 10);
			TextOut(dc, 60, 167, L"Style:", 6);

			EndPaint(hWnd, &ps);
			DeleteObject(hBrushBackground);
			DeleteObject(hPen);
			return 0;
	}

	case WM_CTLCOLOREDIT:
	{
		SetTextColor((HDC)wParam, icThem.ICTextEditColor);
		SetBkColor((HDC)wParam, icThem.ICTextEditBackgroundColor);

		return  icThem.ICTextEditBackgroundBrush;
	}
	break;

	case AIC_OPENLIST:
	{
		if ((HWND)wParam == HwndFontCombo)
		{
			error = AddFont((HWND)lParam);  
		} 
		else if ((HWND)wParam == HwndFontStyleCombo)
		{
			error = AddFontStyle((HWND)lParam);
		}
		return 0;
	}
	break;
	case AIC_ITEMSELECT:
	{
		if ((HWND)wParam == HwndFontCombo)
		{
			char fontStyleName[50];
			GetFontStyle(lParam, fontStyleName);  //lParam - illustrator font index
			SendMessageA(HwndFontStyleCombo, AIC_SETTEXT, 0, (LPARAM)fontStyleName);
			if (lParam > -1)
			{
				SetFullFontName(lParam, 0);
			}
			else
			{
				strcpy(FullFontName, "[default font]");
			}
		}
		if ((HWND)wParam == HwndFontStyleCombo)
		{
			SetFullFontName(-1, lParam);
		}
		return 0;
	}
	break;
	case WM_COMMAND: 
		{
			if(wParam == ID_ButtonRotation_0)
			{
				direction = 0;
				handled = true;
			}
			else if (wParam == ID_ButtonRotation_90)
			{
				direction = 90;
				handled = true;
			}
			else if (wParam == ID_ButtonRotation_180)
			{
				direction = 180;
				handled = true;
			}
			else if (wParam == ID_ButtonRotation_270)
			{
				direction = 270;
				handled = true;
			}
			else if (wParam == ID_ButtonCreate)
			{
				DrawBarcode(0, 0);
				handled = true;
			}
			else if (wParam == ID_OutlineText)
			{
				chkOutlineText = SendMessage(HwndChekBoxOutline, BM_GETCHECK, 0, 0);
				handled = true;
			}
			else if (wParam == ID_OcrbFont)
			{
				chkOCRB = SendMessage(HwndChekBoxOcrb, BM_GETCHECK, 0, 0);
				handled = true;
			}
			else if (wParam == ID_WhiteBox)
			{
				chkWhiteBox= SendMessage(HwndChekBoxBox, BM_GETCHECK, 0, 0);
				handled = true;
			}
		}
	}

	return handled;
}

LRESULT CALLBACK BarcodeToolPlugin::CallDefaultWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(fDefaultWindProc)
		return ::CallWindowProc(fDefaultWindProc, hWnd, msg, wParam, lParam);
	else
	{
		return 0; 
	}
}

ASErr BarcodeToolPlugin::ShutdownPlugin(SPInterfaceMessage *message)
{
	AIErr error = kNoErr;
	error = SetPrefs();
	if(fPanel)
	{
		error = sAIPanel->Destroy(fPanel);
		fPanel = NULL;
	}
	if(fPanelFlyoutMenu)
	{
		error = sAIPanelFlyoutMenu->Destroy(fPanelFlyoutMenu);
		fPanelFlyoutMenu = NULL;
	}
	return kNoErr;
}

ASErr BarcodeToolPlugin::GoMenuItem(AIMenuMessage *message)
{
	AIErr error = kNoErr;
	if (message->menuItem == fAboutPluginMenu) 
	{
		SDKAboutPluginsHelper aboutPluginsHelper;
		aboutPluginsHelper.PopAboutBox(message, "About Barcode Tool", kSDKDefAboutSDKCompanyPluginsAlertString);
	}
	else if (message->menuItem == fBarcodeToolPanelMenuItemHandle)
	{	
		if(fPanel)
		{
			AIBoolean isShown = false;
			error = sAIPanel->IsShown(fPanel, isShown);
			error = sAIPanel->Show(fPanel, !isShown);
		}
	}

	return error;
}

void BarcodeToolPlugin::UpdateMenu(AIBoolean isVisible, ItemType item)
{
	if (item == PANEL)
		sAIMenu->CheckItem (fBarcodeToolPanelMenuItemHandle , isVisible);
}

ASErr BarcodeToolPlugin::ToolMouseUp(AIToolMessage* message)
{
	ASErr error = kNoErr;

	// we want our initial mouse down to base the drag on later	
	fStartingPoint = message->cursor;

	// Activate annotator.
	//error = sAIAnnotator->SetAnnotatorActive(fAnnotatorHandle, true);

	DrawBarcode(fStartingPoint.h, fStartingPoint.v);
	return error;
}
ASErr BarcodeToolPlugin::TrackToolCursor(AIToolMessage* message)
{
	AIErr error = kNoErr;
	ai::int32 cursorId = IDB_PNG3;
	//if (direction == 0) { cursorId = IDB_PNG3; }
	if (direction == 90) { cursorId = IDB_PNG4; }
	if (direction == 180) { cursorId = IDB_PNG5; }
	if (direction == 270) { cursorId = IDB_PNG6; }

	if (sAIUser != NULL)
		error = sAIUser->SetCursor(cursorId, fResourceManagerHandle);

	return error;
}
ASErr BarcodeToolPlugin::Notify (AINotifierMessage *message)
{
	AIErr result = kNoErr;
	if(strcmp(message->type, kAIApplicationShutdownNotifier) == 0)
	{

		if (fResourceManagerHandle != NULL)
		{
			result = sAIUser->DisposeCursorResourceMgr(fResourceManagerHandle);
			fResourceManagerHandle = NULL;
		}
		
		if(fPanel)
		{
			AIPanelPlatformWindow panelPlatfromWindow = NULL;
			result = sAIPanel->GetPlatformWindow(fPanel, panelPlatfromWindow);

			if(panelPlatfromWindow)
			{
				RemovePropA(panelPlatfromWindow, "TPNL");
				SetWindowLongPtr(panelPlatfromWindow, GWLP_WNDPROC, (LONG_PTR)(fDefaultWindProc));
				DestroyWindow(panelPlatfromWindow);
			}

			result = sAIPanel->Destroy(fPanel);
			fPanel = NULL;
		}
		if(fPanelFlyoutMenu)
		{
			result = sAIPanelFlyoutMenu->Destroy(fPanelFlyoutMenu);
			fPanelFlyoutMenu = NULL;
		}

	}

	if (strcmp (message->type, kAIUIBrightnessChangedNotifier) == 0)
	{//Current UI background color.

		AIUIThemeColor UIBackgroundColor;
		AIUIThemeColor UITextColor;
		AIUIThemeColor UITextEditBackgroundColor;
		AIUIThemeColor UITextEditColor;
		AIUIThemeColor UIColorFocusRing;

		result = sAIUITheme->GetUIThemeColor(kAIUIThemeSelectorPanel, kAIUIComponentColorBackground, UIBackgroundColor);

		//Current UI text color.
		result = sAIUITheme->GetUIThemeColor(kAIUIThemeSelectorPanel, kAIUIComponentColorText, UITextColor);

		//Current UI text edit background color.
		result = sAIUITheme->GetUIThemeColor(kAIUIThemeSelectorPanel, kAIUIComponentColorEditTextBackground, UITextEditBackgroundColor);

		//Current UI text edit color.
		result = sAIUITheme->GetUIThemeColor(kAIUIThemeSelectorPanel, kAIUIComponentColorEditText, UITextEditColor);

		result = sAIUITheme->GetUIThemeColor(kAIUIThemeSelectorPanel, kAIUIComponentColorFocusRing, UIColorFocusRing);

		icThem.ICBackgroundColor = (RGB(UIBackgroundColor.red * 255 , UIBackgroundColor.green * 255, UIBackgroundColor.blue * 255));
		icThem.ICTextColor = (RGB(UITextColor.red * 255, UITextColor.green * 255, UITextColor.blue * 255));
		icThem.ICTextEditBackgroundColor = (RGB(UITextEditBackgroundColor.red * 255, UITextEditBackgroundColor.green * 255, UITextEditBackgroundColor.blue * 255));
		icThem.ICTextEditColor = (RGB(UITextEditColor.red * 255, UITextEditColor.green * 255, UITextEditColor.blue * 255));
		icThem.ICFocusRingColor = (RGB(UIColorFocusRing.red * 255, UIColorFocusRing.green * 255, UIColorFocusRing.blue * 255));
		chColor(UIBackgroundColor.red * 255, &icThem);
		icThem.ICTextEditBackgroundBrush = CreateSolidBrush(icThem.ICTextEditBackgroundColor);

		//ai::NumberFormat numFormat;
		//ai::UnicodeString strResult;
		//sAIStringFormatUtils->IntegerToString(numFormat, UIBackgroundColor.red * 255, strResult);
		//sAIUser->MessageAlert(ai::UnicodeString(strResult));
	}

	return result;
}

int BarcodeToolPlugin::chColor(int col, THEMCOLOR *ic)
{
	if (col > 230)
	{
		ic->ICBackgroundColor = RGB(240, 240, 240);
		ic->ICCheckColor = RGB(109, 109, 109);
		ic->ICHoverColor = RGB(72, 72, 72);
		ic->ICBtnUpColor = RGB(224, 224, 224);
		ic->ICBtnDownColor = RGB(204, 204, 204);
		ic->ICBtnFrameColor = RGB(175, 175, 175);
		col = 240;
		return col;
	}
	if (col > 180)
	{
		ic->ICBackgroundColor = RGB(184, 184, 184);
		ic->ICCheckColor = RGB(74, 74, 74);
		ic->ICHoverColor = RGB(35, 35, 35);
		ic->ICBtnUpColor = RGB(168, 168, 168);
		ic->ICBtnDownColor = RGB(145, 145, 145);
		ic->ICBtnFrameColor = RGB(120, 120, 120);
		col = 184;
		return col;
	}
	if (col > 80)
	{
		ic->ICBackgroundColor = RGB(84, 84, 84);
		ic->ICCheckColor = RGB(199, 199, 199);
		ic->ICHoverColor = RGB(255, 255, 255);
		ic->ICBtnUpColor = RGB(100, 100, 100);
		ic->ICBtnDownColor = RGB(60, 60, 60);
		ic->ICBtnFrameColor = RGB(40, 40, 40);
		col = 84;
		return col;
	}
	ic->ICBackgroundColor = RGB(51, 51, 51);
	ic->ICCheckColor = RGB(153, 153, 153);
	ic->ICHoverColor = RGB(205, 205, 205);
	ic->ICBtnUpColor = RGB(60, 60, 60);
	ic->ICBtnDownColor = RGB(30, 30, 30);
	ic->ICBtnFrameColor = RGB(10, 10, 10);
	col = 51;
	return col;
}

ASErr BarcodeToolPlugin::DrawBarcode(AIReal x, AIReal y)
{
	AIErr error = kNoErr;
	AIAppContextHandle AppContext;
	sAIAppContext->PushAppContext(GetPluginRef(), &AppContext);

	ai::int32 count = 0;

	TCHAR CodeStr[255];
	SendMessage(fHWndCodeEdit, (UINT)WM_GETTEXT, 255, (LPARAM)CodeStr);

	while (CodeStr[count] != '\0')
	{
		count++;
	}
	if (count != 13) {
		return error;
	}

	AIArtHandle **store = NULL;
	float barSizeH;
	float barSizeW;

	if (sAIMatchingArt->GetSelectedArt(&store, &count))
	{
		store = NULL;
	}
	if (store != NULL)
	{
		error = GetArtSize(x,y,barSizeH, barSizeW, store, count);
	}
	else
	{
		error = GetSizeFromPanel(barSizeH, barSizeW);
	}

	//ai::NumberFormat numFormat;
	//ai::UnicodeString strResult;
	//sAIStringFormatUtils->RealToString(numFormat, barSizeW, 3, false, strResult);
	//sAIUser->MessageAlert(ai::UnicodeString(strResult));

	error = sDraw(CodeStr, x, y, barSizeH, barSizeW, 0);
	if (error) return error;

	AIWindowRef appWindow;
	error = sAIAppContext->GetPlatformAppWindow(&appWindow);
	aisdk::check_ai_error(error);
	SendMessage(appWindow, WM_SETFOCUS, 0, 0);
	sAIAppContext->PopAppContext(AppContext);
	return error;
}

ASErr BarcodeToolPlugin::GetSizeFromPanel(float &barSizeH, float &barSizeW)
{
	ASErr error = kNoErr;
	ai::int32 count = 0;
	//----------------------Height------------------------		
	TCHAR Str[255];
	SendMessage(fHwndHEdit, (UINT)WM_GETTEXT, 255, (LPARAM)Str);
	count = 0;
	while ((Str[count] != '\0'))
	{
		if (Str[count] == '.')
		{
			Str[count] = ',';
		}
		count++;
	}

	ai::NumberFormat numFormat;
	ai::UnicodeString strResult;

	sAIStringFormatUtils->StringToReal(numFormat, ai::UnicodeString(Str), barSizeH);
	sAIStringFormatUtils->RealToString(numFormat, barSizeH, 3, false, strResult);
	barSizeH /= K_p_to_mm;
	error = sAIUnicodeString->Append(strResult, ai::UnicodeString(" mm"));
	SendMessage(fHwndHEdit, (UINT)WM_SETTEXT, 0, (LPARAM)strResult.as_ASUnicode().c_str());

	//----------------------Width------------------------		
	SendMessage(fHwndWEdit, (UINT)WM_GETTEXT, 255, (LPARAM)Str);
	count = 0;
	while ((Str[count] != '\0'))
	{
		if (Str[count] == '.')
		{
			Str[count] = ',';
		}
		count++;
	}

	sAIStringFormatUtils->StringToReal(numFormat, ai::UnicodeString(Str), barSizeW);
	sAIStringFormatUtils->RealToString(numFormat, barSizeW, 3, false, strResult);
	barSizeW /= K_p_to_mm;
	error = sAIUnicodeString->Append(strResult, ai::UnicodeString(" mm"));
	SendMessage(fHwndWEdit, (UINT)WM_SETTEXT, 0, (LPARAM)strResult.as_ASUnicode().c_str());
	return error;
}
ASErr BarcodeToolPlugin::GetArtSize(AIReal &x, AIReal &y, float &barSizeH, float &barSizeW, AIArtHandle **store, ai::int32 count)
{
	ASErr error = kNoErr;
	AIRealRect myBounds;
	short artType = kUnknownArt;
	AIArtHandle artGroup = NULL;

	error = sAIArt->NewArt(kGroupArt, kPlaceDefault, artGroup, &artGroup);
	if (error) return error;
	for (int i = 1; i < count; i++)
	{
		AIArtHandle art = (*store)[i];
		error = sAIArt->GetArtType(art, &artType);
		if (error) return error;

		switch (artType)
		{
		case kGroupArt:
		case kCompoundPathArt:
			// nothing
			break;
		default:
			error = sAIArt->ReorderArt(art, kPlaceInsideOnTop, artGroup);
			if (error) return error;
			break;
		}
	}
	error = sAIArt->AIArtSuite::GetArtTransformBounds(artGroup, NULL, kVisibleBounds, &myBounds);
	if (!error) {
		x = myBounds.left;
		y = myBounds.bottom;
		if ((direction == 0) | (direction == 180))
		{
			barSizeH = myBounds.top - y;
			barSizeW = myBounds.right- x;
		}
		else
		{
			barSizeW = myBounds.top - y;
			barSizeH = myBounds.right - x;
		}
		error = sAIArt->DisposeArt(artGroup);
		if (error) return error;
		artGroup = NULL;
		sAIMdMemory->MdMemoryDisposeHandle((AIMdMemoryHandle)store);
	}
	return error;
}
ASErr BarcodeToolPlugin::GetCharSize(TCHAR *txt, AIRealRect &myBounds)
{
	ASErr error = kNoErr;
	try {

		AIArtHandle artGroup = NULL;
		error = sAIArt->NewArt(kGroupArt, kPlaceDefault, artGroup, &artGroup);

		// Add the new point text item to the layer.
		AITextOrientation orient = kHorizontalTextOrientation;
		AIRealPoint anchor = { 0, 0 };
		AIArtHandle textFrame = NULL;
		error = sAITextFrame->NewPointText(kPlaceInsideOnTop, artGroup, orient, anchor, &textFrame);
		aisdk::check_ai_error(error);

		// Set the contents of the text range.
		TextRangeRef range = NULL;
		error = sAITextFrame->GetATETextRange(textFrame, &range);
		aisdk::check_ai_error(error);
		ITextRange crange(range);
		crange.InsertAfter(ai::UnicodeString(txt).as_ASUnicode().c_str());

		ICharFeatures features;
		IParaFeatures ParaFeatures;
		ParaFeatures.SetJustification(kCenterJustify);
		ICharInspector inspector = crange.GetCharInspector();

		AIFontKey fontKey;
		FontRef BfontRef;
		AIReal fontSize = 12;
		AIRealRect rect;
		error = GetFont(&fontKey);
		if (!error)
		{
			error = sAIFont->FontFromFontKey(fontKey, &BfontRef);
			IFont font(BfontRef);
			features.SetFont(font);
		}
		features.SetFontSize(fontSize);
		crange.SetLocalCharFeatures(features);
		crange.SetLocalParaFeatures(ParaFeatures);

		AIArtHandle textFrameOutline = NULL;
		error = sAITextFrame->CreateOutline(textFrame, &textFrameOutline);
		error = sAIArt->DisposeArt(textFrame);
		error = sAIArt->AIArtSuite::GetArtTransformBounds(artGroup, NULL, kNoStrokeBounds, &myBounds);

		error = sAIArt->DisposeArt(artGroup);
		if (error) return error;
		artGroup = NULL;

	}
	catch (ai::Error& ex) {
		error = ex;
	}
	catch (ATE::Exception& ex) {
		error = ex.error;
	}
	return error;
}
AIReal BarcodeToolPlugin::GetMaxCharBottomZize(TCHAR *txt)
{
	ASErr error = kNoErr;
	AIRealRect myBounds;
	AIReal bottom = 0;
	int count = 0;
	TCHAR digit[2] = { '\0','\0' };
		while ((txt[count] != '\0'))
	{
		digit[0] = txt[count];
		error = GetCharSize(digit, myBounds);
		if (bottom > myBounds.bottom) bottom = myBounds.bottom;
		count++;
	}
		return bottom;
}
ASErr BarcodeToolPlugin::sDraw(TCHAR *XStr, AIReal x, AIReal y, AIReal barSizeH, AIReal barSizeW, AIReal rotation)
{
	ASErr error = kNoErr;
	int codeDigit;
	int barOffset;
	int charOffset;
	int firstDigitItem = 0;
	//x = x;
	//y = y;
	AIArtHandle artGroup = NULL;
	AIArtHandle path;
	AIArtHandle digitArt = NULL;
	AIArtHandle barArt = NULL;

	AIPathStyle pathStyle;
	AIPathStyleMap pathStyleMap;
	AIDictionaryRef advStrokeParams = NULL;
	TCHAR digit[2] = { '\0','\0' };

	if (chkWhiteBox) 
	{ 
		offset = 0.33 / K_p_to_mm;
		scale = barSizeW / (37.29 / K_p_to_mm); 
		charOffset = 4;
		barOffset = 11;

	}
	else 
	{
		AIRealRect myBounds;
		digit[0] = XStr[0];
		error = GetCharSize(digit,myBounds);
		offset = 0;
		scale = barSizeW / (95.41489 - myBounds.left);
		charOffset = 0;
		barOffset = 7;
		AIReal bottom = GetMaxCharBottomZize(XStr);
		barSizeH += (bottom * scale);
		switch (direction)
		{
		case 0:
			x = x - (myBounds.left * scale);
			y -= (bottom * scale);
			break;
		case 90:
			y = y - (myBounds.left * scale);
			break;
		case 180:
			x = x + (myBounds.left * scale);
			break;
		case 270:
			y = y + (myBounds.left * scale);
			x -= (bottom * scale);
			break;
		}
	}

	TCHAR RStr[255];
	SendMessage(fHwndREdit, (UINT)WM_GETTEXT, 255, (LPARAM)RStr);
	ai::NumberFormat numFormat;
	ai::UnicodeString strResult;
	float rot = 0;
	sAIStringFormatUtils->StringToReal(numFormat, ai::UnicodeString(RStr), rot);
	sAIStringFormatUtils->RealToString(numFormat, rot, 3, false, strResult);
	SendMessage(fHwndREdit, (UINT)WM_SETTEXT, 0, (LPARAM)strResult.as_ASUnicode().c_str());
	rot = rot + direction;
	rotateRadiant = sAIRealMath->DegreeToRadian(rot);

	if ((direction == 90) | (direction == 270))
	{
		x = x - (barSizeW - barSizeH) / 2;
		y = y - (barSizeH - barSizeW) / 2;
	}

	center.h = (barSizeW / 2 + x);
	center.v = (barSizeH / 2 + y);

	error = sAIArt->NewArt(kGroupArt, kPlaceDefault, artGroup, &artGroup);
	if (error) return error;

	if (chkWhiteBox) // белый прямоугольник
	{
		error = sAIShapeConstr->NewRect((barSizeH + y), (barSizeW + x), y, x, false, &path);
		if (error) return error;

		error = rotate(path);

		error = sAIArt->ReorderArt(path, kPlaceInsideOnTop, artGroup);
		if (error) return error;

		// fill and stroke with black; 1 point line
		error = sAIPathStyle->GetCurrentPathStyle(&pathStyle, &pathStyleMap, &advStrokeParams);
		if (error) return error;

		pathStyle.fillPaint = true;
		pathStyle.fill.color.kind = kGrayColor;
		pathStyle.fill.color.c.g.gray = 0;

		pathStyle.strokePaint = false;
		pathStyle.stroke.color.kind = kGrayColor;
		pathStyle.stroke.color.c.g.gray = kAIRealOne;
		pathStyle.stroke.width = kAIRealOne;
		error = sAIPathStyle->SetPathStyle(path, &pathStyle);
		if (error) return error;
	}
	error = sAIArt->NewArt(kGroupArt, kPlaceInsideOnTop, artGroup, &digitArt);
	if (error) return error;

	digit[0] = XStr[0];
	CreatePointText(digit, x, y, 0 + charOffset, digitArt);
	if (error) return error;
	for (unsigned int c = 0; c < 6; c++)
		{
			digit[0] = XStr[c + 1];
			CreatePointText(digit, x, y, (c * 7 + barOffset + 4  + 3), digitArt);
		}
	for (unsigned int c = 6; c < 12; c++)
		{
			digit[0] = XStr[c + 1];
			CreatePointText(digit, x, y, (c * 7 + barOffset + 4 + 7), digitArt);
		}

	error = sAIArt->NewArt(kGroupArt, kPlaceInsideOnTop, artGroup, &barArt);
	if (error) return error;

	error = sBarUnit(x, y, 0 + barOffset, 1, barSizeH, true, barArt);
	if (error) return error;
	error = sBarUnit(x, y, 2 + barOffset, 1, barSizeH, true, barArt);
	if (error) return error;

	firstDigitItem = FirstDigit[XStr[0] - '0'];
	
	for (unsigned int c = 0; c < 6; c++)
	{

		codeDigit = XStr[c + 1] - '0';
		if (firstDigitItem & 0b100000)
		{
			error = sBarUnit(x, y, LbarCode[codeDigit][0] + c * 7 + barOffset + 2, LbarCode[codeDigit][1], barSizeH, false, barArt);
			if (error) return error;
			error = sBarUnit(x, y, LbarCode[codeDigit][2] + c * 7 + barOffset + 2, LbarCode[codeDigit][3], barSizeH, false, barArt);
			if (error) return error;
		}
		else
		{
			error = sBarUnit(x, y, GbarCode[codeDigit][0] + c * 7 + barOffset + 2, GbarCode[codeDigit][1], barSizeH, false, barArt);
			if (error) return error;
			error = sBarUnit(x, y, GbarCode[codeDigit][2] + c * 7 + barOffset + 2, GbarCode[codeDigit][3], barSizeH, false, barArt);
			if (error) return error;
		}
		firstDigitItem <<= 1;
	}

	error = sBarUnit(x, y, 46 + barOffset, 1, barSizeH, true, barArt);
	if (error) return error;
	error = sBarUnit(x, y, 48 + barOffset, 1, barSizeH, true, barArt);
	if (error) return error;

	for (unsigned int c = 6; c < 12; c++)
	{
		codeDigit = XStr[c+1] - '0';
		error = sBarUnit(x, y, RbarCode[codeDigit][0] + c * 7 + barOffset + 5 + 2, RbarCode[codeDigit][1], barSizeH, false, barArt);
		if (error) return error;
		error = sBarUnit(x, y, RbarCode[codeDigit][2] + c * 7 + barOffset + 5 + 2, RbarCode[codeDigit][3], barSizeH, false, barArt);
		if (error) return error;
	}

	error = sBarUnit(x, y, 92 + barOffset, 1, barSizeH, true, barArt);
	if (error) return error;
	error = sBarUnit(x, y, 94 + barOffset, 1, barSizeH, true, barArt);
	if (error) return error;

	error = sAIArt->SetArtName(artGroup, ai::UnicodeString("EAN 13"));
	return error;
}
ASErr BarcodeToolPlugin::rotate(AIArtHandle ah)
{
	ASErr error = 0;

	AIRealMatrix    mtx;
	sAIRealMath->AIRealMatrixSetTranslate(&mtx, -center.h, -center.v);
	sAIRealMath->AIRealMatrixConcatRotate(&mtx, rotateRadiant);
	sAIRealMath->AIRealMatrixConcatTranslate(&mtx, center.h, center.v);

	error = sAITransformArt->TransformArt(ah, &mtx, 1, kTransformObjects);
	return error;
}
ASErr BarcodeToolPlugin::sBarUnit(AIReal x, AIReal y, AIReal barXPosition, AIReal barSizeW, AIReal barSizeH, bool key, AIArtHandle artGroup)
{
	ASErr error = kNoErr;

	AIArtHandle path;
	AIPathStyle pathStyle;
	AIPathStyleMap pathStyleMap;
	AIDictionaryRef advStrokeParams = NULL;
	AIReal h;
	
	if (key) h = 1.43 * scale / K_p_to_mm + offset * scale;
	else h = 3.08 * scale / K_p_to_mm + offset * scale;
	barSizeH = barSizeH - offset * scale;

	error = sAIShapeConstr->NewRect(
		(barSizeH + y), 
		(barXPosition * (0.33 / K_p_to_mm) * scale + x),
		(h + y),
		((barXPosition * (0.33 / K_p_to_mm) + barSizeW * (0.33 / K_p_to_mm)) * scale + x),
		false, &path);

	if (error) return error;

	error = sAIPathStyle->GetCurrentPathStyle(&pathStyle, &pathStyleMap, &advStrokeParams);
	if (error) return error;

	pathStyle.fillPaint = true;
	pathStyle.fill.color.kind = kGrayColor;
	pathStyle.fill.color.c.g.gray = kAIRealOne;

	pathStyle.strokePaint = false;
	error = sAIPathStyle->SetPathStyle(path, &pathStyle);
	if (error) return error;

	error = rotate(path);

	error = sAIArt->ReorderArt(path, kPlaceInsideOnTop, artGroup);
	if (error) return error;

	return error;
}
ASErr BarcodeToolPlugin::CreatePointText(TCHAR *txt, AIReal x, AIReal y, int xPosition, AIArtHandle artGroup)
{
	ASErr error = kNoErr;
	try {
		
		// Add the new point text item to the layer.
		AITextOrientation orient = kHorizontalTextOrientation;
		AIRealPoint anchor = {(xPosition * (0.33 / K_p_to_mm) * scale + x), (y + offset * scale)};
		AIArtHandle textFrame = NULL;
		error = sAITextFrame->NewPointText(kPlaceInsideOnTop, artGroup, orient, anchor, &textFrame);
		aisdk::check_ai_error(error);

		// Set the contents of the text range.
		TextRangeRef range = NULL;
		error = sAITextFrame->GetATETextRange(textFrame, &range);
		aisdk::check_ai_error(error);
		ITextRange crange(range);
		crange.InsertAfter(ai::UnicodeString(txt).as_ASUnicode().c_str());

		ICharFeatures features;
		IParaFeatures ParaFeatures;
		ParaFeatures.SetJustification(kCenterJustify);
		ICharInspector inspector = crange.GetCharInspector();
	
		AIFontKey fontKey;
		FontRef BfontRef;
		AIReal fontSize = 12;
		AIRealRect rect;
		error = GetFont(&fontKey);
		if (!error)
		{
			error = sAIFont->FontFromFontKey(fontKey, &BfontRef);
			IFont font(BfontRef);
			features.SetFont(font);
			error = sAIFont->GetIdeoEmBox(fontKey, rect);
			fontSize = -9.5 / rect.top;
		}
		features.SetFontSize(fontSize * scale);
		crange.SetLocalCharFeatures(features);
		crange.SetLocalParaFeatures(ParaFeatures);

		error = rotate(textFrame);

		LRESULT chkState = SendMessage((HWND)HwndChekBoxOutline, BM_GETCHECK, 0, 0);
		if (chkState== BST_CHECKED)
		{
			AIArtHandle textFrameOutline = NULL;
			AIArtHandle path = NULL;
			error = sAITextFrame->CreateOutline(textFrame, &textFrameOutline);
			error = sAIArt->GetArtFirstChild (textFrameOutline,  &path);
			error = sAIArt->SetArtName(path, ai::UnicodeString(txt));
			error = sAIArt->ReorderArt(path, kPlaceInsideOnTop, artGroup);
			error = sAIArt->MarkDirty(path, true);
			error = sAIArt->DisposeArt(textFrameOutline);
			error = sAIArt->DisposeArt(textFrame);
		}
	}
	catch (ai::Error& ex) {
		error = ex;
	}
	catch (ATE::Exception& ex) {
		error = ex.error;
	}
	return error;
}
ASErr BarcodeToolPlugin::AddFont(HWND hwndCombo)
{
	ASErr error = kNoErr;
	ai::int32 fontCount = 0;
	ai::int16 maxName = 49;
	ai::UnicodeString fontName;

	SendMessageA(hwndCombo, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)"[default font]");
	SendMessageA(hwndCombo, (UINT)LB_SETITEMDATA, (WPARAM)0, (LPARAM)-1);

	error = sAIFont->CountTypefaces(&fontCount);
	if (error) return error;

	for (int i = 0; i < fontCount; i++)
	{
		AITypefaceKey  typefaceKey = NULL;
		error = sAIFont->IndexTypefaceList(i, &typefaceKey);
		if (error) return error;

		error = sAIFont->GetTypefaceUnicodeMenuName(typefaceKey, fontName);
		if (error) return error;

		int idx;
		idx = SendMessage(hwndCombo, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)ai::UnicodeString(fontName).as_ASUnicode().c_str());
		error = SendMessageA(hwndCombo, (UINT)LB_SETITEMDATA, (WPARAM)idx, (LPARAM)i);
	}
	return error;
}
ASErr BarcodeToolPlugin::AddFontStyle(HWND hwndCombo)
{
	ASErr error = kNoErr;
	ai::int32 fontStyleCount;
	ai::int16 styleResult;
	ai::UnicodeString styleName;

	AITypefaceKey  typefaceKey;
	AIFontKey fontKey = NULL;

	error = sAIFont->FindFont(FullFontName, kAIAnyFontTechnology, kNativeAIScript, 0, &fontKey);
	if (error) return error;
	error = sAIFont->TypefaceAndStyleFromFontKey(fontKey, &typefaceKey, &styleResult);
	if (error) return error;
	error = sAIFont->CountTypefaceStyles(typefaceKey, &fontStyleCount);
	if (error) return error;

	for (int i = 0; i < fontStyleCount; i++)
	{
		fontKey = NULL;

		error = sAIFont->IndexTypefaceStyleList(typefaceKey, i, &fontKey);
		if (error) return error;

		error = sAIFont->GetFontStyleNameUnicode(fontKey, styleName, true);
		if (error) return error;

		int indx;
		indx = SendMessage(hwndCombo, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)ai::UnicodeString(styleName).as_ASUnicode().c_str());
		error = SendMessageA(hwndCombo, (UINT)LB_SETITEMDATA, (WPARAM)indx, (LPARAM)i);
	}
	return error;
}
ASErr BarcodeToolPlugin::AddCodeType(HWND hwndCombo)
{
	ASErr error = kNoErr;
	SendMessageA(hwndCombo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"EAN 13");
	SendMessage(hwndCombo, CB_SETCURSEL, 0, 0);
	SendMessage(hwndCombo, EM_SETMODIFY, false, 0);
	
	return error;
}
ASErr BarcodeToolPlugin::GetFont(AIFontKey *fontKey)
{
	ASErr error = kNoErr;

	int fontCount;
	ai::int16 maxName = 49;
	char fontName[50];
	if (chkOCRB)
	{
		error = sAIFont->CountFonts(&fontCount);
		if (error) return error;
		for (int i = 0; i < fontCount; i++)
		{
			*fontKey = NULL;

			error = sAIFont->IndexFontList(i, fontKey);
			if (error) return error;

			error = sAIFont->GetPostScriptFontName(*fontKey, fontName, maxName);
			if (error)
			{
				ai::NumberFormat numFormat;
				ai::UnicodeString strResult;
				sAIStringFormatUtils->IntegerToString(numFormat, error, strResult);
				sAIUser->MessageAlert(ai::UnicodeString(strResult));
				return error;
			}

			if ((_strnicmp(fontName, "OCRB", 5) > 0))
			{
				return error;
			}
		}
		return 1;
	}
	else
	{
		error = sAIFont->FindFont(FullFontName, kAIAnyFontTechnology, kNativeAIScript, 0, fontKey);
		return error;
	}
}
ASErr BarcodeToolPlugin::GetFontStyle(int indx, char *fontStyle)
{
	ASErr error = kNoErr;
	if (indx == -1)
	{
		*fontStyle = '\0';
		return error;
	}

	ai::int16 maxName = 49;
	AIFontKey fontKey = NULL;
	AITypefaceKey  typefaceKey;
	error = sAIFont->IndexTypefaceList(indx, &typefaceKey);
	if (error) return error;
	error = sAIFont->IndexTypefaceStyleList(typefaceKey, 0, &fontKey);
	if (error) return error;
	error = sAIFont->GetFontStyleUIName(fontKey, fontStyle, maxName);
	return error;
}
ASErr BarcodeToolPlugin::SetFullFontName(int indxFace, int indxStyle)
{
	ASErr error = kNoErr;
	ai::int16 maxName = 49;
	ai::int16 styleResult;
	AIFontKey fontKey = NULL;
	AITypefaceKey  typefaceKey;
	if (indxFace < 0) 
	{
		error = sAIFont->FindFont(FullFontName, kAIAnyFontTechnology, kNativeAIScript, 0, &fontKey);
		if (error) return error;
		error = sAIFont->TypefaceAndStyleFromFontKey(fontKey, &typefaceKey, &styleResult);
		if (error) return error;
		error = sAIFont->IndexTypefaceStyleList(typefaceKey, indxStyle, &fontKey);
		if (error) return error;
		error = sAIFont->GetFullFontName(fontKey, FullFontName, maxName);
		return error;
	}
	error = sAIFont->IndexTypefaceList(indxFace, &typefaceKey);
	if (error) return error;
	error = sAIFont->IndexTypefaceStyleList(typefaceKey, indxStyle, &fontKey);
	if (error) return error;
	error = sAIFont->GetFullFontName(fontKey, FullFontName, maxName);
	return error;
}
ASErr BarcodeToolPlugin::FillFontCombo()
{
	ASErr error = kNoErr;
	ai::int16 maxName = 49;
	ai::int16 styleResult;
	char fontName[50];
	AIFontKey fontKey = NULL;
	AITypefaceKey  typefaceKey;

	if ((_strnicmp(FullFontName, "[default font]", 13) != 0))
	{
		error = sAIFont->FindFont(FullFontName, kAIAnyFontTechnology, kNativeAIScript, 0, &fontKey);
		if (error) return error;
		error = sAIFont->GetFontFamilyUIName(fontKey, fontName, maxName);
		if (error) return error;
		SendMessageA(HwndFontCombo, AIC_SETTEXT, 0, (LPARAM)fontName);
		error = sAIFont->GetFontStyleUIName(fontKey, fontName, maxName);
		if (error) return error;
		SendMessageA(HwndFontStyleCombo, AIC_SETTEXT, 0, (LPARAM)fontName);
		return error;
	}
	else
	{
		SendMessageA(HwndFontCombo, AIC_SETTEXT, 0, (LPARAM)FullFontName);
	}
	return error;
}