/*
 *
 * Copyright (C) D-Type Solutions - All Rights Reserved
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 *
 * This file is part of D-Type Engine and may only be used, modified
 * and distributed under the terms of the applicable license agreement
 * For conditions of distribution and use, see license.txt
 *
 * ANY VIOLATIONS OF THE ABOVE LICENSE AGREEMENT WILL BE PROSECUTED TO
 * THE MAXIMUM EXTENT POSSIBLE UNDER THE LAW. NO VENDOR, DISTRIBUTOR,
 * DEALER, RETAILER, SALES PERSON OR OTHER PERSON IS AUTHORIZED TO MODIFY
 * THIS AGREEMENT OR TO MAKE ANY WARRANTY, REPRESENTATION OR PROMISE WHICH
 * IS DIFFERENT THAN, OR IN ADDITION TO, THIS AGREEMENT.
 *
 */


#include <stdio.h>
#include <stdlib.h>
//#include <memory.h>
#include <sys/time.h>

#include "DTWindowPlatform.h"

#define XK_LATIN1
#define XK_MISCELLANY

#include <X11/Xutil.h>
#include <X11/keysymdef.h>

#include "DTWindowPlatformPrivate.h"

#include "DTWindowPlatformConvertFormat.cpp"
#include "DTWindowPlatformConvertUnicode.cpp"


#ifdef DV_ENDIANLOC
	#define DV_WINDOW_ENDIANLOC DV_ENDIANLOC
#else
	#define DV_WINDOW_ENDIANLOC 0
	#pragma message("--- Endianness not detected - will use 0 (LITTLE ENDIAN) --- ");
#endif


#define DF_TV_ADD(a, b) (a)->tv_sec += (b)->tv_sec; (a)->tv_usec += (b)->tv_usec; if ((a)->tv_usec >= 1000000) { (a)->tv_sec++; (a)->tv_usec -= 1000000; }
#define DF_TV_SUB(a, b) (a)->tv_sec -= (b)->tv_sec; (a)->tv_usec -= (b)->tv_usec; if ((a)->tv_usec < 0)        { (a)->tv_sec--; (a)->tv_usec += 1000000; }

const DT_SLONG WINDOW_START_X = 50;
const DT_SLONG WINDOW_START_Y = 50;
const DT_SLONG WINDOW_OFFSET_Y = 25;
const DT_SLONG WINDOW_OFFSET_X = 30;

DT_SLONG WINDOW_X = WINDOW_START_X + WINDOW_OFFSET_X;
DT_SLONG WINDOW_Y = WINDOW_START_Y + WINDOW_OFFSET_Y;


CDTFormat FindFormat(DT_SLONG bitsPerPixel, DT_ULONG redMask, DT_ULONG greenMask, DT_ULONG blueMask)
{
	switch (bitsPerPixel)
	{
	case 16:
		if (redMask == 0x7c00 && greenMask == 0x03e0 && blueMask == 0x001f) return CDTFormat::XRGB1555;
		if (redMask == 0x001f && greenMask == 0x03e0 && blueMask == 0x7c00) return CDTFormat::XBGR1555;
		if (redMask == 0xf800 && greenMask == 0x07e0 && blueMask == 0x001f) return CDTFormat::RGB565;
		if (redMask == 0x001f && greenMask == 0x07e0 && blueMask == 0xf800) return CDTFormat::BGR565;
		break;

	case 24:
		if (redMask == 0xff0000 && greenMask == 0x00ff00 && blueMask == 0x0000ff) return CDTFormat::RGB888;
		if (redMask == 0x0000ff && greenMask == 0x00ff00 && blueMask == 0xff0000) return CDTFormat::BGR888;
		break;

	case 32:
		if (redMask == 0xff0000 && greenMask == 0x00ff00 && blueMask == 0x0000ff) return CDTFormat::XRGB8888;
		if (redMask == 0x0000ff && greenMask == 0x00ff00 && blueMask == 0xff0000) return CDTFormat::XBGR8888;
		break;
	}

	return CDTFormat::Unknown;
}


void* ResetBuffer(void* buffer, DT_SLONG size = 0)
{
	if (buffer != DV_NULL) dtMemFree(buffer);
	buffer = (size <= 0 ? DV_NULL : dtMemAlloc(64 + size));
	return buffer;
}


