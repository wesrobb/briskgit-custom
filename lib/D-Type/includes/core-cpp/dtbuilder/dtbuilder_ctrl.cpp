/* Symbols */

DT_STR_UTF8 CDTBuilderCtrl::Nil = "\x08";
DT_STR_UTF8 CDTBuilderCtrl::NewCol = "\x0C";
DT_STR_UTF8 CDTBuilderCtrl::NewLine = "\x0A";


bool CDTBuilderCtrl::Refresh_Internal(DT_SLONG img, DT_ID_SLONG parent_obj, DT_ID_SWORD command, DT_SLONG param0 /* = 0*/)
{
	//DT_ID_SWORD command = 0;
	DT_PD_NEST_CONFIG config;

	config.InternalPage = DV_PAGE_NEST;
	config.Params[0] = param0; config.Params[1] = 0; config.Params[2] = 0; config.Params[3] = 0;
	config.FontW = DV_NESTFONTW;
	config.FontH = DV_NESTFONTH;
	config.FontIndex = DV_NESTFONTINDEX;
	config.MiscParam = 0; /* unused */
	config.RGBT[0] = 0; config.RGBT[1] = 0; config.RGBT[2] = 0; config.RGBT[3] = 0;
	config.RGBT_Flag = 0;
	config.Reserved1 = 1; /* Bit 0: disable word TEXT in new table cells */
	config.Reserved2 = 0; /* unused */
	//config.PropName_OriginLocation = pdxOriginLocation;
	//config.PropName_VAlignCell = pdxVAlignCell;
	//config.PropName_VAlignRow = pdxVAlignRow;
	//config.PropName_Nw = pdxNw;
	//config.PropName_Nh = pdxNh;
	config.PropName_Dw = 0; //fcDw;
	config.PropName_Dh = 0; //fcDh;

	DT_SWORD res = pdNestRefresh(Doc->GetPdDoc(), img, parent_obj, command, 0, &config);

	return (res == 1);
}


void CDTBuilderCtrl::Put_Internal1(DT_BUILDREF b, DT_ID_SLONG parent_obj)
{
	if (b.IsNil())
	{
		AddRun_Internal(CDTBuilderUTF8(""), Img0, parent_obj, TX_CMD_SELECTFLOW);

		/* kill Nil cell */
		DT_ID_SLONG obj = Doc->LinkGet(parent_obj, pdFnDrawObjectPtr); if (obj < 0) return;
		obj = Doc->LinkGet(obj, pdTextFragmentPtr); if (obj < 0) return;
		obj = Doc->LinkGet(obj, pdTypographyPtr); if (obj < 0) return;
		Doc->PropApplyAsSLong(obj, pdFontDw, 0);
		Doc->PropApplyAsSLong(obj, pdFontDh, 0);
	}
	else AddRun_Internal(b, Img0, parent_obj, TX_CMD_SELECTFLOW);

	Refresh_Internal(Img0, parent_obj, 0);
}


DT_ID_SLONG CDTBuilderCtrl::Put_Internal2(DT_ID_SLONG obj, DT_BUILDREF b, DT_SLONG i, bool put /* = true */)
{
	DT_SLONG j = -1;

	while ((obj = Doc->LinkGet(obj, pdGroupMemberPtr)) >= 0)
	{
		DT_SWORD fn_draw = Doc->PropGetAsSWord(obj, pdFnDraw, pdDrawDefault);
		if (fn_draw != pdDrawRichTextArea && fn_draw != pdDrawRichTextLine) continue;

		//printf("obj=%ld, fn_draw=%d, i=%ld, j=%ld\n", obj, fn_draw, i, j);

		j++; if (i >= 0 && i != j) continue;
		if (put) { Put_Internal1(b, obj); LastPutCell = j; }
		if (i >= 0) return obj;
	}

	return -1;
}


