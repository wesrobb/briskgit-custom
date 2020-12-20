/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


// D-Type header file (level 3)
#include "dtype_l3.h"
// D-Type's platform independent window support
#include "dtwindow.h"
// D-Type's extra system utility functions
#include "dtxsystem.h"

// D-Type CPP Engine
#include "dtengine.cpp"


#define DV_SURFACE_BPP 4
#define DV_SURFACE_FORMAT (DV_SURFACE_BPP * 8)


class CDTApplet : public CDTWindow
{
public:

	CDTApplet(CDTEngineV8& engine, CDTDisplay& display, DT_FLOAT zoom = 1.0) : CDTWindow(&display), Engine(engine)
	{
		Init();
		Zoom *= zoom;
	}

	CDTApplet(CDTEngineV8& engine, CDTDisplay& display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags = 0, DT_FLOAT zoom = 1.0) : CDTWindow(&display, title, w, h, flags), Engine(engine)
	{
		Init();
		Zoom *= zoom;
	}

	virtual ~CDTApplet()
	{
		Exit();
	}

	void DrawFrame()
	{
		DrawMain(); SetPixels(MemorySurface.m, 1);
	}

	bool Redraw;

private:

	bool Init();
	void Exit();

	void Reset()
	{
		Redraw = true;

		LeftMouseButton = false;
		MouseX = MouseY = 0;
		X = Y = 0;
		Zoom = GetDisplay()->GetScale();
		Page = 0;
	}

	bool InitDoc();

	bool DrawMain();
	void DrawBitmap(const DT_BMP& bmp, DT_SLONG x, DT_SLONG y, DT_UBYTE alpha);
	void LoadBitmap(DT_BMP& bmp, DT_SLONG w, DT_SLONG h, const DT_STREAM_DESC& sd);

private: // window callbacks

	void Event_KeyDown(DT_SLONG key);
	void Event_MouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y);
	void Event_MouseMove(DT_SLONG x, DT_SLONG y);
	void Event_MouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y);
	void Event_Resize(DT_SLONG w, DT_SLONG h);

private:

	bool LeftMouseButton;
	DT_SLONG MouseX, MouseY;
	DT_SLONG X, Y;
	DT_FLOAT Zoom;
	DT_SWORD Page;

	CDTDocV8 Doc;

	CDTEngineV8& Engine;

	DT_MDC MemorySurface;

	DT_BMP BitmapA;
	DT_BMP BitmapB;
};


bool CDTApplet::Init()
{
	Reset();

	MemorySurface.m = DV_NULL; MemorySurface.l = 0; MemorySurface.w = MemorySurface.h = 0;

	DT_STREAM_FILE(sa, "../../files/raw/base_180x165_8bpp.raw");  LoadBitmap(BitmapA, 180, 165, sa);
	DT_STREAM_FILE(sb, "../../files/raw/menu4_180x165_8bpp.raw"); LoadBitmap(BitmapB, 180, 165, sb);

	return InitDoc();
}


void CDTApplet::Exit()
{
	dtMemFree(MemorySurface.m);
	dtMemFree(BitmapA.m);
	dtMemFree(BitmapB.m);
}


