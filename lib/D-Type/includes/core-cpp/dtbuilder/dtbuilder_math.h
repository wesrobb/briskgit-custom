/* on Linux 64 bit the words Above and Below are reserved! */
#undef Above
#undef Below


class CDTBuilderMath : public CDTBuilderCtrl
{
public:

	class CDTAttribs : public CDTAttribsBase
	{
	public:

		CDTAttribs() : AStyleID(0), AMagnitude(-1) {}
		virtual ~CDTAttribs() {}

		DEF_ATTRIBS_BASE_METHODS

		inline CDTAttribs& StyleID(DT_ID_SWORD style_id) { AStyleID = style_id; return *this; }
		inline CDTAttribs& Magnitude(DT_SWORD magnitude) { AMagnitude = magnitude; return *this; }

		DT_ID_SWORD AStyleID;
		DT_SWORD AMagnitude; /* magnitude of superscript/subscript (-1 = default, which is 2/3) */
	};

public:

	CDTBuilderMath(CDTDocV8* doc) : CDTBuilderCtrl(doc) {}
	CDTBuilderMath(CDTDocV8& doc) : CDTBuilderCtrl(doc) {}
	virtual ~CDTBuilderMath() {}

	CDTBuilderMath* New() const { CDTBuilderMath* n = new CDTBuilderMath(Doc); Doc->AddAtom(n); return n; }

	/* No AutoMathText */
	virtual DT_BUILDREF M1Fn(DT_BUILDREF b, EDTAlignType /*align*/ = DE_ALIGN_LEFT) const { return b; }
	virtual DT_BUILDREF M2Fn(DT_BUILDREF b, EDTAlignType /*align*/ = DE_ALIGN_LEFT) const { return b; }

	//DT_BUILDREF
	/*const*/ CDTBuilderText& AutoMathText(const DT_CHAR* s, EDTAlignType align = DE_ALIGN_LEFT, CDTMarkerPlus_Closer* f_letter_closer = DV_NULL, CDTMarkerPlus_Closer* f_other_closer = DV_NULL) const;
	//CDTBuilderUTF8 AutoMathText2(DT_BUILDREF b, EDTAlignType align /*= DE_ALIGN_LEFT*/, CDTMarkerPlus_Closer* cl /*= DV_NULL*/) const;

	inline CDTBuilderText& AutoMathText(const DT_CHAR* s, EDTAlignType align /*= DE_ALIGN_LEFT*/, CDTMarkerPlus_Closer& f_letter_closer, CDTMarkerPlus_Closer& f_other_closer) const
	{
		return AutoMathText(s, align, &f_letter_closer, &f_other_closer);
	}

	inline CDTBuilderText& AutoMathText(const DT_CHAR* s, EDTAlignType align /*= DE_ALIGN_LEFT*/, CDTMarkerPlus_Closer& f_letter_closer) const
	{
		return AutoMathText(s, align, &f_letter_closer, DV_NULL);
	}

	//void SetAutoMath(m1, m2) { M1Fn = m1; M2Fn = m2; }

public:

	/* Symbols */
	static DT_STR_UTF8 Nil, Sp, ThinSp, Pi, Lambda, Sigma, Inf, Ellipsis, Integers, Reals, OneQuarter, OneHalf, ThreeQuarters, ParallelTo, PartialDifferential, PartialDifferentialItalic;
	static DT_STR_UTF8 Prime, DoublePrime, TriplePrime, QuadruplePrime, ReversedPrime, ReversedDoublePrime, ReversedTriplePrime;

	/* Binary operators */
	static DT_STR_UTF8 Element, And, Or, Eq, LT, GT, LTEq, GTEq, Sum, Mul, Mul2, MulX, Div, Diff, ArrL, ArrR;

	inline CDTBuilderMath& Matrix(const CDTAttribs& a = CDTAttribs()) const { CDTBuilderMath* b = New(); b->Matrix_Internal(a); return *b; }

	/* Native One */

