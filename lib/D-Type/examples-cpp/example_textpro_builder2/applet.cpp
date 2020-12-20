/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <fstream>

// D-Type header file (level 4)
#include "dtbuilder/dtype_l4.h"
// D-Type's platform independent window support
#include "dtwindow.h"
// D-Type's extra system utility functions
#include "dtxsystem.h"

// D-Type CPP Engine
#include "dtengine.cpp"
#include "dtbuilder/dtbuilder.cpp"

// Autoformat
#include "classes/autoformat.cpp"


#define DV_SURFACE_BPP 4
#define DV_SURFACE_FORMAT (DV_SURFACE_BPP * 8)

#define DV_MAX_FRAMES 5000


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

	void AddRect(DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h);

	bool InitDoc();

	bool DrawMain();
	bool DrawFrames(bool debug = false);
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

	CDTAutoformatDefault AutoformatDefault;
	CDTAutoformatSecondary AutoformatSecondary;
	CDTAutoformatMath AutoformatMath;

	CDTAutoformatDefault& AutoFormat(const DT_CHAR* class_id = "")
	{
		     if (strcmp(class_id, "secondary") == 0) return AutoformatSecondary;
		else if (strcmp(class_id, "math") == 0) return AutoformatMath;

		return AutoformatDefault;
	}
};


bool CDTApplet::Init()
{
	Reset();

	MemorySurface.m = DV_NULL; MemorySurface.l = 0; MemorySurface.w = MemorySurface.h = 0;

	DT_STREAM_FILE(sa, "../../files/raw/base_180x165_8bpp.raw");  LoadBitmap(BitmapA, 180, 165, sa);
	DT_STREAM_FILE(sb, "../../files/raw/menu2_180x165_8bpp.raw"); LoadBitmap(BitmapB, 180, 165, sb);

	return InitDoc();
}


void CDTApplet::Exit()
{
	dtMemFree(MemorySurface.m);
	dtMemFree(BitmapA.m);
	dtMemFree(BitmapB.m);
}


void CDTApplet::AddRect(DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h)
{
	CDTObj o00(Doc, "Rect Outline Color");
	CDTObj o01(Doc, "Rect Outline");
	CDTObj o02(Doc, "Rect");

	o00 + CDTPropStr(pdRGBT, "5A31A588");
	o01 + CDTLink(pdStylePtr, o00) + CDTPropInt(pdLineThickness, 500) + CDTPropInt(pdLinePerimeterGrow, 2000);
	o02 + CDTLink(pdOutlinePtr, o01) + CDTPropInt(pdDw, w) + CDTPropInt(pdDh, h);
	o02.Pin(page, x, y, pdDrawRect);
}


