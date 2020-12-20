/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"
#include "dtwindow.h"


class CDTHandlerDrag
{
public:

	CDTHandlerDrag() {}
	virtual ~CDTHandlerDrag() {}

public:

	virtual void Event_DragChange(DT_SLONG /*x*/, DT_SLONG /*y*/) {}
};


class CDTPanelDrag : public CDTPanelStatic
{
public:

	CDTPanelDrag() : HandlerDrag(DV_NULL) { MouseButtonDown = false; }
	virtual ~CDTPanelDrag() {}

public:

	inline void SetHandler(CDTHandlerDrag* handler_drag) { HandlerDrag = handler_drag; }
	inline CDTHandlerDrag* GetHandler() { return HandlerDrag; }

	void Event_MouseButtonDown(DT_SLONG button, DT_SLONG /*x*/, DT_SLONG /*y*/, DT_ULONG /*flags*/, DT_UBYTE /*clickcount*/)
	{
		if (button != 0) return;
		MouseButtonDown = true;
	}

	void Event_MouseMove(DT_SLONG x, DT_SLONG y, DT_ULONG flags)
	{
		if (!(flags & DV_MBUTTON_LEFT)) return;
		if (!MouseButtonDown) return;

		DT_SLONG w = ParentWindow->GetW();
		DT_SLONG h = ParentWindow->GetH();

		x += XL; if (x < 0) x = 0; else if (x > w) x = w;
		y += YT; if (y < 0) y = 0; else if (y > h) y = h;

		if (HandlerDrag != DV_NULL) HandlerDrag->Event_DragChange(x, y);
	}

	void Event_MouseButtonUp(DT_SLONG button, DT_SLONG /*x*/, DT_SLONG /*y*/, DT_ULONG /*flags*/)
	{
		if (button != 0) return;
		MouseButtonDown = false;
	}

private:

	CDTHandlerDrag* HandlerDrag;
};
