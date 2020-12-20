/* Symbols */

DT_STR_UTF8 CDTBuilderMath::Nil = "\x08";
DT_STR_UTF8 CDTBuilderMath::Sp = "\x20"; //" ";
DT_STR_UTF8 CDTBuilderMath::ThinSp = "\xE2\x80\x89"; //" ";
DT_STR_UTF8 CDTBuilderMath::Pi = "\xCF\x80"; //"π";
DT_STR_UTF8 CDTBuilderMath::Lambda = "\xCE\xBB"; //"λ";
DT_STR_UTF8 CDTBuilderMath::Sigma = "\xCE\xA3"; //"Σ";
DT_STR_UTF8 CDTBuilderMath::Inf = "\xE2\x88\x9E"; //"∞";
DT_STR_UTF8 CDTBuilderMath::Ellipsis = "\xE2\x80\xA6"; //"…";
DT_STR_UTF8 CDTBuilderMath::Integers = "\xE2\x84\x95"; //"ℕ";
DT_STR_UTF8 CDTBuilderMath::Reals = "\xE2\x84\x9D"; //"ℝ";
DT_STR_UTF8 CDTBuilderMath::OneQuarter = "\xC2\xBC"; //"¼";
DT_STR_UTF8 CDTBuilderMath::OneHalf = "\xC2\xBD"; //"½";
DT_STR_UTF8 CDTBuilderMath::ThreeQuarters = "\xC2\xBE"; //"¾";
DT_STR_UTF8 CDTBuilderMath::ParallelTo = "\xE2\x88\xA5"; //"∥";
DT_STR_UTF8 CDTBuilderMath::PartialDifferential = "\xE2\x88\x82"; //"∂";
DT_STR_UTF8 CDTBuilderMath::PartialDifferentialItalic = "\xF0\x9D\x9C\x95"; //"𝜕";

DT_STR_UTF8 CDTBuilderMath::Prime = "\xE2\x80\xB2"; //"′";
DT_STR_UTF8 CDTBuilderMath::DoublePrime = "\xE2\x80\xB3"; //"″";
DT_STR_UTF8 CDTBuilderMath::TriplePrime = "\xE2\x80\xB4"; //"‴";
DT_STR_UTF8 CDTBuilderMath::QuadruplePrime = "\xE2\x81\x97"; //"⁗";
DT_STR_UTF8 CDTBuilderMath::ReversedPrime = "\xE2\x80\xB5"; //"‵";
DT_STR_UTF8 CDTBuilderMath::ReversedDoublePrime = "\xE2\x80\xB6"; //"‶";
DT_STR_UTF8 CDTBuilderMath::ReversedTriplePrime = "\xE2\x80\xB7"; //"‷";

/* Binary operators */

DT_STR_UTF8 CDTBuilderMath::Element = "\x20\xE2\x88\x88\x20"; //" ∈ ";
DT_STR_UTF8 CDTBuilderMath::And = "\x20\xE2\x8B\x80\x20"; //" ⋀ ";
DT_STR_UTF8 CDTBuilderMath::Or = "\x20\xE2\x8B\x81\x20"; //" ⋁ ";
DT_STR_UTF8 CDTBuilderMath::Eq = "\x20\x3D\x20"; //" = ";
DT_STR_UTF8 CDTBuilderMath::LT = "\x20\x3C\x20"; //" < ";
DT_STR_UTF8 CDTBuilderMath::GT = "\x20\x3E\x20"; //" > ";
DT_STR_UTF8 CDTBuilderMath::LTEq = "\x20\xE2\x89\xA4\x20"; //" ≤ ";
DT_STR_UTF8 CDTBuilderMath::GTEq = "\x20\xE2\x89\xA5\x20"; //" ≥ ";
DT_STR_UTF8 CDTBuilderMath::Sum = "\x20\x2B\x20"; //" + ";
DT_STR_UTF8 CDTBuilderMath::Mul = "\xE2\x80\x89"; //" ";
DT_STR_UTF8 CDTBuilderMath::Mul2 = "\x20\xC2\xB7\x20"; //" · ";
DT_STR_UTF8 CDTBuilderMath::MulX = "\x20\xC3\x97\x20"; //" × ";
DT_STR_UTF8 CDTBuilderMath::Div = "\x20\x2F\x20"; //" / ";
DT_STR_UTF8 CDTBuilderMath::Diff = "\x20\xE2\x80\x93\x20"; //" – ";
DT_STR_UTF8 CDTBuilderMath::ArrL = "\x20\xE2\x86\x90\x20"; //" ← ";
DT_STR_UTF8 CDTBuilderMath::ArrR = "\x20\xE2\x86\x92\x20"; //" → ";