	inline CDTBuilderMath& VBar(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		CDTBuilderMath* b = New(); b->Math1_Internal(a, b0_, Val(a.AStyleID, 1)); return *b;
	}
	inline CDTBuilderMath& Cross(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		CDTBuilderMath* b = New(); b->Math1_Internal(a, b0_, Val(a.AStyleID, 1, 16)); return *b;
	}
	inline CDTBuilderMath& Brack(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		CDTBuilderMath* b = New(); b->Math1_Internal(a, b0_, Val(a.AStyleID, 19, 32)); return *b;
	}
	inline CDTBuilderMath& Diag(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		CDTBuilderMath* b = New(); b->Math1_Internal(a, b0_, Val(a.AStyleID, 3, 144)); return *b;
	}
	inline CDTBuilderMath& VBarTight(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		CDTBuilderMath* b = New(); b->Math1_Internal(a, b0_, Val(a.AStyleID, 5, 80)); return *b;
	}
	inline CDTBuilderMath& VBarLoose(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		CDTBuilderMath* b = New(); b->Math1_Internal(a, b0_, Val(a.AStyleID, 5, 112)); return *b;
	}
	inline CDTBuilderMath& Up(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		CDTBuilderMath* b = New(); b->Math1_Internal(a, b0_, Val(a.AStyleID, 8, 176)); return *b;
	}
	inline CDTBuilderMath& Down(DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		CDTBuilderMath* b = New(); b->Math1_Internal(a, b0_, Val(a.AStyleID, 8, 240)); return *b;
	}

	CDTBuilderMath& FnOneArg(DT_ID_SWORD f, DT_BUILDREF b0, const CDTAttribs& a = CDTAttribs()) const;

	/* Native Two */

