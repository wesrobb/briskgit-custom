/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"
#include "dtwindow.h"


//#define DEBUG_POWERDOC_FILE

#ifdef DEBUG_POWERDOC_FILE
DT_SLONG GLOBAL_POWERDOC_FILE_COUNTER = 0;
#endif


class CDTHandlerText
{
public:

	CDTHandlerText() {}
	virtual ~CDTHandlerText() {}

public:

	virtual void Event_ContextSensitiveAction() {}
	virtual void Event_ViewChange(CDTTextDoc* /*text_doc*/, DT_FLOAT /*zoom*/, DT_SLONG /*page*/, DT_SLONG /*offx*/, DT_SLONG /*offy*/, DT_SWORD /*flag*/) {}
};

class CDTPanelText : public CDTPanel
{
public:

	CDTPanelText(CDTTextDoc* text_doc) : TextDoc(text_doc), HandlerText(DV_NULL), MouseButtonDown(false) {}
	virtual ~CDTPanelText() {}

public:

	DT_SLONG GetLastArea()
	{
		return TextDoc->GetLastArea();
	}

	DT_SLONG GetLastPage()
	{
		return TextDoc->GetLastPage();
	}

	DT_SLONG GetFullW()
	{
		return DF_ROUND(TextDoc->GetW(View.page) * View.zoom);
	}

	DT_SLONG GetFullH()
	{
		return DF_ROUND(TextDoc->GetH(View.page) * View.zoom);
	}

	CDTTextDoc* GetTextDoc()
	{
		return TextDoc;
	}

	inline void SetHandler(CDTHandlerText* handler_text)
	{
		HandlerText = handler_text;
	}

	inline CDTHandlerText* GetHandler()
	{
		return HandlerText;
	}

	void Notify(DT_SWORD flag)
	{
		if (HandlerText != DV_NULL) HandlerText->Event_ViewChange(TextDoc, View.zoom, View.page, View.offx, View.offy, flag);
	}

	void Event_TextPaste(DT_ID_UBYTE paste_command, DT_ID_UBYTE text_type, const DT_UBYTE* buffer, DT_SLONG buffer_len)
	{
		DT_STREAM_MEMORY(sd, buffer, buffer_len);
		txTextPasteViaStream(TextDoc->GetTextDoc(), paste_command, text_type, &sd, -1, DV_NULL);

		if (!GoToPageWithCursor(0, DV_NULL)) Redraw(true, true);
		TextCommand(TX_CMD_SET);

	#ifdef DEBUG_POWERDOC_FILE
		DT_CHAR debug_powerdoc_file[255];
		sprintf(debug_powerdoc_file, "debug_%d.pdc", GLOBAL_POWERDOC_FILE_COUNTER++);
		txTextSaveToFile(TextDoc->GetTextDoc(), 0, debug_powerdoc_file);
	#endif
	}

