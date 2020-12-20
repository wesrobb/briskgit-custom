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


#include "dtype.h"
#include <vfw.h>
#pragma comment(lib, "msvfw32.lib")


//#define DEF_REFRESH_UPDATES_FULL_WINDOW

#define DV_WINDOW_HINT_RESIZE 1
#define DV_WINDOW_HINT_CENTER 2
#define DV_WINDOW_HINT_NOFRAME 4
#define DV_WINDOW_HINT_FULLSCREEN 8
#define DV_WINDOW_HINT_FULLRESOLUTION 16
#define DV_WINDOW_HINT_RGB 256
#define DV_WINDOW_HINT_SYSMENU 512

#define DV_WINDOW_MENU_UNCHECK 0
#define DV_WINDOW_MENU_CHECK 1
#define DV_WINDOW_MENU_DISABLE 2
#define DV_WINDOW_MENU_ENABLE 3

#define DV_WINDOW_ATTRIB_TOP 1
#define DV_WINDOW_ATTRIB_BOTTOM 2
#define DV_WINDOW_ATTRIB_TOPMOST 3
#define DV_WINDOW_ATTRIB_DISABLE 4
#define DV_WINDOW_ATTRIB_ENABLE 5
#define DV_WINDOW_ATTRIB_VBSYNC_OFF 6
#define DV_WINDOW_ATTRIB_VBSYNC_ON 7

#define DV_WINDOW_OTHEREVENT_INACTIVE 0
#define DV_WINDOW_OTHEREVENT_ACTIVE 1
#define DV_WINDOW_OTHEREVENT_TIMER 2


class CDTDisplayPlatform
{
public: // Constructors and destructors

	CDTDisplayPlatform(DT_ULONG flags);
	virtual ~CDTDisplayPlatform();

public: // Public methods

	DT_FLOAT GetScale() const;
	DT_SLONG Dist(DT_SLONG d) const;
	void EventLoop();
};


class CDTWindowPlatform
{
public: // Constructors and destructors

	CDTWindowPlatform(bool flags, CDTDisplayPlatform* display);
	CDTWindowPlatform(CDTDisplayPlatform* display, const DT_STREAM_DESC* sd_menu = DV_NULL);
	CDTWindowPlatform(CDTDisplayPlatform* display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu = DV_NULL);
	CDTWindowPlatform(CDTDisplayPlatform* display, const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu = DV_NULL);
	virtual ~CDTWindowPlatform();

public: // Public methods

	DT_SWORD Open(const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu = DV_NULL);
	DT_SWORD Open(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu = DV_NULL);
	void Close();

	CDTDisplayPlatform* GetDisplay() const;
	DT_SLONG GetW() const;
	DT_SLONG GetH() const;
	DT_SLONG GetStatus() const;
	virtual const DT_MDC* GetMDC(DT_ULONG flags);

	DT_SWORD SetPixels(void* pixels, DT_ULONG flags);
	DT_SWORD SetMenu(DT_SLONG operation, DT_SLONG command, const DT_CHAR* extra = DV_NULL);
	DT_SWORD SetTimer(DT_SLONG value);
	DT_SWORD Refresh(DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags);
	DT_SWORD Modify(DT_ULONG flags);
	inline DT_SWORD RefreshUpdatesFullWindow()
	{
	#ifdef DEF_REFRESH_UPDATES_FULL_WINDOW
		return 1;
	#else
		return 0;
	#endif
	}

public: // Common Dialogs

	DT_SLONG Dialog_FileSelect(DT_SWORD save_flag, DT_SWORD config_flags, DT_SLONG selected_filter, const DT_CHAR* title_bar, const DT_CHAR* file_types, DT_CHAR* default_filename, DT_SLONG max_length);

	static wchar_t* UTF8_To_UTF16_StringAlloc(const DT_CHAR* str_utf8, DT_SLONG len_bytes = -1, DT_SLONG* len_out = DV_NULL);
	static DT_CHAR* UTF16_To_UTF8_StringAlloc(const wchar_t* str_utf16, DT_SLONG len_chars = -1, DT_SLONG* len_out = DV_NULL);
	static void StringFree(void* str) { if (str != DV_NULL) free(str); }

protected: // Event-driven callbacks

	virtual void Event_Resize(DT_SLONG /*w*/, DT_SLONG /*h*/) {}
	virtual void Event_KeyDown(DT_SLONG /*key*/) {}
	virtual void Event_KeyUp(DT_SLONG /*key*/) {}
	virtual void Event_CharDown(DT_ID_ULONG /*char_code*/) {}
	virtual void Event_CharUp(DT_ID_ULONG /*char_code*/) {}
	virtual void Event_MouseButtonDown(DT_SLONG /*button*/, DT_SLONG /*x*/, DT_SLONG /*y*/) {}
	virtual void Event_MouseMove(DT_SLONG /*x*/, DT_SLONG /*y*/) {}
	virtual void Event_MouseButtonUp(DT_SLONG /*button*/, DT_SLONG /*x*/, DT_SLONG /*y*/) {}
	virtual void Event_MouseScroll(DT_SLONG /*x*/, DT_SLONG /*y*/, DT_SLONG /*dx*/, DT_SLONG /*dy*/) {}
	virtual void Event_MenuCommand(DT_SLONG /*command*/, DT_SLONG /*extra*/) {}
	virtual void Event_Other(DT_SLONG /*param*/) {}

private:

	DT_SWORD WndOpen(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu);
	DT_SWORD WndUpdate(void* buffer, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG event_flag);
	DT_SWORD WndModify(DT_ULONG flags);
	HMENU WndMakeMenu(const DT_CHAR* menudata, HACCEL* haccel);
	void WndClear();
	void WndClose();

private:

	CDTDisplayPlatform* Display;
	BITMAPINFOHEADER BitmapHeader;
	HDRAWDIB BitmapHDD;
	HWND Wnd;

	DT_ULONG Flags;
	DT_SLONG SurfaceWidth;
	DT_SLONG SurfaceHeight;
	void* CachedBuffer;

	DT_SLONG OriginalX;
	DT_SLONG OriginalY;
	DT_SLONG OriginalW;
	DT_SLONG OriginalH;

	DT_SWORD IsOpen;

public:

	HACCEL HAccel;
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
};


typedef CDTDisplayPlatform CDTDisplay;
typedef CDTWindowPlatform CDTWindow;
//class CDTWindow : public CDTWindowPlatform {};

