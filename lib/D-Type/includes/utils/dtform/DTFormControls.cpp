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


#include <string.h>

#include "dtype.h"
#include "pdextras.h"

#include "DTFormControls.h"


/////////////////////////////////////////////////////////////////////////////
// CDTValue


DT_UBYTE* CDTValue::GetPowerDocProp(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG* len, DT_UBYTE* buffer_format, DT_UBYTE* must_free, bool* is_rich)
{
	DT_UBYTE* addr = DV_NULL;

	*len = 0;
	*buffer_format = 0;
	*must_free = 0;
	*is_rich = false;

	if (pdLinkGet(power_doc, obj, pdTextFragmentPtr) >= 0) { *is_rich = true; return DV_NULL; }

	else if ((addr = pdPropGetC(power_doc, obj, fcValue_32, len, must_free)) != DV_NULL) *buffer_format = TX_BUFFER_FORMAT_4;
	else if ((addr = pdPropGetC(power_doc, obj, fcValue_24, len, must_free)) != DV_NULL) *buffer_format = TX_BUFFER_FORMAT_3;
	else if ((addr = pdPropGetC(power_doc, obj, fcValue_16, len, must_free)) != DV_NULL) *buffer_format = TX_BUFFER_FORMAT_2;
	else if ((addr = pdPropGetC(power_doc, obj, fcValue_08, len, must_free)) != DV_NULL) *buffer_format = TX_BUFFER_FORMAT_1;

	return addr;
}


void CDTValue::ToPowerDocProp(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name_08, DT_SLONG name_16, DT_SLONG name_24, DT_SLONG name_32)
{
	pdPropDel(power_doc, obj, name_32);
	pdPropDel(power_doc, obj, name_24);
	pdPropDel(power_doc, obj, name_16);
	pdPropDel(power_doc, obj, name_08);

	     if (BufferFormat == TX_BUFFER_FORMAT_4) pdPropAddC(power_doc, obj, name_32, Addr, Len, (Free ? TX_TEXT_COMPRESS_ON_4 : TX_TEXT_COMPRESS_OFF_4));
	else if (BufferFormat == TX_BUFFER_FORMAT_3) pdPropAddC(power_doc, obj, name_24, Addr, Len, (Free ? TX_TEXT_COMPRESS_ON_3 : TX_TEXT_COMPRESS_OFF_3));
	else if (BufferFormat == TX_BUFFER_FORMAT_2) pdPropAddC(power_doc, obj, name_16, Addr, Len, (Free ? TX_TEXT_COMPRESS_ON_2 : TX_TEXT_COMPRESS_OFF_2));
	else if (BufferFormat == TX_BUFFER_FORMAT_1) pdPropAddC(power_doc, obj, name_08, Addr, Len, (Free ? TX_TEXT_COMPRESS_ON_1 : TX_TEXT_COMPRESS_OFF_1));
}


void CDTValue::ToRichText(DT_PDDOC power_doc, DT_ID_SLONG obj, bool single_text_fragment, const DT_STREAM_DESC* stream_fontmap, const DT_TX_DEFAULT_ATTRIBS* attribs /* = DV_NULL */)
{
	if (single_text_fragment)
	{
		obj = pdLinkGet(power_doc, obj, pdTextFragmentPtr);
		ToTextProp(power_doc, obj);
		pdLinkDel(power_doc, obj, pdTextFragmentPtr); // remove any subsequent text fragments
		return;
	}

	DT_ID_UBYTE text_type = TX_UCS4_LE;
	DT_SWORD unicode_flags = TX_IMPORT_FULL;

	     if (BufferFormat == TX_BUFFER_FORMAT_4) text_type = TX_UCS4_LE;
	else if (BufferFormat == TX_BUFFER_FORMAT_3) text_type = TX_BUFFER3_LE;
	else if (BufferFormat == TX_BUFFER_FORMAT_2) text_type = TX_UCS2_LE;
	else if (BufferFormat == TX_BUFFER_FORMAT_1) text_type = TX_ANSI;

	//DT_STREAM_FILE(fontmap_sd, "fontmap.inf");
	//if (stream_fontmap == DV_NULL) stream_fontmap = &fontmap_sd;

	DT_TXDOC text_doc = DV_NULL;
	DT_STREAM_MEMORY(text_sd, Addr, Len);

	DT_TX_TEXTFLOW_AREA area = {0, 0, 1000, 1000, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_A, TX_TM_NULL, {DV_NULL}};
	DT_TX_DEFAULT_ATTRIBS default_attribs = {"22221100", "60", "60", "0", "0", "0", "0", "0", "0", "0", DV_NULL, DV_NULL}; /* Note: 22221100 / 60x60 is the standard chrbox color / size */
	if (attribs == DV_NULL) attribs = &default_attribs;

	DT_SLONG obj_text = txTextIniViaPowerDocAndStream(&text_doc, power_doc, text_type, &text_sd, unicode_flags, &area, attribs, stream_fontmap, 0);
	if (text_doc == DV_NULL) obj_text = -1; else txTextExt(text_doc);

	//printf("text_doc=%p, obj=%d, obj_text=%d\n", text_doc, obj, obj_text);

	//pdxTextFragmentChainOptimize(Get_PowerDoc(), obj_text - 1, DV_NULL);
	pdLinkSet(power_doc, obj, pdTextFragmentPtr, obj_text);
}


DT_SLONG CDTValue::GetNrOfRows_FromBuffer(DT_UBYTE* buff_addr, DT_SLONG buff_len, DT_UBYTE buff_format, DT_VALUE_SUBSTRING substr_arr[], DT_SLONG arr_len)
{
	if (buff_addr == DV_NULL || buff_len < 1) return 0;
	if (buff_format < TX_BUFFER_FORMAT_1 || buff_format > TX_BUFFER_FORMAT_4) return 0;

	DT_SLONG nr_of_rows = 0;
	DT_SLONG len2 = buff_len;
	DT_UBYTE* addr1 = buff_addr;
	DT_UBYTE* addr2 = buff_addr;
	DT_ULONG char_code = 0;
	DT_ULONG char_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);
	bool new_row = true;

	if (substr_arr && nr_of_rows < arr_len) substr_arr[nr_of_rows].addr = addr2;

	while (len2 > 0)
	{
		if (new_row) { nr_of_rows++; addr1 = addr2; }

		     if (buff_format == TX_BUFFER_FORMAT_1) char_code = DF_READ_UBYTE_LE(addr2);
		else if (buff_format == TX_BUFFER_FORMAT_2) char_code = DF_READ_UWORD_LE(addr2);
		else if (buff_format == TX_BUFFER_FORMAT_3) char_code = DF_READ_UTRIO_LE(addr2);
		else if (buff_format == TX_BUFFER_FORMAT_4) char_code = DF_READ_ULONG_LE(addr2);

		addr2 += buff_format;
		len2 -= buff_format;

		if (char_code == char_newline)
		{
			if (substr_arr && (nr_of_rows - 1) < arr_len) substr_arr[nr_of_rows - 1].len = (DT_SLONG)(addr2 - addr1);
			if (substr_arr && nr_of_rows < arr_len) substr_arr[nr_of_rows].addr = addr2;

			new_row = true;
		}
		else new_row = false;
	}

	if (substr_arr && (nr_of_rows - 1) < arr_len) substr_arr[nr_of_rows - 1].len = (DT_SLONG)(addr2 - addr1);

	return nr_of_rows;
}


DT_ULONG CDTValue::KillBeforeRow(DT_SLONG row_index)
{
	DT_SLONG i = 0;
	DT_ULONG char_code = 0;
	DT_ULONG char_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);

	if (BufferFormat < TX_BUFFER_FORMAT_1 || BufferFormat > TX_BUFFER_FORMAT_4) return char_code;

	while (Len > 0)
	{
		if (i == row_index) break;

		     if (BufferFormat == TX_BUFFER_FORMAT_1) char_code = DF_READ_UBYTE_LE(Addr);
		else if (BufferFormat == TX_BUFFER_FORMAT_2) char_code = DF_READ_UWORD_LE(Addr);
		else if (BufferFormat == TX_BUFFER_FORMAT_3) char_code = DF_READ_UTRIO_LE(Addr);
		else if (BufferFormat == TX_BUFFER_FORMAT_4) char_code = DF_READ_ULONG_LE(Addr);

		if (char_code == char_newline) i++;

		Addr += BufferFormat;
		Len -= BufferFormat;
	}

	return char_code;
}


