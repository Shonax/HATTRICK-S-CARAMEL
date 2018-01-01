
#include "Misc.h"

VOID D::SetupFonts(VOID)
{
	g_nStyle = I::Surf->AddFont();
	I::Surf->SetFontGlyphSet(g_nStyle, XorString("MS Sans Serif"), 11, FW_MEDIUM, NULL, NULL, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);
};

VOID D::FDrawStr(INT X, INT Y, Color _Clr, bool bCentered, const char * pString)
{
	static WCHAR szWideBuf[512];
	static INT W, H;

	MultiByteToWideChar(CP_UTF8, 0, pString, 256, szWideBuf, 256);
	I::Surf->GetTextSize(g_nStyle, szWideBuf, W, H);
	I::Surf->DrawSetTextFont(g_nStyle);
	I::Surf->DrawSetTextColor(_Clr);
	I::Surf->DrawSetTextPos(bCentered ? (X - (W / 2)) : (X), Y - H / 2);
	I::Surf->DrawPrintText(szWideBuf, wcslen(szWideBuf));
};

VOID D::DrawStr(INT X, INT Y, Color _Clr, bool bCentered, const char * pRules, ...)
{
	static CHAR szBuf[512];
	static WCHAR szWideBuf[512];
	static INT W, H;
	static va_list pAp;

	va_start(pAp, pRules), vsprintf(szBuf, pRules, pAp), va_end(pAp);
	MultiByteToWideChar(CP_UTF8, 0, szBuf, 256, szWideBuf, 256);
	I::Surf->GetTextSize(g_nStyle, szWideBuf, W, H);
	I::Surf->DrawSetTextFont(g_nStyle);
	I::Surf->DrawSetTextColor(_Clr);
	I::Surf->DrawSetTextPos(bCentered ? (X - (W / 2)) : (X), Y - H / 2);
	I::Surf->DrawPrintText(szWideBuf, wcslen(szWideBuf));
};

VOID D::DrawLine(INT X0, INT Y0, INT X1, INT Y1, Color _Clr)
{
	I::Surf->DrawSetColor(_Clr), \
		I::Surf->DrawLine(X0, Y0, X1, Y1);
};

bool D::ScreenTransform(Vector & vecPoint, Vector & vecScreen)
{
	static FLOAT fW, fInvW;
	static bool bBehind;
	static VMatrix_T worldToScreen;

	worldToScreen = I::Engine->WorldToScreenMatrix();
	vecScreen.x = worldToScreen[0][0] * vecPoint.x + worldToScreen[0][1] * vecPoint.y + worldToScreen[0][2] * vecPoint.z + worldToScreen[0][3];
	vecScreen.y = worldToScreen[1][0] * vecPoint.x + worldToScreen[1][1] * vecPoint.y + worldToScreen[1][2] * vecPoint.z + worldToScreen[1][3];
	fW = worldToScreen[3][0] * vecPoint.x + worldToScreen[3][1] * vecPoint.y + worldToScreen[3][2] * vecPoint.z + worldToScreen[3][3];
	vecScreen.z = 0.f;

	if (fW < .001f) bBehind = true, vecScreen.x *= 100000.f, vecScreen.y *= 100000.f;
	else bBehind = false, fInvW = 1.f / fW, vecScreen.x *= fInvW, vecScreen.y *= fInvW;

	return bBehind;
};

bool D::WorldToScreen(Vector & vecOrigin, Vector & vecScreen)
{
	static INT SWidth, SHeight;
	static FLOAT fX, fY;

	if (!ScreenTransform(vecOrigin, vecScreen))
	{
		I::Engine->GetScreenSize(SWidth, SHeight);
		fX = (FLOAT)(SWidth / 2), fY = (FLOAT)(SHeight / 2);
		fX += .5f * vecScreen.x * FLOAT(SWidth) + .5f, fY -= .5f * vecScreen.y * FLOAT(SHeight) + .5f;
		vecScreen.x = fX, vecScreen.y = fY;
		return true;
	};

	return false;
};
