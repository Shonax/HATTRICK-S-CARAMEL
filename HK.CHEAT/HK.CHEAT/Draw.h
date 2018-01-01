#pragma once

#include "Misc.h"

namespace D
{
	extern VOID SetupFonts(VOID);
	extern VOID FDrawStr(INT, INT, Color, bool, CONST CHAR *);
	extern VOID DrawStr(INT, INT, Color, bool, CONST CHAR *, ...);
	extern VOID DrawLine(INT, INT, INT, INT, Color);
	extern bool ScreenTransform(Vector &, Vector &);
	extern bool WorldToScreen(Vector &, Vector &);
};
