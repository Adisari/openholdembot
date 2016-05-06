//******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Clean interface for scraper, casino-interface amd autoplayer
//
//******************************************************************************

#include "stdafx.h"
#include "CAutoplayerButton.h"

#include "CPreferences.h"
#include "MagicNumbers.h"

CAutoplayerButton::CAutoplayerButton() {
  _clickable = false;
  _region.bottom = kUndefined;
  _region.left   = kUndefined;
  _region.right  = kUndefined;
  _region.top    = kUndefined;
}

CAutoplayerButton::~CAutoplayerButton() {
}

bool CAutoplayerButton::Click() {
  if (_clickable) {
    /*!!!!!
    // Try to send a hotkey first, if specified in tablemap
    if (casino_hotkeys.PressHotkey(k_standard_function_names[autoplayer_function_code])) {
      write_log(preferences.debug_autoplayer(), "[CasinoInterface] Pressed hotkey for button button %s\n", k_standard_function_names[autoplayer_function_code]);
      return true;
    }
    */
    // Otherwise: click the button the normal way
    ClickRect(_region);
    write_log(preferences.debug_autoplayer(), "[CasinoInterface] Clicked button %s\n", k_standard_function_names[autoplayer_function_code]);
    return true;
  } else {
      write_log(preferences.debug_autoplayer(), "[CasinoInterface] Could not click button %s. Either undefined or not visible.\n", k_standard_function_names[autoplayer_function_code]);
      return false;
    }
  }
  return true;
}

void CAutoplayerButton::SetVisible(bool visible) {
}

void CAutoplayerButton::SetState(CString state) {
  CString button_state_lower_case = state.MakeLower();
  if (button_state_lower_case.Left(4) == "true"
      || button_state_lower_case.Left(2) == "on"
      || button_state_lower_case.Left(3) == "yes"
      || button_state_lower_case.Left(7) == "checked"
      || button_state_lower_case.Left(3) == "lit") {
    SetVisible(true);
  } else {
    SetVisible(false);
  }
}

bool CAutoplayerButton::IsAllin() {
  s.Remove(' ');
  s.Remove('-');
  CString s_lower_case = s.MakeLower();
  s = s_lower_case.Left(5);
  return (s == "allin"
    || s == "a11in"
    || s == "allln"
    || s == "a111n"
    || s == "aiiin"
    || s == "buyin"
    || s.Left(3) == "max");
}

bool CAutoplayerButton::IsRaise() {
  CString s_lower_case = s.MakeLower();
  s = s_lower_case.Left(5);
  return (s == "raise"
    || s == "ra1se"
    || s == "ralse"
    || s.Left(3) == "bet");
}

bool CAutoplayerButton::IsCall() {
  CString s_lower_case = s.MakeLower();
  s = s_lower_case.Left(4);
  return (s == "call" || s == "caii" || s == "ca11");
}

bool CAutoplayerButton::IsCheck() {
  CString s_lower_case = s.MakeLower();
  s = s_lower_case.Left(5);

  return (s == "check" || s == "cheok");
}

bool CAutoplayerButton::IsFold() {
  CString s_lower_case = s.MakeLower();
  s = s_lower_case.Left(4);
  return (s == "fold" || s == "fo1d" || s == "foid");
}

bool CAutoplayerButton::IsAutopost() {
  s.Remove(' ');
  s.Remove('-');

  CString s_lower_case = s.MakeLower();
  s = s_lower_case.Left(8);

  return (s == "autopost" || s == "aut0p0st");
}

bool CAutoplayerButton::IsSitin() {
  s.Remove(' ');
  s.Remove('-');
  CString s_lower_case = s.MakeLower();
  s = s_lower_case.Left(5);
  return (s == "sitin" || s == "s1t1n");
}

bool CAutoplayerButton::IsSitout() {
  s.Remove(' ');
  s.Remove('-');
  CString s_lower_case = s.MakeLower();
  s = s_lower_case.Left(6);
  return (s == "sitout" || s == "s1tout" || s == "sit0ut" || s == "s1t0ut");
}

bool CAutoplayerButton::IsLeave() {
  return (s.MakeLower().Left(5) == "leave");
}

bool CAutoplayerButton::IsRematch() {
  return (s.MakeLower().Left(5) == "rematch");
}

bool CAutoplayerButton::IsPrefold() {
  return (_label.MakeLower().Left(7) == "prefold");
}






/*
bool CScraper::GetButtonState(int button_index) {
  CString l_button_state = "";
  if (button_index <= 9) {
    if (p_symbol_engine_casino->ConnectedToManualMode() && button_index == 5) {
      // Don't MakeLower our mm_network symbol
      l_button_state = p_table_state->_SCI._button_state[button_index];
    }
    else {
      // Default
      l_button_state = p_table_state->_SCI._button_state[button_index].MakeLower();
    }
    return GetButtonState(l_button_state);
  }
  else if (button_index >= 860) {
    l_button_state = p_table_state->_SCI._i86X_button_state[button_index - 860];
    return GetButtonState(l_button_state);
  }
  return false;
} 


bool CCasinoInterface::ButtonAvailable(int autoplayer_code) {
  return p_scraper_access->available_buttons[autoplayer_code];
}

bool CCasinoInterface::ButtonClickable(int autoplayer_code) {
  return (ButtonAvailable(autoplayer_code)
    && p_scraper_access->visible_buttons[autoplayer_code]);
}*/

bool CScraperAccess::GetBetpotButtonVisible(int button_code)
{
  /*
  Checks if a betpot button is visible
  i.e. available for taking an action
  */

  CString betpot_button_state = p_table_state->_SCI._betpot_button_state[button_code];
  return p_scraper->GetButtonState(betpot_button_state);
}