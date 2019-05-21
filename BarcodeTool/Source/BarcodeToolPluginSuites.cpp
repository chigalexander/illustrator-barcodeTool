//========================================================================================
//  
//  $File: //ai/mainline/devtech/sdk/public/samplecode/BarcodeTool/source/BarcodeToolPluginSuites.cpp $
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

#include "BarcodeToolPluginSuites.h"

#include "Suites.hpp"
#include "ATETextSuitesImportHelper.h"
#include "AIATEPaint.h"



extern "C"
{
	AIArtSuite					*sAIArt = NULL;
	AIMatchingArtSuite			*sAIMatchingArt;
	AIUnicodeStringSuite		*sAIUnicodeString;
	SPBlocksSuite				*sSPBlocks;
	AIPanelSuite				*sAIPanel;
	AIPanelFlyoutMenuSuite		*sAIPanelFlyoutMenu;
	AIMenuSuite					*sAIMenu;
	AIStringFormatUtilsSuite	*sAIStringFormatUtils;

	AIPreferenceSuite			*sAIPref;
	AIToolSuite					*sAITool = NULL;
	AIUIThemeSuite				*sAIUITheme;
	AITransformArtSuite			*sAITransformArt;
	AIMdMemorySuite				*sAIMdMemory = NULL;
	AIUndoSuite					*sAIUndo;
	AIPathSuite					*sAIPath;
	AIPathStyleSuite			*sAIPathStyle;
	AIRealMathSuite				*sAIRealMath;
	AIDocumentViewSuite			*sAIDocumentView;
	AIShapeConstructionSuite	*sAIShapeConstr;
	AITextFrameSuite			*sAITextFrame = NULL;
	AIFontSuite					*sAIFont = NULL;
	AIATECurrentTextFeaturesSuite*	sAIATECurrentTextFeatures = NULL;
	AIDocumentSuite			*sAIDocumentSuite = NULL;
	
	EXTERN_TEXT_SUITES
}

ImportSuite		gImportSuites[] = {
	kAIArtSuite, kAIArtSuiteVersion, &sAIArt,
	kAIMatchingArtSuite, kAIMatchingArtSuiteVersion, &sAIMatchingArt,
	kAIUnicodeStringSuite, kAIUnicodeStringSuiteVersion, &sAIUnicodeString,
	kSPBlocksSuite,	kSPBlocksSuiteVersion,	&sSPBlocks,
	kAIPanelSuite, kAIPanelSuiteVersion, &sAIPanel,
	kAIMenuSuite, kAIMenuSuiteVersion, &sAIMenu,
	kAIPanelFlyoutMenuSuite, kAIPanelFlyoutMenuSuiteVersion, &sAIPanelFlyoutMenu,
	kAIStringFormatUtilsSuite, kAIStringFormatUtilsSuiteVersion, &sAIStringFormatUtils,

	kAIPreferenceSuite, kAIPreferenceSuiteVersion, &sAIPref,
	kAIToolSuite, kAIToolVersion, &sAITool,
	kAIUIThemeSuite, kAIUIThemeSuiteVersion, &sAIUITheme,
	kAITransformArtSuite, kAITransformArtSuiteVersion, &sAITransformArt,
	kAIMdMemorySuite, kAIMdMemorySuiteVersion, &sAIMdMemory,
	kAIUndoSuite, kAIUndoSuiteVersion, &sAIUndo,
	kAIPathSuite, kAIPathSuiteVersion, &sAIPath,
	kAIPathStyleSuite, kAIPathStyleSuiteVersion, &sAIPathStyle,
	kAIRealMathSuite, kAIRealMathVersion, &sAIRealMath,
	kAIDocumentViewSuite, kAIDocumentViewSuiteVersion, &sAIDocumentView,
	kAIShapeConstructionSuite, kAIShapeConstructionSuiteVersion, &sAIShapeConstr,
	kAITextFrameSuite, kAITextFrameVersion, &sAITextFrame,
	kAIFontSuite, kAIFontSuiteVersion, &sAIFont,
	kAIATECurrentTextFeaturesSuite, kAIATECurrentTextFeaturesSuiteVersion, &sAIATECurrentTextFeatures,
	kAIDocumentSuite, kAIDocumentSuiteVersion, &sAIDocumentSuite,
	IMPORT_TEXT_SUITES
	nil, 0, nil
};
