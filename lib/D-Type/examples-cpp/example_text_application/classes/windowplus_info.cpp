/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


class CDTWindowPlus_Info : public CDTWindowPlus
{
public:

	CDTWindowPlus_Info(CDTDisplay* display) : CDTWindowPlus(display, "Info Window", display->Dist(390), display->Dist(330), DV_WINDOW_HINT_FULLRESOLUTION)
	{
		PanelStatic.Load(Engine->GetPdEngine(), "skin/default/static_info.pdc");
		PanelStatic.SetMargins(10, 10, 10, 10);
		PanelStatic.SetBkgByte(240);
		PanelStatic.SetZoom(display->GetScale());

		AddPanel(&PanelStatic);
	}

private:

	CDTPanelStatic PanelStatic;
};
