/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"


class CDTTextDocDemo5 : public CDTTextDoc
{
public:

	CDTTextDocDemo5(CDTEngineV8* engine, const DT_CHAR* filename)
	{
		const DT_TX_ATTRIBS myattr_MediumText[] =
		{
			{TX_ATTR_FONT_WIDTH,    {"16" /* in doc units */}},
			{TX_ATTR_FONT_HEIGHT,   {"16" /* in doc units */}},
			{TX_ATTR_END,           {"" /* mandatory end-of-array */}}
		};

		DT_STREAM_FILE(sd, filename);
		DT_STREAM_FILE(sd_fontmap, "config/fontmap.inf");

		if (TextDoc.Make(engine, TX_AUTO, &sd, TX_IMPORT_FULL_COMPACT, DV_NULL, myattr_MediumText, &sd_fontmap) < 0) return;

		SetFlow();
		SetExtras();
	}

public:

	DT_SLONG GetLastArea() { return 9; }
	DT_SLONG GetLastPage() { return 4; }
	DT_SLONG GetW(DT_SLONG /*page*/) { return 770; }
	DT_SLONG GetH(DT_SLONG /*page*/) { return 850; }

private:

	void SetFlow();
	void SetExtras();
};


void CDTTextDocDemo5::SetFlow()
{
	DT_TX_TEXTFLOW_AREA textflow_areas[10] =
	{
		{  10, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{ 350, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{  10, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{ 350, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{  10, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{ 350, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{  10, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{ 350, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{  10, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}},
		{ 350, 10, 290, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_MIXED_B, TX_TM_NULL, {DV_NULL}}
	};

	DT_SLONG pages[10] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4};

	TextDoc.SetFlow(10, textflow_areas, pages);
}


void CDTTextDocDemo5::SetExtras()
{
	TextDoc.SetNumericValueTX(TX_NVAL_ENABLE_BITS, (1 | 2 | 4 | 8));
	TextDoc.SetNumericValueTX(TX_NVAL_CONFIG_BITS, 1 /* inserted text to inherit styling/formatting attributes of previous text fragment*/);
	TextDoc.SetNumericValuePD(PD_NVAL_CONTROL_GLYPHS, 1);
	TextDoc.SetNumericValuePD(PD_NVAL_CONTROL_ALGORITHMS, 0);

	/*
	same as the following deprecated code:

	DT_PD_DOC_PARAMS pd_params = {4, 6, 10, 10, 2, 0, 1, 0};
	DT_TX_DOC_PARAMS tx_params = {(1 | 2 | 4 | 8), 1, &pd_params};
	TextDoc.SetParams(tx_params);
	*/
}
