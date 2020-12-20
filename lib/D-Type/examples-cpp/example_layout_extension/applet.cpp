/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <iostream>

// D-Type header file (level 2)
#include "dtype_l2.h"
// D-Type's platform independent window support
#include "dtwindow.h"
// D-Type's extra system utility functions
#include "dtxsystem.h"

// D-Type CPP Engine
#include "dtengine.cpp"

#include "classes/fontmap.cpp"
#include "classes/textline.cpp"


#define DV_SURFACE_BPP 4
#define DV_SURFACE_FORMAT (DV_SURFACE_BPP * 8)

#define DV_TEXT_LINES 3


class CDTApplet : public CDTWindow
{
public:

	CDTApplet(CDTEngineV8& engine, CDTDisplay& display, DT_FLOAT zoom = 1.0) : CDTWindow(&display), FontMap(engine), Engine(engine)
	{
		Init();
		Zoom *= zoom;
	}

	CDTApplet(CDTEngineV8& engine, CDTDisplay& display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags = 0, DT_FLOAT zoom = 1.0) : CDTWindow(&display, title, w, h, flags), FontMap(engine), Engine(engine)
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

	CDTFontMap FontMap;
	CDTTextLine TextLine[DV_TEXT_LINES]; // Three sample text lines

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
	DT_STREAM_FILE(sb, "../../files/raw/menu2_180x165_8bpp.raw"); LoadBitmap(BitmapB, 180, 165, sb);

	// Set up FontMap
	FontMap.LoadRequiredFonts();

	// Three sample text files
	DT_SLONG i;
	const DT_CHAR* sample_file_name[DV_TEXT_LINES] =
	{
		"../../files/text/unicode_utf-8/single_line_a.txt",
		"../../files/text/unicode_utf-8/single_line_b.txt",
		"../../files/text/unicode_utf-8/single_line_c.txt"
	};

	DT_STYLE_EFFECTS style_text = {1, {0}, {0, 0, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style_label = {1, {0}, {200, 80, 100, 0}, DV_NULL};

	// Load and process sample text files
	for (i = 0; i < DV_TEXT_LINES; i++)
	{
		std::cout << "\n--- LOAD AND PROCESS sample text line #" << i << " (read from " << sample_file_name[i] << ") ---\n";
		DT_STREAM_FILE(text_line_sd, sample_file_name[i]);

		TextLine[i].SetFontMap(FontMap);
		TextLine[i].LoadUnicodeTextFromFile(text_line_sd, textAuto);
		TextLine[i].ProcessRuns(); /* Perform initial BiDi and split text into runs */
		TextLine[i].ReorderRuns(); /* BiDi rules L1, L2, L3, L4 (Reordering Resolved Levels) */
		TextLine[i].SetStyle_Text(style_text);
		TextLine[i].SetStyle_Label(style_label);
		/* once all of the above is done, our text line can be rendered by calling its DrawRuns_Hor or DrawRuns_Ver methods */
	}

	return true;
}


void CDTApplet::Exit()
{
	dtMemFree(MemorySurface.m);
	dtMemFree(BitmapA.m);
	dtMemFree(BitmapB.m);

	DT_SLONG i;
	for (i = 0; i < DV_TEXT_LINES; i++) TextLine[i].UnloadUnicodeText();

	FontMap.UnloadRequiredFonts();
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

	Engine.OutputSetMDC(DV_SURFACE_FORMAT, 0, MemorySurface, 0, 0, w, h);

	DT_SWORD i = Page / DV_TEXT_LINES;
	DT_SWORD debug_flags = Page % DV_TEXT_LINES;
	DT_SLONG font_size_w = 20;
	DT_SLONG font_size_h = 20;

	TextLine[i].DrawRuns_Hor(Zoom, X, Y,  50, 100, font_size_w, font_size_h, 1, debug_flags, "Mode 1: Draw using the GlyphsDoOutput function and DV_SPACING_DEVICE mode - great for device dependent text");
	TextLine[i].DrawRuns_Hor(Zoom, X, Y,  50, 200, font_size_w, font_size_h, 2, debug_flags, "Mode 2: Draw using the GlyphsDoOutput function and DV_SPACING_DEVICE_2 mode - great for device dependent text");
	TextLine[i].DrawRuns_Hor(Zoom, X, Y,  50, 300, font_size_w, font_size_h, 3, debug_flags, "Mode 3: Draw using the GlyphsDoOutput function and DV_SPACING_FRAC mode - great for device independent text");
	TextLine[i].DrawRuns_Hor(Zoom, X, Y,  50, 400, font_size_w, font_size_h, 0, debug_flags, "Mode 0: Draw using a simple glyph-by-glyph approach - the result looks basically the same as with Mode 3");

	TextLine[i].DrawRuns_Ver(Zoom, X, Y, 100, 450, font_size_w, font_size_h, 1, debug_flags, "Mode 1: Draw using the GlyphsDoOutput function and DV_SPACING_DEVICE mode - great for device dependent text");
	TextLine[i].DrawRuns_Ver(Zoom, X, Y, 200, 450, font_size_w, font_size_h, 2, debug_flags, "Mode 2: Draw using the GlyphsDoOutput function and DV_SPACING_DEVICE_2 mode - great for device dependent text");
	TextLine[i].DrawRuns_Ver(Zoom, X, Y, 300, 450, font_size_w, font_size_h, 3, debug_flags, "Mode 3: Draw using the GlyphsDoOutput function and DV_SPACING_FRAC mode - great for device independent text");
	TextLine[i].DrawRuns_Ver(Zoom, X, Y, 400, 450, font_size_w, font_size_h, 0, debug_flags, "Mode 0: Draw using a simple glyph-by-glyph approach - the result looks basically the same as with Mode 3");

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
		if (Page < 8) Page++; else return;
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
