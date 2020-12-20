/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"
#include "dtwindow.h"

#include "dtform/DTFormEngine.cpp"


class CDTHandlerForm
{
public:

	CDTHandlerForm() {}
	virtual ~CDTHandlerForm() {}

public:

	virtual void Event_Action(CDTPanel* /*parent_panel*/, DT_PDDOC /*power_doc*/, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* /*ctrl*/) {}
	virtual void Event_PreAction(CDTPanel* /*parent_panel*/, DT_PDDOC /*power_doc*/, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* /*ctrl*/) {}
	virtual void Event_PostAction(CDTPanel* /*parent_panel*/, DT_PDDOC /*power_doc*/, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* /*ctrl*/) {}
	virtual void UpdatePanel(bool /*redraw*/) {}
};


class CDTFormEngineWithPanel : public CDTFormEngine
{
public: // Constructor and Destructor

	CDTFormEngineWithPanel(CDTFormControlsLib* form_controls_lib, CDTPanelStatic* panel) : CDTFormEngine(form_controls_lib, panel->GetPowerDoc()), Panel(panel), Handler(DV_NULL) {}
	virtual ~CDTFormEngineWithPanel() {}

public:

	CDTHandlerForm* GetHandler() { return Handler; }
	void SetHandler(CDTHandlerForm* handler) { Handler = handler; }

protected: // Overrides

	void ShowStatus(const DT_CHAR* /*status_text*/) {}

	void Redraw(bool /*frames_have_changed*/)
	{
		Panel->Redraw(false, false);
	}

	void RedrawArea(DT_SLONG xcl, DT_SLONG ycl, DT_SLONG wcl, DT_SLONG hcl)
	{
		Panel->RedrawArea(false, false, xcl, ycl, xcl + wcl - 1, ycl + hcl - 1);
	}

	void Repage(DT_SLONG page)
	{
		Panel->SetPage(page);
		Panel->Redraw(false, false);
	}

	DT_SLONG GetCurrentPage()
	{
		return Panel->GetPage();
	}

	void Event_PreAction(DT_PDDOC power_doc, DT_SLONG signal_id, DT_UBYTE handler, const CDTControl* ctrl)
	{
		Handler->Event_PreAction(Panel, power_doc, signal_id, handler, ctrl);
	}

	void Event_Action(DT_PDDOC power_doc, DT_SLONG signal_id, DT_UBYTE handler, const CDTControl* ctrl)
	{
		Handler->Event_Action(Panel, power_doc, signal_id, handler, ctrl);
	}

	void Event_PostAction(DT_PDDOC power_doc, DT_SLONG signal_id, DT_UBYTE handler, const CDTControl* ctrl)
	{
		Handler->Event_PostAction(Panel, power_doc, signal_id, handler, ctrl);
	}

private:

	CDTPanel* Panel;
	CDTHandlerForm* Handler;
};


class CDTPanelForm : public CDTPanelStatic
{
public: // Constructor and Destructor

	CDTPanelForm(CDTFormControlsLib* form_controls_lib) : FormControlsLib(form_controls_lib), FormEngine(DV_NULL) {}
	virtual ~CDTPanelForm()
	{
		if (FormEngine != DV_NULL) delete FormEngine;
	}

public:

	void Load(DT_PDENGINE power_engine, const DT_CHAR* filename)
	{
		CDTPanelStatic::Load(power_engine, filename);

		if (FormEngine != DV_NULL)
		{
			delete FormEngine;
			FormEngine = DV_NULL;
		}

		FormEngine = new CDTFormEngineWithPanel(FormControlsLib, this);

		pdDocSetDrawCallback(GetPowerDoc(), CDTFormEngine::UserObjDrawStatic);
	}

	CDTHandlerForm* GetHandler()
	{
		if (FormEngine == DV_NULL) return DV_NULL;
		return FormEngine->GetHandler();
	}

	void SetHandler(CDTHandlerForm* handler)
	{
		if (FormEngine == DV_NULL) return;
		FormEngine->SetHandler(handler);
	}

public: // Events

	void Event_FocusLost()
	{
		if (FormEngine == DV_NULL) return;
		FormEngine->Reset(true);
	}

