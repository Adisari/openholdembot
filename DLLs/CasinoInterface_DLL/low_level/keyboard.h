#pragma once
//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: keyboard-handling for OpewnHoldems autoplayer.
//
//******************************************************************************

int SendString(const HWND hwnd, const RECT rect, const CString s, const bool use_comma);
int SendKey(const HWND hwnd, const RECT rect, UINT vkey);
void ProcessMessage(const char *message, const void *param);

const POINT RandomizeClickLocation(const RECT rect);
const void GetClickPoint(const int x, const int y, const int rx, const int ry, POINT *p);
const double RandomNormalScaled(const double scale, const double m, const double s);
const double RandomNormal(const double m, const double s);