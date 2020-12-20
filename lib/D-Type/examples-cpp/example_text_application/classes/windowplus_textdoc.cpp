/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


inline DT_SLONG ZOOM_TO_POSITION(DT_SLONG zoom) { return DF_ROUND(sqrt((zoom - 20) / 0.00098)); }
inline DT_SLONG POSITION_TO_ZOOM(DT_SLONG position) { return DF_ROUND(position * position * 0.00098 + 20); }


class CDTHandlerForm_Toolbar : public CDTHandlerForm
{
public:

	CDTHandlerForm_Toolbar(CDTPanelForm* panel_form_toolbar) : ParentWindow(DV_NULL), PanelFormToolbar(panel_form_toolbar), PanelText(DV_NULL) {}

public:

	inline void SetParentWindow(CDTWindowPlusWithExtras* parent_window)
	{
		ParentWindow = parent_window;
	}

	inline void SetPanelText(CDTPanelText* panel_text)
	{
		PanelText = panel_text;
	}

	void Event_Action(CDTPanel* /*parent_panel*/, DT_PDDOC power_doc, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* ctrl)
	{
		if (PanelText == DV_NULL) return;

		const DT_UBYTE* addr;
		DT_CHAR str[256];
		DT_SLONG len;
		DT_ID_SLONG obj = ctrl->Obj;
		DT_ID_SLONG label_zoom = pdObjFindByNameAndDesc(power_doc, 201, DV_NULL);
		DT_ID_SLONG slider_zoom = pdObjFindByNameAndDesc(power_doc, 200, DV_NULL);
		DT_ID_SLONG label_page = pdObjFindByNameAndDesc(power_doc, 101, DV_NULL);
		DT_ID_SLONG button_page_prev = pdObjFindByNameAndDesc(power_doc, 102, DV_NULL);
		DT_ID_SLONG button_page_next = pdObjFindByNameAndDesc(power_doc, 103, DV_NULL);
		DT_ID_SLONG button_info = pdObjFindByNameAndDesc(power_doc, 500, DV_NULL);
		DT_ID_SLONG button_more = pdObjFindByNameAndDesc(power_doc, 600, DV_NULL);
		DT_ID_SLONG button_clipboard = pdObjFindByNameAndDesc(power_doc, 400, DV_NULL);
		DT_ID_SLONG button_insert = pdObjFindByNameAndDesc(power_doc, 401, DV_NULL);

		if (obj == label_zoom)
		{
			DT_SLONG zoom = 0;
			if ((addr = pdPropGet(power_doc, label_zoom, fcValue_08, &len)) != DV_NULL && len < 255)
			{
				dtMemCopy(str, addr, len);
				str[len] = 0;
				zoom = atol(str);
			}
			pdPropSet(power_doc, label_zoom, fcValue_08, DV_NULL, 0);

			PanelText->SetZoom(zoom * 0.01);

			PanelText->Redraw(true, true);
			//PanelFormToolbar->Redraw(false, false);
		}
		else if (obj == slider_zoom)
		{
			DT_SLONG position = pdPropGetAsSLong(power_doc, slider_zoom, fcSliderPosition, 0);
			PanelText->SetZoom(POSITION_TO_ZOOM(position) * 0.01);

			PanelText->Redraw(true, true);
			//PanelFormToolbar->Redraw(false, false);
		}
		else if (obj == label_page || obj == button_page_prev || obj == button_page_next)
		{
			DT_SLONG page = PanelText->GetPage();

			if (obj == label_page)
			{
				if ((addr = pdPropGet(power_doc, label_page, fcValue_08, &len)) != DV_NULL && len < 255)
				{
					dtMemCopy(str, addr, len);
					str[len] = 0;
					page = atol(str);
				}
				pdPropSet(power_doc, label_page, fcValue_08, DV_NULL, 0);
			}
			else if (obj == button_page_prev)
			{
				if (page <= 0) return;
				page--;
			}
			else if (obj == button_page_next)
			{
				if (page >= PanelText->GetLastPage()) return;
				page++;
			}

			PanelText->SetPage(page);

			PanelText->Redraw(true, true);
			//PanelFormToolbar->Redraw(false, false);
		}
		else if (obj == button_info) /* info button */
		{
			if (!MainDisplay->FocusExistingWindow(1001)) MainDisplay->RegisterNewWindow(new CDTWindowPlus_Info(MainDisplay), 1001, true);
		}
		else if (obj == button_more) /* settings button */
		{
			if (ParentWindow == DV_NULL) return;
			if (!MainDisplay->FocusExistingWindow(1002)) MainDisplay->RegisterNewWindow(new CDTWindowPlus_Settings(MainDisplay, ParentWindow), 1002, true);
		}
		else if (obj == button_clipboard) /* clipboard button */
		{
			DT_SLONG buffer_size = 2 * txTextGetSelectSize(PanelText->GetTextDoc()->GetTextDoc());
			if (buffer_size < 0) return;

			DT_UBYTE* unicode_buffer = static_cast<DT_UBYTE*>(dtMemAlloc(buffer_size + 1));
			if (unicode_buffer == DV_NULL) return;

			DT_SWORD ret = txTextCopy(PanelText->GetTextDoc()->GetTextDoc(), unicode_buffer, buffer_size, TX_BUFFER_FORMAT_2);
			if (ret != 1)
			{
				dtMemFree(unicode_buffer);
				return;
			}

			if (!MainDisplay->FocusExistingWindow(1003)) MainDisplay->RegisterNewWindow(new CDTWindowPlus_Clipboard(MainDisplay, unicode_buffer, buffer_size), 1003, true);

			dtMemFree(unicode_buffer);
		}
		else if (obj == button_insert) /* insert text button */
		{
			//if (ParentWindow == DV_NULL) return;
			if (!MainDisplay->FocusExistingWindow(1004)) MainDisplay->RegisterNewWindow(new CDTWindowPlus_Insert(MainDisplay, PanelText), 1004, true);
		}
	}

