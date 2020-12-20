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


#include "DTFormControls.cpp"
#include "DTFormEngine.h"


/////////////////////////////////////////////////////////////////////////////
// Rendering


DT_SLONG CDTFormEngine::UserObjDraw(DT_PD_DOCDRAW_CANVAS* c, DT_SLONG fn_draw, const DT_TM3X3 tm, bool need_frames, bool update_last_draw)
{
	if (PowerDoc != c->PowerDocInst) return 1; /* this never happens but just in case */

	DT_SRAST_XL x = c->ImgX;
	DT_SRAST_XL y = c->ImgY;
	DT_SLONG ret = FormControlsLib->UserObjDraw(FocusedCtrl.Obj, c, fn_draw, tm, need_frames, &x, &y);

	//printf("obj=%d, img=%d, parent=%d, fn_draw=%d, x=%d, y=%d, tm=[%f,%f,%f,%f]\n", c->Obj, c->Img, c->ParentObj, fn_draw, x, y, tm[0][0], tm[0][1], tm[1][0], tm[1][1]);

	if (c->Obj < 0 || c->Img < 0) return ret; /* this does not happen but just in case */
	if (update_last_draw) UpdateLastDraw(c->Attribs, c->ParentObj, c->Obj, c->Img, fn_draw, x, y, tm);

	return ret;
}


/////////////////////////////////////////////////////////////////////////////
// Interaction


void CDTFormEngine::GetCtrlExtentPix(DT_SLONG idx, const DT_PDX_WH_SLONG* box_doc, DT_PDX_XYWH_SLONG* extent_pix) const
{
#define FE_CLIPCORR_DOC 20
#define FE_CLIPCORR_PIX 5

	DT_SLONG x = -FE_CLIPCORR_DOC, y = -FE_CLIPCORR_DOC;
	DT_SLONG w = 2 * FE_CLIPCORR_DOC + box_doc->W, h = 2 * FE_CLIPCORR_DOC + box_doc->H;

	DT_TM3X3 tm_off =
	{
		{1.0, 0.0, 0.0 + LastDraw[idx].x},
		{0.0, 1.0, 0.0 + LastDraw[idx].y},
		{0.0, 0.0, 1.0}
	};
	DT_TM3X3 tm_img;

	/* Multiply transforms */
	pdxMatrixMultiplyPlus(tm_off, LastDraw[idx].tm, tm_img);

	/* Apply transform */
	pdTransformBoxPlus(&x, &y, &w, &h, tm_img);

	extent_pix->X = x - FE_CLIPCORR_PIX;
	extent_pix->Y = y - FE_CLIPCORR_PIX;
	extent_pix->W = w + 2 * FE_CLIPCORR_PIX;
	extent_pix->H = h + 2 * FE_CLIPCORR_PIX;
}


void CDTFormEngine::DrawCtrl(const CDTControl* ctrl)
{
	DT_PDX_XYWH_SLONG extent_pix;

	if (ctrl->Type < 0 || ctrl->Idx < 0) return;

	GetCtrlExtentPix(ctrl->Idx, &ctrl->BoxDoc, &extent_pix);
	UpdatePaintArea(extent_pix.X, extent_pix.Y, extent_pix.X + extent_pix.W, extent_pix.Y + extent_pix.H);
	//RedrawArea(extent_pix.X, extent_pix.Y, extent_pix.W, extent_pix.H);
}


DT_UBYTE CDTFormEngine::GetCtrlStateAndBox(DT_ID_SLONG obj, DT_PDX_WH_SLONG* box_doc) const
{
	box_doc->W = pdPropGetAsSLong(PowerDoc, obj, fcDw, 500);
	box_doc->H = pdPropGetAsSLong(PowerDoc, obj, fcDh, 100);

	return pdPropGetAsUByte(PowerDoc, obj, fcState, FC_NORMAL);
}


bool CDTFormEngine::FocusCtrlByOrder(bool move_forward)
{
	DT_ID_SLONG obj;
	DT_ID_SWORD fn_draw;
	DT_SLONG order_id, idx, idx_last = LastDrawIdx;
	DT_UBYTE state;
	DT_UBYTE lock_status = 0;
	DT_PDX_WH_SLONG box_doc;

	struct
	{
		DT_SLONG s_order_id;
		DT_SLONG s_idx;
		bool s_exists;
	}
	first = {999999, 0, false}, last = {-999999, 0, false},
	prev = {-999999, 0, false}, next = {999999, 0, false},
	curr = {pdPropGetAsSWord(PowerDoc, FocusedCtrl.Obj, fcTabOrderID, 0), FocusedCtrl.Idx, true};

	FocusedCtrl.Reset(FC_NORMAL);

	for (idx = 0; idx <= idx_last; idx++)
	{
		if ((LastDraw[idx].attribs) & 1) continue; /* hidden */

		if ((fn_draw = LastDraw[idx].fn_draw) < fcDrawFirst || fn_draw > fcDrawLast) continue;

		obj = LastDraw[idx].obj;

		if ((lock_status = pdPropGetAsUByte(PowerDoc, obj, fcLockStatus, 0)) == 1) continue;
		if ((order_id = pdPropGetAsSWord(PowerDoc, obj, fcTabOrderID, 0)) == 0) continue;

		/* Find first */
		if (order_id < first.s_order_id || (order_id == first.s_order_id && idx < first.s_idx))
		{
			first.s_order_id = order_id;
			first.s_idx = idx;
			first.s_exists = true;
		}

		/* Find last */
		if (order_id > last.s_order_id || (order_id == last.s_order_id && idx > last.s_idx))
		{
			last.s_order_id = order_id;
			last.s_idx = idx;
			last.s_exists = true;
		}

		/* Find previous */
		if (order_id < curr.s_order_id || (order_id == curr.s_order_id && idx < curr.s_idx))
		{
			if (order_id > prev.s_order_id || (order_id == prev.s_order_id && idx > prev.s_idx))
			{
				prev.s_order_id = order_id;
				prev.s_idx = idx;
				prev.s_exists = true;
			}
		}

		/* Find next */
		if (order_id > curr.s_order_id || (order_id == curr.s_order_id && idx > curr.s_idx))
		{
			if (order_id < next.s_order_id || (order_id == next.s_order_id && idx < next.s_idx))
			{
				next.s_order_id = order_id;
				next.s_idx = idx;
				next.s_exists = true;
			}
		}
	}

	if (move_forward)
	{
		if (next.s_exists) idx = next.s_idx;
		else if (first.s_exists) idx = first.s_idx;
		else return false;
	}
	else
	{
		if (prev.s_exists) idx = prev.s_idx;
		else if (last.s_exists) idx = last.s_idx;
		else return false;
	}

	obj = LastDraw[idx].obj;
	state = GetCtrlStateAndBox(obj, &box_doc);
	FocusedCtrl.Type = LastDraw[idx].fn_draw;
	FocusedCtrl.State = state;
	FocusedCtrl.PushType = 0;
	FocusedCtrl.Reserved = 0;
	FocusedCtrl.LockStatus = pdPropGetAsUByte(PowerDoc, obj, fcLockStatus, 0); //lock_status;
	FocusedCtrl.Idx = idx;
	FocusedCtrl.Obj = obj;
	FocusedCtrl.Img = LastDraw[idx].img;
	FocusedCtrl.ParentObj = LastDraw[idx].parent_obj;
	FocusedCtrl.Lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, obj, fcConfigPtr), fcCnfLib, 0));
	FocusedCtrl.BoxDoc = box_doc;

	return true;
}


bool CDTFormEngine::GetResolvedMouseXY(DT_SLONG idx, const DT_PDX_WH_SLONG* box_doc, DT_SLONG x, DT_SLONG y, DT_SLONG* x_resolved_doc, DT_SLONG* y_resolved_doc) const
{
	DT_TM3X3 tm_off =
	{
		{1.0, 0.0, 0.0 + LastDraw[idx].x},
		{0.0, 1.0, 0.0 + LastDraw[idx].y},
		{0.0, 0.0, 1.0}
	};
	DT_TM3X3 tm_img;

	/* Multiply transforms */
	pdxMatrixMultiplyPlus(tm_off, LastDraw[idx].tm, tm_img);

	/* Apply inverse transform */
	DT_FLOAT a1 = tm_img[2][0] * tm_img[1][1] - tm_img[2][1] * tm_img[1][0];
	DT_FLOAT a2 = tm_img[0][0] * tm_img[2][1] - tm_img[0][1] * tm_img[2][0];
	DT_FLOAT a3 = tm_img[0][1] * tm_img[1][0] - tm_img[0][0] * tm_img[1][1];

	DT_FLOAT w = x * a1 + y * a2 + a3;
	if (DF_ISEQUAL(w, 0.0)) return false;
	w = 1.0 / w;

	DT_FLOAT b1 = tm_img[2][1] * tm_img[1][2] - tm_img[2][2] * tm_img[1][1];
	DT_FLOAT b2 = tm_img[0][1] * tm_img[2][2] - tm_img[0][2] * tm_img[2][1];
	DT_FLOAT b3 = tm_img[0][2] * tm_img[1][1] - tm_img[0][1] * tm_img[1][2];
	DT_FLOAT c1 = tm_img[2][2] * tm_img[1][0] - tm_img[2][0] * tm_img[1][2];
	DT_FLOAT c2 = tm_img[0][2] * tm_img[2][0] - tm_img[0][0] * tm_img[2][2];
	DT_FLOAT c3 = tm_img[0][0] * tm_img[1][2] - tm_img[0][2] * tm_img[1][0];

	DT_FLOAT x_doc = (x * b1 + y * b2 + b3) * w;
	DT_FLOAT y_doc = (x * c1 + y * c2 + c3) * w;

	*x_resolved_doc = DF_ROUND(x_doc);
	*y_resolved_doc = DF_ROUND(y_doc);

	if ((*x_resolved_doc) >= 0 && (*x_resolved_doc) <= box_doc->W && (*y_resolved_doc) >= 0 && (*y_resolved_doc) <= box_doc->H) return true;
	return false;
}


bool CDTFormEngine::RefocusCtrl()
{
	DT_UBYTE lock_status;
	DT_PDX_WH_SLONG box_doc;
	DT_ID_SLONG obj = FocusedCtrl.Obj;
	DT_UBYTE state = GetCtrlStateAndBox(obj, &box_doc);

	if ((lock_status = pdPropGetAsUByte(PowerDoc, obj, fcLockStatus, 0)) == 1) return false; /* Must not be locked */

	FocusedCtrl.State = state;
	FocusedCtrl.PushType = 0;
	FocusedCtrl.Reserved = 0;
	FocusedCtrl.LockStatus = lock_status;
	FocusedCtrl.Lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, obj, fcConfigPtr), fcCnfLib, 0));
	FocusedCtrl.BoxDoc = box_doc;
	return true;
}


