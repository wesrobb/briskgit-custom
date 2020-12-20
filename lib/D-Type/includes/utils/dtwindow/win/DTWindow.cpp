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

To compile as a test application define DEF_TESTAPP

*/


//#define DEF_TESTAPP


#include "DTWindowPlatform.cpp"


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