CDTDisplayPlatform::CDTDisplayPlatform(DT_ULONG /*flags*/)
{
	X11Display = ::XOpenDisplay(0);
	DispConnNumber = 0; if (X11Display != DV_NULL) DispConnNumber = ConnectionNumber(X11Display);

	WindowsArr = DV_NULL;
	WindowsCount = 0;
	WindowsLimit = 0;

	TimeVal.tv_usec = 40000; /* 0.04 seconds */
	TimeVal.tv_sec = 0;
	TimeVal_Bak = TimeVal;
}


CDTDisplayPlatform::~CDTDisplayPlatform()
{
	if (X11Display != DV_NULL) ::XCloseDisplay(X11Display);
	if (WindowsArr != DV_NULL) dtMemFree(WindowsArr);
}


bool CDTDisplayPlatform::WindowRegsiter(CDTWindowBase* window)
{
	if (window == DV_NULL) return false;

	DT_SLONG i;

	for (i = 0; i < WindowsCount; i++)
	{
		if (WindowsArr[i] == window) return true; /* already registered */
	}

	if (WindowsCount >= DV_WINDOW_WINDOW_MAX) return false; /* we have too many registered windows */

	if (WindowsCount >= WindowsLimit) /* reallocate */
	{
		DT_SLONG new_limit = WindowsLimit + DV_WINDOW_WINDOW_INC;
		CDTWindowBase** new_windows = (CDTWindowBase**)dtMemRealloc(WindowsArr, new_limit * sizeof(CDTWindowBase*));
		if (new_windows == DV_NULL) return false;
		WindowsArr = new_windows;
		WindowsLimit = new_limit;
	}

	/* new registration */
	WindowsArr[WindowsCount] = window;
	WindowsCount++;

	return true;
}


void CDTDisplayPlatform::WindowUnregsiter(const CDTWindowBase* window)
{
	if (window == DV_NULL || WindowsCount <= 0) return;

	DT_SLONG i, j = 0, c = WindowsCount;

	for (i = 0; i < c; i++)
	{
		if (j != i) WindowsArr[j] = WindowsArr[i];
		if (WindowsArr[i] == window) WindowsCount--; /* window to be unregsitered */ else j++; /* window to be kept */
	}
}


/*
void CDTDisplayPlatform::EventLoop()
{
	if (!X11Display) return;
	if (WindowsCount <= 0) return;

	::XEvent event;
	::XNextEvent(X11Display, &event); // blocks

	DT_SLONG i;

	for (i = 0; i < WindowsCount; i++)
	{
		if (WindowsArr[i] == DV_NULL) continue;
		if (WindowsArr[i]->getWindow() != event.xany.window) continue;

		//printf("handle event for window %ld\n", event.xany.window);
		WindowsArr[i]->handleEvent(event);
		break;
	}
}
*/


void CDTDisplayPlatform::EventLoop()
{
	if (X11Display == DV_NULL) return;
	if (WindowsCount <= 0) return;

	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(DispConnNumber, &rfds);

	//printf("tv_sec=%ld, tv_usec=%ld\n", TimeVal.tv_sec, TimeVal.tv_usec);

	timeval tv_before; gettimeofday(&tv_before, 0);

	DT_SLONG i;

	// wait an event or timer
	if (select(DispConnNumber + 1, &rfds, 0, 0, &TimeVal) == 0)
	{
		//printf("Timer Fired!\n"); // handle timer here

		::XEvent bogus;
		bogus.xany.display = DV_NULL;

		for (i = 0; i < WindowsCount; i++) WindowsArr[i]->handleEvent(bogus);

		//TimeVal.tv_usec = 40000; /* 0.04 seconds */
		//TimeVal.tv_sec = 0;
		TimeVal = TimeVal_Bak;

		//return;
	}
	else
	{
		timeval tv_after; gettimeofday(&tv_after, 0);
		DF_TV_SUB(&TimeVal, &tv_after);
		DF_TV_ADD(&TimeVal, &tv_before);
		if (TimeVal.tv_sec < 0) { TimeVal.tv_sec = 0; TimeVal.tv_usec = 0; }
	}

	while (::XPending(X11Display))
	{
		::XEvent event;
		::XNextEvent(X11Display, &event);

		for (i = 0; i < WindowsCount; i++)
		{
			if (WindowsArr[i]->getWindow() != event.xany.window) continue;

			//printf("handle event for window %ld\n", event.xany.window);
			WindowsArr[i]->handleEvent(event);
			break;
		}
	}
}