bool CDTFormEngine::FocusCtrlByCoord(DT_SLONG x, DT_SLONG y, DT_SLONG* x_resolved, DT_SLONG* y_resolved)
{
	DT_ID_SLONG obj;
	DT_ID_SWORD fn_draw;
	DT_SLONG idx, idx_last = LastDrawIdx;
	DT_UBYTE state;
	DT_UBYTE lock_status;
	DT_PDX_WH_SLONG box_doc;

	FocusedCtrl.Reset(FC_NORMAL);

	for (idx = idx_last; idx >= 0; idx--)
	{
		if ((LastDraw[idx].attribs) & 1) continue; /* hidden */
		if ((fn_draw = LastDraw[idx].fn_draw) < fcDrawFirst || fn_draw > fcDrawLast) continue;

		obj = LastDraw[idx].obj;
		state = GetCtrlStateAndBox(obj, &box_doc);

		if (!GetResolvedMouseXY(idx, &box_doc, x, y, x_resolved, y_resolved)) continue;

		if ((lock_status = pdPropGetAsUByte(PowerDoc, obj, fcLockStatus, 0)) == 1) continue; /* Must not be locked */

		FocusedCtrl.Type = LastDraw[idx].fn_draw;
		FocusedCtrl.State = state;
		FocusedCtrl.PushType = 0;
		FocusedCtrl.Reserved = 0;
		FocusedCtrl.LockStatus = lock_status;
		FocusedCtrl.Idx = idx;
		FocusedCtrl.Obj = obj;
		FocusedCtrl.Img = LastDraw[idx].img;
		FocusedCtrl.ParentObj = LastDraw[idx].parent_obj;
		FocusedCtrl.Lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, obj, fcConfigPtr), fcCnfLib, 0));
		FocusedCtrl.BoxDoc = box_doc;
		FocusedCtrl.MouseDoc.X = *x_resolved;
		FocusedCtrl.MouseDoc.Y = *y_resolved;

		return true;
	}

	// No control was clicked on. Try to find if an associated text label was clicked on. */

	DT_TX_TEXT_FLOW textflow;
	DT_SLONG char_index = GetT_EnclosingTextFlow(x, y, &textflow);
	if (char_index < 0) return false;

	DT_SLONG len = 0, dummy = 0;
	DT_SLONG compress_len;
	DT_SWORD dummy2;

	//DT_ID_SLONG ctrl_obj_name = pdPropGetAsSLong(PowerDoc, DF_PDX_READ_OBJ(PowerDoc, textflow.Img, textflow.ParentObj), fcUserParam, -1);

	//obj = DF_PDX_READ_OBJ(PowerDoc, textflow.Img, textflow.ParentObj); if (obj < 0) return false;
	obj = txImgGetRootRichTextArea(PowerDoc, textflow.Img, textflow.ParentObj, &fn_draw); if (obj < 0) return false;
	if (txObjFragmentLookup(PowerDoc, &obj, &char_index, &dummy, &len, &dummy2, &compress_len, DV_NULL) == DV_NULL) return false;

	DT_ID_SLONG obj_ctrl_invisible = pdLinkGet(PowerDoc, obj, fcControlPtr);
	if (obj_ctrl_invisible >= 0)
	{
		FocusedCtrl.Type = fcDrawButton;
		FocusedCtrl.State = FC_NORMAL;
		FocusedCtrl.PushType = 0;
		FocusedCtrl.Reserved = 0;
		FocusedCtrl.LockStatus = 0;
		FocusedCtrl.Idx = -1;
		FocusedCtrl.Obj = obj_ctrl_invisible;
		FocusedCtrl.Img = -1;
		FocusedCtrl.ParentObj = -1;
		FocusedCtrl.Lib = 0;
		FocusedCtrl.BoxDoc.W = FocusedCtrl.BoxDoc.H = 0;

		return true;
	}

	DT_ID_SLONG ctrl_obj_name = pdPropGetAsSLong(PowerDoc, obj, fcUserParam, -1);
	if (ctrl_obj_name < 0) return false;

	DT_ID_SLONG ctrl_obj = pdObjFindByNameAndDesc(PowerDoc, ctrl_obj_name, DV_NULL);
	if (ctrl_obj < 0) return false;

	//printf("char_index=%d, Img=%d, ParentObj=%d, ctrl_obj_name=%d, ctrl_obj=%d\n", char_index, textflow.Img, textflow.ParentObj, ctrl_obj_name, ctrl_obj);

	for (idx = idx_last; idx >= 0; idx--)
	{
		if ((LastDraw[idx].attribs) & 1) continue; /* hidden */

		if ((fn_draw = LastDraw[idx].fn_draw) < fcDrawFirst || fn_draw > fcDrawLast) continue;

		if ((obj = LastDraw[idx].obj) != ctrl_obj) continue;
		if ((lock_status = pdPropGetAsUByte(PowerDoc, obj, fcLockStatus, 0)) == 1) continue;

		state = GetCtrlStateAndBox(obj, &box_doc);
		FocusedCtrl.Type = LastDraw[idx].fn_draw;
		FocusedCtrl.State = state;
		FocusedCtrl.PushType = 0;
		FocusedCtrl.Reserved = 0;
		FocusedCtrl.LockStatus = lock_status;
		FocusedCtrl.Idx = idx;
		FocusedCtrl.Obj = obj;
		FocusedCtrl.Img = LastDraw[idx].img;
		FocusedCtrl.ParentObj = LastDraw[idx].parent_obj;
		FocusedCtrl.Lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, obj, fcConfigPtr), fcCnfLib, 0));
		FocusedCtrl.BoxDoc = box_doc;

		return true;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// Event Handlers


void CDTFormEngine::EvnF_MouseLPush(DT_SLONG x, DT_SLONG y, DT_SWORD click_type)
{
	DT_TX_TEXT_FLOW textflow = {0, 0};
	DT_SLONG x_resolved = 0, y_resolved = 0;
	DT_UBYTE push_type = (click_type == TX_HIT_DOUBLE ? 2 : 1);
	bool focus_changed = false, push_redraw_textbox = false, push_redraw_slider = false, push_redraw_listbox = false;

	LastState.MouseClicked = 1;

	/* new in 7.5.0.1 */
	LastState.MouseClick.X = x;
	LastState.MouseClick.Y = y;

	if (!FormControlsLib->IsInitialized) return;

	if (click_type != TX_HIT_NA) // && click_type != TX_HIT_SHIFT)
	{
		FocusCtrlByCoord(x, y, &x_resolved, &y_resolved);
		focus_changed = ChangeFocusCtrl(true, &FocusedCtrl);
	}
	else RefocusCtrl();

	LastState.MouseCoordDoc.X = x_resolved;
	LastState.MouseCoordDoc.Y = y_resolved;

	if (FocusedCtrl.Type < 0) goto end;

	if (focus_changed) push_redraw_textbox = true;
	if (click_type == TX_HIT_NA) push_redraw_slider = true;
	if (FocusedCtrl.LockStatus == 2 || FocusedCtrl.LockStatus == 3) push_redraw_listbox = true;

	textflow.Img = FocusedCtrl.Img;
	textflow.ParentObj = FocusedCtrl.ParentObj; // -2

	FocusedCtrl.PushType = push_type;

	switch (FocusedCtrl.Type)
	{
	case fcDrawButton: /* Button */
	case fcDrawRichButton: /* Rich Button */
	case fcDrawRadioButton: /* Radio Button */
	case fcDrawCheckBox: /* Check Box */

		PushCtrl(true, &FocusedCtrl, FC_PUSHED, true);
		break;

	case fcDrawTextBox: /* Text Box */
	case fcDrawRichTextBox: /* Rich Text Box */

		if (click_type != TX_HIT_NA) EvnT_HitPlus(click_type, x, y, &textflow);
		PushCtrl(true, &FocusedCtrl, FC_NOTAPPLICABLE, push_redraw_textbox);
		break;

	case fcDrawRichListBox: /* Rich Text Box */

		if (click_type != TX_HIT_NA) EvnT_HitPlus(TX_HIT_NORMAL, x, y, &textflow);
		FocusedCtrl.PushType = 0;
		if (click_type != TX_HIT_NA) SelectRichListBox(&FocusedCtrl, 0 /*y_resolved*/, click_type);
		FocusedCtrl.PushType = push_type;
		break;

	case fcDrawListBox: /* List Box */

		PushCtrl(true, &FocusedCtrl, FC_NOTAPPLICABLE, push_redraw_listbox);
		FocusedCtrl.PushType = 0;
		if (click_type != TX_HIT_NA) SelectListBox(&FocusedCtrl, y_resolved, click_type);
		FocusedCtrl.PushType = push_type;
		break;

	case fcDrawHSlider: /* Horizontal slider */

		PushCtrl(true, &FocusedCtrl, FC_PUSHED, push_redraw_slider);
		FocusedCtrl.PushType = 0;
		if (click_type != TX_HIT_NA) SelectSlider(true, x_resolved, 0);
		FocusedCtrl.PushType = push_type;
		break;

	case fcDrawVSlider: /* Vertical slider */

		PushCtrl(true, &FocusedCtrl, FC_PUSHED, push_redraw_slider);
		FocusedCtrl.PushType = 0;
		if (click_type != TX_HIT_NA) SelectSlider(false, y_resolved, 0);
		FocusedCtrl.PushType = push_type;
		break;
	}

end:

	RePaint();
}


void CDTFormEngine::EvnF_MouseMove(DT_SLONG x, DT_SLONG y)
{
	DT_TX_TEXT_FLOW textflow = {0, 0};
	DT_SLONG x_resolved;
	DT_SLONG y_resolved;
	DT_UBYTE push_type = FocusedCtrl.PushType;

	/* new in 7.5.0.1 */
	LastState.MouseClick.X = x;
	LastState.MouseClick.Y = y;

	if (!FormControlsLib->IsInitialized) return;

	if (FocusedCtrl.Type < 0) goto end;

	if (!LastState.MouseClicked) goto end;

	GetResolvedMouseXY(FocusedCtrl.Idx, &FocusedCtrl.BoxDoc, x, y, &x_resolved, &y_resolved);

	LastState.MouseCoordDoc.X = x_resolved;
	LastState.MouseCoordDoc.Y = y_resolved;

	textflow.Img = FocusedCtrl.Img;
	textflow.ParentObj = FocusedCtrl.ParentObj; // -2

	FocusedCtrl.PushType = 0;

	switch (FocusedCtrl.Type)
	{
	case fcDrawTextBox: /* Text Box */
	case fcDrawRichTextBox: /* Rich Text Box */
	case fcDrawRichListBox: /* Rich List Box */

		EvnT_HitPlus((FocusedCtrl.Type == fcDrawRichListBox ? /*TX_HIT_MOVE_ALT*/ TX_HIT_NORMAL : TX_HIT_MOVE), x, y, &textflow);

		if (FocusedCtrl.Type == fcDrawRichListBox) SelectRichListBox(&FocusedCtrl, 0 /*y_resolved*/, TX_HIT_SHIFT);

		if (y_resolved < 0 || y_resolved > FocusedCtrl.BoxDoc.H) {}
		else if (x_resolved < 0) ScrollTextBox(true, &FocusedCtrl, -1, false, false);
		else if (x_resolved > FocusedCtrl.BoxDoc.W) ScrollTextBox(true, &FocusedCtrl, +1, false, false);

		if (x_resolved < 0 || x_resolved > FocusedCtrl.BoxDoc.W) {}
		else if (y_resolved < 0) ScrollTextBox(true, &FocusedCtrl, -1, true, false);
		else if (y_resolved > FocusedCtrl.BoxDoc.H) ScrollTextBox(true, &FocusedCtrl, +1, true, false);

		break;

	case fcDrawListBox: /* List Box */

		SelectListBox(&FocusedCtrl, y_resolved, TX_HIT_SHIFT);
		break;

	case fcDrawHSlider: /* Horizontal slider */

		DragSlider(true, x_resolved);
		break;

	case fcDrawVSlider: /* Vertical slider */

		DragSlider(false, y_resolved);
		break;
	}

	FocusedCtrl.PushType = push_type;

end:

	RePaint();
}


void CDTFormEngine::EvnF_MouseLHold()
{
	DT_TX_TEXT_FLOW textflow = {0, 0};
	DT_SLONG x_resolved = LastState.MouseCoordDoc.X;
	DT_SLONG y_resolved = LastState.MouseCoordDoc.Y;
	DT_UBYTE push_type = FocusedCtrl.PushType;

	if (!FormControlsLib->IsInitialized) return;

	if (FocusedCtrl.Type < 0) goto end;

	if (!LastState.MouseClicked) goto end;

	textflow.Img = FocusedCtrl.Img;
	textflow.ParentObj = FocusedCtrl.ParentObj; // -2

	FocusedCtrl.PushType = 0;

	switch (FocusedCtrl.Type)
	{
	case fcDrawTextBox: /* Text Box */
	case fcDrawRichTextBox: /* Rich Text Box */
	case fcDrawRichListBox: /* Rich List Box */

		EvnT_HitPlus((FocusedCtrl.Type == fcDrawRichListBox ? TX_HIT_NORMAL : TX_HIT_MOVE), LastState.MouseClick.X, LastState.MouseClick.Y, &textflow); /* new in 7.5.0.1 */

		if (FocusedCtrl.Type == fcDrawRichListBox) SelectRichListBox(&FocusedCtrl, 0 /*y_resolved*/, TX_HIT_SHIFT);

		if (y_resolved < 0 || y_resolved > FocusedCtrl.BoxDoc.H) {}
		else if (x_resolved < 0) ScrollTextBox(true, &FocusedCtrl, -1, false, false);
		else if (x_resolved > FocusedCtrl.BoxDoc.W) ScrollTextBox(true, &FocusedCtrl, +1, false, false);

		if (x_resolved < 0 || x_resolved > FocusedCtrl.BoxDoc.W) {}
		else if (y_resolved < 0) ScrollTextBox(true, &FocusedCtrl, -1, true, false);
		else if (y_resolved > FocusedCtrl.BoxDoc.H) ScrollTextBox(true, &FocusedCtrl, +1, true, false);

		break;

	case fcDrawListBox: /* List Box */

		SelectListBox(&FocusedCtrl, y_resolved, TX_HIT_SHIFT);
		break;
	}

	HoldCtrl(true, &FocusedCtrl);

	FocusedCtrl.PushType = push_type;

end:

	RePaint();
}