bool CDTApplet::InitDoc()
{
	if (!Doc.Init(Engine)) return false;

	Doc.Erase();

	/* PART 1a: Defining objects and their properties */

	CDTObj o00(Doc, "Ellipse Outline Color");
	CDTObj o01(Doc, "Ellipse Body Color");
	CDTObj o02(Doc, "Ellipse Shadow Color");
	CDTObj o03(Doc, "Ellipse Outline");
	CDTObj o04(Doc, "Ellipse Shadow");
	CDTObj o05(Doc, "Ellipse");
	CDTObj o06(Doc, "Ellipse Outline Color");
	CDTObj o07(Doc, "Ellipse Body Color");
	CDTObj o08(Doc, "Ellipse Outline");
	CDTObj o09(Doc, "Ellipse");
	CDTObj o10(Doc, "Arc Color");
	CDTObj o11(Doc, "Arc Line Width");
	CDTObj o12(Doc, "Arc");
	CDTObj o13(Doc, "Ellipse Outline Color");
	CDTObj o14(Doc, "Ellipse Body Color");
	CDTObj o15(Doc, "Ellipse Outline");
	CDTObj o16(Doc, "Ellipse");
	CDTObj o17(Doc, "Ellipse Body Color");
	CDTObj o18(Doc, "Ellipse");
	CDTObj o19(Doc, "TextLine Typography");
	CDTObj o20(Doc, "TextLine");

	/* Properties for object 0 */
	o00 + CDTPropStr(pdRGBT, "50641400");

	/* Properties for object 1 */
	o01 + CDTPropStr(pdRGBT, "FFFF0000");

	/* Properties for object 2 */
	o02 + CDTPropStr(pdRGBT, "80808078");

	/* Properties for object 3 */
	o03 + CDTLink(pdStylePtr, o00) + CDTPropInt(pdLineThickness, 340);

	/* Properties for object 4 */
	o04 + CDTLink(pdStylePtr, o02) + CDTPropInt(pdShadowDx, 5) + CDTPropInt(pdShadowDy, 5);

	/* Properties for object 5 */
	o05 + CDTPropInt(pdDw, 480) + CDTPropInt(pdDh, 480) + CDTLink(pdBodyPtr, o01) + CDTLink(pdOutlinePtr, o03) + CDTLink(pdShadowPtr, o04);

	/* Properties for object 6 */
	o06 + CDTPropStr(pdRGBT, "50641400");

	/* Properties for object 7 */
	o07 + CDTPropStr(pdRGBT, "FFFFFF00");

	/* Properties for object 8 */
	o08 + CDTPropInt(pdLineThickness, 340) + CDTLink(pdStylePtr, o06);

	/* Properties for object 9 */
	o09 + CDTLink(pdOutlinePtr, o08) + CDTLink(pdBodyPtr, o07) + CDTPropInt(pdDw, 140) + CDTPropInt(pdDh, 140);

	/* Properties for object 10 */
	o10 + CDTPropStr(pdRGBT, "50641400");

	/* Properties for object 11 */
	o11 + CDTLink(pdStylePtr, o10) + CDTPropInt(pdLineThickness, 1700);

	/* Properties for object 12 */
	o12 + CDTLink(pdLinePtr, o11) + CDTPropInt(pdAngleTheta, 120) + CDTPropInt(pdAngleAlpha, 30) + CDTPropInt(pdRadiusHor, 160) + CDTPropInt(pdRadiusVer, 160);

	/* Properties for object 13 */
	o13 + CDTPropStr(pdRGBT, "50641400");

	/* Properties for object 14 */
	o14 + CDTPropStr(pdRGBT, "FFC01400");

	/* Properties for object 15 */
	o15 + CDTPropInt(pdLineThickness, 240) + CDTLink(pdStylePtr, o13);

	/* Properties for object 16 */
	o16 + CDTLink(pdOutlinePtr, o15) + CDTLink(pdBodyPtr, o14) + CDTPropInt(pdDh, 130) + CDTPropInt(pdDw, 160);

	/* Properties for object 17 */
	o17 + CDTPropStr(pdRGBT, "50641400");

	/* Properties for object 18 */
	o18 + CDTLink(pdBodyPtr, o17) + CDTPropInt(pdDw, 70) + CDTPropInt(pdDh, 70);

	/* Properties for object 19 */
	o19 + CDTPropInt(pdFontIndex, 3) + CDTPropInt(pdFontDw, 70) + CDTPropInt(pdFontDh, 100) + CDTPropInt(pdSkewHor, 0) + CDTPropInt(pdSkewVer, 0) + CDTPropInt(pdRotation, 0) + CDTLink(pdBodyPtr, o17);

	/* Properties for object 20 */
	o20 + CDTPropInt(pdAngleRotation, 0) + CDTPropStr(pdTextString_08, "Sample PowerDoc Output") + CDTLink(pdTypographyPtr, o19);

	/* PART 1b: Image placement */

	o05.Pin(0, 100, 170, pdDrawEllipse);
	o09.Pin(0, 180, 250, pdDrawEllipse);
	o12.Pin(0, 340, 410, pdDrawArc);
	o18.Pin(0, 205, 260, pdDrawEllipse);
	o09.Pin(0, 360, 250, pdDrawEllipse);
	o18.Pin(0, 385, 260, pdDrawEllipse);
	o16.Pin(0, 260, 340, pdDrawEllipse);
	o20.Pin(0, 300, 100, pdDrawTextLine);

	/* PART 2: Load additional documents from disk */

	DT_STREAM_FILE(sd1, "../../files/powerdoc/starburst-with_text.pdc");
	Doc.Append(sd1, 0, 700, 100);

	DT_STREAM_FILE(sd2, "../../files/powerdoc/tiger.pdc");
	Doc.Append(sd2, 0, 0, 700);

	DT_STREAM_FILE(sd3, "../../files/powerdoc/pie_chart.pdc");
	Doc.Append(sd3, 0, 700, 700);

	return true;
}


