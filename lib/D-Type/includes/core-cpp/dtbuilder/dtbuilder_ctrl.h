enum EDTRuleType
{
	DE_RULE_FIRST = 0,
	DE_RULE_NOLINE = 0,
	DE_RULE_SOLIDLINE = 1,
	DE_RULE_LAST = 1
};

enum EDTBulletType
{
	DE_BULLET_FIRST = 0,
	DE_BULLET_CUSTOMTEXT = 0,
	DE_BULLET_CIRCSOLID = 1,
	DE_BULLET_CIRC = 2,
	DE_BULLET_LINE = 3,
	DE_BULLET_LAST = 3
};

enum EDTDirectionType
{
	DE_DIR_FIRST = 0,
	DE_DIR_LTR = 0,
	DE_DIR_RTL = 1,
	DE_DIR_LAST = 1
};

enum EDTTablePreset
{
	DE_TABLE_FIRST = 0,
	DE_TABLE_NOHEAD = 0,
	DE_TABLE_HHEAD = 1,
	DE_TABLE_VHEAD = 2,
	DE_TABLE_HVHEAD = 3,
	DE_TABLE_LAST = 3
};


class CDTBuilderCtrl : public CDTBuilderText
{
public:

	class CDTAttribs : public CDTAttribsBase
	{
	public:

		CDTAttribs() : ASpacingFactor(512), AMarginL(0), AMarginR(0), AIndentL(0), AIndentR(0), ARuleType(DE_RULE_NOLINE), ABulletType(DE_BULLET_CUSTOMTEXT), ADirection(DE_DIR_LTR), ATablePreset(DE_TABLE_HVHEAD) {}
		virtual ~CDTAttribs() {}

		DEF_ATTRIBS_BASE_METHODS

		inline CDTAttribs& Spacing(DT_SLONG spacing_factor) { ASpacingFactor = spacing_factor; return *this; }
		inline CDTAttribs& MarginL(DT_SLONG margin_l) { AMarginL = margin_l; return *this; }
		inline CDTAttribs& MarginR(DT_SLONG margin_r) { AMarginR = margin_r; return *this; }
		inline CDTAttribs& IndentL(DT_UWORD indent_l) { AIndentL = indent_l; return *this; }
		inline CDTAttribs& IndentR(DT_UWORD indent_r) { AIndentR = indent_r; return *this; }
		inline CDTAttribs& Margin(DT_SLONG margin_l, DT_SLONG margin_r) { AMarginL = margin_l; AMarginR = margin_r; return *this; }
		inline CDTAttribs& Indent(DT_UWORD indent_l, DT_UWORD indent_r) { AIndentL = indent_l; AIndentR = indent_r; return *this; }
		inline CDTAttribs& Margin(DT_SLONG margin) { AMarginL = AMarginR = margin; return *this; }
		inline CDTAttribs& Indent(DT_UWORD indent) { AIndentL = AIndentR = indent; return *this; }
		inline CDTAttribs& Rule(EDTRuleType rule_type) { ARuleType = DF_MIN(DF_MAX(DE_RULE_FIRST, rule_type), DE_RULE_LAST); return *this; }
		inline CDTAttribs& Bullet(EDTBulletType bullet_type) { ABulletType = DF_MIN(DF_MAX(DE_BULLET_FIRST, bullet_type), DE_BULLET_LAST); return *this; }
		inline CDTAttribs& Direction(EDTDirectionType direction_type) { ADirection = DF_MIN(DF_MAX(DE_DIR_FIRST, direction_type), DE_DIR_LAST); return *this; }
		inline CDTAttribs& TablePreset(EDTTablePreset table_preset) { ATablePreset = DF_MIN(DF_MAX(DE_TABLE_FIRST, table_preset), DE_TABLE_LAST); return *this; }

		DT_SLONG ASpacingFactor;
		DT_SLONG AMarginL, AMarginR;
		DT_UWORD AIndentL, AIndentR;
		EDTRuleType ARuleType;
		EDTBulletType ABulletType;
		EDTDirectionType ADirection;
		EDTTablePreset ATablePreset;
	};

public:

	CDTBuilderCtrl(CDTDocV8* doc) : CDTBuilderText(doc), LastPutCell(-1), Col(-1), Row(0), ColMax(0), RowMax(0) { Flags = DV_BUILDER_FLAG_EXBASIC; }
	CDTBuilderCtrl(CDTDocV8& doc) : CDTBuilderText(doc), LastPutCell(-1), Col(-1), Row(0), ColMax(0), RowMax(0) { Flags = DV_BUILDER_FLAG_EXBASIC; }
	virtual ~CDTBuilderCtrl() {}

	CDTBuilderCtrl* New() const { CDTBuilderCtrl* n = new CDTBuilderCtrl(Doc); /*CDTAtom* a = Doc->NextAtom; Doc->NextAtom = n; n->NextAtom = a;*/ Doc->AddAtom(n); return n; }

public:

	/* Symbols */
	static DT_STR_UTF8 Nil, NewCol, NewLine;

public:

	inline CDTBuilderCtrl& Rule_Bare(const CDTAttribs& a = CDTAttribs()) const
	{
		CDTBuilderCtrl* b = New(); b->Rule_Internal(a);
		return *b;
	}

	inline CDTBuilderCtrl& Blank(const CDTAttribs& a = CDTAttribs()) const
	{
		CDTBuilderCtrl* b = New(); b->Base_Internal(a, "");
		return *b;
	}

