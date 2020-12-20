enum
{
	/* --- Ctrl --- */

	/* rule */
	Clip_empty = 0,
	Clip_rule,

	/* base */
	Clip_992001_c_01_ltr,
	Clip_992001_c_01_rtl,

	/* table*/
	Clip_990032_a_02_02,
	Clip_990032_a_02_01,
	Clip_990032_a_01_02,
	Clip_990032_a_01_01,

	/* --- Math --- */

	/* Cross */
	Clip_990005_a,
	Clip_990005_b,

	/* Brackets */
	Clip_990012_01,
	Clip_990012_02,
	Clip_990012_03,
	Clip_990012_04,
	Clip_990012_05,
	Clip_990012_06,
	Clip_990012_07,
	Clip_990012_08,
	Clip_990012_09,
	Clip_990012_10,
	Clip_990012_11,
	Clip_990012_12,
	Clip_990012_13,
	Clip_990012_14,
	Clip_990012_15,
	Clip_990012_16,
	Clip_990012_17,
	Clip_990012_18,
	Clip_990012_19,
	Clip_990012_20,

	/* VBars - Tight */
	Clip_990000_a,
	Clip_990000_b,
	Clip_990000_c,
	Clip_990000_d,
	Clip_990000_e,
	Clip_990000_f,

	/* VBars - Loose */
	Clip_990025_a,
	Clip_990025_b,
	Clip_990025_c,
	Clip_990025_d,
	Clip_990025_e,
	Clip_990025_f,

	/* Diagonals */
	Clip_990005_c,
	Clip_990005_d,
	Clip_990005_e,
	Clip_990005_f,

	/* Above */
	Clip_990013_a_01,
	Clip_990013_a_02,
	Clip_990013_a_03,
	Clip_990013_a_04,
	Clip_990013_b_01,
	Clip_990013_b_02,
	Clip_990013_b_03,
	Clip_990013_b_04,
	Clip_990013_c_01,

	/* Below */
	Clip_990014_a_01,
	Clip_990014_a_02,
	Clip_990014_a_03,
	Clip_990014_a_04,
	Clip_990014_b_01,
	Clip_990014_b_02,
	Clip_990014_b_03,
	Clip_990014_b_04,
	Clip_990014_c_01,

	/* Frac */
	Clip_990001_a,
	Clip_990001_b,
	Clip_990001_c,
	Clip_990002_a,
	Clip_990002_b,
	Clip_990002_c,

	/* Root */
	Clip_990003_b,
	Clip_990004_b,

	/* Sup */
	Clip_990010,
	Clip_990015,

	/* Sub */
	Clip_990020,
	Clip_990021,

	/* Above */
	Clip_990022,

	/* Below */
	Clip_990023,

	/* Summation Sign */
	Clip_990009_a,
	Clip_990008_a,

	/* Product Sign */
	Clip_990009_b,
	Clip_990008_b,

	/* Coproduct Sign */
	Clip_990009_c,
	Clip_990008_c,

	/* Union Sign */
	Clip_990009_d,
	Clip_990008_d,

	/* Intersection Sign */
	Clip_990009_e,
	Clip_990008_e,

	/* Logical Or Sign */
	Clip_990009_f,
	Clip_990008_f,

	/* Logical And Sign */
	Clip_990009_g,
	Clip_990008_g,

	/* SupSub */
	Clip_990011,
	Clip_990016,

	/* Integral */
	Clip_990007_a_01,
	Clip_990006_a_01,

	/* Contour Integral */
	Clip_990007_b_01,
	Clip_990006_b_01,

	/* Clockwise Integral */
	Clip_990007_c_01,
	Clip_990006_c_01,

	/* Clockwise Contour Integral */
	Clip_990007_d_01,
	Clip_990006_d_01,

	/* Anticlockwise Contour Integral */
	Clip_990007_e_01,
	Clip_990006_e_01,

	/* Double Integral */
	Clip_990007_a_02,
	Clip_990006_a_02,

	/* Surface Integral */
	Clip_990007_b_02,
	Clip_990006_b_02,

	/* Triple Integral */
	Clip_990007_a_03,
	Clip_990006_a_03,

	/* Volume Integral */
	Clip_990007_b_03,
	Clip_990006_b_03,

	/* Upright Integral */
	Clip_990019_a_01,
	Clip_990018_a_01,

	/* Upright Contour Integral */
	Clip_990019_b_01,
	Clip_990018_b_01,

	/* Upright Clockwise Integral */
	Clip_990019_c_01,
	Clip_990018_c_01,

	/* Upright Clockwise Contour Integral */
	Clip_990019_d_01,
	Clip_990018_d_01,

	/* Upright Anticlockwise Contour Integral */
	Clip_990019_e_01,
	Clip_990018_e_01,

	/* Upright Double Integral */
	Clip_990019_a_02,
	Clip_990018_a_02,

	/* Upright Surface Integral */
	Clip_990019_b_02,
	Clip_990018_b_02,

	/* Upright Triple Integral */
	Clip_990019_a_03,
	Clip_990018_a_03,

	/* Upright Volume Integral */
	Clip_990019_b_03,
	Clip_990018_b_03,

	/* Anticlockwise Integral */
	Clip_990007_f_01,
	Clip_990006_f_01,

	/* Upright Anticlockwise Integral */
	Clip_990019_f_01,
	Clip_990018_f_01,

	/* Matrix */
	Clip_990031_a_01_01,
	Clip_990031_b_01_01,
	Clip_990031_c_01_01,
	Clip_990031_d_01_01,
	Clip_990031_e_01_01,
	Clip_990031_f_01_01,

	Clip_Count
};