bool CDTApplet::InitDoc()
{
	if (!Doc.Init(Engine)) return false;

	/* formatter */
#if (_MSC_VER >= 1910)
	auto& f = AutoFormat();
#else
	CDTAutoformatDefault& f = AutoFormat();
#endif

	/* builders */
	CDTBuilderCtrl c(Doc);
	CDTBuilderText t(Doc);
	CDTBuilderMath_Autoformat m(Doc, f);

	/* text flow on one pages */
	t.Text(t().X(400).Y(400).W(4750).H(6864).Page(0).PageCount(1).RowType(DE_ROW_STANDARD).VAlign(DE_VALIGN_MIDDLE));

	/* text */

	t + c.Base() + "Hello, here is my formula:"

	+ c.Br()
	+ f.Center + m.Formula_Pi("Formula 1") + f._Center

	+ c.Br()
	+ c.Base() + "And here is my formula once again:"

	+ c.Br()
	+ f.Center + m.Formula_Pi("Formula 2") + f._Center

	+ c.Br()
	+ c.Base() + "Mathematical notation includes " + f.Bold + "letters from various alphabets" + f._Bold + ", as well as " + f.Underline + "special mathematical and scientific symbols" + f._Underline + ". Letters in various fonts often have specific, fixed meanings in particular areas of mathematics. In mathematics, a formula is an entity constructed using the symbols and formation rules of a given logical language. A mathematical formula such as " + m.Root(3, "x + y") + " contains two variables, x and y. Other formulas may be created to solve a particular problem: for example, using the equation of a sine curve to model the movement of the tides in a bay: sin" + m.Brack(m.Frac("a - b", "x + y")) + ". In all cases, however, formulas form the basis for calculations. Expressions are distinct from formulas in that they cannot contain an equals sign. When comparing formulas to grammatical sentences, expressions are more like phrases."

	+ c.Br()
	+ c.Base() + "In a general context, mathematical formulas are applied to provide a mathematical solution for real world problems. Some formulas may be rather complex to typeset, for example:"

	+ c.Br()
	+ f.Center + c.Base() + m.Frac(m.Root(3, "u + v"), t.Text() + m.Pi + m.Sum + m.Integral("a", "b", "cos(x) dx") + m.Diff + m.Root(4, m.Frac("a - b", "x + y"))) + f._Center

	+ c.Br()
	+ c.Base() + "The vast majority of computations with measurements are done in computer programs with no facility for retaining a symbolic computation of the units. Only the numerical quantity is used in the computation. This requires that the universal formula be converted to a formula that is intended to be used only with prescribed units, meaning the numerical quantity is implicitly assumed to be multiplying a particular unit. The requirements about the prescribed units must be given to users of the input and the output of the formula.";

	t + c.Br()
	+ c.Base() + "This line consists of text in the following languages: English, Русский, 日本語, العربية, Ελληνικά, हिन्दी, ไทย.";

	t + c.Br()
	+ c.Base() + "Hello, here is some Arabic/Persian شوایس ۴۷ and here is some Hindi हिन्दी";

	t + c.Br()
	+ c.Base() + "Here's a sample of some text written in Sanskrit: श्रीमद् भगवद्गीता अध्याय अर्जुन विषाद योग धृतराष्ट्र उवाच। धर्मक्षेत्रे कुरुक्षेत्रे समवेता युयुत्सवः मामकाः पाण्डवाश्चैव किमकुर्वत संजय." + f.END;

	/* additional full text format: set font size to 120 doc units and align left */
	t.Format(TX_ATTR_FONT_WIDTH, 110).Format(TX_ATTR_FONT_HEIGHT, 110).Format(TX_ATTR_ALIGN, (DT_SLONG)0);

	/* Make virtual copy of object t on page 1 */

	CDTBuilderBase b(Doc);
	b.VCopy(b().Page(1).X(400).Y(400), t);

	/* Add text extent rectangle on page 0 */
	AddRect(0, 400, 400, 4750, 6864);

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
	Doc.Draw(Page, X, Y, DV_SURFACE_FORMAT, 0, &MemorySurface, DV_NULL);

	if (Page == 1) DrawFrames();

	DrawBitmap(BitmapA, w - BitmapA.w, h - BitmapA.h, 0);
	DrawBitmap(BitmapB, w - BitmapB.w, h - BitmapB.h, 255);

	return true;
}


