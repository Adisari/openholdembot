//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: evaluating autoplayer-functions
//   and calculating OpenPPL backup-actions.
//
//******************************************************************************

#define FORMULA_DLL_EXPORTS

#include "CAutoplayerLogic.h"
#include <assert.h>
#include "CFormula.h"
#include "CFunction.h"
#include "CFunctionCollection.h"
#include "..\CasinoInterface_DLL\CCasinoInterface.h"
#include "..\Debug_DLL\debug.h"
#include "..\OpenHoldem_CallBack_DLL\OpenHoldem_CallBack.h"
#include "..\Preferences_DLL\Preferences.h"
#include "..\..\Shared\MagicNumbers\MagicNumbers.h"

CAutoplayerLogic::CAutoplayerLogic() {
}

CAutoplayerLogic::~CAutoplayerLogic() {
}

void CAutoplayerLogic::EvaluateOpenPPL() {
  CalcPrimaryFormulasOpenPPL();
  CalculateOpenPPLBackupActions();
}

/*#double CAutoplayerLogic::EvaluateAutoplayerFunction(const int function_code) {
  assert(function_code >= 0);
  assert(function_code < k_number_of_standard_functions);
  return Formula()->FunctionCollection()->Evaluate(k_standard_function_names[function_code]);
}*/

void CAutoplayerLogic::CalcPrimaryFormulasOpenPPL() {
}

void CAutoplayerLogic::TranslateOpenPPLDecisionToAutoplayerFunctions(double decision) {
  write_log(Preferences()->debug_formula(), "[CAutoplayerLogic] TranslateOpenPPLDecisionToAutoplayerLogic()\n");
  // Positive values mean:  betsizes (in big-blinds)
  // Small negative values: percentaged potsized bets
  // Large negative values: action constants
  if (decision > 0) {
    // OpenHoldem uses f$betsize in dollars
    double betsize = decision * EvaluateSymbol("bblind");
    Formula()->AutoplayerLogic()->SetValue(k_autoplayer_function_betsize,
      betsize);
  }
  else if (IsPercentagePotsizeBet(decision)) {
    double betsize = 42;///BetSizeForPercentagedPotsizeBet(decision);
    Formula()->AutoplayerLogic()->SetValue(k_autoplayer_function_betsize,
      betsize);
  }
  else if (decision < -1000) {
    // Large negative values: action constants
    for (int action = k_autoplayer_function_beep; action <= k_autoplayer_function_fold; ++action) {
      CheckIfDecisionMatchesElementaryAction(decision, action);
    }
  }
  else {
    // This can only be undefined == 0.0
    assert(decision == kUndefinedZero);
    write_log(Preferences()->debug_symbolengine_open_ppl(),
      "[CAutoplayerLogic] OpenPPL-decision undefined. Defaulting to check/fold.\n");
    Formula()->AutoplayerLogic()->SetValue(k_autoplayer_function_check, true);
    Formula()->AutoplayerLogic()->SetValue(k_autoplayer_function_fold, true);
  }
}

void CAutoplayerLogic::CalculateOpenPPLBackupActions() {
  write_log(Preferences()->debug_formula(),
    "[CAutoplayerLogic] CalculateOpenPPLBackupActions()\n");
  // Beep is a stand-alone action
  // No backup, can't be combined with pother actions
  // (contrary to f$beep in OH-script).
  if (Formula()->FunctionCollection()->EvaluateAutoplayerFunction(
    k_autoplayer_function_beep)) {
    // Turn check/fold off
    Formula()->AutoplayerLogic()->SetValue(
      k_autoplayer_function_check, false);
    Formula()->AutoplayerLogic()->SetValue(
      k_autoplayer_function_fold, false);
    return;
  }
  // Allin -> BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_allin, k_autoplayer_function_betpot_1_1);
  // BetPot -> 3/4 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_1_1, k_autoplayer_function_betpot_3_4);
  // 3/4 BetPot -> 2/3 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_3_4, k_autoplayer_function_betpot_2_3);
  // 2/3 BetPot -> 1/2 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_2_3, k_autoplayer_function_betpot_1_2);
  // 1/2 BetPot -> 1/3 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_1_2, k_autoplayer_function_betpot_1_3);
  // 1/3 BetPot -> 1/4 BetPot
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_1_3, k_autoplayer_function_betpot_1_4);
  // 1/4 BetPot -> Raise
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betpot_1_4, k_autoplayer_function_raise);
  // Also: f$betsize -> Raise
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_betsize, k_autoplayer_function_raise);
  // Raise -> Call
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_raise, k_autoplayer_function_call);
  // Call -> Check
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_call, k_autoplayer_function_check);
  // Call -> Allin, in case we only can fold / (call) allin
  if (IsFoldAllinSituation()) {
    CalculateSingleOpenPPLBackupAction(
      k_autoplayer_function_call, k_autoplayer_function_allin);
  }
  // Check -> Fold
  CalculateSingleOpenPPLBackupAction(
    k_autoplayer_function_check, k_autoplayer_function_fold);
}

void CAutoplayerLogic::CalculateSingleOpenPPLBackupAction(int potential_action, int potential_backup) {
  double action_value = Formula()->FunctionCollection()->EvaluateAutoplayerFunction(
    potential_action);
  write_log(Preferences()->debug_formula(),
    "[CAutoplayerLogic] %s -> %.3f\n",
    k_standard_function_names[potential_action],
    action_value);
  if (action_value) {
    write_log(Preferences()->debug_formula(),
      "[CAutoplayerLogic] Backup action added: %s -> %s\n",
      k_standard_function_names[potential_action],
      k_standard_function_names[potential_backup]);
    Formula()->AutoplayerLogic()->SetValue(
      potential_backup, true);
  }
}

