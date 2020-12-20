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


#ifndef DTYPE_FORM_CONTROLS_H
#define DTYPE_FORM_CONTROLS_H


/* Properties */
enum
{
	fcConfigPtr = 10000,
	fcColumnNextPtr = 10011, fcNextColumnPtr = fcColumnNextPtr,
	fcControlPtr = 10029,
	fcProgramPtr = 10030,
	fcConnectionID = 10001,
	fcLockStatus = 10002,
	fcState = 10003,
	fcName = 10004,
	fcValue_08 = 10005, fcValue_8 = fcValue_08,
	fcValue_16 = 10012,
	fcValue_24 = 10013,
	fcValue_32 = 10014,
	fcDw = 10006, fcWidth = fcDw,
	fcDh = 10007, fcHeight = fcDh,
	fcUserParam = 10008,
	fcTabOrderID = 10009,
	fcGroupID = 10010,
	fcUserString = 10017,
	fcCnfLib = 10015,
	fcCnfCharBox = 10016,
	fcCnfCtrlStyleOnFocus = 10020,
	fcCnfCtrlShiftOnFocus = 10021,
	fcCnfButtonNormal = 10050,
	fcCnfButtonPushed = 10051,
	fcCnfRadioButtonNormal = 10060,
	fcCnfRadioButtonPushed = 10061,
	fcCnfRadioButtonSelected = 10062,
	fcCnfCheckBoxNormal = 10070,
	fcCnfCheckBoxPushed = 10071,
	fcCnfCheckBoxSelected = 10072,
	fcCnfSliderNormal = 10080,
	fcCnfSliderPushed = 10081,
	fcItemFirst = 10100, fcFirstItem = fcItemFirst, fcListFirstItem = fcItemFirst,
	fcListSelectedItems = 10101,
	fcListMultiSelect = 10102,
	fcListColOffset = 10103,
	fcAlign = 10104, fcListColAlign = fcAlign,
	fcAlignEnd = 10105,
	fcSliderLimit = 10110,
	fcSliderCoverage = 10111,
	fcSliderPosition = 10112,
	fcOnCtrlRelease = 10200,
	fcOnCtrlChange = 10201,
	fcOnCtrlScroll = 10202,
	fcOnCtrlPush = 10203,
	fcOnCtrlFocusReceive = 10204, fcOnCtrlReceiveFocus = fcOnCtrlFocusReceive,
	fcOnCtrlHold = 10205,
	fcOnCtrlFocusLose = 10206, fcOnCtrlLoseFocus = fcOnCtrlFocusLose,
	fcTextScrollType = 10207,
	fcTextWrapType = 10208,
	fcResizeGuideIndexHor = 10209,
	fcResizeGuideIndexVer = 10210,
	fcSliderFlags = 10211,
	fcRowFirst = 10212
};

/* Rendering Functions */
enum
{
	fcDrawButton = 10000, fcDrawFirst = fcDrawButton,
	fcDrawRadioButton = 10001,
	fcDrawCheckBox = 10002,
	fcDrawTextBox = 10003,
	fcDrawListBox = 10004,
	fcDrawHSlider = 10005,
	fcDrawVSlider = 10006,
	fcDrawRichTextBox = 10007,
	fcDrawRichButton = 10008,
	fcDrawRichListBox = 10009, fcDrawLast = fcDrawRichListBox
};


/* Button states */
#define FC_NORMAL 0
#define FC_PUSHED 1
#define FC_SELECTED 2
#define FC_NOTAPPLICABLE 255

/* Maximum number of form themes */
#define FC_NROFLIBS 3


typedef struct
{
	const DT_UBYTE* addr;
	DT_SLONG len;

} DT_VALUE_SUBSTRING;


/////////////////////////////////////////////////////////////////////////////
// CDTControl


class CDTControl
{
public:

	CDTControl() : ParentObj(-1), Obj(-1), Img(-1), Idx(0), Lib(0), Type(0), State(0), LockStatus(0), PushType(0), Reserved(0) {}
	virtual ~CDTControl() {}

public:

	void Reset(DT_UBYTE state)
	{
		ParentObj = Obj = -1;
		Img = Idx = -1;
		Lib = 0;
		Type = -1;
		State = state;
		LockStatus = 0;
		PushType = 0;
		Reserved = 0;
		BoxDoc.W = BoxDoc.H =0;
		MouseDoc.X = MouseDoc.Y = 0;
	}

public:

