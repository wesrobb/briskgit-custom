/*
 *
 * Copyright (C) D-Type Solutions - All Rights Reserved
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 *
 * This file is part of D-Type Engine and may only be used, modified
 * and distributed under the terms of the applicable license agreement
 * For conditions of distribution and use, see license.txt
 *
 * ANY VIOLATIONS OF THE ABOVE LICENSE AGREEMENT WILL BE PROSECUTED TO
 * THE MAXIMUM EXTENT POSSIBLE UNDER THE LAW. NO VENDOR, DISTRIBUTOR,
 * DEALER, RETAILER, SALES PERSON OR OTHER PERSON IS AUTHORIZED TO MODIFY
 * THIS AGREEMENT OR TO MAKE ANY WARRANTY, REPRESENTATION OR PROMISE WHICH
 * IS DIFFERENT THAN, OR IN ADDITION TO, THIS AGREEMENT.
 *
 */


#ifndef DTYPE_DTYPELAY_H
#define DTYPE_DTYPELAY_H


#ifdef __cplusplus
DT_EXTERN {
#endif


/* ------------------------- D-Type Text Layout Extension ------------------------- */

typedef struct DT_LXLAYOUT_STRUCT* DT_LXLAYOUT;
typedef struct DT_LXCACHE_STRUCT* DT_LXCACHE;
typedef struct DT_LXHYPHEN_STRUCT* DT_LXHYPHEN;


/* Structures */


/* Layout Extension's glyph structure describing a single positioned glyph */
typedef struct
{
	DT_ID_ULONG glyph_index; /* font dependent index of the glyph to be rendered */
	DT_SLONG char_index;     /* index to map the glyph back to the input text array */
	                         /* (not char_code but character's position in the input text array) */
	DT_SLONG x, y;           /* x and y coordinate of the glyph in font units */
	DT_UBYTE use_xy;         /* tells whether (x, y) members should be respected, always set to 1 */
	DT_UBYTE flag;           /* special purpose internal flag, currently set to 0 */
	DT_UBYTE reserved_1;     /* reserved for future, currently set to 0 */
	DT_UBYTE reserved_2;     /* reserved for future, currently set to 0 */

} LX_GLYPH;


#define LX_INFO_LAYOUT_HOR 1
#define LX_INFO_LAYOUT_VER 2

#define LX_UNICODE_BIDI   1
#define LX_UNICODE_SCRIPT 2

#define LX_DIRECTION_LTR 0
#define LX_DIRECTION_RTL 1
#define LX_DIRECTION_TTB 2
#define LX_DIRECTION_BTT 3

#define LX_ATTRIB_PREVENT_BREAK_FLAG      1
#define LX_ATTRIB_TYPE                    2
#define LX_ATTRIB_MIRROR                  3
#define LX_ATTRIB_PUNCTUATION_COMMON      4
#define LX_ATTRIB_PUNCTUATION_OTHER       5
#define LX_ATTRIB_DASH                    6
#define LX_ATTRIB_SLASH                   7
#define LX_ATTRIB_SEPARATOR               8
#define LX_ATTRIB_VOWEL                   9
#define LX_ATTRIB_ZERO_WIDTH_SPACE        10
#define LX_ATTRIB_HYPHEN_SOFT             11
#define LX_ATTRIB_LETTER_ARABIC           12
#define LX_ATTRIB_LETTER_ARABIC_NO_MEDIAL 13
#define LX_ATTRIB_CLOSING_BRACKET         14
#define LX_ATTRIB_TERMINATOR              15
#define LX_ATTRIB_MIRROR_AND_BREAK_FLAG   16
#define LX_ATTRIB_CTRLCHAR_TAB            1001
#define LX_ATTRIB_CTRLCHAR_SPACE          1002
#define LX_ATTRIB_CTRLCHAR_NEWAREA        1003
#define LX_ATTRIB_CTRLCHAR_NEWLINE        1004
#define LX_ATTRIB_CTRLCHAR_NEWLINE2       1005
#define LX_ATTRIB_CTRLCHAR_HYPHEN         1006
#define LX_ATTRIB_CTRLCHAR_ELLIPSIS       1007
#define LX_ATTRIB_CTRLCHAR_VERTAB         1008
#define LX_ATTRIB_BITMASK_BREAK_FLAG      2001
#define LX_ATTRIB_RANGE_WORDBRK_MIN       3001
#define LX_ATTRIB_RANGE_WORDBRK_MAX       3002
#define LX_ATTRIB_RANGE_PARABRK_MIN       3003
#define LX_ATTRIB_RANGE_PARABRK_MAX       3004

#define LX_FORMAT_UBYTE     1
#define LX_FORMAT_UWORD_LE  2
#define LX_FORMAT_UWORD_BE -2
#define LX_FORMAT_UTRIO_LE  3
#define LX_FORMAT_UTRIO_BE -3
#define LX_FORMAT_ULONG_LE  4
#define LX_FORMAT_ULONG_BE -4
#define LX_FORMAT_UWORD     102
#define LX_FORMAT_ULONG     104
#define LX_FORMAT_UTF16     124


enum
{
	/* public scripts */
	zyyyScriptCode = 0, /* Common */
	zinhScriptCode = 1, /* Inherited */
	qaaiScriptCode = zinhScriptCode, /* manually added alias, for API stability */
	arabScriptCode = 2, /* Arabic */ /**/
	armnScriptCode = 3, /* Armenian */
	bengScriptCode = 4, /* Bengali */ /**/
	bopoScriptCode = 5, /* Bopomofo */
	cherScriptCode = 6, /* Cherokee */
	coptScriptCode = 7, /* Coptic */
	cyrlScriptCode = 8, /* Cyrillic */
	dsrtScriptCode = 9, /* Deseret */
	devaScriptCode = 10, /* Devanagari */ /**/ /* (Hindi, Sanskrit) */
	ethiScriptCode = 11, /* Ethiopic */
	georScriptCode = 12, /* Georgian */
	gothScriptCode = 13, /* Gothic */
	grekScriptCode = 14, /* Greek */
	gujrScriptCode = 15, /* Gujarati */ /**/
	guruScriptCode = 16, /* Gurmukhi */ /**/
	haniScriptCode = 17, /* Han */ /**/
	hangScriptCode = 18, /* Hangul */ /**/
	hebrScriptCode = 19, /* Hebrew */ /**/
	hiraScriptCode = 20, /* Hiragana */
	kndaScriptCode = 21, /* Kannada */ /**/
	kanaScriptCode = 22, /* Katakana */
	khmrScriptCode = 23, /* Khmer */ /**/
	laooScriptCode = 24, /* Lao */
	latnScriptCode = 25, /* Latin */ /**/
	mlymScriptCode = 26, /* Malayalam */ /**/
	mongScriptCode = 27, /* Mongolian */
	mymrScriptCode = 28, /* Myanmar */
	ogamScriptCode = 29, /* Ogham */
	italScriptCode = 30, /* Old Italic */
	oryaScriptCode = 31, /* Oriya */ /**/
	runrScriptCode = 32, /* Runic */
	sinhScriptCode = 33, /* Sinhala */ /**/
	syrcScriptCode = 34, /* Syriac */
	tamlScriptCode = 35, /* Tamil */ /**/
	teluScriptCode = 36, /* Telugu */ /**/
	thaaScriptCode = 37, /* Thaana */
	thaiScriptCode = 38, /* Thai */ /**/
	tibtScriptCode = 39, /* Tibetan */ /**/
	cansScriptCode = 40, /* Unified Canadian Aboriginal Symbols */
	yiiiScriptCode = 41, /* Yi syllables */
	tglgScriptCode = 42, /* Tagalog */
	hanoScriptCode = 43, /* Hanunoo */
	buhdScriptCode = 44, /* Buhid */
	tagbScriptCode = 45, /* Tagbanwa */
	braiScriptCode = 46, /* Braille New script in Unicode 4 */
	cprtScriptCode = 47, /* Cypriot New script in Unicode 4 */
	limbScriptCode = 48, /* Limbu New script in Unicode 4 */
	linbScriptCode = 49, /* Linear B New script in Unicode 4 */
	osmaScriptCode = 50, /* Osmanya New script in Unicode 4 */
	shawScriptCode = 51, /* Shavian New script in Unicode 4 */
	taleScriptCode = 52, /* Tai Le New script in Unicode 4 */
	ugarScriptCode = 53, /* Ugaritic New script in Unicode 4 */
	hrktScriptCode = 54, /* Katakana or Hiragana New script code in Unicode 4.0.1 */
	bugiScriptCode = 55,
	glagScriptCode = 56,
	kharScriptCode = 57,
	syloScriptCode = 58,
	taluScriptCode = 59,
	tfngScriptCode = 60,
	xpeoScriptCode = 61,
	baliScriptCode = 62,
	batkScriptCode = 63,
	blisScriptCode = 64,
	brahScriptCode = 65,
	chamScriptCode = 66,
	cirtScriptCode = 67,
	cyrsScriptCode = 68,
	egydScriptCode = 69,
	egyhScriptCode = 70,
	egypScriptCode = 71,
	geokScriptCode = 72,
	hansScriptCode = 73,
	hantScriptCode = 74,
	hmngScriptCode = 75,
	hungScriptCode = 76,
	indsScriptCode = 77,
	javaScriptCode = 78,
	kaliScriptCode = 79,
	latfScriptCode = 80,
	latgScriptCode = 81,
	lepcScriptCode = 82,
	linaScriptCode = 83,
	mandScriptCode = 84,
	mayaScriptCode = 85,
	meroScriptCode = 86,
	nkooScriptCode = 87,
	orkhScriptCode = 88,
	permScriptCode = 89,
	phagScriptCode = 90,
	phnxScriptCode = 91,
	plrdScriptCode = 92,
	roroScriptCode = 93,
	saraScriptCode = 94,
	syreScriptCode = 95,
	syrjScriptCode = 96,
	syrnScriptCode = 97,
	tengScriptCode = 98,
	vaiiScriptCode = 99,
	vispScriptCode = 100,
	xsuxScriptCode = 101,
	zxxxScriptCode = 102,
	zzzzScriptCode = 103,
	cariScriptCode = 104,
	jpanScriptCode = 105,
	lanaScriptCode = 106,
	lyciScriptCode = 107,
	lydiScriptCode = 108,
	olckScriptCode = 109,
	rjngScriptCode = 110,
	saurScriptCode = 111,
	sgnwScriptCode = 112,
	sundScriptCode = 113,
	moonScriptCode = 114,
	mteiScriptCode = 115,
	armiScriptCode = 116,
	avstScriptCode = 117,
	cakmScriptCode = 118,
	koreScriptCode = 119,
	kthiScriptCode = 120,
	maniScriptCode = 121,
	phliScriptCode = 122,
	phlpScriptCode = 123,
	phlvScriptCode = 124,
	prtiScriptCode = 125,
	samrScriptCode = 126,
	tavtScriptCode = 127,
	zmthScriptCode = 128,
	zsymScriptCode = 129,
	bamuScriptCode = 130,
	lisuScriptCode = 131,
	nkgbScriptCode = 132,
	sarbScriptCode = 133,
	bassScriptCode = 134,
	duplScriptCode = 135,
	elbaScriptCode = 136,
	granScriptCode = 137,
	kpelScriptCode = 138,
	lomaScriptCode = 139,
	mendScriptCode = 140,
	mercScriptCode = 141,
	narbScriptCode = 142,
	nbatScriptCode = 143,
	palmScriptCode = 144,
	sindScriptCode = 145,
	waraScriptCode = 146,
	afakScriptCode = 147,
	jurcScriptCode = 148,
	mrooScriptCode = 149,
	nshuScriptCode = 150,
	shrdScriptCode = 151,
	soraScriptCode = 152,
	takrScriptCode = 153,
	tangScriptCode = 154,
	woleScriptCode = 155,
	hluwScriptCode = 156,
	khojScriptCode = 157,
	tirhScriptCode = 158,
	aghbScriptCode = 159,
	mahjScriptCode = 160,
	ahomScriptCode = 161,
	hatrScriptCode = 162,
	modiScriptCode = 163,
	multScriptCode = 164,
	paucScriptCode = 165,
	siddScriptCode = 166,

	/* private scripts */
	p016ScriptCode = 239,
	p015ScriptCode = 240,
	p014ScriptCode = 241,
	p013ScriptCode = 242,
	p012ScriptCode = 243,
	p011ScriptCode = 244,
	p010ScriptCode = 245,
	p009ScriptCode = 246,
	p008ScriptCode = 247,
	p007ScriptCode = 248,
	p006ScriptCode = 249,
	p005ScriptCode = 250,
	p004ScriptCode = 251,
	p003ScriptCode = 252,
	p002ScriptCode = 253,
	p001ScriptCode = 254,

	/* any script */
	a000ScriptCode = 255,

	/* counts */
	scriptCodeCountPublic = 167,
	scriptCodeCountPrivate = 16,
	scriptCodeCountUnused = 73,
	scriptCodeCount = 256
};


enum
{
	nullLanguageCode = 0,
	araLanguageCode = 1,
	asmLanguageCode = 2,
	benLanguageCode = 3,
	farLanguageCode = 4,
	gujLanguageCode = 5,
	hinLanguageCode = 6,
	iwrLanguageCode = 7,
	jiiLanguageCode = 8,
	janLanguageCode = 9,
	kanLanguageCode = 10,
	kokLanguageCode = 11,
	korLanguageCode = 12,
	kshLanguageCode = 13,
	malLanguageCode = 14,
	marLanguageCode = 15,
	mlrLanguageCode = 16,
	mniLanguageCode = 17,
	oriLanguageCode = 18,
	sanLanguageCode = 19,
	sndLanguageCode = 20,
	snhLanguageCode = 21,
	syrLanguageCode = 22,
	tamLanguageCode = 23,
	telLanguageCode = 24,
	thaLanguageCode = 25,
	urdLanguageCode = 26,
	zhpLanguageCode = 27,
	zhsLanguageCode = 28,
	zhtLanguageCode = 29,
	afkLanguageCode = 30,
	belLanguageCode = 31,
	bgrLanguageCode = 32,
	catLanguageCode = 33,
	cheLanguageCode = 34,
	copLanguageCode = 35,
	csyLanguageCode = 36,
	danLanguageCode = 37,
	deuLanguageCode = 38,
	dznLanguageCode = 39,
	ellLanguageCode = 40,
	engLanguageCode = 41,
	espLanguageCode = 42,
	etiLanguageCode = 43,
	euqLanguageCode = 44,
	finLanguageCode = 45,
	fraLanguageCode = 46,
	gaeLanguageCode = 47,
	hauLanguageCode = 48,
	hrvLanguageCode = 49,
	hunLanguageCode = 50,
	hyeLanguageCode = 51,
	indLanguageCode = 52,
	itaLanguageCode = 53,
	khmLanguageCode = 54,
	mngLanguageCode = 55,
	mtsLanguageCode = 56,
	nepLanguageCode = 57,
	nldLanguageCode = 58,
	pasLanguageCode = 59,
	plkLanguageCode = 60,
	ptgLanguageCode = 61,
	romLanguageCode = 62,
	rusLanguageCode = 63,
	skyLanguageCode = 64,
	slvLanguageCode = 65,
	sqiLanguageCode = 66,
	srbLanguageCode = 67,
	sveLanguageCode = 68,
	tibLanguageCode = 69,
	trkLanguageCode = 70,
	welLanguageCode = 71,

	languageCodeCount = 72
};


enum
{
	textAuto       =  0, /* Auto-detect */
	textAnsi       =  1, /* Ansi */
	textUcs2_le    =  2, /* UCS-2 Little Endian (deprecated in favor of textUtf16_le) */
	textUcs2_be    =  3, /* UCS-2 Big Endian (deprecated in favor of textUtf16_be) */
	textUtf8       =  4, /* UTF-8 */
	textUcs4_le    =  5, /* UCS-4 Little Endian */
	textUcs4_be    =  6, /* UCS-4 Big Endian */
	textUtf16_le   =  7, /* UTF-16 Little Endian */
	textUtf16_be   =  8, /* UTF-16 Big Endian */
	textUtf32_le   =  9, /* UTF-32 Little Endian */
	textUtf32_be   = 10, /* UTF-32 Big Endian */
	textBuffer3_le = 11, /* Like UCS-2 or UCS-4 Little Endian, but 3 bytes per char */
	textBuffer3_be = 12, /* Like UCS-2 or UCS-4 Big Endian, but 3 bytes per char */
	textTypeCount  = 13
};


/* D-Type Text Layout Extension Functions */

/* Initialize text layout instance */
DT_SWORD DTYPE_APICALL lxLayoutIni(DT_LXLAYOUT* layout, DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SLONG script_code);
/* Initialize text layout instance */
DT_SWORD DTYPE_APICALL lxLayoutIniPlus(DT_LXLAYOUT* layout, DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SLONG script_code, DT_ID_SLONG language_code, DT_SLONG typo_flags);
/* Initialize text layout instance */
DT_SWORD DTYPE_APICALL lxLayoutIniPlus2(DT_LXLAYOUT* layout, DT_DTENGINE engine, DT_ID_UBYTE engine_code, DT_ID_SWORD font_index, DT_ID_SLONG script_code, DT_ID_SLONG language_code, DT_SLONG typo_flags);
/* Get various text layout info */
DT_ID_ULONG DTYPE_APICALL lxLayoutGetInfo(DT_LXLAYOUT layout, DT_ID_ULONG info_id, DT_SLONG reserved);
/* Apply text layout instance to a run of text */
DT_SLONG DTYPE_APICALL lxLayoutApply(DT_LXLAYOUT layout, const DT_ID_UWORD chars[], DT_SLONG nr_of_chars, DT_ID_UBYTE direction, LX_GLYPH glyph_arr[], DT_SLONG max_glyphs);
/* Apply text layout instance to a run of text */
DT_SLONG DTYPE_APICALL lxLayoutApplyPlus(DT_LXLAYOUT layout, DT_SLONG offset, DT_SLONG count, DT_SLONG nr_of_chars, const DT_UBYTE* chars, DT_ID_SWORD chars_format, DT_ID_UBYTE direction, LX_GLYPH glyph_arr[], DT_SLONG max_glyphs);
/* Deinitialize (destroy) text layout instance */
void DTYPE_APICALL lxLayoutExt(DT_LXLAYOUT layout);

/* Initialize layout cache */
DT_SWORD DTYPE_APICALL lxCacheIni(DT_LXCACHE* layout_cache, DT_DTENGINE engine, DT_SLONG reserved);
/* Obtain text layout instance from the layout cache */
DT_SWORD DTYPE_APICALL lxCacheObtainLayout(DT_LXCACHE layout_cache, DT_ID_SWORD font_index, DT_ID_SLONG script_code, DT_LXLAYOUT* layout);
/* Obtain text layout instance from the layout cache */
DT_SWORD DTYPE_APICALL lxCacheObtainLayoutPlus(DT_LXCACHE layout_cache, DT_ID_SWORD font_index, DT_ID_SLONG script_code, DT_ID_SLONG language_code, DT_SLONG typo_flags, DT_LXLAYOUT* layout);
/* Obtain text layout instance from the layout cache */
DT_SWORD DTYPE_APICALL lxCacheObtainLayoutPlus2(DT_LXCACHE layout_cache, DT_ID_UBYTE engine_code, DT_ID_SWORD font_index, DT_ID_SLONG script_code, DT_ID_SLONG language_code, DT_SLONG typo_flags, DT_LXLAYOUT* layout);
/* Clear layout cache of text layout instances */
void DTYPE_APICALL lxCacheClear(DT_LXCACHE layout_cache, DT_ID_SWORD font_index, DT_ID_SLONG script_code);
/* Clear layout cache of text layout instances */
void DTYPE_APICALL lxCacheClearPlus(DT_LXCACHE layout_cache, DT_ID_SWORD font_index, DT_ID_SLONG script_code, DT_ID_SLONG language_code, DT_SLONG typo_flags);
/* Deinitialize (destroy) layout cache */
void DTYPE_APICALL lxCacheExt(DT_LXCACHE layout_cache);

/* Unicode Bidirectional (BiDi) algorithm */
DT_SWORD DTYPE_APICALL lxBidiApply(const DT_ID_UWORD chars[], DT_SLONG nr_of_chars, DT_ID_UBYTE types[], DT_ID_UBYTE levels[], DT_ID_UBYTE start_level, DT_SWORD flags);
/* Unicode Bidirectional (BiDi) algorithm */
DT_SWORD DTYPE_APICALL lxBidiApplyPlus(DT_SLONG nr_of_chars, const DT_UBYTE* chars, DT_ID_SWORD chars_format, DT_ID_UBYTE types[], DT_ID_UBYTE levels[], DT_ID_UBYTE start_level, DT_SWORD flags);

/* Break positions */
DT_SWORD DTYPE_APICALL lxBreak(DT_LXCACHE layout_cache, DT_ID_SLONG script_code, const DT_ID_UWORD chars[], DT_SLONG nr_of_chars, LX_GLYPH glyph_arr[], DT_SLONG nr_of_glyphs);
/* Break positions */
DT_SWORD DTYPE_APICALL lxBreakPlus(DT_LXCACHE layout_cache, DT_ID_SLONG script_code, DT_SLONG nr_of_chars, const DT_UBYTE* chars, DT_ID_SWORD chars_format, LX_GLYPH glyph_arr[], DT_SLONG nr_of_glyphs);

/* Get character's attribute */
DT_ID_ULONG DTYPE_APICALL lxCharGetAttrib(DT_ID_ULONG char_code, DT_ID_ULONG attrib);
/* Output character */
DT_SLONG DTYPE_APICALL lxCharOutput(DT_ID_UBYTE text_type, DT_ID_ULONG char_code, DT_SLONG buffer_len, DT_UBYTE* buffer);

/* Load text to a dynamically allocated text buffer - via stream */
DT_UBYTE* DTYPE_APICALL lxTextAllocViaStream(const DT_STREAM_DESC* sd, DT_ID_UBYTE text_type, DT_SWORD flags, DT_SLONG* buffer_len, DT_ID_SWORD* buffer_format);
/* Free dynamically allocated text buffer */
void DTYPE_APICALL lxTextFree(DT_UBYTE* text_buffer);

/* Initialize hyphenator */
DT_SWORD DTYPE_APICALL lxHyphenatorIni(DT_LXHYPHEN* hyphenator, DT_DTENGINE engine, DT_SLONG reserved);
/* Apply hyphenation to a run of text */
DT_SLONG DTYPE_APICALL lxHyphenatorApply(DT_LXHYPHEN hyphenator, DT_UWORD file_id, const DT_ID_UWORD chars[], DT_SLONG nr_of_chars, DT_SLONG reserved, DT_UBYTE* hyphen_bits);
/* Apply hyphenation to a run of text */
DT_SLONG DTYPE_APICALL lxHyphenatorApplyPlus(DT_LXHYPHEN hyphenator, DT_UWORD file_id, DT_SLONG nr_of_chars, const DT_UBYTE* chars, DT_ID_SWORD chars_format, DT_UBYTE* hyphen_bits);
/* Deinitialize (destroy) hyphenator */
void DTYPE_APICALL lxHyphenatorExt(DT_LXHYPHEN hyphenator);


const struct
{
	const DT_CHAR* script;
	DT_ID_UBYTE code;
}
ConfigScriptMap[] =
{
	/* public scripts */
	{"zyyy", zyyyScriptCode}, /* Common */
	{"qaai", qaaiScriptCode}, /* Inherited */
	{"zinh", zinhScriptCode}, /* Inherited */
	{"arab", arabScriptCode}, /* Arabic */ /**/
	{"armn", armnScriptCode}, /* Armenian */
	{"beng", bengScriptCode}, /* Bengali */ /**/
	{"bopo", bopoScriptCode}, /* Bopomofo */
	{"cher", cherScriptCode}, /* Cherokee */
	{"copt", coptScriptCode}, /* Coptic */
	{"cyrl", cyrlScriptCode}, /* Cyrillic */
	{"dsrt", dsrtScriptCode}, /* Deseret */
	{"deva", devaScriptCode}, /* Devanagari */ /**/ /* (Hindi, Sanskrit) */
	{"ethi", ethiScriptCode}, /* Ethiopic */
	{"geor", georScriptCode}, /* Georgian */
	{"goth", gothScriptCode}, /* Gothic */
	{"grek", grekScriptCode}, /* Greek */
	{"gujr", gujrScriptCode}, /* Gujarati */ /**/
	{"guru", guruScriptCode}, /* Gurmukhi */ /**/
	{"hani", haniScriptCode}, /* Han */ /**/
	{"hang", hangScriptCode}, /* Hangul */ /**/
	{"hebr", hebrScriptCode}, /* Hebrew */ /**/
	{"hira", hiraScriptCode}, /* Hiragana */
	{"knda", kndaScriptCode}, /* Kannada */ /**/
	{"kana", kanaScriptCode}, /* Katakana */
	{"khmr", khmrScriptCode}, /* Khmer */ /**/
	{"laoo", laooScriptCode}, /* Lao */
	{"latn", latnScriptCode}, /* Latin */ /**/
	{"mlym", mlymScriptCode}, /* Malayalam */ /**/
	{"mong", mongScriptCode}, /* Mongolian */
	{"mymr", mymrScriptCode}, /* Myanmar */
	{"ogam", ogamScriptCode}, /* Ogham */
	{"ital", italScriptCode}, /* Old Italic */
	{"orya", oryaScriptCode}, /* Oriya */ /**/
	{"runr", runrScriptCode}, /* Runic */
	{"sinh", sinhScriptCode}, /* Sinhala */ /**/
	{"syrc", syrcScriptCode}, /* Syriac */
	{"taml", tamlScriptCode}, /* Tamil */ /**/
	{"telu", teluScriptCode}, /* Telugu */ /**/
	{"thaa", thaaScriptCode}, /* Thaana */
	{"thai", thaiScriptCode}, /* Thai */ /**/
	{"tibt", tibtScriptCode}, /* Tibetan */ /**/
	{"cans", cansScriptCode}, /* Unified Canadian Aboriginal Symbols */
	{"yiii", yiiiScriptCode}, /* Yi syllables */
	{"tglg", tglgScriptCode}, /* Tagalog */
	{"hano", hanoScriptCode}, /* Hanunoo */
	{"buhd", buhdScriptCode}, /* Buhid */
	{"tagb", tagbScriptCode}, /* Tagbanwa */
	{"brai", braiScriptCode}, /* Braille New script in Unicode 4 */
	{"cprt", cprtScriptCode}, /* Cypriot New script in Unicode 4 */
	{"limb", limbScriptCode}, /* Limbu New script in Unicode 4 */
	{"linb", linbScriptCode}, /* Linear B New script in Unicode 4 */
	{"osma", osmaScriptCode}, /* Osmanya New script in Unicode 4 */
	{"shaw", shawScriptCode}, /* Shavian New script in Unicode 4 */
	{"tale", taleScriptCode}, /* Tai Le New script in Unicode 4 */
	{"ugar", ugarScriptCode}, /* Ugaritic New script in Unicode 4 */
	{"hrkt", hrktScriptCode}, /* Katakana or Hiragana New script code in Unicode 4.0.1 */
	{"bugi", bugiScriptCode},
	{"glag", glagScriptCode},
	{"khar", kharScriptCode},
	{"sylo", syloScriptCode},
	{"talu", taluScriptCode},
	{"tfng", tfngScriptCode},
	{"xpeo", xpeoScriptCode},
	{"bali", baliScriptCode},
	{"batk", batkScriptCode},
	{"blis", blisScriptCode},
	{"brah", brahScriptCode},
	{"cham", chamScriptCode},
	{"cirt", cirtScriptCode},
	{"cyrs", cyrsScriptCode},
	{"egyd", egydScriptCode},
	{"egyh", egyhScriptCode},
	{"egyp", egypScriptCode},
	{"geok", geokScriptCode},
	{"hans", hansScriptCode},
	{"hant", hantScriptCode},
	{"hmng", hmngScriptCode},
	{"hung", hungScriptCode},
	{"inds", indsScriptCode},
	{"java", javaScriptCode},
	{"kali", kaliScriptCode},
	{"latf", latfScriptCode},
	{"latg", latgScriptCode},
	{"lepc", lepcScriptCode},
	{"lina", linaScriptCode},
	{"mand", mandScriptCode},
	{"maya", mayaScriptCode},
	{"mero", meroScriptCode},
	{"nkoo", nkooScriptCode},
	{"orkh", orkhScriptCode},
	{"perm", permScriptCode},
	{"phag", phagScriptCode},
	{"phnx", phnxScriptCode},
	{"plrd", plrdScriptCode},
	{"roro", roroScriptCode},
	{"sara", saraScriptCode},
	{"syre", syreScriptCode},
	{"syrj", syrjScriptCode},
	{"syrn", syrnScriptCode},
	{"teng", tengScriptCode},
	{"vaii", vaiiScriptCode},
	{"visp", vispScriptCode},
	{"xsux", xsuxScriptCode},
	{"zxxx", zxxxScriptCode},
	{"zzzz", zzzzScriptCode},
	{"cari", cariScriptCode},
	{"jpan", jpanScriptCode},
	{"lana", lanaScriptCode},
	{"lyci", lyciScriptCode},
	{"lydi", lydiScriptCode},
	{"olck", olckScriptCode},
	{"rjng", rjngScriptCode},
	{"saur", saurScriptCode},
	{"sgnw", sgnwScriptCode},
	{"sund", sundScriptCode},
	{"moon", moonScriptCode},
	{"mtei", mteiScriptCode},
	{"armi", armiScriptCode},
	{"avst", avstScriptCode},
	{"cakm", cakmScriptCode},
	{"kore", koreScriptCode},
	{"kthi", kthiScriptCode},
	{"mani", maniScriptCode},
	{"phli", phliScriptCode},
	{"phlp", phlpScriptCode},
	{"phlv", phlvScriptCode},
	{"prti", prtiScriptCode},
	{"samr", samrScriptCode},
	{"tavt", tavtScriptCode},
	{"zmth", zmthScriptCode},
	{"zsym", zsymScriptCode},
	{"bamu", bamuScriptCode},
	{"lisu", lisuScriptCode},
	{"nkgb", nkgbScriptCode},
	{"sarb", sarbScriptCode},
	{"bass", bassScriptCode},
	{"dupl", duplScriptCode},
	{"elba", elbaScriptCode},
	{"gran", granScriptCode},
	{"kpel", kpelScriptCode},
	{"loma", lomaScriptCode},
	{"mend", mendScriptCode},
	{"merc", mercScriptCode},
	{"narb", narbScriptCode},
	{"nbat", nbatScriptCode},
	{"palm", palmScriptCode},
	{"sind", sindScriptCode},
	{"wara", waraScriptCode},
	{"afak", afakScriptCode},
	{"jurc", jurcScriptCode},
	{"mroo", mrooScriptCode},
	{"nshu", nshuScriptCode},
	{"shrd", shrdScriptCode},
	{"sora", soraScriptCode},
	{"takr", takrScriptCode},
	{"tang", tangScriptCode},
	{"wole", woleScriptCode},
	{"hluw", woleScriptCode},
	{"khoj", khojScriptCode},
	{"tirh", tirhScriptCode},
	{"aghb", aghbScriptCode},
	{"mahj", mahjScriptCode},
	{"ahom", ahomScriptCode},
	{"hatr", hatrScriptCode},
	{"modi", modiScriptCode},
	{"mult", multScriptCode},
	{"pauc", paucScriptCode},
	{"sidd", siddScriptCode},
	/* private scripts */
	{"p016", p016ScriptCode},
	{"p015", p015ScriptCode},
	{"p014", p014ScriptCode},
	{"p013", p013ScriptCode},
	{"p012", p012ScriptCode},
	{"p011", p011ScriptCode},
	{"p010", p010ScriptCode},
	{"p009", p009ScriptCode},
	{"p008", p008ScriptCode},
	{"p007", p007ScriptCode},
	{"p006", p006ScriptCode},
	{"p005", p005ScriptCode},
	{"p004", p004ScriptCode},
	{"p003", p003ScriptCode},
	{"p002", p002ScriptCode},
	{"p001", p001ScriptCode},
	/* any script */
	{"0000", a000ScriptCode},
	{"a000", a000ScriptCode}, /* alias */
	/* end of list */
	{DV_NULL, 0}
};


#ifdef __cplusplus
}
#endif


#endif /* DTYPE_DTYPELAY_H */
