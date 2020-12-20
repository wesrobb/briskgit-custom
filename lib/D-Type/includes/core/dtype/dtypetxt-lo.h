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


#ifndef DTYPE_DTYPETXT_LO_H
#define DTYPE_DTYPETXT_LO_H


#ifdef __cplusplus
DT_EXTERN {
#endif


/* ------------------------- D-Type Text Engine (Low-Level API) ------------------------- */


/* Structures */


/* Typography Flags */
typedef struct
{
	DT_UBYTE PowerFontPtr;
	DT_UBYTE FontIndex;
	/* DT_UBYTE FontUniqueID; */
	DT_UBYTE FontWidth, FontHeight;
	DT_UBYTE SkewHor, SkewVer;
	DT_UBYTE Rotation;
	DT_UBYTE Effect;
	DT_UBYTE EffectParam;

	DT_UBYTE Underline;
	DT_UBYTE UnderlineThickness;
	DT_UBYTE UnderlineSegment;
	DT_UBYTE UnderlineDashSize;
	DT_UBYTE UnderlineRGBT;
	DT_UBYTE UnderlineEP;

	DT_UBYTE BaselineLine;
	DT_UBYTE BaselineLineThickness;
	DT_UBYTE BaselineLineSegment;
	DT_UBYTE BaselineLineDashSize;
	DT_UBYTE BaselineLineRGBT;
	DT_UBYTE BaselineLineEP;

	DT_UBYTE Body;
	DT_UBYTE BodyRGBT;
	DT_UBYTE BodyEP;

	DT_UBYTE TypoFrameBody;
	DT_UBYTE TypoFrameBodyRGBT;
	DT_UBYTE TypoFrameBodyEP;

	DT_UBYTE RowBoundBody;
	DT_UBYTE RowBoundBodyRGBT;
	DT_UBYTE RowBoundBodyEP;

	DT_UBYTE Shadow;
	DT_UBYTE ShadowDx, ShadowDy;
	DT_UBYTE ShadowRGBT;
	DT_UBYTE ShadowEP;

	DT_UBYTE BaselineShadow;
	DT_UBYTE BaselineShadowDx, BaselineShadowDy;
	DT_UBYTE BaselineShadowRGBT;
	DT_UBYTE BaselineShadowEP;

	DT_UBYTE TypoFrameShadow;
	DT_UBYTE TypoFrameShadowDx, TypoFrameShadowDy;
	DT_UBYTE TypoFrameShadowRGBT;
	DT_UBYTE TypoFrameShadowEP;

	DT_UBYTE RowBoundShadow;
	DT_UBYTE RowBoundShadowDx, RowBoundShadowDy;
	DT_UBYTE RowBoundShadowRGBT;
	DT_UBYTE RowBoundShadowEP;

	DT_UBYTE Outline;
	DT_UBYTE OutlineTreatment;
	DT_UBYTE OutlineThickness;
	DT_UBYTE OutlineSegment;
	DT_UBYTE OutlineDashSize;
	DT_UBYTE OutlineRGBT;
	DT_UBYTE OutlineEP;

	DT_UBYTE TypoFrameOutline;
	DT_UBYTE TypoFrameOutlineTreatment;
	DT_UBYTE TypoFrameOutlineThickness;
	DT_UBYTE TypoFrameOutlineSegment;
	DT_UBYTE TypoFrameOutlineDashSize;
	DT_UBYTE TypoFrameOutlineRGBT;
	DT_UBYTE TypoFrameOutlineEP;

	DT_UBYTE RowBoundOutline;
	DT_UBYTE RowBoundOutlineTreatment;
	DT_UBYTE RowBoundOutlineThickness;
	DT_UBYTE RowBoundOutlineSegment;
	DT_UBYTE RowBoundOutlineDashSize;
	DT_UBYTE RowBoundOutlineRGBT;
	DT_UBYTE RowBoundOutlineEP;

	DT_UBYTE TextLayout;
	DT_UBYTE SpacingLetter;
	DT_UBYTE SpacingRow, SpacingBaseline, SpacingFactor;
	DT_UBYTE Dx, Dy;
	DT_UBYTE Align, AlignEnd;
	DT_UBYTE VAlignRow;
	DT_UBYTE Kerning;
	DT_UBYTE Hinting;
	DT_UBYTE Positioning;
	DT_UBYTE Direction;
	DT_UBYTE Orientation;

	DT_UBYTE TextRuler;
	DT_UBYTE StrOff_TabStopPositions;
	DT_UBYTE StrOff_TabStopFlags;
	DT_UBYTE MarginLeft, MarginRight;
	DT_UBYTE IndentLeft, IndentRight;

	DT_UBYTE BaselineLineStart, BaselineLineEnd;
	DT_UBYTE BaselineMarginLeft, BaselineMarginRight;

	DT_UBYTE RowBoundMarginLeft, RowBoundMarginRight, RowBoundMarginTop, RowBoundMarginBottom;
	DT_UBYTE RowBoundRound;
	DT_UBYTE RowBoundMerge;

	DT_UBYTE MiscAttribs;
	DT_UBYTE BreakFlags;
	DT_UBYTE Encoding;
	DT_UBYTE Shaping;
	DT_UBYTE ShapingParam;
	DT_UBYTE Language;
	DT_UBYTE Hyphenation;
	DT_UBYTE UserSpecificContent;

} DT_TX_FORMAT_FLAGS;


/* Typography Params */
typedef struct
{
	/* --- Header --- */

	DT_CHAR* Strings; /* Memory for strings */

	DT_ID_SLONG SuppliedUnderlinePatternFillPtr;
	DT_ID_SLONG SuppliedBodyPatternFillPtr;
	DT_ID_SLONG SuppliedShadowPatternFillPtr;
	DT_ID_SLONG SuppliedOutlinePatternFillPtr;
	DT_ID_SLONG SuppliedTypographyPtr;
	DT_ID_SLONG SuppliedTextRulerPtr;
	DT_ID_SLONG SuppliedTextLayoutPtr;
	DT_ID_SLONG SuppliedBaselineLinePatternFillPtr;
	DT_ID_SLONG SuppliedBaselineShadowPatternFillPtr;
	DT_ID_SLONG SuppliedBaselinePtr;
	DT_ID_SLONG SuppliedTypoFrameBodyPatternFillPtr;
	DT_ID_SLONG SuppliedTypoFrameShadowPatternFillPtr;
	DT_ID_SLONG SuppliedTypoFrameOutlinePatternFillPtr;
	DT_ID_SLONG SuppliedTypoFramePtr;
	DT_ID_SLONG SuppliedRowBoundBodyPatternFillPtr;
	DT_ID_SLONG SuppliedRowBoundShadowPatternFillPtr;
	DT_ID_SLONG SuppliedRowBoundOutlinePatternFillPtr;
	DT_ID_SLONG SuppliedRowBoundPtr;

	DT_SWORD PreferredCharSize; /* 5 is the new default */

	/* --- Data --- */

	/*	DT_UBYTE Typography is required link */
	/*	DT_UBYTE ShadowStyle is required link */
	/*	DT_UBYTE OutlineStyle is required link */

	DT_CHAR FontUniqueID[17]; /* PD_ASC */

	DT_CHAR UnderlineRGBT[9]; /* PD_HEX */
	DT_CHAR UnderlineEP[7]; /* PD_HEX */
	DT_CHAR BodyRGBT[9]; /* PD_HEX */
	DT_CHAR BodyEP[7]; /* PD_HEX */
	DT_CHAR ShadowRGBT[9]; /* PD_HEX */
	DT_CHAR ShadowEP[7]; /* PD_HEX */
	DT_CHAR OutlineRGBT[9]; /* PD_HEX */
	DT_CHAR OutlineEP[7]; /* PD_HEX */
	DT_CHAR BaselineLineRGBT[9]; /* PD_HEX */
	DT_CHAR BaselineLineEP[7]; /* PD_HEX */
	DT_CHAR BaselineShadowRGBT[9]; /* PD_HEX */
	DT_CHAR BaselineShadowEP[7]; /* PD_HEX */
	DT_CHAR TypoFrameBodyRGBT[9]; /* PD_HEX */
	DT_CHAR TypoFrameBodyEP[7]; /* PD_HEX */
	DT_CHAR TypoFrameShadowRGBT[9]; /* PD_HEX */
	DT_CHAR TypoFrameShadowEP[7]; /* PD_HEX */
	DT_CHAR TypoFrameOutlineRGBT[9]; /* PD_HEX */
	DT_CHAR TypoFrameOutlineEP[7]; /* PD_HEX */
	DT_CHAR RowBoundBodyRGBT[9]; /* PD_HEX */
	DT_CHAR RowBoundBodyEP[7]; /* PD_HEX */
	DT_CHAR RowBoundShadowRGBT[9]; /* PD_HEX */
	DT_CHAR RowBoundShadowEP[7]; /* PD_HEX */
	DT_CHAR RowBoundOutlineRGBT[9]; /* PD_HEX */
	DT_CHAR RowBoundOutlineEP[7]; /* PD_HEX */

	DT_ULONG Effect;
	DT_SLONG EffectParam0, EffectParam1, EffectParam2, EffectParam3, EffectParam4;

	DT_SLONG FontWidth, FontHeight;
	DT_ID_SLONG PowerFontPtr;

	DT_SLONG UnderlineThickness;
	DT_ULONG UnderlineSegment;
	DT_SLONG OutlineThickness;
	DT_ULONG OutlineSegment;
	DT_SLONG BaselineLineThickness;
	DT_ULONG BaselineLineSegment;
	DT_SLONG TypoFrameOutlineThickness;
	DT_ULONG TypoFrameOutlineSegment;
	DT_SLONG RowBoundOutlineThickness;
	DT_ULONG RowBoundOutlineSegment;

	DT_SLONG SpacingRow, SpacingBaseline;
	DT_SLONG Dx, Dy;

	DT_ULONG Orientation;

	DT_ULONG Shaping;
	DT_ULONG ShapingParam0;

	DT_SLONG MarginLeft, MarginRight;
	DT_SLONG BaselineMarginLeft, BaselineMarginRight;
	DT_SLONG RowBoundMarginLeft, RowBoundMarginRight, RowBoundMarginTop, RowBoundMarginBottom;

	DT_UWORD UnderlineDashSize;
	DT_UWORD OutlineDashSize;
	DT_UWORD BaselineLineDashSize;
	DT_UWORD TypoFrameOutlineDashSize;
	DT_UWORD RowBoundOutlineDashSize;

	DT_UWORD IndentLeft, IndentRight;
	DT_UWORD StrOff_FontMMContributions; /* PD_ARR_WORD_UNSIGNED */
	DT_UWORD StrOff_TabStopPositions; /* PD_ARR_WORD_UNSIGNED */
	DT_UWORD StrOff_TabStopFlags; /* PD_ARR_BYTE_UNSIGNED */
	DT_UWORD StrOff_UnderlineEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_BodyEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_ShadowEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_OutlineEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_BaselineLineEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_BaselineShadowEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_TypoFrameBodyEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_TypoFrameShadowEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_TypoFrameOutlineEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_RowBoundBodyEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_RowBoundShadowEffectsArr; /* PD_ARR_LONG_SIGNED */
	DT_UWORD StrOff_RowBoundOutlineEffectsArr; /* PD_ARR_LONG_SIGNED */

	DT_UWORD Language;

	DT_SWORD FontIndex;
	DT_SWORD SkewHor, SkewVer;
	DT_SWORD Rotation;

	DT_SLONG ShadowDx, ShadowDy; /* was DT_SWORD before 6.0.2.2 */
	DT_SLONG BaselineShadowDx, BaselineShadowDy;
	DT_SLONG TypoFrameShadowDx, TypoFrameShadowDy;
	DT_SLONG RowBoundShadowDx, RowBoundShadowDy;

	DT_SLONG SpacingLetter; /* was DT_SWORD before 6.0.2.2 */
	DT_SWORD SpacingFactor;

	DT_UBYTE Underline; /* Optional link */
	DT_UBYTE Body; /* Optional link */
	DT_UBYTE Shadow; /* Optional link */
	DT_UBYTE Outline; /* Optional link */
	DT_UBYTE BaselineLine; /* Optional link */
	DT_UBYTE BaselineShadow; /* Optional link */
	DT_UBYTE TypoFrameBody; /* Optional link */
	DT_UBYTE TypoFrameShadow; /* Optional link */
	DT_UBYTE TypoFrameOutline; /* Optional link */
	DT_UBYTE RowBoundBody; /* Optional link */
	DT_UBYTE RowBoundShadow; /* Optional link */
	DT_UBYTE RowBoundOutline; /* Optional link */

	DT_UBYTE OutlineTreatment;
	DT_UBYTE TypoFrameOutlineTreatment;
	DT_UBYTE RowBoundOutlineTreatment;

	DT_UBYTE BaselineLineStart;
	DT_UBYTE BaselineLineEnd;

	DT_UBYTE RowBoundMerge;
	DT_UWORD RowBoundRound;

	DT_UBYTE TextLayout; /* Optional link */
	DT_UBYTE Align, AlignEnd;
	DT_UBYTE VAlignRow;
	DT_UBYTE Kerning;
	DT_UBYTE Hinting;
	DT_UBYTE Positioning;
	DT_UBYTE Direction;

	DT_UBYTE TextRuler; /* Optional link */

	DT_UBYTE MiscAttribs;
	DT_UBYTE BreakFlags;
	DT_UBYTE Encoding;
	DT_UBYTE Hyphenation;

	void* UserSpecificContent;
	DT_SLONG UserSpecificContentLen;

	DT_TX_FORMAT_FLAGS Flags;

} DT_TX_FORMAT;


/* Textedit configuration */
typedef struct
{
	DT_SWORD FnDraw;
	DT_SLONG DefaultWidth, DefaultHeight;
	DT_SLONG Name_TextAreaW, Name_TextAreaH;
	DT_SLONG Name_TextString_8, Name_TextString_16, Name_TextString_24, Name_TextString_32;
	DT_SLONG Name_TextFragmentPtr;
	DT_UBYTE DisableTextFormat;

} DT_TX_TEXTEDIT_CONFIG;


/* Maximum length of static strings */
#define TX_MAXTEXTLENGTH 1024

/* Sizes of DT_TX_FORMAT strings */
#define TX_STRINGS_SIZE_STD 512
#define TX_STRINGS_SIZE_USR 1024
#define TX_STRINGS_SIZE_BIG 2048
/* Offsets of DT_TX_FORMAT strings */
#define TX_STRINGS_OFF_S00 ( 0                      )
#define TX_STRINGS_OFF_S01 ( 1 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S02 ( 2 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S03 ( 3 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S04 ( 4 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S05 ( 5 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S06 ( 6 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S07 ( 7 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S08 ( 8 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S09 ( 9 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S10 (10 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S11 (11 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S12 (12 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S13 (13 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_S14 (14 * TX_STRINGS_SIZE_STD)
#define TX_STRINGS_OFF_USR (15 * TX_STRINGS_SIZE_STD)
/* Maximum length of the DT_TX_FORMAT strings buffer (15 strings + user specific content) */
#define TX_STRINGS_MAXSIZE (TX_STRINGS_OFF_USR + TX_STRINGS_SIZE_USR)

/* Flags */
#define TX_FLAG_UNSET 0
#define TX_FLAG_SET   1
#define TX_FLAG_SET2  2
#define TX_FLAG_SET3  3

/* Text compression methods */
#define TX_TEXT_COMPRESS_ON_1 8
#define TX_TEXT_COMPRESS_ON_2 6
#define TX_TEXT_COMPRESS_ON_3 4
#define TX_TEXT_COMPRESS_ON_4 2
#define TX_TEXT_COMPRESS_OFF_1 0
#define TX_TEXT_COMPRESS_OFF_2 0
#define TX_TEXT_COMPRESS_OFF_3 0
#define TX_TEXT_COMPRESS_OFF_4 0


/* ------------------------- Enums ------------------------- */

enum
{
	txRichTextFirstPtr = 2000,
	txRichTextNextPtr,
	txStatsMinCharIndex,
	txStatsMaxCharIndex,
	txStatsNrOfRows,
	txLockStatus
};


inline DT_SLONG txConfigName_TextFragmentPtr(const DT_TX_TEXTEDIT_CONFIG* config)
{
	if (config == DV_NULL) return pdTextFragmentPtr;
	return config->Name_TextFragmentPtr;
}

inline DT_SLONG txConfigName_TextString_8(const DT_TX_TEXTEDIT_CONFIG* config)
{
	if (config == DV_NULL) return pdTextString_08;
	return config->Name_TextString_8;
}

inline DT_SLONG txConfigName_TextString_16(const DT_TX_TEXTEDIT_CONFIG* config)
{
	if (config == DV_NULL) return pdTextString_16;
	return config->Name_TextString_16;
}

inline DT_SLONG txConfigName_TextString_24(const DT_TX_TEXTEDIT_CONFIG* config)
{
	if (config == DV_NULL) return pdTextString_24;
	return config->Name_TextString_24;
}

inline DT_SLONG txConfigName_TextString_32(const DT_TX_TEXTEDIT_CONFIG* config)
{
	if (config == DV_NULL) return pdTextString_32;
	return config->Name_TextString_32;
}


/* ------------------------- Functions - Character manipulation ------------------------- */

/* Copy characters */
DT_ID_ULONG DTYPE_APICALL txCharsCopy_32(DT_SLONG nr_of_chars, const DT_UBYTE* addr_src, DT_UBYTE* addr_dst, DT_ID_SWORD sz_src);
/* Convert characters */
DT_SWORD DTYPE_APICALL txCharsConvert(DT_SLONG nr_of_chars, const DT_UBYTE* addr_src, DT_UBYTE* addr_dst, DT_ID_SWORD sz_src, DT_ID_SWORD sz_dst, DT_ID_ULONG overflow_char);
/* Optimize text */
DT_ID_SWORD DTYPE_APICALL txCharsOptimize(DT_SLONG nr_of_chars, DT_UBYTE* addr, DT_ID_SWORD sz);


/* ------------------------- Functions - Image manipulation ------------------------- */

/* Get aggregate's root Rich Text Area object */
DT_ID_SLONG DTYPE_APICALL txImgGetRootRichTextArea(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent, DT_SWORD* fn_draw);
/* Reflow aggregate's text */
DT_SLONG DTYPE_APICALL txImgReflow(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE enable_compression);
/* Reflow aggregate's text */
DT_SLONG DTYPE_APICALL txImgReflowPlus(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE enable_compression, DT_SLONG count_from, DT_SLONG count_to);


/* ------------------------- Functions - Text manipulation ------------------------- */

/* Get information about the text fragment based on the specified character position */
DT_UBYTE* DTYPE_APICALL txObjFragmentLookup(DT_PDDOC power_doc, DT_ID_SLONG* obj, DT_SLONG* first_char, DT_SLONG* last_char, DT_SLONG* len, DT_ID_SWORD* sz, DT_SLONG* compress_len, const DT_TX_TEXTEDIT_CONFIG* config);

/* Reset text format */
void DTYPE_APICALL txFormatReset(DT_TX_FORMAT* format, DT_ID_UBYTE flag);
/* Uncheck all text format flags */
void DTYPE_APICALL txFormatUncheckFlags(DT_TX_FORMAT* format, const DT_TX_FORMAT* temp_params);
/* Sum all text format flags */
DT_SLONG DTYPE_APICALL txFormatSumFlags(const DT_TX_FORMAT* format);
/* Test all text format flags */
DT_SWORD DTYPE_APICALL txFormatTestFlags(const DT_TX_FORMAT* format);
/* Apply text format to a text fragment */
void DTYPE_APICALL txFormatApplyToTextFragment(const DT_TX_FORMAT* format, DT_PDDOC power_doc, DT_ID_SLONG obj, DT_UBYTE insert_mode);
/* Apply text format to a chain of text fragments */
void DTYPE_APICALL txFormatApplyToTextFragmentChain(const DT_TX_FORMAT* format, DT_PDDOC power_doc, DT_ID_SLONG obj, const DT_TX_TEXTEDIT_CONFIG* config);
/* Get text format for a text fragment */
void DTYPE_APICALL txFormatGetForTextFragment(DT_TX_FORMAT* format, DT_CHAR* buffer, DT_PDDOC power_doc, DT_ID_SLONG obj);
/* Get text format for a chain of text fragments */
DT_SWORD DTYPE_APICALL txFormatGetForTextFragmentChain(DT_TX_FORMAT* format, DT_CHAR* buffer, DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, DT_SWORD reset, const DT_TX_TEXTEDIT_CONFIG* config);
/* Save user supplied content in a text format */
DT_SWORD DTYPE_APICALL txFormatSaveUserSuppliedContent(DT_TX_FORMAT* format);

/* Count the number of character in a PowerDoc image */
DT_SLONG DTYPE_APICALL txObjCharsCount(DT_PDDOC power_doc, DT_ID_SLONG obj, const DT_TX_TEXTEDIT_CONFIG* config);
/* Copy a range of characters from a PowerDoc image to the specified UCS4 text buffer */
DT_SLONG DTYPE_APICALL txObjCharsCopy_32(DT_PDDOC power_doc, DT_UBYTE* text_addr, DT_UBYTE* text_encoding_addr, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE null_terminate, DT_ID_ULONG* max_char_code);
/* Copy a range of characters from a PowerDoc image to the specified UCS3 text buffer */
DT_SLONG DTYPE_APICALL txObjCharsCopy_24(DT_PDDOC power_doc, DT_UBYTE* text_addr, DT_UBYTE* text_encoding_addr, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE null_terminate, DT_ID_ULONG* max_char_code);
/* Copy a range of characters from a PowerDoc image to the specified UCS2 text buffer */
DT_SLONG DTYPE_APICALL txObjCharsCopy_16(DT_PDDOC power_doc, DT_UBYTE* text_addr, DT_UBYTE* text_encoding_addr, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE null_terminate, DT_ID_ULONG* max_char_code);
/* Copy a range of characters from a PowerDoc image to the specified ANSI text buffer */
DT_SLONG DTYPE_APICALL txObjCharsCopy_8(DT_PDDOC power_doc, DT_UBYTE* text_addr, DT_UBYTE* text_encoding_addr, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE null_terminate, DT_ID_ULONG* max_char_code);
/* Paste buffer containing UCS4 text to the specified character selection in a PowerDoc image */
DT_SWORD DTYPE_APICALL txObjCharsPaste_32(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_UBYTE* text_addr, DT_SLONG nr_of_chars, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE enable_compression);
/* Paste buffer containing UCS3 text to the specified character selection in a PowerDoc image */
DT_SWORD DTYPE_APICALL txObjCharsPaste_24(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_UBYTE* text_addr, DT_SLONG nr_of_chars, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE enable_compression);
/* Paste buffer containing UCS2 text to the specified character selection in a PowerDoc image */
DT_SWORD DTYPE_APICALL txObjCharsPaste_16(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_UBYTE* text_addr, DT_SLONG nr_of_chars, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE enable_compression);
/* Paste buffer containing ANSI text to the specified character selection in a PowerDoc image */
DT_SWORD DTYPE_APICALL txObjCharsPaste_8(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_UBYTE* text_addr, DT_SLONG nr_of_chars, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE enable_compression);
/* Delete specified character selection in a PowerDoc image */
DT_SWORD DTYPE_APICALL txObjCharsDelete(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE enable_compression);

/* Insert a chain of text fragments */
DT_SWORD DTYPE_APICALL txObjFragmentChainInsert(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_ID_SLONG obj_chain, const DT_TX_TEXTEDIT_CONFIG* config);
/* Format a chain of text fragments */
DT_SWORD DTYPE_APICALL txObjFragmentChainFormat(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG first_char, DT_SLONG last_char, const DT_TX_FORMAT* format, const DT_TX_TEXTEDIT_CONFIG* config, DT_UBYTE enable_compression, DT_UBYTE enable_descriptions, DT_SWORD preferred_char_size);
/* Join neighbouring fragments with the same format */
DT_SLONG DTYPE_APICALL txObjFragmentChainJoinSameFormat(DT_PDDOC power_doc, DT_ID_SLONG obj, const DT_TX_TEXTEDIT_CONFIG* config);
/* Remove all empty fragments from the chain */
DT_SLONG DTYPE_APICALL txObjFragmentChainRemoveEmpty(DT_PDDOC power_doc, DT_ID_SLONG obj, const DT_TX_TEXTEDIT_CONFIG* config);
/* Search a chain of text fragments for obj_search */
DT_SLONG DTYPE_APICALL txObjFragmentChainSearch(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_ID_SLONG obj_search, const DT_TX_TEXTEDIT_CONFIG* config);


#ifdef __cplusplus
}
#endif


#endif /* DTYPE_DTYPETXT_LO_H */
