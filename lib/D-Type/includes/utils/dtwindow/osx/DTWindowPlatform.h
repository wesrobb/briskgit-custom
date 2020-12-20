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

#define DV_WINDOW_WINDOW_MAX 1000
#define DV_WINDOW_WINDOW_INC 10


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
public:

	CDTWindowPlatform(bool flags, CDTDisplayPlatform* display);
	CDTWindowPlatform(CDTDisplayPlatform* display, const DT_STREAM_DESC* sd_menu = DV_NULL);
	virtual ~CDTWindowPlatform();

	virtual void onMenuCommand(DT_SLONG command, DT_SLONG extra) = 0;
	virtual void onResize(DT_SLONG width, DT_SLONG height) = 0;
	virtual void onActivate(bool active) = 0;
	virtual void onPeriodic() = 0;

	static void eventLoop();

	void shutdown();

protected:

	bool open(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG width, DT_SLONG height, DT_ULONG flags);
	bool update(DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG event_flag);
	bool update(void* pixels, DT_ULONG event_flag);
	void close();

	void modify_menu_item(DT_SLONG menu_item, DT_SLONG flag, bool is_set);
	void modify_timer(DT_SLONG value);

	DT_SLONG modify(DT_ULONG flags);
	DT_SLONG dialog_fileselect(DT_SLONG save_flag, DT_SLONG config_flags, DT_SLONG selected_filter, const DT_CHAR* title_bar, const DT_CHAR* file_types, DT_CHAR* default_filename, DT_SLONG max_length);

	virtual void onKeyDown(DT_SLONG key) = 0;
	virtual void onKeyPressed(DT_SLONG key) = 0;
	virtual void onKeyUp(DT_SLONG key) = 0;
	virtual void onCharDown(DT_ULONG unicode_char) = 0;
	virtual void onCharUp(DT_ULONG unicode_char) = 0;
	virtual void onMouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y) = 0;
	virtual void onMouseMove(DT_SLONG x, DT_SLONG y) = 0;
	virtual void onMouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y) = 0;
	virtual void onMouseScroll(DT_SLONG x, DT_SLONG y, DT_SLONG dx, DT_SLONG dy) = 0;

private:

	void defaults();
	void set_title(const DT_CHAR* title);
	bool windowed(const DT_CHAR* title, DT_SLONG x0, DT_SLONG y0);
	bool update(void* pixels, bool update_region, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG event_flag);

private:

	class CDTWindowPlatformPrivate;

	CDTWindowPlatformPrivate* Priv;
	DT_ULONG Flags;

protected:

	CDTDisplayPlatform* Display;
	void* CachedBuffer;

public:

	DT_SLONG Width, Height;
	DT_SLONG DiffX, DiffY;

	bool IsOpen;
	bool IsDisabled;
	bool ShouldClose;
};


typedef CDTDisplayPlatform CDTDisplay;


class CDTWindow : public CDTWindowPlatform
{
public: // Constructors and destructors

	CDTWindow(bool flags, CDTDisplay* display);
	CDTWindow(CDTDisplay* display, const DT_STREAM_DESC* sd_menu = DV_NULL);
	CDTWindow(CDTDisplay* display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu = DV_NULL);
	CDTWindow(CDTDisplay* display, const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu = DV_NULL);
	virtual ~CDTWindow();

public: // Public methods

	DT_SWORD Open(const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu = DV_NULL);
	DT_SWORD Open(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* sd_menu = DV_NULL);
	void Close();

	CDTDisplay* GetDisplay() const;
	DT_SLONG GetW() const;
	DT_SLONG GetH() const;
	DT_SLONG GetStatus() const;
	virtual const DT_MDC* GetMDC(DT_ULONG flags);

	DT_SWORD SetPixels(void* pixels, DT_ULONG flags);
	DT_SWORD SetMenu(DT_SLONG operation, DT_SLONG command, const DT_CHAR* extra = DV_NULL);
	DT_SWORD SetTimer(DT_SLONG value);
	DT_SWORD Refresh(DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags);
	DT_SWORD Modify(DT_ULONG flags);
	inline DT_SWORD RefreshUpdatesFullWindow() { return 1; }

public: // Common Dialogs

	DT_SLONG Dialog_FileSelect(DT_SWORD save_flag, DT_SWORD config_flags, DT_SLONG selected_filter, const DT_CHAR* title_bar, const DT_CHAR* file_types, DT_CHAR* default_filename, DT_SLONG max_length);

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

	void onResize(DT_SLONG width, DT_SLONG height);
	void onKeyDown(DT_SLONG key);
	void onKeyPressed(DT_SLONG key);
	void onKeyUp(DT_SLONG key);
	void onCharDown(DT_ULONG unicode_char);
	void onCharUp(DT_ULONG unicode_char);
	void onMouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y);
	void onMouseMove(DT_SLONG x, DT_SLONG y);
	void onMouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y);
	void onMouseScroll(DT_SLONG x, DT_SLONG y, DT_SLONG dx, DT_SLONG dy);
	void onMenuCommand(DT_SLONG command, DT_SLONG extra);
	void onActivate(bool active);
	void onPeriodic();
};

