DT_SLONG GlobalFlowIndex = 0;

enum EDTLayoutType
{
	DE_LAYOUT_FIRST = 0,
	DE_LAYOUT_DEVICE_INDEPENDENT_FRAC = 0,
	DE_LAYOUT_DEVICE_INDEPENDENT_INT = 1,
	DE_LAYOUT_DEVICE_DEPENDENT_1 = 2,
	DE_LAYOUT_DEVICE_DEPENDENT_2 = 3,
	DE_LAYOUT_MIXED_1 = 4,
	DE_LAYOUT_MIXED_2 = 5,
	DE_LAYOUT_LAST = 5
};

enum EDTAlignType
{
	DE_ALIGN_FIRST = 0,
	DE_ALIGN_LEFT = 0,
	DE_ALIGN_CENTER = 1,
	DE_ALIGN_RIGHT = 2,
	DE_ALIGN_JUSTIFY_A_LEFT = 3,
	DE_ALIGN_JUSTIFY_B_LEFT = 4,
	DE_ALIGN_JUSTIFY_A_CENTER = 5,
	DE_ALIGN_JUSTIFY_B_CENTER = 6,
	DE_ALIGN_JUSTIFY_A_RIGHT = 7,
	DE_ALIGN_JUSTIFY_B_RIGHT = 8,
	DE_ALIGN_JUSTIFY_B_JUSTIFY_A_LEFT = 9,
	DE_ALIGN_JUSTIFY_B_JUSTIFY_A_CENTER = 10,
	DE_ALIGN_JUSTIFY_B_JUSTIFY_A_RIGHT = 11,
	DE_ALIGN_LAST = 11
};

enum EDTValignType
{
	DE_VALIGN_FIRST = 0,
	DE_VALIGN_TOP = 0,
	DE_VALIGN_MIDDLE = 1,
	DE_VALIGN_BOTTOM = 2,
	DE_VALIGN_JUSTIFIED = 3,
	DE_VALIGN_LAST = 3
};

enum EDTAreaType
{
	DE_AREA_FIRST = 0,
	DE_AREA_RECT = 0,
	DE_AREA_QUARTER_CIRCULAR_A = 10,
	DE_AREA_QUARTER_CIRCULAR_B = 11,
	DE_AREA_QUARTER_CIRCULAR_C = 12,
	DE_AREA_QUARTER_CIRCULAR_D = 13,
	DE_AREA_HALF_CIRCULAR_A = 14,
	DE_AREA_HALF_CIRCULAR_B = 15,
	DE_AREA_HALF_CIRCULAR_C = 16,
	DE_AREA_HALF_CIRCULAR_D = 17,
	DE_AREA_CIRCULAR = 18,
	DE_AREA_QUARTER_DIAMOND_A = 20,
	DE_AREA_QUARTER_DIAMOND_B = 21,
	DE_AREA_QUARTER_DIAMOND_C = 22,
	DE_AREA_QUARTER_DIAMOND_D = 23,
	DE_AREA_HALF_DIAMOND_A = 24,
	DE_AREA_HALF_DIAMOND_B = 25,
	DE_AREA_HALF_DIAMOND_C = 26,
	DE_AREA_HALF_DIAMOND_D = 27,
	DE_AREA_DIAMOND = 28,
	DE_AREA_LAST = 28
};

enum EDTRowType
{
	DE_ROW_FIRST = 0,
	DE_ROW_MATH = 0,
	DE_ROW_TYPO = 10,
	DE_ROW_TYPO_LINEGAP = 20,
	DE_ROW_STANDARD = 20,
	DE_ROW_WIN = 30,
	DE_ROW_TYPO_CJK = 110,
	DE_ROW_TYPO_LINEGAP_CJK = 120,
	DE_ROW_LAST = 120
};


class CDTBuilderText : public CDTBuilderUTF8
{
public:

	class CDTAttribs : public CDTAttribsBase
	{
	public:

