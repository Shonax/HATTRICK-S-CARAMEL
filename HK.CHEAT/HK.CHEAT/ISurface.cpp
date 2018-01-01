
#include "Misc.h"

void ISurface::DrawSetColor(Color col)
{
	typedef void(__thiscall* OriginalFn)(void*, Color);
	return GetVFunc<OriginalFn>(this, 14)(this, col);
};

void ISurface::DrawSetTextFont(ULONG font)
{
	typedef void(__thiscall* OriginalFn)(void*, ULONG);
	return GetVFunc<OriginalFn>(this, 23)(this, font);
};

void ISurface::DrawSetTextColor(Color col)
{
	typedef void(__thiscall* OriginalFn)(void*, Color);
	return GetVFunc<OriginalFn>(this, 24)(this, col);
};

void ISurface::DrawSetTextPos(int x, int y)
{
	typedef void(__thiscall* OriginalFn)(void*, int, int);
	return GetVFunc<OriginalFn>(this, 26)(this, x, y);
};

void ISurface::DrawLine(int x0, int y0, int x1, int y1)
{
	typedef void(__thiscall* OriginalFn)(void*, int, int, int, int);
	return GetVFunc<OriginalFn>(this, 19)(this, x0, y0, x1, y1);
};

void ISurface::DrawPrintText(CONST WCHAR * text, int textLen)
{
	typedef void(__thiscall* OriginalFn)(void*, CONST WCHAR *, int, int);
	return GetVFunc<OriginalFn>(this, 28)(this, text, textLen, NULL);
};

void ISurface::DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall)
{
	typedef void(__thiscall* OriginalFn)(void*, int, const unsigned char*, int, int);
	return GetVFunc<OriginalFn>(this, 37)(this, id, rgba, wide, tall);
};

void ISurface::DrawSetTexture(int id)
{
	typedef void(__thiscall* OriginalFn)(void*, int);
	return GetVFunc<OriginalFn>(this, 38)(this, id);
};

ULONG ISurface::AddFont(VOID)
{
	typedef ULONG(__thiscall* OriginalFn)(void *);
	return GetVFunc<OriginalFn>(this, 71)(this);
};

bool ISurface::SetFontGlyphSet(ULONG font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin, int nRangeMax)
{
	typedef bool(__thiscall* OriginalFn)(void*, ULONG, const char*, int, int, int, int, int, int, int);
	return GetVFunc<OriginalFn>(this, 72)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax);
};

void ISurface::GetTextSize(ULONG font, CONST WCHAR * text, int &wide, int &tall)
{
	typedef void(__thiscall* OriginalFn)(void*, ULONG, CONST WCHAR *, int&, int&);
	return GetVFunc<OriginalFn>(this, 79)(this, font, text, wide, tall);
};
