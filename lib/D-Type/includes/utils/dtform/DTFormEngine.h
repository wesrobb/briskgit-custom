/*
 *
 * Copyright (C) D-Type Solutions - All Rights Reserved
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 *
 * This file is part of D-Type Engine and may only be used, modified
 * and distributed under the terms of the applicable license agreement
 * For conditions of distribution and use, see license.txt
 *
 * ANY VIOLATIONS OF THE ABOVE LICENSE AGREEMENT WILL BE PROSECUTED TO
 * THE MAXIMUM EXTENT POSSIBLE UNDER THE LAW. NO VENDOR, DISTRIBUTOR,
 * DEALER, RETAILER, SALES PERSON OR OTHER PERSON IS AUTHORIZED TO MODIFY
 * THIS AGREEMENT OR TO MAKE ANY WARRANTY, REPRESENTATION OR PROMISE WHICH
 * IS DIFFERENT THAN, OR IN ADDITION TO, THIS AGREEMENT.
 *
 */


#ifndef DTYPE_ENGINE_FORM_H
#define DTYPE_ENGINE_FORM_H


//#define FE_DISABLE_XL

#define FE_CMD_PREV 201
#define FE_CMD_NEXT 202
#define FE_CMD_SCROLL_LEFT 301
#define FE_CMD_SCROLL_RIGHT 302
#define FE_CMD_SCROLL_UP 303
#define FE_CMD_SCROLL_DOWN 304

//#define FE_LAST_DRAW_EXTRASLOTS 50 /* Static method (expand by 50 slots) */
#define FE_LAST_DRAW_EXTRASLOTS -1 /* Dynamic method (expand by one half of current size) */


class CDTFormEngine
{
public: // Constructor and Destructor

	CDTFormEngine(CDTFormControlsLib* form_controls_lib, DT_PDDOC power_doc, DT_TXDOC text_doc = DV_NULL, const DT_STREAM_DESC* stream_fontmap = DV_NULL) : PowerDoc(power_doc), TextDoc(text_doc), FormControlsLib(form_controls_lib)
	{
		StreamFontmap = stream_fontmap;

		TextStats = DV_NULL;

		FreeFlag = 0;

		if (TextDoc == DV_NULL)
		{
			txTextIniViaPowerDocAndBuffer(&TextDoc, PowerDoc, 0, DV_NULL, 0, (stream_fontmap == DV_NULL ? 0 : TX_IMPORT_POWERDOC_REFORMAT), DV_NULL, DV_NULL, stream_fontmap, 0);

			txTextSetNumericValue(TextDoc, TX_NVAL_ENABLE_BITS, 9 /* default */ | 128 /* alt proximity algorithm */);
			txTextSetNumericValue(TextDoc, TX_NVAL_CONFIG_BITS, 16 /* BiDi cursor support is enabled */);

			//DT_TX_DOC_PARAMS tx_params = {9 /* default */ | 128 /* alt proximity algorithm */, 16 /* BiDi cursor support is enabled */, DV_NULL};
			//txTextSetParams(TextDoc, &tx_params, 0);

			//txTextCommand(TextDoc, TX_CMD_RESET, DV_NULL, 0); /* Reset does this */
			FreeFlag = 1;
		}

		Reset(false);

		LastDraw = DV_NULL;
		LastDrawLen = 0;
		LastDrawIdx = -1;

		//txTextRegisterConfig(TextDoc, DV_NULL); /* reset */

		DT_TX_TEXTEDIT_CONFIG Config;

		Config.Name_TextAreaW = fcDw;
		Config.Name_TextAreaH = fcDh;


		Config.DisableTextFormat = 0;
		Config.Name_TextString_8 = pdTextString_08;
		Config.Name_TextString_16 = pdTextString_16;
		Config.Name_TextString_24 = pdTextString_24;
		Config.Name_TextString_32 = pdTextString_32;
		Config.Name_TextFragmentPtr = pdTextFragmentPtr;

		Config.FnDraw = fcDrawRichTextBox;
		Config.DefaultWidth = 500;
		Config.DefaultHeight = 100;
		txTextRegisterConfig(TextDoc, &Config);

		Config.FnDraw = fcDrawRichButton;
		Config.DefaultWidth = -1;
		Config.DefaultHeight = -1;
		txTextRegisterConfig(TextDoc, &Config);

		Config.FnDraw = fcDrawRichListBox;
		Config.DefaultWidth = 500;
		Config.DefaultHeight = 100;
		txTextRegisterConfig(TextDoc, &Config);


		Config.DisableTextFormat = 1;
		Config.Name_TextString_8 = fcValue_08;
		Config.Name_TextString_16 = fcValue_16;
		Config.Name_TextString_24 = fcValue_24;
		Config.Name_TextString_32 = fcValue_32;
		Config.Name_TextFragmentPtr = pd0;


		Config.FnDraw = fcDrawTextBox;
		Config.DefaultWidth = 500;
		Config.DefaultHeight = 100;
		txTextRegisterConfig(TextDoc, &Config);

		Config.FnDraw = fcDrawButton;
		Config.DefaultWidth = -1;
		Config.DefaultHeight = -1;
		txTextRegisterConfig(TextDoc, &Config);
	}