DT_FLOAT CDTDisplayPlatform::GetScale() const
{
	return 1.0;
}


DT_SLONG CDTDisplayPlatform::Dist(DT_SLONG d) const
{
	return d;
}


CDTWindowPlatform::CDTWindowPlatform(CDTDisplayPlatform* display)
{
	DstBuffer = DV_NULL;
	defaults();
	Disp = display;
}


CDTWindowPlatform::~CDTWindowPlatform()
{
	close();
}


void CDTWindowPlatform::defaults()
{
	X11GC = 0;
	X11Window = 0;
	X11Image = 0;

	ColorConverter = DV_NULL;

	SrcBuffer = DV_NULL;
	DstBuffer = ResetBuffer(DstBuffer);
	CachedBuffer = DV_NULL;

	IsShuttingDown = false;

	IsOpen = false;
	Width = 0;
	Height = 0;
}


bool CDTWindowPlatform::open(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG width, DT_SLONG height, DT_SLONG flags)
{
	if (IsOpen) return 0; /* we can't open twice! */

	defaults();

	if (!Disp) return false;
	if (!Disp->X11Display) return false;

	const DT_SLONG screen = DefaultScreen(Disp->X11Display);
	Visual* visual = DefaultVisual(Disp->X11Display, screen);
	if (!visual) return false;

	// It gets messy when talking about color depths.
	//
	// For the image buffer, we either need 8, 16 or 32 bitsPerPixel. 8 bits we'll
	// never have (hopefully), 16 bits will be used for displayDepth 15 & 16, and
	// 32 bits must be used for depths 24 and 32.
	//
	// The converters will get this right when talking about displayDepth 15 & 16, but
	// it will wrongly assume that displayDepth 24 takes _exactly_ 24 bitsPerPixel. We
	// solve that by tricking the converter requester by presenting it a 32 bit
	// bufferDepth instead.

	const DT_SLONG displayDepth = DefaultDepth(Disp->X11Display, screen);
	const DT_SLONG bufferDepth = displayDepth == 24 ? 32 : displayDepth;
	const DT_SLONG bytesPerPixel = (bufferDepth + 7) / 8;
	const DT_SLONG bitsPerPixel = 8 * bytesPerPixel;

	if (bitsPerPixel != 16 && bitsPerPixel != 32) return false;

	CDTFormat destFormat_ = FindFormat(bufferDepth, visual->red_mask, visual->green_mask, visual->blue_mask);

	if (destFormat_ == CDTFormat::XRGB8888) ColorConverter = DV_NULL;
	else if ((ColorConverter = RequestConverter(CDTFormat::XRGB8888, destFormat_)) == DV_NULL) return false;

	// let's create a window

	DT_SLONG eventMask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask;
	//if (flags & DV_WINDOW_HINT_RESIZE) eventMask = eventMask | StructureNotifyMask;

	const Window root = DefaultRootWindow(Disp->X11Display);
	const DT_SLONG screenWidth = DisplayWidth(Disp->X11Display, screen);
	const DT_SLONG screenHeight = DisplayHeight(Disp->X11Display, screen);

	if (flags & DV_WINDOW_HINT_FULLSCREEN) { width = screenWidth; height = screenHeight; }

	// clamp cascading windows to screen size
	if (WINDOW_X + width > screenWidth) WINDOW_X = WINDOW_START_X;
	if (WINDOW_Y + height > screenHeight) WINDOW_Y = WINDOW_START_Y;

	DT_SLONG x0 = WINDOW_X;
	DT_SLONG y0 = WINDOW_Y;

	if (flags & DV_WINDOW_HINT_CENTER)
	{
		x0 = (screenWidth - width) / 2;
		y0 = (screenHeight - height) / 2;
	}
	else if (flags & DV_WINDOW_HINT_FULLSCREEN)
	{
		x0 = 0;
		y0 = 0;
	}
	else
	{
		WINDOW_X += WINDOW_OFFSET_X;
		WINDOW_Y += WINDOW_OFFSET_Y;
	}

	if (x >= 0) x0 = x;
	if (y >= 0) y0 = y;

	::XSetWindowAttributes attributes;
	attributes.border_pixel = attributes.background_pixel = WhitePixel(Disp->X11Display, screen);
	//attributes.border_pixel = attributes.background_pixel = BlackPixel(Disp->X11Display, screen);
	attributes.backing_store = NotUseful;
	//attributes.backing_store = Always;

	X11Window = ::XCreateWindow(Disp->X11Display, root, x0, y0, width, height, 0, displayDepth, InputOutput, visual, /*CWBackPixel |*/ CWBorderPixel | CWBackingStore, &attributes);
	if (!X11Window) goto error;

	if (flags & DV_WINDOW_HINT_NOFRAME) {} else ::XStoreName(Disp->X11Display, X11Window, title);

	WMProtocols = ::XInternAtom(Disp->X11Display, "WM_PROTOCOLS", True);
	WMDeleteWindow = ::XInternAtom(Disp->X11Display, "WM_DELETE_WINDOW", True);
	if (WMProtocols == 0 || WMDeleteWindow == 0) goto error;

	if (::XSetWMProtocols(Disp->X11Display, X11Window, &WMDeleteWindow, 1) == 0) goto error;

	::XSizeHints sizeHints;

	sizeHints.x = sizeHints.y = 0;
	sizeHints.flags = PPosition | PMinSize | PMaxSize;
	sizeHints.min_width = sizeHints.max_width = width;
	sizeHints.min_height = sizeHints.max_height = height;

	if (flags & DV_WINDOW_HINT_RESIZE)
	{
		sizeHints.flags = PPosition | PMinSize | PMaxSize;
		sizeHints.min_width = sizeHints.min_height = 32;
		sizeHints.max_width = sizeHints.max_height = 4096;
	}

	::XSetNormalHints(Disp->X11Display, X11Window, &sizeHints);
	::XSelectInput(Disp->X11Display, X11Window, eventMask);
	::XClearWindow(Disp->X11Display, X11Window);

	// Generate full screen event if required.
	if ((flags & DV_WINDOW_HINT_FULLSCREEN) && (flags & DV_WINDOW_HINT_NOFRAME))
	{
		// Make the window viewable and flush the output buffer.
		::XMapWindow(Disp->X11Display, X11Window);
		//::XFlush(Disp->X11Display);

		Window sRootWindow = RootWindow(Disp->X11Display, screen);
		Atom x11_state_atom = ::XInternAtom(Disp->X11Display, "_NET_WM_STATE", False );
		Atom x11_fs_atom = ::XInternAtom(Disp->X11Display, "_NET_WM_STATE_FULLSCREEN", False );

		::XEvent x11_event;
		x11_event.xclient.type = ClientMessage;
		x11_event.xclient.serial = 0;
		x11_event.xclient.send_event = True;
		x11_event.xclient.window = X11Window;
		x11_event.xclient.message_type = x11_state_atom;
		x11_event.xclient.format = 32;
		x11_event.xclient.data.l[0] = 1;
		x11_event.xclient.data.l[1] = x11_fs_atom;
		x11_event.xclient.data.l[2] = 0;

		::XSendEvent(Disp->X11Display, sRootWindow, False, SubstructureRedirectMask | SubstructureNotifyMask, &x11_event);
	}

	// create (image) buffer

	//DstBuffer.reset(width * height * bytesPerPixel);
	//if (DstBuffer.isEmpty()) { close(); return false; }

	if (ColorConverter) DstBuffer = ResetBuffer(DstBuffer, width * height * bytesPerPixel);

	X11GC = DefaultGC(Disp->X11Display, screen);
	X11Image = ::XCreateImage(Disp->X11Display, CopyFromParent, displayDepth, ZPixmap, 0, 0, width, height, bitsPerPixel, width * bytesPerPixel);
	if (!X11Image) goto error;

#if (DV_WINDOW_ENDIANLOC == 1)
	X11Image->byte_order = MSBFirst;
#else
	X11Image->byte_order = LSBFirst;
#endif

	// we have a winner!
	if (!Disp->WindowRegsiter(this)) goto error;

	if (flags & DV_WINDOW_HINT_FULLSCREEN)
	{
		// Find out what size window we actually got.
		::XWindowAttributes xWindowAttributes;
		::XGetWindowAttributes(Disp->X11Display, X11Window, &xWindowAttributes);
		width = xWindowAttributes.width;
		height = xWindowAttributes.height;
	}

	Width = width;
	Height = height;
	IsOpen = true;

	::XMapRaised(Disp->X11Display, X11Window);
	::XFlush(Disp->X11Display);

	return true;

error:

	close();
	return false;
}