	DT_ID_SLONG ParentObj; // dependant on Idx; can be obtained as LastDraw[ctrl.Idx].parent_obj
	DT_ID_SLONG Obj; // dependant on Idx; can be obtained as LastDraw[ctrl.Idx].obj
	DT_SLONG Img; // dependant on Idx; can be obtained as LastDraw[ctrl.Idx].img
	DT_SLONG Idx;
	DT_SWORD Lib;
	DT_SWORD Type;
	DT_UBYTE State;
	DT_UBYTE LockStatus;
	DT_UBYTE PushType;
	DT_UBYTE Reserved;
	DT_PDX_WH_SLONG BoxDoc;
	DT_PDX_XY_SLONG MouseDoc;
};


/////////////////////////////////////////////////////////////////////////////
// CDTValue


class CDTValue
{
public:

	CDTValue() : Addr(DV_NULL), Len(0), Free(0), BufferFormat(TX_BUFFER_FORMAT_1) {}
	virtual ~CDTValue() { if (Free) pdFree(Addr); }

public:

	static void SortBuffer(DT_UBYTE* buff_addr, DT_SLONG buff_len, DT_UBYTE buff_format, bool descending = false, DT_SLONG nr_of_rows = -1);
	static DT_SLONG GetNrOfRows_FromBuffer(DT_UBYTE* buff_addr, DT_SLONG buff_len, DT_UBYTE buff_format, DT_VALUE_SUBSTRING substr_arr[], DT_SLONG arr_len);

	DT_SLONG ToString(DT_CHAR* str, DT_SLONG max_len) { DT_SLONG n = DF_MAX(0, Len); if (n >= max_len) n = max_len - 1; if (n > 0) dtMemCopy(str, Addr, n); str[n] = 0; return Len; }
	void ToTextProp(DT_PDDOC power_doc, DT_ID_SLONG obj) { ToPowerDocProp(power_doc, obj, pdTextString_08, pdTextString_16, pdTextString_24, pdTextString_32); }
	void ToValueProp(DT_PDDOC power_doc, DT_ID_SLONG obj) { ToPowerDocProp(power_doc, obj, fcValue_08, fcValue_16, fcValue_24, fcValue_32); }
	void ToRichText(DT_PDDOC power_doc, DT_ID_SLONG obj, bool single_text_fragment, const DT_STREAM_DESC* stream_fontmap, const DT_TX_DEFAULT_ATTRIBS* attribs = DV_NULL);

	const DT_UBYTE* MakeByPowerObj(DT_PDDOC power_doc, DT_ID_SLONG obj, bool* is_rich) { if (Free) { pdFree(Addr); Free = 0; } Addr = GetPowerDocProp(power_doc, obj, &Len, &BufferFormat, &Free, is_rich); return Addr; }
	const DT_UBYTE* MakeByCString(/*const*/ DT_CHAR* str) { if (Free) { pdFree(Addr); Free = 0; } Addr = reinterpret_cast<DT_UBYTE*>(str); Len = static_cast<DT_SLONG>(strlen(str)); BufferFormat = TX_BUFFER_FORMAT_1; return Addr; }
	const DT_UBYTE* MakeByBuffer(/*const*/ DT_UBYTE* addr, DT_SLONG len, DT_UBYTE buffer_format) { if (Free) { pdFree(Addr); Free = 0; } Addr = addr; Len = len; BufferFormat = buffer_format; return Addr; }

	void KillExceptRows(DT_SLONG row_index, DT_SLONG nr_of_rows);
	void SortRows(bool descending = false, DT_SLONG nr_of_rows = -1) { SortBuffer(Addr, Len, BufferFormat, descending, nr_of_rows); }

	DT_SLONG GetNrOfRows() const { return GetNrOfRows_FromBuffer(Addr, Len, BufferFormat, DV_NULL, 0); }
	DT_SLONG GetLen() const { return Len; }
	const DT_UBYTE* GetAddr() const { return Addr; }

