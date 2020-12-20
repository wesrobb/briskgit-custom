/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


// D-Type header file (level 4)
#include "dtype_l4.h"
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
		Frame += Step; Frame %= 360; return Frame;
	}

	void DrawFrame()
	{
		DrawMain(); SetPixels(MemorySurface.m, 1);
	}

	bool Redraw;
	bool Pause;

private:

	bool Init();
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

	DT_SWORD Frame;
	DT_SWORD Step;

	CDTDocV8 Doc;

	CDTEngineV8& Engine;

	DT_MDC MemorySurface;

	DT_BMP BitmapA;
	DT_BMP BitmapB;
};


bool CDTApplet::Init()
{
	Reset();

	Frame = 0;
	Step = 6;

	MemorySurface.m = DV_NULL; MemorySurface.l = 0; MemorySurface.w = MemorySurface.h = 0;

	DT_STREAM_FILE(sa, "../../files/raw/base_180x165_8bpp.raw");  LoadBitmap(BitmapA, 180, 165, sa);
	DT_STREAM_FILE(sb, "../../files/raw/menu1_180x165_8bpp.raw"); LoadBitmap(BitmapB, 180, 165, sb);

	Engine.TypesetterSetQuality(DV_QUALITY_MEDIUM, 0); /* Set the quality level to the lowest setting and improve the rendering speed */
	Engine.TypesetterSetCachePolicy(16000, 0); /* Improve the effectiveness of the bitmap cache (glyph images that exceed 16000 bytes in size will not be cached) */

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

	const DT_SLONG text_w = 1000;
	const DT_SLONG text_h = 400;
	DT_STR_UTF8 text_utf8 =

		"D-Type Portable Unicode Text\r" \
		"The quick brown fox jumps over the lazy dog\r" \
		"Снимите кастрюлю с огня и дайте пасте остыть\r" \
		"Προσθέτετε το αλεύρι ολικής αλέσεως 1 φλυτζάνι την φορά\r" \
		"לקלוף את תפוחי האדמה והבצל ולרסקם על פומפיה דקה\r" \
		"在最近幾年，有個叫做 Unicode(萬國碼)\r" \
		"نخفق 2 زبدة مع السكر في الخلاط ثم نضيف البيض مع الفانيليا\r";

	/* Note: Set TX_ATTR_POSITIONING to 4 */

	DT_TX_DEFAULT_ATTRIBS attribs = {"00000000" /*rgbt*/ , "36" /*font_width*/, "36" /*font_height*/, "0" /*skew_hor*/, "0" /*skew_ver*/, "10" /*spacing_row*/, "0" /*spacing_letter*/, "1" /*align*/, "1" /*align_end*/, "7" /*hinting*/, DV_NULL /*font_index*/, "0" /*reserved*/};

	DT_TX_TEXTFLOW_AREA textflow_area = {-text_w / 2, -text_h / 2, text_w, text_h, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_FRAC, TX_TM_NULL, {DV_NULL}};

	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(text_utf8), static_cast<DT_SLONG>(strlen(text_utf8)));

	CDTDocV8::CDTTextEmbeddable doc_text(Doc);

	if (doc_text.Make(TX_UTF8, stream_text, TX_IMPORT_FULL_COMPACT, textflow_area, attribs, Engine.GetDefaultFontmapRef(), 1 /*1 = make text flow*/) < 0) return false;

	doc_text.SetAttribs(0, 29, attrBig);
	doc_text.SetAttribs(0, 29, attrBlueDark);

	//Doc.Save("debug.pdc");

	return true;
}


bool CDTApplet::DrawMain()
{
	DT_UBYTE* m;
	DT_UBYTE clear_byte = 240;
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

	DT_TM2X2 tm = {{1.0, 0.0}, {0.0, 1.0}};
	DT_FLOAT c = 0.75 * cos(DF_RAD(Frame));
	DT_FLOAT s = 0.75 * sin(DF_RAD(Frame));
	DT_SLONG cx = static_cast<DT_SLONG>(400 * GetDisplay()->GetScale());
	DT_SLONG cy = static_cast<DT_SLONG>(300 * GetDisplay()->GetScale());

	if (Page == 0) /* apply rotation */
	{
		tm[0][0] = c;
		tm[0][1] = -s;
		tm[1][0] = s;
		tm[1][1] = c;
	}
	else if (Page == 1) /* apply horizontal oscillation */
	{
		tm[0][0] = 1.5 * c;
		tm[0][1] = 0;
		tm[1][0] = 0;
		tm[1][1] = 1;
	}
	else if (Page == 2) /* apply vertical oscillation */
	{
		tm[0][0] = 1;
		tm[0][1] = 0;
		tm[1][0] = 0;
		tm[1][1] = 1.5 * c;
	}
	else if (Page == 3) /* apply horizontal skew */
	{
		tm[0][0] = 1;
		tm[0][1] = -s;
		tm[1][0] = 0;
		tm[1][1] = 1;
	}
	else if (Page == 4) /* apply vertical skew */
	{
		tm[0][0] = 1;
		tm[0][1] = 0;
		tm[1][0] = s;
		tm[1][1] = 1;
	}

	/* apply zoom */
	tm[0][0] *= Zoom;
	tm[0][1] *= Zoom;
	tm[1][0] *= Zoom;
	tm[1][1] *= Zoom;

	Doc.SetTransform(tm);
	Doc.Draw(0, X - cx, Y - cy, DV_SURFACE_FORMAT, 0, MemorySurface);

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
