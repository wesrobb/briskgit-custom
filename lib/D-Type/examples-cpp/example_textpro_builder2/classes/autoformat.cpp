/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


class CDTAutoformatDefault : public CDTAutoformat
{
public:

	CDTAutoformatDefault()
	{
		const DT_TX_ATTRIBS myattr_Head[] =
		{
			{TX_ATTR_FONT_UNIQUEID, {"F0010_GENTI__NS0" /* Gentium font id */}},
			{TX_ATTR_FONT_WIDTH,    {"130" /* in doc units */}},
			{TX_ATTR_FONT_HEIGHT,   {"130" /* in doc units */}},
			{TX_ATTR_BODY_RGBT,     {"5A31A500"}},
			{TX_ATTR_SHADOW_RGBT,   {"AD87E800"}},
			{TX_ATTR_SHADOW_EP,     {"030A" /* effect=03, parameter=0A */}},
			{TX_ATTR_END,           {"" /* mandatory end-of-array */}}
		};

		Make(H1, _H1, 0, 1).Add(TX_ATTR_FONT_WIDTH, 350).Add(TX_ATTR_FONT_HEIGHT, 300).Add(TX_ATTR_FONT_UNIQUEID, "F0011_STIX1__cS0").Add(TX_ATTR_ALIGN, 1).Add(TX_ATTR_ALIGNEND, 1).Add(TX_ATTR_HYPHENATION, (DT_SLONG)0);
		//Make(H2, _H2, 0, 1).Add(myattr_Head);
		Make(H2, _H2, 0, 1) + myattr_Head;
		Make(Center, _Center, 0, 1).Add(TX_ATTR_ALIGN, 1).Add(TX_ATTR_ALIGNEND, 1);
		Make(Bold, _Bold).Add(TX_ATTR_FONT_UNIQUEID_RULE_CHECK, "*************c**");
		Make(Small, _Small).Add(TX_ATTR_FONT_WIDTH, 50).Add(TX_ATTR_FONT_HEIGHT, 50);
		Make(Italic, _Italic).Add(TX_ATTR_FONT_UNIQUEID_RULE, "***************4");
		Make(SmallItalic, _SmallItalic).Add(TX_ATTR_FONT_WIDTH, 50).Add(TX_ATTR_FONT_HEIGHT, 50).Add(TX_ATTR_FONT_UNIQUEID_RULE, "***************4");
		Make(Underline, _Underline).Add(TX_ATTR_EFFECT, 1);
	}

	virtual ~CDTAutoformatDefault() {}

public:

	CDTMarkerPlus_Opener  H1,  H2,  Center,  Bold,  Small,  Italic,  SmallItalic,  Underline;
	CDTMarkerPlus_Closer _H1, _H2, _Center, _Bold, _Small, _Italic, _SmallItalic, _Underline;
};


class CDTAutoformatSecondary : public CDTAutoformatDefault
{
public:

	CDTAutoformatSecondary()
	{
		H1.Add(TX_ATTR_ROWBOUND_BODY_RGBT, "F5EDC688");
		H2.Replace(TX_ATTR_SHADOW_RGBT, "88888800").Apply(TX_ATTR_SHADOW_EP, "0320");
	}
};


class CDTAutoformatMath : public CDTAutoformatDefault
{
public:

	CDTAutoformatMath()
	{
		/* any changes go here */
	}
};


class CDTBuilderMath_Autoformat : public CDTBuilderMath
{
public:

	CDTBuilderMath_Autoformat(CDTDocV8* doc, CDTAutoformatDefault& f) : CDTBuilderMath(doc), F(f) {}
	CDTBuilderMath_Autoformat(CDTDocV8& doc, CDTAutoformatDefault& f) : CDTBuilderMath(doc), F(f) {}
	virtual ~CDTBuilderMath_Autoformat() { /*delete F1;*/ }

public:

	DT_BUILDREF M1Fn(DT_BUILDREF b, EDTAlignType align = DE_ALIGN_LEFT) const
	{
		DT_STR_UTF8 s = b.IsPlainText(); if (!s) return b; /* already object, i.e. not text string */

		//auto& t = AutoMathText(s, align, F._Italic); /* break b into fragments, format a..z and A..Z letter as italic */
		CDTBuilderText& t = AutoMathText(s, align, F._Italic); /* break b into fragments, format a..z and A..Z letter as italic */
		t + F.DO; /* do format */
		return t;
	}

	DT_BUILDREF M2Fn(DT_BUILDREF b, EDTAlignType align = DE_ALIGN_LEFT) const
	{
		DT_STR_UTF8 s = b.IsPlainText(); if (!s) return b; /* already object, i.e. not text string */

		CDTBuilderText& t = AutoMathText(s, align, F._SmallItalic, F._Small); /* break b into fragments, format a..z and A..Z letter as italic */
		t + F.DO; /* do format */
		return t;
	}

	DT_BUILDREF Formula_Pi(const DT_CHAR* label);

private:

	CDTAutoformatDefault& F;
};


DT_BUILDREF CDTBuilderMath_Autoformat::Formula_Pi(const DT_CHAR* label)
{
	CDTString s(Doc);

	CDTBuilderCtrl c(Doc);
	CDTBuilderText t(Doc);

	/* formula example from http://functions.wolfram.com/Constants/Pi/09/0014 */
	return c.Base() + label + "\r" + Pi + Eq + LimitInf("n") + Sp + Frac(Sup(2, s() + "n" + Sum + 1), t.Text() + 2 + Diff + Sub("b", 1)) + Mul + Brack(t.Text() + Frac(Sub("b", "n"), 2) + Mul + Sqrt(t.Text() + 2 + Sum + Sub(s.Str() + "b", s.Str() + "n" + Diff + 1) + Mul + Sqrt(t.Text() + 2 + Sum + Sub("b", s.Str() + "n" + Diff + 2) + Mul + Sqrt(t.Text() + 2 + Sum + Ellipsis + Sum + Sub("b", 2) + Mul + Sqrt(t.Text() + 2 + Sum + "sin" + Brack(Frac(t.Text() + Pi + Mul + Sub("b", 1), 4))))))) + Sp + "/;\r" + Sub("b", "n") + Eq + 1 + And + Sub("b", s.Str() + "n" + Diff + 1) + Eq + -1 + And + "(" + Sub("b", "k") + Eq + 1 + And + 2 + LTEq + "k" + LTEq + "n" + Diff + 2 + And + "k" + Element + Integers + ")" + And + Sub("b", 1) + Element + Reals + And + -2 + LTEq + Sub("b", 1) + LTEq + 2;
}