		CDTAttribs() : AStrUTF8(DV_NULL), AExistingImg(-1), ASpacing(1024), AColumnDist(100), AColumnCount(1), APageCount(1), ALayoutType(DE_LAYOUT_MIXED_2), AAreaType(DE_AREA_RECT), ARowType(DE_ROW_STANDARD), AAlign(DE_ALIGN_LEFT), AAlignEnd(DE_ALIGN_LEFT), AVAlign(DE_VALIGN_TOP), AVAlignEnd(DE_VALIGN_TOP), AReflow(true), AdjustmentsLen(0) { AW = AH = 0; }
		virtual ~CDTAttribs() { StrFree(AStrUTF8); }

		DEF_ATTRIBS_BASE_METHODS

		inline CDTAttribs& StrUTF8(const DT_CHAR* str_utf8) { StrCopy(AStrUTF8, str_utf8); return *this; }

		inline CDTAttribs& ExistingImg(DT_SLONG existing_img) { AExistingImg = existing_img; return *this; }
		inline CDTAttribs& Spacing(DT_SLONG spacing) { ASpacing = spacing; return *this; }
		inline CDTAttribs& ColumnDist(DT_SLONG column_dist) { AColumnDist = column_dist; return *this; }
		inline CDTAttribs& ColumnCount(DT_UWORD column_count) { if (column_count < 1) column_count = 1; AColumnCount = column_count; return *this; }
		inline CDTAttribs& PageCount(DT_UWORD page_count) { if (page_count < 1) page_count = 1; APageCount = page_count; return *this; }
		inline CDTAttribs& LayoutType(EDTLayoutType layout_type) { ALayoutType = DF_MIN(DF_MAX(DE_LAYOUT_FIRST, layout_type), DE_LAYOUT_LAST); return *this; }
		inline CDTAttribs& AreaType(EDTAreaType area_type) { AAreaType = DF_MIN(DF_MAX(DE_AREA_FIRST, area_type), DE_AREA_LAST); return *this; }
		inline CDTAttribs& RowType(EDTRowType row_type) { ARowType = DF_MIN(DF_MAX(DE_ROW_FIRST, row_type), DE_ROW_LAST); return *this; }
		inline CDTAttribs& Align(EDTAlignType align_type) { AAlign = DF_MIN(DF_MAX(DE_ALIGN_FIRST, align_type), DE_ALIGN_LAST); return *this; }
		inline CDTAttribs& AlignEnd(EDTAlignType align_end_type) { AAlignEnd = DF_MIN(DF_MAX(DE_ALIGN_FIRST, align_end_type), DE_ALIGN_LAST); return *this; }
		inline CDTAttribs& VAlign(EDTValignType valign_type) { AVAlign = DF_MIN(DF_MAX(DE_VALIGN_FIRST, valign_type), DE_VALIGN_LAST); return *this; }
		inline CDTAttribs& VAlignEnd(EDTValignType valign_end_type) { AVAlignEnd = DF_MIN(DF_MAX(DE_VALIGN_FIRST, valign_end_type), DE_VALIGN_LAST); return *this; }
		inline CDTAttribs& Reflow(bool reflow) { AReflow = reflow; return *this; }

		DT_BUILDER_STR AStrUTF8;
		DT_SLONG AExistingImg;
		DT_SLONG ASpacing;
		DT_SLONG AColumnDist;
		DT_UWORD AColumnCount;
		DT_UWORD APageCount;
		EDTLayoutType ALayoutType;
		EDTAreaType AAreaType;
		EDTRowType ARowType;
		EDTAlignType AAlign;
		EDTAlignType AAlignEnd;
		EDTValignType AVAlign;
		EDTValignType AVAlignEnd;

		bool AReflow;

		struct DT_ADJUSTMENT
		{
			DT_SLONG Dx;
			DT_SLONG Dy;
			DT_SLONG Img1;
			DT_SLONG Img2;
			DT_UBYTE Flags;
			DT_UBYTE Reserved;
		};

		inline DT_SWORD GetAdjustmentsLen() const { return AdjustmentsLen; }
		inline const DT_ADJUSTMENT* GetAdjustment(DT_SWORD i) const { if (i < 0 || i >= 16) i = 0; return AdjustmentsArr + i; }

