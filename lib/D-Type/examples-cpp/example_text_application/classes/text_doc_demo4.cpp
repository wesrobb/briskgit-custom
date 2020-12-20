/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"


class CDTTextDocDemo4 : public CDTTextDoc
{
public:

	CDTTextDocDemo4(CDTEngineV8* engine, const DT_CHAR* filename)
	{
		const DT_TX_ATTRIBS myattr_SmallTextJustified[] =
		{
			{TX_ATTR_FONT_WIDTH,    {"13" /* in doc units */}},
			{TX_ATTR_FONT_HEIGHT,   {"13" /* in doc units */}},
			{TX_ATTR_SPACING_ROW,   {"4" /* in doc units */}},
			{TX_ATTR_ALIGN,         {"3" /* justified */}},
			{TX_ATTR_HYPHENATION,   {"1" /* EN */}},
			{TX_ATTR_END,           {"" /* mandatory end-of-array */}}
		};

		DT_STREAM_FILE(sd, filename);
		DT_STREAM_FILE(sd_fontmap, "config/fontmap.inf");

		if (TextDoc.Make(engine, TX_AUTO, &sd, TX_IMPORT_FULL_COMPACT, DV_NULL, myattr_SmallTextJustified, &sd_fontmap) < 0) return;

		SetFlow();
		SetExtras();
	}

public:

	DT_SLONG GetLastArea() { return 1; }
	DT_SLONG GetLastPage() { return 1; }
	DT_SLONG GetW(DT_SLONG /*page*/) { return 770; }
	DT_SLONG GetH(DT_SLONG /*page*/) { return 850; }

private:

	void SetFlow();
	void SetExtras();
};


void CDTTextDocDemo4::SetFlow()
{
	const DT_UBYTE edge_array[] = {20, 120, 0, 20, 114, 35, 20, 120, 40, 20, 116, 125, 25, 78, 135, 88, 160, 24, 108, 200, 32, 225, 110, 255};

	DT_TX_TEXTFLOW_AREA_EXTRA textflow_area_extra_0 = {DV_NULL, edge_array, edge_array, 24, 24, 0, 0};
	DT_TX_TEXTFLOW_AREA_EXTRA textflow_area_extra_1 = {DV_NULL, DV_NULL, DV_NULL, 0, 0, TX_VALIGN_JUSTIFIED, TX_VALIGN_JUSTIFIED};

	DT_TX_TEXTFLOW_AREA textflow_areas[2] =
	{
		{10, 10, 720, 800, TX_CUSTOM, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_EXTRA_NULL, {DV_NULL}},
		{10, 10, 720, 800, TX_RECT,   TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_EXTRA_NULL, {DV_NULL}}
	};

	textflow_areas[0].Extra = &textflow_area_extra_0;
	textflow_areas[1].Extra = &textflow_area_extra_1;

	TextDoc.SetFlow(2, textflow_areas);
}


void CDTTextDocDemo4::SetExtras()
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