	virtual ~CDTFormEngine()
	{
		if (LastDraw != DV_NULL) dtMemFree(LastDraw);
		if (FreeFlag && TextDoc != DV_NULL) txTextExt(TextDoc);
		if (TextStats != DV_NULL) pdFree(TextStats);
	}


public: // Commands

	void Reset(bool refresh);

	void ResetTextBoxStats(DT_ID_SLONG obj_text, const DT_CHAR* /*reason*/)
	{
		//if (obj_text < 0) printf("TextBoxStats cleared due to %s\n", reason);

		LastTextStatsObj = obj_text;
		if (TextStats != DV_NULL) { pdFree(TextStats); TextStats = DV_NULL; }
	}


public: // General Overrides

	virtual void Render(bool refresh, DT_SLONG x, DT_SLONG y, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_MDC* dc)
	{
		DT_PD_DOCDRAW_PARAMS params;
		pdxDrawParamsReset(&params);
		params.UserData = this;

		if (refresh) txTextRefresh(TextDoc, -1);
		txTextDraw(TextDoc, GetCurrentPage(), x, y, format, subformat, dc, &params, 1);
	}

	inline void CmdF_Refresh(bool reset_textbox_stats = true) /* refresh device metrics */
	{
		LastDrawIdx = -1;
		if (reset_textbox_stats) ResetTextBoxStats(-1, "CmdF_Refresh");
	}

	inline void CmdF_CursorUpdateAfterExternalContentChange(DT_ID_SLONG obj_text, DT_ID_SWORD pos = 0) /* Move cursor to end after outside text change */
	{
		DT_ID_SWORD command = TX_CMD_CURRTEXT;
		if (pos < 0) command = TX_CMD_STARTTEXT; else if (pos > 0) command = TX_CMD_ENDTEXT;
		if (FocusedTextCtrl.Obj == obj_text) txTextCommand(TextDoc, command, DV_NULL, 0);
	}


public: // Setters

	inline void SetExportFlags(DT_UWORD flags) { FormControlsLib->SetExportFlags(flags); }


public: // Getters

	inline DT_TXDOC GetTextDoc() const { return TextDoc; }
	inline DT_SWORD GetFousedCtrlType() const { return FocusedCtrl.Type; }


public: // Events