const DT_CHAR* DTypeClips[] =
{
	/* --- Ctrl --- */

	/* rule */
	"empty",
	"rule",

	/* base */
	"992001-c-01-ltr",
	"992001-c-01-rtl",

	/* table*/
	"990032-a-02-02",
	"990032-a-02-01",
	"990032-a-01-02",
	"990032-a-01-01",

	/* --- Math --- */

	/* Cross */
	"990005-a",
	"990005-b",

	/* Brackets */
	"990012-01",
	"990012-02",
	"990012-03",
	"990012-04",
	"990012-05",
	"990012-06",
	"990012-07",
	"990012-08",
	"990012-09",
	"990012-10",
	"990012-11",
	"990012-12",
	"990012-13",
	"990012-14",
	"990012-15",
	"990012-16",
	"990012-17",
	"990012-18",
	"990012-19",
	"990012-20",

	/* VBars - Tight */
	"990000-a",
	"990000-b",
	"990000-c",
	"990000-d",
	"990000-e",
	"990000-f",

	/* VBars - Loose */
	"990025-a",
	"990025-b",
	"990025-c",
	"990025-d",
	"990025-e",
	"990025-f",

	/* Diagonals */
	"990005-c",
	"990005-d",
	"990005-e",
	"990005-f",

	/* Above */
	"990013-a-01",
	"990013-a-02",
	"990013-a-03",
	"990013-a-04",
	"990013-b-01",
	"990013-b-02",
	"990013-b-03",
	"990013-b-04",
	"990013-c-01",

	/* Below */
	"990014-a-01",
	"990014-a-02",
	"990014-a-03",
	"990014-a-04",
	"990014-b-01",
	"990014-b-02",
	"990014-b-03",
	"990014-b-04",
	"990014-c-01",

	/* Frac */
	"990001-a",
	"990001-b",
	"990001-c",
	"990002-a",
	"990002-b",
	"990002-c",

	/* Root */
	"990003-b",
	"990004-b",

	/* Sup */
	"990010",
	"990015",

	/* Sub */
	"990020",
	"990021",

	/* Above */
	"990022",

	/* Below */
	"990023",

	/* Summation Sign */
	"990009-a",
	"990008-a",

	/* Product Sign */
	"990009-b",
	"990008-b",

	/* Coproduct Sign */
	"990009-c",
	"990008-c",

	/* Union Sign */
	"990009-d",
	"990008-d",

	/* Intersection Sign */
	"990009-e",
	"990008-e",

	/* Logical Or Sign */
	"990009-f",
	"990008-f",

	/* Logical And Sign */
	"990009-g",
	"990008-g",

	/* SupSub */
	"990011",
	"990016",

	/* Integral */
	"990007-a-01",
	"990006-a-01",

	/* Contour Integral */
	"990007-b-01",
	"990006-b-01",

	/* Clockwise Integral */
	"990007-c-01",
	"990006-c-01",

	/* Clockwise Contour Integral */
	"990007-d-01",
	"990006-d-01",

	/* Anticlockwise Contour Integral */
	"990007-e-01",
	"990006-e-01",

	/* Double Integral */
	"990007-a-02",
	"990006-a-02",

	/* Surface Integral */
	"990007-b-02",
	"990006-b-02",

	/* Triple Integral */
	"990007-a-03",
	"990006-a-03",

	/* Volume Integral */
	"990007-b-03",
	"990006-b-03",

	/* Upright Integral */
	"990019-a-01",
	"990018-a-01",

	/* Upright Contour Integral */
	"990019-b-01",
	"990018-b-01",

	/* Upright Clockwise Integral */
	"990019-c-01",
	"990018-c-01",

	/* Upright Clockwise Contour Integral */
	"990019-d-01",
	"990018-d-01",

	/* Upright Anticlockwise Contour Integral */
	"990019-e-01",
	"990018-e-01",

	/* Upright Double Integral */
	"990019-a-02",
	"990018-a-02",

	/* Upright Surface Integral */
	"990019-b-02",
	"990018-b-02",

	/* Upright Triple Integral */
	"990019-a-03",
	"990018-a-03",

	/* Upright Volume Integral */
	"990019-b-03",
	"990018-b-03",

	/* Anticlockwise Integral */
	"990007-f-01",
	"990006-f-01",

	/* Upright Anticlockwise Integral */
	"990019-f-01",
	"990018-f-01",

	/* Matrix */
	"990031-a-01-01",
	"990031-b-01-01",
	"990031-c-01-01",
	"990031-d-01-01",
	"990031-e-01-01",
	"990031-f-01-01"
};


