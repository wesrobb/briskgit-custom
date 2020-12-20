/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


// D-Type header file (level 1)
#include "dtype_l1.h"
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

	DT_SWORD NextFrame()
	{
		Frame += Step; if (Frame > 450 || Frame < 0) { Step = -Step; Frame += Step; }
		return Frame;
	}

	void DrawFrame()
	{
		DrawMain(); SetPixels(MemorySurface.m, 1);
	}

	bool Redraw;
	bool Pause;

private:

	void Init();
	void Exit();

	void Reset()
	{
		Redraw = true;
		Pause = false;

		LeftMouseButton = false;
		MouseX = MouseY = 0;
		X = Y = 0;
		Zoom = GetDisplay()->GetScale();
		Page = 0;
	}

	bool DrawMain();

	void DrawPage();
	void DrawBitmap(const DT_BMP& bmp, DT_SLONG x, DT_SLONG y, DT_UBYTE alpha);
	void LoadBitmap(DT_BMP& bmp, DT_SLONG w, DT_SLONG h, const DT_STREAM_DESC& sd);

	inline DT_SLONG Scale(DT_SLONG d)
	{
		DT_FLOAT z = DF_ROUND(Zoom * 1000) * 0.001;
		return DF_ROUND(d * z);
	}

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

	DT_SWORD Frame;
	DT_SWORD Step;

	CDTEngineV8& Engine;
	DT_MDC MemorySurface;

	DT_BMP BitmapA;
	DT_BMP BitmapB;
};


void CDTApplet::Init()
{
	Reset();

	Frame = 0;
	Step = 6;

	MemorySurface.m = DV_NULL; MemorySurface.l = 0; MemorySurface.w = MemorySurface.h = 0;

	DT_STREAM_FILE(sa, "../../files/raw/base_180x165_8bpp.raw");  LoadBitmap(BitmapA, 180, 165, sa);
	DT_STREAM_FILE(sb, "../../files/raw/menu3_180x165_8bpp.raw"); LoadBitmap(BitmapB, 180, 165, sb);

	Engine.TypesetterSetHinting(DV_HINTING_XOFF_YOFF, 0); /* Disable font hinting in both X and Y direction */
	Engine.TypesetterSetPositioning(DV_POSITIONING_INTX_INTY, 0); /* Whole-pixel positioning in both X and Y direction */

	Engine.TypesetterSetQuality(DV_QUALITY_MEDIUM, 0); /* Set the quality level to the lowest setting and improve the rendering speed */
	Engine.TypesetterSetCachePolicy(16000, 0); /* Improve the effectiveness of the bitmap cache (glyph images that exceed 16000 bytes in size will not be cached) */
}


void CDTApplet::Exit()
{
	dtMemFree(MemorySurface.m);
	dtMemFree(BitmapA.m);
	dtMemFree(BitmapB.m);
}


bool CDTApplet::DrawMain()
{
	DT_UBYTE* m;
	DT_UBYTE clear_byte = 0;
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

	Engine.OutputSetMDC(DV_SURFACE_FORMAT, 0, MemorySurface, 0, 0, w, h);

	DrawPage();

	DrawBitmap(BitmapA, w - BitmapA.w, h - BitmapA.h, 0);
	DrawBitmap(BitmapB, w - BitmapB.w, h - BitmapB.h, 255);

	return true;
}