	void EvnF_MouseLPush(DT_SLONG x, DT_SLONG y, DT_SWORD click_type);
	void EvnF_MouseMove(DT_SLONG x, DT_SLONG y);
	void EvnF_MouseLHold();
	void EvnF_MouseLRelease(DT_SLONG x = 0, DT_SLONG y = 0);
	void EvnF_MouseScroll(DT_SLONG x, DT_SLONG y, DT_SLONG dx, DT_SLONG dy);
	void EvnF_KeyChar(DT_ID_UBYTE command, const DT_UBYTE* buffer, DT_SLONG nr_of_chars, DT_UBYTE buffer_format);
	void EvnF_KeyCtrl(DT_ID_SWORD command);
	void EvnF_Handler(DT_ID_SLONG signal_id) { if (FocusedCtrl.Type != -1) Handler_Invoke(signal_id, &FocusedCtrl); }
	void EvnF_HandlerText(DT_ID_SLONG signal_id) { if (FocusedTextCtrl.Type != -1) Handler_Invoke(signal_id, &FocusedTextCtrl); }


protected: // General Overrides

	virtual void Event_Action(DT_PDDOC /*power_doc*/, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* /*ctrl*/) {}
	virtual void Event_PreAction(DT_PDDOC /*power_doc*/, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* /*ctrl*/) {}
	virtual void Event_PostAction(DT_PDDOC /*power_doc*/, DT_SLONG /*signal_id*/, DT_UBYTE /*handler*/, const CDTControl* /*ctrl*/) {}

	virtual void Repage(DT_SLONG /*page*/) {} //= 0;
	virtual void Redraw(bool /*unused*/) {} //= 0;
	virtual void RedrawArea(DT_SLONG /*x_clip*/, DT_SLONG /*y_clip*/, DT_SLONG /*w_clip*/, DT_SLONG /*h_clip*/) {} //= 0;

	virtual void ShowStatus(const DT_CHAR* /*status_text*/) {} //= 0;
	virtual void OpenURL(const DT_CHAR* /*url*/, DT_ID_SWORD /*url_type*/) {} //= 0;

	virtual DT_SLONG GetCurrentPage() /* const - will break backwards compatibility */ { return 0; } //= 0;
	virtual DT_SWORD GetTextFormatFlags(DT_UBYTE* compression) /* const - will break backwards compatibility */ { *compression = 0; return TX_IMPORT_POWERDOC_REFORMAT; } //= 0;


public:

	DT_SLONG UserObjDraw(DT_PD_DOCDRAW_CANVAS* c, DT_SLONG fn_draw, const DT_TM3X3 tm, bool need_frames, bool update_last_draw);

	static DT_SWORD UserObjDrawStatic(DT_PD_DOCDRAW_CANVAS* c, DT_SLONG fn_draw, const DT_TM3X3 tm, DT_SWORD /*flag*/)
	{
		if (c == DV_NULL) return false;
		if (c->UserData == DV_NULL) return false;

		CDTFormEngine* form_engine = reinterpret_cast<CDTFormEngine*>(c->UserData);
		bool need_frames = false;

		if (form_engine->GetCurrentPage() == c->Page && c->Flags == 0) need_frames = true;

		if (form_engine->UserObjDraw(c, fn_draw, tm, need_frames, true) == 0) return 1;

		return 1;
	}

	DT_SLONG GetNrOfVisibleRows(DT_ID_SLONG ctrl_obj) const
	{
		DT_PDX_WH_SLONG box_doc;
		DT_SWORD lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, ctrl_obj, fcConfigPtr), fcCnfLib, 0));
		GetCtrlStateAndBox(ctrl_obj, &box_doc);
		return FormControlsLib->DTListBox[lib].GetNrOfVisibleRows(box_doc.H);
	}

	void UpdatePaintArea(DT_SLONG left, DT_SLONG top, DT_SLONG right, DT_SLONG bottom);

	DT_SLONG GetTextBoxLimitAndCoverage(DT_ID_SWORD fn_draw, DT_ID_SLONG obj, DT_SLONG h, DT_UWORD* coverage);
	bool GetTextBoxStats(DT_ID_SWORD fn_draw, DT_ID_SLONG obj);