DT_ID_SLONG CDTBuilderCtrl::Head_Internal(const DT_UBYTE symbol_rgbt[] /* = DV_NULL */, DT_SWORD magnitude /* = -1 */)
{
	//DT_ID_SLONG obj = Doc->ObjFindByNameAndDesc(981000, DV_NULL); if (obj < 0) return -1;
	DT_ID_SLONG obj = Doc->ImgGetObj(Img0); if (obj < 0) return -1;
	obj = Doc->LinkGet(obj, pdTextFragmentPtr); if (obj < 0) return -1;
	obj = Doc->LinkGet(obj, pdTypographyPtr); if (obj < 0) return -1;
	obj = Doc->LinkGet(obj, pdPowerFontPtr); if (obj < 0) return -1;
	DT_ID_SLONG obj_powerfont = obj;
	obj = Doc->LinkGet(obj, pdPowerFontRecordPtr); if (obj < 0) return -1;
	DT_SWORD glyph_attribs = Doc->PropGetAsSWord(obj, pdPowerGlyphAttribs, 0);
	obj = Doc->LinkGet(obj, pdFnDrawObjectPtr); if (obj < 0) return -1;

	if (glyph_attribs & 16) /* bracketed matrices are set up this way */
	{
		obj = Doc->LinkGet(obj, pdGroupMemberPtr); if (obj < 0) return -1;
		obj = Doc->LinkGet(obj, pdFnDrawObjectPtr); if (obj < 0) return -1;
		obj = Doc->LinkGet(obj, pdTextFragmentPtr); if (obj < 0) return -1;
		obj = Doc->LinkGet(obj, pdTypographyPtr); if (obj < 0) return -1;
		obj = Doc->LinkGet(obj, pdPowerFontPtr); if (obj < 0) return -1;
		obj_powerfont = obj;
		obj = Doc->LinkGet(obj, pdPowerFontRecordPtr); if (obj < 0) return -1;
		obj = Doc->LinkGet(obj, pdFnDrawObjectPtr); if (obj < 0) return -1;
	}

	DT_ID_SLONG obj_head = obj;

	if (magnitude >= 0) Doc->PropAddAsSWord(obj_powerfont, pdxMagnitude, magnitude);

	if (symbol_rgbt == DV_NULL) return obj_head;

	while ((obj = Doc->LinkGet(obj, pdGroupMemberPtr)) >= 0)
	{
		DT_ID_SLONG o = Doc->LinkGet(obj, pdFnDrawObjectPtr); if (o < 0) continue;
		DT_SWORD fn_draw = Doc->PropGetAsSWord(obj, pdFnDraw, pdDrawDefault);
		//if (fn_draw != pdDrawChar && fn_draw != pdDrawLine && fn_draw != pdDrawPoly) continue;

		if (fn_draw == pdDrawChar)
		{
			o = Doc->LinkGet(o, pdTypographyPtr); if (o < 0) continue;
			o = Doc->LinkGet(o, pdBodyPtr);
		}
		else if (fn_draw == pdDrawLine)
		{
			o = Doc->LinkGet(o, pdLinePtr); if (o < 0) continue;
			o = Doc->LinkGet(o, pdStylePtr);
		}
		else if (fn_draw == pdDrawPoly || fn_draw == pdDrawLibraryPoly)
		{
			o = Doc->LinkGet(o, pdBodyPtr);
		}
		else continue;

		if (o >= 0) Doc->PropSet(o, pdRGBT, symbol_rgbt, 4);
		return obj_head;
	}

	return obj_head;
}


bool CDTBuilderCtrl::Make_Internal(const CDTAttribsBase& a, DT_PDDOC doc, bool clear)
{
	DT_SLONG page = a.APage;
	DT_SLONG x = a.AX;
	DT_SLONG y = a.AY;

	LastPutCell = -1; Col = -1; ColMax = 0; Row = 0; RowMax = 0; Flags = DV_BUILDER_FLAG_EXBASIC;

	//Del_Internal(); /* destroy name so we can add more objects with the same names to the same document later */

	if (Doc->Append(doc, page, 0, 0) != 1) return false;
	if (Doc->ImgGetLast() == Img0 + 1) Doc->ImgDel(Img0 + 1); /* remove background rectangle */

	Doc->ImgSetXY(Img0, x, y);

	// Set font size
	DT_ID_SLONG obj = Doc->ImgGetObj(Img0); if (obj < 0) goto end;
	obj = Doc->LinkGet(obj, pdTextFragmentPtr); if (obj < 0) goto end;
	obj = Doc->LinkGet(obj, pdTypographyPtr); if (obj < 0) goto end;
	Doc->PropApplyAsSLong(obj, pdFontDw, a.AFontW);
	Doc->PropApplyAsSLong(obj, pdFontDh, a.AFontH);

end:

	if (clear) Put_Internal3(CDTBuilderUTF8(""), -1); /* clear all operands */

	return true;
}


