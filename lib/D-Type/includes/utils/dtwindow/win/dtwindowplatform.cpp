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


#include <stdio.h> /* for sprintf */
#include <windowsx.h> /* for GET_X_LPARAM and GET_Y_LPARAM */

#include "DTWindowPlatform.h"


// configuration
#define DEF_ENABLE_64BIT
#define DEF_WINDOWED
#define DEF_ALLOW_CLOSE
#define DEF_CLOSE_ON_ESCAPE
#define DEF_ICON "IDI_MAIN"
#define DEF_CLASS_NAME "dtwindow"
#define DEF_NOBACKGROUND
//#define DEF_SYSTEMMENU
//#define DEF_DISABLE_SCREENSAVER

// menu option identifier
#define DV_SC_ZOOM_MSK 0x400
#define DV_SC_ZOOM_1x1 0x401
#define DV_SC_ZOOM_2x2 0x402
#define DV_SC_ZOOM_4x4 0x404

#define DV_WM_TIMER1 (WM_USER + 1)

#define WM_MOUSEWHEEL 0x020A
#define WM_MOUSEHWHEEL 0x020E
#define DEF_GET_WHEEL_DELTA_WPARAM(wParam) (static_cast<short>(HIWORD(wParam)))


CDTDisplayPlatform::CDTDisplayPlatform(DT_ULONG /*flags*/)
{
}


CDTDisplayPlatform::~CDTDisplayPlatform()
{
}


/*static*/ void CDTDisplayPlatform::EventLoop()
{
	MSG message;

	if (GetMessage(&message, DV_NULL, 0, 0) == -1) return;

	HWND hwnd = message.hwnd;

#ifdef DEF_ENABLE_64BIT
	LONG_PTR extra = GetWindowLongPtr(hwnd, GWLP_USERDATA); /* for 64 and 32-bit Windows */
#else
	LONG extra = GetWindowLong(hwnd, GWL_USERDATA);
#endif

	if (extra)
	{
		CDTWindow* window = reinterpret_cast<CDTWindow*>(extra);
		if (window->HAccel && TranslateAccelerator(hwnd, window->HAccel, &message)) { /*printf("TranslateAccelerator done\n");*/ return; }
	}

	TranslateMessage(&message);
	DispatchMessage(&message);
	return;
}


DT_FLOAT CDTDisplayPlatform::GetScale() const
{
	return 1.0;
}


DT_SLONG CDTDisplayPlatform::Dist(DT_SLONG d) const
{
	return d;
}


LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
#ifdef DEF_ENABLE_64BIT
	LONG_PTR extra = GetWindowLongPtr(hwnd, GWLP_USERDATA); /* for 64 and 32-bit Windows */
#else
	LONG extra = GetWindowLong(hwnd, GWL_USERDATA);
#endif

	if (!extra) return DefWindowProc(hwnd, umsg, wparam, lparam);

	CDTWindow* window = reinterpret_cast<CDTWindow*>(extra);
	return window->WindowProc(hwnd, umsg, wparam, lparam);
}


DT_CHAR* ReadFile(const DT_STREAM_DESC* sd, DT_SLONG* len)
{
	*len = 0;

	if (sd == DV_NULL) return DV_NULL;
	DT_DTSTREAM f = dtStreamOpen(sd, DV_STREAM_OPEN_MUST);
	if (f == DV_NULL) return DV_NULL;

	/* Determine the file size */
	dtStreamSeek(f, 0, DV_SEEK_END);
	*len = dtStreamLoc(f); if (*len < 0) *len = 0;
	dtStreamSeek(f, 0, DV_SEEK_BGN);

	DT_UBYTE* addr = static_cast<DT_UBYTE*>(dtMemAlloc(*len + 1 + 1));
	if (addr == DV_NULL) *len = 0;
	else
	{
		dtStreamRead(f, addr, *len, 0);
		addr[*len] = 0;
	}

	dtStreamClose(f, DV_STREAM_CLOSE_YES);

	return reinterpret_cast<DT_CHAR*>(addr);
}


CDTWindowPlatform::CDTWindowPlatform(bool /*flags*/, CDTDisplay* display) : Display(display)
{
	WndClear();
}


CDTWindowPlatform::CDTWindowPlatform(CDTDisplay* display, const DT_STREAM_DESC* /*sd_menu*/) : Display(display)
{
	WndClear();
}


CDTWindowPlatform::CDTWindowPlatform(CDTDisplay* display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu) : Display(display)
{
	WndClear();
	Open(title, w, h, flags, sd_menu);
}


CDTWindowPlatform::CDTWindowPlatform(CDTDisplay* display, const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu) : Display(display)
{
	WndClear();
	Open(title, x, y, w, h, flags, sd_menu);
}


CDTWindowPlatform::~CDTWindowPlatform()
{
	Close();
}