	bool UpdatePanel()
	{
		if (PanelText == DV_NULL) return false;

		const DT_UBYTE* addr;
		DT_CHAR str[256] = "";
		DT_CHAR str_c[256] = "";
		DT_CHAR str_l[256] = "";
		DT_SLONG len;
		DT_PDDOC power_doc = PanelFormToolbar->GetPowerDoc();
		bool render = false;

		DT_ID_SLONG label_page = pdObjFindByNameAndDesc(power_doc, 101, DV_NULL);
		DT_SLONG p_c = -1, page_curr = PanelText->GetPage();
		DT_SLONG p_l = -1, page_last = PanelText->GetLastPage();

		if ((addr = pdPropGet(power_doc, label_page, fcValue_08, &len)) != DV_NULL && len < 255)
		{
			dtMemCopy(str, addr, len);
			str[len] = 0;
			if (sscanf(str, "%s/%s", str_c, str_l) == 2)
			{
				p_c = atol(str_c);
				p_l = atol(str_l);
			}
		}

		if (page_curr != p_c || page_last != p_l)
		{
			sprintf(str, "%.0f/%.0f", static_cast<DT_FLOAT>(page_curr), static_cast<DT_FLOAT>(page_last));
			pdPropSet(power_doc, label_page, fcValue_08, reinterpret_cast<const DT_UBYTE*>(str), static_cast<DT_SLONG>(strlen(str)));
			render = true;
		}

		DT_ID_SLONG label_zoom = pdObjFindByNameAndDesc(power_doc, 201, DV_NULL);
		DT_SLONG z = 0, zoom = DF_ROUND(PanelText->GetZoom() * 100);

		if ((addr = pdPropGet(power_doc, label_zoom, fcValue_08, &len)) != DV_NULL && len < 255)
		{
			dtMemCopy(str, addr, len);
			str[len] = 0;
			z = atol(str);
		}
		if (zoom != z)
		{
			DT_ID_SLONG slider_zoom = pdObjFindByNameAndDesc(power_doc, 200, DV_NULL);
			sprintf(str, "%.0f", static_cast<DT_FLOAT>(zoom));
			pdPropSet(power_doc, label_zoom, fcValue_08, reinterpret_cast<const DT_UBYTE*>(str), static_cast<DT_SLONG>(strlen(str)));
			pdPropApplyAsSLong(power_doc, slider_zoom, fcSliderPosition, ZOOM_TO_POSITION(zoom), 0, PD_APPLY_STANDARD);
			render = true;
		}

		return render;
	}