void CDTFormEngine::EvnF_MouseLRelease(DT_SLONG x, DT_SLONG y)
{
	DT_TX_TEXT_FLOW textflow = {0, 0};

	LastState.MouseClicked = 0;
	LastState.MouseClick.X = 0;
	LastState.MouseClick.Y = 0;
	LastState.MouseCoordDoc.X = 0;
	LastState.MouseCoordDoc.Y = 0;

	if (!FormControlsLib->IsInitialized) return;

	if (FocusedCtrl.Type < 0) goto end;

	textflow.Img = FocusedCtrl.Img;
	textflow.ParentObj = FocusedCtrl.ParentObj; // -2

	switch (FocusedCtrl.Type)
	{
	case fcDrawTextBox: /* Text Box */
	case fcDrawRichTextBox: /* Rich Text Box */
	case fcDrawRichListBox: /* Rich List Box */

		EvnT_HitPlus(TX_HIT_RELEASE, x, y, &textflow);
		/* fall through */

	case fcDrawListBox: /* List Box */

		ReleaseCtrl(true, &FocusedCtrl, FC_NOTAPPLICABLE);
		break;

	case fcDrawButton: /* Button */
	case fcDrawRichButton: /* Rich Button */

		ReleaseCtrl(true, &FocusedCtrl, FC_NORMAL);
		break;

	case fcDrawRadioButton: /* Radio Button */

		ReleaseRadioButton(true, &FocusedCtrl);
		break;

	case fcDrawCheckBox: /* Check Box */

		ReleaseCheckBox(true, &FocusedCtrl);
		break;

	case fcDrawHSlider: /* Horizontal slider */

		ReleaseCtrl(true, &FocusedCtrl, FC_NORMAL);
		break;

	case fcDrawVSlider: /* Vertical slider */

		ReleaseCtrl(true, &FocusedCtrl, FC_NORMAL);
		break;
	}

end:

	FocusedCtrl.PushType = 0;
	RePaint();
}


void CDTFormEngine::EvnF_KeyChar(DT_ID_UBYTE command, const DT_UBYTE* buffer, DT_SLONG nr_of_chars, DT_UBYTE buffer_format)
{
	DT_ULONG char_code = 0;
	DT_ULONG char_space = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_SPACE);
	DT_ULONG char_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);

	if (!FormControlsLib->IsInitialized) return;

	if (FocusedCtrl.Type < 0) goto end;

	switch (FocusedCtrl.Type)
	{
	case fcDrawTextBox: /* Text Box */
	case fcDrawRichTextBox: /* Rich Text Box */

		ChangeTextBox(true, &FocusedCtrl, command, buffer, nr_of_chars, buffer_format);
		goto end;
	}

	if (nr_of_chars != 1) goto end;

	     if (buffer_format == TX_BUFFER_FORMAT_1) { char_code = DF_READ_UBYTE_LE(buffer); }
	else if (buffer_format == TX_BUFFER_FORMAT_2) { char_code = DF_READ_UWORD_LE(buffer); }
	else if (buffer_format == TX_BUFFER_FORMAT_3) { char_code = DF_READ_UTRIO_LE(buffer); }
	else if (buffer_format == TX_BUFFER_FORMAT_4) { char_code = DF_READ_ULONG_LE(buffer); }

	if (char_code == char_space || char_code == char_newline)
	{
		EvnF_MouseLPush(0, 0, TX_HIT_NA);
		EvnF_MouseLRelease(0, 0);
	}

end:

	RePaint();
}


void CDTFormEngine::EvnF_KeyCtrl(DT_ID_SWORD command)
{
	DT_TX_TEXT_FLOW textflow = {0, 0};
	bool move_forward;

	if (!FormControlsLib->IsInitialized) return;

	switch (FocusedCtrl.Type)
	{
	case fcDrawTextBox: /* Text Box */
	case fcDrawRichTextBox: /* Rich Text Box */

		     if (command == FE_CMD_PREV || command == FE_CMD_NEXT) break;
		else if (command == FE_CMD_SCROLL_DOWN) ScrollTextBox(true, &FocusedCtrl, +1, true, false);
		else if (command == FE_CMD_SCROLL_UP) ScrollTextBox(true, &FocusedCtrl, -1, true, false);
		else if (command == FE_CMD_SCROLL_RIGHT) ScrollTextBox(true, &FocusedCtrl, +1, false, false);
		else if (command == FE_CMD_SCROLL_LEFT) ScrollTextBox(true, &FocusedCtrl, -1, false, false);
		else if (command == TX_CMD_DOWN || command == TX_CMD_SHIFT_DOWN) MoveCursor_Text(&FocusedCtrl, command, +1, true);
		else if (command == TX_CMD_UP || command == TX_CMD_SHIFT_UP) MoveCursor_Text(&FocusedCtrl, command, -1, true);
		else if (command == TX_CMD_RIGHT || command == TX_CMD_SHIFT_RIGHT) MoveCursor_Text(&FocusedCtrl, command, +1, false);
		else if (command == TX_CMD_LEFT || command == TX_CMD_SHIFT_LEFT) MoveCursor_Text(&FocusedCtrl, command, -1, false);
		else CmdT_Command(command);

		goto end;
		break;

	case fcDrawRichListBox: /* Rich List Box */

		if (command == TX_CMD_DOWN || command == FE_CMD_SCROLL_DOWN)
		{
			/*if (command == FE_CMD_SCROLL_DOWN)*/ LastState.SelectedListBoxItemOld = -1;
			MoveCursor_RichListBox(&FocusedCtrl, TX_CMD_DOWN, +1, TX_HIT_NORMAL, true);
			//CmdT_Command(TX_CMD_DOWN); SelectRichListBox(&FocusedCtrl, 0, TX_HIT_NORMAL /*TX_HIT_SHIFT*/); /*ScrollTextBox(true, &FocusedCtrl, +1, true, false);*/ /* NORMAL SELECT */
			goto end;
		}
		else if (command == TX_CMD_SHIFT_DOWN)
		{
			MoveCursor_RichListBox(&FocusedCtrl, TX_CMD_DOWN, +1, TX_HIT_SHIFT, true);
			//CmdT_Command(TX_CMD_DOWN); SelectRichListBox(&FocusedCtrl, 0, TX_HIT_SHIFT);
			goto end;
		}
		else if (command == TX_CMD_UP || command == FE_CMD_SCROLL_UP)
		{
			/*if (command == FE_CMD_SCROLL_UP)*/ LastState.SelectedListBoxItemOld = -1;
			MoveCursor_RichListBox(&FocusedCtrl, TX_CMD_UP, -1, TX_HIT_NORMAL, true);
			//CmdT_Command(TX_CMD_UP); SelectRichListBox(&FocusedCtrl, 0, TX_HIT_NORMAL /*TX_HIT_SHIFT*/); /*ScrollTextBox(true, &FocusedCtrl, -1, true, false);*/ /* NORMAL SELECT */
			goto end;
		}
		else if (command == TX_CMD_SHIFT_UP)
		{
			MoveCursor_RichListBox(&FocusedCtrl, TX_CMD_UP, -1, TX_HIT_SHIFT, true);
			//CmdT_Command(TX_CMD_UP); SelectRichListBox(&FocusedCtrl, 0, TX_HIT_SHIFT);
			goto end;
		}
		break;

	case fcDrawListBox: /* List Box */

		     if (command == TX_CMD_DOWN || command == FE_CMD_SCROLL_DOWN) { /*if (command == FE_CMD_SCROLL_DOWN)*/ LastState.SelectedListBoxItemOld = -1; RelSelectionInListBox(true, &FocusedCtrl, true, 0); /* NORMAL SELECT */ goto end; }
		else if (command == TX_CMD_UP || command == FE_CMD_SCROLL_UP) { /*if (command == FE_CMD_SCROLL_UP)*/ LastState.SelectedListBoxItemOld = -1; RelSelectionInListBox(true, &FocusedCtrl, false, 0); /* NORMAL SELECT */ goto end; }
		else if (command == TX_CMD_SHIFT_DOWN) { RelSelectionInListBox(true, &FocusedCtrl, true, 1); /* SELECT WITH SHIFT */ goto end; }
		else if (command == TX_CMD_SHIFT_UP) { RelSelectionInListBox(true, &FocusedCtrl, false, 1); /* SELECT WITH SHIFT */ goto end; }
		break;

	case fcDrawHSlider: /* Horizontal slider */

		     if (command == TX_CMD_RIGHT || command == FE_CMD_SCROLL_RIGHT) { SelectSlider(true, 0, 1); goto end; }
		else if (command == TX_CMD_LEFT || command == FE_CMD_SCROLL_LEFT) { SelectSlider(true, 0, -1); goto end; }
		break;

	case fcDrawVSlider: /* Vertical slider */

		     if (command == TX_CMD_DOWN || command == FE_CMD_SCROLL_DOWN) { SelectSlider(false, 0, 1); goto end; }
		else if (command == TX_CMD_UP || command == FE_CMD_SCROLL_UP) { SelectSlider(false, 0, -1); goto end; }
		break;
	}

	     if (command == TX_CMD_NEXTCHAR || command == TX_CMD_DOWN || command == TX_CMD_RIGHT || command == FE_CMD_NEXT) move_forward = true;
	else if (command == TX_CMD_PREVCHAR || command == TX_CMD_UP || command == TX_CMD_LEFT || command == FE_CMD_PREV) move_forward = false;
	else goto end;

	FocusCtrlByOrder(move_forward);
	ChangeFocusCtrl(true, &FocusedCtrl);

	if (FocusedCtrl.Type < 0) goto end;

	textflow.Img = FocusedCtrl.Img;
	textflow.ParentObj = FocusedCtrl.ParentObj; // -2

	if (FocusedCtrl.Type == fcDrawTextBox || FocusedCtrl.Type == fcDrawRichTextBox || FocusedCtrl.Type == fcDrawRichListBox) EvnT_HitPlus(TX_HIT_NORMAL, 0, 0, &textflow);
	DrawCtrl(&FocusedCtrl);

end:

	RePaint();
}


void CDTFormEngine::EvnF_MouseScroll(DT_SLONG /*x*/, DT_SLONG /*y*/, DT_SLONG dx, DT_SLONG dy)
{
	if (abs(dy) > abs(dx)) EvnF_KeyCtrl(dy < 0 ? FE_CMD_SCROLL_DOWN : FE_CMD_SCROLL_UP); else EvnF_KeyCtrl(dx < 0 ? FE_CMD_SCROLL_RIGHT : FE_CMD_SCROLL_LEFT);
}


void CDTFormEngine::Reset(bool refresh)
{
	ResetTextBoxStats(-1, "Reset");

	txTextCommand(TextDoc, TX_CMD_RESET, DV_NULL, 0);
	//CmdT_Command(TX_CMD_RESET); /* new */

	FocusedCtrl.Reset(FC_NORMAL);

	FocusedTextCtrl = FocusedCtrl;

	PaintArea.xmn = PaintArea.ymn = PaintArea.xmx = PaintArea.ymx = 0;
	PaintFlag = 0;

	if (refresh && FormControlsLib->IsInitialized) ChangeFocusCtrl(true, &FocusedCtrl); //DrawCtrl(&PrevioslyFocusedCtrl);
	PrevioslyFocusedCtrl = FocusedCtrl;

	LastState.MouseClicked = 0;
	LastState.MouseClick.X = 0;
	LastState.MouseClick.Y = 0;
	LastState.MouseCoordDoc.X = 0;
	LastState.MouseCoordDoc.Y = 0;
	LastState.SliderPositionOriginal = 0;
	LastState.SliderScrollableRange = 0;
	LastState.SelectedListBoxItemOld = -1;
}


/////////////////////////////////////////////////////////////////////////////
// Events


bool CDTFormEngine::ChangeFocusCtrl(bool do_events, const CDTControl* ctrl)
{
	// Clear text selection
	//if (ctrl->Type != fcDrawTextBox && ctrl->Type != fcDrawRichTextBox) CmdT_Command(TX_CMD_RESET);

	// Don't clear text selection is selected text control is the same as the previous one.
	if ((ctrl->Type == fcDrawTextBox || ctrl->Type == fcDrawRichTextBox || ctrl->Type == fcDrawRichListBox) && (ctrl->Idx == PrevioslyFocusedCtrl.Idx)) {}
	// Clear text selection unless selected control is a text box, button or horizontal/vertical slider...
	else if (ctrl->Type != fcDrawTextBox && ctrl->Type != fcDrawRichTextBox && ctrl->Type != fcDrawRichListBox && ctrl->Type != fcDrawButton && ctrl->Type != fcDrawHSlider && ctrl->Type != fcDrawVSlider) CmdT_Command(TX_CMD_RESET);
	// ...in which case clear text selection if selected control is unconnected.
	else if (pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcConnectionID, 0) != pdPropGetAsSLong(PowerDoc, PrevioslyFocusedCtrl.Obj, fcConnectionID, 1)) CmdT_Command(TX_CMD_RESET);

	if (ctrl->Idx == PrevioslyFocusedCtrl.Idx) return false;

	DrawCtrl(&PrevioslyFocusedCtrl);

	LastState.SliderPositionOriginal = 0;
	LastState.SliderScrollableRange = 0;
	LastState.SelectedListBoxItemOld = -1;

	if (PrevioslyFocusedCtrl.Type != -1 && do_events) /* Old control loses focus */
	{
		Handler_Invoke(fcOnCtrlFocusLose, &PrevioslyFocusedCtrl);
	}

	PrevioslyFocusedCtrl = *ctrl;
	if (ctrl->Type == fcDrawTextBox || ctrl->Type == fcDrawRichTextBox || ctrl->Type == fcDrawRichListBox) FocusedTextCtrl = *ctrl;

	if (ctrl->Type != -1 && do_events) /* New control receives focus */
	{
		Handler_Invoke(fcOnCtrlFocusReceive, ctrl);
	}

	return true;
}