private:

	bool GetResolvedMouseXY(DT_SLONG idx, const DT_PDX_WH_SLONG* box_doc, DT_SLONG x, DT_SLONG y, DT_SLONG* x_resolved_doc, DT_SLONG* y_resolved_doc) const;

	void DrawCtrl(const CDTControl* ctrl);
	void GetCtrlExtentPix(DT_SLONG idx, const DT_PDX_WH_SLONG* box_doc, DT_PDX_XYWH_SLONG* extent_pix) const;
	DT_UBYTE GetCtrlStateAndBox(DT_ID_SLONG obj, DT_PDX_WH_SLONG* box_doc) const;
	bool RefocusCtrl();
	bool FocusCtrlByCoord(DT_SLONG x, DT_SLONG y, DT_SLONG* x_resolved, DT_SLONG* y_resolved);
	bool FocusCtrlByOrder(bool move_forward);
	void SetSelectionInListBox(DT_ID_SLONG obj, DT_SLONG selected_item, DT_UBYTE select_type);

	// Common Events
	bool ChangeFocusCtrl(bool do_events, const CDTControl* ctrl);
	void PushCtrl(bool do_events, const CDTControl* ctrl, DT_UBYTE state_flag, bool redraw);
	void HoldCtrl(bool do_events, const CDTControl* ctrl);
	void ReleaseCtrl(bool do_events, const CDTControl* ctrl, DT_UBYTE state_flag);

	// Radio Button Specific Events
	void ReleaseRadioButton(bool do_events, const CDTControl* ctrl);

	// Check Box Specific Events
	void ReleaseCheckBox(bool do_events, const CDTControl* ctrl);

	// Text Box Specific Events
	void ChangeTextBox(bool do_events, const CDTControl* ctrl, DT_UBYTE command, const DT_UBYTE* buffer, DT_SLONG nr_of_chars, DT_UBYTE buffer_format);
	bool ScrollTextBox(bool do_events, const CDTControl* ctrl, DT_SLONG scroll_amount, bool by_row, bool absolute_scroll);
	DT_SLONG GetTextBoxFirstItem(const CDTControl* ctrl, DT_SLONG line_break_item);
	DT_SLONG GetTextBoxRowInfo(const CDTControl* ctrl, DT_SLONG line_break_item, DT_SLONG* first_char_index, DT_SLONG* last_char_index);
	DT_SLONG GetTextBoxLimitAndCoverage(const CDTControl* ctrl, DT_UWORD* coverage);
	DT_SLONG GetTextBoxFirstRow(const CDTControl* ctrl, bool cache = true);
	bool GetTextBoxStats(const CDTControl* ctrl);

	// List Box Specific Events
	DT_SLONG SelectRichListBox(const CDTControl* ctrl, DT_SLONG y, DT_SWORD click_type);
	DT_SLONG SelectListBox(const CDTControl* ctrl, DT_SLONG y, DT_SWORD click_type);
	DT_SLONG AbsSelectionInListBox(bool do_events, const CDTControl* ctrl, DT_SLONG selected_item, DT_UBYTE select_type);
	DT_SLONG RelSelectionInListBox(bool do_events, const CDTControl* ctrl, bool increase, DT_UBYTE select_type);
	DT_SLONG ScrollListBox(bool do_events, const CDTControl* ctrl, DT_SLONG scroll_amount, bool absolute_scroll);

	// Slider Specific Events
	void SelectSlider(bool hor_ver_flag, DT_SLONG t, DT_SWORD direction);
	void DragSlider(bool hor_ver_flag, DT_SLONG t);
	DT_SLONG MoveSliderToPosition(bool do_events, const CDTControl* ctrl, DT_SLONG slider_position, DT_SLONG slider_limit = -1, DT_UWORD slider_coverage = 0);

	void Handler_Invoke(DT_ID_SLONG signal_id, const CDTControl* ctrl)
	{
		//DT_UBYTE handler = pdPropGetAsUByte(PowerDoc, ctrl->Obj, signal_id, 0);

		DT_SLONG i, len = 0;
		const DT_UBYTE* handler_arr = pdPropGet(PowerDoc, ctrl->Obj, signal_id, &len); if (handler_arr == DV_NULL || len < 1) return;
		for (i = 0; i < len; i++) { if (handler_arr[i] > 0) Handler_CallbackFunction(signal_id, handler_arr[i], ctrl); }
	}

	// Callback Function
	void Handler_CallbackFunction(DT_SLONG signal_id, DT_UBYTE handler, const CDTControl* ctrl);

	// Built-in Callback Functions
	void Handler_SetScrollForConnCtrls(const CDTControl* ctrl, DT_SLONG first_item);
	void Handler_UpdateConnSliders(const CDTControl* ctrl, DT_SLONG slider_position, DT_SLONG slider_limit = -1, DT_UWORD slider_coverage = 0);
	void Handler_ScrollConnCtrls(const CDTControl* ctrl, bool increase);
	void Handler_CopyListBoxValueToTextCtrls(const CDTControl* ctrl, DT_SLONG selected_item);
	void Handler_CopySliderPosValueToConnTextCtrls(const CDTControl* ctrl, DT_SLONG position_value);
	void Handler_CopyValueToConnTextCtrls(const CDTControl* ctrl, bool single_text_fragment, CDTValue& value);
	void Handler_Popup(DT_ID_SLONG ctrl_obj, DT_SWORD flag);
	void Handler_OpenURL(const CDTControl* ctrl, DT_ID_SWORD url_type);
	void Handler_JumpToImg(const CDTControl* ctrl, bool first);
	DT_ID_SLONG Handler_CopySliderPosValueToConnMedia(const CDTControl* ctrl, DT_ID_UBYTE next_command, DT_SLONG next_time, DT_SLONG next_frame);

	void Handler_JumpToPage(const CDTControl* ctrl)
	{
		DT_SLONG page = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcUserParam, 0);
		CmdF_Refresh(true);
		Repage(page);
	}

	void Handler_JumpToPrevPage(const CDTControl* /*ctrl*/) { CmdF_Refresh(true); Repage(GetCurrentPage() - 1); }
	void Handler_JumpToNextPage(const CDTControl* /*ctrl*/) { CmdF_Refresh(true); Repage(GetCurrentPage() + 1); }

	void MoveCursor_Text(const CDTControl* ctrl, DT_ID_SWORD command, DT_SLONG scroll_amount, bool by_row);
	void MoveCursor_RichListBox(const CDTControl* ctrl, DT_ID_SWORD command, DT_SLONG scroll_amount, DT_SWORD click_type, bool by_row);


