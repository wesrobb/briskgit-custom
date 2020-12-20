/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"
#include "dtwindow.h"


#define DV_PANEL_MAX 50


class CDTWindowPlus : public CDTWindow
{
private: // private copy constructor and assignment operator

	CDTWindowPlus(const CDTWindowPlus&);
	CDTWindowPlus& operator = (const CDTWindowPlus&);

protected: /* Constructor and destructor */

	CDTWindowPlus(CDTDisplay* display) : CDTWindow(display)
	{
		RemoveAllPanels();
		BkgByte = 255;
		MDC.m = DV_NULL;
		MouseX = MouseY = -1;
		Time0MButtonLeft = Time0MButtonRight = 0;
		Time1MButtonLeft = Time1MButtonRight = 0;
	}

	CDTWindowPlus(CDTDisplay* display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags) : CDTWindow(display, title, w, h, flags)
	{
		RemoveAllPanels();
		BkgByte = 255;
		MDC.m = DV_NULL;
		MouseX = MouseY = -1;
		Time0MButtonLeft = Time0MButtonRight = 0;
		Time1MButtonLeft = Time1MButtonRight = 0;
		SetSize(w, h, false);
	}

public:

	virtual ~CDTWindowPlus()
	{
		Free();
	}

public:

	DT_SWORD Open(const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags)
	{
		SetSize(w, h, false);
		return CDTWindow::Open(title, w, h, flags);
	}

	void Close()
	{
		SetSize(1, 1, false);
		CDTWindow::Close();
	}

	void SetBkgByte(DT_SWORD bkg_byte)
	{
		BkgByte = bkg_byte;
	}

	DT_SWORD AddPanel(CDTPanel* panel)
	{
		if (PanelCount >= DV_PANEL_MAX) return 0;
		Panel[PanelCount] = panel;
		Panel[PanelCount]->Event_WindowBind(this, &MDC);
		PanelCount++;
		return 1;
	}

	void RemoveAllPanels()
	{
		PanelCount = 0;
		PanelActive = -1;
		Flags = 0;
	}

	//virtual void Event_Command(DT_ULONG command_id, DT_SLONG param_1, void* param_2) {}

protected: // Event-driven callbacks

	virtual void Event_WindowResize(DT_SLONG /*w*/, DT_SLONG /*h*/) {}

	virtual void Event_PanelActive(CDTPanel* /*panel*/, DT_SLONG /*panel_index*/) {}

	void Event_Resize(DT_SLONG w, DT_SLONG h)
	{
		Event_WindowResize(w, h);
		SetSize(w, h, true);

		if (PanelActive < 0) return;
		Event_PanelActive(Panel[PanelActive], PanelActive);
	}

	void Event_KeyDown(DT_SLONG key)
	{
		if (key == 16) Flags |= DV_KEY_SHIFT;
		else if (key == 17) Flags |= DV_KEY_CTRL;
		else if (key == 18) Flags |= DV_KEY_ALT;
		else if (key == 20)
		{
			if (Flags & DV_KEY_CAPS) Flags &= ~DV_KEY_CAPS; else Flags |= DV_KEY_CAPS;
		}

		if (PanelActive < 0) return;
		Panel[PanelActive]->Event_KeyDown(key, Flags);
	}

	void Event_KeyUp(DT_SLONG key)
	{
		if (key == 16) Flags &= ~DV_KEY_SHIFT;
		else if (key == 17) Flags &= ~DV_KEY_CTRL;
		else if (key == 18) Flags &= ~DV_KEY_ALT;

		if (PanelActive < 0) return;
		Panel[PanelActive]->Event_KeyUp(key, Flags);
	}

