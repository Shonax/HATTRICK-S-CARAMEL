#pragma once

#include "Misc.h"

#define FW_DONTCARE 0
#define FW_THIN 100
#define FW_EXTRALIGHT 200
#define FW_LIGHT 300
#define FW_NORMAL 400
#define FW_MEDIUM 500
#define FW_SEMIBOLD 600
#define FW_BOLD 700
#define FW_EXTRABOLD 800
#define FW_HEAVY 900

enum FontFlags_t : INT
{
	FONTFLAG_NONE = 0x000,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

class ISurface
{
public:

	VOID DrawSetColor(Color);
	VOID DrawSetTextFont(ULONG);
	VOID DrawSetTextColor(Color);
	VOID DrawSetTextPos(INT, INT);
	VOID DrawLine(INT, INT, INT, INT);
	VOID DrawPrintText(CONST WCHAR *, INT);
	VOID DrawSetTextureRGBA(INT, CONST UCHAR *, INT, INT);
	VOID DrawSetTexture(INT);
	ULONG AddFont(VOID);
	bool SetFontGlyphSet(ULONG, CONST CHAR *, INT, INT, INT, INT, INT, INT = NULL, INT = NULL);
	VOID GetTextSize(ULONG, CONST WCHAR *, INT &, INT &);
};
