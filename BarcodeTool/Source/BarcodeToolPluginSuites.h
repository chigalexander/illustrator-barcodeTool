//========================================================================================
//  
//  $File: //ai/mainline/devtech/sdk/public/samplecode/BarcodeTool/source/BarcodeToolPluginSuites.h $
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

#ifndef __BarcodeToolPluginSuites_H__
#define __BarcodeToolPluginSuites_H__

#include "IllustratorSDK.h"
#include "Suites.hpp"

// AI suite headers
#include "AIArt.h"
#include "AIMatchingArt.h"
#include "AIUnicodeString.h"
#include "AIUser.h"
#include "AIPanel.h"
#include "AIMenu.h"
#include "AIStringFormatUtils.h"
#include "SPBlocks.h"

#include "AIShapeConstruction.h"
#include "AIATECurrTextFeatures.h"
#include "AIFont.h"
#include "AITransformArt.h"
#include <AIUITheme.h>
#include <AIDocument.h>

// Suite externs
extern "C" AIArtSuite				*sAIArt;
extern "C" AIMatchingArtSuite		*sAIMatchingArt;
extern "C" AIUnicodeStringSuite		*sAIUnicodeString;
extern "C" AIPanelSuite				*sAIPanel;
extern "C" AIPanelFlyoutMenuSuite	*sAIPanelFlyoutMenu;
extern "C" AIMenuSuite				*sAIMenu;
extern "C" AIStringFormatUtilsSuite *sAIStringFormatUtils;
extern "C" SPBlocksSuite			*sSPBlocks;

extern	"C"	AIPreferenceSuite		*sAIPref;
extern	"C"	AIToolSuite				*sAITool;
extern  "C" AIUIThemeSuite			*sAIUITheme;
extern  "C"	AITransformArtSuite		*sAITransformArt;
extern	"C" AIMdMemorySuite			*sAIMdMemory;
extern	"C"	AIUndoSuite				*sAIUndo;
extern	"C"	AIPathSuite				*sAIPath;
extern	"C"	AIPathStyleSuite		*sAIPathStyle;
extern	"C"	AIRealMathSuite			*sAIRealMath;
extern  "C" AIDocumentViewSuite		*sAIDocumentView;
extern  "C" AIShapeConstructionSuite *sAIShapeConstr;
extern	"C" AITextFrameSuite		*sAITextFrame;
extern	"C" AIFontSuite				*sAIFont;
extern	"C" AIATECurrentTextFeaturesSuite*	sAIATECurrentTextFeatures;
extern	"C" AIDocumentSuite			*sAIDocumentSuite;
//extern  "C" AIAppContextSuite*				sAIAppContext;

#endif   // __BarcodeToolPluginSuites_H__