void CDTFormEngine::PushCtrl(bool do_events, const CDTControl* ctrl, DT_UBYTE state_flag, bool redraw)
{
	if (state_flag != FC_NOTAPPLICABLE)
	{
		/* Change status to state_flag */
		pdPropApplyAsUByte(PowerDoc, ctrl->Obj, fcState, state_flag, 0, PD_APPLY_STANDARD);
	}

	if (redraw) DrawCtrl(ctrl);

	if (!do_events) return;
	Handler_Invoke(fcOnCtrlPush, ctrl);
}


void CDTFormEngine::HoldCtrl(bool do_events, const CDTControl* ctrl)
{
	if (!do_events) return;
	Handler_Invoke(fcOnCtrlHold, ctrl);
}


void CDTFormEngine::ReleaseCtrl(bool do_events, const CDTControl* ctrl, DT_UBYTE state_flag)
{
	if (state_flag != FC_NOTAPPLICABLE)
	{
		/* Change status to state_flag */
		pdPropApplyAsUByte(PowerDoc, ctrl->Obj, fcState, state_flag, 0, PD_APPLY_STANDARD);
		DrawCtrl(ctrl);
	}

	if (!do_events) return;
	Handler_Invoke(fcOnCtrlRelease, ctrl);
}


void CDTFormEngine::ReleaseCheckBox(bool do_events, const CDTControl* ctrl)
{
	DT_UBYTE state_flag = FC_NORMAL;

	if (ctrl->LockStatus == 2 || ctrl->LockStatus == 3) state_flag = ctrl->State; /* Don't change the state */
	else
	{
		if (ctrl->State == FC_NORMAL) state_flag = FC_SELECTED;
		else state_flag = FC_NORMAL;
	}

	ReleaseCtrl(do_events, ctrl, state_flag);

	if (ctrl->LockStatus == 2 || ctrl->LockStatus == 3) return;

	if (!do_events) return;
	Handler_Invoke(fcOnCtrlChange, ctrl);
}


void CDTFormEngine::ReleaseRadioButton(bool do_events, const CDTControl* ctrl)
{
	DT_SLONG len0, len;
	DT_SLONG idx_last = LastDrawIdx;
	DT_UBYTE fc_state = 0;
	DT_UBYTE* fc_name = DV_NULL;
	DT_UBYTE* fc_name_other = DV_NULL;
	DT_UBYTE state_flag_new, state_flag_old;

	CDTControl ctrl_radio;
	ctrl_radio.Type = fcDrawRadioButton;

	state_flag_old = ctrl->State;
	if (ctrl->LockStatus == 2 || ctrl->LockStatus == 3) state_flag_new = state_flag_old; /* Don't change the state */
	else state_flag_new = FC_SELECTED;

	fc_name = pdPropGet(PowerDoc, ctrl->Obj, fcName, &len0);

	/* Search for other selected radio buttons of the same name */
	for (ctrl_radio.Idx = 0; ctrl_radio.Idx <= idx_last; ctrl_radio.Idx++)
	{
		if (LastDraw[ctrl_radio.Idx].fn_draw != fcDrawRadioButton) continue;

		ctrl_radio.Obj = LastDraw[ctrl_radio.Idx].obj;
		ctrl_radio.Img = LastDraw[ctrl_radio.Idx].img;
		ctrl_radio.ParentObj = LastDraw[ctrl_radio.Idx].parent_obj;

		/* if this is the radio button the user is releasing, ignore it */
		if (ctrl_radio.Obj == ctrl->Obj) continue;

		/* does the radio button have the same name? */
		//fc_name_other = DV_NULL;
		fc_name_other = pdPropGet(PowerDoc, ctrl_radio.Obj, fcName, &len);
		if (len0 != len) continue;
		if (dtMemCompare(fc_name, fc_name_other, len) != 0) continue;

		/* if so, is it selected? */
		fc_state = pdPropGetAsUByte(PowerDoc, ctrl_radio.Obj, fcState, FC_NORMAL);
		if (fc_state == FC_NORMAL) continue;

		/* our radio button not in FC_NORMAL state, so let's change it to FC_NORMAL and draw it */
		pdPropApplyAsUByte(PowerDoc, ctrl_radio.Obj, fcState, 0, 0, PD_APPLY_STANDARD);
		ctrl_radio.State = GetCtrlStateAndBox(LastDraw[ctrl_radio.Idx].obj, &ctrl_radio.BoxDoc);
		DrawCtrl(&ctrl_radio);

		if (!do_events) continue;
		Handler_Invoke(fcOnCtrlChange, &ctrl_radio);
	}

	ReleaseCtrl(do_events, ctrl, state_flag_new);

	if (ctrl->LockStatus == 2 || ctrl->LockStatus == 3) return;

	if ((!do_events) || state_flag_new == state_flag_old) return;
	Handler_Invoke(fcOnCtrlChange, ctrl);
}


void CDTFormEngine::ChangeTextBox(bool do_events, const CDTControl* ctrl, DT_ID_UBYTE command, const DT_UBYTE* buffer, DT_SLONG nr_of_chars, DT_UBYTE buffer_format)
{
	DT_UBYTE* buffer_filtered = DV_NULL;

	if (ctrl->LockStatus == 2 || ctrl->LockStatus == 3) return;

	if (nr_of_chars > 0 && buffer != DV_NULL && (ctrl->LockStatus == 4 || pdPropGetAsUByte(PowerDoc, ctrl->Obj, fcTextScrollType, 0) != 0))
	{
		/* If a text box is scrolled by letter or if scrolling is disabled, then it's likely a single text line. In this case filter out any newline characters */

		DT_SLONG len = dtSecMul(nr_of_chars, buffer_format);
		if ((buffer_filtered = static_cast<DT_UBYTE*>(dtMemAlloc(len))) == DV_NULL) return;

		const DT_UBYTE* src = buffer;
		DT_UBYTE* dst = buffer_filtered;
		DT_ULONG char_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);
		DT_ULONG char_code = 0;
		DT_SLONG i, n = 0;

		for (i = 0; i < nr_of_chars; i++)
		{
			     if (buffer_format == TX_BUFFER_FORMAT_1) char_code = DF_READ_UBYTE_LE(src);
			else if (buffer_format == TX_BUFFER_FORMAT_2) char_code = DF_READ_UWORD_LE(src);
			else if (buffer_format == TX_BUFFER_FORMAT_3) char_code = DF_READ_UTRIO_LE(src);
			else if (buffer_format == TX_BUFFER_FORMAT_4) char_code = DF_READ_ULONG_LE(src);

			src += buffer_format;

			if (char_code == char_newline) continue;

			     if (buffer_format == TX_BUFFER_FORMAT_1) DF_WRITE_UBYTE_LE(dst, (DT_UBYTE)char_code);
			else if (buffer_format == TX_BUFFER_FORMAT_2) DF_WRITE_UWORD_LE(dst, (DT_UWORD)char_code);
			else if (buffer_format == TX_BUFFER_FORMAT_3) DF_WRITE_UTRIO_LE(dst, char_code);
			else if (buffer_format == TX_BUFFER_FORMAT_4) DF_WRITE_ULONG_LE(dst, char_code);

			dst += buffer_format;
			n++;
		}

		buffer = buffer_filtered;
		nr_of_chars = n;
	}

	ActT_Paste(command, nr_of_chars, buffer, buffer_format);

	if (buffer_filtered != DV_NULL) dtMemFree(buffer_filtered);

	if (!do_events) return;
	Handler_Invoke(fcOnCtrlChange, ctrl);
}


void CDTFormEngine::MoveCursor_Text(const CDTControl* ctrl, DT_ID_SWORD command, DT_SLONG scroll_amount, bool by_row)
{
	DT_RECT_SLONG extent;

	DT_SLONG fc0, fc1;
	DT_SLONG lc0, lc1;
	DT_SWORD n0 = txTextGetSelect(TextDoc, &fc0, &lc0, 1);
	DT_SWORD ret = txTextCommand(TextDoc, command, &extent, 0);
	DT_SWORD n1 = txTextGetSelect(TextDoc, &fc1, &lc1, 1);

	if (n0 == n1 && fc0 == fc1 && lc0 == lc1)
	{
		if (!ScrollTextBox(true, ctrl, scroll_amount, by_row, false)) return;

		//printf("no change\n");

		//Redraw(true);
		//RedrawArea(extent.xmn, extent.ymn, extent.xmx - extent.xmn, extent.ymx - extent.ymn);

		DT_PDX_XYWH_SLONG extent_pix;
		GetCtrlExtentPix(ctrl->Idx, &ctrl->BoxDoc, &extent_pix);
		RedrawArea(extent_pix.X, extent_pix.Y, extent_pix.W, extent_pix.H);
		//UpdatePaintArea(extent_pix.X, extent_pix.Y, extent_pix.X + extent_pix.W, extent_pix.Y + extent_pix.H);

		txTextCommand(TextDoc, command, &extent, 0);
	}
	else if (ret != 2) UpdatePaintArea(extent.xmn, extent.ymn, extent.xmx, extent.ymx);

}


void CDTFormEngine::MoveCursor_RichListBox(const CDTControl* ctrl, DT_ID_SWORD command, DT_SLONG scroll_amount, DT_SWORD click_type, bool by_row)
{
	if (click_type == TX_HIT_SHIFT)
	{
		DT_UBYTE allow_multi_selection = pdPropGetAsUByte(PowerDoc, ctrl->Obj, fcListMultiSelect, 0);
		if (allow_multi_selection != 1) pdPropDel(PowerDoc, ctrl->Obj, fcListSelectedItems); /*to reduce blinking */
	}
	else pdPropDel(PowerDoc, ctrl->Obj, fcListSelectedItems); /*to reduce blinking */

	MoveCursor_Text(ctrl, command, scroll_amount, by_row);
	SelectRichListBox(ctrl, 0, click_type /*TX_HIT_SHIFT*/); /*ScrollTextBox(true, ctrl, +1, true, false);*/ /* NORMAL SELECT */
}


bool CDTFormEngine::GetTextBoxStats(const CDTControl* ctrl)
{
	DT_ID_SWORD fn_draw = (ctrl->Idx < 0 ? static_cast<DT_ID_SWORD>(pdDrawNothing) : LastDraw[ctrl->Idx].fn_draw);
	return GetTextBoxStats(fn_draw, ctrl->Obj);
}


bool CDTFormEngine::GetTextBoxStats(DT_ID_SWORD fn_draw, DT_ID_SLONG obj)
{
	if (fn_draw != fcDrawTextBox && fn_draw != fcDrawRichTextBox && fn_draw != fcDrawRichListBox) return false;

	DT_ID_SLONG config = pdLinkGet(PowerDoc, obj, fcConfigPtr);
	DT_ID_SWORD lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, config, fcCnfLib, 0));
	DT_ID_SLONG obj_text = FormControlsLib->DTTextBox[lib].GetObjText();
	//DT_ID_SLONG ret = -1;

	if (obj_text < 0) return false;

	if (LastTextStatsObj == obj) return true; /* cached text stats are still valid */

	DT_PDDOC source_doc = DV_NULL;
	DT_UBYTE wrap_type = pdPropGetAsUByte(PowerDoc, obj, fcTextWrapType, 0); if (fn_draw == fcDrawRichListBox) wrap_type = 4;
	DT_SLONG width = pdPropGetAsSLong(PowerDoc, obj, fcDw, 500);
	DT_SLONG height = 0; /* 999999 */ //pdPropGetAsSLong(PowerDoc, obj, fcDh, 100);
	DT_SLONG source_page = 0;

	if (config != -1) source_page = pdPropGetAsUByte(PowerDoc, config, fcCnfCharBox, 0);

	ResetTextBoxStats(obj, "GetTextBoxStats");

	if (fn_draw == fcDrawTextBox || fn_draw == fcDrawRichTextBox || fn_draw == fcDrawRichListBox)
	{
		//DT_SLONG stats[] = {0, 0, 0, 2 /* dynamic stats with extra_len */, 2 /* get char_idx_max */ , 0, 0, 256 /* extra_len */};
		DT_SLONG stats[] = {0, 0, 0, 2 /* dynamic stats with extra_len */, 9 /* get MaxCharIdx */ , 0, 0, 256 /* extra_len */};
		DT_SLONG* text_stats = stats;

		source_doc = FormControlsLib->DTTextBox[lib].Get(false /*focus*/, source_page, width, height, PowerDoc, obj, 0 /*first_item*/, DV_NULL, 0, wrap_type, 10, true, 0);
		//ret = pdTextReflow(source_doc, pdDrawRichTextArea, 0, obj_text, DV_NULL /* must be DV_NULL to get extra stats */, TextStats);
		/*ret =*/ pdTextReflowPlus(source_doc, pdDrawRichTextArea, 0, obj_text, DV_NULL, &text_stats);
		//printf("GetTextBoxStats for object %d\n", obj);
		//printf("pdTextReflowPlus (ret = %d, text_stats = %d)\n", ret, text_stats);
		TextStats = text_stats; /* text_stats is dynamically allocated */
	}

	//if (TextStats[2] <= FE_MAX_TEXTSTATS) return true;
	if (/*ret != -1 &&*/ TextStats != DV_NULL) return true;

	ResetTextBoxStats(-1, "GetTextBoxStats");
	return false;
}