void CDTWindowPlatform::close()
{
	IsShuttingDown = true;

	if (X11Image)
	{
		XDestroyImage(X11Image);
	}

	if (Disp)
	{
		if (Disp->X11Display && X11Window)
		{
			::XUnmapWindow(Disp->X11Display, X11Window);
			::XDestroyWindow(Disp->X11Display, X11Window);
		}

		if (Disp->X11Display) ::XFlush(Disp->X11Display);

		Disp->WindowUnregsiter(this);
	}

	defaults();
}


bool CDTWindowPlatform::update(DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG event_flag)
{
	CachedBuffer = DV_NULL;

	if (IsShuttingDown)
	{
		close();
		return false;
	}

	if (SrcBuffer == DV_NULL) return false;
	if (!Disp) return false;
	if (!Disp->X11Display || !X11Window || !X11Image) return false;

	if (ColorConverter)
	{
		if (DstBuffer == DV_NULL) return false; /* can't convert */
		ColorConverter->convert(SrcBuffer, DstBuffer, Width * Height);
		CachedBuffer = DstBuffer;
	}
	else CachedBuffer = SrcBuffer; /* no need to convert */

	X11Image->data = (DT_CHAR*)CachedBuffer;

	::XPutImage(Disp->X11Display, X11Window, X11GC, X11Image, x, y, x, y, w, h);

	::XFlush(Disp->X11Display);
	//::XSync(Disp->X11Display, true);

	X11Image->data = DV_NULL;

	if (!(event_flag & 1)) return true;

	//printf("receive messages between redraws\n");

	::XEvent event;
	if (::XCheckWindowEvent(Disp->X11Display, X11Window, -1, &event)) handleEvent(event);

	return true;
}


