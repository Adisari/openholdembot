//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Virtual base class for all symbol-engines
//
//******************************************************************************

#include "CVirtualSymbolEngine.h"
#include "CEngineContainer.h"
#include "CSymbolEngineIsOmaha.h"
#include "..\CasinoInterface_DLL\CCasinoInterface.h"
#include "..\Formula_DLL\CFormulaParser.h"
#include "..\WindowFunctions_DLL\window_functions.h"
#include "..\..\OpenHoldem\OpenHoldem.h"

CVirtualSymbolEngine::CVirtualSymbolEngine()
{}

CVirtualSymbolEngine::~CVirtualSymbolEngine()
{}

void CVirtualSymbolEngine::InitOnStartup()
{}

void CVirtualSymbolEngine::UpdateOnConnection()
{}

void CVirtualSymbolEngine::UpdateOnHandreset()
{}

void CVirtualSymbolEngine::UpdateOnNewRound()
{}

void CVirtualSymbolEngine::UpdateOnMyTurn()
{}

void CVirtualSymbolEngine::UpdateOnHeartbeat()
{}

void CVirtualSymbolEngine::UpdateAfterAutoplayerAction(int autoplayer_action_code)
{}

bool CVirtualSymbolEngine::EvaluateSymbol(const CString name, double *result, bool log /* = false */) {
	// We don't provide any symbols
	return false;
}

CString CVirtualSymbolEngine::SymbolsProvided() {
  // Default for symbol-engines that don't provide any symbols
  return "";
}

void CVirtualSymbolEngine::WarnIfSymbolRequiresMyTurn(CString name) {
  if (OpenHoldem()->FormulaParser()->IsParsing()) {
    // No error-message while parsing,
    // as we only verify existence, 
    // but don't care about the result.
    return;
  }
  if (!CasinoInterface()->IsMyTurn()) {
    CString error_message;
    error_message.Format("%s%s%s%s%s%s%s%s",
      "The symbol \"", name, "\"\n",
      "requires my turn and is currently undefined.\n",
      "\n",
      "(This error might also be caused by derived OpenPPL-symbols\n",
      "like Raises, Calls, CallsSinceLastRaise, ...\n",
      "which use basic OpenHoldem-symbols.)");
    MessageBox_Error_Warning(error_message, "Warning");
  }
}

void CVirtualSymbolEngine::WarnIfSymbolIsHoldemOnly(CString name) {
  if (EngineContainer()->symbol_engine_isomaha()->isomaha()) {
    CString error_message;
    error_message.Format("%s%s%s%s%s%s%s",
      "The symbol \"", name, "\"\n",
      "is currently only available for Hold'em games.\n",
      "\n",
      "Please get in contact with the development team\n",
      "if you volunteer to implement it for Omaha.");
    MessageBox_Error_Warning(error_message, "Warning");
  }
}