CDTBuilderMath& CDTBuilderMath::FnOneArg(DT_ID_SWORD f, DT_BUILDREF b0, const CDTAttribs& a /*= CDTAttribs()*/) const
{
	switch (f)
	{
	case 2403: return VBar(b0, a);
	case 2404: return Cross(b0, a);
	case 2405: return Brack(b0, a);
	case 2419: return Diag(b0, a);
	case 2420: return VBarTight(b0, a);
	case 2421: return VBarLoose(b0, a);
	case 2424: return Up(b0, a);
	case 2425: return Down(b0, a);
	}
	return VBar(b0, a);
}


CDTBuilderMath& CDTBuilderMath::FnTwoArg(DT_ID_SWORD f, DT_BUILDREF b0, DT_BUILDREF b1, const CDTAttribs& a /*= CDTAttribs()*/) const
{
	switch (f)
	{
	case 2401: return Root(b0, b1, a);
	case 2402: return Frac(b0, b1, a);
	case 2406: return Sup(b0, b1, a);
	case 2407: return Sub(b0, b1, a);
	case 2408: return AboveX(b0, b1, a);
	case 2409: return BelowX(b0, b1, a);
	}
	return Root(b0, b1, a);
}


CDTBuilderMath& CDTBuilderMath::FnThreeArg(DT_ID_SWORD f, DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, const CDTAttribs& a /*= CDTAttribs()*/) const
{
	switch (f)
	{
	case 2412: return Summation(b0, b1, b2, a);
	case 2413: return Product(b0, b1, b2, a);
	case 2414: return Coproduct(b0, b1, b2, a);
	case 2415: return Union(b0, b1, b2, a);
	case 2416: return Intersect(b0, b1, b2, a);
	case 2417: return LogicalOr(b0, b1, b2, a);
	case 2418: return LogicalAnd(b0, b1, b2, a);
	case 2422: return SupSub(b0, b1, b2, a);
	case 2423: return SubSup(b0, b1, b2, a);
	case 2426: return Integral(b0, b1, b2, a);
	case 2427: return IntegralCont(b0, b1, b2, a);
	case 2428: return IntegralCW(b0, b1, b2, a);
	case 2429: return IntegralCWCont(b0, b1, b2, a);
	case 2430: return IntegralCCWCont(b0, b1, b2, a); //return IntegralACWCont(b0, b1, b2, a);
	case 2431: return IntegralDouble(b0, b1, b2, a);
	case 2432: return IntegralSurface(b0, b1, b2, a);
	case 2433: return IntegralTriple(b0, b1, b2, a);
	case 2434: return IntegralVolume(b0, b1, b2, a);
	case 2435: return IntegralUpright(b0, b1, b2, a);
	case 2436: return IntegralUprightCont(b0, b1, b2, a);
	case 2437: return IntegralUprightCW(b0, b1, b2, a);
	case 2438: return IntegralUprightCWCont(b0, b1, b2, a);
	case 2439: return IntegralUprightCCWCont(b0, b1, b2, a); //return IntegralUprightACWCont(b0, b1, b2, a);
	case 2440: return IntegralUprightDouble(b0, b1, b2, a);
	case 2441: return IntegralUprightSurface(b0, b1, b2, a);
	case 2442: return IntegralUprightTriple(b0, b1, b2, a);
	case 2443: return IntegralUprightVolume(b0, b1, b2, a);
	case 2444: return IntegralCCW(b0, b1, b2, a); //return IntegralACW(b0, b1, b2, a);
	case 2445: return IntegralUprightCCW(b0, b1, b2, a); //return IntegralUprightACW(b0, b1, b2, a);
	}
	return Summation(b0, b1, b2, a);
}


