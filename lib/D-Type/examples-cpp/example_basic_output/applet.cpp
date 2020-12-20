/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <iostream>

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

#define DV_MM_INTERPOLATIONS_X 6
#define DV_MM_INTERPOLATIONS_Y 10
#define DV_MM_INTERPOLATIONS_X_AND_Y (1 + (DV_MM_INTERPOLATIONS_X + 1) * (DV_MM_INTERPOLATIONS_Y + 1))

#define DV_MAXPOINTS 1500


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

	bool InitMMInstances();

	bool DrawMain();

	void DrawPage_00();
	void DrawPage_01();
	void DrawPage_02();
	void DrawPage_03();
	void DrawPage_04();
	void DrawPage_05();
	void DrawPage_06();
	void DrawPage_07();
	void DrawPage_08();
	void DrawPage_09();
	void DrawPage_10();
	void DrawPage_11();
	void DrawPage_12();
	void DrawPage_13();
	void DrawPage_14();
	void DrawPage_15();
	void DrawPage_16();
	void DrawPage_17();

	void DrawBitmap(const DT_BMP& bmp, DT_SLONG x, DT_SLONG y, DT_UBYTE alpha);
	void LoadBitmap(DT_BMP& bmp, DT_SLONG w, DT_SLONG h, const DT_STREAM_DESC& sd);

	inline DT_SLONG ScaleA(DT_SLONG d)
	{
		DT_FLOAT z = DF_ROUND(Zoom * 1000) * 0.001;
		return DF_ROUND(d * z);
	}

	inline DT_SLONG ScaleB(DT_SLONG d)
	{
		return DF_ROUND(d * Zoom);
	}

	inline DT_SWORD ScaleC(DT_SLONG d)
	{
		return static_cast<DT_SWORD>(DF_ROUND(d * Zoom));
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

	DT_ID_SWORD FontIndex_MM[DV_MM_INTERPOLATIONS_X_AND_Y];

	CDTEngineV8& Engine;
	DT_MDC MemorySurface;

	DT_BMP BitmapA;
	DT_BMP BitmapB;
};


bool CDTApplet::Init()
{
	DT_SLONG i;

	Reset();

	for (i = 0; i < DV_MM_INTERPOLATIONS_X_AND_Y; i++) FontIndex_MM[i] = 0;

	MemorySurface.m = DV_NULL; MemorySurface.l = 0; MemorySurface.w = MemorySurface.h = 0;

	DT_STREAM_FILE(sa, "../../files/raw/base_180x165_8bpp.raw");  LoadBitmap(BitmapA, 180, 165, sa);
	DT_STREAM_FILE(sb, "../../files/raw/menu2_180x165_8bpp.raw"); LoadBitmap(BitmapB, 180, 165, sb);

	InitMMInstances();

	return true;
}


void CDTApplet::Exit()
{
	dtMemFree(MemorySurface.m);
	dtMemFree(BitmapA.m);
	dtMemFree(BitmapB.m);
}


bool CDTApplet::InitMMInstances()
{
	// Load a Multiple-Master font
	const DT_CHAR* mm_font = "../../fonts/format/pfb/HSansMM.pfb";

	DT_STREAM_FILE(sd, mm_font);
	FontIndex_MM[0] = Engine.FontAddViaStream(DV_FONT_TYPE1_ADOBE, DV_NULL, 0, -1, 0, 1, sd);

	if (FontIndex_MM[0] < 0)
	{
		/* Error opening font */
		std::cout << "Error opening Multiple-Master font " << mm_font << "\n";
		return false;
	}

	DT_SWORD font_axis_count = Engine.FontGetMMAxisInfo(FontIndex_MM[0]);

	if (font_axis_count < 0)
	{
		std::cout << "Something is wrong with this font!\n";
		return false;
	}
	else if (font_axis_count > 16)
	{
		std::cout << "This is a complex Multiple-Master font with more than 16 axes! This font cannot be used for this demo.\n";
		return false;
	}
	else if (font_axis_count == 0)
	{
		std::cout << "This is not a Multiple-Master, but a regular font! This font cannot be used for this demo.\n";
		return false;
	}
	else if (font_axis_count < 2)
	{
		std::cout << "Although this is a Multiple-Master font, it cannot be used for this demo because it has less than 2 axes!\n";
		return false;
	}

	DT_SLONG mm_contribs[2];
	DT_SLONG i = 1, i_x, i_y;
	bool result = true;

	// Create interpolated font instances

	for (i_y = 0; i_y <= DV_MM_INTERPOLATIONS_Y; i_y++)
	for (i_x = 0; i_x <= DV_MM_INTERPOLATIONS_X; i_x++)
	{
		mm_contribs[0] = 1024 * i_x / DV_MM_INTERPOLATIONS_X;
		mm_contribs[1] = 1024 * i_y / DV_MM_INTERPOLATIONS_Y;

		// Now we can make an MM instance
		FontIndex_MM[i] = Engine.FontAddAsMMInstance(FontIndex_MM[0], DV_MMCONTRIB_TYPE_AXIS_NORM1024, 2, mm_contribs);
		if (FontIndex_MM[i] < 0) result = false; /* Error creating font instance */

		i++;
	}

	return result;
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

	switch (Page)
	{
	case  0: DrawPage_00(); break;
	case  1: DrawPage_01(); break;
	case  2: DrawPage_02(); break;
	case  3: DrawPage_03(); break;
	case  4: DrawPage_04(); break;
	case  5: DrawPage_05(); break;
	case  6: DrawPage_06(); break;
	case  7: DrawPage_07(); break;
	case  8: DrawPage_08(); break;
	case  9: DrawPage_09(); break;
	case 10: DrawPage_10(); break;
	case 11: DrawPage_11(); break;
	case 12: DrawPage_12(); break;
	case 13: DrawPage_13(); break;
	case 14: DrawPage_14(); break;
	case 15: DrawPage_15(); break;
	case 16: DrawPage_16(); break;
	case 17: DrawPage_17(); break;
	}

	DrawBitmap(BitmapA, w - BitmapA.w, h - BitmapA.h, 0);
	DrawBitmap(BitmapB, w - BitmapB.w, h - BitmapB.h, 255);

	return true;
}


void CDTApplet::DrawPage_00()
{
	// This example shows how to draw individual characters using D-Type Engine

	DT_STYLE_EFFECTS style = {1, {0}, {255, 0, 0, 0}, DV_NULL};
	/*
		Same as
		style.effects_len = 1;
		style.effects_arr[0] = 0;
		style.rgbt[0] = 255;
		style.rgbt[1] = 0;
		style.rgbt[2] = 0;
		style.rgbt[3] = 0;
		style.palette = DV_NULL;
	*/

	DT_TYPE_EFFECTS_L type = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(200), ScaleA(200), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	/*
		Same as
		type.font_index = Engine.FontFind("F0010_HSANSS_SS0", 0);
		type.reserved = 0;
		type.descriptor = 0;
		type.transform.params.size_h = ScaleA(200);
		type.transform.params.size_v = ScaleA(200);
		type.transform.params.skew_h = 0;
		type.transform.params.skew_v = 0;
		type.transform.params.rotation = 0;
		type.linedecor.thickness = 0;
		type.linedecor.segment = 0;
		type.linedecor.shift = 0;
		type.linedecor.dash_size = 0;
		type.linedecor.flags = 0;
		type.linedecor.scale_id = DV_SCALE_100;
	*/

	// Draw character A
	Engine.OutputSetStyleEffects(style);
	Engine.TypesetterSetTypeEffects(type);
	Engine.CharDoOutput('A', 20 * Zoom - X, 200 * Zoom - Y);

	// Draw character B
	style.rgbt[0] = 0;
	style.rgbt[1] = 200;
	style.rgbt[2] = 80;
	type.font_index = Engine.FontFind("F0011_CMCONC_SS0", 0);
	type.linedecor.thickness = ScaleC(400);
	Engine.OutputSetStyleEffects(style);
	Engine.TypesetterSetTypeEffects(type);
	Engine.CharDoOutput('B', 130 * Zoom - X, 250 * Zoom - Y);

	// Draw character C
	style.rgbt[0] = 100;
	style.rgbt[1] = 80;
	style.rgbt[2] = 210;
	type.font_index = Engine.FontFind("F0011_HINDUN_NS0", 0);
	type.linedecor.thickness = 0;
	Engine.OutputSetStyleEffects(style);
	Engine.TypesetterSetTypeEffects(type);
	Engine.CharDoOutput('C', 270 * Zoom - X, 175 * Zoom - Y);

	// Draw character D
	style.rgbt[0] = 200;
	style.rgbt[1] = 220;
	style.rgbt[2] = 60;
	type.font_index = Engine.FontFind("F0010_HSANSS_SS0", 0);
	type.linedecor.thickness = ScaleC(250);
	type.linedecor.segment = 100 * ScaleC(20);
	Engine.OutputSetStyleEffects(style);
	Engine.TypesetterSetTypeEffects(type);
	Engine.CharDoOutput('D', 370 * Zoom - X, 235 * Zoom - Y);
}


