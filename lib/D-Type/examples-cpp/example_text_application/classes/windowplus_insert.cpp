/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


class CDTHandlerForm_Insert : public CDTHandlerForm
{
public:

	CDTHandlerForm_Insert(CDTPanelForm* /*panel_form*/) : PanelText(DV_NULL) /*, PanelForm(panel_form)*/ {}

public:

	inline void SetPanelText(CDTPanelText* panel_text)
	{
		PanelText = panel_text;
	}

	void Event_Action(CDTPanel* parent_panel, DT_PDDOC power_doc, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* ctrl)
	{
		DT_ID_SLONG obj = ctrl->Obj;
		DT_ID_SLONG button_close = pdObjFindByNameAndDesc(power_doc, 1, DV_NULL);
		DT_ID_SLONG button_insert = pdObjFindByNameAndDesc(power_doc, 2, DV_NULL);

		if (obj == button_insert)
		{
			const DT_UBYTE* addr;
			DT_UBYTE* unicode_buffer;
			DT_CHAR str[2049] = "";
			DT_SLONG len = 0;
			DT_UBYTE dynamic = 0;
			DT_ID_SLONG textbox = pdObjFindByNameAndDesc(power_doc, 101, DV_NULL);

			if ((addr = pdPropGet(power_doc, textbox, fcValue_08, &len)) != DV_NULL && len < 2048)
			{
				dtMemCopy(str, addr, len);
				str[len] = 0;
			}

			if ((unicode_buffer = pdMakeBinary(str, PD_ARR_WORD_UNSIGNED, DV_NULL, &len, &dynamic)) == DV_NULL) return;

			PanelText->Event_TextPaste(TX_PASTE_TEXT_REFORMAT, TX_UCS2_LE, unicode_buffer, len);

			pdFree(unicode_buffer);
		}
		else if (obj == button_close)
		{
			parent_panel->GetParentWindow()->Close();
			return;
		}
	}

private:

	CDTPanelText* PanelText;
	//CDTPanelForm* PanelForm;
};


class CDTWindowPlus_Insert : public CDTWindowPlus
{
public:

	CDTWindowPlus_Insert(CDTDisplay* display, CDTPanelText* panel_text) : CDTWindowPlus(display, "Unicode Text Insert Window", display->Dist(660), display->Dist(330), DV_WINDOW_HINT_FULLRESOLUTION),
	 PanelForm(FormControlsLib),
	 InsertHandler(&PanelForm)
	{
		PanelForm.Load(Engine->GetPdEngine(), "skin/default/form_insert.pdc");
		PanelForm.SetMargins(10, 10, 10, 10);
		PanelForm.SetBkgByte(240);
		PanelForm.SetZoom(0.275 * display->GetScale());
		PanelForm.SetHandler(&InsertHandler);

		AddPanel(&PanelForm);

		InsertHandler.SetPanelText(panel_text);
	}

private:

	CDTPanelForm PanelForm;
	CDTHandlerForm_Insert InsertHandler;
};
