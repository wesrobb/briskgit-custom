/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


class CDTHandlerForm_Settings : public CDTHandlerForm
{
public:

	CDTHandlerForm_Settings(CDTPanelForm* panel_form) : ParentWindow(DV_NULL), PanelForm(panel_form) {}

public:

	inline void SetParentWindow(CDTWindowPlusWithExtras* parent_window)
	{
		ParentWindow = parent_window;
	}

	void Event_PostAction(CDTPanel* /*parent_panel*/, DT_PDDOC power_doc, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* ctrl)
	{
		if (ParentWindow == DV_NULL) return;

		DT_ID_SLONG obj = ctrl->Obj;
		DT_ID_SLONG slider_zoom = pdObjFindByNameAndDesc(power_doc, 200, DV_NULL);

		if (obj == slider_zoom)
		{
			DT_SLONG position = pdPropGetAsSLong(power_doc, slider_zoom, fcSliderPosition, 0);

			ParentWindow->SetZoom((position + 20) * 0.01);
			ParentWindow->Redraw();
		}
	}

	void Event_Action(CDTPanel* parent_panel, DT_PDDOC power_doc, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* ctrl)
	{
		const DT_UBYTE* addr;
		DT_CHAR str[256];
		DT_SLONG len;
		DT_SLONG i, nh = 1, nv = 1;
		DT_ID_SLONG radio_i;
		DT_ID_SLONG obj = ctrl->Obj;
		DT_ID_SLONG button_close = pdObjFindByNameAndDesc(power_doc, 1, DV_NULL);
		DT_ID_SLONG label_zoom = pdObjFindByNameAndDesc(power_doc, 201, DV_NULL);

		if (obj == button_close)
		{
			parent_panel->GetParentWindow()->Close();
			return;
		}
		else if (obj == label_zoom)
		{
			DT_SLONG position = 0;
			if ((addr = pdPropGet(power_doc, label_zoom, fcValue_08, &len)) != DV_NULL && len < 255)
			{
				dtMemCopy(str, addr, len);
				str[len] = 0;
				position = atol(str);
			}
			if (position < 0) position = 0;
			if (position > 50) position = 50;

			ParentWindow->SetZoom((position + 20) * 0.01);
			UpdatePanel(true);

			ParentWindow->Redraw();
		}

		if (ParentWindow == DV_NULL) return;

		/* Check for radio buttons */
		for (i = 101; i <= 109; i++, nh++)
		{
			if (nh > 3) { nh = 1; nv++; }

			radio_i = pdObjFindByNameAndDesc(power_doc, i, DV_NULL);
			if (pdPropGetAsUByte(power_doc, radio_i, fcState, 0) != 2) continue;

			ParentWindow->SetLayout(nh, nv);
			ParentWindow->Redraw();
			return;
		}
	}

	bool UpdatePanel()
	{
		if (ParentWindow == DV_NULL) return false;

		DT_CHAR buff[255];
		DT_PDDOC power_doc = PanelForm->GetPowerDoc();

		DT_SLONG h, v;
		DT_FLOAT zoom = ParentWindow->GetZoom();
		DT_SLONG position = DF_ROUND(zoom * 100 - 20);
		ParentWindow->GetLayout(&h, &v);

		DT_ID_SLONG slider_zoom = pdObjFindByNameAndDesc(power_doc, 200, DV_NULL);
		DT_ID_SLONG label_zoom = pdObjFindByNameAndDesc(power_doc, 201, DV_NULL);
		DT_ID_SLONG radio_i;
		DT_SLONG i, nh = 1, nv = 1;
		DT_UBYTE state = 0;

		sprintf(buff, "%.0f", static_cast<DT_FLOAT>(position));
		pdPropSet(power_doc, label_zoom, fcValue_08, reinterpret_cast<const DT_UBYTE*>(buff), static_cast<DT_SLONG>(strlen(buff)));
		pdPropApplyAsSLong(power_doc, slider_zoom, fcSliderPosition, position, 0, PD_APPLY_STANDARD);

		for (i = 101; i <= 109; i++, nh++)
		{
			if (nh > 3) { nh = 1; nv++; }

			if (h == nh && v == nv) state = 2; else state = 0;

			radio_i = pdObjFindByNameAndDesc(power_doc, i, DV_NULL);
			pdPropApplyAsUByte(power_doc, radio_i, fcState, state, 0, PD_APPLY_STANDARD);
		}

		return true;
	}

	void UpdatePanel(bool redraw)
	{
		if (!UpdatePanel()) return;

		if (redraw) PanelForm->Redraw(false, false);
		else PanelForm->Render();
	}

private:

	CDTWindowPlusWithExtras* ParentWindow;
	CDTPanelForm* PanelForm;
};


class CDTWindowPlus_Settings : public CDTWindowPlus
{
public:

	CDTWindowPlus_Settings(CDTDisplay* display, CDTWindowPlusWithExtras* parent_window) : CDTWindowPlus(display, "Settings Window", display->Dist(390), display->Dist(330), DV_WINDOW_HINT_FULLRESOLUTION),
	 PanelForm(FormControlsLib),
	 SettingsHandler(&PanelForm)
	{
		PanelForm.Load(Engine->GetPdEngine(), "skin/default/form_settings.pdc");
		PanelForm.SetMargins(10, 10, 10, 10);
		PanelForm.SetBkgByte(240);
		PanelForm.SetZoom(0.275 * display->GetScale());
		PanelForm.SetHandler(&SettingsHandler);

		AddPanel(&PanelForm);

		SettingsHandler.SetParentWindow(parent_window);
		SettingsHandler.UpdatePanel();
	}

private:

	CDTPanelForm PanelForm;
	CDTHandlerForm_Settings SettingsHandler;
};