CDTBuilderMath& CDTBuilderMath::Math1_Internal(const CDTAttribs& a, DT_BUILDREF b0, DT_ID_SWORD style_id)
{
	DT_UBYTE black[4] = {0, 0, 0, 0};

	//printf("style_id=%d\n", style_id);

	/* VBars - Legacy */

	     if (style_id == 0) Make_Internal(a, Doc->GetClip(Clip_990000_a), false); /* Tight Vertical Bars */
	else if (style_id == 1) Make_Internal(a, Doc->GetClip(Clip_990000_b), false); /* Tight Dotted Vertical Bars */

	/* Cross */

	else if (style_id == 0 + 16) Make_Internal(a, Doc->GetClip(Clip_990005_a), false); /* Crossed Lines */
	else if (style_id == 1 + 16) Make_Internal(a, Doc->GetClip(Clip_990005_b), false); /* Dotted Crossed Lines */

	/* Brackets */

	else if (style_id ==  0 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_01), false); /* Regular */
	else if (style_id ==  1 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_02), false); /* Square */
	else if (style_id ==  2 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_03), false); /* Curly */
	else if (style_id ==  3 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_04), false); /* Angle */
	else if (style_id ==  4 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_05), false); /* Double Angle */
	else if (style_id ==  5 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_06), false);
	else if (style_id ==  6 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_07), false);
	else if (style_id ==  7 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_08), false);
	else if (style_id ==  8 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_09), false);
	else if (style_id ==  9 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_10), false);
	else if (style_id == 10 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_11), false);
	else if (style_id == 11 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_12), false);
	else if (style_id == 12 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_13), false);
	else if (style_id == 13 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_14), false);
	else if (style_id == 14 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_15), false);
	else if (style_id == 15 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_16), false);
	else if (style_id == 16 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_17), false);
	else if (style_id == 17 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_18), false);
	else if (style_id == 18 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_19), false); /* Abs */
	else if (style_id == 19 + 32) Make_Internal(a, Doc->GetClip(Clip_990012_20), false); /* Norm */

	/* VBars - Tight */

	else if (style_id == 0 + 80) Make_Internal(a, Doc->GetClip(Clip_990000_a), false); /* Tight Vertical Bars */
	else if (style_id == 1 + 80) Make_Internal(a, Doc->GetClip(Clip_990000_b), false); /* Tight Dotted Vertical Bars */
	else if (style_id == 2 + 80) Make_Internal(a, Doc->GetClip(Clip_990000_c), false); /* Tight Vertical Bar - Left */
	else if (style_id == 3 + 80) Make_Internal(a, Doc->GetClip(Clip_990000_d), false); /* Tight Dotted Vertical Bar - Left */
	else if (style_id == 4 + 80) Make_Internal(a, Doc->GetClip(Clip_990000_e), false); /* Tight Vertical Bar - Right */
	else if (style_id == 5 + 80) Make_Internal(a, Doc->GetClip(Clip_990000_f), false); /* Tight Dotted Vertical Bar - Right */

	/* VBars - Loose */

	else if (style_id == 0 + 112) Make_Internal(a, Doc->GetClip(Clip_990025_a), false); /* Loose Vertical Bars */
	else if (style_id == 1 + 112) Make_Internal(a, Doc->GetClip(Clip_990025_b), false); /* Loose Dotted Vertical Bars */
	else if (style_id == 2 + 112) Make_Internal(a, Doc->GetClip(Clip_990025_c), false); /* Loose Vertical Bar - Left */
	else if (style_id == 3 + 112) Make_Internal(a, Doc->GetClip(Clip_990025_d), false); /* Loose Dotted Vertical Bar - Left */
	else if (style_id == 4 + 112) Make_Internal(a, Doc->GetClip(Clip_990025_e), false); /* Loose Vertical Bar - Right */
	else if (style_id == 5 + 112) Make_Internal(a, Doc->GetClip(Clip_990025_f), false); /* Loose Dotted Vertical Bar - Right */

	/* Diagonals */

	else if (style_id == 0 + 144) Make_Internal(a, Doc->GetClip(Clip_990005_c), false); /* Diagonal Line - Descending */
	else if (style_id == 1 + 144) Make_Internal(a, Doc->GetClip(Clip_990005_d), false); /* Dotted Diagonal Line - Descending */
	else if (style_id == 2 + 144) Make_Internal(a, Doc->GetClip(Clip_990005_e), false); /* Diagonal Line - Ascending */
	else if (style_id == 3 + 144) Make_Internal(a, Doc->GetClip(Clip_990005_f), false); /* Dotted Diagonal Line - Ascending */

	/* Above */

	else if (style_id == 0 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_a_01), false); /* Right Arrow Above */
	else if (style_id == 1 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_a_02), false); /* Left Arrow Above */
	else if (style_id == 2 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_a_03), false); /* Bidirectional Arrow Above */
	else if (style_id == 3 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_a_04), false); /* Line Above */
	else if (style_id == 4 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_b_01), false); /* Right Arrow Above, Dotted */
	else if (style_id == 5 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_b_02), false); /* Left Arrow Above, Dotted */
	else if (style_id == 6 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_b_03), false); /* Bidirectional Arrow Above, Dotted */
	else if (style_id == 7 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_b_04), false); /* Line Above, Dotted */
	else if (style_id == 8 + 176) Make_Internal(a, Doc->GetClip(Clip_990013_c_01), false); /* Tilde Above */

	/* Below */

	else if (style_id == 0 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_a_01), false); /* Right Arrow Below */
	else if (style_id == 1 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_a_02), false); /* Left Arrow Below */
	else if (style_id == 2 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_a_03), false); /* Bidirectional Arrow Below */
	else if (style_id == 3 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_a_04), false); /* Line Below */
	else if (style_id == 4 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_b_01), false); /* Right Arrow Below, Dotted */
	else if (style_id == 5 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_b_02), false); /* Left Arrow Below, Dotted */
	else if (style_id == 6 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_b_03), false); /* Bidirectional Arrow Below, Dotted */
	else if (style_id == 7 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_b_04), false); /* Line Below, Dotted */
	else if (style_id == 8 + 240) Make_Internal(a, Doc->GetClip(Clip_990014_c_01), false); /* Tilde Below */

	else return *this;

	DT_ID_SLONG obj_head = Head_Internal(black);
	Put_Internal2(obj_head, b0, 0);
	return *this;
}


