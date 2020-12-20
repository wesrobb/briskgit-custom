/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


/*
	This example shows how to retrieve and process Text Flows and Frames that D-Type Engine
	generates during its text layout operations.

	The procedure consists of the following steps:

	Step 1. Create a simple Text Line object

	Step 2. Render this text line to a null-surface using the CDTDocV8::Draw function. The null-surface
	is not a real memory surface but an imaginary surface. When this surface is specified and the
	DT_PD_DOCDRAW_PARAMS structure initialized appropriately, the CDTDocV8::Draw function will perform
	text layout operations and process all the characters on the page.

	Step 3. Information about these processed characters is returned back to the application.
	For each text object, there will be one text flow. Because we only have one text line in this
	example, there will only be one text flow. This text flow will contain an array of frames.
	Each frame corresponds to one character in the text line. We request that frames be returned
	in DT_PD_FRAME_FORMAT1 format so that:

	  frame.Idx will give us the index of the character in the text line
	  frame.XMin, frame.YMin, frame.XMax and frame.YMax will give us the bounding box of the character

	Step 4. The application then uses the above information to calculate the global minimum and
	maximum (x, y) coordinates of the rectangle that will fully enclose all frames.

	Step 5. The application calls CDTDocV8::Draw once again, but this time we render to a real
	24-bpp RGB surface.

	As we process frames one by one in step 3, we print information about each frame to an output
	file called frames_debug.txt.

	After step 5, we also draw the global enclosing rectangle and the frames themselves.
	This helps visualize the process and confirm that our calculations were correct.

	Note: For simplicity, this example assumes that all returned Frames are rectangles (i.e.
	it ignores frame.XMid and frame.YMid). However, this can be easily enhanced to handle Frames
	that are parallelograms.
*/ 


#include <fstream>

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

#define DV_MAX_FRAMES 100


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

	/* PART 1: Defining objects and their properties */

	/* 3 PowerDoc Object(s) */
	CDTObj o0(Doc, "Body Color");
	CDTObj o1(Doc, "Typography");
	CDTObj o2(Doc, "Text Line");

	/* Properties for object 0 */
	o0 + CDTPropStr(pdRGBT, "75330A00");

	/* Properties for object 1 */
	o1 + CDTLink(pdBodyPtr, o0) + CDTPropInt(pdFontIndex, 0) + CDTPropInt(pdFontDw, 48) + CDTPropInt(pdFontDh, 48);

	/* Properties for object 2 */
	o2 + CDTLink(pdTypographyPtr, o1) + CDTPropStr(pdTextString_08, "PowerDoc Frames - Test");

	/* PART 2: Image placement */

	/* Pin Image of object o2 on page 0 at coordinate X=100, Y=100 */
	o2.Pin(0, 100, 100, pdDrawTextLine);

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

	DrawFrames(true);

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
	DT_PD_DOCDRAW_PARAMS params; /* Extended parameters for CDTDocV8::Draw */
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

	/* We call CDTDocV8::Draw to perform Step 2 described above. */
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
				   the size of this array in chunks (to make memory reallocations more efficient). */
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
		Engine.Free(params.FlowsArr.Ptr[i].Frames.Format1Ptr);

		debug_file << "--- End of text flow " << i << " ---\n";
	}

	/* now we can free all text flows */
	if (params.FlowsArr.Ptr != DV_NULL) Engine.Free(params.FlowsArr.Ptr);

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