		CDTAttribs& Adjust(DT_UBYTE flags, DT_SLONG dx, DT_SLONG dy, DT_SLONG img_idx, DT_SLONG img_n = 1)
		{
			if (AdjustmentsLen >= 16) return *this;
			DT_ADJUSTMENT a = {dx, dy, img_idx, img_n, flags, 0};
			AdjustmentsArr[AdjustmentsLen] = a; AdjustmentsLen++;
			return *this;
		}

		inline CDTAttribs& ShiftHor(DT_SLONG dx, DT_SLONG img_idx, DT_SLONG img_n = 1) { return Adjust(2, dx, 0, img_idx, img_n); }
		inline CDTAttribs& ShiftVer(DT_SLONG dy, DT_SLONG img_idx, DT_SLONG img_n = 1) { return Adjust(8, 0, dy, img_idx, img_n); }
		inline CDTAttribs& ResizeFromRight(DT_SLONG dx, DT_SLONG img_idx, DT_SLONG img_n = 1) { return Adjust(1, dx, 0, img_idx, img_n); }
		inline CDTAttribs& ResizeFromLeft(DT_SLONG dx, DT_SLONG img_idx, DT_SLONG img_n = 1) { return Adjust(3, dx, 0, img_idx, img_n); }
		inline CDTAttribs& ResizeFromBottom(DT_SLONG dy, DT_SLONG img_idx, DT_SLONG img_n = 1) { return Adjust(4, 0, dy, img_idx, img_n); }
		inline CDTAttribs& ResizeFromTop(DT_SLONG dy, DT_SLONG img_idx, DT_SLONG img_n = 1) { return Adjust(12, 0, dy, img_idx, img_n); }

	private:

		DT_SWORD AdjustmentsLen;
		DT_ADJUSTMENT AdjustmentsArr[16];
	};

private: // private copy constructor and assignment operator

	CDTBuilderText(const CDTBuilderText&);
	CDTBuilderText& operator = (const CDTBuilderText&);

public:

	CDTBuilderText(CDTDocV8* doc) : CDTBuilderUTF8(false, doc), W(0), H(0) { Img1 = Img0; Flags = DV_BUILDER_FLAG_KEEPALL; }
	CDTBuilderText(CDTDocV8& doc) : CDTBuilderUTF8(false, doc), W(0), H(0) { Img1 = Img0; Flags = DV_BUILDER_FLAG_KEEPALL; }
	//CDTBuilderText(CDTDocV8* doc, const CDTAttribs& a) : CDTBuilder(doc), W(0), H(0), Img1(0) { Text(a); } /* is this used? */
	virtual ~CDTBuilderText() {}

	CDTBuilderText* New() const { CDTBuilderText* n = new CDTBuilderText(Doc); Doc->AddAtom(n); return n; }

public:

	inline CDTBuilderText& Text_Basic(const CDTAttribs& a = CDTAttribs())
	{
		::GlobalFlowIndex++;
		CDTBuilderText* b = New(); b->Text_Internal(a); b->SetFlowIndex(GlobalFlowIndex);
		Flags = DV_BUILDER_FLAG_KEEPALL;
		return *b;
	}

	inline CDTBuilderText& Text(const CDTAttribs& a = CDTAttribs())
	{
		DT_SLONG img0 = Doc->ImgGetLast() + 1;

		CDTBuilderText& ret = Text_Basic(a);

		DT_SWORD l = a.GetAdjustmentsLen();

		for (DT_SWORD i = 0; i < l; i++)
		{
			const CDTAttribs::DT_ADJUSTMENT* adjust = a.GetAdjustment(i);

			DT_UBYTE flags = adjust->Flags;
			DT_SLONG dx = adjust->Dx;
			DT_SLONG dy = adjust->Dy;
			DT_SLONG img1 = img0 + adjust->Img1;
			DT_SLONG img2 = img0 + adjust->Img2;

			for (DT_SLONG img = img1; img < img2; img++)
			{
				DT_ID_SLONG o = Doc->ImgGetObj(img);
				DT_SLONG x = Doc->ImgGetX(img);
				DT_SLONG y = Doc->ImgGetY(img);
				DT_SLONG w = Doc->PropGetAsSLong(o, pdTextAreaDw);
				DT_SLONG h = Doc->PropGetAsSLong(o, pdTextAreaDh);

				if (flags & 1) Doc->PropApplyAsSLong(o, pdTextAreaDw, w - dx);
				if (flags & 2) Doc->ImgSetX(img, x + dx);
				if (flags & 4) Doc->PropApplyAsSLong(o, pdTextAreaDh, h - dy);
				if (flags & 8) Doc->ImgSetY(img, y + dy);
			}
		}

		return ret;
	}