CDTBuilderMath& CDTBuilderMath::Math2_Internal(const CDTAttribs& a, DT_BUILDREF b0, DT_BUILDREF b1, DT_ID_SWORD style_id)
{
	DT_UBYTE black[4] = {0, 0, 0, 0}; const DT_UBYTE* body = black;
	DT_SWORD magnitude = -1;

	//DT_STREAM_FILE(sd, (n == DV_NULL ? Doc->GetClip(Clip_990004-a) : Doc->GetClip(Clip_990004-b)));

	//printf("style_id=%d\n", style_id);

	/* Frac */

	     if (style_id == 0) Make_Internal(a, Doc->GetClip(Clip_990001_a), false); /* Standard */
	else if (style_id == 1) Make_Internal(a, Doc->GetClip(Clip_990001_b), false); /* Standard Dotted */
	else if (style_id == 2) Make_Internal(a, Doc->GetClip(Clip_990001_c), false); /* Standard Invisible */
	else if (style_id == 3) Make_Internal(a, Doc->GetClip(Clip_990002_a), false); /* Diagonal */
	else if (style_id == 4) Make_Internal(a, Doc->GetClip(Clip_990002_b), false); /* Diagonal Dotted */
	else if (style_id == 5) Make_Internal(a, Doc->GetClip(Clip_990002_c), false); /* Diagonal Invisible */

	/* Root */

	else if (style_id == 0 + 16) Make_Internal(a, Doc->GetClip(Clip_990003_b), false); /* N-Root Type A */
	else if (style_id == 1 + 16) Make_Internal(a, Doc->GetClip(Clip_990004_b), false); /* N-Root Type B */

	/* Sup */
	else if (style_id == 0 + 32) { Make_Internal(a, Doc->GetClip(Clip_990010), false); body = DV_NULL; magnitude = a.AMagnitude; } /* Superscript, After */
	else if (style_id == 1 + 32) { Make_Internal(a, Doc->GetClip(Clip_990015), false); body = DV_NULL; magnitude = a.AMagnitude; } /* Superscript, Before */

	/* Sub */
	else if (style_id == 0 + 40) { Make_Internal(a, Doc->GetClip(Clip_990020), false); body = DV_NULL; magnitude = a.AMagnitude; } /* Subscript, After */
	else if (style_id == 1 + 40) { Make_Internal(a, Doc->GetClip(Clip_990021), false); body = DV_NULL; magnitude = a.AMagnitude; } /* Subscript, Before */

	/* Above */
	else if (style_id == 0 + 48) { Make_Internal(a, Doc->GetClip(Clip_990022), false); /* Text Above */ body = DV_NULL; }

	/* Below */
	else if (style_id == 0 + 56) { Make_Internal(a, Doc->GetClip(Clip_990023), false); /* Text Below */ body = DV_NULL; }

	else return *this;

	DT_ID_SLONG obj_head = Head_Internal(body, magnitude);
	Put_Internal2(obj_head, b0, 0);
	Put_Internal2(obj_head, b1, 1);
	return *this;
}