	inline CDTBuilderMath& Root(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M1Fn(b1);
		CDTBuilderMath* b = New(); b->Math2_Internal(a, b0_, b1_, Val(a.AStyleID, 1, 16)); return *b;
	}
	inline CDTBuilderMath& Frac(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		DT_BUILDREF b1_ = M1Fn(b1);
		CDTBuilderMath* b = New(); b->Math2_Internal(a, b0_, b1_, Val(a.AStyleID, 5)); return *b;
	}
	inline CDTBuilderMath& Sup(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		CDTBuilderMath* b = New(); b->Math2_Internal(a, b0_, b1_, Val(a.AStyleID, 1, 32)); return *b;
	}
	inline CDTBuilderMath& Sub(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		CDTBuilderMath* b = New(); b->Math2_Internal(a, b0_, b1_, Val(a.AStyleID, 1, 40)); return *b;
	}
	/* on Linux 64 bit the words Above and Below are reserved! */
	inline CDTBuilderMath& AboveX(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		CDTBuilderMath* b = New(); b->Math2_Internal(a, b0_, b1_, Val(a.AStyleID, 0, 48)); return *b;
	}
	inline CDTBuilderMath& BelowX(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M1Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		CDTBuilderMath* b = New(); b->Math2_Internal(a, b0_, b1_, Val(a.AStyleID, 0, 56)); return *b;
	}
	inline CDTBuilderMath& Above(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const { return AboveX(b0, b1, a); }
	inline CDTBuilderMath& Below(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const { return BelowX(b0, b1, a); }

	CDTBuilderMath& FnTwoArg(DT_ID_SWORD f, DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const;

	/* Shortcuts Two */

	inline CDTBuilderMath& Sqrt(DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const { return Root(CDTBuilderUTF8(""), b1, a); }
	inline CDTBuilderMath& Sqrt(const CDTAttribs& a = CDTAttribs()) const { return Root(CDTBuilderUTF8(""), CDTBuilderUTF8(""), a); }
	inline CDTBuilderMath& Limit(DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const { return BelowX(CDTBuilderText(Doc).Text() + "lim", b1, a); }
	inline CDTBuilderMath& LimitInf(DT_STR_UTF8 s1, const CDTAttribs& a = CDTAttribs()) const { DT_STRING_CHARS(str, PDX_MAXTEXTLENGTH, ""); dtStrConcatChars(&str, s1); dtStrConcatChars(&str, ArrR); dtStrConcatChars(&str, Inf); return Limit(str.str, a); }

	/* Native Three */

	inline CDTBuilderMath& Summation(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1)); return *b;
	}
	inline CDTBuilderMath& Product(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 8)); return *b;
	}
	inline CDTBuilderMath& Coproduct(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 16)); return *b;
	}
	inline CDTBuilderMath& Union(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 24)); return *b;
	}
	inline CDTBuilderMath& Intersect(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 32)); return *b;
	}
	inline CDTBuilderMath& LogicalOr(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 40)); return *b;
	}
	inline CDTBuilderMath& LogicalAnd(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 48)); return *b;
	}
	inline CDTBuilderMath& SupSub(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		/* different from others */
		DT_BUILDREF b0_ = M1Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M2Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 56)); return *b;
	}
	/* the order of b1 and b2_ is swapped compared to SupSub */
	inline CDTBuilderMath& SubSup(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		/* different from others */
		DT_BUILDREF b0_ = M1Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M2Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b2_, b1_, Val(a.AStyleID, 1, 56)); return *b;
	}
	inline CDTBuilderMath& Integral(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 64)); return *b;
	}
	inline CDTBuilderMath& IntegralCont(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 72)); return *b;
	}
	inline CDTBuilderMath& IntegralCW(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 80)); return *b;
	}
	inline CDTBuilderMath& IntegralCWCont(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 88)); return *b;
	}
	inline CDTBuilderMath& IntegralCCWCont(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 96)); return *b;
	}
	inline CDTBuilderMath& IntegralACWCont(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 96)); return *b;
	}
	inline CDTBuilderMath& IntegralDouble(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 104)); return *b;
	}
	inline CDTBuilderMath& IntegralSurface(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 112)); return *b;
	}
	inline CDTBuilderMath& IntegralTriple(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 120)); return *b;
	}
	inline CDTBuilderMath& IntegralVolume(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 128)); return *b;
	}
	inline CDTBuilderMath& IntegralUpright(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 136)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightCont(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 144)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightCW(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 152)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightCWCont(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 160)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightCCWCont(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 168)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightACWCont(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 168)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightDouble(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 176)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightSurface(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 184)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightTriple(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 192)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightVolume(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 200)); return *b;
	}
	inline CDTBuilderMath& IntegralCCW(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 208)); return *b;
	}
	inline CDTBuilderMath& IntegralACW(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 208)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightCCW(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 216)); return *b;
	}
	inline CDTBuilderMath& IntegralUprightACW(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const
	{
		DT_BUILDREF b0_ = M2Fn(b0);
		DT_BUILDREF b1_ = M2Fn(b1);
		DT_BUILDREF b2_ = M1Fn(b2);
		CDTBuilderMath* b = New(); b->Math3_Internal(a, b0_, b1_, b2_, Val(a.AStyleID, 1, 216)); return *b;
	}

	CDTBuilderMath& FnThreeArg(DT_ID_SWORD f, DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const;

	/* Derived */
	inline CDTBuilderMath& Vector(DT_BUILDREF b0) const { return Up(b0); }
	inline CDTBuilderMath& Tilde(DT_BUILDREF b0) const { return Up(b0, CDTAttribs().StyleID(8)); }
	inline CDTBuilderMath& TildeDown(DT_BUILDREF b0) const { return Down(b0, CDTAttribs().StyleID(8)); }
	inline CDTBuilderMath& BrackRound(DT_BUILDREF b0) const { return Brack(b0); }
	inline CDTBuilderMath& BrackSquare(DT_BUILDREF b0) const { return Brack(b0, CDTAttribs().StyleID(1)); }
	inline CDTBuilderMath& BrackCurly(DT_BUILDREF b0) const { return Brack(b0, CDTAttribs().StyleID(2)); }
	inline CDTBuilderMath& Abs(DT_BUILDREF b0) const { return Brack(b0, CDTAttribs().StyleID(18)); }
	inline CDTBuilderMath& Norm(DT_BUILDREF b0) const { return Brack(b0, CDTAttribs().StyleID(19)); }

	inline DT_ID_SWORD GetStyleID(const CDTAttribs& a) const { return a.AStyleID; }

	inline CDTBuilderMath& EvalBarA(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const { return Sup(VBarLoose(b0, CDTAttribs().StyleID((GetStyleID(a) > 0) ? 2 : 4)), b1, a); }
	inline CDTBuilderMath& EvalBarB(DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a = CDTAttribs()) const { return Sub(VBarLoose(b0, CDTAttribs().StyleID((GetStyleID(a) > 0) ? 2 : 4)), b1, a); }
	inline CDTBuilderMath& EvalBarAB(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const { return SupSub(VBarLoose(b0, CDTAttribs().StyleID((GetStyleID(a) > 0) ? 2 : 4)), b1, b2, a); }
	inline CDTBuilderMath& EvalBarBA(DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a = CDTAttribs()) const { return SubSup(VBarLoose(b0, CDTAttribs().StyleID((GetStyleID(a) > 0) ? 2 : 4)), b1, b2, a); }

	//inline CDTBuilderMath& Test(DT_STR_UTF8 s = "") { return Abs(s); }

	inline CDTBuilderMath::CDTAttribs operator () () { return CDTAttribs(); }

private:

	CDTBuilderMath& Math1_Internal(const CDTAttribs& a, DT_BUILDREF b0, DT_ID_SWORD style_id);
	CDTBuilderMath& Math2_Internal(const CDTAttribs& a, DT_BUILDREF b0, DT_BUILDREF b1, DT_ID_SWORD style_id);
	CDTBuilderMath& Math3_Internal(const CDTAttribs& a, DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, DT_ID_SWORD style_id);
	CDTBuilderMath& Matrix_Internal(const CDTAttribs& a);
};