void CDTApplet::DrawPage()
{
	DT_TM2X2 tm;
	DT_FLOAT c, s, kh, kv;
	DT_UWORD font_em_h, font_em_v;
	DT_SWORD i = DF_MIN(Frame, 360);

	/* Precomputed values for the current frame */
	DT_UBYTE t = static_cast<DT_UBYTE>(255 - i * 70 / 100);
	DT_SWORD size1 = i * 75 / 100;
	DT_SWORD size2 = i * 50 / 100;
	DT_SWORD size3 = 2430 - 6 * i;
	DT_SWORD dilation1 = 10 + 2 * i;
	DT_SWORD font_id = Engine.FontFind("F0010_DENMRK_SS0", 0);

	DT_STYLE_EFFECTS style1 = {1, {0}, {255, 220, 0, t}, DV_NULL};
	DT_STYLE_EFFECTS style2 = {1, {0}, {200, 200, 200, 150}, DV_NULL};
	DT_STYLE_EFFECTS style3 = {1, {0}, {120, 50, 230, t}, DV_NULL};

	DT_TYPE_EFFECTS_L type1 = {font_id, 0, 0, {{Scale(size1), Scale(size1), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type2 = {font_id, 0, 0, {{Scale(size1), Scale(size1), 0, 0, i}}, {Scale(dilation1), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type3 = {font_id, 0, 0, {{Scale(size1), Scale(size1), 0, 0, static_cast<DT_SWORD>(360 - i)}}, {Scale(dilation1), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type4 = {font_id, 0, 0, {{Scale(size1), Scale(size2), static_cast<DT_SWORD>(-i / 10), 0, i}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type5 = {font_id, 0, 0, {{Scale(size3), Scale(size3), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	Engine.FontGetMetrics(font_id, &font_em_h, &font_em_v);

	c = cos(DF_RAD(i));
	s = sin(DF_RAD(i));
	kh = Zoom * size1; kh /= font_em_h;
	kv = Zoom * size1; kv /= font_em_v;

	// Set transform matrix
	tm[0][0] = kh;
	tm[0][1] = 0;
	tm[1][0] = 0;
	tm[1][1] = kv;

	/* Shadow - 'ANIMATION' */
	Engine.OutputSetStyleEffects(style2);
	Engine.TypesetterSetTypeEffects(type1);
	Engine.CharsDoOutput(Scale(250) - X, Scale(2 * i - 20) - Y, 0, DV_SPACING_ADD_ROUND, tm, "ANIMATION");

	/* Body - 'ANIMATION' */
	Engine.OutputSetStyleEffects(style1);
	Engine.TypesetterSetTypeEffects(type1);
	Engine.CharsDoOutput(Scale(250) - X, Scale(700) - Y, 0, DV_SPACING_ADD_ROUND, tm, "ANIMATION");

	// Set transform matrix for rotation
	tm[0][0] = c * kh;
	tm[0][1] = -s * kv;
	tm[1][0] = s * kh;
	tm[1][1] = c * kv;

	/* Outline - 'ANIMATION' */
	Engine.OutputSetStyleEffects(style3);
	Engine.TypesetterSetTypeEffects(type2);
	Engine.CharsDoOutput(Scale(250) - X, Scale(700) - Y, 0, DV_SPACING_ADD_ROUND, tm, "ANIMATION");

	/* Shadow - 'D-TYPE' */
	Engine.OutputSetStyleEffects(style2);
	Engine.TypesetterSetTypeEffects(type4);
	Engine.CharsDoOutput(Scale(600) - X, Scale(i + 40) - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "D-Type");

	/* Body - 'D-TYPE' */
	Engine.OutputSetStyleEffects(style3);
	Engine.TypesetterSetTypeEffects(type5);
	Engine.CharsDoOutput(Scale(-840 + 4 * i) - X, Scale(1480 - 3 * i) - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "D-Type");

	/* Outline - 'D-TYPE' */
	Engine.OutputSetStyleEffects(style1);
	Engine.TypesetterSetTypeEffects(type3);

	// Set transform matrix for rotation
	tm[0][0] = c * kh;
	tm[0][1] = s * kv;
	tm[1][0] = -s * kh;
	tm[1][1] = c * kv;

	Engine.CharsDoOutput(Scale(600) - X, Scale(400) - Y, 0, DV_SPACING_ADD_ROUND, tm, "D-Type");
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
		if (Page < 5) Page++; else return;
	}
	else if (key == 32) /* SPACE */
	{
		Redraw = Pause;
		Pause = !Pause;
		return;
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