void CDTValue::KillExceptRows(DT_SLONG row_index, DT_SLONG nr_of_rows)
{
	DT_SLONG len2;
	DT_UBYTE* addr2;
	DT_ULONG char_code;
	DT_ULONG char_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);

	if (BufferFormat < TX_BUFFER_FORMAT_1 || BufferFormat > TX_BUFFER_FORMAT_4) return;

	KillBeforeRow(row_index);
	addr2 = Addr;
	len2 = Len;
	char_code = KillBeforeRow(nr_of_rows);
	Addr = addr2;
	Len = len2 - Len;

	if (Len < BufferFormat) { Len = 0; return; }
	else if (char_code == char_newline) Len -= BufferFormat;
}


DT_SWORD CDTValue::SortCompareFunction(const void* a1, const void* a2, DT_UBYTE buffer_format)
{
	DT_UBYTE end1;
	DT_UBYTE end2;
	DT_ULONG char_code_1 = 0;
	DT_ULONG char_code_2 = 0;
	DT_ULONG char_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);
	const DT_VALUE_SUBSTRING* s1 = static_cast<const DT_VALUE_SUBSTRING*>(a1);
	const DT_VALUE_SUBSTRING* s2 = static_cast<const DT_VALUE_SUBSTRING*>(a2);
	const DT_UBYTE* addr1 = s1->addr;
	const DT_UBYTE* addr2 = s2->addr;

	if ((s1->len <= 0) && (s2->len <= 0)) return 0; else if (s1->len <= 0) return -1; else if (s2->len <= 0) return 1;

	while (1)
	{
		     if (buffer_format == TX_BUFFER_FORMAT_1) { char_code_1 = DF_READ_UBYTE_LE(addr1); char_code_2 = DF_READ_UBYTE_LE(addr2); }
		else if (buffer_format == TX_BUFFER_FORMAT_2) { char_code_1 = DF_READ_UWORD_LE(addr1); char_code_2 = DF_READ_UWORD_LE(addr2); }
		else if (buffer_format == TX_BUFFER_FORMAT_3) { char_code_1 = DF_READ_UTRIO_LE(addr1); char_code_2 = DF_READ_UTRIO_LE(addr2); }
		else if (buffer_format == TX_BUFFER_FORMAT_4) { char_code_1 = DF_READ_ULONG_LE(addr1); char_code_2 = DF_READ_ULONG_LE(addr2); }

		end1 = (char_code_1 == 0 || char_code_1 == char_newline);
		end2 = (char_code_2 == 0 || char_code_2 == char_newline);

		if (end1 && end2) return 0; else if (end1) return -1; else if (end2) return 1;

		if (char_code_1 < char_code_2) return -1; else if (char_code_1 > char_code_2) return 1;

		addr1++;
		addr2++;
	}

	return 0;
}


void CDTValue::SortBuffer(DT_UBYTE* buff_addr, DT_SLONG buff_len, DT_UBYTE buff_format, bool descending /* = false */, DT_SLONG nr_of_rows /* = -1 */)
{
	if (buff_addr == DV_NULL || buff_len < 1) return;
	if (buff_format < TX_BUFFER_FORMAT_1 || buff_format > TX_BUFFER_FORMAT_4) return;

	if (nr_of_rows <= 0) nr_of_rows = GetNrOfRows_FromBuffer(buff_addr, buff_len, buff_format, DV_NULL, 0);
	if (nr_of_rows < 2) return;

	DT_SLONG len, i, s = sizeof(DT_VALUE_SUBSTRING);
	DT_UBYTE* addr0 = static_cast<DT_UBYTE*>(dtMemAlloc(buff_len)); if (addr0 == DV_NULL) return;
	DT_VALUE_SUBSTRING* substr_arr = static_cast<DT_VALUE_SUBSTRING*>(dtMemAlloc(dtSecMul(nr_of_rows, s))); if (substr_arr == DV_NULL) { dtMemFree(addr0); return; }

	DT_SLONG nr_of_rows2 = GetNrOfRows_FromBuffer(buff_addr, buff_len, buff_format, substr_arr, nr_of_rows);

	//if (nr_of_rows > nr_of_rows2) nr_of_rows = nr_of_rows2;
	if (nr_of_rows != nr_of_rows2) /* should never happen */
	{
		dtMemFree(substr_arr);
		dtMemFree(addr0);
		return;
	}

	//printf("----- Before Sort -----\n");
	//DT_SLONG s = 0;
	//for (i = 0; i < nr_of_rows; i++)
	//{
	//	printf("%ld) addr=%ld, len=%ld, buff_addr=%ld\n", i, substr_arr[i].addr, substr_arr[i].len, buff_addr);
	//	s += substr_arr[i].len;
	//}
	//printf("----------------------- s=%ld, buff_len=%ld\n", s, buff_len);

		 if (buff_format == TX_BUFFER_FORMAT_1) qsort(substr_arr, nr_of_rows, s, SortCompareFunction_1);
	else if (buff_format == TX_BUFFER_FORMAT_2) qsort(substr_arr, nr_of_rows, s, SortCompareFunction_2);
	else if (buff_format == TX_BUFFER_FORMAT_3) qsort(substr_arr, nr_of_rows, s, SortCompareFunction_3);
	else if (buff_format == TX_BUFFER_FORMAT_4) qsort(substr_arr, nr_of_rows, s, SortCompareFunction_4);

	//printf("----- After Sort -----\n");
	//s = 0;
	//for (i = 0; i < nr_of_rows; i++)
	//{
	//	printf("%ld) addr=%ld, len=%ld, buff_addr=%ld\n", i, substr_arr[i].addr, substr_arr[i].len, buff_addr);
	//	s += substr_arr[i].len;
	//}
	//printf("----------------------- s=%ld, buff_len=%ld\n", s, buff_len);

	DT_UBYTE* addr = addr0;
	DT_ULONG char_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);
	DT_SLONG j = 0, inc = 1;
	if (descending) { j = nr_of_rows - 1; inc = -1; }

	//dtMemSet(addr0, 0, buff_len);

	//printf("----- Start Copy -----\n");
	s = 0;
	for (i = 0; i < nr_of_rows; i++, j += inc)
	{
		len = substr_arr[j].len; if (len <= 0) continue;
		dtMemCopy(addr, substr_arr[j].addr, len);
		//printf("%ld) addr=%ld, buff_addr=%ld, substr_arr[j].addr=%ld, len=%ld\n", j, addr, buff_addr, substr_arr[j].addr, len);

		s += len; if (s > buff_len) break; /* should never happen */
		addr += len;

		//*(addr - 1) = char_newline; //((i == nr_of_rows - 1) ? 0 : char_newline);

			 if (buff_format == TX_BUFFER_FORMAT_1) { DF_WRITE_UBYTE_LE(addr - 1, (DT_UBYTE)char_newline); }
		else if (buff_format == TX_BUFFER_FORMAT_2) { DF_WRITE_UWORD_LE(addr - 2, (DT_UWORD)char_newline); }
		else if (buff_format == TX_BUFFER_FORMAT_3) { DF_WRITE_UTRIO_LE(addr - 3, char_newline); }
		else if (buff_format == TX_BUFFER_FORMAT_4) { DF_WRITE_ULONG_LE(addr - 4, char_newline); }
	}
	//printf("----- End Copy -----\n");

	if (s == buff_len) dtMemCopy(buff_addr, addr0, buff_len);

	dtMemFree(substr_arr);
	dtMemFree(addr0);
}


/////////////////////////////////////////////////////////////////////////////
// CDTPowerDocControl