	void Event_KeyDown(DT_SLONG key, DT_ULONG flags)
	{
		DT_ULONG shift = flags & DV_KEY_SHIFT;
		DT_ULONG ctrl = flags & DV_KEY_CTRL;
		DT_SLONG d = DF_ROUND(25 * View.zoom);
		DT_SLONG info[8];
		bool new_page = false;

		if (key == 37 /*LEFT*/)
		{
			GoToPageWithCursor(shift ? 0 : -1, info);
			DT_SLONG cursor_idx = info[0], cursor_area = info[2];

			txTextInfo(TextDoc->GetTextDoc(), TX_INFO_FRAMESET, cursor_area, 0, DV_NULL, info);
			DT_SLONG min_idx = info[0];
			if (cursor_idx >= 0 && cursor_idx == min_idx) GoToPageWithArea(cursor_area - 1, true);

			TextCommand(ctrl ? (shift ? TX_CMD_SHIFT_PREVCHAR : TX_CMD_PREVCHAR) : (shift ? TX_CMD_SHIFT_LEFT : TX_CMD_LEFT));
		}
		else if (key == 39 /*RIGHT*/)
		{
			GoToPageWithCursor(shift ? 0 : 1, info);
			DT_SLONG cursor_idx = info[0], cursor_area = info[2];

			txTextInfo(TextDoc->GetTextDoc(), TX_INFO_FRAMESET, cursor_area, 0, DV_NULL, info);
			DT_SLONG max_idx = info[1];
			if (cursor_idx >= 0 && cursor_idx == max_idx) GoToPageWithArea(cursor_area + 1, true);

			TextCommand(ctrl ? (shift ? TX_CMD_SHIFT_NEXTCHAR : TX_CMD_NEXTCHAR) : (shift ? TX_CMD_SHIFT_RIGHT : TX_CMD_RIGHT));
		}
		else if (key == 38 /*UP*/)
		{
			GoToPageWithCursor(shift ? 0 : -1, info);
			DT_SLONG cursor_row = info[1], cursor_area = info[2];

			txTextInfo(TextDoc->GetTextDoc(), TX_INFO_FRAMESET, cursor_area, 0, DV_NULL, info);
			DT_SLONG min_row = info[2];
			if (cursor_row >= 0 && cursor_row == min_row) new_page = GoToPageWithArea(cursor_area - 1, true);

			TextCommand(new_page ? (shift ? TX_CMD_SHIFT_LEFT : TX_CMD_LEFT) : (shift ? TX_CMD_SHIFT_UP : TX_CMD_UP));
		}
		else if (key == 40 /*DOWN*/)
		{
			GoToPageWithCursor(shift ? 0 : 1, info);
			DT_SLONG cursor_row = info[1], cursor_area = info[2];

			txTextInfo(TextDoc->GetTextDoc(), TX_INFO_FRAMESET, cursor_area, 0, DV_NULL, info);
			DT_SLONG max_row = info[3];
			if (cursor_row >= 0 && cursor_row == max_row) new_page = GoToPageWithArea(cursor_area + 1, true);

			TextCommand(new_page ? (shift ? TX_CMD_SHIFT_RIGHT : TX_CMD_RIGHT) : (shift ? TX_CMD_SHIFT_DOWN : TX_CMD_DOWN));
		}

		else if (key == 33 /*PageUp*/)
		{
			RePage(View.page - 1, false);
		}
		else if (key == 34 /*PageDown*/)
		{
			RePage(View.page + 1, false);
		}

		/* horizontal/vertical scrolling using keyboard */
		else if (key == 104)
		{
			View.offy -= d;
			Redraw(true, true);
		}
		else if (key == 98)
		{
			View.offy += d;
			Redraw(true, true);
		}
		else if (key == 100)
		{
			View.offx -= d;
			Redraw(true, true);
		}
		else if (key == 102)
		{
			View.offx += d;
			Redraw(true, true);
		}

		/* diagonal scrolling using keyboard */
		else if (key == 103)
		{
			View.offy -= d;
			View.offx -= d;
			Redraw(true, true);
		}
		else if (key == 99)
		{
			View.offy += d;
			View.offx += d;
			Redraw(true, true);
		}
		else if (key == 105)
		{
			View.offy -= d;
			View.offx += d;
			Redraw(true, true);
		}
		else if (key == 97)
		{
			View.offy += d;
			View.offx -= d;
			Redraw(true, true);
		}

		/* reset scrolling position using keyboard */
		else if (key == 101)
		{
			View.offy = 0;
			View.offx = 0;
			Redraw(true, true);
		}

		else if (key == 109 /*-*/)
		{
			if (View.zoom <= 0.2) return;
			View.zoom /= 1.2;
			Redraw(true, true);
		}
		else if (key == 107 /*+*/)
		{
			if (View.zoom >= 10.0) return;
			View.zoom *= 1.2;
			Redraw(true, true);
		}

		else if (ctrl)
		{
		}

		else if (key == 8 /*BAKSPACE*/) Event_TextPaste(TX_PASTE_BACKSPACE_REFORMAT, TX_AUTO, DV_NULL, 0);
		else if (key == 46 /*DELETE*/) Event_TextPaste(TX_PASTE_DELETE_REFORMAT, TX_AUTO, DV_NULL, 0);
		else
		{
			DT_UBYTE k = static_cast<DT_UBYTE>(KeyToChar(key, flags, 0));
			if (k == 0) return;
			Event_TextPaste(TX_PASTE_TEXT_REFORMAT, TX_ANSI, &k, 1);
		}
	}

	void Event_MouseButtonDown(DT_SLONG button, DT_SLONG x, DT_SLONG y, DT_ULONG flags, DT_UBYTE clickcount)
	{
		if (button != 0) return;
		MouseButtonDown = true;

		DT_ID_SWORD hit_type = TX_HIT_NORMAL;
		if (clickcount == 3) hit_type = TX_HIT_TRIPLE;
		else if (clickcount == 2) hit_type = TX_HIT_DOUBLE;
		else if (flags & DV_KEY_SHIFT) hit_type = TX_HIT_SHIFT;
		else if ((flags & DV_KEY_CTRL) && (flags & DV_KEY_ALT)) hit_type = TX_HIT_CTRLALT;
		else if (flags & DV_KEY_CTRL) hit_type = TX_HIT_CTRL;
		else if (flags & DV_KEY_ALT) hit_type = TX_HIT_ALT;

		TextHit(hit_type, x + View.offx, y + View.offy);
	}

	void Event_MouseMove(DT_SLONG x, DT_SLONG y, DT_ULONG flags)
	{
		if (!(flags & DV_MBUTTON_LEFT)) return;
		if (!MouseButtonDown) return;

		DT_SLONG d = DF_ROUND(25 * View.zoom);
		DT_SLONG offx = View.offx;
		DT_SLONG offy = View.offy;

		if (x < 30) View.offx -= d;
		else if (x > MDC->w - XL - XR - 30) View.offx += d;
		if (y < 30) View.offy -= d;
		else if (y > abs(MDC->h) - YB - YT - 30) View.offy += d;

		ClipView();

		if (View.offx != offx || View.offy != offy) Redraw(true, true);

		TextHit(TX_HIT_MOVE, x + View.offx, y + View.offy);
	}