DT_SLONG CDTFormEngine::GetTextBoxLimitAndCoverage(const CDTControl* ctrl, DT_UWORD* coverage)
{
	DT_ID_SWORD fn_draw = (ctrl->Idx < 0 ? static_cast<DT_ID_SWORD>(pdDrawNothing) : LastDraw[ctrl->Idx].fn_draw);
	return GetTextBoxLimitAndCoverage(fn_draw, ctrl->Obj, ctrl->BoxDoc.H, coverage);
}


DT_SLONG CDTFormEngine::GetTextBoxLimitAndCoverage(DT_ID_SWORD fn_draw, DT_ID_SLONG obj, DT_SLONG h, DT_UWORD* coverage)
{
	*coverage = 1000;
	if (!GetTextBoxStats(fn_draw, obj)) return -1;

	DT_SLONG limit = DF_MAX(0, TextStats[2] - 1);
	DT_FLOAT percent = (TextStats[5] > 0 ? h / (DT_FLOAT)(h + TextStats[5]) : 1.0);
	if (percent < 0.2) percent = 0.2; else if (percent > 1.0) percent = 1.0;
	if (limit <= 0) percent = 1.0;

	*coverage = (DT_UWORD)DF_ROUND(percent * 10000);
	return limit;
}


DT_SLONG CDTFormEngine::GetTextBoxFirstItem(const CDTControl* ctrl, DT_SLONG line_break_item)
{
	bool by_row = (pdPropGetAsUByte(PowerDoc, ctrl->Obj, fcTextScrollType, 0) == 0);

	if (!by_row) return line_break_item;

	if (!GetTextBoxStats(ctrl)) return 0;

	DT_SLONG i, j = TextStats[2] - 1;

	/* find smallest char index larger than line_break_item */

	for (i = 0; i <= j; i++)
	{
		if (TextStats[8 + i] > line_break_item) { /*first_item = TextStats[8 + i] + 1;*/ break; }
	}

	return i;
}


DT_SLONG CDTFormEngine::GetTextBoxFirstRow(const CDTControl* ctrl, bool cache /* = true */)
{
	/* non-text control (fcItemFirst is the row) */

	if (ctrl->Type != fcDrawTextBox && ctrl->Type != fcDrawRichTextBox && ctrl->Type != fcDrawRichListBox) return pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, 0);

	/* cached text control */

	DT_SLONG len = 0;
	if (pdPropGet(PowerDoc, ctrl->Obj, fcRowFirst, &len) != DV_NULL) return pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcRowFirst, 0);

	/* non-cached text control */

	DT_SLONG first_item = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, 0);
	DT_SLONG first_row = GetTextBoxFirstItem(ctrl, first_item);
	if (cache) pdPropApplyAsSLong(PowerDoc, ctrl->Obj, fcRowFirst, first_row, 0, PD_APPLY_STANDARD);

	return first_row;
}


DT_SLONG CDTFormEngine::GetTextBoxRowInfo(const CDTControl* ctrl, DT_SLONG line_break_item, DT_SLONG* first_char_index, DT_SLONG* last_char_index)
{
	*first_char_index = 0;
	*last_char_index = 0;

	bool by_row = (pdPropGetAsUByte(PowerDoc, ctrl->Obj, fcTextScrollType, 0) == 0);

	if (!by_row) { *first_char_index = line_break_item; *last_char_index = line_break_item; return line_break_item; }

	if (!GetTextBoxStats(ctrl)) return 0;

	DT_SLONG i, j = TextStats[2] - 1;

	for (i = 0; i <= j; i++)
	{
		if (i >= line_break_item) { *first_char_index = ((i > 0) ? (TextStats[8 + i - 1] + 1) : 0); *last_char_index = TextStats[8 + i]; break; }
	}

	if (*first_char_index == *last_char_index && i == j) { *first_char_index = 0; *last_char_index = 0; } /* last row is empty newline */

	return i; /* row_index */
}


bool CDTFormEngine::ScrollTextBox(bool do_events, const CDTControl* ctrl, DT_SLONG scroll_amount, bool by_row, bool absolute_scroll)
{
	if (ctrl->Idx < 0) return false;

	DT_UBYTE lock_status = pdPropGetAsUByte(PowerDoc, ctrl->Obj, fcLockStatus, 0);
	DT_SLONG first_item = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, 0), first_item_old = first_item;
	DT_SLONG line_break_item;
	DT_SLONG len = 0, i, j;

	if (lock_status == 2 || lock_status == 4) return false;

	DT_ID_SWORD fn_draw = LastDraw[ctrl->Idx].fn_draw;

	//DT_UBYTE buffer_format = 0;
	//
	//if (fn_draw == fcDrawTextBox)
	//{
	//	if (CDTValue::GetPowerDocProp(PowerDoc, ctrl->Obj, &len2, &buffer_format) == DV_NULL) return false;
	//	len2 /= buffer_format;
	//}
	//else
	//{
		len = txObjCharsCount(PowerDoc, ctrl->Obj, txTextGetConfig(TextDoc, -1, fn_draw));
	//}

	if (len < 1) return false;
	//len--;

	bool scroll_type_by_row = (pdPropGetAsUByte(PowerDoc, ctrl->Obj, fcTextScrollType, 0) == 0);

	if (scroll_type_by_row != by_row) return false;

	if (by_row) { if (!GetTextBoxStats(ctrl)) return false; }

	if (by_row && absolute_scroll) /* absolute scroll by row */
	{
		if (scroll_amount <= 0) first_item = 0;
		else
		{
			if (scroll_amount >= TextStats[2] - 1) scroll_amount = TextStats[2] - 1;
			first_item = TextStats[8 + scroll_amount - 1] + 1;
		}
	}
	else if (absolute_scroll) first_item = scroll_amount; /* absolute scroll by character */
	else if (by_row && scroll_amount < 0) /* relative scroll by row -- decrease first_item */
	{
		for (j = 0; j < -scroll_amount; j++)
		{
			line_break_item = first_item - 1;
			first_item = 0; /* if the loop does not find anything, this will be the result */
			/* find largest char index smaller than first_item */
			for (i = TextStats[2] - 1; i >= 0; i--)
			{
				if (TextStats[8 + i] < line_break_item) { first_item = TextStats[8 + i] + 1; break; }
			}
		}
	}
	else if (by_row && scroll_amount > 0) /* relative scroll by row -- increase first_item */
	{
		for (j = 0; j < scroll_amount; j++)
		{
			line_break_item = first_item - 1;
			/* find smallest char index larger than first_item */
			for (i = 0; i <= TextStats[2] - 1; i++)
			{
				if (TextStats[8 + i] > line_break_item) { first_item = TextStats[8 + i] + 1; break; }
			}
		}
	}
	else first_item += scroll_amount; /* relative scroll by character */

	if (first_item < 0) first_item = 0; else if (first_item > len) first_item = len;

	if (first_item_old == first_item) return false;

	pdPropApplyAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, first_item, 0, PD_APPLY_STANDARD);

	if (by_row) /* GetTextBoxFirstItem */
	{
		/* find smallest char index larger than first_item */
		for (i = 0; i <= TextStats[2] - 1; i++)
		{
			if (TextStats[8 + i] > first_item) break;
		}

		pdPropApplyAsSLong(PowerDoc, ctrl->Obj, fcRowFirst, i, 0, PD_APPLY_STANDARD);
	}
	else pdPropDel(PowerDoc, ctrl->Obj, fcRowFirst);

	/*if (ctrl->Idx >= 0)*/ CmdT_Refresh(LastDraw[ctrl->Idx].img);
	DrawCtrl(ctrl);

	if (!do_events) return true;
	Handler_Invoke(fcOnCtrlScroll, ctrl);

	return true;
}


void CDTFormEngine::SelectSlider(bool hor_ver_flag, DT_SLONG t, DT_SWORD direction)
{
	DT_SLONG l;
	DT_SLONG slider_position_new, scroll_amount = 1000;
	DT_SLONG slider_limit = 100, slider_position = 0;
	DT_UWORD slider_coverage = 1000;
	DT_SWORD slider_direction;
	DT_FLOAT coverage_factor;

	if (hor_ver_flag)
	{
		l = FocusedCtrl.BoxDoc.W;
		//t -= FocusedCtrl.BoxDoc.X;
	}
	else
	{
		l = FocusedCtrl.BoxDoc.H;
		//t -= FocusedCtrl.BoxDoc.Y;
	}

	DT_UBYTE flags = pdPropGetAsUByte(PowerDoc, FocusedCtrl.Obj, fcSliderFlags, 0);

	slider_position = CDTSlider::GetInfoFromPowerDoc(PowerDoc, FocusedCtrl.Obj, &slider_limit, &slider_coverage);
	coverage_factor = slider_coverage * 0.0001;

	if (flags == 1 || flags == 2 /* operate on coverage */) { LastState.SliderScrollableRange = l; slider_position = slider_coverage; }
	else LastState.SliderScrollableRange = DF_ROUND((1 - coverage_factor) * l);

	if (direction == 0)
	{
		DT_FLOAT position_factor = (slider_limit == 0 ? 0 : slider_position / (DT_FLOAT)slider_limit);
		slider_direction = FormControlsLib->DTHorSlider[FocusedCtrl.Lib].GetDirection(l, coverage_factor, position_factor, t);
	}
	else
	{
		slider_direction = direction;
	}

	     if (flags == 1 /* operate on coverage */) {}
	else if (flags == 2 /* operate on coverage */) { slider_direction = -slider_direction; }
	else scroll_amount = DF_ROUND(coverage_factor * slider_limit);

	if (scroll_amount < 1) scroll_amount = 1;

	if (slider_direction < 0) slider_position_new = slider_position - scroll_amount;
	else if (slider_direction > 0) slider_position_new = slider_position + scroll_amount;
	else
	{
		LastState.SliderPositionOriginal = slider_position;
		DrawCtrl(&FocusedCtrl);
		return;
	}

	LastState.SliderPositionOriginal = MoveSliderToPosition(true, &FocusedCtrl, slider_position_new);
}


void CDTFormEngine::DragSlider(bool hor_ver_flag, DT_SLONG t)
{
	DT_SLONG slider_position_new;
	DT_SLONG slider_limit = 10000;

	DT_UBYTE flags = pdPropGetAsUByte(PowerDoc, FocusedCtrl.Obj, fcSliderFlags, 0);

	if (flags == 1 || flags == 2 /* operate on coverage */) {}
	else
	{
		slider_limit = pdPropGetAsSLong(PowerDoc, FocusedCtrl.Obj, fcSliderLimit, 100);
		if (hor_ver_flag) t -= FocusedCtrl.MouseDoc.X; else t -= FocusedCtrl.MouseDoc.Y;
	}

	DT_FLOAT k = (LastState.SliderScrollableRange == 0 ? 0 : t / (DT_FLOAT)LastState.SliderScrollableRange);

	     if (flags == 1 /* operate on coverage */) slider_position_new = DF_ROUND(slider_limit * k);
	else if (flags == 2 /* operate on coverage */) slider_position_new = DF_ROUND(slider_limit * (1 - k));
	else slider_position_new = LastState.SliderPositionOriginal + DF_ROUND(slider_limit * k);

	MoveSliderToPosition(true, &FocusedCtrl, slider_position_new);
}


DT_SLONG CDTFormEngine::MoveSliderToPosition(bool do_events, const CDTControl* ctrl, DT_SLONG slider_position, DT_SLONG slider_limit /*= -1 */, DT_UWORD slider_coverage /*= 0 */)
{
	DT_UBYTE flags = pdPropGetAsUByte(PowerDoc, ctrl->Obj, fcSliderFlags, 0);
	DT_SLONG prop = fcSliderCoverage; if (flags == 1 || flags == 2 /* operate on coverage */) {} else prop = fcSliderPosition;

	DT_SLONG slider_limit_old = 10000;
	DT_SLONG slider_position_old = pdPropGetAsSLong(PowerDoc, ctrl->Obj, prop, 0);
	DT_UBYTE lock_status = pdPropGetAsUByte(PowerDoc, ctrl->Obj, fcLockStatus, 0);

	if (lock_status == 2 || lock_status == 3) return slider_position_old;

	if (flags == 1 || flags == 2 /* operate on coverage */) {} else slider_limit_old = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcSliderLimit, 100);

	if (slider_limit >= 0) /* no scroll, just update limit and coverge */
	{
		DT_UWORD slider_coverage_old = pdPropGetAsUWord(PowerDoc, ctrl->Obj, fcSliderCoverage, 1000);

		if (slider_coverage > 10000) slider_coverage = 10000;
		if (slider_limit_old == slider_limit && slider_coverage_old == slider_coverage) return slider_position_old;

		pdPropApplyAsUWord(PowerDoc, ctrl->Obj, fcSliderCoverage, slider_coverage, 0, PD_APPLY_STANDARD);
		pdPropApplyAsSLong(PowerDoc, ctrl->Obj, fcSliderLimit, slider_limit, 0, PD_APPLY_STANDARD);
		DrawCtrl(ctrl);

		return slider_position_old;
	}

	if (slider_position < 0) slider_position = 0; else if (slider_position > slider_limit_old) slider_position = slider_limit_old;
	if (slider_position_old == slider_position) return slider_position;

	pdPropApplyAsSLong(PowerDoc, ctrl->Obj, prop, slider_position, 0, PD_APPLY_STANDARD);
	DrawCtrl(ctrl);

	if (!do_events) return slider_position;
	Handler_Invoke(fcOnCtrlChange, ctrl);

	return slider_position;
}