bool CDTApplet::DrawMain()
{
	DT_UBYTE* m;
	DT_UBYTE clear_byte = 255;
	DT_SLONG w = GetW(); if (w < 1 || w > 16384) return false;
	DT_SLONG h = GetH(); if (h < 1 || h > 16384) return false;
	DT_SLONG l = DV_SURFACE_BPP * w * h;

	if (l <= MemorySurface.l) m = MemorySurface.m; else m = static_cast<DT_UBYTE*>(dtMemRealloc(MemorySurface.m, l));
	if (m == DV_NULL) return false;

	dtMemSet(m, clear_byte, l);

	MemorySurface.m = m;
	MemorySurface.l = l;
	MemorySurface.w = w;
#ifdef DEF_WIN32 /* flip the surface vertically */
	MemorySurface.h = h;
#else
	MemorySurface.h = -h;
#endif

	Doc.SetScale(Zoom);
	Doc.Draw(Page, X, Y, DV_SURFACE_FORMAT, 0, MemorySurface);

	DrawBitmap(BitmapA, w - BitmapA.w, h - BitmapA.h, 0);
	DrawBitmap(BitmapB, w - BitmapB.w, h - BitmapB.h, 255);

	return true;
}


void CDTApplet::DrawBitmap(const DT_BMP& bmp, DT_SLONG x, DT_SLONG y, DT_UBYTE alpha)
{
	DT_STYLE_EFFECTS style1 = {1, {0}, {alpha, alpha, alpha, 0}, DV_NULL};
	Engine.OutputSetStyleEffects(style1);
	Engine.BitmapDoOutput(bmp, x, y);
}


void CDTApplet::LoadBitmap(DT_BMP& bmp, DT_SLONG w, DT_SLONG h, const DT_STREAM_DESC& sd)
{
	DT_SLONG l = w * h; 

	bmp.m = DV_NULL; bmp.l = 0; bmp.x = bmp.y = bmp.w = bmp.h = 0;
	bmp.m = static_cast<DT_UBYTE*>(dtMemAlloc(l)); if (bmp.m == DV_NULL) return;
	bmp.w = w;
	bmp.h = h;
	bmp.l = l;

	dtMemSet(bmp.m, 0, bmp.l);

	DT_DTSTREAM f = dtStreamOpen(&sd, DV_STREAM_OPEN_MUST); if (f == DV_NULL) return;
	dtStreamRead(f, bmp.m, bmp.l, 0);
	dtStreamClose(f, DV_STREAM_CLOSE_YES);
}


void CDTApplet::Event_KeyDown(DT_SLONG key)
{
	if (key == 109 || key == 108) /* - */
	{
		if (Zoom > 0.2) Zoom /= 1.2; else return;
	}
	else if (key == 107 || key == 61) /* + */
	{
		if (Zoom < 10.0) Zoom *= 1.2; else return;
	}
	else if (key == 37) /* LEFT */
	{
		X += 10;
	}
	else if (key == 39) /* RIGHT */
	{
		X -= 10;
	}
	else if (key == 38) /* UP */
	{
		Y += 10;
	}
	else if (key == 40) /* DOWN */
	{
		Y -= 10;
	}
	else if (key == 82) /* R */
	{
		Reset();
	}
	else if (key == 33) /* PageUp */
	{
		if (Page > 0) Page--; else return;
	}
	else if (key == 34) /* PageDown */
	{
		if (Page < 0) Page++; else return;
	}
	else return;

	Redraw = true;
}


void CDTApplet::Event_MouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y)
{
	if (button == 0) LeftMouseButton = true;

	MouseX = x;
	MouseY = y;
}


void CDTApplet::Event_MouseMove(DT_SLONG x, DT_SLONG y)
{
	if (!LeftMouseButton) return;

	X += MouseX - x; MouseX = x;
	Y += MouseY - y; MouseY = y;

	Redraw = true;
}


void CDTApplet::Event_MouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y)
{
	if (button == 0) LeftMouseButton = false;

	MouseX = x;
	MouseY = y;
}


void CDTApplet::Event_Resize(DT_SLONG /*w*/, DT_SLONG /*h*/)
{
	Redraw = true;
}