CDTBuilderCtrl& CDTBuilderCtrl::Base_Internal(const CDTAttribs& a, DT_BUILDREF b0)
{
	Make_Internal(a, (a.ADirection == DE_DIR_LTR ? Doc->GetClip(Clip_992001_c_01_ltr) : Doc->GetClip(Clip_992001_c_01_rtl)), false);

	Flags |= DV_BUILDER_FLAG_EXRULER;
	//Flags = DV_BUILDER_FLAG_KEEPALL;

	//DT_SLONG obj1 = Doc->ObjFindByNameAndDesc(981001, DV_NULL); if (obj1 >= 0) Doc->PropApplyAsUWord(obj1, pdIndentLeft, indent);
	DT_ID_SLONG obj = Doc->ImgGetObj(Img0); if (obj < 0) goto end;
	obj = Doc->LinkGet(obj, pdTextFragmentPtr); if (obj < 0) goto end;
	obj = Doc->LinkGet(obj, pdTextRulerHorPtr); if (obj < 0) goto end;

	Doc->PropApplyAsSLong(obj, pdMarginLeft, a.AMarginL);
	Doc->PropApplyAsUWord(obj, pdIndentLeft, a.AIndentL);
	Doc->PropApplyAsSLong(obj, pdMarginRight, a.AMarginR);
	Doc->PropApplyAsUWord(obj, pdIndentRight, a.AIndentR);

	/*
	Doc->PropApplyAsSLong(obj, pdMarginLeft, a.Direction == DE_DIR_LTR ? a.AMarginL : a.AMarginR);
	Doc->PropApplyAsUWord(obj, pdIndentLeft, a.Direction == DE_DIR_LTR ? a.AIndentL : a.AIndentR);
	Doc->PropApplyAsSLong(obj, pdMarginRight, a.Direction == DE_DIR_LTR ? a.AMarginR : a.AMarginL);
	Doc->PropApplyAsUWord(obj, pdIndentRight, a.Direction == DE_DIR_LTR ? a.AIndentR : a.AIndentL);
	*/

end:

	Put_Internal3(b0, 0);

	Format(a.ADirection == DE_DIR_LTR ? attrRightFull : attrLeftFull);

	return *this;
}


CDTBuilderCtrl& CDTBuilderCtrl::Rule_Internal(const CDTAttribs& a)
{
	if (a.ARuleType == DE_RULE_SOLIDLINE) Make_Internal(a, Doc->GetClip(Clip_rule), false);
	else                                  Make_Internal(a, Doc->GetClip(Clip_empty), false);

	DT_ID_SLONG obj = Doc->ImgGetObj(Img0); if (obj < 0) goto end;
	obj = Doc->LinkGet(obj, pdTextFragmentPtr); if (obj < 0) goto end;
	obj = Doc->LinkGet(obj, pdTextRulerHorPtr); if (obj < 0) goto end;

	Doc->PropApplyAsSLong(obj, pdMarginLeft, a.AMarginL);
	Doc->PropApplyAsUWord(obj, pdIndentLeft, a.AIndentL);
	Doc->PropApplyAsSLong(obj, pdMarginRight, a.AMarginR);
	Doc->PropApplyAsUWord(obj, pdIndentRight, a.AIndentR);

	/*
	Doc->PropApplyAsSLong(obj, pdMarginLeft, ltr ? a.AMarginL : a.AMarginR);
	Doc->PropApplyAsUWord(obj, pdIndentLeft, ltr ? a.AIndentL : a.AIndentR);
	Doc->PropApplyAsSLong(obj, pdMarginRight, ltr ? a.AMarginR : a.AMarginL);
	Doc->PropApplyAsUWord(obj, pdIndentRight, ltr ? a.AIndentR : a.AIndentL);
	*/

end:

	Flags |= DV_BUILDER_FLAG_NEWLINE | DV_BUILDER_FLAG_EXRULER;

	return *this;
}


CDTBuilderCtrl& CDTBuilderCtrl::Table_Internal(const CDTAttribs& a)
{
	     if (a.ATablePreset == DE_TABLE_HVHEAD) { Make_Internal(a, Doc->GetClip(Clip_990032_a_02_02), true); RowMax = 2; ColMax = 2; }
	else if (a.ATablePreset == DE_TABLE_HHEAD)  { Make_Internal(a, Doc->GetClip(Clip_990032_a_02_01), true); RowMax = 2; ColMax = 1; }
	else if (a.ATablePreset == DE_TABLE_VHEAD)  { Make_Internal(a, Doc->GetClip(Clip_990032_a_01_02), true); RowMax = 1; ColMax = 2; }
	else if (a.ATablePreset == DE_TABLE_NOHEAD) { Make_Internal(a, Doc->GetClip(Clip_990032_a_01_01), true); RowMax = 1; ColMax = 1; }

	else return *this;

	return *this;
}


/*
void CDTBuilderCtrl::Del_Internal()
{
	DT_ID_SLONG obj0 = Doc->ObjFindByNameAndDesc(981000, DV_NULL); if (obj0 >= 0) Doc->ObjSet(obj0, 0, DV_NULL);
	DT_ID_SLONG obj1 = Doc->ObjFindByNameAndDesc(981001, DV_NULL); if (obj1 >= 0) Doc->ObjSet(obj1, 0, DV_NULL);
}
*/
