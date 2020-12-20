/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"
#include "dtwindow.h"


class CDTPanelStatic : public CDTPanel
{
public:

	CDTPanelStatic() : PowerDoc(DV_NULL), MouseButtonDown(false) {}
	virtual ~CDTPanelStatic()
	{
		if (PowerDoc != DV_NULL) pdDocExt(PowerDoc);
	}

public:

	virtual void SetZoom(DT_FLOAT zoom)
	{
		CDTPanel::SetZoom(zoom);
		pdDocSetScale(PowerDoc, zoom);
	}

	void Render()
	{
		CDTPanel::Render();
		pdDocSetScale(PowerDoc, View.zoom);
		pdDocDraw(PowerDoc, View.page, View.offx, View.offy, SURFACE_FORMAT, 0, &DC, DV_NULL);
	}

	bool RenderArea(DT_SLONG* xmn, DT_SLONG* ymn, DT_SLONG* xmx, DT_SLONG* ymx)
	{
		if (!CDTPanel::RenderArea(xmn, ymn, xmx, ymx)) return false;
		pdDocSetScale(PowerDoc, View.zoom);
		pdDocDraw(PowerDoc, View.page, View.offx + *xmn, View.offy + *ymn, SURFACE_FORMAT, 0, &DC, DV_NULL);
		return true;
	}

	void Load(DT_PDENGINE power_engine, const DT_CHAR* filename)
	{
		if (PowerDoc != DV_NULL)
		{
			pdDocExt(PowerDoc);
			PowerDoc = DV_NULL;
		}

		if (pdDocIni(&PowerDoc, power_engine) == 0) return;

		DT_STREAM_FILE(sd, filename);
		pdDocAppendFromStream(PowerDoc, 0, 0, 0, &sd);
	}

	DT_PDDOC GetPowerDoc() { return PowerDoc; }

protected:

	DT_PDDOC PowerDoc;
	bool MouseButtonDown;
};