bool CAutoplayerLogic::IsFoldAllinSituation() {
  // Determine this situation by button-states
  // and not by easily misscraped bets and balances.
  // Fold and allin-button must be visible.
  // Raise. call and check must not.
  double myturnbits = EvaluateSymbol("myturnbits");
  int fold_allin = kMyTurnBitsFold | kMyTurnBitsAllin;
  write_log(Preferences()->debug_formula(),
    "[CAutoplayerLogic] Buttons seen (myturnbits): %0.f\n", myturnbits);
  if (myturnbits == fold_allin) {
    write_log(Preferences()->debug_formula(),
      "[CAutoplayerLogic] Fold / allin situation\n");
    return true;
  }
  return false;
}

bool CAutoplayerLogic::IsPercentagePotsizeBet(double decision) {
  // Small negative values: percentaged potsized bets
  // According to the old library...
  return ((decision < 0) && (decision >= -1000));
}

void CAutoplayerLogic::CheckIfDecisionMatchesElementaryAction(int decision, int action) {
  if (action == k_autoplayer_function_betsize) {
    // Not an elementary action
    return;
  }
  CString action_name;
  // Translate aqutoplayer-constant to OPPL-action-name
  switch (action) {
  case k_autoplayer_function_beep:
    action_name = "Beep";
    break;
  case k_autoplayer_function_allin:
    action_name = "RaiseMax";
    break;
  case k_autoplayer_function_betpot_2_1:
    action_name = "RaiseTwoPot";
    break;
  case k_autoplayer_function_betpot_1_1:
    action_name = "RaisePot";
    break;
  case k_autoplayer_function_betpot_3_4:
    action_name = "RaiseThreeFourthPot";
    break;
  case k_autoplayer_function_betpot_2_3:
    action_name = "RaiseTwoThirdPot";
    break;
  case k_autoplayer_function_betpot_1_2:
    action_name = "RaiseHalfPot";
    break;
  case k_autoplayer_function_betpot_1_3:
    action_name = "RaiseThirdPot";
    break;
  case k_autoplayer_function_betpot_1_4:
    action_name = "RaiseFourthPot";
    break;
    // k_autoplayer_function_betsize already handled above
    // Not an elementary action -> return
  case k_autoplayer_function_raise:
    action_name = "Raise";
    break;
  case k_autoplayer_function_call:
    action_name = "Call";
    break;
  case k_autoplayer_function_check:
    action_name = "Check";
    break;
  case k_autoplayer_function_fold:
    action_name = "Fold";
    break;
  default:
    assert(kThisMustNotHappen);
  }
  double open_ppl_action_code = Formula()->FunctionCollection()->Evaluate(action_name);
  write_log(Preferences()->debug_symbolengine_open_ppl(),
    "[CAutoplayerLogic] Checking if decision %.3f matches action %s (%.3f)\n",
    decision, action_name, open_ppl_action_code);
  if (decision == open_ppl_action_code) {
    write_log(Preferences()->debug_symbolengine_open_ppl(),
      "[CAutoplayerLogic] Decision matches action\n");
    Formula()->AutoplayerLogic()->SetValue(action, true);
  }
}

double CAutoplayerLogic::GetValue(const int function_code) {
  CString function_name = k_standard_function_names[function_code];
  assert(function_name != "");
  CFunction *p_function = (CFunction *)Formula()->FunctionCollection()->LookUp(function_name);
  if (p_function == NULL) {
    return kUndefinedZero;
  }
  return p_function->Evaluate(); /// log?
}

void CAutoplayerLogic::SetValue(const int function_code, double new_value) {
  CString function_name = k_standard_function_names[function_code];
  assert(function_name != "");
  CFunction *p_function = (CFunction *)Formula()->FunctionCollection()->LookUp(function_name);
  if (p_function == NULL) {
    return;
  }
  p_function->SetValue(new_value);
}

void CAutoplayerLogic::CalcPrimaryFormulas() {
  write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] CalcPrimaryFormulas()\n");
  ///assert(EngineContainer()->symbol_engine_autoplayer()->isfinalanswer());
  // Otherwiese: OH-script
  assert(!Formula()->FunctionCollection()->IsOpenPPLProfile());
  CalcPrimaryFormulasOHScript();
}

void CAutoplayerLogic::CalcPrimaryFormulasOHScript() {
  write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] CalcPrimaryFormulasOHScript()\n");
  for (int i = k_autoplayer_function_beep; i <= k_autoplayer_function_fold; i++) {
    double result = Formula()->FunctionCollection()->Evaluate(k_standard_function_names[i], kAlwaysLogAutoplayerFunctions);
    write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] Primary formulas; %s: %f\n",
      k_standard_function_names[i], result);
  }
}

void CAutoplayerLogic::CalcSecondaryFormulas(void) {
  // Not considering k_standard_function_shoot_replay_frame here,
  // as this function gets handled by CSymbolEngineReplayFrameController
  for (int i = k_hopper_function_sitin; i <= k_standard_function_allin_on_betsize_balance_ratio; ++i) {
    double result = Formula()->FunctionCollection()->Evaluate(k_standard_function_names[i], Preferences()->log_hopper_functions());
    write_log(Preferences()->debug_formula(), "[CAutoplayerFunctions] Secondary formulas; %s: %f\n",
      k_standard_function_names[i], result);
  }
}