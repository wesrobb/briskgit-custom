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


/*

To compile as a test application define DEF_TESTAPP and type:

  g++ -DDEF_TESTAPP DTWindow.cpp -o dtwindow -O3 -Wall -Isource -I ../../../../includes/core/ -L/usr/X11R6/lib -lX11 -lrt

To compile as library undefine DEF_TESTAPP and type:

  rm *.o
  rm *.a
  g++ -c DTWindow.cpp -O3 -Wall -I ../../../../includes/core/
  ar -q dtwindow.a DTWindow.o
  ranlib dtwindow.a

*/


//#define DEF_TESTAPP


#include "DTWindowPlatform.cpp"


CDTWindow::CDTWindow(bool /*flags*/, CDTDisplay* display) : CDTWindowPlatform(display)
{
}


CDTWindow::CDTWindow(CDTDisplay* display, const DT_STREAM_DESC* /*sd_menu*/) : CDTWindowPlatform(display)
{
}


CDTWindow::CDTWindow(CDTDisplay* display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* /*sd_menu*/) : CDTWindowPlatform(display)
{
	Open(title, w, h, flags);
}


CDTWindow::CDTWindow(CDTDisplay* display, const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* /*sd_menu*/) : CDTWindowPlatform(display)
{
	Open(title, x, y, w, h, flags);
}


CDTWindow::~CDTWindow()
{
	close();
}


CDTDisplay* CDTWindow::GetDisplay() const
{
	return Disp;
}


DT_SLONG CDTWindow::GetW() const
{
	return Width;
}


DT_SLONG CDTWindow::GetH() const
{
	return Height;
}


DT_SLONG CDTWindow::GetStatus() const
{
	if (IsOpen) return 1;
	return 0;
}


DT_SWORD CDTWindow::Modify(DT_ULONG flags)
{
	return modify(flags);
}


const DT_MDC* CDTWindow::GetMDC(DT_ULONG /*flags*/)
{
	return DV_NULL;
}


DT_SWORD CDTWindow::SetPixels(void* pixels, DT_ULONG flags)
{
#ifdef DEF_REFRESH_UPDATES_FULL_WINDOW
	if (flags & 2) return 1;
#endif
	if (update(pixels, flags)) return 1;
	return 0;
}


DT_SWORD CDTWindow::SetTimer(DT_SLONG value)
{
	if (value < 0) value = -value;
	modify_timer(value);
	return 1;
}


DT_SWORD CDTWindow::Refresh(DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags)
{
	if (w <= 0 || h <= 0) return 0;
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if (w <= 0 || h <= 0) return 0;

#ifdef DEF_REFRESH_UPDATES_FULL_WINDOW
	if (flags & 2) return 1;
	if (update(0, 0, Width, Height, flags)) return 1;
#else
	if (update(x, y, w, h, flags)) return 1;
#endif
	return 0;
}


DT_SWORD CDTWindow::Open(const DT_CHAR* title, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* /*sd_menu*/)
{
	if (w <= 0 || h <= 0) return 0;
	return open(title, x, y, w, h, flags);
}


DT_SWORD CDTWindow::Open(const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags, const DT_STREAM_DESC* /*sd_menu*/)
{
	return Open(title, -1, -1, w, h, flags);
}


void CDTWindow::Close()
{
	//shutdown();
	close();
}


void CDTWindow::onResize(DT_SLONG width, DT_SLONG height)
{
	//printf("onResize: width=%d, height=%d\n", width, height);

	Event_Resize(width, height);
}


void CDTWindow::onKeyDown(DT_SLONG key)
{
	//printf("onKeyDown: key=%d\n", key);

	Event_KeyDown(key);
}


void CDTWindow::onKeyPressed(DT_SLONG /*key*/)
{
	//printf("onKeyPressed: key=%s\n", key);

	//Event_KeyPressed(key);
}


void CDTWindow::onKeyUp(DT_SLONG key)
{
	//printf("onKeyUp: key=%s\n", key);

	Event_KeyUp(key);
}


void CDTWindow::onCharDown(DT_ULONG unicode_char)
{
	//printf("onCharDown: unicode_char=%ld\n", unicode_char);

	Event_CharDown(unicode_char);
}


void CDTWindow::onCharUp(DT_ULONG unicode_char)
{
	//printf("onCharUp: unicode_char=%ld\n", unicode_char);

	Event_CharUp(unicode_char);
}