DT_SLONG CDTFormEngine::SelectRichListBox(const CDTControl* ctrl, DT_SLONG /*y*/, DT_SWORD click_type)
{
	//return SelectListBox(ctrl, y, click_type);

	//if (GetTextBoxStats(ctrl)) printf("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", TextStats[0], TextStats[1], TextStats[2], TextStats[3], TextStats[4], TextStats[5], TextStats[6], TextStats[7], TextStats[8], TextStats[9], TextStats[10], TextStats[11]);

	DT_SLONG extra[] = {0, 0, 0, 0};
	DT_SLONG char_index = 0;
	DT_SLONG selected_item, first_item;

	if (txTextInfo(TextDoc, TX_INFO_CURSOR, 0, 0, &char_index, extra) != 1) return 0;

	selected_item = extra[1]; /* Row in the frameset that corresponds to the returned info value; -1 if not found */

	if (selected_item < 0) return 0; /* not found - likely due to scroll */

	//first_item = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, 0);
	//first_item = GetTextBoxFirstItem(ctrl, first_item);

	//first_item = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcRowFirst, 0);
	first_item = GetTextBoxFirstRow(ctrl);

	//printf("first_item=%d, extra[1]=%d\n", first_item, extra[1]);

	selected_item += first_item;

//	DT_UBYTE select_array[64];
//	//printf("char_index=%d, selected_item=%d, first_item=%d\n", char_index, selected_item, first_item);
//
//	DF_WRITE_SLONG_LE(select_array, selected_item);
//	pdPropApply(PowerDoc, ctrl->Obj, fcListSelectedItems, select_array, sizeof(DT_SLONG), 0);
//
//	Handler_Invoke(fcOnCtrlChange, ctrl);
//	return selected_item;


	if (LastState.SelectedListBoxItemOld == selected_item && click_type != TX_HIT_CTRL) return selected_item;

	     if (click_type == TX_HIT_SHIFT) AbsSelectionInListBox(true, ctrl, selected_item, 1); /* SELECT WITH SHIFT */
	else if (click_type == TX_HIT_CTRL)  AbsSelectionInListBox(true, ctrl, selected_item, 2); /* SELECT WITH CONTROL */
	else                                 AbsSelectionInListBox(true, ctrl, selected_item, 0); /* NORMAL SELECT */

	LastState.SelectedListBoxItemOld = selected_item;

	return selected_item;
}


DT_SLONG CDTFormEngine::SelectListBox(const CDTControl* ctrl, DT_SLONG y, DT_SWORD click_type)
{
	bool is_rich = false;
	CDTValue fc_value;
	DT_SLONG selected_item, first_item;

	first_item = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, 0);
	//first_item = GetTextBoxFirstRow(ctrl);
	fc_value.MakeByPowerObj(PowerDoc, ctrl->Obj, &is_rich);

	//if (is_rich) selected_item = FormControlsLib->DTTextBox[ctrl->Lib].GetItemByCoord(first_item, y, DV_NULL);
	//else

	selected_item = FormControlsLib->DTListBox[ctrl->Lib].GetItemByCoord(first_item, y, &fc_value);

	//selected_item = FormControlsLib->DTListBox[ctrl->Lib].GetItemByCoord(first_item, y - ctrl->BoxDoc.Y, &fc_value);

	if (LastState.SelectedListBoxItemOld == selected_item && click_type != TX_HIT_CTRL) return selected_item;

	     if (click_type == TX_HIT_SHIFT) AbsSelectionInListBox(true, ctrl, selected_item, 1); /* SELECT WITH SHIFT */
	else if (click_type == TX_HIT_CTRL)  AbsSelectionInListBox(true, ctrl, selected_item, 2); /* SELECT WITH CONTROL */
	else                                 AbsSelectionInListBox(true, ctrl, selected_item, 0); /* NORMAL SELECT */

	LastState.SelectedListBoxItemOld = selected_item;

	return selected_item;
}


void CDTFormEngine::SetSelectionInListBox(DT_ID_SLONG obj, DT_SLONG selected_item, DT_UBYTE select_type)
{
	DT_SLONG i, n, len;
	DT_SLONG item, first_selected_item;
	DT_UBYTE mini_buffer[4] = {0, 0, 0, 0};
	DT_UBYTE* select_array = mini_buffer;
	const DT_UBYTE* selected_items = pdPropGet(PowerDoc, obj, fcListSelectedItems, &len);
	DT_SLONG nr_of_selected_items = len / 4;
	DT_UBYTE allow_multi_selection;
	bool keep = true;

	if ((allow_multi_selection = pdPropGetAsUByte(PowerDoc, obj, fcListMultiSelect, 0)) != 1) select_type = 0;

	if (len != 4 * nr_of_selected_items) select_type = 0;
	if (nr_of_selected_items < 1) select_type = 0;

	if (select_type == 1) /* SELECT WITH SHIFT */
	{
		first_selected_item = DF_READ_SLONG_LE(selected_items);

		if (selected_item >= first_selected_item) n = selected_item - first_selected_item;
		else                                      n = first_selected_item - selected_item;

		n = dtSecAdd(n, 1); if (n < 0) return;

		if ((select_array = static_cast<DT_UBYTE*>(dtMemAlloc(dtSecMul(n, sizeof(DT_SLONG))))) == DV_NULL) return;

		if (selected_item >= first_selected_item) for (i = 0; i < n; i++) { DF_WRITE_SLONG_LE(select_array + 4 * i, first_selected_item + i); }
		else                                      for (i = 0; i < n; i++) { DF_WRITE_SLONG_LE(select_array + 4 * i, first_selected_item - i); }

		pdPropApply(PowerDoc, obj, fcListSelectedItems, select_array, n * sizeof(DT_SLONG), 0);
		dtMemFree(select_array);
	}
	else if (select_type == 2) /* SELECT WITH CONTROL */
	{
		n = nr_of_selected_items + 1;
		if (n < 0) return; /* should never happen */
		if ((select_array = static_cast<DT_UBYTE*>(dtMemAlloc(dtSecMul(dtSecAdd(n, 1), sizeof(DT_SLONG))))) == DV_NULL) return;

		for (i = 0, n = 0; i < nr_of_selected_items; i++)
		{
			item = DF_READ_SLONG_LE(selected_items + 4 * i);
			if (selected_item == item) keep = false; else { DF_WRITE_SLONG_LE(select_array + 4 * n, item); n++; }
		}

		if (keep) { DF_WRITE_SLONG_LE(select_array + 4 * n, selected_item); n++; }
		pdPropApply(PowerDoc, obj, fcListSelectedItems, select_array, n * sizeof(DT_SLONG), 0);
		dtMemFree(select_array);

	}
	else /* NORMAL SELECT */
	{
		DF_WRITE_SLONG_LE(select_array, selected_item);
		pdPropApply(PowerDoc, obj, fcListSelectedItems, select_array, sizeof(DT_SLONG), 0);
	}
}


DT_SLONG CDTFormEngine::AbsSelectionInListBox(bool do_events, const CDTControl* ctrl, DT_SLONG selected_item, DT_UBYTE select_type)
{
	if (ctrl->LockStatus == 2 || ctrl->LockStatus == 3) return 0;

	bool scroll = false;
	bool is_rich = false;
	CDTValue fc_value;

	fc_value.MakeByPowerObj(PowerDoc, ctrl->Obj, &is_rich);

	if (is_rich)
	{
		SetSelectionInListBox(ctrl->Obj, selected_item, select_type);

		DrawCtrl(ctrl);

		if (!do_events) return selected_item;
		Handler_Invoke(fcOnCtrlChange, ctrl);

		return selected_item;
	}

	DT_SLONG first_item = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, 0);
	DT_SLONG nr_of_items = fc_value.GetNrOfRows();
	DT_SLONG nr_of_visible_rows = FormControlsLib->DTListBox[ctrl->Lib].GetNrOfVisibleRows(ctrl->BoxDoc.H);

	if (selected_item < 0) selected_item = 0; else if (selected_item >= nr_of_items) selected_item = nr_of_items - 1;

	if (selected_item < first_item)
	{
		first_item = selected_item;
		scroll = true;
	}
	else if (selected_item >= first_item + nr_of_visible_rows)
	{
		first_item = selected_item - nr_of_visible_rows + 1;
		scroll = true;
	}

	SetSelectionInListBox(ctrl->Obj, selected_item, select_type);

	if (ctrl->LockStatus == 4) scroll = false;

	if (scroll) ScrollListBox(true, ctrl, first_item, true);
	else DrawCtrl(ctrl);

	if (!do_events) return selected_item;
	Handler_Invoke(fcOnCtrlChange, ctrl);

	return selected_item;
}


DT_SLONG CDTFormEngine::RelSelectionInListBox(bool do_events, const CDTControl* ctrl, bool increase, DT_UBYTE select_type)
{
	DT_SLONG i, len;
	const DT_UBYTE* selected_items = pdPropGet(PowerDoc, ctrl->Obj, fcListSelectedItems, &len);
	DT_SLONG first_selected_item = 0, min_selected_item = 0, max_selected_item = 0, selected_item = 0;
	DT_SLONG nr_of_selected_items = len / 4;

	if (nr_of_selected_items > 0 && len == 4 * nr_of_selected_items)
	{
		first_selected_item = min_selected_item = max_selected_item = selected_item = DF_READ_SLONG_LE(selected_items);

		for (i = 1; i < nr_of_selected_items; i++)
		{
			selected_item = DF_READ_SLONG_LE(selected_items + 4 * i);

			     if (selected_item < min_selected_item) min_selected_item = selected_item;
			else if (selected_item > max_selected_item) max_selected_item = selected_item;
		}
	}

	if (increase) { if (max_selected_item > first_selected_item) selected_item = max_selected_item + 1; else selected_item = min_selected_item + 1; }
	else          { if (min_selected_item < first_selected_item) selected_item = min_selected_item - 1; else selected_item = max_selected_item - 1; }

	return AbsSelectionInListBox(do_events, ctrl, selected_item, select_type);
}


DT_SLONG CDTFormEngine::ScrollListBox(bool do_events, const CDTControl* ctrl, DT_SLONG scroll_amount, bool absolute_scroll)
{
	DT_SLONG first_item = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, 0), first_item_old = first_item;

	bool is_rich = false;
	CDTValue fc_value;

	fc_value.MakeByPowerObj(PowerDoc, ctrl->Obj, &is_rich);

	DT_SLONG MaxFirstItem = FormControlsLib->DTListBox[ctrl->Lib].GetMaxFirstItem(ctrl->BoxDoc.H, fc_value);

	if (absolute_scroll) first_item = scroll_amount; else first_item += scroll_amount;

	if (first_item < 0) first_item = 0; else if (first_item > MaxFirstItem) first_item = MaxFirstItem;

	if (first_item_old == first_item) return first_item;

	pdPropApplyAsSLong(PowerDoc, ctrl->Obj, fcItemFirst, first_item, 0, PD_APPLY_STANDARD); pdPropDel(PowerDoc, ctrl->Obj, fcRowFirst);
	DrawCtrl(ctrl);

	if (!do_events) return first_item;
	Handler_Invoke(fcOnCtrlScroll, ctrl);

	return first_item;
}


/////////////////////////////////////////////////////////////////////////////
// Built-in Handler Functions