DT_SWORD CDTWindowPlatform::Open(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu)
{
	CachedBuffer = DV_NULL;
	if (w <= 0 || h <= 0) return 0;
	if (IsOpen) return 0; /* we can't open twice! */
	DT_SWORD ret = WndOpen(title, x, y, w, h, flags, sd_menu);
	IsOpen = 1;
	return ret;
}


DT_SWORD CDTWindowPlatform::Open(const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu)
{
	return Open(title, -1, -1, w, h, flags, sd_menu);
}


void CDTWindowPlatform::Close()
{
	if (HAccel) DestroyAcceleratorTable(HAccel);
	if (Wnd != DV_NULL) DestroyWindow(Wnd);

	//WndClear();
}


CDTDisplayPlatform* CDTWindow::GetDisplay() const
{
	return Display;
}


DT_SLONG CDTWindowPlatform::GetW() const
{
	return SurfaceWidth;
}


DT_SLONG CDTWindowPlatform::GetH() const
{
	return SurfaceHeight;
}


DT_SLONG CDTWindowPlatform::GetStatus() const
{
	if (Wnd == DV_NULL) return 0;
	return 1;
}


DT_SWORD CDTWindowPlatform::SetTimer(DT_SLONG value)
{
	if (value < 0) value = -value;
	::SetTimer(Wnd, DV_WM_TIMER1, value / 1000, DV_NULL);
	return 1;
}


DT_SWORD CDTWindowPlatform::SetMenu(DT_SLONG operation, DT_SLONG command, const DT_CHAR* /*extra*/)
{
	if (operation == DV_WINDOW_MENU_UNCHECK) CheckMenuItem(GetMenu(Wnd), command, MF_UNCHECKED);
	else if (operation == DV_WINDOW_MENU_CHECK) CheckMenuItem(GetMenu(Wnd), command, MF_CHECKED);
	else if (operation == DV_WINDOW_MENU_DISABLE) EnableMenuItem(GetMenu(Wnd), command, MF_GRAYED);
	else if (operation == DV_WINDOW_MENU_ENABLE) EnableMenuItem(GetMenu(Wnd), command, MF_ENABLED);
	return 1;
}


const DT_MDC* CDTWindowPlatform::GetMDC(DT_ULONG /*flags*/)
{
	return DV_NULL;
}


DT_SWORD CDTWindowPlatform::SetPixels(void* pixels, DT_ULONG flags)
{
#ifdef DEF_REFRESH_UPDATES_FULL_WINDOW
	if (flags & 2) return 1;
#endif
	return WndUpdate(pixels, 0, 0, SurfaceWidth, SurfaceHeight, flags);
}


DT_SWORD CDTWindowPlatform::Modify(DT_ULONG flags)
{
	return WndModify(flags);
}


DT_SWORD CDTWindowPlatform::Refresh(DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags)
{
	if (w <= 0 || h <= 0) return 0;
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if (w <= 0 || h <= 0) return 0;

#ifdef DEF_REFRESH_UPDATES_FULL_WINDOW
	if (flags & 2) return 1;
	return WndUpdate(CachedBuffer, 0, 0, SurfaceWidth, SurfaceHeight, flags);
#else
	return WndUpdate(CachedBuffer, x, y, w, h, flags);
#endif
}