	void Event_MouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y)
	{
		DT_FLOAT clocks_per_sec = CLOCKS_PER_SEC;
		DT_UBYTE clickcount = 1;
		clock_t c = clock();

		if (button == 0)
		{
			if ((c - Time1MButtonLeft) / clocks_per_sec < 0.375) { clickcount = 3; Time0MButtonLeft = Time1MButtonLeft = 0; } /* third click */
			else if ((c - Time0MButtonLeft) / clocks_per_sec < 0.375) { clickcount = 2; Time1MButtonLeft = c; } /* second click */
			else Time1MButtonLeft = 0;

			Flags |= DV_MBUTTON_LEFT;
			Time0MButtonLeft = c;
		}
		else if (button == 1)
		{
			if ((c - Time1MButtonRight) / clocks_per_sec < 0.375) { clickcount = 3; Time0MButtonRight = Time1MButtonRight = 0; } /* third click */
			else if ((c - Time0MButtonRight) / clocks_per_sec < 0.375) { clickcount = 2; Time1MButtonRight = c; } /* second click */
			else Time1MButtonRight = 0;

			Flags |= DV_MBUTTON_RIGHT;
			Time0MButtonRight = c;
		}

		for (DT_SLONG i = 0; i < PanelCount; i++)
		{
			DT_SLONG xt = x, yt = y;
			if (!PointInsidePanel(Panel[i], &xt, &yt)) continue;

			if (PanelActive != i)
			{
				if (PanelActive >= 0) Panel[PanelActive]->Event_FocusLost();

				PanelActive = i;
				Event_PanelActive(Panel[PanelActive], PanelActive);

				Panel[PanelActive]->Event_FocusReceived();
			}

			Panel[i]->Event_MouseButtonDown(button, xt, yt, Flags, clickcount);
			return;
		}
	}

	void Event_MouseMove(DT_SLONG x, DT_SLONG y)
	{
		if (x == MouseX && y == MouseY) return;

		MouseX = x;
		MouseY = y;

		Time0MButtonLeft = Time0MButtonRight = 0;
		Time1MButtonLeft = Time1MButtonRight = 0;

		if (PanelActive >= 0)
		{
			DT_SLONG xt = x, yt = y;
			PointInsidePanel(Panel[PanelActive], &xt, &yt);
			Panel[PanelActive]->Event_MouseMove(xt, yt, Flags);
			return;
		}
	}

	void Event_MouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y)
	{
		if (button == 0) Flags &= ~DV_MBUTTON_LEFT; else if (button == 1) Flags &= ~DV_MBUTTON_RIGHT;

		if (PanelActive >= 0)
		{
			DT_SLONG xt = x, yt = y;
			PointInsidePanel(Panel[PanelActive], &xt, &yt);
			Panel[PanelActive]->Event_MouseButtonUp(button, xt, yt, Flags);
			return;
		}
	}

public:

	void Redraw()
	{
		Render();
		SetPixels(MDC.m, 0);
	}

private:

	CDTPanel* Panel[DV_PANEL_MAX];
	DT_SLONG PanelCount;
	DT_SLONG PanelActive;
	DT_ULONG Flags;
	DT_SWORD BkgByte;
	DT_MDC MDC;

	DT_SLONG MouseX, MouseY;
	clock_t Time0MButtonLeft, Time0MButtonRight;
	clock_t Time1MButtonLeft, Time1MButtonRight;

private:

	void Free()
	{
		if (MDC.m != DV_NULL) dtMemFree(MDC.m);
		MDC.m = DV_NULL;
		MDC.w = MDC.h = MDC.l = 0;
	}

	DT_SWORD PointInsidePanel(CDTPanel* panel, DT_SLONG* x, DT_SLONG* y)
	{
		DT_SWORD ret = 0;
		if (*x < panel->XL) goto end;
		if (*y < panel->YT) goto end;
		if (*x > MDC.w - panel->XR) goto end;
		if (*y > abs(MDC.h) - panel->YB) goto end;
		ret = 1;
	end:
		*x = *x - panel->XL;
		*y = *y - panel->YT;
		return ret;
	}

	void Render()
	{
		if (MDC.m == DV_NULL) return;

		if (BkgByte >= 0 && BkgByte <= 255)
		{
			DT_UBYTE bkg = static_cast<DT_UBYTE>(BkgByte);
			dtMemSet(MDC.m, bkg, MDC.l);
		}

		for (DT_SLONG i = 0; i < PanelCount; i++) Panel[i]->Render();
	}

	void SetSize(DT_SLONG w, DT_SLONG h, bool redraw)
	{
		DT_UBYTE* m;

		if (w < 1) w = 1;
		if (h < 1) h = 1;

		// Create memory surface
		MDC.w = w;
	#ifdef WIN32 /* flip the surface vertically */
		MDC.h = h;
	#else
		MDC.h = -h;
	#endif
		MDC.l = w * h * SURFACE_BPP;

		if ((m = static_cast<DT_UBYTE*>(dtMemRealloc(MDC.m, MDC.l))) == DV_NULL)
		{
			Free();
			return;
		}
		MDC.m = m;

		if (redraw) Redraw();
	}
};


class CDTWindowPlusWithExtras : public CDTWindowPlus
{
protected: /* Constructor and destructor */

	CDTWindowPlusWithExtras(CDTDisplay* display) : CDTWindowPlus(display) {}
	CDTWindowPlusWithExtras(CDTDisplay* display, const DT_CHAR* title, DT_SLONG w, DT_SLONG h, DT_ULONG flags) : CDTWindowPlus(display, title, w, h, flags) {}
	virtual ~CDTWindowPlusWithExtras() {}

public:

	virtual void SetZoom(DT_FLOAT /*zoom*/) {}
	virtual DT_FLOAT GetZoom() const { return 1.0; }

	virtual void SetLayout(DT_SLONG /*nh*/, DT_SLONG /*nv*/) {}
	virtual void GetLayout(DT_SLONG* nh, DT_SLONG* nv) const { *nh = 0; *nv = 0; }

	virtual void SetSize_ActiveHorPanels(DT_SLONG /*hor_d*/) {}
	virtual void SetSize_ActiveVerPanels(DT_SLONG /*ver_d*/) {}
};