void CDTFormEngine::Handler_CallbackFunction(DT_SLONG signal_id, DT_UBYTE handler, const CDTControl* ctrl)
{
	DT_STRING_CHARS(s, 1000, "");
	DT_UWORD slider_coverage;
	DT_SLONG slider_position, slider_limit, first_item, selected_item;
	DT_ID_SLONG obj;

	Event_PreAction(PowerDoc, signal_id, handler, ctrl);

	switch (handler)
	{
	case 100:
	case 1:

		//sprintf(s, "%s | Signal ID: %ld | Control Type: %d | Control Index: %ld | PowerDoc Object: %ld", (handler == 100 ? "INTERPRET PROGRAM" : "USER CALLBACK FUNCTION"), signal_id, ctrl->Type, ctrl->Idx, ctrl->Obj);
		dtStrConcatChars(&s, (handler == 100 ? "INTERPRET PROGRAM" : "USER CALLBACK FUNCTION"));
		dtStrConcatChars(&s, " | Signal ID: ");
		dtStrConcatSLong(&s, signal_id);
		dtStrConcatChars(&s, " | Control Type: ");
		dtStrConcatSLong(&s, ctrl->Type);
		dtStrConcatChars(&s, " | Control Index: ");
		dtStrConcatSLong(&s, ctrl->Idx);
		dtStrConcatChars(&s, " | PowerDoc Object: ");
		dtStrConcatSLong(&s, ctrl->Obj);
		ShowStatus(s.str);

		Event_Action(PowerDoc, signal_id, handler, ctrl);
		break;

		// Built-in Callback Functions

	case 2: /* Jump to the page specified in fcUserParam */

		Handler_JumpToPage(ctrl);
		break;

	case 3: /* Scroll up (left) connected sliders, text boxes and list boxes */

		Handler_ScrollConnCtrls(ctrl, false);
		break;

	case 4: /* Scroll down (right) connected sliders, text boxes and list boxes */

		Handler_ScrollConnCtrls(ctrl, true);
		break;

	case 5: /* Scroll connected list boxes and text boxes according to the slider's position */

		slider_position = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcSliderPosition, 0);
		Handler_SetScrollForConnCtrls(ctrl, slider_position);
		break;

	case 6: /* Copy the value of the slider's position to connected text boxes */

		slider_position = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcSliderPosition, 0);
		Handler_CopySliderPosValueToConnTextCtrls(ctrl, slider_position);
		break;

	case 7: /* Scroll connected sliders according to the first visible item in the list box or text box */

		first_item = GetTextBoxFirstRow(ctrl);
		Handler_UpdateConnSliders(ctrl, first_item);
		break;

	case 8: /* Copy the value of the selected item in the list to connected text boxes */

		selected_item = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcListSelectedItems, 0);
		Handler_CopyListBoxValueToTextCtrls(ctrl, selected_item);
		break;

	case 9: /* Hide Popups */

		Handler_Popup(ctrl->Obj, 1);
		break;

	case 10: /* Unhide Popups */

		Handler_Popup(ctrl->Obj, -1);
		break;

	case 11: /* Alternate Popups */

		Handler_Popup(ctrl->Obj, 0);
		break;

	case 12: /* Jump to the previous page */

		Handler_JumpToPrevPage(ctrl);
		break;

	case 13: /* Jump to the next page */

		Handler_JumpToNextPage(ctrl);
		break;

	case 14: /* Update connected sliders to reflect the amount of content in the text box */

		slider_limit = GetTextBoxLimitAndCoverage(ctrl, &slider_coverage);
		Handler_UpdateConnSliders(ctrl, -1, slider_limit, slider_coverage);
		break;

	case 15: /* Copy the value of the slider's position to joined media - by time */

		slider_position = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcSliderPosition, 0);
		obj = Handler_CopySliderPosValueToConnMedia(ctrl, 1, slider_position, -1);
		if (obj >= 0) pdPropSetAsRef(PowerDoc, ctrl->Obj, fcSliderPosition, obj, pdxMediaInfoCurrentTime);
		break;

	case 16: /* Copy the value of the slider's position to joined media - by frame */

		slider_position = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcSliderPosition, 0);
		obj = Handler_CopySliderPosValueToConnMedia(ctrl, 2, -1, slider_position);
		if (obj >= 0) pdPropSetAsRef(PowerDoc, ctrl->Obj, fcSliderPosition, obj, pdxMediaInfoCurrentFrame);
		break;

	case 17: /* Pause joined media */
	case 18: /* Play joined media */
	case 19: /* Restart joined media */
	case 20: /* Toggle joined media */

		Handler_CopySliderPosValueToConnMedia(ctrl, (handler - 14), -1, -1);
		break;

	case 21: /* Open URL specified in fcUserString */
	case 24: /* Open E-mail address specified in fcUserString */

		Handler_OpenURL(ctrl, (handler == 24 ? 1 : 0));
		break;

	case 22: /* Jump to the first page that holds an image with the name ID specified in fcUserParam */
	case 23: /* Jump to the last page that holds an image with the name ID specified in fcUserParam */

		Handler_JumpToImg(ctrl, (handler == 22));
		break;
	}

	Event_PostAction(PowerDoc, signal_id, handler, ctrl);
}


DT_ID_SLONG CDTFormEngine::Handler_CopySliderPosValueToConnMedia(const CDTControl* ctrl, DT_ID_UBYTE next_command, DT_SLONG next_time, DT_SLONG next_frame)
{
	DT_ID_SLONG obj, obj_last = pdObjGetLast(PowerDoc);
	DT_SLONG group_id = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcUserParam, 0);

	if (next_command == 0) next_time = next_frame = -1;

	for (obj = 0; obj <= obj_last; obj++)
	{
		DT_SLONG len = 0;
		DT_UBYTE* b = pdPropGet(PowerDoc, obj, pdxMediaDescriptor, &len);
		if (b == DV_NULL || len < 0) continue; /* not a media object */

		if (pdPropGetAsSLong(PowerDoc, obj, fcUserParam, 0) != group_id) continue; /* not joined */

		if (next_command == 0) pdPropDel(PowerDoc, obj, pdxMediaNextCommand); else pdPropApplyAsUByte(PowerDoc, obj, pdxMediaNextCommand, next_command, 0, PD_APPLY_STANDARD);
		if (next_time < 0) pdPropDel(PowerDoc, obj, pdxMediaNextTime); else pdPropApplyAsSLong(PowerDoc, obj, pdxMediaNextTime, next_time, 0, PD_APPLY_STANDARD);
		if (next_frame < 0) pdPropDel(PowerDoc, obj, pdxMediaNextFrame); else pdPropApplyAsSLong(PowerDoc, obj, pdxMediaNextFrame, next_frame, 0, PD_APPLY_STANDARD);

		return obj;
	}

	return -1;
}


void CDTFormEngine::Handler_SetScrollForConnCtrls(const CDTControl* ctrl, DT_SLONG first_item)
{
	CDTControl ctrl_conn;
	DT_PDX_WH_SLONG box_doc;
	DT_UBYTE state;
	DT_ID_SLONG obj;
	DT_ID_SWORD fn_draw;
	DT_SLONG idx, idx_last = LastDrawIdx;
	DT_SLONG fc_connection_id = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcConnectionID, 0);
	bool by_row;

	for (idx = 0; idx <= idx_last; idx++)
	{
		fn_draw = LastDraw[idx].fn_draw;
		if (fn_draw != fcDrawListBox && fn_draw != fcDrawTextBox && fn_draw != fcDrawRichTextBox && fn_draw != fcDrawRichListBox) continue;

		obj = LastDraw[idx].obj;
		state = GetCtrlStateAndBox(obj, &box_doc);
		if (pdPropGetAsSLong(PowerDoc, obj, fcConnectionID, 0) != fc_connection_id) continue;

		ctrl_conn.Type = fn_draw;
		ctrl_conn.State = state;
		ctrl_conn.PushType = 0;
		ctrl_conn.Reserved = 0;
		ctrl_conn.Idx = idx;
		ctrl_conn.Obj = obj;
		ctrl_conn.Img = LastDraw[idx].img;
		ctrl_conn.ParentObj = LastDraw[idx].parent_obj;
		ctrl_conn.Lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, obj, fcConfigPtr), fcCnfLib, 0));

		ctrl_conn.BoxDoc = box_doc;
		if (fn_draw == fcDrawListBox) ScrollListBox(false, &ctrl_conn, first_item, true);
		else if (fn_draw == fcDrawTextBox || fn_draw == fcDrawRichTextBox || fn_draw == fcDrawRichListBox)
		{
			by_row = (pdPropGetAsUByte(PowerDoc, obj, fcTextScrollType, 0) == 0);
			ScrollTextBox(false, &ctrl_conn, first_item, by_row, true);
		}
	}
}


void CDTFormEngine::Handler_UpdateConnSliders(const CDTControl* ctrl, DT_SLONG slider_position, DT_SLONG slider_limit /*= -1 */, DT_UWORD slider_coverage /*= 0*/)
{
	CDTControl ctrl_conn;
	DT_PDX_WH_SLONG box_doc;
	DT_UBYTE state;
	DT_ID_SLONG obj;
	DT_ID_SWORD fn_draw;
	DT_SLONG idx, idx_last = LastDrawIdx;
	DT_SLONG fc_connection_id = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcConnectionID, 0);

	for (idx = 0; idx <= idx_last; idx++)
	{
		fn_draw = LastDraw[idx].fn_draw;
		if (fn_draw != fcDrawVSlider && fn_draw != fcDrawHSlider) continue;

		obj = LastDraw[idx].obj;
		state = GetCtrlStateAndBox(obj, &box_doc);
		if (pdPropGetAsSLong(PowerDoc, obj, fcConnectionID, 0) != fc_connection_id) continue;

		ctrl_conn.Type = fn_draw;
		ctrl_conn.State = state;
		ctrl_conn.PushType = 0;
		ctrl_conn.Reserved = 0;
		ctrl_conn.Idx = idx;
		ctrl_conn.Obj = obj;
		ctrl_conn.Img = LastDraw[idx].img;
		ctrl_conn.ParentObj = LastDraw[idx].parent_obj;
		ctrl_conn.Lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, obj, fcConfigPtr), fcCnfLib, 0));
		ctrl_conn.BoxDoc = box_doc;
		MoveSliderToPosition(false, &ctrl_conn, slider_position, slider_limit, slider_coverage);
	}
}


void CDTFormEngine::Handler_ScrollConnCtrls(const CDTControl* ctrl, bool increase)
{
	CDTControl ctrl_conn;
	DT_PDX_WH_SLONG box_doc;
	DT_UBYTE state;
	DT_ID_SLONG obj;
	DT_ID_SWORD fn_draw;
	DT_SLONG idx, idx_last = LastDrawIdx;
	DT_SLONG slider_position;
	DT_SLONG fc_connection_id = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcConnectionID, 0);
	DT_SLONG scroll_amount = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcUserParam, 0);
	bool by_row;

	if (scroll_amount == 0) scroll_amount = 1;
	if (!increase) scroll_amount = -scroll_amount;

	for (idx = 0; idx <= idx_last; idx++)
	{
		fn_draw = LastDraw[idx].fn_draw;
		if (fn_draw != fcDrawListBox && fn_draw != fcDrawTextBox && fn_draw != fcDrawRichTextBox && fn_draw != fcDrawRichListBox && fn_draw != fcDrawHSlider && fn_draw != fcDrawVSlider) continue;

		obj = LastDraw[idx].obj;
		state = GetCtrlStateAndBox(obj, &box_doc);
		if (pdPropGetAsSLong(PowerDoc, obj, fcConnectionID, 0) != fc_connection_id) continue;

		ctrl_conn.Type = fn_draw;
		ctrl_conn.State = state;
		ctrl_conn.PushType = 0;
		ctrl_conn.Reserved = 0;
		ctrl_conn.Idx = idx;
		ctrl_conn.Obj = obj;
		ctrl_conn.Img = LastDraw[idx].img;
		ctrl_conn.ParentObj = LastDraw[idx].parent_obj;
		ctrl_conn.Lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, obj, fcConfigPtr), fcCnfLib, 0));
		ctrl_conn.BoxDoc = box_doc;

		if (fn_draw == fcDrawListBox) ScrollListBox(false, &ctrl_conn, scroll_amount, false);
		else if (fn_draw == fcDrawTextBox || fn_draw == fcDrawRichTextBox || fn_draw == fcDrawRichListBox)
		{
			by_row = (pdPropGetAsUByte(PowerDoc, obj, fcTextScrollType, 0) == 0);
			ScrollTextBox(false, &ctrl_conn, scroll_amount, by_row, false);
		}
		else
		{
			slider_position = scroll_amount + pdPropGetAsSLong(PowerDoc, obj, fcSliderPosition, 0);
			MoveSliderToPosition(false, &ctrl_conn, slider_position);
		}
	}
}


void CDTFormEngine::Handler_CopyListBoxValueToTextCtrls(const CDTControl* ctrl, DT_SLONG selected_item)
{
	bool is_rich = false;
	CDTValue value;

	//if (ctrl->Type != fcDrawRichListBox)
	if (value.MakeByPowerObj(PowerDoc, ctrl->Obj, &is_rich) != DV_NULL)
	{
		//value.MakeByPowerObj(PowerDoc, ctrl->Obj, &is_rich);
		value.KillExceptRows(selected_item, 1);
		Handler_CopyValueToConnTextCtrls(ctrl, true, value);
	}
	else if (is_rich)
	{
		DT_SLONG first_char_index = 0, last_char_index = 0;
		DT_UBYTE buffer[4 * (1024 + 1)] = {0, 0, 0, 0};

		GetTextBoxRowInfo(ctrl, selected_item, &first_char_index, &last_char_index);

		DT_SWORD fn_draw;
		DT_SLONG len = 0;
		DT_ID_SLONG obj = -1;
		DT_ID_ULONG max_char_code = 0;
		DT_SLONG char_len = last_char_index - first_char_index;

		if (last_char_index > first_char_index && char_len < 1024)
		{
			obj = txImgGetRootRichTextArea(PowerDoc, ctrl->Img, ctrl->ParentObj, &fn_draw);
			len = txObjCharsCopy_32(PowerDoc, buffer, DV_NULL, obj, first_char_index, last_char_index, DV_NULL /*SelectedText.TextEditConfig*/, 1, &max_char_code); if (len < 0) len = 0; /* should never happen */
		}

		//printf("fcDrawRichListBox selected_item=%ld, copy from %ld to %ld | len=%ld, max_char_code=%ld, s=[%s]\n", selected_item, first_char_index, last_char_index, len, max_char_code, buffer);

		value.MakeByBuffer(buffer, 4 * len, TX_BUFFER_FORMAT_4);
		Handler_CopyValueToConnTextCtrls(ctrl, false, value);

		//DT_STRING_CHARS(s, 64, "");
		//dtStrConcatSLong(&s, selected_item);
		//value.MakeByCString(s.str);
		//Handler_CopyValueToConnTextCtrls(ctrl, true, value);
	}
}