	//inline CDTBuilderText& Text(DT_STR_UTF8 str_utf8, DT_SLONG font_w = DV_FONTW, DT_SLONG font_h = DV_FONTH, DT_SLONG page = DV_PAGE_TEMP, DT_SLONG x = 0, DT_SLONG y = 0) { return Text(0, 0, str_utf8, font_w, font_h, page, x, y); }

	inline CDTBuilderText& ToObj(DT_SLONG font_w = DV_FONTW, DT_SLONG font_h = DV_FONTH)
	{
		CDTBuilder::ToObj(Img0, Img1, 0, H, W, H, font_w, font_h, font_w, font_h, W, 0);
		//CDTBuilder::ToObj(Img0, Img1, 0, H, W, H, W, H, W, H, W, 0);
		return *this;
	}

	inline CDTBuilderText& Format(const DT_TX_ATTRIBS text_attribs[], const DT_CHAR* extra = DV_NULL, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { CDTBuilder::Format(text_attribs, extra, first_char, last_char); return *this; }
	inline CDTBuilderText& Format(const CDTFormatter& formatter, const DT_CHAR* extra = DV_NULL, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { CDTBuilder::Format(formatter, extra, first_char, last_char); return *this; }
	inline CDTBuilderText& Format(const CDTFormatArr& formatarr, const DT_CHAR* value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { CDTBuilder::Format(formatarr, value, first_char, last_char); return *this; }
	//inline CDTBuilderText& Format(const CDTFormatter& formatter, DT_SLONG value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { CDTBuilder::Format(formatter, value, first_char, last_char); return *this; }
	inline CDTBuilderText& Format(const CDTFormatArr& formatarr, DT_SLONG value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { CDTBuilder::Format(formatarr, value, first_char, last_char); return *this; }
	inline CDTBuilderText& Format(DT_ID_SLONG id, const DT_CHAR* value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { CDTBuilder::Format(id, value, first_char, last_char); return *this; }
	inline CDTBuilderText& Format(DT_ID_SLONG id, DT_SLONG value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { CDTBuilder::Format(id, value, first_char, last_char); return *this; }
	inline CDTBuilderText& Format_FontSize(DT_SLONG font_w, DT_SLONG font_h = 0, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { CDTBuilder::Format_FontSize(font_w, font_h, first_char, last_char); return *this; }

	inline CDTBuilderText& Reflow() { TextReflow_Internal(); return *this; }

	//inline CDTBuilderText& F(const DT_TX_ATTRIBS text_attribs[], CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { return Format(text_attribs, first_char, last_char); }

	inline CDTBuilderText& operator + (DT_BUILDREF b) { AddRun(b); return *this; }

	inline CDTBuilderText& operator + (CDTMarker& m) { AddMarker(m); return *this; }

	inline CDTBuilderText& operator ~ () { ToObj(); return *this; }

	inline CDTBuilderText::CDTAttribs operator () () { return CDTAttribs(); }

	//inline CDTBuilderText& Test(DT_STR_UTF8 s) { return Text(100, 100, s); }
	//inline CDTBuilderText& Test(DT_STR_UTF8 s) { return Text(CDTAttribs().W(100).H(100).StrUTF8(s)); }

private:

	CDTBuilderText& Text_Existing(DT_SLONG img);

protected:

	CDTBuilderText& Text_Internal(const CDTAttribs& a);
	bool TextFlow_Internal(const CDTAttribs& a, DT_ID_SWORD font_index, DT_SWORD unicode_flags);
	DT_SLONG TextReflow_Internal();

private:

	DT_SLONG W, H;
	DT_SLONG Img1;
};