	void UpdatePanel(bool redraw)
	{
		if (!UpdatePanel()) return;

		if (redraw) PanelFormToolbar->Redraw(false, false);
		else PanelFormToolbar->Render();
	}

private:

	CDTWindowPlusWithExtras* ParentWindow;
	CDTPanelForm* PanelFormToolbar;
	CDTPanelText* PanelText;
};


class CDTHandlerText_Text : public CDTHandlerText
{
public:

	CDTHandlerText_Text(CDTPanelForm* panel_form_toolbar) : PanelText(DV_NULL), PanelFormToolbar(panel_form_toolbar), PanelFormHorScroll(DV_NULL), PanelFormVerScroll(DV_NULL) {}

	CDTHandlerText_Text(CDTPanelForm* panel_form_toolbar, CDTPanelText* panel_text, CDTPanelForm* panel_form_horscroll, CDTPanelForm* panel_form_verscroll) : PanelText(panel_text), PanelFormToolbar(panel_form_toolbar), PanelFormHorScroll(panel_form_horscroll), PanelFormVerScroll(panel_form_verscroll) {}

public:

	inline void SetPanelText(CDTPanelText* panel_text)
	{
		PanelText = panel_text;
	}

	inline void SetPanelFormToolbar(CDTPanelForm* panel_form_toolbar)
	{
		PanelFormToolbar = panel_form_toolbar;
	}

	inline void SetPanelFormHorScroll(CDTPanelForm* panel_form_horscroll)
	{
		PanelFormHorScroll = panel_form_horscroll;
	}

	inline void SetPanelFormVerScroll(CDTPanelForm* panel_form_verscroll)
	{
		PanelFormVerScroll = panel_form_verscroll;
	}

	void Event_ContextSensitiveAction()
	{
		if (!MainDisplay->FocusExistingWindow(1004)) MainDisplay->RegisterNewWindow(new CDTWindowPlus_Insert(MainDisplay, PanelText), 1004, true);
	}