CDTBuilderMath& CDTBuilderMath::Math3_Internal(const CDTAttribs& a, DT_BUILDREF b0, DT_BUILDREF b1, DT_BUILDREF b2, DT_ID_SWORD style_id)
{
	DT_UBYTE black[4] = {0, 0, 0, 0};
	DT_SWORD magnitude = -1;
	bool normal_order = false;

	/* Summation Sign */

	     if (style_id == 0) Make_Internal(a, Doc->GetClip(Clip_990009_a), false); /* Standard */
	else if (style_id == 1) Make_Internal(a, Doc->GetClip(Clip_990008_a), false); /* Expanding */

	/* Product Sign */

	else if (style_id == 0 + 8) Make_Internal(a, Doc->GetClip(Clip_990009_b), false); /* Standard */
	else if (style_id == 1 + 8) Make_Internal(a, Doc->GetClip(Clip_990008_b), false); /* Expanding */

	/* Coproduct Sign */

	else if (style_id == 0 + 16) Make_Internal(a, Doc->GetClip(Clip_990009_c), false); /* Standard */
	else if (style_id == 1 + 16) Make_Internal(a, Doc->GetClip(Clip_990008_c), false); /* Expanding */

	/* Union Sign */

	else if (style_id == 0 + 24) Make_Internal(a, Doc->GetClip(Clip_990009_d), false); /* Standard */
	else if (style_id == 1 + 24) Make_Internal(a, Doc->GetClip(Clip_990008_d), false); /* Expanding */

	/* Intersection Sign */

	else if (style_id == 0 + 32) Make_Internal(a, Doc->GetClip(Clip_990009_e), false); /* Standard */
	else if (style_id == 1 + 32) Make_Internal(a, Doc->GetClip(Clip_990008_e), false); /* Expanding */

	/* Logical Or Sign */

	else if (style_id == 0 + 40) Make_Internal(a, Doc->GetClip(Clip_990009_f), false); /* Standard */
	else if (style_id == 1 + 40) Make_Internal(a, Doc->GetClip(Clip_990008_f), false); /* Expanding */

	/* Logical And Sign */

	else if (style_id == 0 + 48) Make_Internal(a, Doc->GetClip(Clip_990009_g), false); /* Standard */
	else if (style_id == 1 + 48) Make_Internal(a, Doc->GetClip(Clip_990008_g), false); /* Expanding */

	/* SupSub */

	else if (style_id == 0 + 56) { Make_Internal(a, Doc->GetClip(Clip_990011), false); normal_order = true; magnitude = a.AMagnitude; } /* Superscript and Subscript, After */
	else if (style_id == 1 + 56) { Make_Internal(a, Doc->GetClip(Clip_990016), false); normal_order = true; magnitude = a.AMagnitude; } /* Superscript and Subscript, Before */

	/* Integral */

	else if (style_id == 0 + 64) Make_Internal(a, Doc->GetClip(Clip_990007_a_01), false); /* Standard */
	else if (style_id == 1 + 64) Make_Internal(a, Doc->GetClip(Clip_990006_a_01), false); /* Expanding */

	/* Contour Integral */

	else if (style_id == 0 + 72) Make_Internal(a, Doc->GetClip(Clip_990007_b_01), false); /* Standard */
	else if (style_id == 1 + 72) Make_Internal(a, Doc->GetClip(Clip_990006_b_02), false); /* Expanding */

	/* Clockwise Integral */

	else if (style_id == 0 + 80) Make_Internal(a, Doc->GetClip(Clip_990007_c_01), false); /* Standard */
	else if (style_id == 1 + 80) Make_Internal(a, Doc->GetClip(Clip_990006_c_01), false); /* Expanding */

	/* Clockwise Contour Integral */

	else if (style_id == 0 + 88) Make_Internal(a, Doc->GetClip(Clip_990007_d_01), false); /* Standard */
	else if (style_id == 1 + 88) Make_Internal(a, Doc->GetClip(Clip_990006_d_01), false); /* Expanding */

	/* Anticlockwise Contour Integral */

	else if (style_id == 0 + 96) Make_Internal(a, Doc->GetClip(Clip_990007_e_01), false); /* Standard */
	else if (style_id == 1 + 96) Make_Internal(a, Doc->GetClip(Clip_990006_e_01), false); /* Expanding */

	/* Double Integral */

	else if (style_id == 0 + 104) Make_Internal(a, Doc->GetClip(Clip_990007_a_02), false); /* Standard */
	else if (style_id == 1 + 104) Make_Internal(a, Doc->GetClip(Clip_990006_a_02), false); /* Expanding */

	/* Surface Integral */

	else if (style_id == 0 + 112) Make_Internal(a, Doc->GetClip(Clip_990007_b_02), false); /* Standard */
	else if (style_id == 1 + 112) Make_Internal(a, Doc->GetClip(Clip_990006_b_02), false); /* Expanding */

	/* Triple Integral */

	else if (style_id == 0 + 120) Make_Internal(a, Doc->GetClip(Clip_990007_a_03), false); /* Standard */
	else if (style_id == 1 + 120) Make_Internal(a, Doc->GetClip(Clip_990006_a_03), false); /* Expanding */

	/* Volume Integral */

	else if (style_id == 0 + 128) Make_Internal(a, Doc->GetClip(Clip_990007_b_03), false); /* Standard */
	else if (style_id == 1 + 128) Make_Internal(a, Doc->GetClip(Clip_990006_b_03), false); /* Expanding */

	/* Upright Integral */

	else if (style_id == 0 + 136) Make_Internal(a, Doc->GetClip(Clip_990019_a_01), false); /* Standard */
	else if (style_id == 1 + 136) Make_Internal(a, Doc->GetClip(Clip_990018_a_01), false); /* Expanding */

	/* Upright Contour Integral */

	else if (style_id == 0 + 144) Make_Internal(a, Doc->GetClip(Clip_990019_b_01), false); /* Standard */
	else if (style_id == 1 + 144) Make_Internal(a, Doc->GetClip(Clip_990018_b_01), false); /* Expanding */

	/* Upright Clockwise Integral */

	else if (style_id == 0 + 152) Make_Internal(a, Doc->GetClip(Clip_990019_c_01), false); /* Standard */
	else if (style_id == 1 + 152) Make_Internal(a, Doc->GetClip(Clip_990018_c_01), false); /* Expanding */

	/* Upright Clockwise Contour Integral */

	else if (style_id == 0 + 160) Make_Internal(a, Doc->GetClip(Clip_990019_d_01), false); /* Standard */
	else if (style_id == 1 + 160) Make_Internal(a, Doc->GetClip(Clip_990018_d_01), false); /* Expanding */

	/* Upright Anticlockwise Contour Integral */

	else if (style_id == 0 + 168) Make_Internal(a, Doc->GetClip(Clip_990019_e_01), false); /* Standard */
	else if (style_id == 1 + 168) Make_Internal(a, Doc->GetClip(Clip_990018_e_01), false); /* Expanding */

	/* Upright Double Integral */

	else if (style_id == 0 + 176) Make_Internal(a, Doc->GetClip(Clip_990019_a_02), false); /* Standard */
	else if (style_id == 1 + 176) Make_Internal(a, Doc->GetClip(Clip_990018_a_02), false); /* Expanding */

	/* Upright Surface Integral */

	else if (style_id == 0 + 184) Make_Internal(a, Doc->GetClip(Clip_990019_b_02), false); /* Standard */
	else if (style_id == 1 + 184) Make_Internal(a, Doc->GetClip(Clip_990018_b_02), false); /* Expanding */

	/* Upright Triple Integral */

	else if (style_id == 0 + 192) Make_Internal(a, Doc->GetClip(Clip_990019_a_03), false); /* Standard */
	else if (style_id == 1 + 192) Make_Internal(a, Doc->GetClip(Clip_990018_a_03), false); /* Expanding */

	/* Upright Volume Integral */

	else if (style_id == 0 + 200) Make_Internal(a, Doc->GetClip(Clip_990019_b_03), false); /* Standard */
	else if (style_id == 1 + 200) Make_Internal(a, Doc->GetClip(Clip_990018_b_03), false); /* Expanding */

	/* Anticlockwise Integral */

	else if (style_id == 0 + 208) Make_Internal(a, Doc->GetClip(Clip_990007_f_01), false); /* Standard */
	else if (style_id == 1 + 208) Make_Internal(a, Doc->GetClip(Clip_990006_f_01), false); /* Expanding */

	/* Upright Anticlockwise Integral */

	else if (style_id == 0 + 216) Make_Internal(a, Doc->GetClip(Clip_990019_f_01), false); /* Standard */
	else if (style_id == 1 + 216) Make_Internal(a, Doc->GetClip(Clip_990018_f_01), false); /* Expanding */

	else return *this;

	DT_ID_SLONG obj_head = Head_Internal(black, magnitude);

	if (normal_order)
	{
		Put_Internal2(obj_head, b0, 0);
		Put_Internal2(obj_head, b1, 1);
		Put_Internal2(obj_head, b2, 2);
	}
	else
	{
		Put_Internal2(obj_head, b2, 0);
		Put_Internal2(obj_head, b1, 1);
		Put_Internal2(obj_head, b0, 2);
	}

	return *this;
}


