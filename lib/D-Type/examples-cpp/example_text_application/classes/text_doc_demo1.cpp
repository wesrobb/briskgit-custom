/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"


class CDTTextDocDemo1 : public CDTTextDoc
{
public:

	CDTTextDocDemo1(CDTEngineV8* engine, const DT_CHAR* filename)
	{
		DT_STREAM_FILE(sd, filename);
		DT_STREAM_FILE(sd_fontmap, "config/fontmap.inf");

		DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 800, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};
		if (TextDoc.Make(engine, TX_AUTO, &sd, TX_IMPORT_FULL_COMPACT, &textflow_area, NULL, &sd_fontmap) < 0) return;

		SetFormat();
		SetFlow();
		SetExtras();
	}

public:

	DT_SLONG GetLastArea() { return 11; }
	DT_SLONG GetLastPage() { return 11; }
	DT_SLONG GetW(DT_SLONG /*page*/) { return 850; }
	DT_SLONG GetH(DT_SLONG /*page*/) { return 900; }

private:

	void SetFormat();
	void SetFlow();
	void SetExtras();
};


void CDTTextDocDemo1::SetFormat()
{
	/* Style the fragment "D-Type Portable Unicode Text Rendering Engine" using our own style */
	const DT_TX_ATTRIBS myattr_GentiumBigBlueWithGlow[] =
	{
		{TX_ATTR_FONT_UNIQUEID, {"F0010_GENTI__NS0" /* Gentium font id */}},
		{TX_ATTR_FONT_WIDTH,    {"55" /* in doc units */}},
		{TX_ATTR_FONT_HEIGHT,   {"55" /* in doc units */}},
		{TX_ATTR_BODY,          {"1"}},
		{TX_ATTR_BODY_RGBT,     {"5A31A500"}},
		{TX_ATTR_SHADOW_RGBT,   {"AD87E800"}},
		{TX_ATTR_SHADOW_EP,     {"030A" /* effect=03, parameter=0A */}},
		{TX_ATTR_END,           {"" /* mandatory end-of-array */}}
	};
	TextDoc.SetAttribs(0, 60, myattr_GentiumBigBlueWithGlow);

	/* Style the word "quick" using our own outline style */
	const DT_TX_ATTRIBS myattr_outline[] =
	{
		{TX_ATTR_BODY_RGBT,         {"FFFFAA00"}},
		{TX_ATTR_OUTLINE_TREATMENT, {"1"}},
		{TX_ATTR_OUTLINE_RGBT,      {"00660000"}},
		{TX_ATTR_OUTLINE_THICKNESS, {"100" /* outline thickness in 1/100 doc units */}},
		{TX_ATTR_END,               {"" /* mandatory end-of-array */}}
	};
	TextDoc.SetAttribs(90, 95, myattr_outline);

	/* Style the fragment "Portable Unicode Text Rendering Engine" using our own style */
	const DT_TX_ATTRIBS myattr_GeorgiaItalicRed[] =
	{
		{TX_ATTR_FONT_UNIQUEID, {"F0011_GEORG__NS0"}},
		{TX_ATTR_SKEW_HOR,      {"-12" /* in degrees */}},
		{TX_ATTR_BODY_RGBT,     {"A0000000"}},
		{TX_ATTR_END,           {"" /* mandatory end-of-array */}}
	};
	TextDoc.SetAttribs(117, 155, myattr_GeorgiaItalicRed);

	/* Tab stop positions */
	const DT_TX_ATTRIBS myattr_tab[] =
	{
		{TX_ATTR_TABSTOP_POSITIONS, {"13107, 32768, 52428"}},
		{TX_ATTR_TABSTOP_FLAGS,     {"0, 1, 2"}},
		{TX_ATTR_END,               {"" /* mandatory end-of-array */}}
	};
	TextDoc.SetAttribs(0, 500, myattr_tab);

	/* Style the fragment "There are 12 pages in this document." using a predefined attrUnderline style */
	TextDoc.SetAttribs(166, 202, attrUnderline);

	/* Style "12 pages" using our own underline style */
	const DT_TX_ATTRIBS myattr_underline[] =
	{
		{TX_ATTR_EFFECT,              {"2" /* double underline effect */}},
		{TX_ATTR_UNDERLINE_RGBT,      {"00660000"}},
		{TX_ATTR_UNDERLINE_THICKNESS, {"300" /* underline thickness in 1/100 doc units */}},
		{TX_ATTR_UNDERLINE_SEGMENT,   {"10" /* underline segment */}},
		{TX_ATTR_END,                 {"" /* mandatory end-of-array */}}
	};
	TextDoc.SetAttribs(176, 184, myattr_underline);

	/* Style the words "Previous", "Next Page" and "C++" using a predefined attrBold style */
	TextDoc.SetAttribs(211, 219, attrBold);
	TextDoc.SetAttribs(224, 233, attrBold);
	TextDoc.SetAttribs(325, 328, attrBold);

	/* Style the words "Hindi", "Thai and "Arabic" using a predefined attrRedDark style */
	TextDoc.SetAttribs(382, 388, attrRedDark);
	TextDoc.SetAttribs(397, 400, attrRedDark);
	TextDoc.SetAttribs(414, 421, attrRedDark);

	/* Style the words "start.txt" and "/files/text/unicode_utf-8" using a predefined attrBold style */
	TextDoc.SetAttribs(463, 472, attrBold);
	TextDoc.SetAttribs(489, 514, attrBold);

	/* Style the fragment "The Eiffel Tower" and "Saturn" using our own style */
	const DT_TX_ATTRIBS myattr_FontNr4BigRedWithShadow[] =
	{
		{TX_ATTR_FONT_INDEX,    {"4" /* CMU Bright Bold Extended font index */}},
		{TX_ATTR_FONT_WIDTH,    {"55" /* in doc units */}},
		{TX_ATTR_FONT_HEIGHT,   {"55" /* in doc units */}},
		{TX_ATTR_BODY_RGBT,     {"A0000000"}},
		{TX_ATTR_SHADOW_RGBT,   {"CCCCCC00"}},
		{TX_ATTR_SHADOW_DX,     {"5" /* in doc units */}},
		{TX_ATTR_SHADOW_DY,     {"5" /* in doc units */}},
		{TX_ATTR_SHADOW_EP,     {"0308" /* effect=03, parameter=08 */}},
		{TX_ATTR_END,           {"" /* mandatory end-of-array */}}
	};
	TextDoc.SetAttribs(1245, 1261, myattr_FontNr4BigRedWithShadow);
	TextDoc.SetAttribs(4416, 4422, myattr_FontNr4BigRedWithShadow);

	/* Style the entire "The Eiffel Tower" chapter using a predefined attrJustified style */
	TextDoc.SetAttribs(1265, 4412, attrJustified);
}


