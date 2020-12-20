//DT_BUILDREF
/*const*/ CDTBuilderText& CDTBuilderMath::AutoMathText(const DT_CHAR* s, EDTAlignType align /*= DE_ALIGN_LEFT*/, CDTMarkerPlus_Closer* f_letter_closer /*= DV_NULL*/, CDTMarkerPlus_Closer* f_other_closer /*= DV_NULL*/) const
{
	//DT_STR_UTF8 s = b.IsPlainText(); if (!s) { /*printf("not plain text\n");*/ return b; }
	//if (s[0] == 0) { /*printf("empty plain text\n");*/ return b; } //CDTBuilderUTF8("");

	/*printf("plain text\n");*/

	CDTMarkerPlus_NoOp no_op;
	CDTMarker* f_letter_opener = &no_op; if (f_letter_closer != DV_NULL) f_letter_opener = f_letter_closer->GetOpener(); else f_letter_closer = &no_op;
	CDTMarker* f_other_opener = &no_op; if (f_other_closer != DV_NULL) f_other_opener = f_other_closer->GetOpener(); else f_other_closer = &no_op;

	CDTBuilderText builder_text(Doc);
	CDTBuilderText& t = builder_text.Text(builder_text().Align(align).AlignEnd(align));
	//t + (*f_letter_opener); t + s; t + (*f_letter_closer); return t;

	if (s[0] == 0) return t;

	DT_SLONG i, j;
	DT_SWORD p_type = -1;
	DT_SWORD c_type = -1;
	DT_CHAR buff[1024 + 8] = "";

	for (i = j = 0; ; i++, p_type = c_type)
	{
		DT_UBYTE c = s[i]; buff[j] = c; j++; if (j >= 1024) p_type = -1; /* this ensures the buffer is reset */

		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) c_type = 1; /*letter*/ else c_type = 0; /*other*/

		if (i == 0) /* first char */
		{
			if (c_type == 1) t + (*f_letter_opener); else t + (*f_other_opener);
		}
		else if (c == 0) /* last char */
		{
			if (p_type == 1) t + buff + (*f_letter_closer); else t + buff + (*f_other_closer);
			break;
		}
		else if (c_type == 1) /* letter */
		{
			if (c_type == p_type) continue; /* no change, still 'letter'*/
			/* start of new 'letter' run */
			buff[j - 1] = 0;
			t + buff + (*f_other_closer) + (*f_letter_opener);
			buff[0] = c; j = 1;
		}
		else /* other */
		{
			if (c_type == p_type) continue; /* no change, still 'other'*/
			/* start of new 'other' run */
			buff[j - 1] = 0;
			t + buff + (*f_letter_closer) + (*f_other_opener);
			buff[0] = c; j = 1;
		}
	}

	return t;
}


//CDTBuilderUTF8 CDTBuilderMath::AutoMathText2(DT_BUILDREF b, EDTAlignType align /*= DE_ALIGN_LEFT*/, CDTMarkerPlus_Closer* cl /*= DV_NULL*/) const
//{
//	DT_STR_UTF8 s = b.IsPlainText(); if (!s) return b;
//	if (s[0] == 0) return b; //CDTBuilderUTF8("");
//
//	CDTMarker* op = cl->GetOpener();
//
//	CDTBuilderText builder_text(Doc);
//	CDTBuilderText t = builder_text.Text(builder_text().Align(align).AlignEnd(align));
//
//	t + (*op) + s + "*" + (*cl);
//	return t;
//}

