/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


class CDTHandlerForm_Clipboard : public CDTHandlerForm
{
public:

	CDTHandlerForm_Clipboard(CDTPanelForm* /*panel_form*/) /*: PanelForm(panel_form)*/ {}

public:

	void Event_Action(CDTPanel* parent_panel, DT_PDDOC power_doc, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* ctrl)
	{
		DT_ID_SLONG obj = ctrl->Obj;
		DT_ID_SLONG button_close = pdObjFindByNameAndDesc(power_doc, 1, DV_NULL);

		if (obj == button_close)
		{
			parent_panel->GetParentWindow()->Close();
			return;
		}
	}

//private:
//
//	CDTPanelForm* PanelForm;
};


class CDTWindowPlus_Clipboard : public CDTWindowPlus
{
public:

	CDTWindowPlus_Clipboard(CDTDisplay* display, const DT_UBYTE* unicode_buffer, DT_SLONG buffer_len) : CDTWindowPlus(display, "Clipboard Window", display->Dist(660), display->Dist(520), DV_WINDOW_HINT_FULLRESOLUTION),
	 PanelForm(FormControlsLib),
	 ClipboardHandler(&PanelForm)
	{
		PanelForm.Load(Engine->GetPdEngine(), "skin/default/form_clipboard.pdc");
		PanelForm.SetMargins(10, 10, 10, 10);
		PanelForm.SetBkgByte(240);
		PanelForm.SetZoom(0.275 * display->GetScale());
		PanelForm.SetHandler(&ClipboardHandler);

		AddPanel(&PanelForm);

		DT_ID_SLONG textbox = pdObjFindByNameAndDesc(PanelForm.GetPowerDoc(), 101, DV_NULL);
		pdPropSet(PanelForm.GetPowerDoc(), textbox, fcValue_16, unicode_buffer, buffer_len);
	}

private:

	CDTPanelForm PanelForm;
	CDTHandlerForm_Clipboard ClipboardHandler;
};