bool CDTWindowPlatform::update(void* pixels, DT_ULONG event_flag)
{
	SrcBuffer = pixels;
	return update(0, 0, Width, Height, event_flag);
}


void CDTWindowPlatform::modify_timer(DT_SLONG value)
{
	if (!Disp) return;
	Disp->TimeVal.tv_sec = value / 1000000;
	Disp->TimeVal.tv_usec = value - (1000000 * Disp->TimeVal.tv_sec);
	Disp->TimeVal_Bak = Disp->TimeVal;
}


DT_SLONG CDTWindowPlatform::modify(DT_SLONG flags)
{
	if (!IsOpen) return 0;
	if (!Disp) return 0;
	if (flags == 0) return 1;

	else if (flags == DV_WINDOW_ATTRIB_TOP) { ::XRaiseWindow(Disp->X11Display, X11Window); return 1; } /* TOP */
	else if (flags == DV_WINDOW_ATTRIB_BOTTOM) { ::XLowerWindow(Disp->X11Display, X11Window); return 1; } /* BOTTOM */
	else if (flags == DV_WINDOW_ATTRIB_TOPMOST) { /* not implemented */ return 1; } /* TOPMOST */
	else if (flags == DV_WINDOW_ATTRIB_DISABLE || flags == DV_WINDOW_ATTRIB_ENABLE) /* Disable */ /* Enable */
	{
		DT_SLONG eventMask = ExposureMask | StructureNotifyMask;
		if (flags == DV_WINDOW_ATTRIB_ENABLE) eventMask |= KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask;
		::XSelectInput(Disp->X11Display, X11Window, eventMask);
		return 1;
	}

	return 0;
}


::Window CDTWindowPlatform::getWindow()
{
	return X11Window;
}