	void Event_ViewChange(CDTTextDoc* /*text_doc*/, DT_FLOAT /*zoom*/, DT_SLONG /*page*/, DT_SLONG offx, DT_SLONG offy, DT_SWORD flag)
	{
		if (PanelText == DV_NULL) return;

		bool redraw;
		DT_PDDOC PowerDoc;
		DT_SLONG position;
		DT_SLONG coverage;
		DT_ID_SLONG button_decr;
		DT_ID_SLONG button_incr;
		DT_SLONG x, y;

		if (PanelFormHorScroll != DV_NULL)
		{
			/* Horizontal scroll */
			redraw = false;
			PowerDoc = PanelFormHorScroll->GetPowerDoc();
			DT_SLONG text_width = PanelText->GetFullW();
			DT_SLONG panel_width = PanelText->GetW();
			DT_ID_SLONG slider_h = pdObjFindByNameAndDesc(PowerDoc, 101, DV_NULL);
			DT_SLONG slider_limit = pdPropGetAsSLong(PowerDoc, slider_h, fcSliderLimit, 0);
			if (slider_limit <= 0) return;

			DT_FLOAT tw = text_width - panel_width; position = DF_ROUND(slider_limit * offx / tw);
			if (position < 0) position = 0; else if (position > slider_limit) position = slider_limit;
			if (position != pdPropGetAsSLong(PowerDoc, slider_h, fcSliderPosition, 0))
			{
				pdPropApplyAsSLong(PowerDoc, slider_h, fcSliderPosition, position, 0, PD_APPLY_STANDARD);
				redraw = true;
			}

			coverage = 10000 * panel_width / text_width;
			if (coverage < 2000) coverage = 2000; else if (coverage > 9000) coverage = 9000;
			if (coverage != pdPropGetAsUWord(PowerDoc, slider_h, fcSliderCoverage, 0))
			{
				pdPropApplyAsUWord(PowerDoc, slider_h, fcSliderCoverage, static_cast<DT_UWORD>(coverage), 0, PD_APPLY_STANDARD);
				redraw = true;
			}

			if (flag == 1)
			{
				button_decr = pdObjFindByNameAndDesc(PowerDoc, 102, DV_NULL);
				button_incr = pdObjFindByNameAndDesc(PowerDoc, 103, DV_NULL);
				DT_SLONG w_decr = pdPropGetAsSLong(PowerDoc, button_decr, fcDw, 0);
				DT_SLONG w_incr = pdPropGetAsSLong(PowerDoc, button_incr, fcDw, 0);
				DT_SLONG w = DF_ROUND(PanelText->GetW() / PanelFormHorScroll->GetZoom());
				pdPropApplyAsSLong(PowerDoc, slider_h, fcDw, w - w_decr - w_incr + 20, 0, PD_APPLY_STANDARD);
				pdImgGetXY(PowerDoc, 0, &x, &y);
				pdImgSetXY(PowerDoc, 0, 0, y);
				pdImgGetXY(PowerDoc, 1, &x, &y);
				pdImgSetXY(PowerDoc, 1, w_decr - 10, y);
				pdImgGetXY(PowerDoc, 2, &x, &y);
				pdImgSetXY(PowerDoc, 2, w - w_incr, y);
			}

			if (flag == 0 && redraw) PanelFormHorScroll->Redraw(false, false); /*Render();*/
		}

		if (PanelFormVerScroll != DV_NULL)
		{
			/* Vertical scroll */
			redraw = false;
			PowerDoc = PanelFormVerScroll->GetPowerDoc();
			DT_SLONG text_height = PanelText->GetFullH();
			DT_SLONG panel_height = PanelText->GetH();
			DT_ID_SLONG slider_v = pdObjFindByNameAndDesc(PowerDoc, 101, DV_NULL);
			DT_SLONG slider_limit = pdPropGetAsSLong(PowerDoc, slider_v, fcSliderLimit, 0);
			if (slider_limit <= 0) return;

			DT_FLOAT th = text_height - panel_height; position = DF_ROUND(slider_limit * offy / th);
			if (position < 0) position = 0; else if (position > slider_limit) position = slider_limit;
			if (position != pdPropGetAsSLong(PowerDoc, slider_v, fcSliderPosition, 0))
			{
				pdPropApplyAsSLong(PowerDoc, slider_v, fcSliderPosition, position, 0, PD_APPLY_STANDARD);
				redraw = true;
			}

			coverage = 10000 * panel_height / text_height;
			if (coverage < 2000) coverage = 2000; else if (coverage > 9000) coverage = 9000;
			if (coverage != pdPropGetAsUWord(PowerDoc, slider_v, fcSliderCoverage, 0))
			{
				pdPropApplyAsUWord(PowerDoc, slider_v, fcSliderCoverage, static_cast<DT_UWORD>(coverage), 0, PD_APPLY_STANDARD);
				redraw = true;
			}

			if (flag == 1)
			{
				button_decr = pdObjFindByNameAndDesc(PowerDoc, 102, DV_NULL);
				button_incr = pdObjFindByNameAndDesc(PowerDoc, 103, DV_NULL);
				DT_SLONG h_decr = pdPropGetAsSLong(PowerDoc, button_decr, fcDh, 0);
				DT_SLONG h_incr = pdPropGetAsSLong(PowerDoc, button_incr, fcDh, 0);
				DT_SLONG h = DF_ROUND(PanelText->GetH() / PanelFormVerScroll->GetZoom());
				pdPropApplyAsSLong(PowerDoc, slider_v, fcDh, h - h_decr - h_incr + 20, 0, PD_APPLY_STANDARD);
				pdImgGetXY(PowerDoc, 0, &x, &y);
				pdImgSetXY(PowerDoc, 0, x, 0);
				pdImgGetXY(PowerDoc, 1, &x, &y);
				pdImgSetXY(PowerDoc, 1, x, h_decr - 10);
				pdImgGetXY(PowerDoc, 2, &x, &y);
				pdImgSetXY(PowerDoc, 2, x, h - h_incr);
			}

			if (flag == 0 && redraw) PanelFormVerScroll->Redraw(false, false); /*Render();*/
		}

		if (PanelFormToolbar == DV_NULL) return;

		/* flag==0 --> event generated by text panel */
		/* flag==1 --> event generated by window resize */
		PanelFormToolbar->GetHandler()->UpdatePanel(flag == 0);
	}

private:

	CDTPanelText* PanelText;
	CDTPanelForm* PanelFormToolbar;
	CDTPanelForm* PanelFormHorScroll;
	CDTPanelForm* PanelFormVerScroll;
};


class CDTHandlerForm_Scroll : public CDTHandlerForm
{
public:

	CDTHandlerForm_Scroll(bool horizontal) : Panel(DV_NULL), Horizontal(horizontal) {}

	CDTHandlerForm_Scroll(CDTPanel* panel, bool horizontal) : Panel(panel), Horizontal(horizontal) {}

public:

	inline void SetPanel(CDTPanel* panel)
	{
		Panel = panel;
	}

	void Event_PostAction(CDTPanel* /*parent_panel*/, DT_PDDOC power_doc, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* ctrl)
	{
		if (Panel == DV_NULL) return;

		DT_ID_SLONG obj = ctrl->Obj;
		DT_ID_SLONG slider = pdObjFindByNameAndDesc(power_doc, 101, DV_NULL);
		DT_ID_SLONG button_decr = pdObjFindByNameAndDesc(power_doc, 102, DV_NULL);
		DT_ID_SLONG button_incr = pdObjFindByNameAndDesc(power_doc, 103, DV_NULL);

		if (obj != button_decr && obj != slider && obj != button_incr) return;

		DT_SLONG position = pdPropGetAsSLong(power_doc, slider, fcSliderPosition, 0);
		DT_SLONG slider_limit = pdPropGetAsSLong(power_doc, slider, fcSliderLimit, 0);
		if (slider_limit <= 0) return;

		if (Horizontal)
		{
			DT_SLONG length = Panel->GetFullW() - Panel->GetW();
			Panel->SetOffX(length * position / slider_limit);
		}
		else
		{
			DT_SLONG length = Panel->GetFullH() - Panel->GetH();
			Panel->SetOffY(length * position / slider_limit);
		}

		Panel->Redraw(false, false);
	}

private:

	CDTPanel* Panel;
	bool Horizontal;
};


class CDTHandlerDrag_Drag : public CDTHandlerDrag
{
public:

	CDTHandlerDrag_Drag(bool horizontal) : ParentWindow(DV_NULL), Horizontal(horizontal) {}

	CDTHandlerDrag_Drag(CDTWindowPlusWithExtras* parent_window, bool horizontal) : ParentWindow(parent_window), Horizontal(horizontal) {}

public:

	inline void SetParentWindow(CDTWindowPlusWithExtras* parent_window)
	{
		ParentWindow = parent_window;
	}

	void Event_DragChange(DT_SLONG x, DT_SLONG y)
	{
		if (ParentWindow == DV_NULL) return;

		if (Horizontal) ParentWindow->SetSize_ActiveHorPanels(x);
		else ParentWindow->SetSize_ActiveVerPanels(y);
	}

private:

	CDTWindowPlusWithExtras* ParentWindow;
	bool Horizontal;
};



#define DV_VER_PANEL_MAX 4
#define DV_HOR_PANEL_MAX 4

class CDTWindowPlusWithExtras_TextDoc : public CDTWindowPlusWithExtras
{
public:

	CDTWindowPlusWithExtras_TextDoc(CDTDisplay* display, CDTTextDoc* text_doc, const DT_CHAR* title, DT_SLONG w, DT_SLONG h) : CDTWindowPlusWithExtras(display, title, w, h, DV_WINDOW_HINT_RESIZE | DV_WINDOW_HINT_CENTER | DV_WINDOW_HINT_FULLRESOLUTION /* | DV_WINDOW_HINT_RGB */),
	 Zoom(0.24 * display->GetScale()),
	 HPanelCount(1), HPanelActive(0),
	 VPanelCount(1), VPanelActive(0),
	 TextPanelActive(0),
	 PanelToolbar(FormControlsLib),
	 TextHandler(&PanelToolbar),
	 HorScrollHandler(true),
	 VerScrollHandler(false),
	 ToolbarHandler(&PanelToolbar),
	 HorDragHandler(true),
	 VerDragHandler(false)
	{
		KH[0] = 0;
		KH[1] = 32768;
		KV[0] = 0;
		KV[1] = 32768;
		ToolbarHandler.SetParentWindow(this);
		HorDragHandler.SetParentWindow(this);
		VerDragHandler.SetParentWindow(this);
		TextDoc[0] = text_doc;
		Construct();
	}