void CDTFormEngine::Handler_CopySliderPosValueToConnTextCtrls(const CDTControl* ctrl, DT_SLONG position_value)
{
	CDTValue value;

	//DT_CHAR str[64]; sprintf(str, "%ld", position_value);
	DT_STRING_CHARS(s, 64, ""); dtStrConcatSLong(&s, position_value);
	value.MakeByCString(s.str);

	Handler_CopyValueToConnTextCtrls(ctrl, true, value);
}


void CDTFormEngine::Handler_CopyValueToConnTextCtrls(const CDTControl* ctrl, bool single_text_fragment, CDTValue& value)
{
	CDTControl ctrl_conn;
	DT_PDX_WH_SLONG box_doc;
	DT_UBYTE state;
	DT_ID_SLONG obj;
	DT_ID_SWORD fn_draw;
	DT_SLONG idx, idx_last = LastDrawIdx;
	DT_SLONG fc_connection_id = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcConnectionID, 0);

	for (idx = 0; idx <= idx_last; idx++)
	{
		fn_draw = LastDraw[idx].fn_draw;
		//if (fn_draw != fcDrawButton && fn_draw != fcDrawTextBox) continue;
		if (fn_draw != fcDrawTextBox && fn_draw != fcDrawRichTextBox) continue;

		obj = LastDraw[idx].obj;
		state = GetCtrlStateAndBox(obj, &box_doc);
		if (pdPropGetAsSLong(PowerDoc, obj, fcConnectionID, 0) != fc_connection_id) continue;

		ctrl_conn.Type = fn_draw;
		ctrl_conn.State = state;
		ctrl_conn.PushType = 0;
		ctrl_conn.Reserved = 0;
		ctrl_conn.Idx = idx;
		ctrl_conn.Obj = obj;
		ctrl_conn.Img = LastDraw[idx].img;
		ctrl_conn.ParentObj = LastDraw[idx].parent_obj;
		ctrl_conn.Lib = CDTFormControlsLib::GetLib(pdPropGetAsSWord(PowerDoc, pdLinkGet(PowerDoc, obj, fcConfigPtr), fcCnfLib, 0));
		ctrl_conn.BoxDoc = box_doc;

		if (fn_draw == fcDrawTextBox) value.ToValueProp(PowerDoc, obj);
		else if (fn_draw == fcDrawRichTextBox) value.ToRichText(PowerDoc, obj, single_text_fragment, StreamFontmap);
		//{
		//	obj = pdLinkGet(PowerDoc, obj, pdTextFragmentPtr);
		//	value.ToTextProp(PowerDoc, obj);
		//	pdLinkDel(PowerDoc, obj, pdTextFragmentPtr); // remove any subsequent text fragments
		//}

		CmdF_CursorUpdateAfterExternalContentChange(obj);
		CmdT_Refresh(LastDraw[idx].img);
		DrawCtrl(&ctrl_conn);
	}
}


void CDTFormEngine::Handler_Popup(DT_ID_SLONG ctrl_obj, DT_SWORD flag)
{
	DT_PDX_WH_SLONG box_doc = {0, 0};
	DT_PDX_XYWH_SLONG extent_pix_ctrl, extent_pix = {0, 0, 0, 0};
	//DT_UBYTE state;
	DT_UBYTE attribs = 0;
	DT_ID_SWORD fn_draw;
	DT_ID_SLONG obj, parent_obj;
	DT_SLONG img, idx, idx_last = LastDrawIdx;
	DT_SLONG group_id = pdPropGetAsSLong(PowerDoc, ctrl_obj, fcUserParam, 0);
	bool hide_new = false, hide_old = false, redraw = false, extent = true, alternate = false;

	if (flag == 1) hide_new = true; /* Hide */
	else if (flag == -1) hide_new = false; /* Unhide */
	else alternate = true; /* Alternate */

	for (idx = 0; idx <= idx_last; idx++)
	{
		obj = LastDraw[idx].obj;
		//state = GetCtrlStateAndBox(obj, &box_doc);
		if (pdPropGetAsSLong(PowerDoc, obj, fcGroupID, 0) != group_id) continue;

		img = LastDraw[idx].img;
		parent_obj = LastDraw[idx].parent_obj;

		/* read attribs */
		attribs = DF_PDX_READ_ATTRIBS(PowerDoc, img, parent_obj);
		fn_draw = DF_PDX_READ_FNDRAW(PowerDoc, img, parent_obj);

		if (attribs & 1) hide_old = true; else hide_old = false;

		/* compare */
		if (alternate) hide_new = !hide_old; else if (hide_new == hide_old) continue;

		/* write attribs */
		if (hide_new) attribs |= 1; else attribs &= 254;
		DF_PDX_WRITE_ATTRIBS(PowerDoc, img, parent_obj, attribs);

		if (fn_draw < fcDrawFirst || fn_draw > fcDrawLast) extent = false;

		if (extent)
		{
			GetCtrlExtentPix(idx, &box_doc, &extent_pix_ctrl);

			if (redraw)
			{
				extent_pix.W = DF_MAX(extent_pix.X + extent_pix.W, extent_pix_ctrl.X + extent_pix_ctrl.W);
				extent_pix.H = DF_MAX(extent_pix.Y + extent_pix.H, extent_pix_ctrl.Y + extent_pix_ctrl.H);
				extent_pix.X = DF_MIN(extent_pix.X, extent_pix_ctrl.X);
				extent_pix.Y = DF_MIN(extent_pix.Y, extent_pix_ctrl.Y);
				extent_pix.W -= extent_pix.X;
				extent_pix.H -= extent_pix.Y;
			}
			else extent_pix = extent_pix_ctrl;
		}

		redraw = true;
	}

	if (!redraw) return;

	//else if (extent) UpdatePaintArea(extent_pix.X, extent_pix.Y, extent_pix.X+extent_pix.W, extent_pix.Y+extent_pix.H); //RedrawArea(extent_pix.X, extent_pix.Y, extent_pix.W, extent_pix.H);
	//else
	UpdatePaintArea(0, 0, 0, 0); //Redraw(false);
}


void CDTFormEngine::Handler_OpenURL(const CDTControl* ctrl, DT_ID_SWORD url_type)
{
	DT_SLONG n = 0;
	const DT_UBYTE* s = pdPropGet(PowerDoc, ctrl->Obj, fcUserString, &n); if (s == DV_NULL || n <= 1 || n >= 2048) return;
	DT_CHAR str[2048]; dtMemCopy(str, s, n); str[n] = 0;
	OpenURL(str, url_type);
}


void CDTFormEngine::Handler_JumpToImg(const CDTControl* ctrl, bool first)
{
	DT_SLONG img_last = pdImgGetLast(PowerDoc);
	DT_SLONG img_name = pdPropGetAsSLong(PowerDoc, ctrl->Obj, fcUserParam, 0);
	DT_SLONG i, page_min = 0, page_max = 0;
	bool not_found = true;

	for (i = 0; i <= img_last; i++)
	{
		if (img_name != pdImgGetName(PowerDoc, i)) continue;

		DT_SLONG page = pdImgGetPage(PowerDoc, i);

		if (not_found) { page_min = page_max = page; not_found = false; continue; }
		if (page < page_min) page_min = page; else if (page > page_max) page_max = page;
	}

	if (not_found) return;
	CmdF_Refresh(true);
	if (first) Repage(page_min); else Repage(page_max);
}


void CDTFormEngine::UpdatePaintArea(DT_SLONG left, DT_SLONG top, DT_SLONG right, DT_SLONG bottom)
{
	if (left == right && top == bottom)
	{
		if (left == 0 && top == 0) { PaintFlag = 1; /* repaint all */ /*PaintArea.xmn = PaintArea.ymn = PaintArea.xmx = PaintArea.ymx = 0;*/ }
		return;
	}

	//if (left > right) Swap(&left, &right);
	//if (top > bottom) Swap(&top, &bottom);

	//printf("--> %d, %d, %d, %d\n", left, top, right, bottom);

	if (PaintArea.xmn == 0 && PaintArea.ymn == 0 && PaintArea.xmx == 0 && PaintArea.ymx == 0)
	{
		PaintArea.xmn = left;
		PaintArea.ymn = top;
		PaintArea.xmx = right;
		PaintArea.ymx = bottom;
		return;
	}

	PaintArea.xmn = DF_MIN(PaintArea.xmn, left);
	PaintArea.ymn = DF_MIN(PaintArea.ymn, top);
	PaintArea.xmx = DF_MAX(PaintArea.xmx, right);
	PaintArea.ymx = DF_MAX(PaintArea.ymx, bottom);
}


DT_SWORD CDTFormEngine::GetPaintArea(DT_RECT_SLONG* Extent, bool clear)
{
	DT_SWORD ret = 1;
	DT_SLONG corr = 4;

	if (PaintArea.xmn == 0 && PaintArea.ymn == 0 && PaintArea.xmx == 0 && PaintArea.ymx == 0)
	{
		ret = 2;
		corr = 0;
	}
	if (PaintFlag == 1) ret = 0; /* repaint all */
	if (Extent != DV_NULL)
	{
		Extent->xmn = PaintArea.xmn - corr;
		Extent->ymn = PaintArea.ymn - corr;
		Extent->xmx = PaintArea.xmx + corr;
		Extent->ymx = PaintArea.ymx + corr;
	}
	if (clear)
	{
		PaintArea.xmn = PaintArea.ymn = PaintArea.xmx = PaintArea.ymx = 0;
		PaintFlag = 0;
	}

	return ret;
}


bool CDTFormEngine::UpdateLastDraw(DT_UBYTE attribs, DT_ID_SLONG parent_obj, DT_ID_SLONG obj, DT_SLONG img, DT_SLONG fn_draw, DT_SRAST_XL x, DT_SRAST_XL y, const DT_TM3X3 tm)
{
	DT_SLONG expand = FE_LAST_DRAW_EXTRASLOTS;
	DT_SLONG i, last_draw_len, inc = 1;
	FE_LAST_DRAW* last_draw;

	for (i = 0; i <= LastDrawIdx; i++)
	{
		if (LastDraw[i].obj != obj || LastDraw[i].img != img || LastDraw[i].parent_obj != parent_obj) continue;

		/* we found it - update */
		inc = 0;
		goto end;
	}

	/* we did not find it - add */
	if (i < LastDrawLen) goto end; /* no need to grow the array */

	/* first grow the array */
	last_draw_len = dtSecAdd(LastDrawLen, 1); if (last_draw_len < 0) return false;
	if (expand <= 0) expand = last_draw_len / 2; /* Dynamic method (expand by one half of current size) */
	last_draw_len = dtSecAdd(last_draw_len, expand);

	//last_draw_len = LastDrawLen + 1 + FE_LAST_DRAW_EXTRASLOTS;

	if ((last_draw = static_cast<FE_LAST_DRAW*>(dtMemRealloc(LastDraw, dtSecMul(last_draw_len, sizeof(FE_LAST_DRAW))))) == DV_NULL) return false;
	LastDraw = last_draw;
	LastDrawLen = last_draw_len;

end:

	LastDraw[i].parent_obj = parent_obj;
	LastDraw[i].obj = obj;
	LastDraw[i].fn_draw = static_cast<DT_SWORD>(fn_draw);
	LastDraw[i].img = img;
#ifdef FE_DISABLE_XL
	LastDraw[i].x = static_cast<DT_SRAST_L>(x);
	LastDraw[i].y = static_cast<DT_SRAST_L>(y);
#else
	LastDraw[i].x = x;
	LastDraw[i].y = y;
#endif
	LastDraw[i].tm[0][0] = tm[0][0];
	LastDraw[i].tm[0][1] = tm[0][1];
	LastDraw[i].tm[0][2] = tm[0][2];
	LastDraw[i].tm[1][0] = tm[1][0];
	LastDraw[i].tm[1][1] = tm[1][1];
	LastDraw[i].tm[1][2] = tm[1][2];
	LastDraw[i].tm[2][0] = tm[2][0];
	LastDraw[i].tm[2][1] = tm[2][1];
	LastDraw[i].tm[2][2] = tm[2][2];
	LastDraw[i].attribs = attribs;

	LastDrawIdx += inc;
	return true;
}