	void Event_KeyDown(DT_SLONG key, DT_ULONG flags)
	{
		DT_ULONG shift = flags & DV_KEY_SHIFT;
		DT_ULONG ctrl = flags & DV_KEY_CTRL;

		if (FormEngine == DV_NULL) return;

		else if (key == 37 /*LEFT*/) FormEngine->EvnF_KeyCtrl(shift ? TX_CMD_SHIFT_LEFT : TX_CMD_LEFT);
		else if (key == 39 /*RIGHT*/) FormEngine->EvnF_KeyCtrl(shift ? TX_CMD_SHIFT_RIGHT : TX_CMD_RIGHT);
		else if (key == 38 /*UP*/) FormEngine->EvnF_KeyCtrl(shift ? TX_CMD_SHIFT_UP : TX_CMD_UP);
		else if (key == 40 /*DOWN*/) FormEngine->EvnF_KeyCtrl(shift ? TX_CMD_SHIFT_DOWN : TX_CMD_DOWN);

		//else if (key == 9 /*TAB*/) FormEngine->EvnF_KeyCtrl(shift ? TX_CMD_PREVCHAR : TX_CMD_NEXTCHAR);
		else if (key == 9 /*TAB*/) FormEngine->EvnF_KeyCtrl(shift ? FE_CMD_PREV : FE_CMD_NEXT);

		else if (ctrl) {}

		else if (key == 8 /*BAKSPACE*/) FormEngine->EvnF_KeyChar(TX_PASTE_BACKSPACE, DV_NULL, 0, TX_BUFFER_FORMAT_1);
		else if (key == 46 /*DELETE*/) FormEngine->EvnF_KeyChar(TX_PASTE_DELETE, DV_NULL, 0, TX_BUFFER_FORMAT_1);
		else
		{
			DT_UBYTE k = static_cast<DT_UBYTE>(KeyToChar(key, flags, 0));
			if (k != 0) FormEngine->EvnF_KeyChar(TX_PASTE_TEXT, &k, 1, TX_BUFFER_FORMAT_1);
		}
	}

	void Event_MouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y, DT_ULONG flags, DT_UBYTE clickcount)
	{
		if (button != 0) return;
		MouseButtonDown = true;

		DT_ID_SWORD click_type = TX_HIT_NORMAL;
		if (clickcount == 3) click_type = TX_HIT_TRIPLE;
		else if (clickcount == 2) click_type = TX_HIT_DOUBLE;
		else if (flags & DV_KEY_SHIFT) click_type = TX_HIT_SHIFT;
		else if ((flags & DV_KEY_CTRL) && (flags & DV_KEY_ALT)) click_type = TX_HIT_CTRLALT;
		else if (flags & DV_KEY_CTRL) click_type = TX_HIT_CTRL;
		else if (flags & DV_KEY_ALT) click_type = TX_HIT_ALT;

		if (FormEngine == DV_NULL) return;
		FormEngine->EvnF_MouseLPush(x + View.offx, y + View.offy, click_type);
	}

	void Event_MouseMove(DT_SLONG x, DT_SLONG y, DT_ULONG flags)
	{
		if (!(flags & DV_MBUTTON_LEFT)) return;
		if (!MouseButtonDown) return;

		if (FormEngine == DV_NULL) return;
		FormEngine->EvnF_MouseMove(x + View.offx, y + View.offy);
	}

	void Event_MouseButtonUp(DT_SLONG button, DT_SLONG /*x*/, DT_SLONG /*y*/, DT_ULONG /*flags*/)
	{
		if (button != 0) return;
		MouseButtonDown = false;

		if (FormEngine == DV_NULL) return;
		FormEngine->EvnF_MouseLRelease();
	}

	void Render()
	{
		CDTPanel::Render();

		if (FormEngine == DV_NULL) return;

		FormEngine->Render(true, View.offx, View.offy, SURFACE_FORMAT, 0, &DC);
	}

	bool RenderArea(DT_SLONG* xmn, DT_SLONG* ymn, DT_SLONG* xmx, DT_SLONG* ymx)
	{
		if (!CDTPanel::RenderArea(xmn, ymn, xmx, ymx)) return false;

		if (FormEngine == DV_NULL) return false;
		FormEngine->Render(false, *xmn + View.offx, *ymn + View.offy, SURFACE_FORMAT, 0, &DC);

		return true;
	}

private:

	CDTFormControlsLib* FormControlsLib;
	CDTFormEngineWithPanel* FormEngine;
};