void CDTPowerDocControl::ConfigProc(DT_PDDOC power_doc, bool focus, DT_SLONG w0, DT_SLONG h0, DT_SLONG w1, DT_SLONG h1, DT_SLONG w2, DT_SLONG h2, bool h2_is_infinite)
{
	DT_SLONG obj_name;
	const DT_CHAR* obj_desc;
	DT_SLONG wf = 0, hf = 0, w0_neg = -w0, h0_neg = -h0, wt, ht;
	DT_ID_SLONG obj, obj_last = pdObjGetLast(power_doc);

	if (focus)
	{
		wf = w0;
		hf = h0;
	}

	for (obj = 0; obj <= obj_last; obj++)
	{
		pdObjGet(power_doc, obj, &obj_name, &obj_desc);
		if (obj_name == 90) /* Focus */
		{
			pdPropApplyAsSLong(power_doc, obj, pdDw, wf, 0, PD_APPLY_STANDARD);
			pdPropApplyAsSLong(power_doc, obj, pdDh, hf, 0, PD_APPLY_STANDARD);
		}
		if (obj_name >= 10 && obj_name <= 19) /* Outer Rectangle */
		{
			pdPropApplyAsSLong(power_doc, obj, pdDw, w0, 0, PD_APPLY_STANDARD);
			pdPropApplyAsSLong(power_doc, obj, pdDh, h0, 0, PD_APPLY_STANDARD);
		}
		else if (obj_name >= 20 && obj_name <= 29) /* Inner Rectangle */
		{
			pdPropApplyAsSLong(power_doc, obj, pdDw, w1, 0, PD_APPLY_STANDARD);
			pdPropApplyAsSLong(power_doc, obj, pdDh, h1, 0, PD_APPLY_STANDARD);
		}
		else if (obj_name == 31) /* Text Area */
		{
			wt = (w2 > 0 ? w2 : -1);
			ht = (h2 > 0 ? h2 : -1); if (h2_is_infinite) ht = 0;
			pdPropApplyAsSLong(power_doc, obj, pdTextAreaDw, wt, 0, PD_APPLY_STANDARD);
			pdPropApplyAsSLong(power_doc, obj, pdTextAreaDh, ht, 0, PD_APPLY_STANDARD);
		}
		else if (obj_name == 32) /* Dot, Checkmark, Slider - Outer Rectangle */
		{
			pdPropApplyAsSLong(power_doc, obj, pdDw, w2, 0, PD_APPLY_STANDARD);
			pdPropApplyAsSLong(power_doc, obj, pdDh, h2, 0, PD_APPLY_STANDARD);
		}
		else if (obj_name >= 40 && obj_name <= 49) /* Horizontal Line, Left to Right */
		{
			pdPropApplyAsSLong(power_doc, obj, pdDw, w0, 0, PD_APPLY_STANDARD);
		}
		else if (obj_name >= 50 && obj_name <= 59) /* Vertical Line, Top to Bottom */
		{
			pdPropApplyAsSLong(power_doc, obj, pdDh, h0, 0, PD_APPLY_STANDARD);
		}
		else if (obj_name >= 60 && obj_name <= 69) /* Horizontal Line, Right to Left */
		{
			pdPropApplyAsSLong(power_doc, obj, pdDw, w0_neg, 0, PD_APPLY_STANDARD);
		}
		else if (obj_name >= 70 && obj_name <= 79) /* Vertical Line, Bottom to Top */
		{
			pdPropApplyAsSLong(power_doc, obj, pdDh, h0_neg, 0, PD_APPLY_STANDARD);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDTButton


bool CDTButton::Load(DT_PDENGINE engine, const DT_CHAR* lib_path)
{
	DT_SLONG page, img, img_last, x, y, w0, h0, w1, h1;
	DT_ID_SLONG obj_outer_rect;
	DT_ID_SLONG obj_inner_rect;
	DT_SLONG obj_name;
	const DT_CHAR* obj_desc;

	PowerDoc = DV_NULL;

	// Initialize and load control's PowerDoc
	if (pdDocIni(&PowerDoc, engine) == 0) return false;

	//DT_PD_DOC_PARAMS params = {4, 6, 10, 10, 2, 0, 1, 2 | 8};
	//pdDocSetParams(PowerDoc, &params, 0);

	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_GLYPHS, 1);
	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_ALGORITHMS, 2 | 8);

	DT_STREAM_FILE(sd, lib_path);
	if (pdDocAppendFromStream(PowerDoc, 0, 0, 0, &sd) != 1) return false;

	LastObj = pdObjGetLast(PowerDoc);

	// Find and precompute important features
	obj_outer_rect = pdObjFindByNameAndDesc(PowerDoc, 10, DV_NULL); if (obj_outer_rect < 0) return false;
	obj_inner_rect = pdObjFindByNameAndDesc(PowerDoc, 20, DV_NULL); if (obj_inner_rect < 0) return false;
	ObjText = pdObjFindByNameAndDesc(PowerDoc, 31, DV_NULL); if (ObjText < 0) { ObjText = pdObjFindByNameAndDesc(PowerDoc, 30, DV_NULL); if (ObjText < 0) return false; }
	ObjFragment = pdLinkGet(PowerDoc, ObjText, pdTextFragmentPtr); if (ObjFragment < 0) return false;

	w0 = pdPropGetAsSLong(PowerDoc, obj_outer_rect, pdDw, 500);
	h0 = pdPropGetAsSLong(PowerDoc, obj_outer_rect, pdDh, 100);
	w1 = pdPropGetAsSLong(PowerDoc, obj_inner_rect, pdDw, 500);
	h1 = pdPropGetAsSLong(PowerDoc, obj_inner_rect, pdDh, 100);

	Size.X = w0;
	Size.Y = h0;
	Padding.X = w1 - w0;
	Padding.Y = h1 - h0;

	w1 = pdPropGetAsSLong(PowerDoc, ObjText, pdTextAreaDw, 500);
	h1 = pdPropGetAsSLong(PowerDoc, ObjText, pdTextAreaDh, 100);

	Padding2.X = w1 - w0;
	Padding2.Y = h1 - h0;

	for (page = 0; page <= 255; page++) ValueImg[page] = -1;

	img_last = pdImgGetLast(PowerDoc);
	for (img = 0; img <= img_last; img++)
	{
		if ((page = pdImgGetPage(PowerDoc, img)) < 0 || page > 255) continue;
		pdObjGet(PowerDoc, pdImgGetObj(PowerDoc, img), &obj_name, &obj_desc);
		if (obj_name != 30 && obj_name != 33) continue;

		pdImgGetXY(PowerDoc, img, &x, &y);
		ValueImg[page] = img;
		ValuePos[page].X = x;
		ValuePos[page].Y = y;
	}

	return true;
}


DT_PDDOC CDTButton::Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_PDDOC power_doc, DT_ID_SLONG c_obj)
{
	UsesRichText = false;

	if (ValueImg[source_page] >= 0)
	{
		pdImgSetXY(PowerDoc, ValueImg[source_page], ValuePos[source_page].X + (fc_width - Size.X) / 2, ValuePos[source_page].Y + (fc_height - Size.Y) / 2);
	}

	DT_SLONG px = Padding.X, p2x = Padding2.X; if (fc_width <= 0) { px = p2x = 0; }
	DT_SLONG py = Padding.Y, p2y = Padding2.Y; if (fc_height <= 0) { py = p2y = 0; }

	ConfigProc(PowerDoc, focus, fc_width, fc_height, fc_width + px, fc_height + py, fc_width + p2x, fc_height + p2y, false);

	DT_ID_SLONG obj;

	for (obj = pdObjGetLast(PowerDoc); obj > LastObj; obj--) { pdObjDel(PowerDoc, obj); }

	bool is_rich = false;
	CDTValue fc_value;

	pdLinkDel(PowerDoc, ObjText, pdTextFragmentPtr);
	obj = ObjFragment;

	if (fc_value.MakeByPowerObj(power_doc, c_obj, &is_rich) != DV_NULL)
	{
		fc_value.ToTextProp(PowerDoc, ObjFragment);
	}
	else if (is_rich)
	{
		UsesRichText = true;

		DT_ID_SLONG obj_fragment = pdLinkGet(power_doc, c_obj, pdTextFragmentPtr); if (obj_fragment < 0) return PowerDoc;

		if ((obj = pdObjCopySimple(power_doc, PowerDoc, -2, obj_fragment)) == -1) return PowerDoc;
	}

	pdLinkAdd(PowerDoc, ObjText, pdTextFragmentPtr, obj);

	return PowerDoc;
}