	virtual ~CDTWindowPlusWithExtras_TextDoc()
	{
		Destruct();
	}

public:

	DT_FLOAT GetZoom() const
	{
		return Zoom;
	}

	void SetZoom(DT_FLOAT zoom)
	{
		DT_SLONG i;

		if (DF_ISEQUAL(zoom, Zoom)) return;

		Zoom = zoom;

		for (i = 0; i < HPanelCount * VPanelCount; i++)
		{
			PanelHorScroll[i]->SetZoom(Zoom);
			PanelVerScroll[i]->SetZoom(Zoom);
		}
		PanelToolbar.SetZoom(Zoom * 1.1458);

		Event_WindowResize(GetW(), GetH());
	}

	void SetLayout(CDTTextDoc* text_doc[], DT_SLONG nh, DT_SLONG nv)
	{
		DT_SLONG i;

		if (text_doc == DV_NULL) return;

		Destruct();

		if (nh < 1) nh = 1; else if (nh > DV_HOR_PANEL_MAX) nh = DV_HOR_PANEL_MAX;
		if (nv < 1) nv = 1; else if (nv > DV_VER_PANEL_MAX) nv = DV_VER_PANEL_MAX;

		HPanelCount = nh;
		VPanelCount = nv;

		for (i = 0; i < HPanelCount * VPanelCount; i++) TextDoc[i] = text_doc[i];
		for (i = 0; i <= HPanelCount; i++) KH[i] = static_cast<DT_UWORD>(32768 * i / HPanelCount);
		for (i = 0; i <= VPanelCount; i++) KV[i] = static_cast<DT_UWORD>(32768 * i / VPanelCount);

		Construct();
	}

	void SetLayout(DT_SLONG nh, DT_SLONG nv)
	{
		CDTTextDoc * text_doc[9] = {TextDoc[0], TextDoc[0], TextDoc[0], TextDoc[0], TextDoc[0], TextDoc[0], TextDoc[0], TextDoc[0], TextDoc[0]};
		SetLayout(text_doc, nh, nv);
	}

	virtual void GetLayout(DT_SLONG* nh, DT_SLONG* nv) const
	{
		*nh = HPanelCount;
		*nv = VPanelCount;
	}

	void SetSize_ActiveHorPanels(DT_SLONG hor_d)
	{
		hor_d = 32768 * (hor_d - GetXOff()) / (GetW() - GetXOff());

		if (hor_d < 0) hor_d = 0; else if (hor_d > 32768) hor_d = 32768;
		if (hor_d < KH[HPanelActive]) hor_d = KH[HPanelActive]; else if (hor_d > KH[HPanelActive + 2]) hor_d = KH[HPanelActive + 2];

		KH[HPanelActive + 1] = static_cast<DT_UWORD>(hor_d);
		Event_WindowResize(GetW(), GetH());
		Redraw();
	}

	void SetSize_ActiveVerPanels(DT_SLONG ver_d)
	{
		ver_d = 32768 * (ver_d - GetYOff()) / (GetH() - GetYOff());

		if (ver_d < 0) ver_d = 0; else if (ver_d > 32768) ver_d = 32768;
		if (ver_d < KV[VPanelActive]) ver_d = KV[VPanelActive]; else if (ver_d > KV[VPanelActive + 2]) ver_d = KV[VPanelActive + 2];

		KV[VPanelActive + 1] = static_cast<DT_UWORD>(ver_d);
		Event_WindowResize(GetW(), GetH());
		Redraw();
	}

protected:

	void Event_WindowResize(DT_SLONG w, DT_SLONG h)
	{
		DT_SLONG d_padding = DF_ROUND(15 * Zoom) + 4, d_scroller = DF_ROUND(86 * Zoom);
		DT_SLONG x_off = GetXOff(), x0, x1, x2, lx = w - x_off + d_padding;
		DT_SLONG y_off = GetYOff(), y0, y1, y2, ly = h - y_off + d_padding;
		DT_SLONG i, ix, iy;

		PanelToolbar.SetMargins(20, 10, 5, h - y_off + 5);

		for (ix = 0; ix < HPanelCount - 1; ix++)
		{
			x1 = x_off + KH[ix + 1] * lx / 32768;
			PanelHorDrag[ix].SetMargins(x1 - d_padding + 1, y_off, w - x1, 0);
		}

		for (iy = 0; iy < VPanelCount - 1; iy++)
		{
			y1 = y_off + KV[iy + 1] * ly / 32768;
			PanelVerDrag[iy].SetMargins(x_off, y1 - d_padding + 1, 0, h - y1);
		}

		for (i = 0, iy = 0; iy < VPanelCount; iy++)
		{
			y0 = y_off + KV[iy  ] * ly / 32768;
			y2 = y_off + KV[iy + 1] * ly / 32768 - d_padding;
			y1 = y2 - d_scroller;

			for (ix = 0; ix < HPanelCount; ix++, i++)
			{
				x0 = x_off + KH[ix  ] * lx / 32768;
				x2 = x_off + KH[ix + 1] * lx / 32768 - d_padding;
				x1 = x2 - d_scroller;

				TextHandler.SetPanelText(PanelText[i]);
				TextHandler.SetPanelFormHorScroll(PanelHorScroll[i]);
				TextHandler.SetPanelFormVerScroll(PanelVerScroll[i]);

				PanelHorScroll[i]->SetMargins(x0, y1, w - x1 + 1, h - y2 + 1);
				PanelVerScroll[i]->SetMargins(x1, y0, w - x2, h - y1);

				PanelText[i]->SetMargins(x0, y0, w - x1 + 1, h - y1 + 1);
				PanelText[i]->ClipView();
				PanelText[i]->Notify(1);
			}
		}

		/* Reset active scrollers */
		PanelHorScroll[TextPanelActive]->Reset(0);
		PanelVerScroll[TextPanelActive]->Reset(0);
		/* Restore text handler to the active panel */
		TextHandler.SetPanelText(PanelText[TextPanelActive]);
		TextHandler.SetPanelFormHorScroll(PanelHorScroll[TextPanelActive]);
		TextHandler.SetPanelFormVerScroll(PanelVerScroll[TextPanelActive]);
	}

	void Event_PanelActive(CDTPanel* panel, DT_SLONG /*panel_index*/)
	{
		DT_SLONG i;

		for (i = 0; i < HPanelCount - 1; i++)
		{
			if (panel != &PanelHorDrag[i]) continue;
			HPanelActive = i;
			return;
		}

		for (i = 0; i < VPanelCount - 1; i++)
		{
			if (panel != &PanelVerDrag[i]) continue;
			VPanelActive = i;
			return;
		}

		for (i = 0; i < HPanelCount * VPanelCount; i++)
		{
			if (panel == PanelText[i])
			{
				TextHandler.SetPanelText(PanelText[i]);
				TextHandler.SetPanelFormHorScroll(PanelHorScroll[i]);
				TextHandler.SetPanelFormVerScroll(PanelVerScroll[i]);

				ToolbarHandler.SetPanelText(PanelText[i]);
				ToolbarHandler.UpdatePanel(true);

				PanelText[i]->Redraw(true, true); /*!*/
				TextPanelActive = i;
				return;
			}
			else if (panel == PanelHorScroll[i])
			{
				HorScrollHandler.SetPanel(PanelText[i]);
				return;
			}
			else if (panel == PanelVerScroll[i])
			{
				VerScrollHandler.SetPanel(PanelText[i]);
				return;
			}
		}
	}

private:

	void Construct()
	{
		DT_SLONG i;

		RemoveAllPanels();

		for (i = 0; i < HPanelCount - 1; i++)
		{
			AddPanel(&PanelHorDrag[i]);
			PanelHorDrag[i].SetBkgByte(240);
			PanelHorDrag[i].SetHandler(&HorDragHandler);
			PanelHorDrag[i].Load(Engine->GetPdEngine(), "skin/default/static_drag_hor.pdc");
		}

		for (i = 0; i < VPanelCount - 1; i++)
		{
			AddPanel(&PanelVerDrag[i]);
			PanelVerDrag[i].SetBkgByte(240);
			PanelVerDrag[i].SetHandler(&VerDragHandler);
			PanelVerDrag[i].Load(Engine->GetPdEngine(), "skin/default/static_drag_ver.pdc");
		}

		for (i = 0; i < HPanelCount * VPanelCount; i++)
		{
			PanelText[i] = new CDTPanelText(TextDoc[i]);
			PanelHorScroll[i] = new CDTPanelForm(FormControlsLib);
			PanelVerScroll[i] = new CDTPanelForm(FormControlsLib);

			PanelText[i]->SetHandler(&TextHandler);

			PanelHorScroll[i]->Load(Engine->GetPdEngine(), "skin/default/form_scroll_hor.pdc");
			PanelHorScroll[i]->SetBkgByte(240);
			PanelHorScroll[i]->SetZoom(Zoom);
			PanelHorScroll[i]->SetHandler(&HorScrollHandler);

			PanelVerScroll[i]->Load(Engine->GetPdEngine(), "skin/default/form_scroll_ver.pdc");
			PanelVerScroll[i]->SetBkgByte(240);
			PanelVerScroll[i]->SetZoom(Zoom);
			PanelVerScroll[i]->SetHandler(&VerScrollHandler);

			AddPanel(PanelText[i]);
			AddPanel(PanelHorScroll[i]);
			AddPanel(PanelVerScroll[i]);
		}

		ToolbarHandler.SetPanelText(PanelText[0]);

		PanelToolbar.Load(Engine->GetPdEngine(), "skin/default/form_toolbar.pdc");
		PanelToolbar.SetBkgByte(248);
		PanelToolbar.SetZoom(Zoom * 1.1458);
		PanelToolbar.SetHandler(&ToolbarHandler);

		AddPanel(&PanelToolbar);

		SetBkgByte(240);

		Event_WindowResize(GetW(), GetH());
	}

	void Destruct()
	{
		DT_SLONG i;

		for (i = 0; i < HPanelCount * VPanelCount; i++)
		{
			delete PanelText[i];
			delete PanelHorScroll[i];
			delete PanelVerScroll[i];
		}

		HPanelCount = HPanelActive = 0;
		HPanelCount = VPanelActive = 0;
		TextPanelActive = 0;
	}

	inline DT_SLONG GetXOff() const
	{
		return 20;
	}

	inline DT_SLONG GetYOff() const
	{
		return 10 + DF_ROUND(168 * Zoom) + 5;
	}

private:

	DT_FLOAT Zoom;
	DT_SLONG HPanelCount, HPanelActive;
	DT_SLONG VPanelCount, VPanelActive;
	DT_SLONG TextPanelActive;

	CDTTextDoc* TextDoc[DV_HOR_PANEL_MAX * DV_VER_PANEL_MAX];

	CDTPanelText* PanelText[DV_HOR_PANEL_MAX * DV_VER_PANEL_MAX];
	CDTPanelForm* PanelHorScroll[DV_HOR_PANEL_MAX * DV_VER_PANEL_MAX];
	CDTPanelForm* PanelVerScroll[DV_HOR_PANEL_MAX * DV_VER_PANEL_MAX];
	CDTPanelForm PanelToolbar;
	CDTPanelDrag PanelHorDrag[DV_HOR_PANEL_MAX + 1];
	CDTPanelDrag PanelVerDrag[DV_HOR_PANEL_MAX + 1];

	DT_UWORD KH[DV_HOR_PANEL_MAX + 1];
	DT_UWORD KV[DV_VER_PANEL_MAX + 1];

	CDTHandlerText_Text TextHandler;
	CDTHandlerForm_Scroll HorScrollHandler;
	CDTHandlerForm_Scroll VerScrollHandler;
	CDTHandlerForm_Toolbar ToolbarHandler;
	CDTHandlerDrag_Drag HorDragHandler;
	CDTHandlerDrag_Drag VerDragHandler;
};
