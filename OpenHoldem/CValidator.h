//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

#ifndef INC_VALIDATOR_H
#define INC_VALIDATOR_H



// !!! ATTENTION
// !!! CString not properly declared/included.
// !!! atlstr / afxstr don't work.
// !!! Any other includes cause lots of erros
// !!! (escpecially CSpaceOptimiedObject.h).

class CValidator
{
public:
	// public functions
	CValidator();
	~CValidator();
	void ValidateGameState();
	void SetEnabledManually(bool Enabled);
private:
	// private functions
	void ValidateSingleRule();
	void ValidateVersusDBOnlyIfInstalled();
  void ValidateICMOnlyIfTournament();
	CString Symbols_And_Values(const CString symbols_possibly_affected);
	double gws(const char *the_Symbol);
private:
	// private data members, not (directly) accessible
	char	*_testcase_id;
	bool	_heuristic;
	char    *_reasoning;
	bool	_precondition;
	bool	_postcondition;
	char	*_symbols_possibly_affected;
private:
	bool _enabled_manually;
	bool _no_errors_this_heartbeat;
};

extern CValidator *p_validator; 

#endif

																																																						