bool CDTApplet::DrawFrames(bool debug)
{
	DT_SWORD is_first_frame = 1;
	DT_SLONG i, j, frames_count = 0;
	DT_PD_FRAME_FORMAT1 frames_helper_array[DV_MAX_FRAMES]; /* A simple static array of PowerDoc Frames */
	/* A static array is convenient for this test application, however in a real application we wouldn't use it. */

	DT_RECT_SLONG extent = {0, 0, 0, 0}; /* This will be the global enclosing rectangle */
	DT_PD_DOCDRAW_PARAMS params; /* Extended parameters for pdDocDraw */
	DT_PD_FRAME_FORMAT1 frame; /* A single PowerDoc frame in DT_PD_FRAME_FORMAT1 format */

	/* Extended parameters for rendering */

	/* Tell the engine to perform text layout operations for any and all text objects and characters on the page, even if they are completely outside the visible portion of the page. */
	params.Flags = 1;

	/* Tell the engine that we want to retrieve text flows and frames resulting from its text layout operations (via params.FlowsArr.Len and params.FlowsArr.Ptr) and we want to retrieve them all in DT_PD_FRAME_FORMAT1 format. */
	params.Flows = 1;

	/* all other parameters are set to their default values */
	params.Origins = 0;
	params.Boundaries = 0;
	params.Background = 0;
	params.Reserved = 0;
	params.R = 255;
	params.G = 255;
	params.B = 255;
	params.A = 255;
	params.FlowsArr.Len = 0;
	params.FlowsArr.Ptr = DV_NULL;
	params.UserData = DV_NULL;

	/* We call pdDocDraw to perform Step 2 described above. */
	if (Doc.Draw(Page, X, Y, 0, 0, DV_NULL, &params) != 1) return false;

	/* (pointer to MemorySurface can be set to DV_NULL only when params.Flags=1) */

	std::ofstream debug_file; if (debug) debug_file.open("frames_debug.txt"); /* We will print info about frames to this output file */

	/* There should be 1 text flow returned. Let's print this. */
	debug_file << "There are " << params.FlowsArr.Len << " text flow(s) on this page.\n";

	/* Now let's iterate through all the text flows (again, there will be only one in our case). */
	for (i = 0; i < params.FlowsArr.Len; i++)
	{
		debug_file << "--- Beginning of text flow " << i << " ---\n";
		debug_file << "Text flow " << i << " corresponds to PowerDoc image " << params.FlowsArr.Ptr[i].Img << "\n";
		debug_file << "There are " << params.FlowsArr.Ptr[i].FramesLen << " frame(s) in this text flow\n";

		/* Now let's iterate through all the frames in each text flow. */

		for (j = 0; j < params.FlowsArr.Ptr[i].FramesLen; j++)
		{
			frame = params.FlowsArr.Ptr[i].Frames.Format1Ptr[j];

			debug_file << "  frame #" << j << " is for character #" << frame.Idx << ". Its bounding box is XMin=" << frame.XMin << ", YMin=" << frame.YMin << ", XMax=" << frame.XMax << ", YMax=" << frame.YMax << "\n";

			if (frame.Idx < 0)
			{
				/* ignore empty (not valid) frames */
				/* There are usually some empty frames at the end of the frames array because PowerDoc increases
				   the size of this array in chunks of 50 or so (to make memory reallocations more efficient). */
				/* Any invalid frames have frame.Idx set to a negative value */

				debug_file << "  --> This is an empty (not valid) frame, so we are skipping it...\n";
			}
			else if (is_first_frame)
			{
				/* this is the first valid frame - initialize extent (enclosing rectangle) */
				extent.xmn = frame.XMin;
				extent.ymn = frame.YMin;
				extent.xmx = frame.XMax;
				extent.ymx = frame.YMax;
				is_first_frame = 0;
			}
			else
			{
				/* these are subsequent valid frames - recalculate extent (enclosing rectangle) */
				extent.xmn = DF_MIN(extent.xmn, frame.XMin);
				extent.ymn = DF_MIN(extent.ymn, frame.YMin);
				extent.xmx = DF_MAX(extent.xmx, frame.XMax);
				extent.ymx = DF_MAX(extent.ymx, frame.YMax);
			}

			if (frame.Idx >= 0 && frames_count < DV_MAX_FRAMES)
			{
				/* store the frame in our helper array so we can draw it later */
				frames_helper_array[frames_count] = frame;
				frames_count++;
			}
		}

		/* now we can free all frames in this text flow */
		pdFree(params.FlowsArr.Ptr[i].Frames.Format1Ptr);

		debug_file << "--- End of text flow " << i << " ---\n";
	}

	/* now we can free all text flows */
	if (params.FlowsArr.Ptr != DV_NULL) pdFree(params.FlowsArr.Ptr);

	/* That's it. Now we know the size of the enclosing rectangle. */
	debug_file << "The enclosing rectangle is: XMIN=" << extent.xmn << ", YMIN=" << extent.ymn << ", XMAX=" << extent.xmx << ", YMAX=" << extent.ymx << "\n";

	debug_file.close();

	/* As the last step, draw the enclosing rectangle. */
	/* This is only a test to confirm that all the characters are really inside the enclosing rectangle. */

	/* Redirect all D-Type output to a memory surface (MDC) */
	Engine.OutputSetMDC(DV_SURFACE_FORMAT, 0, MemorySurface, 0, 0, MemorySurface.w, MemorySurface.h < 0 ? -MemorySurface.h : MemorySurface.h);

	Engine.Shape_Rectangle(0, 0, extent.xmn - X, extent.ymn - Y, extent.xmx - extent.xmn, extent.ymx - extent.ymn, 0, 1);

	/* Also draw the frames themselves */
	for (i = 0; i < frames_count; i++)
	{
		DT_SLONG frame_x = frames_helper_array[i].XMin;
		DT_SLONG frame_y = frames_helper_array[i].YMin;
		DT_SLONG frame_w = frames_helper_array[i].XMax - frames_helper_array[i].XMin;
		DT_SLONG frame_h = frames_helper_array[i].YMax - frames_helper_array[i].YMin;

		Engine.Shape_Rectangle(0, 0, frame_x - X, frame_y - Y, frame_w, frame_h, 0, 0.15);
	}

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
		if (Page < 1) Page++; else return;
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