void CDTButton::Unload()
{
	if (PowerDoc != DV_NULL) pdDocExt(PowerDoc);
}



/////////////////////////////////////////////////////////////////////////////
// CDTSelectButton


bool CDTSelectButton::Load(DT_PDENGINE engine, const DT_CHAR* lib_path)
{
	DT_SLONG page, img, img_last, x, y, w0, h0, w1, h1;
	DT_ID_SLONG obj_outer_rect;
	DT_ID_SLONG obj_inner_rect;
	DT_ID_SLONG obj_dot_rect;
	DT_SLONG obj_name;
	const DT_CHAR* obj_desc;

	PowerDoc = DV_NULL;

	if (pdDocIni(&PowerDoc, engine) == 0) return false;

	//DT_PD_DOC_PARAMS params = {4, 6, 10, 10, 2, 0, 1, 2 | 8};
	//pdDocSetParams(PowerDoc, &params, 0);

	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_GLYPHS, 1);
	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_ALGORITHMS, 2 | 8);

	DT_STREAM_FILE(sd, lib_path);
	if (pdDocAppendFromStream(PowerDoc, 0, 0, 0, &sd) != 1) return false;

	// Find and precompute important features
	obj_outer_rect = pdObjFindByNameAndDesc(PowerDoc, 10, DV_NULL); if (obj_outer_rect < 0) return false;
	obj_inner_rect = pdObjFindByNameAndDesc(PowerDoc, 20, DV_NULL); if (obj_inner_rect < 0) return false;
	obj_dot_rect = pdObjFindByNameAndDesc(PowerDoc, 32, DV_NULL); if (obj_dot_rect < 0) return false;

	w0 = pdPropGetAsSLong(PowerDoc, obj_outer_rect, pdDw, 500);
	h0 = pdPropGetAsSLong(PowerDoc, obj_outer_rect, pdDh, 100);
	w1 = pdPropGetAsSLong(PowerDoc, obj_inner_rect, pdDw, 500);
	h1 = pdPropGetAsSLong(PowerDoc, obj_inner_rect, pdDh, 100);

	Size.X = w0;
	Size.Y = h0;
	Padding.X = w1 - w0;
	Padding.Y = h1 - h0;

	w1 = pdPropGetAsSLong(PowerDoc, obj_dot_rect, pdDw, 500);
	h1 = pdPropGetAsSLong(PowerDoc, obj_dot_rect, pdDh, 100);

	Size2.X = w1;
	Size2.Y = h1;

	for (page = 0; page <= 255; page++) ValueImg[page] = -1;

	img_last = pdImgGetLast(PowerDoc);
	for (img = 0; img <= img_last; img++)
	{
		if ((page = pdImgGetPage(PowerDoc, img)) < 0 || page > 255) continue;
		pdObjGet(PowerDoc, pdImgGetObj(PowerDoc, img), &obj_name, &obj_desc);
		if (obj_name != 32) continue;

		pdImgGetXY(PowerDoc, img, &x, &y);
		ValueImg[page] = img;
		ValuePos[page].X = x;
		ValuePos[page].Y = y;
	}

	return true;
}


DT_PDDOC CDTSelectButton::Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height) const
{
	if (Size.X == 0 || Size.Y == 0) return PowerDoc;

	pdImgSetXY(PowerDoc, ValueImg[source_page], (ValuePos[source_page].X * fc_width) / Size.X, (ValuePos[source_page].Y * fc_height) / Size.Y);
	ConfigProc(PowerDoc, focus, fc_width, fc_height, fc_width + Padding.X, fc_height + Padding.Y, (Size2.X * fc_width) / Size.X, (Size2.Y * fc_height) / Size.Y, false);

	return PowerDoc;
}


void CDTSelectButton::Unload()
{
	if (PowerDoc != DV_NULL) pdDocExt(PowerDoc);
}



/////////////////////////////////////////////////////////////////////////////
// CDTTextBox


bool CDTTextBox::Load(DT_PDENGINE engine, const DT_CHAR* lib_path)
{
	DT_SWORD rs;
	DT_SLONG fh;
	DT_SLONG page, img, img_last, x, y, w0, h0, w1, h1;
	DT_ID_SLONG obj_outer_rect;
	DT_ID_SLONG obj_inner_rect;
	DT_ID_SLONG obj_layout;
	DT_ID_SLONG obj_typography;
	DT_SLONG obj_name;
	const DT_CHAR* obj_desc;

	PowerDoc = DV_NULL;

	// Initialize and load control's PowerDoc
	if (pdDocIni(&PowerDoc, engine) == 0) return false;

	//DT_PD_DOC_PARAMS params = {4, 6, 10, 10, 2, 0, 1, 2 | 8};
	//pdDocSetParams(PowerDoc, &params, 0);

	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_GLYPHS, 1);
	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_ALGORITHMS, 2 | 8);

	DT_STREAM_FILE(sd, lib_path);
	if (pdDocAppendFromStream(PowerDoc, 0, 0, 0, &sd) != 1) return false;

	LastObj = pdObjGetLast(PowerDoc);

	// Find and precompute important features
	obj_outer_rect = pdObjFindByNameAndDesc(PowerDoc, 10, DV_NULL); if (obj_outer_rect < 0) return false;
	obj_inner_rect = pdObjFindByNameAndDesc(PowerDoc, 20, DV_NULL); if (obj_inner_rect < 0) return false;
	ObjText = pdObjFindByNameAndDesc(PowerDoc, 31, DV_NULL); if (ObjText < 0) return false;
	ObjFragment = pdLinkGet(PowerDoc, ObjText, pdTextFragmentPtr); if (ObjFragment < 0) return false;

	obj_layout = pdLinkGet(PowerDoc, ObjFragment, pdTextLayoutPtr);

	DefaultAlign = pdPropGetAsUByte(PowerDoc, obj_layout, pdAlign, 0);
	DefaultAlignEnd = pdPropGetAsUByte(PowerDoc, obj_layout, pdAlignEnd, 0);

	obj_typography = pdObjFindByNameAndDesc(PowerDoc, 80, DV_NULL); if (obj_typography < 0) return false;

	w0 = pdPropGetAsSLong(PowerDoc, obj_outer_rect, pdDw, 500);
	h0 = pdPropGetAsSLong(PowerDoc, obj_outer_rect, pdDh, 100);
	w1 = pdPropGetAsSLong(PowerDoc, obj_inner_rect, pdDw, 500);
	h1 = pdPropGetAsSLong(PowerDoc, obj_inner_rect, pdDh, 100);

	Size.X = w0;
	Size.Y = h0;
	Padding.X = w1 - w0;
	Padding.Y = h1 - h0;

	BoxX = 0;
	//BoxW=w1;

	w1 = pdPropGetAsSLong(PowerDoc, ObjText, pdTextAreaDw, 500);
	h1 = pdPropGetAsSLong(PowerDoc, ObjText, pdTextAreaDh, 100);
	rs = pdPropGetAsSWord(PowerDoc, ObjText, pdSpacingRow, 0);

	Padding2.X = w1 - w0;
	Padding2.Y = h1 - h0;

	BoxY = 0;
	//BoxH=h1;

	fh = pdPropGetAsSLong(PowerDoc, obj_typography, pdFontDh, 20);

	FontH = fh + rs;

	for (page = 0; page <= 255; page++)
	{
		LineHImg[page] = -1;
		LineVImg[page] = -1;
	}

	img_last = pdImgGetLast(PowerDoc);

	LastImg = img_last;

	for (img = 0; img <= img_last; img++)
	{
		if ((page = pdImgGetPage(PowerDoc, img)) < 0 || page > 255) continue;
		pdObjGet(PowerDoc, pdImgGetObj(PowerDoc, img), &obj_name, &obj_desc);
		if (obj_name >= 60 && obj_name <= 69) LineHImg[page] = img;
		else if (obj_name >= 70 && obj_name <= 79) LineVImg[page] = img;
		else if (obj_name == 20) /* Inner Rect */
		{
			pdImgGetXY(PowerDoc, img, &x, &y);
			BoxX = x;
		}
		else if (obj_name == 31) /* Text Area */
		{
			TextImg[page] = img;
			pdImgGetXY(PowerDoc, img, &x, &y);
			BoxY = y;
		}
	}

	return true;
}