	inline CDTBuilderCtrl& Br_Bare(const CDTAttribs& a = CDTAttribs()) const
	{
		CDTBuilderCtrl* b = New(); b->Flags |= DV_BUILDER_FLAG_NEWLINE;
		b->Text_Internal(CDTBuilderText::CDTAttribs().StrUTF8("\r").Spacing(a.ASpacingFactor).W(0).H(0).FontW(a.AFontW).FontH(a.AFontH));
		return *b;
	}

	inline CDTBuilderCtrl& Br(const CDTAttribs& a = CDTAttribs()) const
	{
		CDTBuilderCtrl* b = New(); b->Rule_Internal(a);
		CDTBuilderCtrl div(Doc);
		div.Text_Internal(CDTBuilderText::CDTAttribs().W(0).H(0).StrUTF8("\r").FontW(a.AFontW).FontH(a.AFontH).Spacing(a.ASpacingFactor));
		b->AddRun(div); b->Flags |= div.Flags | DV_BUILDER_FLAG_NEWLINE;
		return *b;
	}

	inline CDTBuilderCtrl& Base(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		//CDTBuilderUTF8 bullets[DE_BULLET_LAST + 1] = {b0, "\342\200\242 ", "\342\227\246 ", "\342\201\203 "};
		CDTBuilderUTF8 bullets[DE_BULLET_LAST + 1] = {b0, "\xE2\x80\xA2 ", "\xE2\x97\xA6 ", "\xE2\x81\x83 "};
		DT_SLONG t = a.ABulletType; if (t < 0 || t >= DE_BULLET_LAST) t = 0;
		CDTBuilderCtrl* b = New(); b->Base_Internal(a, bullets[t]);
		return *b;
	}

	inline CDTBuilderCtrl& Base(const CDTAttribs& a = CDTAttribs()) const
	{
		return Base(CDTBuilderUTF8(""), a);
	}

	inline CDTBuilderCtrl& Table(const CDTAttribs& a = CDTAttribs()) const
	{
		CDTBuilderCtrl* b = New(); b->Table_Internal(a);
		return *b;
	}

	CDTBuilderCtrl& Put(DT_BUILDREF b0, DT_SLONG i = -1)
	{
		DT_ID_SLONG parent_obj;

		if (LastPutCell < 0) /* error */
		{
			return *this;
		}
		else if (i == -1) /* next col */
		{
			if (ColMax && Col + 1 >= ColMax) { if ((parent_obj = Put_Internal3("", LastPutCell, false)) < 0) return *this; Refresh_Internal(Img0, parent_obj, 10); ColMax = Col + 2; }
			Col++;
		}
		else if (i == -2) /* next row */
		{
			if (RowMax && Row + 1 >= RowMax) { if ((parent_obj = Put_Internal3("", LastPutCell, false)) < 0) return *this; Refresh_Internal(Img0, parent_obj, 9); RowMax = Row + 2; }
			Col = 0; Row++;
		}

		if (i < 0) i = Row * ColMax + Col;

		//printf("i=%ld, Col=%d, Row=%d, ColMax=%d, RowMax=%d\n", i, Col, Row, ColMax, RowMax);

		Put_Internal3(b0, i);
		return *this;
	}

	CDTBuilderCtrl& SetWidth(DT_SLONG width, DT_SLONG i = -1)
	{
		if (width < 0 || i >= ColMax) return *this; else if (i < 0) i = Col;
		DT_ID_SLONG parent_obj = Put_Internal3("", i, false);
		if (parent_obj >= 0) Refresh_Internal(Img0, parent_obj, 6, width);
		return *this;
	}

	inline CDTBuilderCtrl& operator / (DT_BUILDREF b) { Put(b, -1); return *this; }

	inline CDTBuilderCtrl& operator % (DT_BUILDREF b) { Put(b, -2); return *this; }

	inline CDTBuilderCtrl& operator * (DT_SLONG w) { SetWidth(w, -1); return *this; }

	inline CDTBuilderCtrl::CDTAttribs operator () () { return CDTAttribs(); }

private:

	CDTBuilderCtrl& Rule_Internal(const CDTAttribs& a);
	CDTBuilderCtrl& Base_Internal(const CDTAttribs& a, DT_BUILDREF b0);
	CDTBuilderCtrl& Table_Internal(const CDTAttribs& a);

	void Put_Internal1(DT_BUILDREF b, DT_ID_SLONG parent_obj);
	bool Refresh_Internal(DT_SLONG img, DT_ID_SLONG parent_obj, DT_ID_SWORD command, DT_SLONG param0 = 0);

protected:

	void ChangeFontSize();
	bool Make_Internal(const CDTAttribsBase& a, DT_PDDOC doc, bool clear);
	DT_ID_SLONG Head_Internal(const DT_UBYTE symbol_rgbt[] = DV_NULL, DT_SWORD magnitude = -1);
	DT_ID_SLONG Put_Internal2(DT_ID_SLONG obj, DT_BUILDREF b, DT_SLONG i, bool put = true);
	DT_ID_SLONG Put_Internal3(DT_BUILDREF b, DT_SLONG i, bool put = true) { return Put_Internal2(Head_Internal(DV_NULL, -1), b, i, put); }

	//void Del_Internal();

private:

	DT_SLONG LastPutCell;
	DT_SWORD Col, Row;

protected:

	DT_SWORD ColMax, RowMax;
};