DT_SWORD CDTWindowPlatform::WndOpen(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu)
{
	RECT rect;
	WNDCLASS wc;
	DT_SWORD ret = 0;
	DT_SLONG len = 0;
	DT_CHAR* menudata = DV_NULL;
	HMENU hmenu = 0;
	HMENU hmenu_temp = 0;

	if (flags & DV_WINDOW_HINT_FULLSCREEN) { w = GetSystemMetrics(SM_CXSCREEN); h = GetSystemMetrics(SM_CYSCREEN); }

	if (flags & DV_WINDOW_HINT_RGB) { w >>= 2; w += 4; w <<= 2; }

	menudata = ReadFile(sd_menu, &len);
	hmenu_temp = WndMakeMenu(menudata, &HAccel);
	if (menudata != DV_NULL) dtMemFree(menudata);

	if (flags & DV_WINDOW_HINT_SYSMENU) hmenu = hmenu_temp; /* hmenu will be assigned to the window; a menu that is assigned to a window is automatically destroyed when the window is destroyed */
	else if (hmenu_temp != 0) DestroyMenu(hmenu_temp); /* we only wanted to create system HAccel for the window */

	// register window class
	wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW; /* very important */
	wc.lpfnWndProc = &StaticWindowProc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
#ifdef DEF_ICON
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = LoadIcon(wc.hInstance, DEF_ICON);
#else
	wc.hInstance = 0;
	wc.hIcon = DV_NULL;
#endif
	wc.hCursor = LoadCursor(0, IDC_ARROW);
#ifdef DEF_NOBACKGROUND
	wc.hbrBackground = DV_NULL;
#else
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //COLOR_BACKGROUND;
#endif
	wc.lpszMenuName = DV_NULL;
	wc.lpszClassName = DEF_CLASS_NAME;
	RegisterClass(&wc);

	// calculate window size
	rect.left = 0;
	rect.top = 0;
	rect.right = w;
	rect.bottom = h;

	     if (flags & DV_WINDOW_HINT_NOFRAME) AdjustWindowRect(&rect, WS_POPUPWINDOW, hmenu != 0);
	else if (flags & DV_WINDOW_HINT_RESIZE)  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, hmenu != 0);
	else                                     AdjustWindowRect(&rect, WS_POPUP | WS_SYSMENU | WS_CAPTION, hmenu != 0);

	//printf("AdjustWindowRect w=%ld, h=%ld, rect: %ld, %ld, %ld, %ld\n", w, h, rect.left, rect.top, rect.right, rect.bottom);

	rect.right -= rect.left;
	rect.bottom -= rect.top;

	// save surface size and original window size
	SurfaceWidth = w;
	SurfaceHeight = h;
	OriginalX = CW_USEDEFAULT;
	OriginalY = CW_USEDEFAULT;
	OriginalW = rect.right;
	OriginalH = rect.bottom;
	Flags = flags;

	if (flags & DV_WINDOW_HINT_CENTER)
	{
		// center window
		OriginalX = (GetSystemMetrics(SM_CXSCREEN) - OriginalW) / 2; if (OriginalX < 0) OriginalX = 0;
		OriginalY = (GetSystemMetrics(SM_CYSCREEN) - OriginalH) / 2; if (OriginalY < 0) OriginalY = 0;
	}
	else if (flags & DV_WINDOW_HINT_FULLSCREEN)
	{
		OriginalX = 0;
		OriginalY = 0;
	}

	if (x >= 0) OriginalX = x;
	if (y >= 0) OriginalY = y;

	// create window and show it
	if (flags & DV_WINDOW_HINT_NOFRAME)
		Wnd = CreateWindowEx(0, DEF_CLASS_NAME, title, WS_POPUPWINDOW, OriginalX, OriginalY, OriginalW, OriginalH, 0, hmenu, 0, 0);
	else if (flags & DV_WINDOW_HINT_RESIZE)
		Wnd = CreateWindowEx(0, DEF_CLASS_NAME, title, WS_OVERLAPPEDWINDOW, OriginalX, OriginalY, OriginalW, OriginalH, 0, hmenu, 0, 0);
	else
		Wnd = CreateWindowEx(0, DEF_CLASS_NAME, title, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, OriginalX, OriginalY, OriginalW, OriginalH, 0, hmenu, 0, 0);

	if (Wnd == DV_NULL) goto end;

	ret = 1;

#ifdef DEF_ENABLE_64BIT
	SetWindowLongPtr(Wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)); /* for 64 and 32-bit Windows */
#else
	SetWindowLongW(Wnd, GWL_USERDATA, reinterpret_cast<LONG>(this));
#endif

	ShowWindow(Wnd, SW_NORMAL);

	// create bitmap header
	BitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapHeader.biPlanes = 1;
	BitmapHeader.biWidth = SurfaceWidth;
	BitmapHeader.biHeight = SurfaceHeight;
	if (flags & DV_WINDOW_HINT_RGB) BitmapHeader.biBitCount = 24; else BitmapHeader.biBitCount = 32;
	BitmapHeader.biCompression = BI_RGB;
	BitmapHeader.biSizeImage = 0 * 0;
	BitmapHeader.biXPelsPerMeter = 640;
	BitmapHeader.biYPelsPerMeter = 640;
	BitmapHeader.biClrUsed = 0;
	BitmapHeader.biClrImportant = 0;

	// initialize DrawDib
	BitmapHDD = DrawDibOpen();

#ifdef DEF_SYSTEMMENU
	// add entry to system menu
	HMENU menu = GetSystemMenu(Wnd, FALSE);
	AppendMenu(menu, MF_STRING, DV_SC_ZOOM_1x1, "Zoom 1 x 1");
	AppendMenu(menu, MF_STRING, DV_SC_ZOOM_2x2, "Zoom 2 x 2");
	AppendMenu(menu, MF_STRING, DV_SC_ZOOM_4x4, "Zoom 4 x 4");
#endif

#ifdef DEF_DISABLE_SCREENSAVER

	// disable screensaver while ptc is open
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);

#endif

	SetForegroundWindow(Wnd); /* added to bring window to front when opened from terminal window */

	//::SetTimer(Wnd, DV_WM_TIMER1, 200, DV_NULL);
	::SetTimer(Wnd, DV_WM_TIMER1, 40, DV_NULL);

end:

	return ret;
}


