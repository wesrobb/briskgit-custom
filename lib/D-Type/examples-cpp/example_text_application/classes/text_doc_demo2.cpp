/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"


class CDTTextDocDemo2 : public CDTTextDoc
{
public:

	CDTTextDocDemo2(CDTEngineV8* engine, const DT_CHAR* filename)
	{
		DT_STREAM_FILE(sd, filename);
		DT_STREAM_FILE(sd_fontmap, "config/fontmap.inf");

		if (TextDoc.Make(engine, TX_AUTO, &sd, TX_IMPORT_FULL_COMPACT, NULL, NULL, &sd_fontmap) < 0) return;

		SetFlow();
		SetExtras();
	}

public:

	DT_SLONG GetLastArea() { return 6; }
	DT_SLONG GetLastPage() { return 6; }
	DT_SLONG GetW(DT_SLONG /*page*/) { return 770; }
	DT_SLONG GetH(DT_SLONG /*page*/) { return 850; }

private:

	void SetFlow();
	void SetExtras();
};


void CDTTextDocDemo2::SetFlow()
{
	DT_TX_TEXTFLOW_AREA textflow_areas[7] =
	{
		{ 10, 10, 720, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10, 10, 720, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10, 10, 720, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10, 10, 720, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10, 10, 720, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10, 10, 720, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_COL_TB_RL, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}},
		{ 10, 10, 720, 800, TX_RECT, TX_TYPOROW, 0, TX_DIR_COL_TB_RL, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}}
	};

	TextDoc.SetFlow(7, textflow_areas);
}


void CDTTextDocDemo2::SetExtras()
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