DT_SLONG CDTTextBox::GetTextImg(DT_SLONG source_page) const
{
	return TextImg[source_page];
}


void CDTTextBox::GetBoxWH(DT_SLONG fc_width, DT_SLONG fc_height, DT_SLONG* box_w, DT_SLONG* box_h) const
{
	*box_w = fc_width + Padding.X;
	*box_h = fc_height + Padding2.Y;
}


void CDTTextBox::SelectedRows(DT_PDDOC power_doc, DT_SLONG fc_first_item, const DT_UBYTE* fc_selected_items, DT_SLONG nr_of_selected_items)
{
	pdPropDel(power_doc, ObjText, pdRowInvertArr);

	if (fc_selected_items == DV_NULL || nr_of_selected_items < 1) return;

	DT_SLONG i, j, max_item = 0, selected_item;

	for (i = 0; i < nr_of_selected_items; i++)
	{
		selected_item = DF_READ_SLONG_LE(fc_selected_items + 4 * i);
		//printf("nr_of_selected_items=%d, selected_item=%d\n", nr_of_selected_items, selected_item);
		if (selected_item > max_item) max_item = selected_item;
	}

	if (max_item < fc_first_item) return;

	DT_UBYTE val0 = 0;
	DT_SLONG len1 = max_item + 1 - fc_first_item, len2 = 0;

	pdPropAddPlus(power_doc, ObjText, pdRowInvertArr, &val0, len1, 1 /* = fill with zeros */);
	DT_UBYTE* addr = pdPropGet(power_doc, ObjText, pdRowInvertArr, &len2);

	if (addr == DV_NULL || len1 != len2) { /*printf("ERR addr=%p, len1=%d, len2=%d\n", addr, len1, len2);*/ return; }

	for (i = 0; i < nr_of_selected_items; i++)
	{
		selected_item = DF_READ_SLONG_LE(fc_selected_items + 4 * i);
		j = selected_item - fc_first_item; if (j < 0) continue;
		//printf("fc_selected_item=%d, fc_first_item=%d, addr[%d]=1\n", fc_selected_items[i], fc_first_item, j);
		addr[j] = InvertCmd[0];
	}
}


void CDTTextBox::SelectedRows_Approx(DT_PDDOC power_doc, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_SLONG fc_first_item, const DT_UBYTE* fc_selected_items, DT_SLONG nr_of_selected_items)
{
	if (fc_selected_items == DV_NULL || nr_of_selected_items < 1) return;

	DT_SLONG box_w = 0, box_h = 0;

	GetBoxWH(fc_width, fc_height, &box_w, &box_h);

	DT_SLONG approx_nr_of_visible_items = (FontH > 0 ? dtSecAdd(box_h / FontH, 5) : 0); if (approx_nr_of_visible_items < 0) return;

	/* Calculate and create selection bar */

	DT_UBYTE* items_array = static_cast<DT_UBYTE*>(dtMemAlloc(dtSecAdd(approx_nr_of_visible_items, 1))); if (items_array == DV_NULL) return;
	DT_SLONG x = BoxX, y;
	DT_SLONG w, h = FontH, h2;
	DT_SLONG n, i, i_start, i_end, selected_item;
	DT_SLONG n_max = 0;
	DT_SLONG n_min = approx_nr_of_visible_items;

	box_h -= h;
	w = box_w;

	/* Only draw visible selection bars. Also, merge neighbouring bars into a single bar. */
	for (i = 0; i < approx_nr_of_visible_items; i++) items_array[i] = 0;
	for (i = 0; i < nr_of_selected_items; i++)
	{
		selected_item = DF_READ_SLONG_LE(fc_selected_items + 4 * i);
		if ((n = selected_item - fc_first_item) < 0) continue;
		else if (n * h > box_h) continue;

		if (n > n_max) n_max = n;
		if (n < n_min) n_min = n;
		items_array[n] = 1;
	}

	/* Proceed only if there is selection in the visible area */
	if (n_max < n_min) { dtMemFree(items_array); return; }

	DT_ID_SLONG obj, obj_body = pdObjAdd(power_doc, 0, "Body");

	i_start = n_min;
	for (i = n_min; i <= n_max + 1; i++)
	{
		/* If the bar start exists and we reach its end */
		if (i_start != -1 && items_array[i] == 0)
		{
			i_end = i;
			if (i == n_max) i_end++;

			y = i_start * h + BoxY;
			h2 = h * (i_end - i_start);

			/* Create an inverter */
			if ((obj = pdObjAdd(power_doc, 0, "Inverter")) >= 0)
			{
				pdPropAddAsSLong(power_doc, obj, pdDw, w);
				pdPropAddAsSLong(power_doc, obj, pdDh, h2);
				//pdImgAddPlus(power_doc, obj, source_page, x, y, 0, 0, 0, pdDrawGradient, DV_NULL);

				pdLinkAdd(power_doc, obj, pdBodyPtr, obj_body);
				pdImgAddPlus(power_doc, obj, source_page, x, y, 0, 2, 0, pdDrawRect, DV_NULL);
			}

			i_start = -1;
		}
		else if (i_start == -1 && items_array[i] == 1) i_start = i;
	}

	dtMemFree(items_array);
}


DT_PDDOC CDTTextBox::Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_PDDOC power_doc, DT_ID_SLONG c_obj, DT_SLONG start_position, const DT_UBYTE* fc_selected_items, DT_SLONG nr_of_selected_items, DT_UBYTE wrap_type, DT_UBYTE row_type, bool h2_is_infinite, DT_SLONG fc_first_row)
{
	UsesRichText = false;

	pdImgSetXY(PowerDoc, LineHImg[source_page], fc_width, fc_height);
	pdImgSetXY(PowerDoc, LineVImg[source_page], fc_width, fc_height);

	DT_SLONG px = Padding.X, p2x = Padding2.X; if (fc_width <= 0) { px = p2x = 0; }
	DT_SLONG py = Padding.Y, p2y = Padding2.Y; if (fc_height <= 0) { py = p2y = 0; }

	ConfigProc(PowerDoc, focus, fc_width, fc_height, fc_width + px, fc_height + py, fc_width + p2x, fc_height + p2y, h2_is_infinite);

	DT_SLONG img;
	DT_ID_SLONG obj;

	for (img = pdImgGetLast(PowerDoc); img > LastImg; img--) { pdImgDel(PowerDoc, img); }
	for (obj = pdObjGetLast(PowerDoc); obj > LastObj; obj--) { pdObjDel(PowerDoc, obj); }
	pdPropDel(power_doc, ObjText, pdRowInvertArr);

	pdPropApplyAsUByte(PowerDoc, ObjText, pdTextAreaWrapType, wrap_type, 0, PD_APPLY_STANDARD);
	pdPropApplyAsUByte(PowerDoc, ObjText, pdTextAreaRowType, row_type, 0, PD_APPLY_STANDARD);

	if (c_obj < 0) return PowerDoc;

	bool is_rich = false;
	CDTValue fc_value;

	pdLinkDel(PowerDoc, ObjText, pdTextFragmentPtr);
	obj = ObjFragment;

	if (fc_value.MakeByPowerObj(power_doc, c_obj, &is_rich) != DV_NULL)
	{
		fc_value.ToTextProp(PowerDoc, ObjFragment);

		DT_ID_SLONG obj_layout = pdLinkGet(PowerDoc, obj, pdTextLayoutPtr);
		DT_UBYTE align = pdPropGetAsUByte(power_doc, c_obj, fcAlign, DefaultAlign);
		DT_UBYTE align_end = pdPropGetAsUByte(power_doc, c_obj, fcAlignEnd, DefaultAlignEnd);
		/*if (align != 255)*/ pdPropApplyAsUByte(PowerDoc, obj_layout, pdAlign, align, 0, PD_APPLY_STANDARD);
		/*if (align_end != 255)*/ pdPropApplyAsUByte(PowerDoc, obj_layout, pdAlignEnd, align_end, 0, PD_APPLY_STANDARD);
	}
	else if (is_rich)
	{
		UsesRichText = true;

		DT_ID_SLONG obj_fragment = pdLinkGet(power_doc, c_obj, pdTextFragmentPtr); if (obj_fragment < 0) return PowerDoc;

		if ((obj = pdObjCopySimple(power_doc, PowerDoc, -2, obj_fragment)) == -1) return PowerDoc;
	}

	pdLinkAdd(PowerDoc, ObjText, pdTextFragmentPtr, obj);

	txObjCharsDelete(PowerDoc, ObjText, 0, start_position, DV_NULL, 2); /* kill first start_position characters */
	//pdPropApplyAsSLong(PowerDoc, ObjText, pdGlyphStart, start_position, 0, PD_APPLY_STANDARD);

	//SelectedRows_Approx(PowerDoc, source_page, fc_width, fc_height, fc_first_row, fc_selected_items, nr_of_selected_items);
	SelectedRows(PowerDoc, fc_first_row, fc_selected_items, nr_of_selected_items);

	return PowerDoc;
}