	static DT_SWORD SortCompareFunction(const void* a1, const void* a2, DT_UBYTE buffer_format);
	static int SortCompareFunction_1(const void* a1, const void* a2) { return SortCompareFunction(a1, a2, TX_BUFFER_FORMAT_1); }
	static int SortCompareFunction_2(const void* a1, const void* a2) { return SortCompareFunction(a1, a2, TX_BUFFER_FORMAT_2); }
	static int SortCompareFunction_3(const void* a1, const void* a2) { return SortCompareFunction(a1, a2, TX_BUFFER_FORMAT_3); }
	static int SortCompareFunction_4(const void* a1, const void* a2) { return SortCompareFunction(a1, a2, TX_BUFFER_FORMAT_4); }

private:

	DT_ULONG KillBeforeRow(DT_SLONG row_index);

	void ToPowerDocProp(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name_08, DT_SLONG name_16, DT_SLONG name_24, DT_SLONG name_32);
	static DT_UBYTE* GetPowerDocProp(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG* len, DT_UBYTE* buffer_format, DT_UBYTE* must_free, bool* is_rich);

private:

	DT_UBYTE* Addr;
	DT_SLONG Len;
	DT_UBYTE Free;
	DT_UBYTE BufferFormat;
};


/////////////////////////////////////////////////////////////////////////////
// CDTListBoxColumn


class CDTListBoxColumn
{
public:

	CDTListBoxColumn() : Off(0), Alg(0) {}
	virtual ~CDTListBoxColumn() {}

public:

	DT_SLONG Off;
	DT_UBYTE Alg;
	CDTValue Val;
};


/////////////////////////////////////////////////////////////////////////////
// CDTPowerDocControl


class CDTPowerDocControl
{
public:

	CDTPowerDocControl() : PowerDoc(DV_NULL), UsesRichText(false) {}
	virtual ~CDTPowerDocControl() {}

public:

	DT_ID_SLONG GetRenumeratedParentObj(DT_ID_SLONG obj) const
	{
		if (!UsesRichText) return obj;
		if (obj < 0) return -1;

		DT_SLONG name = -1;
		const DT_CHAR* desc;
		pdObjGet(PowerDoc, obj, &name, &desc);
		return name;
	}

protected:

	static void ConfigProc(DT_PDDOC power_doc, bool focus, DT_SLONG w0, DT_SLONG h0, DT_SLONG w1, DT_SLONG h1, DT_SLONG w2, DT_SLONG h2, bool h2_is_infinite);

protected:

	DT_PDDOC PowerDoc;
	bool UsesRichText;
};


/////////////////////////////////////////////////////////////////////////////
// CDTButton


class CDTButton : public CDTPowerDocControl
{
public:

	CDTButton() : LastObj(-1), ObjText(0), ObjFragment(0) {}
	virtual ~CDTButton() {}

public:

	bool Load(DT_PDENGINE engine, const DT_CHAR* lib_path);
	DT_PDDOC Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_PDDOC power_doc, DT_ID_SLONG obj);
	void Unload();

private:

	DT_ID_SLONG LastObj;
	DT_ID_SLONG ObjText;
	DT_ID_SLONG ObjFragment;
	DT_PDX_XY_SLONG Size;
	DT_PDX_XY_SLONG Padding;
	DT_PDX_XY_SLONG Padding2;
	DT_PDX_XY_SLONG ValuePos[256];
	DT_SLONG ValueImg[256];
};


/////////////////////////////////////////////////////////////////////////////
// CDTSelectButton


class CDTSelectButton : public CDTPowerDocControl
{
public:

	CDTSelectButton() {}
	virtual ~CDTSelectButton() {}

public:

	bool Load(DT_PDENGINE engine, const DT_CHAR* lib_path);
	DT_PDDOC Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height) const;
	void Unload();

private:

	DT_PDX_XY_SLONG Size;
	DT_PDX_XY_SLONG Size2;
	DT_PDX_XY_SLONG Padding;
	DT_PDX_XY_SLONG ValuePos[256];
	DT_SLONG ValueImg[256];
};


/////////////////////////////////////////////////////////////////////////////
// CDTTextBox


class CDTTextBox : public CDTPowerDocControl
{
public:

	CDTTextBox() : BoxX(0), BoxY(0), LastImg(-1), LastObj(-1), ObjText(0), ObjFragment(0), FontH(0), DefaultAlign(0), DefaultAlignEnd(0)
	{
		//InvertCmd[0] = 1 | (3 << 3);
		InvertCmd[0] = 3 /*| (10 << 3)*/;
		InvertCmd[1] = InvertCmd[2] = InvertCmd[3] = 0;
	}

