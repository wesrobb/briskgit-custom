#include <string.h> /* only for strlen */
#include "dtbuilder.h"


CDTFormatter& CDTFormatter::Format_Internal(const DT_TX_ATTRIBS text_attribs[], DT_SLONG /*first_char*/, DT_SLONG /*last_char*/)
{
	for (DT_SLONG i = 0; Count < DV_FORMATTER_MAX_COUNT; i++)
	{
		if (text_attribs[i].Id == TX_ATTR_END) break;
		DT_SLONG l = strlen(text_attribs[i].Value); if (l >= 255) continue; //break;
		DT_SLONG j = 0;

		/* check if we need to update or append */
		for (j = 0; j < Count; j++) { if (text_attribs[i].Id == Attr[j].Id) break; }

		/* update or append */
		Attr[j].Id = text_attribs[i].Id; dtMemCopy(Attr[j].Value, text_attribs[i].Value, l + 1);

		if (j < Count) break; /* we updated existing */

		/* we append, so also add TX_ATTR_END at the end */
		Count++;
		Attr[Count].Id = TX_ATTR_END;
		Attr[Count].Value[0] = 0;
	}

	return *this;
}


DT_ID_SLONG CDTBuilder::ToObj(DT_SLONG img_min, DT_SLONG img_max, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_SLONG xbs, DT_SLONG ybs, DT_SLONG font_w, DT_SLONG font_h, DT_SLONG aw, DT_SLONG ah) const
{
	if (img_min < 0 || img_max < img_min) return -1;

	bool area = true;
	DT_PDDOC powerdoc = Doc->GetPdDoc();
	DT_SLONG page = Doc->ImgGetPage(img_min);
	DT_SLONG x0, y0; Doc->ImgGetXY(img_min, &x0, &y0); Doc->ImgSetXY(img_min, 0, 0);
	DT_SLONG arr_len = 3;
	DT_SLONG metrics_x_arr[3] = {font_w, xbs, aw /* Advance width */};
	DT_SLONG metrics_y_arr[3] = {font_h, ybs, ah /* Presently unused */};
	DT_RECT_SLONG extent = {0 - x, 0 - y, w - x, h - y}; /* Bounding box relative to (x, y) */

	/* Make a text fragment object that we can paste into text */
	//DT_ID_SLONG obj = pdxPageToText(powerdoc, page, x, y, &extent, metrics_x_arr, metrics_y_arr, 3); if (obj < 0) return -1;
	pdxImgSelSelectByImgRange(powerdoc, img_min, img_max);
	pdxImgSelGroup(powerdoc, page, x, y, 2); /* make a group and its image, place it on page */
	DT_ID_SLONG obj = pdxImgSelToText(powerdoc, &extent, metrics_x_arr, metrics_y_arr, arr_len, 2); if (obj < 0) return -1;

	//txTextPasteViaPowerObj(Doc->GetTxDoc(), powerdoc, obj, 0);

	/* image of the group that was created by pdxImgSelGroup */
	DT_SLONG img_grp = Doc->ImgGetLast(); if (img_grp < 0) return -1;

	/* Convert it to RichTextArea */
	//Doc->ImgSetStatus(img_grp, 0); /* must deselect the image */
	//Doc->ImgSetObj(img_grp, obj);
	//Doc->ImgSetXY(img_grp, 0, 0);
	//Doc->ImgSetFnDraw(img_grp, pdDrawRichTextArea);

	/* Delete it and add RichTextArea */
	Doc->ImgDel(img_grp);

	if (area)
	{
		Doc->PropAddAsSLong(obj, pdTextAreaDw, 0);
		Doc->PropAddAsSLong(obj, pdTextAreaDh, 0);
		Doc->ImgAdd(obj, page, x0, y0, 0, 0, 0, pdDrawRichTextArea);
		//Doc->ImgAdd(obj, DV_PAGE_TEMP, 0, 0, 0, 0, 0, pdDrawRichTextArea);
	}
	else
	{
		Doc->ImgAdd(obj, page, x0, y0, 0, 0, 0, pdDrawRichTextLine);
	}

	return obj;
}


DT_SLONG CDTBuilder::TxCmd_Internal(DT_SLONG img, DT_ID_SLONG parent_obj, DT_ID_SWORD command)
{
	DT_TX_TEXT_FLOW textflow = {img, parent_obj};

	if (command == TX_CMD_ENDTEXT)
	{
		return txTextHitPlus(Doc->GetTxDoc(), TX_HIT_ENDTEXT, 0, 0, &textflow, DV_NULL, -1);
	}
	else if (command == TX_CMD_SELECTFLOW)
	{
		txTextHitPlus(Doc->GetTxDoc(), TX_HIT_SELECTFLOW, 0, 0, &textflow, DV_NULL, -1);
		return 0;
	}
	//else /* will never happen */
	//{
	//	exit(0);
	//	txTextHitPlus(Doc->GetTxDoc(), TX_HIT_NA, 0, 0, &textflow, DV_NULL, -1);
	//	txTextCommand(Doc->GetTxDoc(), command, DV_NULL, 0);
	//}

	return 0;
}


