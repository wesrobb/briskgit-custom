DT_SLONG CDTBuilderText::TextReflow_Internal()
{
	return txTextReflow(Doc->GetTxDoc(), 0);
}


bool CDTBuilderText::TextFlow_Internal(const CDTAttribs& a, DT_ID_SWORD font_index, DT_SWORD unicode_flags)
{
	Img1 = Img0 + a.AColumnCount - 1;

	DT_SLONG n;
	DT_SLONG obj_text = -1;
	DT_SLONG font_w = a.AFontW;
	DT_SLONG font_h = a.AFontH;
	//DT_SLONG font_h = a.ASpacing * a.AFontH / 1024;

	DT_STRING_CHARS(font_w_str, 32, ""); dtStrConcatSLong(&font_w_str, font_w);
	DT_STRING_CHARS(font_h_str, 32, ""); dtStrConcatSLong(&font_h_str, font_h);
	DT_STRING_CHARS(font_index_str, 32, ""); dtStrConcatSLong(&font_index_str, font_index);
	DT_STRING_CHARS(spacing_factor_str, 32, ""); dtStrConcatSLong(&spacing_factor_str, a.ASpacing);
	DT_STRING_CHARS(align_str, 32, ""); dtStrConcatSLong(&align_str, a.AAlign);
	DT_STRING_CHARS(align_end_str, 32, ""); dtStrConcatSLong(&align_end_str, a.AAlignEnd);

	const DT_CHAR* font_index_ptr = font_index_str.str;
	if (a.AStrUTF8) font_index_ptr = DV_NULL;

	DT_STREAM_MEMORY(text_sd_m, reinterpret_cast<const DT_UBYTE*>(a.AStrUTF8), (a.AStrUTF8 ? strlen(a.AStrUTF8) : 0));

	DT_TX_DEFAULT_ATTRIBS attribs = {"00000000", font_w_str.str, font_h_str.str, "0", "0", "0", "0", align_str.str, align_end_str.str, "0", font_index_ptr, spacing_factor_str.str};
	DT_TXDOC text_doc = DV_NULL;
	DT_PDDOC powerdoc = Doc->GetPdDoc();

	obj_text = txTextIniViaPowerDocAndStream(&text_doc, powerdoc, TX_UTF8, &text_sd_m, unicode_flags, DV_NULL, &attribs, GetFontmap(), 0);
	if (text_doc == DV_NULL) obj_text = -1; else txTextExt(text_doc);

	n = pdxImgMakeTextFlow(powerdoc, 0, static_cast<DT_UBYTE>(a.AAreaType | 128), static_cast<DT_UBYTE>(a.ARowType), static_cast<DT_UBYTE>(a.AVAlign), static_cast<DT_UBYTE>(a.AVAlignEnd), static_cast<DT_UBYTE>(a.ALayoutType), a.APage, a.APageCount, a.AColumnCount, a.AColumnDist, a.AX, a.AY, a.AW, a.AH, obj_text);

	TxCmd_Internal(Img0, -1, TX_CMD_ENDTEXT);

	bool text_reflow = a.AReflow; //true;
	if (Doc->DoTextReflow && text_reflow) txTextReflow(Doc->GetTxDoc(), 0);

	if (n <= 0) return false;

	W = a.AW;
	H = a.AH;

	return true;
}


CDTBuilderText& CDTBuilderText::Text_Existing(DT_SLONG img)
{
	Img1 = Img0 = img;
	TxCmd_Internal(Img0, -1, TX_CMD_ENDTEXT);
	W = H = 100; /* for now */
	return *this;
}


CDTBuilderText& CDTBuilderText::Text_Internal(const CDTAttribs& a)
{
	if (a.AExistingImg >= 0) return Text_Existing(a.AExistingImg);

	DT_SWORD unicode_flags = TX_IMPORT_BASIC;
	/*if (true bBidi)*/ unicode_flags |= TX_IMPORT_UNICODE_BIDI;
	if (Doc->HasTextCompression())
	{
		/*if (true bOptimize)*/ unicode_flags |= TX_IMPORT_UNICODE_OPTIMIZE;
		/*if (true bCompress)*/ unicode_flags |= TX_IMPORT_UNICODE_COMPRESS;
	}
	/*if (true bScript)*/ unicode_flags |= TX_IMPORT_UNICODE_SCRIPT;
	/*if (false bCJKVAlt) unicode_flags |= TX_IMPORT_OPENTYPE_CJK_VALT;*/

	//DT_UBYTE text_layout_id = 0;
	//DT_UBYTE text_area_type = 0; if (true /*bEmptyLines*/) text_area_type |= 128;

	TextFlow_Internal(a, 0, unicode_flags);

	return *this;
}
