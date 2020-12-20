/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

// D-Type header file
#include "dtype_rast.h"
// D-Type's platform independent window support
#include "dtwindow.h"
// D-Type's extra system utility functions
#include "dtxsystem.h"
// Vector data for this example
#include "data.h"


#define DV_SURFACE_BPP 4
#define DV_SURFACE_FORMAT (DV_SURFACE_BPP * 8)


class CDTApplet : public CDTWindow
{
public:

	CDTApplet(CDTDisplay& display, DT_FLOAT zoom = 1.0) : CDTWindow(&display)
	{
		Init();
		Zoom *= zoom;
	}

	CDTApplet(CDTDisplay& display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags = 0, DT_FLOAT zoom = 1.0) : CDTWindow(&display, title, w, h, flags)
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

	bool DrawMain();

	void DrawPage_00(CDTRasterizerV8& rast);
	void DrawPage_01(CDTRasterizerV8& rast);

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

	DT_MDC MemorySurface;
};


bool CDTApplet::Init()
{
	Reset();

	MemorySurface.m = DV_NULL; MemorySurface.l = 0; MemorySurface.w = MemorySurface.h = 0;

	return true;
}


void CDTApplet::Exit()
{
	dtMemFree(MemorySurface.m);
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

	DT_SLONG raster_id = 0;

	     if (Page == 1) raster_id = 10;
	else if (Page == 2) raster_id = 11;
	else if (Page == 3) raster_id = 12;
	else if (Page == 4) raster_id = 13;
	else if (Page == 5) raster_id = 15;
	else if (Page == 6) raster_id = 16;
	else if (Page == 7) raster_id = 17;

	CDTRasterizerV8 rast(MemorySurface, 0, raster_id);

	if (rast.InitFailed()) return false;

	// Draw output Page to D-Type memory surface
	switch (Page)
	{
	case  0: DrawPage_00(rast); break;
	default: DrawPage_01(rast); break;
	}

	rast.DoOutput(DV_SURFACE_FORMAT);

	return true;
}


void CDTApplet::DrawPage_00(CDTRasterizerV8& rast)
{
	DT_SLONG i = 0, instr = 0;
	DT_UBYTE r = 0, g = 0, b = 0, t = 0;
	/* Switch to fractional space */
	DT_SFRAC fr_zoom = DF_ROUND(Zoom * 256);
	DT_SFRAC fr_x = X * 256;
	DT_SFRAC fr_y = Y * 256;

	rast.SetFillRule(1);

	while ((instr = InstrArr[i++]) > 0)
	{
		DT_SFRAC fr_x1 = ((InstrArr[i++] * fr_zoom) >> 6) - fr_x;
		DT_SFRAC fr_y1 = ((InstrArr[i++] * fr_zoom) >> 6) - fr_y;

		if (instr == 16) /* New contour */
		{
			r = static_cast<DT_UBYTE>(InstrArr[i++]);
			g = static_cast<DT_UBYTE>(InstrArr[i++]);
			b = static_cast<DT_UBYTE>(InstrArr[i++]);
			t = static_cast<DT_UBYTE>(InstrArr[i++]);

			rast.MoveToFrac(fr_x1, fr_y1, r, g, b, t);
		}
		else if (instr == 20) /* LineTo command */
		{
			rast.LineToFrac(fr_x1, fr_y1);
		}
		else if (instr == 25) /* BSplineTo command */
		{
			DT_SFRAC fr_x2 = ((InstrArr[i++] * fr_zoom) >> 6) - fr_x;
			DT_SFRAC fr_y2 = ((InstrArr[i++] * fr_zoom) >> 6) - fr_y;

			rast.BSplineToFrac(fr_x1, fr_y1, fr_x2, fr_y2, 0);
		}
		else if (instr == 24) /* BezierTo command */
		{
			DT_SFRAC fr_x2 = ((InstrArr[i++] * fr_zoom) >> 6) - fr_x;
			DT_SFRAC fr_y2 = ((InstrArr[i++] * fr_zoom) >> 6) - fr_y;
			DT_SFRAC fr_x3 = ((InstrArr[i++] * fr_zoom) >> 6) - fr_x;
			DT_SFRAC fr_y3 = ((InstrArr[i++] * fr_zoom) >> 6) - fr_y;

			rast.BezierToFrac(fr_x1, fr_y1, fr_x2, fr_y2, fr_x3, fr_y3, 0);
		}
	}
}