	virtual ~CDTTextBox() {}

public:

	void SetInvertCmd(DT_UBYTE cmd) { InvertCmd[0] = cmd; }

	void SelectedRows(DT_PDDOC power_doc, DT_SLONG fc_first_item, const DT_UBYTE* fc_selected_items, DT_SLONG nr_of_selected_items);
	void SelectedRows_Approx(DT_PDDOC power_doc, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_SLONG fc_first_item, const DT_UBYTE* fc_selected_items, DT_SLONG nr_of_selected_items);
	bool Load(DT_PDENGINE engine, const DT_CHAR* lib_path);
	DT_PDDOC Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_PDDOC power_doc, DT_ID_SLONG c_obj, DT_SLONG start_position, const DT_UBYTE* fc_selected_items, DT_SLONG nr_of_selected_items, DT_UBYTE wrap_type, DT_UBYTE row_type, bool h2_is_infinite, DT_SLONG fc_first_row);
	DT_SLONG GetNrOfVisibleRows(DT_SLONG fc_height) const;
	DT_SLONG GetItemByCoord(DT_SLONG first_item, DT_SLONG y, const CDTValue* fc_value) const;

	void GetBoxWH(DT_SLONG fc_width, DT_SLONG fc_height, DT_SLONG* box_w, DT_SLONG* box_h) const;
	DT_SLONG GetTextImg(DT_SLONG source_page) const;
	DT_ID_SLONG GetObjText() const { return ObjText; }
	void Unload();

protected:

	DT_SLONG BoxX;
	DT_SLONG BoxY;
	DT_SLONG LastImg;
	DT_ID_SLONG LastObj;
	DT_ID_SLONG ObjText;
	DT_ID_SLONG ObjFragment;
	DT_PDX_XY_SLONG Size;
	DT_PDX_XY_SLONG Padding;
	DT_PDX_XY_SLONG Padding2;
	DT_SLONG LineHImg[256];
	DT_SLONG LineVImg[256];
	DT_SLONG TextImg[256];
	DT_UBYTE InvertCmd[4];

public:

	DT_SLONG FontH;
	DT_UBYTE DefaultAlign, DefaultAlignEnd;
};


/////////////////////////////////////////////////////////////////////////////
// CDTListBox


class CDTListBox : public CDTPowerDocControl
{
public:

	CDTListBox() {}
	virtual ~CDTListBox() {}

public:

	void SetInvertCmd(DT_UBYTE cmd) { DTTextBox.SetInvertCmd(cmd); }

	bool Load(DT_PDENGINE engine, const DT_CHAR* lib_path);
	DT_PDDOC Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_PDDOC power_doc, DT_ID_SLONG c_obj, DT_SLONG fc_first_item, const DT_UBYTE* fc_selected_items, DT_SLONG nr_of_selected_items);
	DT_SLONG GetItemByCoord(DT_SLONG first_item, DT_SLONG y, const CDTValue* fc_value) const { return DTTextBox.GetItemByCoord(first_item, y, fc_value); }
	DT_SLONG GetNrOfVisibleRows(DT_SLONG fc_height) const { return DTTextBox.GetNrOfVisibleRows(fc_height); }
	DT_SLONG GetMaxFirstItem(DT_SLONG fc_height, CDTValue& fc_value) const;
	void Unload();

private:

	CDTTextBox DTTextBox;
};


/////////////////////////////////////////////////////////////////////////////
// CDTSlider


class CDTSlider : public CDTPowerDocControl
{
public:

	CDTSlider() : IsHorizontal(true) {}
	virtual ~CDTSlider() {}

public:

	bool Load(DT_PDENGINE engine, const DT_CHAR* lib_path, bool hor_ver_flag);
	DT_PDDOC Get(bool focus, DT_SLONG source_page, DT_SLONG fc_width, DT_SLONG fc_height, DT_FLOAT coverage_factor, DT_FLOAT position_factor) const;
	DT_SWORD GetDirection(DT_SLONG length, DT_FLOAT coverage_factor, DT_FLOAT position_factor, DT_SLONG position) const;
	static DT_SLONG GetInfoFromPowerDoc(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG* slider_limit, DT_UWORD* slider_coverage);
	void Unload();

private:

	bool IsHorizontal;

	DT_PDX_XY_SLONG Size;
	DT_PDX_XY_SLONG Size2;
	DT_PDX_XY_SLONG Padding;
	DT_SLONG ValueImg[256];
};



/////////////////////////////////////////////////////////////////////////////
// CDTFormControlsLib


class CDTFormControlsLib
{
public:

	CDTFormControlsLib(DT_PDENGINE engine)
	{
		DT_SWORD i;
		DT_CHAR path[128], file[256];

		IsInitialized = true;

		for (i = 0; i < FC_NROFLIBS; i++)
		{
			sprintf(path, "system/fc/lib_%c", 'a' + i);
			sprintf(file, "%s/sys_buttn.pdc", path); IsInitialized &= DTButton[i].Load(engine, file);
			sprintf(file, "%s/sys_radio.pdc", path); IsInitialized &= DTRadioButton[i].Load(engine, file);
			sprintf(file, "%s/sys_check.pdc", path); IsInitialized &= DTCheckBox[i].Load(engine, file);
			sprintf(file, "%s/sys_chrbx.pdc", path); IsInitialized &= DTTextBox[i].Load(engine, file);
			sprintf(file, "%s/sys_chrbx.pdc", path); IsInitialized &= DTListBox[i].Load(engine, file);
			sprintf(file, "%s/sys_slidr.pdc", path); IsInitialized &= DTHorSlider[i].Load(engine, file, true);
			sprintf(file, "%s/sys_slidr.pdc", path); IsInitialized &= DTVerSlider[i].Load(engine, file, false);
		}
	}

	virtual ~CDTFormControlsLib()
	{
		DT_SWORD i;

		for (i = 0; i < FC_NROFLIBS; i++)
		{
			DTButton[i].Unload();
			DTRadioButton[i].Unload();
			DTCheckBox[i].Unload();
			DTTextBox[i].Unload();
			DTListBox[i].Unload();
			DTHorSlider[i].Unload();
			DTVerSlider[i].Unload();
		}
	}

	static DT_SWORD GetLib(DT_SWORD lib)
	{
		if (lib < 0 || lib >= FC_NROFLIBS) return 0;
		return lib;
	}

	DT_SLONG UserObjDraw(DT_ID_SLONG obj_focus, DT_PD_DOCDRAW_CANVAS* c, DT_SLONG fn_draw, const DT_TM3X3 tm, bool need_frames, DT_SRAST_XL* x, DT_SRAST_XL* y);

	static DT_SWORD UserObjDrawStatic(DT_PD_DOCDRAW_CANVAS* c, DT_SLONG fn_draw, const DT_TM3X3 tm, DT_SWORD /*flag*/)
	{
		if (c == DV_NULL) return false;
		if (c->UserData == DV_NULL) return false;

		CDTFormControlsLib* form_controls_lib = reinterpret_cast<CDTFormControlsLib*>(c->UserData);
		DT_SRAST_XL x = 0;
		DT_SRAST_XL y = 0;
		bool need_frames = false;

		if (form_controls_lib->UserObjDraw(-1, c, fn_draw, tm, need_frames, &x, &y) == 0) return 1;

		return 1;
	}

	void SetExportFlags(DT_UWORD flags)
	{
		DT_UBYTE cmd = 3 /*| (10 << 3)*/; if (flags == 1) cmd = 1 | (3 << 3);
		DT_SWORD i;

		for (i = 0; i < FC_NROFLIBS; i++)
		{
			DTTextBox[i].SetInvertCmd(cmd);
			DTListBox[i].SetInvertCmd(cmd);
		}
	}

public:

	CDTButton DTButton[FC_NROFLIBS];
	CDTSelectButton DTRadioButton[FC_NROFLIBS];
	CDTSelectButton DTCheckBox[FC_NROFLIBS];
	CDTTextBox DTTextBox[FC_NROFLIBS];
	CDTListBox DTListBox[FC_NROFLIBS];
	CDTSlider DTHorSlider[FC_NROFLIBS];
	CDTSlider DTVerSlider[FC_NROFLIBS];

	bool IsInitialized;
};


#endif /* DTYPE_FORM_CONTROLS_H */