DT_SLONG CDTTextBox::GetNrOfVisibleRows(DT_SLONG fc_height) const
{
	DT_SLONG box_w, box_h;

	GetBoxWH(0, fc_height, &box_w, &box_h);
	return (FontH > 0 ? (box_h / FontH) : 0);
}


DT_SLONG CDTTextBox::GetItemByCoord(DT_SLONG first_item, DT_SLONG y, const CDTValue* fc_value) const
{
	DT_SLONG item_height = FontH; if (item_height <= 0) return 0;
	DT_SLONG d = y - BoxY, item;
	DT_SLONG nr_of_items = -1; if (fc_value != DV_NULL) nr_of_items = fc_value->GetNrOfRows();

	if (d >= 0) item = d / item_height; else item = -((-d) / item_height + 1);

	item += first_item;

	if (item < 0) item = 0; else if (nr_of_items >= 0 && item >= nr_of_items) item = nr_of_items - 1;

	return item;
}


void CDTTextBox::Unload()
{
	if (PowerDoc != DV_NULL) pdDocExt(PowerDoc);
}



/////////////////////////////////////////////////////////////////////////////
// CDTListBox


bool CDTListBox::Load(DT_PDENGINE engine, const DT_CHAR* lib_path)
{
	PowerDoc = DV_NULL;

	// Initialize and load control's PowerDoc
	if (pdDocIni(&PowerDoc, engine) == 0) return false;

	//DT_PD_DOC_PARAMS params = {4, 6, 10, 10, 2, 0, 1, 2 | 8};
	//pdDocSetParams(PowerDoc, &params, 0);

	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_GLYPHS, 1);
	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_ALGORITHMS, 2 | 8);

	// Load text box
	if (!DTTextBox.Load(engine, lib_path)) return false;

	return true;
}


DT_PDDOC CDTListBox::Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_PDDOC power_doc, DT_ID_SLONG c_obj, DT_SLONG fc_first_item, const DT_UBYTE* fc_selected_items, DT_SLONG nr_of_selected_items)
{
	DT_PDDOC power_doc_for_text_box = DTTextBox.Get(focus, source_page, fc_width, fc_height, power_doc, -1, 0, DV_NULL, 0, 4, 11, false, 0); if (power_doc_for_text_box == DV_NULL) return PowerDoc;

	pdDocErase(PowerDoc);
	pdDocAppend(PowerDoc, 0, 0, 0, power_doc_for_text_box);

	DT_SLONG i, nr_of_columns = 0;
	DT_SLONG x = 0, y = 0, w = 0, box_w = 0, box_h = 0, x2 = 0, x_max = 0;
	DT_SLONG img_new, img;

	DTTextBox.GetBoxWH(fc_width, fc_height, &box_w, &box_h);
	DT_SLONG approx_nr_of_visible_items = (DTTextBox.FontH > 0 ? dtSecAdd(box_h / DTTextBox.FontH, 5) : 0); if (approx_nr_of_visible_items < 0) return PowerDoc;

	DT_ID_SLONG obj = c_obj, obj_new;
	DT_ID_SLONG obj_layout, obj_layout_new;
	DT_ID_SLONG obj_fragment, obj_fragment_new;

	bool is_rich = false;
	CDTListBoxColumn col[100];

	while (nr_of_columns < 100)
	{
		col[nr_of_columns].Val.MakeByPowerObj(power_doc, obj, &is_rich);
		col[nr_of_columns].Off = pdPropGetAsSLong(power_doc, obj, fcListColOffset, 100);
		col[nr_of_columns].Alg = pdPropGetAsUByte(power_doc, obj, fcAlign, DTTextBox.DefaultAlignEnd);
		nr_of_columns++;
		if ((obj = pdLinkGet(power_doc, obj, fcColumnNextPtr)) < 0) break;
	}

	img_new = img = DTTextBox.GetTextImg(source_page);
	obj_new = obj = pdImgGetObj(PowerDoc, img);
	obj_fragment_new = obj_fragment = pdLinkGet(PowerDoc, obj, pdTextFragmentPtr);
	obj_layout_new = obj_layout = pdLinkGet(PowerDoc, obj_fragment, pdTextLayoutPtr);

	//if (col[0].Alg != 255) pdPropApplyAsUByte(PowerDoc, obj_layout, pdAlignEnd, col[0].Alg, 0, PD_APPLY_STANDARD);

	/* Make columns */

	pdImgGetXY(PowerDoc, img, &x, &y);
	x_max = x + pdPropGetAsSLong(PowerDoc, obj, pdTextAreaDw, fc_width);

	for (i = 0; i < nr_of_columns; x = x2, i++)
	{
		if (i + 1 < nr_of_columns) x2 = DF_MIN(x + col[i + 1].Off, x_max); else x2 = x_max;

		//w = DF_MAX(0, x2 - x);
		w = x2 - x; if (w <= 0) w = -1;

		if (i > 0) /* for additional columns */
		{
			if (col[i].Val.GetLen() < 0 || w <= 0) continue;

			img_new = pdImgDup(PowerDoc, img);
			obj_new = pdObjDup(PowerDoc, obj);
			obj_fragment_new = pdObjDup(PowerDoc, obj_fragment);
			obj_layout_new = pdObjDup(PowerDoc, obj_layout);

			pdImgSetObj(PowerDoc, img_new, obj_new);
			pdImgSetXY(PowerDoc, img_new, x, y);

			pdLinkSet(PowerDoc, obj_new, pdTextFragmentPtr, obj_fragment_new);
			pdLinkSet(PowerDoc, obj_fragment_new, pdTextLayoutPtr, obj_layout_new);
		}

		pdPropApplyAsSLong(PowerDoc, obj_new, pdTextAreaDw, w, 0, PD_APPLY_STANDARD);
		/*if (col[i].Alg != 255)*/ pdPropApplyAsUByte(PowerDoc, obj_layout_new, pdAlignEnd, col[i].Alg, 0, PD_APPLY_STANDARD);

		col[i].Val.KillExceptRows(fc_first_item, approx_nr_of_visible_items);
		col[i].Val.ToTextProp(PowerDoc, obj_fragment_new);
	}

	//DTTextBox.SelectedRows_Approx(PowerDoc, source_page, fc_width, fc_height, fc_first_item, fc_selected_items, nr_of_selected_items);
	DTTextBox.SelectedRows(PowerDoc, fc_first_item, fc_selected_items, nr_of_selected_items);

	return PowerDoc;
}


//DT_SLONG CDTListBox::GetNrOfVisibleRows(DT_SLONG fc_height) const
//{
//	DT_SLONG box_w, box_h;
//
//	DTTextBox.GetBoxWH(0, fc_height, &box_w, &box_h);
//	return (DTTextBox.FontH > 0 ? (box_h / DTTextBox.FontH) : 0);
//}
//
//
//DT_SLONG CDTListBox::GetItemByCoord(DT_SLONG first_item, DT_SLONG y, const CDTValue* fc_value) const
//{
//	DT_SLONG item_height = DTTextBox.FontH; if (item_height <= 0) return 0;
//	DT_SLONG d = y - DTTextBox.BoxY, item;
//	DT_SLONG nr_of_items = 99999; if (fc_value != DV_NULL) nr_of_items = fc_value->GetNrOfRows();
//
//	if (d >= 0) item = d / item_height; else item = -((-d) / item_height + 1);
//
//	item += first_item;
//
//	if (item < 0) item = 0; else if (item >= nr_of_items) item = nr_of_items - 1;
//
//	return item;
//}