	void Event_MouseButtonUp(DT_SLONG button, DT_SLONG x, DT_SLONG y, DT_ULONG /*flags*/)
	{
		MouseButtonDown = false;

		TextHit(TX_HIT_RELEASE, x + View.offx, y + View.offy);

		if (button != 0)
		{
			if (HandlerText != DV_NULL) HandlerText->Event_ContextSensitiveAction();
			return;
		}
	}

	void Render()
	{
		txTextRefresh(TextDoc->GetTextDoc(), -1);

		CDTPanel::Render();
		txTextSetScale(TextDoc->GetTextDoc(), View.zoom);
		txTextDraw(TextDoc->GetTextDoc(), View.page, View.offx, View.offy, SURFACE_FORMAT, 0, &DC, DV_NULL, 18);
	}

	bool RenderArea(DT_SLONG* xmn, DT_SLONG* ymn, DT_SLONG* xmx, DT_SLONG* ymx)
	{
		if (!CDTPanel::RenderArea(xmn, ymn, xmx, ymx)) return false;
		txTextSetScale(TextDoc->GetTextDoc(), View.zoom);
		txTextDraw(TextDoc->GetTextDoc(), View.page, View.offx + *xmn, View.offy + *ymn, SURFACE_FORMAT, 0, &DC, DV_NULL, 18);
		return true;
	}

private:

	bool RePage(DT_SLONG page, bool /*refocus*/)
	{
		if (page == View.page || page < 0 || page > GetLastPage()) return false;

		View.page = page;
		Redraw(true, true);

		//if (refocus) txTextHit(TextDoc->GetTextDoc(), TX_HIT_AREAFOCUS, 0, 0, DV_NULL, View.page);
		return true;
	}

	bool GoToPageWithArea(DT_SLONG area, bool refocus)
	{
		DT_SLONG page;

		if (area < 0 || area > GetLastArea()) return false;

		txTextInfo(TextDoc->GetTextDoc(), TX_INFO_TEXTAREA, area, 0, &page, DV_NULL);
		return RePage(page, refocus);
	}

	bool GoToPageWithCursor(DT_SLONG param, DT_SLONG output_info[])
	{
		DT_SLONG info[8], char_pos;
		DT_SLONG page, start_pos, end_pos;
		DT_SLONG area, last_area = GetLastArea();

		txTextInfo(TextDoc->GetTextDoc(), TX_INFO_CURSOR, param, 0, &char_pos, output_info);

		for (area = 0; area <= last_area; area++)
		{
			txTextInfo(TextDoc->GetTextDoc(), TX_INFO_TEXTAREA, area, 0, &page, info);
			start_pos = info[0]; end_pos = info[1];

			if (char_pos < start_pos || char_pos > end_pos) continue;

			if (!RePage(page, true)) return false;
			txTextInfo(TextDoc->GetTextDoc(), TX_INFO_CURSOR, param, 0, &char_pos, output_info);
			return true;
		}

		return false;
	}

	void TextHit(DT_ID_SWORD hit_type, DT_SLONG x, DT_SLONG y)
	{
		DT_RECT_SLONG extent;

		if (txTextHit(TextDoc->GetTextDoc(), hit_type, x, y, &extent, -1) == 2) return;

		RedrawArea(true, true, extent.xmn, extent.ymn, extent.xmx, extent.ymx);
	}

	void TextCommand(DT_ID_SWORD command)
	{
		DT_RECT_SLONG extent;

		if (txTextCommand(TextDoc->GetTextDoc(), command, &extent, 0) == 2) return;

		DT_SLONG offx = View.offx;
		DT_SLONG offy = View.offy;

		DT_SLONG diffx = (extent.xmn + extent.xmx) / 2 - View.offx - (MDC->w - XL - XR) / 2;
		DT_SLONG diffy = (extent.ymn + extent.ymx) / 2 - View.offy - (abs(MDC->h) - YT - YB) / 2;

		if (extent.xmx - View.offx > MDC->w - XL - XR - 10) View.offx += diffx;
		else if (extent.xmn - View.offx < 10) View.offx += diffx;

		if (extent.ymx - View.offy > abs(MDC->h) - YT - YB - 10) View.offy += diffy;
		else if (extent.ymn - View.offy < 10) View.offy += diffy;

		if (View.offx < 0) View.offx = 0;
		if (View.offy < 0) View.offy = 0;

		if (View.offx != offx || View.offy != offy) Redraw(true, true);
		else RedrawArea(true, true, extent.xmn, extent.ymn, extent.xmx, extent.ymx);
	}

private:

	CDTTextDoc* TextDoc;
	CDTHandlerText* HandlerText;
	bool MouseButtonDown;
};
