/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include "dtype.h"


class CDTTextDocDemo3 : public CDTTextDoc
{
public:

	CDTTextDocDemo3(CDTEngineV8* engine, const DT_CHAR* filename)
	{
		DT_STREAM_FILE(sd, filename);
		DT_STREAM_FILE(sd_fontmap, "config/fontmap.inf");

		DT_TX_TEXTFLOW_AREA textflow_area = {10, 10, 800, 1000, TX_RECT, TX_TYPOROW, 0, TX_DIR_ROW_LR_TB, TX_LAY_DEVICE_B, TX_TM_NULL, {DV_NULL}};
		if (TextDoc.Make(engine, TX_AUTO, &sd, TX_IMPORT_FULL_COMPACT, &textflow_area, NULL, &sd_fontmap) < 0) return;

		EmbedPowerDocObjects(engine->GetPdEngine());
		SetExtras();
	}

public:

	DT_SLONG GetLastArea() { return 0; }
	DT_SLONG GetLastPage() { return 0; }
	DT_SLONG GetW(DT_SLONG /*page*/) { return 850; }
	DT_SLONG GetH(DT_SLONG /*page*/) { return 1000; }

private:

	void EmbedPowerDocObjects(DT_PDENGINE power_engine);
	void SetExtras();
};


void CDTTextDocDemo3::EmbedPowerDocObjects(DT_PDENGINE power_engine)
{
	DT_PDDOC powerdoc;

	/* Initialize a PowerDoc */
	if (pdDocIni(&powerdoc, power_engine) == 0) return;

	/* Open the saturn-photo.pdc document which contains a picture of the planet Saturn */
	DT_STREAM_FILE(sd1, "../../files/powerdoc/saturn-photo.pdc");
	if (pdDocAppendFromStream(powerdoc, 0, 0, 0, &sd1) == 1)
	{
		/* The size of the picture is 300 x 224 document units */
		DT_SLONG page = 0;
		DT_SLONG x = 0, y = 200; /* Origin point */
		DT_SLONG metrics_x_arr[3] = {140 /* font width */, 300 /* base font width */, 330 /* Advance width */};
		DT_SLONG metrics_y_arr[3] = {140 /* font height */, 300 /* base font height */, 0 /* Presently unused */};
		DT_RECT_SLONG extent = {0 - x, 0 - y, 300 - x, 224 - y}; /* Bounding box relative to (x, y) */
		/* Make a text fragment object that we can paste into text */
		DT_ID_SLONG obj = pdxPageToText(powerdoc, page, x, y, &extent, metrics_x_arr, metrics_y_arr, 3);

		/* Paste the text fragment object at character position 7 */
		TextDoc.SetSelect(7);
		TextDoc.PasteViaPowerObj(powerdoc, obj);
	}

	pdDocErase(powerdoc);

	/* Open the saturn-table.pdc document which contains a table with Saturn's specifications */
	DT_STREAM_FILE(sd2, "../../files/powerdoc/saturn-table.pdc");
	if (pdDocAppendFromStream(powerdoc, 0, 0, 0, &sd2) == 1)
	{
		/* The size of the table is 600 x 200 document units */
		DT_SLONG page = 0;
		DT_SLONG x = 0, y = 200; /* Origin point */
		DT_SLONG metrics_x_arr[3] = {100 /* font width */, 100 /* base font width */, 600 /* Advance width */};
		DT_SLONG metrics_y_arr[3] = {100 /* font height */, 100 /* base font height */, 0 /* Presently unused */};
		DT_RECT_SLONG extent = {0 - x, 0 - y, 600 - x, 200 - y}; /* Bounding box relative to (x, y) */
		/* Make a text fragment object that we can paste into text */
		DT_ID_SLONG obj = pdxPageToText(powerdoc, page, x, y, &extent, metrics_x_arr, metrics_y_arr, 3);

		/* Paste the text fragment object at character position 323 */
		DT_SLONG first_char = 323;
		TextDoc.SetSelect(first_char);
		TextDoc.PasteViaPowerObj(powerdoc, obj);

		/* Center table (must include the CR+LF characters) */
		TextDoc.SetAttribs(first_char, first_char + 3, attrCenterFull);
	}

	/* Destroy a PowerDoc */
	pdDocExt(powerdoc);

	/* Move cursor to the beginning of the text */
	TextDoc.SetSelect(0);
}


void CDTTextDocDemo3::SetExtras()
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