DT_SWORD CDTWindowPlatform::WndModify(DT_ULONG flags)
{
	if (Wnd == DV_NULL) return 0;
	if (flags == 0) return 1;

	else if (flags == DV_WINDOW_ATTRIB_TOP) { SetWindowPos(Wnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); return 1; } /* TOP */
	else if (flags == DV_WINDOW_ATTRIB_BOTTOM) { SetWindowPos(Wnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); return 1; } /* BOTTOM */
	else if (flags == DV_WINDOW_ATTRIB_TOPMOST) { SetWindowPos(Wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); return 1; } /* TOPMOST */
	else if (flags == DV_WINDOW_ATTRIB_DISABLE) { EnableWindow(Wnd, FALSE); return 1; } /* Disable */
	else if (flags == DV_WINDOW_ATTRIB_ENABLE) { EnableWindow(Wnd, TRUE); return 1; } /* Enable */

	return 0;
}


DT_SWORD CDTWindowPlatform::WndUpdate(void* buffer, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG event_flag)
{
	MSG message;

	// update buffer ptr cache
	CachedBuffer = buffer; /* this is kinda naughty */

	BitmapHeader.biWidth = SurfaceWidth;
	BitmapHeader.biHeight = SurfaceHeight;

	if (Wnd == DV_NULL) return 0;

	if (event_flag & 4) { /* response to resize event - no need to call DrawDibDraw since WM_PAINT that follows will do that */ }
	else
	{
		HDC dc = GetDC(Wnd);
		if (CachedBuffer) DrawDibDraw(BitmapHDD, dc, x, y, w, h, &BitmapHeader, CachedBuffer, x, y, w, h, 0);
		ReleaseDC(Wnd, dc);
	}

	if (!(event_flag & 1)) return 1;

	// process messages
	while (PeekMessage(&message, Wnd, 0, 0, PM_REMOVE))
	{
		// translate and dispatch
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	// sleep
	Sleep(0);

	return 1;
}


void CDTWindowPlatform::WndClear()
{
	Flags = 0;
	BitmapHDD = DV_NULL;
	Wnd = DV_NULL;
	HAccel = 0;

	SurfaceWidth = SurfaceHeight = 0;
	CachedBuffer = DV_NULL;

	OriginalX = OriginalY = 0;
	OriginalW = OriginalH = 0;

	IsOpen = 0;
}


void CDTWindowPlatform::WndClose()
{
	if (Wnd == DV_NULL) return;

	KillTimer(Wnd, DV_WM_TIMER1);

	// close down the drawdib library
	DrawDibClose(BitmapHDD);

	WndClear();

#ifdef DEF_DISABLE_SCREENSAVER

	// enable screensaver now that ptc is closed
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1, 0, 0);

#endif

}