CDTBuilderMath& CDTBuilderMath::Matrix_Internal(const CDTAttribs& a)
{
	//m.Matrix() / "a" / "b" / "c" / "d";

	DT_ID_SWORD style_id = Val(a.AStyleID, 5);
	//printf("style_id=%d\n", a.AStyleID);

	/* normal nesting */
	if (style_id == 0) { Make_Internal(a, Doc->GetClip(Clip_990031_a_01_01), true); RowMax = 1; ColMax = 1; } /* Matrix 1x1 */
	/* deep nesting (have pdPowerGlyphAttribs set to 16) */
	else if (style_id == 1) { Make_Internal(a, Doc->GetClip(Clip_990031_b_01_01), true); RowMax = 1; ColMax = 1; } /* Matrix 1x1, Regular Bracket */
	else if (style_id == 2) { Make_Internal(a, Doc->GetClip(Clip_990031_c_01_01), true); RowMax = 1; ColMax = 1; } /* Matrix 1x1, Square Bracket */
	else if (style_id == 3) { Make_Internal(a, Doc->GetClip(Clip_990031_d_01_01), true); RowMax = 1; ColMax = 1; } /* Matrix 1x1, Curly Bracket */
	else if (style_id == 4) { Make_Internal(a, Doc->GetClip(Clip_990031_e_01_01), true); RowMax = 1; ColMax = 1; } /* Matrix 1x1, Angle Bracket */
	else if (style_id == 5) { Make_Internal(a, Doc->GetClip(Clip_990031_f_01_01), true); RowMax = 1; ColMax = 1; } /* Matrix 1x1, Double Angle Bracket */

	else return *this;

	return *this;
}


#include "dtbuilder_math_autoformat.cpp"