void CDTTextDocDemo1::SetFlow()
{
	/* --------------------------- AUTOMATIC TEXT FLOW ---------------------------	*/

	DT_FLOAT transf_2x2_a[4] = {1.0, -0.3, 0.0, 1.4};
	DT_FLOAT transf_2x2_b[4] = {1.3, 0.1, -0.4, 0.9};
	DT_FLOAT transf_3x3_a[9] = {1.4, 0.0, 0.0, 0.0, 1.9, 0.0, 0.002, 0.000, 1.0};
	DT_FLOAT transf_3x3_b[9] = {0.4795, -0.0058, -180.0, 0.0000, 0.4550, -120.0, 0.000, -0.001, 1.0};

	DT_TX_TEXTFLOW_AREA textflow_areas[12] =
	{
		{ 10,  10, 800, 850, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{200,  10, 650, 630, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_2X2, {transf_2x2_a}},
		{ 10,  10, 880, 880, TX_CIRC,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B,  TX_TM_NULL, {DV_NULL}},
		{ 10,  10, 880, 880, TX_DIAM_HALF_B, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B,  TX_TM_NULL, {DV_NULL}},
		{ 10, 280, 600, 690, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_2X2, {transf_2x2_b}},
		{ 10,  10,1400, 480, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_3X3, {transf_3x3_a}},
		{500, 130, 800, 630, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_3X3, {transf_3x3_b}},
		{ 10,  10, 840, 880, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10,  10, 840, 880, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10,  10, 840, 880, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10,  10, 840, 880, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10,  10, 840, 880, TX_RECT,        TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}}
	};

	TextDoc.SetFlow(12, textflow_areas);
}


void CDTTextDocDemo1::SetExtras()
{
	TextDoc.SetNumericValueTX(TX_NVAL_ENABLE_BITS, (1 | 2 | 4 | 8));
	TextDoc.SetNumericValueTX(TX_NVAL_CONFIG_BITS, (1 | 16) /* inserted text to inherit styling/formatting attributes of previous text fragment + BiDi cursor support enabled */);

	/*
	same as the following deprecated code:

	DT_TX_DOC_PARAMS tx_params = {(1 | 2 | 4 | 8), (1 | 16), DV_NULL};
	TextDoc.SetParams(tx_params);
	*/

	const DT_UBYTE cursor_blend_arr[5] = {TX_BLEND_HIGHLIGHT_1, 255, 0, 0, 0};
	const DT_UBYTE select_blend_arr[5] = {TX_BLEND_HIGHLIGHT_1, 102, 204, 255, 0};

	TextDoc.SetHighlighter(cursor_blend_arr, select_blend_arr);
}
