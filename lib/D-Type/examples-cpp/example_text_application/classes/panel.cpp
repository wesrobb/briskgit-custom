/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"
#include "dtwindow.h"

#define DV_KEY_SHIFT 1
#define DV_KEY_CTRL 2
#define DV_KEY_CAPS 4
#define DV_KEY_ALT 32
#define DV_MBUTTON_LEFT 8
#define DV_MBUTTON_RIGHT 16

#include "keyboard.cpp"


class CDTPanel
{
private: // private copy constructor and assignment operator

	CDTPanel(const CDTPanel&);
	CDTPanel& operator = (const CDTPanel&);

protected:

	CDTPanel() : ParentWindow(DV_NULL), MDC(DV_NULL)
	{
		DC.m = DV_NULL;
		DC.w = DC.h = DC.l = 0;
		BkgByte = 255;
		//SetMargins(0, 0, 0, 0);
		XL = YT = XR = YB = 0;

		View.zoom = 1.0;
		View.page = 0;
		View.offx = 0;
		View.offy = 0;
	}

	virtual ~CDTPanel() {}

public:

	virtual void SetMargins(DT_SLONG xl, DT_SLONG yt, DT_SLONG xr, DT_SLONG yb)
	{
		if (xl < 0) xl = 0;
		if (yt < 0) yt = 0;
		if (xr < 0) xr = 0;
		if (yb < 0) yb = 0;

		XL = xl;
		YT = yt;
		XR = xr;
		YB = yb;
	}

	virtual void SetBkgByte(DT_SWORD bkg_byte)
	{
		BkgByte = bkg_byte;
	}

	virtual DT_FLOAT GetZoom()
	{
		return View.zoom;
	}

	virtual DT_SLONG GetPage()
	{
		return View.page;
	}

	virtual DT_FLOAT GetXOffset()
	{
		return View.offx;
	}

	virtual DT_FLOAT GetYOffset()
	{
		return View.offy;
	}

	virtual void SetZoom(DT_FLOAT zoom)
	{
		if (DF_ISEQUAL(zoom, View.zoom)) return;
		View.zoom = zoom;
	}

	virtual void SetPage(DT_SLONG page)
	{
		if (page == View.page) return;
		View.page = page;
	}

	virtual void SetOffX(DT_SLONG offx)
	{
		if (offx == View.offx) return;
		View.offx = offx;
	}

	virtual void SetOffY(DT_SLONG offy)
	{
		if (offy == View.offy) return;
		View.offy = offy;
	}

	inline CDTWindow* GetParentWindow()
	{
		return ParentWindow;
	}

	inline DT_SLONG GetW()
	{
		if (ParentWindow == DV_NULL) return 0;
		return ParentWindow->GetW() - XL - XR;
	}

	inline DT_SLONG GetH()
	{
		if (ParentWindow == DV_NULL) return 0;
		return ParentWindow->GetH() - YT - YB;
	}

	virtual DT_SLONG GetLastPage()
	{
		return 0;
	}

	virtual DT_SLONG GetFullW()
	{
		return GetW();
	}

	virtual DT_SLONG GetFullH()
	{
		return GetH();
	}

	virtual void Event_FocusReceived() {}

	virtual void Event_FocusLost() {}

	virtual void Event_KeyDown(DT_SLONG /*key*/, DT_ULONG /*flags*/) {}

	virtual void Event_KeyUp(DT_SLONG /*key*/, DT_ULONG /*flags*/) {}

	virtual void Event_MouseButtonDown(DT_SLONG /*button*/, DT_SLONG /*x*/, DT_SLONG /*y*/, DT_ULONG /*flags*/, DT_UBYTE /*clickcount*/) {}

	virtual void Event_MouseMove(DT_SLONG /*x*/, DT_SLONG /*y*/, DT_ULONG /*flags*/) {}

	virtual void Event_MouseButtonUp(DT_SLONG /*button*/, DT_SLONG /*x*/, DT_SLONG /*y*/, DT_ULONG /*flags*/) {}

	virtual void Event_WindowBind(CDTWindow* parent_window, const DT_MDC* mdc)
	{
		ParentWindow = parent_window;
		MDC = mdc;
	}

	virtual void Notify(DT_SWORD /*flag*/) {}

	virtual void Reset(DT_SWORD /*flag*/) {}