void CDTApplet::DrawPage_01()
{
	// This example demonstrates how to create true mathematical circles and ellipses using D-Type Engine

	DT_STYLE_EFFECTS style1 = {1, {0}, {210, 160, 133, 0}, DV_NULL};
	DT_STYLE_EFFECTS style2 = {1, {0}, {220, 20, 20, 0}, DV_NULL};
	DT_STYLE_EFFECTS style3 = {1, {0}, {30, 180, 70, 0}, DV_NULL};

	// Circle 1
	Engine.OutputSetStyleEffects(style1);
	Engine.Shape_TrueEllipseFilled(ScaleB(200) - X, ScaleB(200) - Y, 0, 0, ScaleB(150), ScaleB(150));

	// Circle 2
	Engine.OutputSetStyleEffects(style2);
	Engine.Shape_TrueEllipse(ScaleB(600) - X, ScaleB(200) - Y, 0, 0, ScaleB(150), ScaleB(150), 1 * Zoom);

	// Circle 3
	Engine.OutputSetStyleEffects(style3);
	Engine.Shape_TrueEllipse(ScaleB(1000) - X, ScaleB(200) - Y, 0, 0, ScaleB(150), ScaleB(150), 5 * Zoom);

	// Ellipse 1
	Engine.Shape_TrueEllipseDashed(ScaleB(600) - X, ScaleB(650) - Y, 0, 0, ScaleB(550), ScaleB(250), 110 * Zoom, DV_NULL, 45 * Zoom);

	// Ellipse 2
	Engine.Shape_TrueEllipseFilled(ScaleB(1500) - X, ScaleB(450) - Y, 0, 0, ScaleB(250), ScaleB(400));
}