DT_SLONG CDTListBox::GetMaxFirstItem(DT_SLONG fc_height, CDTValue& fc_value) const
{
	DT_SLONG MaxFirstItem = fc_value.GetNrOfRows() - GetNrOfVisibleRows(fc_height);
	if (MaxFirstItem < 0) MaxFirstItem = 0;
	return MaxFirstItem;
}


void CDTListBox::Unload()
{
	DTTextBox.Unload();
	if (PowerDoc != DV_NULL) pdDocExt(PowerDoc);
}



/////////////////////////////////////////////////////////////////////////////
// CDTSlider


bool CDTSlider::Load(DT_PDENGINE engine, const DT_CHAR* lib_path, bool hor_ver_flag)
{
	DT_ID_SLONG obj_outer_rect;
	DT_ID_SLONG obj_inner_rect;
	DT_ID_SLONG obj_slider_outer_rect;
	DT_SLONG w0, h0, w1, h1;

	DT_SLONG page, img, img_last;
	DT_SLONG obj_name;
	const DT_CHAR* obj_desc;

	PowerDoc = DV_NULL;
	IsHorizontal = hor_ver_flag;

	if (pdDocIni(&PowerDoc, engine) == 0) return false;

	//DT_PD_DOC_PARAMS params = {4, 6, 10, 10, 2, 0, 1, 2 | 8};
	//pdDocSetParams(PowerDoc, &params, 0);

	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_GLYPHS, 1);
	pdDocSetNumericValue(PowerDoc, PD_NVAL_CONTROL_ALGORITHMS, 2 | 8);

	DT_STREAM_FILE(sd, lib_path);
	if (pdDocAppendFromStream(PowerDoc, 0, 0, 0, &sd) != 1) return false;

	// Find and precompute important features
	obj_outer_rect = pdObjFindByNameAndDesc(PowerDoc, 10, DV_NULL); if (obj_outer_rect < 0) return false;
	obj_inner_rect = pdObjFindByNameAndDesc(PowerDoc, 20, DV_NULL); if (obj_inner_rect < 0) return false;
	obj_slider_outer_rect = pdObjFindByNameAndDesc(PowerDoc, 32, DV_NULL); if (obj_slider_outer_rect < 0) return false;

	w0 = pdPropGetAsSLong(PowerDoc, obj_outer_rect, pdDw, 500);
	h0 = pdPropGetAsSLong(PowerDoc, obj_outer_rect, pdDh, 100);
	w1 = pdPropGetAsSLong(PowerDoc, obj_inner_rect, pdDw, 500);
	h1 = pdPropGetAsSLong(PowerDoc, obj_inner_rect, pdDh, 100);

	Size.X = w0;
	Size.Y = h0;
	Padding.X = w1 - w0;
	Padding.Y = h1 - h0;

	w1 = pdPropGetAsSLong(PowerDoc, obj_slider_outer_rect, pdDw, 500);
	h1 = pdPropGetAsSLong(PowerDoc, obj_slider_outer_rect, pdDh, 100);

	Size2.X = w1;
	Size2.Y = h1;

	for (page = 0; page <= 255; page++) ValueImg[page] = -1;

	img_last = pdImgGetLast(PowerDoc);
	for (img = 0; img <= img_last; img++)
	{
		if ((page = pdImgGetPage(PowerDoc, img)) < 0 || page > 255) continue;
		pdObjGet(PowerDoc, pdImgGetObj(PowerDoc, img), &obj_name, &obj_desc);
		if (obj_name != 32) continue;

		ValueImg[page] = img;
	}

	return true;
}


DT_PDDOC CDTSlider::Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_FLOAT coverage_factor, DT_FLOAT position_factor) const
{
	DT_SLONG w2, h2, x, y;

	if (IsHorizontal)
	{
		h2 = fc_height + Padding.Y;
		w2 = DF_ROUND((fc_width + Padding.X) * coverage_factor);
		x = DF_ROUND((fc_width + Padding.X - w2) * position_factor);
		pdImgSetXY(PowerDoc, ValueImg[source_page], x - Padding.X / 2, -Padding.Y / 2);
	}
	else
	{
		w2 = fc_width + Padding.X;
		h2 = DF_ROUND((fc_height + Padding.Y) * coverage_factor);
		y = DF_ROUND((fc_height + Padding.Y - h2) * position_factor);
		pdImgSetXY(PowerDoc, ValueImg[source_page], -Padding.X / 2, y - Padding.Y / 2);
	}

	ConfigProc(PowerDoc, focus, fc_width, fc_height, fc_width + Padding.X, fc_height + Padding.Y, w2, h2, false);

	return PowerDoc;
}


DT_SWORD CDTSlider::GetDirection(DT_SLONG length, DT_FLOAT coverage_factor, DT_FLOAT position_factor, DT_SLONG position) const
{
	DT_SLONG l, t, p;

	if (IsHorizontal) p = Padding.X; else p = Padding.Y;

	l = DF_ROUND((length + p) * coverage_factor);
	t = DF_ROUND((length + p - l) * position_factor);

	if (position < t - p / 2) return -1; else if (position > t - p / 2 + l) return 1;
	return 0;
}


DT_SLONG CDTSlider::GetInfoFromPowerDoc(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG* slider_limit, DT_UWORD* slider_coverage)
{
	DT_SLONG slider_position = pdPropGetAsSLong(power_doc, obj, fcSliderPosition, 0);
	*slider_limit = pdPropGetAsSLong(power_doc, obj, fcSliderLimit, 100);
	*slider_coverage = pdPropGetAsUWord(power_doc, obj, fcSliderCoverage, 1000);

	if (slider_position < 0) slider_position = 0; else if (slider_position > *slider_limit) slider_position = *slider_limit;
	if (*slider_coverage > 10000) *slider_coverage = 10000;

	return slider_position;
}


void CDTSlider::Unload()
{
	if (PowerDoc != DV_NULL) pdDocExt(PowerDoc);
}



/////////////////////////////////////////////////////////////////////////////
// CDTFormControlsLib