void CDTWindow::onMouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y)
{
	//printf("onMouseButtonDown: buttons=%d,%d,%d x=%f, y=%f\n", mouse.buttons.left, mouse.buttons.middle, mouse.buttons.right, mouse.x, mouse.y);

	Event_MouseButtonDown(button, x, y);
}


void CDTWindow::onMouseMove(DT_SLONG x, DT_SLONG y)
{
	//printf("onMouseMove: buttons=%d,%d,%d x=%f, y=%f\n", mouse.buttons.left, mouse.buttons.middle, mouse.buttons.right, mouse.x, mouse.y);

	Event_MouseMove(x, y);
}


void CDTWindow::onMouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y)
{
	//printf("onMouseButtonUp: buttons=%d,%d,%d x=%f, y=%f\n", mouse.buttons.left, mouse.buttons.middle, mouse.buttons.right, mouse.x, mouse.y);

	Event_MouseButtonUp(button, x, y);
}


void CDTWindow::onMouseScroll(DT_SLONG x, DT_SLONG y, DT_SLONG dx, DT_SLONG dy)
{
	Event_MouseScroll(x, y, dx, dy);
}


void CDTWindow::onPeriodic()
{
	Event_Other(DV_WINDOW_OTHEREVENT_TIMER);
}


void CDTWindow::onActivate(bool active)
{
	if (active) Event_Other(DV_WINDOW_OTHEREVENT_ACTIVE);
	else Event_Other(DV_WINDOW_OTHEREVENT_INACTIVE);
}


DT_SLONG CDTWindow::Dialog_FileSelect(DT_SWORD /*save_flag*/, DT_SWORD /*config_flags*/, DT_SLONG /*selected_filter*/, const DT_CHAR* /*title_bar*/, const DT_CHAR* /*file_types*/, DT_CHAR* /*default_filename*/, DT_SLONG /*max_length*/)
{
	return -2; /* not implemented */
}


DT_SWORD CDTWindow::SetMenu(DT_SLONG /*operation*/, DT_SLONG /*command*/, const DT_CHAR* /*extra*/)
{
	return 0; /* not implemented */
}


#ifdef DEF_TESTAPP


class CMyWindow : public CDTWindow
{
public:

	CMyWindow(CDTDisplay* display) : CDTWindow(display) {}

protected: // callback

	void Event_KeyDown(DT_SLONG key)
	{
		printf("Event_KeyDown key=%ld\n", key);
	}

	void Event_MouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y)
	{
		printf("Event_MouseButtonDown button=%ld, x=%ld, y=%ld\n", button, x, y);
	}

	void Event_MouseMove(DT_SLONG x, DT_SLONG y)
	{
		printf("Event_MouseMove x=%ld, y=%ld\n", x, y);
	}

	void Event_MouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y)
	{
		printf("Event_MouseButtonUp button=%ld, x=%ld, y=%ld\n", button, x, y);
	}
};


int main()
{
	DT_SLONG width = 320;
	DT_SLONG height = 240;
	DT_SLONG pixel[320 * 240];
	DT_SLONG noise;
	DT_SLONG carry;
	DT_SLONG seed = 0x12345;


	CDTDisplay display(0);
	//CDTWindow window(&display);
	CMyWindow window(&display);
	window.Open("CMyWindow-A", width, height, 0);
	window.Close();
	window.Open("CMyWindow-B", width, height, DV_WINDOW_HINT_CENTER /*| DV_WINDOW_HINT_NOFRAME | DV_WINDOW_HINT_FULLSCREEN*/);

	for (DT_SLONG i = 0; i < 500; i++)
	{

		for (DT_SLONG index = 0; index < width * height; index++)
		{
			noise = seed;
			noise >>= 3;
			noise ^= seed;
			carry = noise & 1;
			noise >>= 1;
			seed >>= 1;
			seed |= (carry << 30);
			noise &= 0xFF;
			pixel[index] = (noise << 16) | (noise << 8) | noise;
		}

		//window1.SetPixels(pixel);
		//window1.EventLoop();

		window.SetPixels(pixel, 0 * 1);
		//window.EventLoop();

	}

	//window.Close();
	//while (window.GetStatus()) window.SetPixels(pixel, 0 * 1);
	while (window.GetStatus()) display.EventLoop();


	DT_CHAR buffer[1024];
	printf("Enter something: ");
	scanf("%s", buffer);

	return 1;
}


#endif