void CDTEngineV8::DestroyClips()
{
	if (Clip_Count > DEF_ENGINE_BUILDER_CLIPS) return;

	DT_SWORD i;

	for (i = 0; i < Clip_Count; i++)
	{
		if (Clip[i] == DV_NULL) continue;
		pdDocExt(Clip[i]);
		Clip[i] = DV_NULL;
	}
}


bool CDTEngineV8::MakeClips()
{
	if (Clip_Count > DEF_ENGINE_BUILDER_CLIPS) { /*printf("Clip_Count=%d, DEF_ENGINE_BUILDER_CLIPS=%d\n", Clip_Count, DEF_ENGINE_BUILDER_CLIPS);*/ return false; }

	bool ret = true;
	DT_SWORD i;

	//printf("Clip_Count=%d\n", Clip_Count);

	for (i = 0; i < Clip_Count; i++)
	{
		Clip[i] = DV_NULL;
		if (pdDocIni(&Clip[i], GetPdEngine()) != 1) { /*printf("Clip %d failed (1).\n", i);*/ ret = false; continue; }
		pdDocSetInternalFlags(Clip[i], 1); /* discard descriptions */
		DT_STRING_CHARS(filename, 256, "library/text/dynamic/"); dtStrConcatChars(&filename, DTypeClips[i]); dtStrConcatChars(&filename, ".pdc");
		DT_STREAM_FILE(sd, filename.str);
		if (pdDocAppendFromStream(Clip[i], 0, 0, 0, &sd) != 1) { /*printf("Clip %d failed (2).\n", i);*/ ret = false; continue; }
		//printf("Clip %d loaded.\n", i);
	}

	return ret;
}