DT_SLONG CDTFormControlsLib::UserObjDraw(DT_ID_SLONG obj_focus, DT_PD_DOCDRAW_CANVAS* c, DT_SLONG fn_draw, const DT_TM3X3 tm, bool need_frames, DT_SRAST_XL* x, DT_SRAST_XL* y)
{
	if (!IsInitialized) return 0;

	*x = c->ImgX;
	*y = c->ImgY;

	DT_PDDOC power_doc = c->PowerDocInst; if (power_doc == DV_NULL) return 0;
	DT_PDDOC source_doc = DV_NULL;
	const DT_UBYTE* fc_selected_items = DV_NULL;
	DT_SLONG nr_of_selected_items = 0;
	DT_SLONG source_page = 0, f = 0;
	DT_SLONG l;
	DT_SLONG ret = 0;
	DT_SLONG len;
	DT_ID_SLONG fc_config = pdLinkGet(power_doc, c->Obj, fcConfigPtr);
	DT_ID_SWORD lib = GetLib(pdPropGetAsSWord(power_doc, fc_config, fcCnfLib, 0));
	DT_UBYTE fc_state = FC_NORMAL;
	DT_UBYTE fc_wrap_type = 0;
	DT_SLONG fc_width = 500, fc_height = 100;
	DT_SLONG fc_first_item = 0, fc_first_row = 0;
	DT_SLONG slider_position = 0, slider_limit = 100;
	DT_UWORD slider_coverage = 1000;
	DT_FLOAT position_factor;
	DT_PD_DOCDRAW_PARAMS params;
	DT_PD_DOCDRAW_PARAMS* params_ptr = DV_NULL;

	bool focus = false;

	CDTPowerDocControl* control_with_frames = DV_NULL;
	CDTSlider* slider = DTHorSlider;


	if (c->Attribs & 1) goto end; /* hidden image */

	//if (img_focus == c->Img)
	if (obj_focus == c->Obj)
	{
		if (pdPropGetAsUByte(power_doc, fc_config, fcCnfCtrlStyleOnFocus, 0) == 0) focus = true;
		l = pdPropGetAsSWord(power_doc, fc_config, fcCnfCtrlShiftOnFocus, 0);
		//if (l > 0) l = DF_ROUND(l * zoom); else if (l < 0) l = -DF_ROUND(-l * zoom);
		*x += l;
		*y += l;
	}

	fc_state = pdPropGetAsUByte(power_doc, c->Obj, fcState, FC_NORMAL);
	fc_width = pdPropGetAsSLong(power_doc, c->Obj, fcDw, 500);
	fc_height = pdPropGetAsSLong(power_doc, c->Obj, fcDh, 100);
	fc_first_item = pdPropGetAsSLong(power_doc, c->Obj, fcItemFirst, 0); if (fc_first_item < 0) fc_first_item = 0;


	switch (fn_draw)
	{
	case fcDrawButton: /* Button */
	case fcDrawRichButton: /* Rich Button */

		if (fc_config != -1)
		{
			if (fc_state == FC_NORMAL) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfButtonNormal, 0);
			else if (fc_state == FC_PUSHED) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfButtonPushed, 0);
		}

		source_doc = DTButton[lib].Get(focus, source_page, fc_width, fc_height, power_doc, c->Obj);
		control_with_frames = &DTButton[lib];
		//need_frames = false;
		break;

	case fcDrawRadioButton: /* Radio Button */

		if (fc_config != -1)
		{
			if (fc_state == FC_NORMAL) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfRadioButtonNormal, 0);
			else if (fc_state == FC_PUSHED) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfRadioButtonPushed, 0);
			else if (fc_state == FC_SELECTED) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfRadioButtonSelected, 0);
		}

		source_doc = DTRadioButton[lib].Get(focus, source_page, fc_width, fc_height);
		need_frames = false;
		break;

	case fcDrawCheckBox: /* Check Box */

		if (fc_config != -1)
		{
			if (fc_state == FC_NORMAL) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfCheckBoxNormal, 0);
			else if (fc_state == FC_PUSHED) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfCheckBoxPushed, 0);
			else if (fc_state == FC_SELECTED) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfCheckBoxSelected, 0);
		}

		source_doc = DTCheckBox[lib].Get(focus, source_page, fc_width, fc_height);
		need_frames = false;
		break;

	case fcDrawTextBox: /* Text Box */
	case fcDrawRichTextBox: /* Rich Text Box */
	case fcDrawRichListBox: /* Rich List Box */

		if (fc_config != -1) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfCharBox, 0);
		fc_wrap_type = pdPropGetAsUByte(power_doc, c->Obj, fcTextWrapType, 0); if (fn_draw == fcDrawRichListBox) fc_wrap_type = 4;

		if (fn_draw == fcDrawRichListBox)
		{
			fc_first_row = pdPropGetAsSLong(power_doc, c->Obj, fcRowFirst, 0); if (fc_first_row < 0) fc_first_row = 0;
			fc_selected_items = pdPropGet(power_doc, c->Obj, fcListSelectedItems, &len);
			nr_of_selected_items = len / 4;
			if (len != 4 * nr_of_selected_items) { fc_selected_items = DV_NULL; nr_of_selected_items = 0; }
		}

		source_doc = DTTextBox[lib].Get(focus, source_page, fc_width, fc_height, power_doc, c->Obj, fc_first_item, fc_selected_items, nr_of_selected_items, fc_wrap_type, 10, false, fc_first_row);
		control_with_frames = &DTTextBox[lib];
		//need_frames = false;
		break;

	case fcDrawListBox: /* List Box */

		if (fc_config != -1) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfCharBox, 0);

		fc_selected_items = pdPropGet(power_doc, c->Obj, fcListSelectedItems, &len);
		nr_of_selected_items = len / 4;
		if (len != 4 * nr_of_selected_items) { fc_selected_items = DV_NULL; nr_of_selected_items = 0; }

		source_doc = DTListBox[lib].Get(focus, source_page, fc_width, fc_height, power_doc, c->Obj, /*j, col,*/ fc_first_item, fc_selected_items, nr_of_selected_items);
		need_frames = false;
		break;

	case fcDrawVSlider: /* Vertical slider */

		slider = DTVerSlider;
		// fall through

	case fcDrawHSlider: /* Horizontal slider */

		if (fc_config != -1)
		{
			if (fc_state == FC_NORMAL) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfSliderNormal, 0);
			else if (fc_state == FC_PUSHED) source_page = pdPropGetAsUByte(power_doc, fc_config, fcCnfSliderPushed, 0);
		}

		slider_position = CDTSlider::GetInfoFromPowerDoc(power_doc, c->Obj, &slider_limit, &slider_coverage);
		position_factor = (slider_limit == 0 ? 0 : slider_position / (DT_FLOAT)slider_limit);

		source_doc = slider[lib].Get(focus, source_page, fc_width, fc_height, slider_coverage * 0.0001, position_factor);
		need_frames = false;
		break;

	default:

		goto end;
	}


	pdDocSetTransformPlus(source_doc, tm);

	if (need_frames)
	{
		params_ptr = &params;
		pdxDrawParamsReset(params_ptr);
		params_ptr->Flows = 2; /* we want flows with all frames in DT_PD_FRAME_FORMAT2 format */
	}

	DT_MDC dc_mem;
	dc_mem.m = c->Memory;
	dc_mem.l = c->Length;
	dc_mem.w = c->Width;
	dc_mem.h = c->Height;

	pdDocTransfer(source_doc, c->PowerDocInst, 16 | 32 | 64, 0);
	ret = pdDocDrawXL(source_doc, source_page, c->XOff - (*x), c->YOff - (*y), c->Format, c->Subformat, &dc_mem, params_ptr);
	pdDocTransfer(source_doc, DV_NULL, 0, 0);

	if (params_ptr == DV_NULL) goto end;
	if (!need_frames) goto end;
	if (!control_with_frames) goto end;

	//if (params_ptr->FlowsArr.Len == 1)
	//if (params_ptr->FlowsArr.Len >= 1)

	for (f = 0; f < params_ptr->FlowsArr.Len; f++)
	{
		DT_PD_FRAMES frames;
		frames.Format2Ptr = params_ptr->FlowsArr.Ptr[f].Frames.Format2Ptr;

		//DT_SLONG frames_len = params_ptr->FlowsArr.Ptr[f].FramesLen;
		//for (i = 0; i < frames_len; i++) if (frames.Format2Ptr[i].Idx >= 0) frames.Format2Ptr[i].Idx += fc_first_item;

		DT_SLONG parent_obj = control_with_frames->GetRenumeratedParentObj(params_ptr->FlowsArr.Ptr[f].ParentObj);

		//printf("--f=%ld, ParentFlow=%ld, ParentObj=%ld (%ld), Img=%ld, c->Img=%ld\n", f, params_ptr->FlowsArr.Ptr[f].ParentFlow, params_ptr->FlowsArr.Ptr[f].ParentObj, parent_obj, params_ptr->FlowsArr.Ptr[f].Img, c->Img);

		/* Associate frames of this subdocument with the correct image in the main document */

		//pdCanvasAddFrames(c, (*x) - c->XOff, (*y) - c->YOff, fc_first_item, parent_obj, c->Img * 1 /*f*/, 2, frames, params_ptr->FlowsArr.Ptr[f].FramesLen);
		//pdCanvasAddFrames(c, (*x) - c->XOff, (*y) - c->YOff, (f == 0 ? fc_first_item : 0), (f == 0 ? -1 : parent_obj), params_ptr->FlowsArr.Ptr[f].ParentFlow - f, params_ptr->FlowsArr.Ptr[f].ParentFrame, f /*params_ptr->FlowsArr.Ptr[f].LocalNumber*/, 2, frames, params_ptr->FlowsArr.Ptr[f].FramesLen);

		pdCanvasAddFramesXL(c, (*x) - c->XOff, (*y) - c->YOff, fc_first_item, parent_obj, f, 2, frames, params_ptr->FlowsArr.Ptr[f].FramesLen);
	}

	pdFlowsFree(&params_ptr->FlowsArr, params_ptr->Flows, -1);

end:

	return ret;
}