bool CDTBuilder::AddRun_Builder(const CDTBuilder& b, DT_SLONG img, DT_ID_SLONG parent_obj, DT_ID_SWORD command, DT_SWORD flags, DT_SLONG* last_char_count)
{
	//if (Doc == DV_NULL) exit(0); /* will never happen */
	if (Doc == DV_NULL) { *last_char_count = 0; return false; }

	*last_char_count = TxCmd_Internal(img, parent_obj, command);

	DT_UWORD b_flags = b.Flags;

	if (b_flags & DV_BUILDER_FLAG_NEWLINE) AddRun_UTF8("\r", last_char_count);

	DT_STR_UTF8 str_utf8 = b.IsPlainText(); if (str_utf8) return AddRun_UTF8(str_utf8, last_char_count);

	DT_SWORD fn_draw;
	DT_ID_SLONG obj = b.Doc->ImgGetObjAndFnDraw(b.Img0, -1, &fn_draw); if (obj < 0) return false;

	DT_ULONG exclusions = 0;
	DT_ULONG* exclusions_addr = DV_NULL;

	if (b_flags & DV_BUILDER_FLAG_KEEPALL) goto paste;

	exclusions_addr = &exclusions;
	exclusions = 1; /* always exclude PowerFont definition */
	exclusions |= 2; /* exclude Font width and height (but also SpacingRow and SpacingFactor!) */
	if (b_flags & DV_BUILDER_FLAG_EXRULER) exclusions |= 4; /* exclude text ruler */

paste:

	/* 2 = don't optimize, 8 = don't copy */
	DT_SLONG paste_flags = ((flags & 1) ? 2 /* full copy */ : (2 | 8) /* don't copy */);
	if (flags & 2) { exclusions_addr = &exclusions; exclusions = 1; /* use standard excludes (for TOC) */ }
	DT_SLONG char_count = txTextPasteViaPowerObjPlus(Doc->GetTxDoc(), b.Doc->GetPdDoc(), obj, paste_flags, exclusions_addr); if (char_count < 0) return false;
	*last_char_count = *last_char_count + char_count;

	//bool text_reflow = true;
	if (Doc->DoTextReflow) txTextReflow(Doc->GetTxDoc(), 0);

	return true;
}


bool CDTBuilder::AddRun_UTF8(DT_STR_UTF8 str_utf8, DT_SLONG* last_char_count)
{
	DT_ID_UBYTE paste_command = TX_PASTE_TEXT;
	paste_command |= 64; /* don't optimize */

	//bool compression = true;
	bool compression = Doc->HasTextCompression();
	DT_ID_UBYTE text_type = TX_UTF8;
	DT_SWORD unicode_flags = GetTextFormatFlags(false);
	if (unicode_flags != TX_IMPORT_POWERDOC_REFORMAT) { unicode_flags |= 8192; paste_command |= 128; /* reformat */ }
	if (compression) unicode_flags |= TX_IMPORT_UNICODE_OPTIMIZE | TX_IMPORT_UNICODE_COMPRESS;

	//CDTEngineV8* engine = Doc->GetEngine();

	DT_STREAM_MEMORY(stream_text, reinterpret_cast<const DT_UBYTE*>(str_utf8), (str_utf8 ? strlen(str_utf8) : 0));
	DT_SLONG char_count = txTextPasteViaStream(Doc->GetTxDoc(), paste_command, text_type, &stream_text, unicode_flags, GetFontmap()); if (char_count < 0) return false;
	*last_char_count = *last_char_count + char_count;

	//bool text_reflow = true;
	if (Doc->DoTextReflow) txTextReflow(Doc->GetTxDoc(), 0);

	return true;
}


CDTFormatter& CDTBuilder::Format_Internal(const DT_TX_ATTRIBS text_attribs[], DT_SLONG first_char, DT_SLONG last_char)
{
	if (first_char == DV_SEL_LAST && last_char == DV_SEL_ALL) { first_char = FormatFirstChar; last_char = FormatLastChar; } else { FormatFirstChar = first_char; FormatLastChar = last_char; }

	if (IsPlainText()) return *this;

	if (first_char == DV_SEL_ALL && last_char == DV_SEL_ALL)
	{
		first_char = last_char = -1;
		txTextCommand(Doc->GetTxDoc(), TX_CMD_SELECTFLOW, DV_NULL, 0);
	}
	else
	{
		if (first_char < 0) first_char = 0;
		if (last_char < 0) last_char = 0;
		if (last_char < first_char) { DT_SLONG t = first_char; first_char = last_char; last_char = t; }
	}

	txTextSetAttribs(Doc->GetTxDoc(), first_char, last_char, text_attribs, 2 /* don't store format */);

	//bool text_reflow = true;
	if (Doc->DoTextReflow) txTextReflow(Doc->GetTxDoc(), 0);

	Flags |= DV_BUILDER_FLAG_KEEPALL;
	//Flags = DV_BUILDER_FLAG_KEEPALL;
	//Flags = DV_BUILDER_FLAG_EXBASIC; /* we really need keep all except font width and height */
	return *this;
}


#include "dtbuilder_rast.cpp"
#include "dtbuilder_text.cpp"
#include "dtbuilder_ctrl.cpp"
#include "dtbuilder_math.cpp"