void CDTWindowPlatform::handleEvent(const ::XEvent& event)
{
	if (event.xany.display == DV_NULL) { onPeriodic(); return; }

	if (event.xany.window != X11Window) return;
	if (IsShuttingDown)
	{
		close();
		return;
	}

	if (!Disp) return;

	::XEvent temp_event;
	::XEvent ev[500] = {event};
	DT_SLONG n = 1, i, j1, j2, j3, j4, j5;

	//handleSingleEvent(event, 0); return;

	if (/*event.type == Expose ||*/ event.type == ConfigureNotify || event.type == MotionNotify || event.type == KeyPress || event.type == KeyRelease)
	{
	}
		//if (event.type == ConfigureNotify || event.type == MotionNotify) {}
	else
	{
		handleSingleEvent(event, 0);
		goto end;
	}

	/* Collect all pending events into an array */
	while (n < 500)
	{
		if (!::XCheckWindowEvent(Disp->X11Display, X11Window, -1, &temp_event)) break;
		ev[n] = temp_event;
		n++;
	}

	j1 = j2 = j3 = j4 = j5 = -1;
	for (i = 0; i < n; i++)
	{
		/* kill all MotionNotify events except the last one */
		if (ev[i].type == MotionNotify)
		{
			ev[i].type = -1;
			j1 = i;
		}
			/* kill all ConfigureNotify events except the last one */
		else if (ev[i].type == ConfigureNotify)
		{
			ev[i].type = -1;
			if (Width == ev[i].xconfigure.width && Height == ev[i].xconfigure.height)
			{
				/* totally useless ConfigureNotify event */
			}
			else j2 = i;
		}
			/* kill all Expose events except the last one */
			//else if (ev[i].type==Expose) { ev[i].type=-1; j3=i; }
			/* kill all KeyPress events except the last one */
		else if (ev[i].type == KeyPress)
		{
			ev[i].type = -1;
			j4 = i;
		}
			/* kill all KeyRelease events except the last one */
		else if (ev[i].type == KeyRelease)
		{
			ev[i].type = -1;
			j5 = i;
		}
	}

	/* resore last events */
	if (j1 != -1) ev[j1].type = MotionNotify;
	if (j2 != -1) ev[j2].type = ConfigureNotify;
	//if (j3!=-1) ev[j3].type=Expose;
	if (j4 != -1) ev[j4].type = KeyPress;
	if (j5 != -1) ev[j5].type = KeyRelease;

	/* Remove any Expose events before the last ConfigureNotify event */
	for (i = 0; i < j2; i++)
	{
		if (ev[i].type == Expose)
		{
			ev[i].type = -1;
			j3 = i;
		}
	}

	/* process remaining events */
	for (i = 0; i < n; i++)
	{
		if (ev[i].type != -1) handleSingleEvent(ev[i], 1);
	}

end:
	if (IsShuttingDown) close();
}