LRESULT CALLBACK CDTWindowPlatform::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;
	DT_SLONG i;
	DT_UBYTE translate[256];

	for (i = 0; i < 256; i++) { translate[i] = static_cast<DT_UBYTE>(i); }

	translate[13] = '\n'; // Enter
	translate[45] = 0x9B; // Insert
	translate[46] = 0x7F; // Delete
	translate[186] = 0x3B; // SemiColon
	translate[187] = 0x3D; // Equals
	translate[188] = 0x2C; // Comma
	translate[189] = 0x6C; // Separator
	translate[190] = 0x2E; // Period
	translate[191] = 0x2F; // Slash
	translate[219] = 0x5B; // OpenBracket
	translate[220] = 0x5C; // BackSlash
	translate[221] = 0x5D; // CloseBracket


	// handle message
	switch (message)
	{
	case WM_TIMER:
	{
		//printf("DV_WM_TIMER1\n");
		if (wparam == DV_WM_TIMER1) Event_Other(DV_WINDOW_OTHEREVENT_TIMER);
		break;
	}

	case WM_COMMAND:
	{
		//printf("command %d, %d\n", LOWORD(wparam), HIWORD(lparam));
		Event_MenuCommand(LOWORD(wparam), 0);
		break;
	}

	case WM_SIZE:
	{
		if (IsOpen == 0) break; /* ignore this event until the window is open */
		if (LOWORD(lparam) < 1 || HIWORD(lparam) < 1) break;
		if (SurfaceWidth == LOWORD(lparam) && SurfaceHeight == HIWORD(lparam)) break;

		SurfaceWidth = LOWORD(lparam);
		SurfaceHeight = HIWORD(lparam);

		if (BitmapHeader.biBitCount == 24) { SurfaceWidth >>= 2; SurfaceWidth += 4; SurfaceWidth <<= 2; }

		//BitmapHeader.biWidth = SurfaceWidth;
		//BitmapHeader.biHeight = SurfaceHeight;

		Event_Resize(SurfaceWidth, SurfaceHeight);
		break;
	}

	case WM_ACTIVATE:
	{
		if (wparam == WA_INACTIVE) Event_Other(DV_WINDOW_OTHEREVENT_INACTIVE);
		else Event_Other(DV_WINDOW_OTHEREVENT_ACTIVE);
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(hwnd, &ps);
		if (dc && CachedBuffer) DrawDibDraw(BitmapHDD, dc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom, &BitmapHeader, CachedBuffer, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom, 0);
		EndPaint(hwnd, &ps);

		// validate window
		ValidateRect(Wnd, DV_NULL);
		break;
	}

#ifdef DEF_WINDOWED
#ifdef DEF_SYSTEMMENU

	// check for message from our system menu entry
	case WM_SYSCOMMAND:
	{
		if ((wparam & 0xFFFFFFF0) == DV_SC_ZOOM_MSK)
		{
			if (Flags & DV_WINDOW_HINT_CENTER)
			{
				DT_SLONG zoom = wparam & 0x7;
				DT_SLONG x = (GetSystemMetrics(SM_CXSCREEN) - OriginalW * zoom) / 2; if (x < 0) x = 0;
				DT_SLONG y = (GetSystemMetrics(SM_CYSCREEN) - OriginalH * zoom) / 2; if (y < 0) y = 0;
				SetWindowPos(hwnd, DV_NULL, x, y, OriginalW * zoom, OriginalH * zoom, SWP_NOZORDER);
			}
			else
			{
				DT_SLONG zoom = wparam & 0x7;
				SetWindowPos(hwnd, DV_NULL, 0, 0, OriginalW * zoom, OriginalH * zoom, SWP_NOMOVE | SWP_NOZORDER);
			}
		}
			// pass everything else to the default (this is rather important)
		else return DefWindowProc(hwnd, message, wparam, lparam);
	}

#endif
#endif

	case WM_CHAR:
	{
		Event_CharDown(static_cast<DT_ID_ULONG>(wparam));
		break;
	}

	case WM_SYSKEYUP:
	{
		if ((wparam & 0xFF) != 18) break;
		// else fall through
	}

	case WM_KEYUP:
	{
		Event_KeyUp(translate[wparam & 0xFF]);
		break;
	}

	case WM_SYSKEYDOWN:
	{
		if ((wparam & 0xFF) != 18) break;
		// else fall through
	}

	case WM_KEYDOWN:
	{
		Event_KeyDown(translate[wparam & 0xFF]);

#ifdef DEF_CLOSE_ON_ESCAPE
		// close on escape key
		if ((wparam & 0xFF) != 27) break;
#else
		break;
#endif
	}
		// else fall through

	case WM_CLOSE:
	{
#ifdef DEF_ALLOW_CLOSE
		DestroyWindow(hwnd);
#endif
		break;
	}

	case WM_DESTROY:
	{
		WndClose();
		break;
	}

	case WM_LBUTTONDOWN:
	{
		SetCapture(hwnd);
		Event_MouseButtonDown(0, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;
	}

	case WM_RBUTTONDOWN:
	{
		SetCapture(hwnd);
		Event_MouseButtonDown(1, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;
	}

	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		Event_MouseButtonUp(0, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;
	}

	case WM_RBUTTONUP:
	{
		ReleaseCapture();
		Event_MouseButtonUp(1, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;
	}

	case WM_MOUSEMOVE:
	{
		Event_MouseMove(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;
	}

	case WM_MOUSEWHEEL:
	{
		Event_MouseScroll(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, DEF_GET_WHEEL_DELTA_WPARAM(wparam));
		break;
	}

	case WM_MOUSEHWHEEL:
	{
		Event_MouseScroll(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), DEF_GET_WHEEL_DELTA_WPARAM(wparam), 0);
		break;
	}

	default:
	{
		// unhandled messages
		result = DefWindowProc(hwnd, message, wparam, lparam);
	}
	}

	// finished
	return result;
}


HMENU CDTWindowPlatform::WndMakeMenu(const DT_CHAR* menudata, HACCEL* haccel)
{
	*haccel = DV_NULL;

	if (menudata == DV_NULL) return 0;

	HMENU hmenu[100];
	ACCEL accel[512];
	wchar_t ws2[1024 * sizeof(wchar_t)]; /* for MultiByteToWideChar */

	DT_CHAR s[1024], s2[1024], empty[1] = "";
	DT_SLONG i, j, menu_count, count_1 = 0, count_2 = 0, accel_count = 0;
	const DT_CHAR* addr;

	addr = menudata;
	while (1)
	{
		addr = strchr(addr, '}');
		if (addr == DV_NULL) break;
		addr++;
		count_1++;
	}

	addr = menudata;
	while (1)
	{
		addr = strchr(addr, '{');
		if (addr == DV_NULL) break;
		addr++;
		count_2++;
	}

	menu_count = count_1;
	if (count_1 != count_2) return 0; /* bad menudata */
	if (menu_count <= 0 || menu_count >= 100) return 0; /* bad menudata */

	for (i = 0; i < menu_count; i++) hmenu[i] = CreateMenu();

	//printf("menu_count = %ld\n", menu_count);

	addr = menudata;

	for (i = 0; i < menu_count;)
	{
		j = 0;
		while (*addr != 0)
		{
			DT_CHAR c = *addr;
			s[j] = c;
			addr++;
			if (c == 10) { if (s[j] == 13) addr++; break; } /* deal with OS specific linebreaks (10 or 10+13) */
			if (c == 13) { if (s[j] == 10) addr++; break; } /* deal with OS specific linebreaks (13 or 13+10) */
			j++;
		}
		s[j] = 0;

		     if (s[0] == '#') continue;
		else if (s[0] == '!') continue;
		else if (s[0] == '}') { i++; continue; }
		else if (s[0] == '{') continue;
		else if (strlen(s) < 2) continue;

		DT_SLONG child = atol(s); if (child < 0 || child >= menu_count) continue;
		DT_CHAR* menucode = strchr(s, '|'); if (menucode == DV_NULL) continue; menucode++;
		DT_CHAR* text = strchr(menucode, '|'); if (text == DV_NULL) continue; text++;
		DT_CHAR* key = strchr(text, '|'); if (key == DV_NULL) key = empty; else { *key = 0; key++; }
		DT_SLONG type = (text[0] == 0 ? MF_SEPARATOR : MF_STRING);
		DT_SLONG key_code = 0, key_flag = 0;

		if (strlen(key) == 1 && key[0] >= '0' && key[0] <= '9') { key_code = key[0]; } /* Ctrl */
		if (strlen(key) == 1 && key[0] >= 'a' && key[0] <= 'z') { key_code = key[0] + 'A' - 'a'; } /* Ctrl */
		if (strlen(key) == 1 && key[0] >= 'A' && key[0] <= 'Z') { key_code = key[0]; key_flag = 1; } /* Alt */
		if (strlen(key) == 1 && (key[0] == '-' || key[0] == '+')) { key_code = key[0]; key_flag = 2; } /* Ctrl */

		if (key_code == 0) sprintf(s2, "%s", text);
		else if (key_flag == 1) sprintf(s2, "%s\tAlt+%c", text, key_code);
		else if (key_flag == 2) sprintf(s2, "%s\tCtrl %c", text, key_code);
		else sprintf(s2, "%s\tCtrl+%c", text, key_code);

		//printf("** Menu Item ** %d, %d, %s, %s (%d)\n", child, atoi(menucode), s2, key, key_code);

		MultiByteToWideChar(CP_UTF8, 0, s2, -1, ws2, 1024);

		if (child == 0) AppendMenuW(hmenu[i], type, atoi(menucode), ws2);
		else AppendMenuW(hmenu[i], type | MF_POPUP, reinterpret_cast<UINT_PTR>(hmenu[child]), ws2);

		if (key_code == 0) continue;

		//printf("** Menu Accelertor ** %d: %d -> %d\n", accel_count, key_code, atoi(menucode));

		if (accel_count >= 512) continue;

		accel[accel_count].key = static_cast<WORD>(key_code);
		accel[accel_count].cmd = static_cast<WORD>(atoi(menucode));
		accel[accel_count].fVirt = FVIRTKEY;
		if (key_flag == 1) accel[accel_count].fVirt |= FALT; else accel[accel_count].fVirt |= FCONTROL;
		accel_count++;
	}

	if (accel_count > 0) *haccel = CreateAcceleratorTable(accel, accel_count);
	return hmenu[0];
}


//DT_SLONG CDTWindowPlatform::Dialog_FileSelect(DT_SWORD save_flag, DT_SWORD /*config_flags*/, DT_SLONG selected_filter, const DT_CHAR* title_bar, const DT_CHAR* file_types, DT_CHAR* default_filename, DT_SLONG max_length)
//{
//	if (title_bar == DV_NULL || file_types == DV_NULL || default_filename == DV_NULL) return -1;
//
//	DT_SLONG ret = 0;
//	OPENFILENAME ofn; // common dialog box structure
//
//	// Initialize OPENFILENAME
//	ZeroMemory(&ofn, sizeof(OPENFILENAME));
//	ofn.lStructSize = sizeof(OPENFILENAME);
//	ofn.hwndOwner = Wnd;
//	ofn.lpstrFile = default_filename;
//	ofn.nMaxFile = max_length;
//	ofn.lpstrFilter = file_types;
//	ofn.nFilterIndex = selected_filter;
//	ofn.lpstrFileTitle = DV_NULL;
//	ofn.nMaxFileTitle = 0;
//	ofn.lpstrInitialDir = DV_NULL;
//	ofn.lpstrTitle = title_bar;
//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
//
//	if (save_flag) ret = GetSaveFileName(&ofn); else ret = GetOpenFileName(&ofn);
//
//	if (ret) return ofn.nFilterIndex - 1;
//	return -1;
//}


wchar_t* CDTWindowPlatform::UTF8_To_UTF16_StringAlloc(const DT_CHAR* str_utf8, DT_SLONG len_bytes /*= -1*/, DT_SLONG* len_out /*= DV_NULL*/) /* note: len_out includes the terminating null character */
{
	if (len_out != DV_NULL) *len_out = 0;
	if (str_utf8 == DV_NULL) return DV_NULL; if (*str_utf8 == 0) return DV_NULL;

	UINT code_page = CP_UTF8;
	DT_SLONG len1 = MultiByteToWideChar(code_page, 0, str_utf8, len_bytes, DV_NULL, 0);
	if (len1 <= 0) { code_page = CP_ACP; len1 = MultiByteToWideChar(code_page, 0, str_utf8, len_bytes, DV_NULL, 0); /* fallback to Windows ANSI codepage */ }
	if (len1 <= 0) return DV_NULL;

	wchar_t* str_utf16 = static_cast<wchar_t*>(calloc(len1, sizeof(wchar_t))); if (str_utf16 == DV_NULL) return DV_NULL;
	/* if successful, returns the number of characters written to the buffer */
	DT_SLONG len2 = MultiByteToWideChar(code_page, 0, str_utf8, len_bytes, str_utf16, len1);
	if (len_out != DV_NULL) *len_out = len2;
	if (len2 > 0) return str_utf16;

	free(str_utf16);
	return DV_NULL;
}


DT_CHAR* CDTWindowPlatform::UTF16_To_UTF8_StringAlloc(const wchar_t* str_utf16, DT_SLONG len_chars /*= -1*/, DT_SLONG* len_out /*= DV_NULL*/) /* note: len_out includes the terminating null character */
{
	if (len_out != DV_NULL) *len_out = 0;
	if (str_utf16 == DV_NULL) return DV_NULL; if (*str_utf16 == 0) return DV_NULL;

	UINT code_page = CP_UTF8;
	DT_SLONG len1 = WideCharToMultiByte(code_page, 0, str_utf16, len_chars, DV_NULL, 0, DV_NULL, DV_NULL);
	if (len1 <= 0) { code_page = CP_ACP; len1 = WideCharToMultiByte(code_page, 0, str_utf16, len_chars, DV_NULL, 0, DV_NULL, DV_NULL); /* fallback to Windows ANSI codepage */ }
	if (len1 <= 0) return DV_NULL;

	DT_CHAR* str_utf8 = static_cast<DT_CHAR*>(calloc(len1, sizeof(DT_CHAR))); if (str_utf8 == DV_NULL) return DV_NULL;
	/* if successful, returns the number of bytes written to the buffer */
	DT_SLONG len2 = WideCharToMultiByte(code_page, 0, str_utf16, len_chars, str_utf8, len1, DV_NULL, DV_NULL);
	if (len_out != DV_NULL) *len_out = len2;
	if (len2 > 0) return str_utf8;

	free(str_utf8);
	return DV_NULL;
}


DT_SLONG CDTWindowPlatform::Dialog_FileSelect(DT_SWORD save_flag, DT_SWORD /*config_flags*/, DT_SLONG selected_filter, const DT_CHAR* title_bar, const DT_CHAR* file_types, DT_CHAR* default_filename, DT_SLONG max_length)
{
	if (title_bar == DV_NULL || file_types == DV_NULL || default_filename == DV_NULL) return -1;

	DT_SLONG len_bytes = 0, len_out = 0;

	wchar_t default_filename_w_static[1024] = {0};
	wchar_t* default_filename_w = UTF8_To_UTF16_StringAlloc(default_filename, -1, &len_out);
	if (default_filename_w != DV_NULL && len_out < 1024) dtMemCopy(default_filename_w_static, default_filename_w, len_out * sizeof(wchar_t));
	StringFree(default_filename_w);

	for (;; len_bytes++)
	{
		if (file_types[len_bytes] != 0) continue;
		if (file_types[len_bytes + 1] == 0) { len_bytes += 2; break; }
	}

	wchar_t* title_bar_w = UTF8_To_UTF16_StringAlloc(title_bar, -1, &len_out); if (title_bar_w == DV_NULL) return -1;
	wchar_t* file_types_w = UTF8_To_UTF16_StringAlloc(file_types, len_bytes, &len_out); if (file_types_w == DV_NULL) { StringFree(title_bar_w); return -1; }

	DT_SLONG ret = 0;
	OPENFILENAMEW ofn; // common dialog box structure

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = Wnd;
	ofn.lpstrFile = default_filename_w_static;
	ofn.nMaxFile = 1024; //max_length;
	ofn.lpstrFilter = file_types_w;
	ofn.nFilterIndex = selected_filter;
	ofn.lpstrFileTitle = DV_NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = DV_NULL;
	ofn.lpstrTitle = title_bar_w;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	if (save_flag) ret = GetSaveFileNameW(&ofn); else ret = GetOpenFileNameW(&ofn);

	StringFree(title_bar_w);
	StringFree(file_types_w);

	len_out = 0; *default_filename = 0;
	DT_CHAR* default_filename_utf8 = UTF16_To_UTF8_StringAlloc(ofn.lpstrFile, -1, &len_out);
	if (len_out < max_length) dtMemCopy(default_filename, default_filename_utf8, len_out);
	StringFree(default_filename_utf8);

	if (ret) return ofn.nFilterIndex - 1;
	return -1;
}


/*
	//Accelerator example


	ACCEL accel[]=
	{
		FCONTROL|FVIRTKEY, 'X', 37, //X or 0x58
		FCONTROL|FVIRTKEY, 'C', 38, //C or 0x43
		FCONTROL|FVIRTKEY, 'V', 39, //V or 0x56
		FALT|FVIRTKEY, VK_BACK, 1003,
		FSHIFT|FVIRTKEY, VK_DELETE, 1004,
		FVIRTKEY, VK_DELETE, 1005,
	};
*/


/*

	// Menu example


	DT_SLONG j = 0;

	typedef struct
	{
		const DT_CHAR* Text;
		DT_ID_SWORD Child;
		DT_UWORD Attribs;

	} DT_MENUITEM;

	const DT_MENUITEM* pmenu[50];

	const DT_MENUITEM menu0[] = {{"File",1,0}, {"Edit",2,0}, {"Image",0,0}, {DV_NULL,0,0}};
	hmenu[menu_count] = CreateMenu();
	pmenu[menu_count] = menu0;
	menu_count++;

	const DT_MENUITEM menu1[] = {{"Open",0,0}, {"Save",0,0}, {"",0,0}, {"Print",3,0}, {DV_NULL,0,0}};
	hmenu[menu_count] = CreateMenu();
	pmenu[menu_count] = menu1;
	menu_count++;

	const DT_MENUITEM menu2[] = {{"Cut",0,0}, {"Copy",0,0}, {"Paste",0,0}, {DV_NULL,0,0}};
	hmenu[menu_count] = CreateMenu();
	pmenu[menu_count] = menu2;
	menu_count++;

	const DT_MENUITEM menu3[] = {{"Document",0,0}, {"Page",0,0}, {"Selection",0,0}, {DV_NULL,0,0}};
	hmenu[menu_count] = CreateMenu();
	pmenu[menu_count] = menu3;
	menu_count++;

	for (i = 0; i < menu_count; j++)
	{
		DT_MENUITEM item = pmenu[i][j];

		if (item.Text == DV_NULL) { j = -1; i++; continue; }

		DT_SLONG type = (item.Text[0] == 0 ? MF_SEPARATOR : MF_STRING);

		if (item.Child == 0) AppendMenu(hmenu[i], type, DV_SC_ZOOM_1x1, item.Text);
		else AppendMenu(hmenu[i], type | MF_POPUP, (UINT)hmenu[item.Child], item.Text);
	}


	SetMenu(Wnd, hmenu[0]);
*/


/*
	submenu = CreatePopupMenu();
	AppendMenu(submenu, MF_STRING, DV_SC_ZOOM_1x1, "Open");
	AppendMenu(submenu, MF_STRING, DV_SC_ZOOM_1x1, "Save");
	AppendMenu(submenu, MF_SEPARATOR, 0, "");
	AppendMenu(submenu, MF_STRING, DV_SC_ZOOM_1x1, "Print");
	AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)submenu, "File");
	submenu = CreatePopupMenu();
	AppendMenu(submenu, MF_STRING, DV_SC_ZOOM_1x1, "Cut");
	AppendMenu(submenu, MF_STRING, DV_SC_ZOOM_1x1, "Copy");
	AppendMenu(submenu, MF_STRING, DV_SC_ZOOM_1x1, "Paste");
	AppendMenu(submenu, MF_SEPARATOR, 0, "");
	AppendMenu(submenu, MF_STRING, DV_SC_ZOOM_1x1, "Undo");
	AppendMenu(submenu, MF_STRING, DV_SC_ZOOM_1x1, "Redo");
	AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)submenu, "Edit");
	AppendMenu(menu, MF_STRING, DV_SC_ZOOM_1x1, "Image");
	AppendMenu(menu, MF_STRING, DV_SC_ZOOM_1x1, "View");
	AppendMenu(menu, MF_STRING, DV_SC_ZOOM_1x1, "Page");
*/