void CDTApplet::DrawPage_02()
{
	// This example shows how to draw lines of glyphs/characters using D-Type Engine

	DT_TM2X2 tm;
	DT_UWORD font_em_h, font_em_v;
	DT_BMP bmp;

	DT_STYLE_EFFECTS style = {1, {0}, {120, 120, 220, 0}, DV_NULL};
	DT_TYPE_EFFECTS_L type = {Engine.FontFind("F0011_HINDUN_NS0", 0), 0, 0, {{ScaleA(80), ScaleA(80), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	const DT_CHAR* line1 = "This is a horizontal line";
	const DT_CHAR* line2 = "This is a skewed line";

	Engine.OutputSetStyleEffects(style);

	Engine.FontGetMetrics(type.font_index, &font_em_h, &font_em_v);

	// LINE #1

	// Set transform matrix for line #1
	tm[0][0] = type.transform.params.size_h; tm[0][0] /= font_em_h;
	tm[0][1] = 0.0;
	tm[1][0] = 0.0;
	tm[1][1] = type.transform.params.size_v; tm[1][1] /= font_em_v;

	// Draw line #1
	Engine.TypesetterSetTypeEffects(type);
	Engine.CharsDoOutput(30 * Zoom - X, 100 * Zoom - Y, 0, DV_SPACING_KERN_ROUND_ADD, tm, line1);

	// Draw bounding box for line #1
	Engine.CharsGetBound(DV_CROP_HMIN_VMAX, 30 * Zoom - X, 100 * Zoom - Y, 0, DV_SPACING_KERN_ROUND_ADD, tm, line1, &bmp);
	Engine.Shape_Rectangle(0, 0, bmp.x, -bmp.y, bmp.w, bmp.h, 0, 1 * Zoom);

	// LINE #2
	// (similar to line #1 but skewed vertically)

	// Set transform matrix for line #2
	type.transform.params.skew_v = 10;
	tm[1][0] = tan(DF_RAD(type.transform.params.skew_v)) * tm[0][0];

	// Draw line #2
	Engine.TypesetterSetTypeEffects(type);
	Engine.CharsDoOutput(30 * Zoom - X, 250 * Zoom - Y, 0, DV_SPACING_KERN_ROUND_ADD, tm, line2);

	// Draw bounding box for line #2
	Engine.CharsGetBound(DV_CROP_HMIN_VMAX, 30 * Zoom - X, 250 * Zoom - Y, 0, DV_SPACING_KERN_ROUND_ADD, tm, line2, &bmp);
	Engine.Shape_Rectangle(0, 0, bmp.x, -bmp.y, bmp.w, bmp.h, 0, 1 * Zoom);
}


void CDTApplet::DrawPage_03()
{
	// This example shows how to draw rotated lines of glyphs/characters using D-Type Engine

	DT_TM2X2 tm;
	DT_UWORD font_em_h, font_em_v;
	DT_FLOAT s, c, kh, kv;
	DT_FLOAT xc1, yc1, xc2, yc2;

	DT_SWORD angle;
	const DT_CHAR* line = "     D-Type Font Engine !!!!!!!!!!!!!!";

	DT_STYLE_EFFECTS style = {1, {0}, {90, 40, 180, 0}, DV_NULL};
	DT_TYPE_EFFECTS_L type = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(35), ScaleA(35), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	// Set style
	Engine.OutputSetStyleEffects(style);

	// Set type
	Engine.TypesetterSetTypeEffects(type);

	// Info line for fractional positioning
	Engine.CharsDoOutput(10 * Zoom - X, 40 * Zoom - Y, 0, DV_SPACING_FRAC, DV_NULL, "Fractional (subpixel) positioning");
	// Info line for standard positioning
	Engine.CharsDoOutput(610 * Zoom - X, 40 * Zoom - Y, 0, DV_SPACING_ROUND_ADD, DV_NULL, "Standard (whole-pixel) positioning");

	Engine.FontGetMetrics(type.font_index, &font_em_h, &font_em_v);

	type.transform.params.size_h = ScaleA(20);
	type.transform.params.size_v = ScaleA(20);

	xc1 = ScaleB(300) - X;
	yc1 = ScaleB(350) - Y;

	xc2 = ScaleB(900) - X;
	yc2 = ScaleB(350) - Y;

	for (angle = 0; angle < 360; angle += 30)
	{
		// Rotate individual characters
		type.transform.params.rotation = angle;
		Engine.TypesetterSetTypeEffects(type);

		// Compute transform matrix for rotation
		c = cos(DF_RAD(angle));
		s = sin(DF_RAD(angle));
		kh = type.transform.params.size_h; kh /= font_em_h;
		kv = type.transform.params.size_v; kv /= font_em_v;

		tm[0][0] = c * kh;
		tm[0][1] = -s * kv;
		tm[1][0] = s * kh;
		tm[1][1] = c * kv;

		// Method 1: Draw line using fractional character positioning
		// THIS METHOD PRODUCES BETTER RESULTS WITH ROTATED GLYPHS/CHARACTERS
		Engine.CharsDoOutput(xc1, yc1, 0, DV_SPACING_FRAC, tm, line);

		// Method 2: Draw line using standard integer character positioning
		Engine.CharsDoOutput(xc2, yc2, 0, DV_SPACING_ADD_ROUND, tm, line);
	}
}


void CDTApplet::DrawPage_04()
{
	// This example shows how to draw some common shapes using D-Type Engine

	DT_STYLE_EFFECTS style1 = {1, {0}, {255, 0, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style2 = {1, {0}, {154, 180, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style3 = {1, {0}, {0, 180, 244, 0}, DV_NULL};
	DT_STYLE_EFFECTS style4 = {1, {0}, {100, 0, 180, 0}, DV_NULL};
	DT_STYLE_EFFECTS style5 = {1, {0}, {210, 160, 133, 0}, DV_NULL};
	DT_STYLE_EFFECTS style6 = {1, {0}, {220, 20, 20, 0}, DV_NULL};
	DT_STYLE_EFFECTS style7 = {1, {0}, {7, 100, 250, 0}, DV_NULL};
	DT_STYLE_EFFECTS style8 = {1, {0}, {120, 50, 230, 0}, DV_NULL};
	DT_STYLE_EFFECTS style9 = {1, {0}, {89, 167, 234, 0}, DV_NULL};
	DT_STYLE_EFFECTS style10 = {1, {0}, {52, 130, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style11 = {1, {0}, {52, 30, 80, 0}, DV_NULL};
	DT_STYLE_EFFECTS style12 = {1, {0}, {20, 0, 180}, DV_NULL};

	// Rectangle 1
	Engine.OutputSetStyleEffects(style1);
	Engine.Shape_RectangleFilled(ScaleB(20) - X, ScaleB(20) - Y, 0, 0, ScaleB(400), ScaleB(300), 0);
	// Rectangle 2
	Engine.OutputSetStyleEffects(style2);
	Engine.Shape_Rectangle(ScaleB(510) - X, ScaleB(20) - Y, 0, 0, ScaleB(400), ScaleB(300), 0, 4 * Zoom);
	// Rectangle 3
	Engine.Shape_RectangleDashed(ScaleB(1000) - X, ScaleB(20) - Y, 0, 0, ScaleB(400), ScaleB(300), 0, 60 * Zoom, DV_NULL, 30 * Zoom);

	// Rounded Rectangle 1
	Engine.OutputSetStyleEffects(style3);
	Engine.Shape_RectangleFilled(ScaleB(20) - X, ScaleB(400) - Y, 0, 0, ScaleB(400), ScaleB(300), 0.4);
	// Rounded Rectangle 2
	Engine.OutputSetStyleEffects(style4);
	Engine.Shape_Rectangle(ScaleB(510) - X, ScaleB(400) - Y, 0, 0, ScaleB(400), ScaleB(300), 0.5, 4 * Zoom);
	// Rounded Rectangle 3
	Engine.Shape_RectangleDashed(ScaleB(1000) - X, ScaleB(400) - Y, 0, 0, ScaleB(400), ScaleB(300), 0.3, 60 * Zoom, DV_NULL, 30 * Zoom);

	// Ellipse 1
	Engine.OutputSetStyleEffects(style5);
	Engine.Shape_EllipseFilled(ScaleB(20) - X, ScaleB(780) - Y, 0, 0, ScaleB(400), ScaleB(300));
	// Ellipse 2
	Engine.OutputSetStyleEffects(style6);
	Engine.Shape_Ellipse(ScaleB(510) - X, ScaleB(780) - Y, 0, 0, ScaleB(400), ScaleB(300), 20 * Zoom);
	// Ellipse 3
	Engine.Shape_Ellipse(ScaleB(1000) - X, ScaleB(780) - Y, 0, 0, ScaleB(400), ScaleB(300), 80 * Zoom);

	// Draw Lines

	Engine.OutputSetStyleEffects(style1);
	Engine.Shape_Line(ScaleB(1500) - X, ScaleB(30) - Y, 0, 0, ScaleB(360), ScaleB(80), 2 * Zoom, 0, 0);

	Engine.OutputSetStyleEffects(style7);
	Engine.Shape_Line(ScaleB(1600) - X, ScaleB(185) - Y, 0, 0, ScaleB(580), ScaleB(-40), 60 * Zoom, 0, 0);

	Engine.OutputSetStyleEffects(style8);
	Engine.Shape_Line(ScaleB(1500) - X, ScaleB(250) - Y, 0, 0, ScaleB(580), ScaleB(40), 14 * Zoom, 0, 0);

	Engine.OutputSetStyleEffects(style9);
	Engine.Shape_Line(ScaleB(1500) - X, ScaleB(380) - Y, 0, 0, ScaleB(80), ScaleB(890), 35 * Zoom, 1, 1);

	Engine.OutputSetStyleEffects(style10);
	Engine.Shape_Line(ScaleB(1650) - X, ScaleB(380) - Y, 0, 0, ScaleB(80), ScaleB(890), 45 * Zoom, 2, 2);

	Engine.OutputSetStyleEffects(style11);
	Engine.Shape_Line(ScaleB(1800) - X, ScaleB(380) - Y, 0, 0, ScaleB(80), ScaleB(890), 65 * Zoom, 3, 3);

	Engine.OutputSetStyleEffects(style12);
	Engine.Shape_Line(ScaleB(2000) - X, ScaleB(380) - Y, 0, 0, ScaleB(80), ScaleB(890), 85 * Zoom, 4, 4);

	Engine.OutputSetStyleEffects(style9);
	Engine.Shape_LineDashed(ScaleB(100) - X, ScaleB(1150) - Y, 0, 0, ScaleB(1200), ScaleB(60), 155 * Zoom, DV_NULL, 50 * Zoom, 2, 3);

	Engine.OutputSetStyleEffects(style10);
	Engine.Shape_LineDashed(ScaleB(100) - X, ScaleB(1250) - Y, 0, 0, ScaleB(1200), ScaleB(160), 70 * Zoom, DV_NULL, 17 * Zoom, 0, 0);
}


void CDTApplet::DrawPage_05()
{
	// This example shows how to draw outlined lines of glyphs/characters with custom thickness using D-Type Engine

	DT_STYLE_EFFECTS style1 = {1, {0}, {120, 50, 230, 0}, DV_NULL};
	DT_STYLE_EFFECTS style2 = {1, {0}, {7, 100, 250, 0}, DV_NULL};
	DT_STYLE_EFFECTS style3 = {1, {0}, {89, 167, 234, 0}, DV_NULL};
	DT_STYLE_EFFECTS style4 = {1, {0}, {20, 90, 200, 0}, DV_NULL};

	DT_TYPE_EFFECTS_L type1 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(100), ScaleA(100), 0, 0, 0}}, {ScaleC(100), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type2 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(100), ScaleA(100), 0, 20, 0}}, {ScaleC(500), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type3 = {Engine.FontFind("F0011_TEX10__SS0", 0), 0, 0, {{ScaleA(100), ScaleA(100), 0, 0, 0}}, {ScaleC(300), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type4 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(100), ScaleA(100), 0, 0, 0}}, {ScaleC(150), 100 * ScaleC(15), 0, 0, 0, DV_SCALE_100}};

	// Draw
	Engine.OutputSetStyleEffects(style1);
	Engine.TypesetterSetTypeEffects(type1);
	Engine.CharsDoOutput(10 * Zoom - X, 100 * Zoom - Y, 0, DV_SPACING_ROUND_ADD, DV_NULL, "HELLO WORLD!");

	Engine.OutputSetStyleEffects(style2);
	Engine.TypesetterSetTypeEffects(type2);
	Engine.CharsDoOutput(10 * Zoom - X, 210 * Zoom - Y, 0, DV_SPACING_ROUND_ADD, DV_NULL, "THIS IS JUST A TEST");

	Engine.OutputSetStyleEffects(style3);
	Engine.TypesetterSetTypeEffects(type3);
	Engine.CharsDoOutput(10 * Zoom - X, 340 * Zoom - Y, 0, DV_SPACING_ROUND_ADD, DV_NULL, "D-Type Font Engine");

	Engine.OutputSetStyleEffects(style4);
	Engine.TypesetterSetTypeEffects(type4);
	Engine.CharsDoOutput(10 * Zoom - X, 470 * Zoom - Y, 0, DV_SPACING_ROUND_ADD, DV_NULL, "Outlined and Dashed Text");
}


void CDTApplet::DrawPage_06()
{
	// This example draws all Windows ANSI characters using D-Type Engine

	DT_CHAR s[512];
	DT_UBYTE buffer[256];
	DT_STYLE_EFFECTS style = {1, {0}, {0, 0, 0, 0}, DV_NULL};
	DT_TYPE_EFFECTS_L type = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(17), ScaleA(17), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_SLONG nr_of_glyphs, i;
	DT_SLONG y2;
	DT_FLOAT f;
	DT_CHAR c;

	Engine.OutputSetStyleEffects(style);
	Engine.TypesetterSetTypeEffects(type);

	nr_of_glyphs = Engine.FontGetNrOfGlyphs(type.font_index);
	Engine.CharsDoOutput(ScaleB(5) - X, ScaleB(20) - Y, 0, DV_SPACING_DEVICE, DV_NULL, "Windows ANSI Character Table");

	Engine.FontGetStringValue(type.font_index, DV_SVAL_ASC_FONTNAME, buffer, 255, true);
	sprintf(s, "FONT NAME: %s, NUMBER OF GLYPHS IN FONT: %.0f", buffer, static_cast<DT_FLOAT>(nr_of_glyphs));
	Engine.CharsDoOutput(ScaleB(5) - X, ScaleB(40) - Y, 0, DV_SPACING_DEVICE, DV_NULL, s);

	for (i = 0; i < 32; i++)
	{
		y2 = ScaleB(23 * (i + 3)) - Y;
		f = i; c = static_cast<DT_CHAR>(i); sprintf(s, "%.0f) %c", f, c);
		Engine.CharsDoOutput(ScaleB(5) - X, y2, 0, DV_SPACING_DEVICE, DV_NULL, s);

		f = i + 32; c = static_cast<DT_CHAR>(i + 32); sprintf(s, "%.0f) %c", f, c);
		Engine.CharsDoOutput(ScaleB(105) - X, y2, 0, DV_SPACING_DEVICE, DV_NULL, s);

		f = i + 64; c = static_cast<DT_CHAR>(i + 64); sprintf(s, "%.0f) %c", f, c);
		Engine.CharsDoOutput(ScaleB(205) - X, y2, 0, DV_SPACING_DEVICE, DV_NULL, s);

		f = i + 96; c = static_cast<DT_CHAR>(i + 96); sprintf(s, "%.0f) %c", f, c);
		Engine.CharsDoOutput(ScaleB(305) - X, y2, 0, DV_SPACING_DEVICE, DV_NULL, s);

		f = i + 128; c = static_cast<DT_CHAR>(i + 128); sprintf(s, "%.0f) %c", f, c);
		Engine.CharsDoOutput(ScaleB(405) - X, y2, 0, DV_SPACING_DEVICE, DV_NULL, s);

		f = i + 160; c = static_cast<DT_CHAR>(i + 160); sprintf(s, "%.0f) %c", f, c);
		Engine.CharsDoOutput(ScaleB(505) - X, y2, 0, DV_SPACING_DEVICE, DV_NULL, s);

		f = i + 192; c = static_cast<DT_CHAR>(i + 192); sprintf(s, "%.0f) %c", f, c);
		Engine.CharsDoOutput(ScaleB(605) - X, y2, 0, DV_SPACING_DEVICE, DV_NULL, s);

		f = i + 224; c = static_cast<DT_CHAR>(i + 224); sprintf(s, "%.0f) %c", f, c);
		Engine.CharsDoOutput(ScaleB(705) - X, y2, 0, DV_SPACING_DEVICE, DV_NULL, s);
	}
}


void CDTApplet::DrawPage_07()
{
	// This example draws lines of glyphs/characters with kerning off and on

	DT_STYLE_EFFECTS style = {1, {0}, {120, 50, 230, 0}, DV_NULL};
	DT_TYPE_EFFECTS_L type = {Engine.FontFind("F0010_BACHR__SS0", 0), 0, 0, {{ScaleA(50), ScaleA(50), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	Engine.OutputSetStyleEffects(style);
	Engine.TypesetterSetTypeEffects(type);

	Engine.CharsDoOutput(10 * Zoom - X, 70 * Zoom - Y, 0, DV_SPACING_ROUND_ADD, DV_NULL, "ALTAWAY (KERNING OFF)");
	Engine.CharsDoOutput(10 * Zoom - X, 140 * Zoom - Y, 0, DV_SPACING_KERN_ROUND_ADD, DV_NULL, "ALTAWAY (KERNING ON)");
}


void CDTApplet::DrawPage_08()
{
	// This example draws 77 interpolated font instances using a single Multiple-Master font

	DT_CHAR s[1024];
	DT_UBYTE buffer1[256], buffer2[256];
	DT_SWORD font_axis_count, font_dim;
	DT_SLONG font_instance_count;

	DT_STYLE_EFFECTS style = {1, {0}, {20, 115, 0, 0}, DV_NULL};
	DT_TYPE_EFFECTS_L type = {0, 0, 0, {{ScaleA(30), ScaleA(30), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	type.font_index = FontIndex_MM[0];

	Engine.OutputSetStyleEffects(style);
	Engine.TypesetterSetTypeEffects(type);

	// Draw the info line

	Engine.FontGetStringValue(type.font_index, DV_SVAL_ASC_FONTNAME, buffer1, 255, true);
	Engine.FontGetStringValue(type.font_index, DV_SVAL_ASC_MMDESIGNNAMES, buffer2, 255, true);
	sprintf(s, "FONT NAME: %s, BASE DESIGN NAMES: %s", buffer1, buffer2);

	Engine.CharsDoOutput(20 * Zoom - X, 40 * Zoom - Y, 0, DV_SPACING_KERN_FRAC, DV_NULL, s);

	font_axis_count = Engine.FontGetMMAxisInfo(type.font_index, &font_dim, &font_instance_count);
	sprintf(s, "FONT AXES: %.0f, FONT DIM: %.0f, FONT INSTANCES: %.0f", static_cast<DT_FLOAT>(font_axis_count), static_cast<DT_FLOAT>(font_dim), static_cast<DT_FLOAT>(font_instance_count));

	Engine.CharsDoOutput(20 * Zoom - X, 70 * Zoom - Y, 0, DV_SPACING_KERN_FRAC, DV_NULL, s);

	// Draw a sample of all the previously created font instances

	Engine.CharsDoOutput(20 * Zoom - X, 125 * Zoom - Y, 0, DV_SPACING_KERN_FRAC, DV_NULL, "INTERPOLATED FONT INSTANCES:");

	type.transform.params.size_h = ScaleA(50);
	type.transform.params.size_v = ScaleA(50);

	DT_SLONG i = 1, i_x, i_y;

	for (i_y = 0; i_y <= DV_MM_INTERPOLATIONS_Y; i_y++)
	for (i_x = 0; i_x <= DV_MM_INTERPOLATIONS_X; i_x++)
	{
		type.font_index = FontIndex_MM[i];
		Engine.TypesetterSetTypeEffects(type);

		Engine.CharsDoOutput((20 + 340 * i_x) * Zoom - X, (180 + 80 * i_y) * Zoom - Y, 80, DV_SPACING_KERN_FRAC, DV_NULL, "D-Type");
		i++;
	}
}


void CDTApplet::DrawPage_09()
{
	// This example demonstrates special style effects

	DT_TM2X2 tm;
	DT_UWORD font_em_h, font_em_v;
	DT_FLOAT s, c, kh, kv;

	DT_STYLE_EFFECTS style1 = {1, {0}, {255, 190, 190, 120}, DV_NULL};
	DT_STYLE_EFFECTS style2 = {1, {0}, {160, 140, 54, 160}, DV_NULL};
	DT_STYLE_EFFECTS style3 = {3, {3, 8, 8}, {80, 80, 80, 150}, DV_NULL};
	DT_STYLE_EFFECTS style4 = {1, {0}, {160, 140, 54, 0}, DV_NULL};
	DT_STYLE_EFFECTS style5 = {1, {0}, {10, 140, 30, 0}, DV_NULL};
	DT_STYLE_EFFECTS style6 = {1, {0}, {230, 220, 250, 0}, DV_NULL};
	DT_STYLE_EFFECTS style7 = {1, {0}, {10, 50, 90, 0}, DV_NULL};
	DT_STYLE_EFFECTS style8 = {3, {3, 2, 2}, {120, 120, 120, 200}, DV_NULL};
	DT_STYLE_EFFECTS style9 = {1, {0}, {0, 0, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style10 = {1, {0}, {10, 140, 30, 0}, DV_NULL};
	DT_STYLE_EFFECTS style11 = {3, {7, 8, 8}, {70, 20, 80, 0}, DV_NULL};

	DT_TYPE_EFFECTS_L type1 = {Engine.FontFind("F1040_INVISB_NS0", 0), 0, 0, {{ScaleA(700), ScaleA(700), 0, 0, 25}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type2 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(290), ScaleA(200), -10, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type3 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(265), ScaleA(200), 0, 10, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type4 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(370), ScaleA(200), 0, 0, 0}}, {ScaleC(800), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type5 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(230), ScaleA(200), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type6 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(230), ScaleA(200), 0, 0, 0}}, {ScaleC(400), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type7 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(220), ScaleA(200), 0, 0, 0}}, {ScaleC(450), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type8 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(190), ScaleA(200), 0, 0, 0}}, {ScaleC(-900), 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type9 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(320), ScaleA(200), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	// Draw big rotated text in the background

	Engine.TypesetterSetTypeEffects(type1);
	Engine.OutputSetStyleEffects(style1);

	// Compute transform matrix for rotation
	Engine.FontGetMetrics(type1.font_index, &font_em_h, &font_em_v);
	c = cos(DF_RAD(type1.transform.params.rotation));
	s = sin(DF_RAD(type1.transform.params.rotation));
	kh = type1.transform.params.size_h; kh /= font_em_h;
	kv = type1.transform.params.size_v; kv /= font_em_v;

	tm[0][0] = c * kh;
	tm[0][1] = -s * kv;
	tm[1][0] = s * kh;
	tm[1][1] = c * kv;

	Engine.CharsDoOutput(350 * Zoom - X, -80 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, tm, "DType");
	Engine.CharsDoOutput(40 * Zoom - X, 550 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, tm, "DType");
	Engine.CharsDoOutput(-270 * Zoom - X, 1180 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, tm, "DType");

	// Draw tansparent text
	Engine.TypesetterSetTypeEffects(type2);
	Engine.OutputSetStyleEffects(style2);
	Engine.CharsDoOutput(10 * Zoom - X, 200 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Transparent text");

	// Draw text with shadow
	Engine.TypesetterSetTypeEffects(type3);
	Engine.OutputSetStyleEffects(style3);
	Engine.CharsDoOutput(20 * Zoom - X, 410 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Text with shadow");
	Engine.OutputSetStyleEffects(style4);
	Engine.CharsDoOutput(10 * Zoom - X, 400 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Text with shadow");

	// Draw outlined text
	Engine.TypesetterSetTypeEffects(type4);
	Engine.OutputSetStyleEffects(style5);
	Engine.CharsDoOutput(10 * Zoom - X, 600 * Zoom - Y, 10, DV_SPACING_ADD_ROUND, DV_NULL, "Outlined text");

	// Draw outlined text with interior fill
	Engine.TypesetterSetTypeEffects(type5);
	Engine.OutputSetStyleEffects(style6);
	Engine.CharsDoOutput(10 * Zoom - X, 800 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Outlined with interior");
	Engine.TypesetterSetTypeEffects(type6);
	Engine.OutputSetStyleEffects(style7);
	Engine.CharsDoOutput(10 * Zoom - X, 800 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Outlined with interior");

	// Draw outlined text with shadow
	Engine.TypesetterSetTypeEffects(type7);
	Engine.OutputSetStyleEffects(style8);
	Engine.CharsDoOutput(18 * Zoom - X, 1008 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Outlined with shadow");
	Engine.OutputSetStyleEffects(style9);
	Engine.CharsDoOutput(10 * Zoom - X, 1000 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Outlined with shadow");

	// Draw dilated text
	Engine.TypesetterSetTypeEffects(type8);
	Engine.OutputSetStyleEffects(style10);
	Engine.CharsDoOutput(18 * Zoom - X, 1208 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Dilated (emboldened) text");

	// Draw embossed text
	Engine.TypesetterSetTypeEffects(type9);
	Engine.OutputSetStyleEffects(style11);
	Engine.CharsDoOutput(18 * Zoom - X, 1408 * Zoom - Y, 0, DV_SPACING_ADD_ROUND, DV_NULL, "Embossed text");
}


void CDTApplet::DrawPage_10()
{
	// This example demonstrates how to draw glyph/character arcs using D-Type Engine

	DT_TYPE_EFFECTS_L arc_effects;
	DT_STYLE_EFFECTS style1 = {1, {0}, {120, 50, 230, 0}, DV_NULL};
	DT_STYLE_EFFECTS style2 = {1, {0}, {52, 130, 0, 0}, DV_NULL};

	Engine.OutputSetStyleEffects(style1);

	// Set arc type effects
	arc_effects.font_index = Engine.FontFind("F0010_DENMRK_SS0", 0);
	arc_effects.reserved = 0;
	arc_effects.descriptor = 0;
	arc_effects.transform.params.size_h = ScaleA(30);
	arc_effects.transform.params.size_v = ScaleA(30);
	arc_effects.transform.params.skew_h = 0;
	arc_effects.transform.params.skew_v = 0;
	arc_effects.linedecor.thickness = 0;
	arc_effects.linedecor.segment = 0;
	arc_effects.linedecor.shift = 0;
	arc_effects.linedecor.dash_size = 0;
	arc_effects.linedecor.flags = 0;
	arc_effects.linedecor.scale_id = DV_SCALE_100;

	// Draw - method 1 (TOP LEFT CORNER)
	Engine.CharsDoOutput_Arc(ScaleB(200) - X, ScaleB(180) - Y, ScaleB(150), 205, 0, arc_effects, 0, DV_SPACING_FRAC, DV_NULL, "Hello World! This is just a test!");
	Engine.CharsDoOutput_Arc(ScaleB(200) - X, ScaleB(180) - Y, ScaleB(100), 205, 0, arc_effects, 0, DV_SPACING_FRAC, DV_NULL, "Hello World! This is just a test!");
	Engine.CharsDoOutput_Arc(ScaleB(200) - X, ScaleB(180) - Y, ScaleB(50), 205, 0, arc_effects, 0, DV_SPACING_FRAC, DV_NULL, "D-TYPE");

	// Draw - method 2 (TOP RIGHT CORNER)
	Engine.CharsDoOutput_Arc(ScaleB(600) - X, ScaleB(180) - Y, ScaleB(150), 205, 130, arc_effects, 0, DV_SPACING_FRAC, DV_NULL, "Hello World! This is just a test!");
	Engine.CharsDoOutput_Arc(ScaleB(600) - X, ScaleB(180) - Y, ScaleB(100), 205, 130, arc_effects, 0, DV_SPACING_FRAC, DV_NULL, "Hello World! This is just a test!");
	Engine.CharsDoOutput_Arc(ScaleB(600) - X, ScaleB(180) - Y, ScaleB(50), 205, 130, arc_effects, 0, DV_SPACING_FRAC, DV_NULL, "D-TYPE");

	Engine.OutputSetStyleEffects(style2);

	// Set arc type effects
	arc_effects.font_index = Engine.FontFind("F0010_DENMRK_SS0", 0);
	arc_effects.reserved = 0;
	arc_effects.descriptor = 0;
	arc_effects.transform.params.size_h = ScaleA(30);
	arc_effects.transform.params.size_v = ScaleA(60);
	arc_effects.transform.params.skew_h = 0;
	arc_effects.transform.params.skew_v = 0;
	arc_effects.linedecor.thickness = ScaleC(100);
	arc_effects.linedecor.segment = 0;
	arc_effects.linedecor.shift = 0;
	arc_effects.linedecor.dash_size = 0;
	arc_effects.linedecor.flags = 0;
	arc_effects.linedecor.scale_id = DV_SCALE_100;

	// Draw - method 1 (BOTTOM LEFT CORNER)
	Engine.CharsDoOutput_Arc(ScaleB(200) - X, ScaleB(530) - Y, ScaleB(150), 180, 0, arc_effects, 100, DV_SPACING_FRAC, DV_NULL, "D-Type Font Engine");
	Engine.CharsDoOutput_Arc(ScaleB(200) - X, ScaleB(530) - Y, ScaleB(50), 180, 0, arc_effects, 100, DV_SPACING_FRAC, DV_NULL, "TEXT IN ARC");

	// Draw - method 2 (BOTTOM RIGHT CORNER)
	Engine.CharsDoOutput_Arc(ScaleB(600) - X, ScaleB(530) - Y, ScaleB(150), 180, 180, arc_effects, 100, DV_SPACING_FRAC, DV_NULL, "D-Type Font Engine");
	Engine.CharsDoOutput_Arc(ScaleB(600) - X, ScaleB(530) - Y, ScaleB(50), 180, 180, arc_effects, 100, DV_SPACING_FRAC, DV_NULL, "TEXT IN ARC");
}


void CDTApplet::DrawPage_11()
{
	// This example draws a more complex layout using D-Type Engine

	DT_SLONG i, j;
	DT_FLOAT x2, y2;

	DT_TM2X2 tm;
	DT_UWORD font_em_h, font_em_v;
	DT_FLOAT s, c, kh, kv;

	DT_STYLE_EFFECTS style1 = {1, {0}, {120, 50, 230, 0}, DV_NULL};
	DT_STYLE_EFFECTS style2 = {1, {0}, {7, 100, 250, 0}, DV_NULL};
	DT_STYLE_EFFECTS style3 = {1, {0}, {89, 167, 234, 0}, DV_NULL};
	DT_STYLE_EFFECTS style4 = {1, {0}, {52, 130, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style5 = {1, {0}, {52, 30, 80, 0}, DV_NULL};
	DT_STYLE_EFFECTS style6 = {1, {0}, {20, 0, 180, 0}, DV_NULL};
	DT_STYLE_EFFECTS style7 = {1, {0}, {0, 0, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style8 = {1, {0}, {255, 0, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style9 = {1, {0}, {255, 189, 0, 127}, DV_NULL};

	DT_TYPE_EFFECTS_L type1 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(35), ScaleA(35), -10, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type2 = {Engine.FontFind("F0011_TEX10__SS0", 0), 0, 0, {{ScaleA(15), ScaleA(15), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type3 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(30), ScaleA(21), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type4 = {Engine.FontFind("F0011_TEX10__SS0", 0), 0, 0, {{ScaleA(26), ScaleA(28), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type5 = {Engine.FontFind("F0011_TEX10__SS0", 0), 0, 0, {{ScaleA(20), ScaleA(-21), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type6 = {Engine.FontFind("F0011_TEX10__SS0", 0), 0, 0, {{ScaleA(23), ScaleA(24), -12, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type7 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(30), ScaleA(15), 0, -12, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type8 = {Engine.FontFind("F0011_TEX10__SS0", 0), 0, 0, {{ScaleA(30), ScaleA(30), -20, -20, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type9 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(25), ScaleA(25), 0, 0, 25}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type10 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(15), ScaleA(35), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type11 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(10), ScaleA(20), 12, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type12 = {Engine.FontFind("F0011_SYMBL7_SS0", 0), 0, 0, {{ScaleA(35), ScaleA(35), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};
	DT_TYPE_EFFECTS_L type13 = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(230), ScaleA(230), 0, 0, 28}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	// Draw lines of characters

	for (j = 0; j < 3; j++)
	{
		x2 = X - Zoom * j * 750;
		for (i = 0; i < 3; i++) if ((j != 1) || (i != 1))
		{
			y2 = Y - Zoom * i * 450;
			Engine.OutputSetStyleEffects(style1);
			Engine.TypesetterSetTypeEffects(type1);
			Engine.CharsDoOutput(12 * Zoom - x2, 53 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, "D-TYPE: New font technology");

			Engine.OutputSetStyleEffects(style2);
			Engine.TypesetterSetTypeEffects(type2);
			Engine.CharsDoOutput(12 * Zoom - x2, 72 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, "smooth and beautiful type at any size");

			Engine.OutputSetStyleEffects(style3);
			Engine.TypesetterSetTypeEffects(type3);
			Engine.CharsDoOutput(12 * Zoom - x2, 100 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, "D-TYPE: New font technology");

			Engine.OutputSetStyleEffects(style4);
			Engine.TypesetterSetTypeEffects(type4);
			Engine.CharsDoOutput(12 * Zoom - x2, 135 * Zoom - y2, 25, DV_SPACING_ROUND_ADD, DV_NULL, "smooth and beautiful type at any size");

			Engine.OutputSetStyleEffects(style5);
			Engine.TypesetterSetTypeEffects(type5);
			Engine.CharsDoOutput(12 * Zoom - x2, 155 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, "smooth and beautiful type at any size");

			Engine.OutputSetStyleEffects(style6);
			Engine.TypesetterSetTypeEffects(type6);
			Engine.CharsDoOutput(11 * Zoom - x2, 205 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, "smooth and beautiful type at any size");

			Engine.OutputSetStyleEffects(style7);
			Engine.TypesetterSetTypeEffects(type7);
			Engine.CharsDoOutput(12 * Zoom - x2, 235 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, "smooth and beautiful type at any size");

			Engine.TypesetterSetTypeEffects(type8);
			Engine.CharsDoOutput(12 * Zoom - x2, 275 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, "D-TYPE: New font technology");

			Engine.TypesetterSetTypeEffects(type9);
			Engine.CharsDoOutput(11 * Zoom - x2, 305 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, "smooth and beautiful type at any size");

			Engine.TypesetterSetTypeEffects(type10);
			Engine.CharsDoOutput(12 * Zoom - x2, 355 * Zoom - y2, 400, DV_SPACING_ROUND_ADD, DV_NULL, "D-TYPE: New font technology");

			Engine.TypesetterSetTypeEffects(type11);
			Engine.CharsDoOutput(10 * Zoom - x2, 390 * Zoom - y2, 10, DV_SPACING_ROUND_ADD, DV_NULL, " D-TYPE: New font technology");

			Engine.TypesetterSetTypeEffects(type12);
			Engine.CharsDoOutput(12 * Zoom - x2, 430 * Zoom - y2, 150, DV_SPACING_ROUND_ADD, DV_NULL, "ABCdefghijklmnopqrstuvwxyz");
		}
	}

	// Draw shapes

	Engine.OutputSetStyleEffects(style8);
	Engine.Shape_Line(ScaleB(820) - X, ScaleB(580) - Y, 0, 0, ScaleB(160), ScaleB(40), 1 * Zoom, 0, 0);

	Engine.OutputSetStyleEffects(style1);
	Engine.Shape_Line(ScaleB(770) - X, ScaleB(520) - Y, 0, 0, ScaleB(360), ScaleB(40), 3.4 * Zoom, 0, 0);

	Engine.OutputSetStyleEffects(style2);
	Engine.Shape_Ellipse(ScaleB(770) - X, ScaleB(520) - Y, 0, 0, ScaleB(100), ScaleB(222), 3 * Zoom);

	Engine.OutputSetStyleEffects(style3);
	Engine.Shape_EllipseFilled(ScaleB(1230) - X, ScaleB(510) - Y, 0, 0, ScaleB(150), ScaleB(150));

	Engine.OutputSetStyleEffects(style4);
	Engine.Shape_Line(ScaleB(890) - X, ScaleB(635) - Y, 0, 0, ScaleB(300), ScaleB(150), 25 * Zoom, 1, 0);

	Engine.OutputSetStyleEffects(style3);
	Engine.Shape_Ellipse(ScaleB(810) - X, ScaleB(650) - Y, 0, 0, ScaleB(600), ScaleB(150), 20 * Zoom);

	// Draw transparent rotated lines of characters on top

	// Compute transform matrix for rotation
	Engine.FontGetMetrics(type13.font_index, &font_em_h, &font_em_v);
	c = cos(DF_RAD(type13.transform.params.rotation));
	s = sin(DF_RAD(type13.transform.params.rotation));
	kh = type13.transform.params.size_h; kh /= font_em_h;
	kv = type13.transform.params.size_v; kv /= font_em_v;

	tm[0][0] = c * kh;
	tm[0][1] = -s * kv;
	tm[1][0] = s * kh;
	tm[1][1] = c * kv;

	Engine.OutputSetStyleEffects(style9);
	Engine.TypesetterSetTypeEffects(type13);
	Engine.CharsDoOutput(30 * Zoom - X, 180 * Zoom - Y, 0, DV_SPACING_ROUND_ADD, tm, "50% TRANSPARENCY");

}


void CDTApplet::DrawPage_12()
{
	// This example draws user-defined polygons and characters as polygons using D-Type Engine

	DT_SLONG i, j;
	DT_UWORD font_em_h, font_em_v;
	DT_FLOAT kh, kv;

	DT_STYLE_EFFECTS style1 = {1, {0}, {7, 100, 250, 0}, DV_NULL};
	DT_STYLE_EFFECTS style2 = {1, {0}, {220, 70, 130, 0}, DV_NULL};
	DT_STYLE_EFFECTS style3 = {1, {0}, {0, 255, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style4 = {1, {0}, {0, 0, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style5 = {1, {0}, {255, 0, 0, 0}, DV_NULL};
	DT_STYLE_EFFECTS style6 = {1, {0}, {255, 0, 255, 0}, DV_NULL};

	DT_UBYTE* ct = DV_NULL;
	DT_SLONG* cx = DV_NULL;
	DT_SLONG* cy = DV_NULL;
	DT_FLOAT* cx_f = DV_NULL;
	DT_FLOAT* cy_f = DV_NULL;

	// Allocate memory for the 3-dimensional array
	if ((ct = new DT_UBYTE[DV_MAXPOINTS]) == DV_NULL) goto end;
	if ((cx = new DT_SLONG[DV_MAXPOINTS]) == DV_NULL) goto end;
	if ((cy = new DT_SLONG[DV_MAXPOINTS]) == DV_NULL) goto end;
	if ((cx_f = new DT_FLOAT[DV_MAXPOINTS]) == DV_NULL) goto end;
	if ((cy_f = new DT_FLOAT[DV_MAXPOINTS]) == DV_NULL) goto end;

	// Get the outline curve for an outline character
	j = Engine.CharGetOutline(Engine.FontFind("F0010_HSANSS_SS0", 0), 'M', DV_MAXPOINTS, ct, cx, cy, 0);
	if (j > 0 && j <= DV_MAXPOINTS)
	{
		/* Scale polygon so it fits within 500x500 pixel box */
		Engine.FontGetMetrics(Engine.FontFind("F0010_HSANSS_SS0", 0), &font_em_h, &font_em_v);
		kh = 500; kh /= font_em_h;
		kv = 500; kv /= font_em_v;

		for (i = 0; i < j; i++)
		{
			cx_f[i] = kh * cx[i] * Zoom;
			cy_f[i] = -kv * cy[i] * Zoom;
		}

		Engine.OutputSetStyleEffects(style1);
		Engine.Shape_PolyFilled(ScaleB(100) - X, ScaleB(400) - Y, ct, cx_f, cy_f, j);
		Engine.OutputSetStyleEffects(style2);
		Engine.Shape_Poly(ScaleB(600) - X, ScaleB(400) - Y, ct, cx_f, cy_f, j, 5 * Zoom, 1, 1);
		Engine.OutputSetStyleEffects(style3);
		Engine.Shape_Poly(ScaleB(1100) - X, ScaleB(400) - Y, ct, cx_f, cy_f, j, 30 * Zoom, 1, 1);
		Engine.OutputSetStyleEffects(style4);
		Engine.Shape_PolyDashed(ScaleB(1600) - X, ScaleB(400) - Y, ct, cx_f, cy_f, j, Zoom * 35, DV_NULL, 1 * Zoom, 1, 1);
	}

	// Draw Polygons
	ct[0] = 16; cx_f[0] =   0 * Zoom; cy_f[0] =    0 * Zoom;
	ct[1] = 20; cx_f[1] = 100 * Zoom; cy_f[1] = -200 * Zoom;
	ct[2] = 20; cx_f[2] = 200 * Zoom; cy_f[2] =  -50 * Zoom;
	ct[3] = 20; cx_f[3] = 300 * Zoom; cy_f[3] = -200 * Zoom;
	ct[4] = 20; cx_f[4] = 400 * Zoom; cy_f[4] =    0 * Zoom;

	Engine.OutputSetStyleEffects(style5);
	Engine.Shape_PolyFilled(ScaleB(100) - X, ScaleB(680) - Y, ct, cx_f, cy_f, 5);
	Engine.OutputSetStyleEffects(style4);
	Engine.Shape_Poly(ScaleB(600) - X, ScaleB(680) - Y, ct, cx_f, cy_f, 5, 30 * Zoom, 1, 1);
	Engine.Shape_Poly(ScaleB(1100) - X, ScaleB(680) - Y, ct, cx_f, cy_f, 5, 5 * Zoom, 1, 1);
	Engine.OutputSetStyleEffects(style5);
	Engine.Shape_PolyDashed(ScaleB(1600) - X, ScaleB(680) - Y, ct, cx_f, cy_f, 5, Zoom * 50, DV_NULL, 20 * Zoom, 1, 1);

	// Get the outline curve for an outline character
	j = Engine.CharGetOutline(Engine.FontFind("F0016_OL54___NS0", 0), 'A', DV_MAXPOINTS, ct, cx, cy, 0);
	if (j > 0 && j <= DV_MAXPOINTS)
	{
		/* Scale polygon so it fits within 500x500 pixel box */
		Engine.FontGetMetrics(Engine.FontFind("F0016_OL54___NS0", 0), &font_em_h, &font_em_v);
		kh = 500; kh /= font_em_h;
		kv = 500; kv /= font_em_v;

		for (i = 0; i < j; i++)
		{
			cx_f[i] = kh * cx[i] * Zoom;
			cy_f[i] = -kv * cy[i] * Zoom;
		}

		Engine.OutputSetStyleEffects(style1);
		Engine.Shape_PolyFilled(ScaleB(100) - X, ScaleB(1150) - Y, ct, cx_f, cy_f, j);
		Engine.OutputSetStyleEffects(style2);
		Engine.Shape_Poly(ScaleB(600) - X, ScaleB(1150) - Y, ct, cx_f, cy_f, j, 1 * Zoom, 1, 1);
		Engine.OutputSetStyleEffects(style3);
		Engine.Shape_Poly(ScaleB(1100) - X, ScaleB(1150) - Y, ct, cx_f, cy_f, j, 10 * Zoom, 1, 1);
		Engine.OutputSetStyleEffects(style6);
		Engine.Shape_PolyDashed(ScaleB(1600) - X, ScaleB(1150) - Y, ct, cx_f, cy_f, j, Zoom * 30, DV_NULL, 5 * Zoom, 1, 1);
	}

end:

	delete[] ct;
	delete[] cx;
	delete[] cy;
	delete[] cx_f;
	delete[] cy_f;
}


void CDTApplet::DrawPage_13()
{
	// This example first draws characters using the Font Engine, then it retrieves their scaled outlines and draws them again using the Rasterizer alone

	DT_BMP bmp;

	DT_FLOAT f, x2, y2;
	DT_SLONG xr, yr, dy;
	DT_UWORD font_em_h, font_em_v, char_width;

	DT_STYLE_EFFECTS style1 = {1, {0}, {7, 100, 250, 0}, DV_NULL};
	DT_TYPE_EFFECTS_L type1 = {Engine.FontFind("F0010_DENMRK_SS0", 0), 0, 0, {{ScaleA(60), ScaleA(90), -12, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	const DT_CHAR* line = "Text sample rendered first by Font Engine, then by Rasterizer";
	DT_SLONG i = 0;

	DT_SLONG n;
	DT_UBYTE* ct = DV_NULL;
	DT_FLOAT* cx_f = DV_NULL;
	DT_FLOAT* cy_f = DV_NULL;
	DT_RECT_SLONG bound_box;

	// Allocate memory for the 3-dimensional array
	if ((ct = new DT_UBYTE[DV_MAXPOINTS]) == DV_NULL) goto end;
	if ((cx_f = new DT_FLOAT[DV_MAXPOINTS]) == DV_NULL) goto end;
	if ((cy_f = new DT_FLOAT[DV_MAXPOINTS]) == DV_NULL) goto end;

	Engine.OutputSetStyleEffects(style1);

	// Get font information
	Engine.FontGetMetrics(type1.font_index, &font_em_h, &font_em_v);
	f = type1.transform.params.size_h; f /= font_em_h;

	x2 = 10 * Zoom - X;
	y2 = 100 * Zoom - Y;

	yr = DF_ROUND(y2);
	dy = DF_ROUND(110 * Zoom);

	// Draw characters one by one
	while (line[i] != 0)
	{
		xr = DF_ROUND(x2);

		// Line 1: Normal characters render by Font Engine
		type1.linedecor.thickness = 0;
		Engine.TypesetterSetTypeEffects(type1);
		Engine.CharDoOutput(line[i], xr, yr);

		// Line 2: Outlined characters rendered by Font Engine
		type1.linedecor.thickness = ScaleC(100);
		Engine.TypesetterSetTypeEffects(type1);
		Engine.CharDoOutput(line[i], xr, yr + dy);

		n = Engine.CharDoTransform(line[i], xr, yr, 1, DV_MAXPOINTS, ct, cx_f, cy_f, &bmp);
		if (n > 0 && n <= DV_MAXPOINTS)
		{
			bound_box.xmn = 0;
			bound_box.ymn = 0;
			bound_box.xmx = bmp.w;
			bound_box.ymx = bmp.h;

			// Line 3: Normal characters render by Rasterizer
			Engine.Shape_PolyFilled(xr + bmp.x, yr - bmp.y + 2 * dy, ct, cx_f, cy_f, n, &bound_box);

			// Line 4: Outlined characters rendered by Rasterizer
			Engine.Shape_Poly(xr + bmp.x, yr - bmp.y + 3 * dy, ct, cx_f, cy_f, n, type1.linedecor.thickness * 0.01, 1, 1);
		}

		// Increase distance by character's width
		Engine.CharGetMetrics(type1.font_index, line[i], &char_width);
		x2 += f * char_width;

		i++;
	}

end:
	// Release the 3-dimensional array

	delete[] ct;
	delete[] cx_f;
	delete[] cy_f;
}


void CDTApplet::DrawPage_14()
{
	// This example shows how to draw custom shapes using the Shape Engine

	#define DV_NR_OF_POINTS 21

	DT_RECT_SLONG extent = {0, 0, ScaleB(560), ScaleB(415)};
	DT_UBYTE i_arr[DV_NR_OF_POINTS] = {16, 20, 20, 20, 20, 16, 20, 20, 20, 20, 16, 20, 20, 20, 20, 16, 20, 20, 20, 20, 8};
	DT_FLOAT x_arr[DV_NR_OF_POINTS] = {27, 6, -27, 39, -36, 109, -38, -43, 107, -130, 154, 114, -207, 222, -151, 429, 27, -210, 314, -295, 0};
	DT_FLOAT y_arr[DV_NR_OF_POINTS] = {373, 38, -27, 6, 17, 275, 126, -125, 75, 3, 112, 195, -93, -47, 168, 4, 314, -240, 73, 123, 0};
	DT_SLONG i;

	DT_STYLE_EFFECTS style1 = {1, {0}, {7, 100, 250, 0}, DV_NULL};

	for (i = 0; i < DV_NR_OF_POINTS; i++)
	{
		x_arr[i] = x_arr[i] * Zoom;
		y_arr[i] = y_arr[i] * Zoom;
	}

	Engine.OutputSetStyleEffects(style1);

	Engine.RasterizerSetFillRule(DV_NZW_OFF);
	Engine.ShapesDoOutput(DV_BITMAP_OFF, ScaleB(10) - X, ScaleB(10) - Y, extent, DV_SHAPE_ARRAY_MIXED, i_arr, x_arr, y_arr);
	Engine.RasterizerSetFillRule(DV_NZW_ON);
	Engine.ShapesDoOutput(DV_BITMAP_OFF, ScaleB(500) - X, ScaleB(10) - Y, extent, DV_SHAPE_ARRAY_MIXED, i_arr, x_arr, y_arr);
}


void CDTApplet::DrawPage_15()
{
	// This example demonstrates various glyph/spacing character modes

	DT_STYLE_EFFECTS style = {1, {0}, {0, 0, 0, 0}, DV_NULL};
	DT_TYPE_EFFECTS_L type = {Engine.FontFind("F0010_HSANSS_SS0", 0), 0, 0, {{ScaleA(16), ScaleA(16), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	const DT_CHAR* lines[] =
	{
		"Saturn is the most distant of the five planets known to ancient stargazers. In 1610, Italian",
		"Galileo Galilei was the first astronomer to gaze at Saturn through a telescope. To his surprise,",
		"he saw a pair of objects on either side of the planet, which he later drew as cup handles",
		"attached to the planet on each side. In 1659, Dutch astronomer Christiaan Huygens announced",
		"that this was a ring encircling the planet. In 1675, Italian-born astronomer Jean Dominique",
		"Cassini discovered a gap between what are now called the A and B rings."
	};

	DT_SLONG i, nr_of_rows = 6;
	DT_SLONG row_height1 = ScaleB(11);
	DT_SLONG row_height2 = 2 * row_height1;
	DT_SLONG row_height3 = 3 * row_height1;
	DT_FLOAT hx = ScaleB(10) - X;
	DT_FLOAT tx = ScaleB(40) - X;
	DT_FLOAT ty = ScaleB(0) - Y;

	Engine.OutputSetStyleEffects(style);
	Engine.TypesetterSetTypeEffects(type);

	ty += row_height2;
	Engine.CharsDoOutput(hx, ty, 0, DV_SPACING_DEVICE_2, DV_NULL, "D-Type Font Engine offers a variety of spacing modes. Here are a few examples:");
	ty += row_height2;

	// DV_SPACING_DEVICE_2 - great for device dependent output
	ty += row_height2;
	Engine.CharsDoOutput(hx, ty, 0, DV_SPACING_DEVICE_2, DV_NULL, "DV_SPACING_DEVICE_2 and DV_SPACING_KERN_DEVICE_2 - great for device dependent output");
	ty += row_height3;
	for (i = 0; i < nr_of_rows; i++, ty += row_height2)
	{
		Engine.CharsDoOutput(tx, ty, 0, DV_SPACING_DEVICE_2, DV_NULL, lines[i]);
	}

	// DV_SPACING_DEVICE - great for device dependent output
	ty += row_height2;
	Engine.CharsDoOutput(hx, ty, 0, DV_SPACING_DEVICE, DV_NULL, "DV_SPACING_DEVICE and DV_SPACING_KERN_DEVICE - great for device dependent output");
	ty += row_height3;
	for (i = 0; i < nr_of_rows; i++, ty += row_height2)
	{
		Engine.CharsDoOutput(tx, ty, 0, DV_SPACING_DEVICE, DV_NULL, lines[i]);
	}

	// DV_SPACING_FRAC - great for device independent output
	ty += row_height2;
	Engine.CharsDoOutput(hx, ty, 0, DV_SPACING_FRAC, DV_NULL, "DV_SPACING_FRAC and DV_SPACING_KERN_FRAC - great for device independent output");
	ty += row_height3;
	for (i = 0; i < nr_of_rows; i++, ty += row_height2)
	{
		Engine.CharsDoOutput(tx, ty, 0, DV_SPACING_FRAC, DV_NULL, lines[i]);
	}

	// DV_SPACING_ROUND_ADD - for device dependent output, but not great character spacing
	ty += row_height2;
	Engine.CharsDoOutput(hx, ty, 0, DV_SPACING_ROUND_ADD, DV_NULL, "DV_SPACING_ROUND_ADD and DV_SPACING_KERN_ROUND_ADD - for device dependent output, but not great character spacing");
	ty += row_height3;
	for (i = 0; i < nr_of_rows; i++, ty += row_height2)
	{
		Engine.CharsDoOutput(tx, ty, 0, DV_SPACING_ROUND_ADD, DV_NULL, lines[i]);
	}

	// DV_SPACING_ADD_ROUND - for device dependent output, but not great character spacing
	ty += row_height2;
	Engine.CharsDoOutput(hx, ty, 0, DV_SPACING_ADD_ROUND, DV_NULL, "DV_SPACING_ADD_ROUND and DV_SPACING_KERN_ADD_ROUND - for device dependent output, but not great character spacing");
	ty += row_height3;
	for (i = 0; i < nr_of_rows; i++, ty += row_height2)
	{
		Engine.CharsDoOutput(tx, ty, 0, DV_SPACING_ADD_ROUND, DV_NULL, lines[i]);
	}
}


void CDTApplet::DrawPage_16()
{
	// This example demonstrates how to draw lines of glyphs/characters using a 2D perspective transformation

	DT_UBYTE ct[DV_MAXPOINTS];
	DT_SLONG cx[DV_MAXPOINTS];
	DT_SLONG cy[DV_MAXPOINTS];
	DT_FLOAT cx_f[DV_MAXPOINTS];
	DT_FLOAT cy_f[DV_MAXPOINTS];

	DT_STYLE_EFFECTS style = {1, {0}, {160, 140, 54, 0}, DV_NULL};
	DT_ID_SWORD font_index = Engine.FontFind("F0010_DENMRK_SS0", 0);

	const DT_CHAR* lines[] =
	{
		"Saturn is the most distant of the five planets known to ancient stargazers. In 1610, Italian",
		"Galileo Galilei was the first astronomer to gaze at Saturn through a telescope. To his surprise,",
		"he saw a pair of objects on either side of the planet, which he later drew as cup handles",
		"attached to the planet on each side. In 1659, Dutch astronomer Christiaan Huygens announced",
		"that this was a ring encircling the planet. In 1675, Italian-born astronomer Jean Dominique",
		"Cassini discovered a gap between what are now called the A and B rings."
	};

	DT_SLONG nr_of_rows = 6, i, j, n;
	DT_SLONG font_size = 80; /* font size in pixels */
	DT_SLONG dx = 0, dy = 0; /* character's position in font units */

	DT_ID_ULONG char_code;
	DT_UWORD font_em_h, font_em_v, char_width;
	DT_FLOAT kh, kv;

	DT_TM3X3 tm;

	// Get font information
	Engine.FontGetMetrics(font_index, &font_em_h, &font_em_v);
	kh = font_size; kh /= font_em_h;
	kv = font_size; kv /= font_em_v;

	// Set the 2D perspective transformation matrix
	tm[0][0] = kh * Zoom;
	tm[0][1] = 0.0;
	tm[0][2] = 100.0 * Zoom;
	tm[1][0] = 0.0;
	tm[1][1] = -kv * Zoom;
	tm[1][2] = 100.0 * Zoom;
	tm[2][0] = 0.0001;
	tm[2][1] = 0.0001;
	tm[2][2] = 1.0;

	Engine.OutputSetStyleEffects(style);

	// Draw lines
	for (i = 0; i < nr_of_rows; i++)
	{
		// Draw characters one by one
		j = 0;
		dx = 0;

		while ((char_code = lines[i][j]) != 0)
		{
			n = Engine.CharGetOutline(font_index, char_code, DV_MAXPOINTS, ct, cx, cy, 0);
			if (n > 0 && n <= DV_MAXPOINTS)
			{
				Engine.TransformPerspective(tm, dx, dy, n, cx, cy, cx_f, cy_f);
				Engine.Shape_PolyFilled(-X, -Y, ct, cx_f, cy_f, n);
			}

			// Increase horizontal distance by character's width
			Engine.CharGetMetrics(font_index, char_code, &char_width);
			dx += char_width;

			j++;
		}

		// Move down by font's height
		dy -= font_em_v;
	}
}


void CDTApplet::DrawPage_17()
{
	// This example demonstrates how to draw lines of glyphs/characters using a 2D perspective transformation

	DT_UBYTE ct[DV_MAXPOINTS];
	DT_SLONG cx[DV_MAXPOINTS];
	DT_SLONG cy[DV_MAXPOINTS];
	DT_FLOAT cx_f[DV_MAXPOINTS];
	DT_FLOAT cy_f[DV_MAXPOINTS];

	DT_STYLE_EFFECTS style = {1, {0}, {120, 50, 230, 0}, DV_NULL};
	DT_ID_SWORD font_index = Engine.FontFind("F0011_CMCONC_SS0", 0);

	const DT_CHAR* lines[] =
	{
		"Saturn is the most distant",
		"of the five planets known to",
		"ancient stargazers. In 1610,",
		"Italian Galileo Galilei was the",
		"first astronomer to gaze at",
		"Saturn through a telescope."
	};

	DT_SLONG nr_of_rows = 6, i, j, n;
	DT_SLONG font_size = 30; /* font size in pixels */
	DT_SLONG dx = 0, dy = 0; /* character's position in font units */
	DT_SLONG box_width = 12000; /* width of the box in font units */
	DT_SLONG line_width, spacing; /* width of the line and spacing in font units */

	DT_ID_ULONG char_code;
	DT_UWORD font_em_h, font_em_v, char_width;
	DT_FLOAT kh, kv;

	DT_TM3X3 tm;

	// Get font information
	Engine.FontGetMetrics(font_index, &font_em_h, &font_em_v);
	kh = font_size; kh /= font_em_h;
	kv = font_size; kv /= font_em_v;

	// Set the 2D perspective transformation matrix
	tm[0][0] = kh * Zoom;
	tm[0][1] = 500.0 * 0.00012 * Zoom;
	tm[0][2] = 500.0 * Zoom;
	tm[1][0] = 0.0;
	tm[1][1] = -kv * Zoom;
	tm[1][2] = 30.0 * Zoom;
	tm[2][0] = 0.00000;
	tm[2][1] = 0.00012;
	tm[2][2] = 1.0;

	Engine.OutputSetStyleEffects(style);

	// Draw lines
	for (i = 0; i < nr_of_rows; i++)
	{
		// Calculate the total width of the current line
		j = 0;
		line_width = 0;

		while ((char_code = lines[i][j]) != 0)
		{
			Engine.CharGetMetrics(font_index, char_code, &char_width);
			line_width += char_width;
			j++;
		}

		spacing = (box_width - line_width) / j;

		// Draw characters one by one
		j = 0;
		dx = -box_width / 2;

		while ((char_code = lines[i][j]) != 0)
		{
			n = Engine.CharGetOutline(font_index, char_code, DV_MAXPOINTS, ct, cx, cy, 0);
			if (n > 0 && n <= DV_MAXPOINTS)
			{
				Engine.TransformPerspective(tm, dx, dy, n, cx, cy, cx_f, cy_f);
				Engine.Shape_PolyFilled(-X, -Y, ct, cx_f, cy_f, n);
			}

			// Increase horizontal distance by character's width
			Engine.CharGetMetrics(font_index, char_code, &char_width);
			dx += char_width;
			// Add extra spacing so that line is justified
			dx += spacing;

			j++;
		}

		// Move down by font's height
		dy -= font_em_v;
	}
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
		if (Page < 17) Page++; else return;
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