void CDTWindowPlatform::handleSingleEvent(const ::XEvent& event, DT_SLONG /*source*/)
{
	switch (event.type)
	{
	case ConfigureNotify:
	{
		if (!Disp->X11Display) break;
		if (event.xconfigure.window != X11Window) break;
		if (event.xconfigure.width < 1 || event.xconfigure.height < 1) break;
		if (Width == event.xconfigure.width && Height == event.xconfigure.height) break;

		Width = event.xconfigure.width;
		Height = event.xconfigure.height;

		const DT_SLONG screen = DefaultScreen(Disp->X11Display);
		const DT_SLONG displayDepth = DefaultDepth(Disp->X11Display, screen);
		const DT_SLONG bufferDepth = displayDepth == 24 ? 32 : displayDepth;
		const DT_SLONG bytesPerPixel = (bufferDepth + 7) / 8;
		const DT_SLONG bitsPerPixel = 8 * bytesPerPixel;

		CachedBuffer = DV_NULL;

		//DstBuffer.reset(Width * Height * bytesPerPixel);
		if (ColorConverter) DstBuffer = ResetBuffer(DstBuffer, Width * Height * bytesPerPixel);

		if (X11Image) XDestroyImage(X11Image);
		X11Image = ::XCreateImage(Disp->X11Display, CopyFromParent, displayDepth, ZPixmap, 0, 0, Width, Height, bitsPerPixel, Width * bytesPerPixel);

		onResize(Width, Height);
		break;
	}

	case Expose:
	{
		if (!Disp->X11Display || !X11Window || !X11Image || !CachedBuffer) break;
		if (event.xexpose.window != X11Window) break;
		if (event.xexpose.width < 1 || event.xexpose.height < 1) break;

		X11Image->data = (DT_CHAR*)CachedBuffer;
		::XPutImage(Disp->X11Display, X11Window, X11GC, X11Image, event.xexpose.x, event.xexpose.y, event.xexpose.x, event.xexpose.y, event.xexpose.width, event.xexpose.height);

		X11Image->data = DV_NULL;
		break;
	}

	case KeyPress:
	case KeyRelease:
	{
		const KeySym keySym = ::XKeycodeToKeysym(Disp->X11Display, event.xkey.keycode, 0);
		const KeySym keySym2 = (event.xkey.state & ShiftMask) ? ::XKeycodeToKeysym(Disp->X11Display, event.xkey.keycode, 1) : keySym;
		const DT_SLONG hiSym = (keySym & 0xff00) >> 8;
		const DT_SLONG loSym = keySym & 0xff;
		DT_ULONG unicode_char = 0;

		//printf("event.xkey.keycode = %d, keySym = %d, hiSym=%d, loSym=%d\n", event.xkey.keycode, keySym, hiSym, loSym);

		CDTKey key = CDTKey::Undefined;
		//DT_SLONG key = Undefined;

		switch (hiSym)
		{
		case 0x00:
			key = normalKeys_[loSym];
			break;

		case 0xff:
			key = functionKeys_[loSym];
			break;
		}

		if (event.type == KeyPress)
		{
			onKeyDown(key);
			if (loSym == 13) unicode_char = 13; else unicode_char = ConvertKeyToUnicode(keySym2);
			if (unicode_char > 0) onCharDown(unicode_char);

			bool defaultKeyHandlers = true;
			if (defaultKeyHandlers && key == CDTKey::Escape) IsShuttingDown = true;
		}
		else
		{
			onKeyUp(key);
			if (loSym == 13) unicode_char = 13; else unicode_char = ConvertKeyToUnicode(keySym2);
			if (unicode_char > 0) onCharUp(unicode_char);
		}
		break;
	}

	case ButtonPress:
	case ButtonRelease:
	{
		/*
		1 = left button
		2 = middle button (pressing the scroll wheel)
		3 = right button
		4 = roll scroll wheel up
		5 = roll scroll wheel down
		6 = push/tilt scroll wheel left (some mice)
		7 = push/tilt scroll wheel right (some mice)
		8 = 4th button (aka backward button)
		9 = 5th button (aka forward button)
		*/

		DT_SLONG button = 0; /* left button */
		DT_SLONG x = event.xbutton.x;
		DT_SLONG y = event.xbutton.y;

		     if (event.xbutton.button == Button3) button = 1; /* right button */
		else if (event.xbutton.button == Button4) button = 2; /* scroll wheel up */
		else if (event.xbutton.button == Button5) button = -2; /* scroll wheel down */
		else if (event.xbutton.button == 6 /*Button6*/) button = 3; /* scroll wheel left */
		else if (event.xbutton.button == 7 /*Button7*/) button = -3; /* scroll wheel right */

		if (event.type == ButtonPress)
		{
			     if (button == -3) onMouseScroll(x, y, -120, 0); else if (button == 3) onMouseScroll(x, y, 120, 0);
			else if (button == -2) onMouseScroll(x, y, 0, -120); else if (button == 2) onMouseScroll(x, y, 0, 120);
			else onMouseButtonDown(button, x, y);
		}
		else
		{
			if (button == 0 || button == 1) onMouseButtonUp(button, x, y);
		}
		break;
	}

	case MotionNotify:
	{
		if (event.xmotion.window != X11Window) break;
		DT_SLONG x = event.xmotion.x;
		DT_SLONG y = event.xmotion.y;
		onMouseMove(x, y);
		break;
	}

	case ClientMessage:
	{
		if (event.xclient.message_type == WMProtocols && event.xclient.format == 32 && event.xclient.data.l[0] == (DT_SLONG)WMDeleteWindow) IsShuttingDown = true;
		break;
	}

	case FocusIn:
	case FocusOut:
	{
		onActivate(event.type == FocusIn);
		break;
	}
	}
}