protected: // Text Handling Overrides

	virtual void CmdT_Refresh(DT_SLONG img)
	{
		txTextRefresh(TextDoc, img);
	}

	virtual void CmdT_Command(DT_ID_SWORD command)
	{
		DT_RECT_SLONG extent;
		if (txTextCommand(TextDoc, command, &extent, 0) == 2) return;
		UpdatePaintArea(extent.xmn, extent.ymn, extent.xmx, extent.ymx);
		//RedrawArea(extent.xmn, extent.ymn, extent.xmx, extent.ymx);
	}

	virtual void EvnT_HitPlus(DT_ID_SWORD click_type, DT_SLONG x, DT_SLONG y, const DT_TX_TEXT_FLOW* textflow)
	{
		DT_RECT_SLONG extent;
		//textflow = DV_NULL; /* allow selection of any text flow */

		if (txTextHitPlus(TextDoc, click_type, x, y, textflow, &extent, -1) == 2) return;
		UpdatePaintArea(extent.xmn, extent.ymn, extent.xmx, extent.ymx);
		//RedrawArea(extent.xmn, extent.ymn, extent.xmx, extent.ymx);
	}

	virtual bool ActT_UpdateAfterContentChange(DT_ID_SWORD /*operation_id*/) { return true; }

	virtual bool ActT_Paste(DT_ID_UBYTE paste_command, DT_SLONG text_len, const DT_UBYTE* text_addr, DT_UBYTE buffer_format)
	{
		paste_command |= 64; /* don't optimize */

		DT_UBYTE compression = 0;
		DT_SWORD unicode_flags = GetTextFormatFlags(&compression);
		if (unicode_flags != TX_IMPORT_POWERDOC_REFORMAT) { unicode_flags |= 8192; paste_command |= 128; /* reformat */ }
		if (compression == 1) unicode_flags |= TX_IMPORT_UNICODE_OPTIMIZE | TX_IMPORT_UNICODE_COMPRESS;

		if (txTextPasteViaBuffer(TextDoc, paste_command, text_len, text_addr, buffer_format, unicode_flags, DV_NULL) < 0) return false;
		txTextRefresh(TextDoc, FocusedCtrl.Img);
		if (FocusedCtrl.Obj == LastTextStatsObj) ResetTextBoxStats(-1, "ActT_PasteIntoTextSelection");
		ActT_UpdateAfterContentChange(0);
		DrawCtrl(&FocusedCtrl);
		return true;
	}

	virtual DT_SLONG GetT_EnclosingTextFlow(DT_SLONG x, DT_SLONG y, DT_TX_TEXT_FLOW* textflow) /* const - will break backwards compatibility */
	{
		DT_SLONG extra[] = {x, y};
		DT_SLONG char_index = -1;
		if (txTextInfo(TextDoc, TX_INFO_HIT, 0, 0, &char_index, extra) != 1) { textflow->Img = -1; textflow->ParentObj = -1; return -1; }
		textflow->Img = extra[0];
		textflow->ParentObj = extra[1];
		return char_index;
	}