void CDTApplet::DrawPage_01(CDTRasterizerV8& rast)
{
	DT_UBYTE color1[4] = {0, 255, 0, 0};
	DT_UBYTE color2[4] = {0, 0, 255, 0};
	DT_UBYTE color3[4] = {255, 255, 0, 0};

	rast.SetFillRule(1);

	rast.MoveTo(DF_ROUND(Zoom * 100) - X, DF_ROUND(Zoom * 200) - Y, 255, 0, 0, 0);
	//rast.LineTo3(DF_ROUND(Zoom * 200) - X, DF_ROUND(Zoom * 197) - Y, 0, 255, 0, 0);
	//rast.LineTo3(DF_ROUND(Zoom * 300) - X, DF_ROUND(Zoom * 400) - Y, 0, 0, 255, 0);
	rast.BSplineTo2(DF_ROUND(Zoom * 200) - X, DF_ROUND(Zoom * 197) - Y, DF_ROUND(Zoom * 300) - X, DF_ROUND(Zoom * 400) - Y, 0, color1, color2);
	rast.LineTo3(DF_ROUND(Zoom * 250) - X, DF_ROUND(Zoom * 450) - Y, 0, 0, 255, 0);
	//rast.LineTo(DF_ROUND(Zoom * 250) - X, DF_ROUND(Zoom * 450) - Y);
	rast.LineTo3(DF_ROUND(Zoom * 100) - X, DF_ROUND(Zoom * 220) - Y, 128, 128, 128, 0);
	//rast.LineTo3(DF_ROUND(Zoom * 100) - X, DF_ROUND(Zoom * 200) - Y, 255, 0, 0, 0);

	// ------------

	rast.MoveTo(DF_ROUND(Zoom * 400) - X, DF_ROUND(Zoom * 250) - Y, 255, 0, 0, 0);
	rast.LineTo3(DF_ROUND(Zoom * 500) - X, DF_ROUND(Zoom * 150) - Y, 0, 255, 0, 0);
	rast.LineTo3(DF_ROUND(Zoom * 600) - X, DF_ROUND(Zoom * 350) - Y, 0, 0, 255, 0);
	rast.LineTo3(DF_ROUND(Zoom * 750) - X, DF_ROUND(Zoom * 250) - Y, 255, 255, 0, 0);
	rast.LineTo3(DF_ROUND(Zoom * 550) - X, DF_ROUND(Zoom * 500) - Y, 0, 0, 255, 0);
	rast.LineTo3(DF_ROUND(Zoom * 400) - X, DF_ROUND(Zoom * 270) - Y, 255, 0, 0, 0);
	//rast.LineTo3(DF_ROUND(Zoom * 400) - X, DF_ROUND(Zoom * 250) - Y, 255, 0, 0, 0);

	// ------------

	rast.MoveTo(DF_ROUND(Zoom * 400) - X, DF_ROUND(Zoom * 250) - Y, 255, 0, 0, 0);
	rast.LineTo3(DF_ROUND(Zoom * 500) - X, DF_ROUND(Zoom * 150) - Y, 0, 255, 0, 0);
	rast.LineTo3(DF_ROUND(Zoom * 300) - X, DF_ROUND(Zoom * 50) - Y, 255, 255, 0, 0);

	// ------------

	rast.MoveTo(DF_ROUND(Zoom * 800) - X, DF_ROUND(Zoom * 250) - Y, 255, 0, 0, 0);
	rast.BezierTo2(DF_ROUND(Zoom * 900) - X, DF_ROUND(Zoom * 150) - Y, DF_ROUND(Zoom * 1000) - X, DF_ROUND(Zoom * 350) - Y, DF_ROUND(Zoom * 1150) - X, DF_ROUND(Zoom * 250) - Y, 0, color1, color2, color3);
	rast.LineTo3(DF_ROUND(Zoom * 950) - X, DF_ROUND(Zoom * 500) - Y, 0, 0, 255, 0);
	rast.LineTo3(DF_ROUND(Zoom * 800) - X, DF_ROUND(Zoom * 270) - Y, 255, 0, 0, 0);
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
		if (Page < 7) Page++; else return;
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
