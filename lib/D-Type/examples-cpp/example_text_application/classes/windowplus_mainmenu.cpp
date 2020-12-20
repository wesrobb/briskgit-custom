/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


class CDTHandlerForm_MainMenu : public CDTHandlerForm
{
public:

	CDTHandlerForm_MainMenu() : TextDoc1(DV_NULL), TextDoc2(DV_NULL), TextDoc3(DV_NULL), TextDoc4(DV_NULL), TextDoc5(DV_NULL), TextDoc6(DV_NULL) {}

	virtual ~CDTHandlerForm_MainMenu()
	{
		if (TextDoc1 != DV_NULL) delete TextDoc1;
		if (TextDoc2 != DV_NULL) delete TextDoc2;
		if (TextDoc3 != DV_NULL) delete TextDoc3;
		if (TextDoc4 != DV_NULL) delete TextDoc4;
		if (TextDoc5 != DV_NULL) delete TextDoc5;
		if (TextDoc6 != DV_NULL) delete TextDoc6;
	}

	void Event_Action(CDTPanel* /*parent_panel*/, DT_PDDOC power_doc, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* ctrl)
	{
		DT_ID_SLONG obj = ctrl->Obj;
		DT_ID_SLONG button_info = pdObjFindByNameAndDesc(power_doc, 10, DV_NULL);
		DT_ID_SLONG button_doc1 = pdObjFindByNameAndDesc(power_doc, 100, DV_NULL);
		DT_ID_SLONG button_doc2 = pdObjFindByNameAndDesc(power_doc, 200, DV_NULL);
		DT_ID_SLONG button_doc3 = pdObjFindByNameAndDesc(power_doc, 300, DV_NULL);
		DT_ID_SLONG button_doc4 = pdObjFindByNameAndDesc(power_doc, 400, DV_NULL);
		DT_ID_SLONG button_doc5 = pdObjFindByNameAndDesc(power_doc, 500, DV_NULL);
		DT_ID_SLONG button_doc6 = pdObjFindByNameAndDesc(power_doc, 600, DV_NULL);
		DT_SLONG w = MainDisplay->Dist(700);
		DT_SLONG h = MainDisplay->Dist(500);

		if (obj == button_info) /* info button */
		{
			if (MainDisplay->FocusExistingWindow(1001)) return;
			MainDisplay->RegisterNewWindow(new CDTWindowPlus_Info(MainDisplay), 1001, true);
		}
		else if (obj == button_doc1) /* text document 1 */
		{
			if (MainDisplay->FocusExistingWindow(101)) return;
			if (TextDoc1 == DV_NULL) TextDoc1 = new CDTTextDocDemo1(Engine, "../../files/text/unicode_utf-8/start.txt");
			MainDisplay->RegisterNewWindow(new CDTWindowPlusWithExtras_TextDoc(MainDisplay, TextDoc1, "Text Document 1", w, h), 101, true);
		}
		else if (obj == button_doc2) /* text document 2 */
		{
			if (MainDisplay->FocusExistingWindow(102)) return;
			if (TextDoc2 == DV_NULL) TextDoc2 = new CDTTextDocDemo2(Engine, "../../files/text/unicode_utf-8/chinese.txt");
			MainDisplay->RegisterNewWindow(new CDTWindowPlusWithExtras_TextDoc(MainDisplay, TextDoc2, "Text Document 2", w, h), 102, true);
		}
		else if (obj == button_doc3) /* text document 3 */
		{
			if (MainDisplay->FocusExistingWindow(103)) return;
			if (TextDoc3 == DV_NULL) TextDoc3 = new CDTTextDocDemo3(Engine, "../../files/text/unicode_utf-8/complex.txt");
			MainDisplay->RegisterNewWindow(new CDTWindowPlusWithExtras_TextDoc(MainDisplay, TextDoc3, "Text Document 3", w, h), 103, true);
		}
		else if (obj == button_doc4) /* text document 4 */
		{
			if (MainDisplay->FocusExistingWindow(104)) return;
			if (TextDoc4 == DV_NULL) TextDoc4 = new CDTTextDocDemo4(Engine, "../../files/text/unicode_utf-8/eiffel_tower.txt");
			MainDisplay->RegisterNewWindow(new CDTWindowPlusWithExtras_TextDoc(MainDisplay, TextDoc4, "Text Document 4", w, h), 104, true);
		}
		else if (obj == button_doc5) /* text document 5 */
		{
			if (MainDisplay->FocusExistingWindow(105)) return;
			if (TextDoc5 == DV_NULL) TextDoc5 = new CDTTextDocDemo5(Engine, "../../files/text/unicode_utf-8/latin_and_chinese.txt");
			MainDisplay->RegisterNewWindow(new CDTWindowPlusWithExtras_TextDoc(MainDisplay, TextDoc5, "Text Document 5", w, h), 105, true);
		}
		else if (obj == button_doc6) /* text document 6 */
		{
			if (MainDisplay->FocusExistingWindow(106)) return;
			if (TextDoc6 == DV_NULL) TextDoc6 = new CDTTextDocDemo6(Engine, "../../files/text/unicode_utf-8/mix.txt");
			MainDisplay->RegisterNewWindow(new CDTWindowPlusWithExtras_TextDoc(MainDisplay, TextDoc6, "Text Document 6", w, h), 106, true);
		}
	}

private:

	CDTTextDocDemo1* TextDoc1;
	CDTTextDocDemo2* TextDoc2;
	CDTTextDocDemo3* TextDoc3;
	CDTTextDocDemo4* TextDoc4;
	CDTTextDocDemo5* TextDoc5;
	CDTTextDocDemo6* TextDoc6;
};


class CDTWindowPlus_MainMenu : public CDTWindowPlus
{
public:

	CDTWindowPlus_MainMenu(CDTDisplay* display) : CDTWindowPlus(display, "D-Type Text Engine Demo App - Main Menu", display->Dist(600), display->Dist(480), DV_WINDOW_HINT_FULLRESOLUTION),
	 PanelForm(FormControlsLib)
	{
		PanelForm.Load(Engine->GetPdEngine(), "skin/default/form_mainmenu.pdc");
		PanelForm.SetMargins(10, 10, 10, 10);
		PanelForm.SetBkgByte(240);
		PanelForm.SetZoom(0.275 * display->GetScale());
		PanelForm.SetHandler(&MainMenuHandler);

		AddPanel(&PanelForm);
	}

private:

	CDTPanelForm PanelForm;
	CDTHandlerForm_MainMenu MainMenuHandler;
};