private:

	DT_SWORD GetPaintArea(DT_RECT_SLONG* Extent, bool clear);

	void RePaint()
	{
		DT_RECT_SLONG extent;
		DT_SWORD ret = GetPaintArea(&extent, true);
		if (ret == 2) return;
		else if (ret == 1) RedrawArea(extent.xmn, extent.ymn, extent.xmx - extent.xmn, extent.ymx - extent.ymn);
		else { CmdF_Refresh(false); Redraw(true); }
	}

	bool UpdateLastDraw(DT_UBYTE attribs, DT_ID_SLONG parent_obj, DT_ID_SLONG obj, DT_SLONG img, DT_SLONG fn_draw, DT_SRAST_XL x, DT_SRAST_XL y, const DT_TM3X3 tm);


protected:

	DT_PDDOC PowerDoc;
	DT_TXDOC TextDoc;


private:

	const DT_STREAM_DESC* StreamFontmap;

	DT_SLONG* TextStats;

	CDTFormControlsLib* FormControlsLib;

	CDTControl FocusedCtrl, FocusedTextCtrl, PrevioslyFocusedCtrl;

	struct FE_LAST_STATE
	{
		DT_SLONG SliderPositionOriginal;
		DT_SLONG SliderScrollableRange;
		DT_SLONG SelectedListBoxItemOld;
		DT_PDX_XY_SLONG MouseCoordDoc;
		DT_PDX_XY_SLONG MouseClick; /* new in 7.5.0.1 */
		DT_UBYTE MouseClicked;

	} LastState;

	struct FE_LAST_DRAW
	{
		DT_TM3X3 tm;
		DT_ID_SLONG parent_obj;
		DT_ID_SLONG obj;
		DT_ID_SWORD fn_draw;
		DT_SLONG img;
	#ifdef FE_DISABLE_XL
		DT_SRAST_L x, y;
	#else
		DT_SRAST_XL x, y;
	#endif
		DT_UBYTE attribs;

	} *LastDraw;

	DT_SLONG LastTextStatsObj;

	DT_SLONG LastDrawIdx;
	DT_SLONG LastDrawLen;

	DT_RECT_SLONG PaintArea;

	DT_SWORD PaintFlag;
	DT_SWORD FreeFlag;
};


#endif /* DTYPE_ENGINE_FORM_H */