	void ClipView()
	{
		if (View.zoom < 0.20) View.zoom = 0.20;
		else if (View.zoom > 10.0) View.zoom = 10.0;
		if (View.page < 0) View.page = 0;
		else if (View.page > GetLastPage()) View.page = GetLastPage();

		DT_SLONG offx_max = DF_MAX(1, GetFullW() - GetW());
		DT_SLONG offy_max = DF_MAX(1, GetFullH() - GetH());

		if (View.offx < 0) View.offx = 0; else if (View.offx > offx_max) View.offx = offx_max;
		if (View.offy < 0) View.offy = 0; else if (View.offy > offy_max) View.offy = offy_max;
	}

	static DT_SWORD RectIntersect(DT_SLONG xmn1, DT_SLONG ymn1, DT_SLONG xmx1, DT_SLONG ymx1, DT_SLONG* xmn2, DT_SLONG* ymn2, DT_SLONG* xmx2, DT_SLONG* ymx2)
	{
		DT_SLONG w, h;
		DT_SLONG x3 = DF_MIN(xmx1, *xmx2);
		DT_SLONG y3 = DF_MIN(ymx1, *ymx2);
		*xmn2 = DF_MAX(xmn1, *xmn2); w = x3 - *xmn2;
		*ymn2 = DF_MAX(ymn1, *ymn2); h = y3 - *ymn2;
		*xmx2 = *xmn2 + w;
		*ymx2 = *ymn2 + h;
		return (w > 0 && h > 0);
	}

	virtual void Render()
	{
		Clear(BkgByte);
	}

	virtual bool RenderArea(DT_SLONG* xmn, DT_SLONG* ymn, DT_SLONG* xmx, DT_SLONG* ymx)
	{
		if (!RectIntersect(0, 0, MDC->w - XL - XR - 1, abs(MDC->h) - YT - YB - 1, xmn, ymn, xmx, ymx)) return false;
		ClearArea(BkgByte, *xmn, *ymn, *xmx - *xmn + 1, *ymx - *ymn + 1);
		return true;
	}

	void Redraw(bool clip, bool notify)
	{
		if (clip) ClipView();
		if (notify) Notify(0);

		Render();
		ParentWindow->Refresh(XL, YT, GetW(), GetH(), 0);
		//ParentWindow->SetPixels(MDC->m, 0);
	}

	void RedrawArea(bool clip, bool notify, DT_SLONG xmn, DT_SLONG ymn, DT_SLONG xmx, DT_SLONG ymx)
	{
		if (clip) ClipView();
		if (notify) Notify(0);

		xmn -= View.offx;
		ymn -= View.offy;
		xmx -= View.offx;
		ymx -= View.offy;

		if (!RenderArea(&xmn, &ymn, &xmx, &ymx)) return;
		ParentWindow->Refresh(xmn + XL, ymn + YT, xmx - xmn + 1, ymx - ymn + 1, 0);
	}

protected:

	inline void Clear(DT_SWORD bkg_byte)
	{
		ClearArea(bkg_byte, 0, 0, MDC->w - XL - XR, abs(MDC->h) - YT - YB);
	}

	void ClearArea(DT_SWORD bkg_byte, DT_SLONG xcl, DT_SLONG ycl, DT_SLONG wcl, DT_SLONG hcl)
	{
		DT_SLONG i;
		DT_UBYTE bkg;

		if (MDC == DV_NULL) return;
		if (MDC->m == DV_NULL) return;

		DT_SLONG pitch = SURFACE_BPP * MDC->w;
		DC = *MDC;

		xcl += XL;
		ycl += YT;

		if (DC.h < 0)
		{
			DC.m += pitch * ycl + SURFACE_BPP * xcl;
			DC.h = -hcl; if (DC.h > 0) DC.h = 0;
		}
		else
		{
			DC.m += pitch * (DC.h - hcl - ycl) + SURFACE_BPP * xcl;
			DC.h = hcl; if (DC.h < 0) DC.h = 0;
		}

		DC.w = wcl;
		if (DC.w < 0) DC.w = 0;

		DC.l = pitch * abs(DC.h);

		DT_UBYTE* m = DC.m;

		if (bkg_byte < 0 || bkg_byte > 255) return; // transparent background

		bkg = static_cast<DT_UBYTE>(bkg_byte);

		for (i = 0; i < abs(DC.h); i++)
		{
			dtMemSet(m, bkg, SURFACE_BPP * DC.w);
			m += pitch;
		}
	}

public:

	DT_SLONG XL, YT, XR, YB;
	CDTWindow* ParentWindow;

protected:

	const DT_MDC* MDC;
	DT_MDC DC;
	DT_SWORD BkgByte;

	struct
	{

		DT_FLOAT zoom;
		DT_SLONG page;
		DT_SLONG offx;
		DT_SLONG offy;

	} View;
};
