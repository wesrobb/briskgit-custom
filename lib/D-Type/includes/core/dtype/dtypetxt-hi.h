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


#ifndef DTYPE_DTYPETXT_HI_H
#define DTYPE_DTYPETXT_HI_H


#ifdef __cplusplus
DT_EXTERN {
#endif


/* ------------------------- D-Type Text Engine (High-Level API) ------------------------- */

typedef struct DT_TXENGINE_STRUCT* DT_TXENGINE;
typedef struct DT_TXDOC_STRUCT* DT_TXDOC;


/* Structures */


/* Text default parameters */
typedef struct
{
	const DT_CHAR* RGBT;
	const DT_CHAR* FontWidth;
	const DT_CHAR* FontHeight;
	const DT_CHAR* SkewHor;
	const DT_CHAR* SkewVer;
	const DT_CHAR* SpacingRow;
	const DT_CHAR* SpacingLetter;
	const DT_CHAR* Align;
	const DT_CHAR* AlignEnd;
	const DT_CHAR* Hinting;
	const DT_CHAR* FontIndex;
	const DT_CHAR* Reserved;

} DT_TX_DEFAULT_ATTRIBS;


/* Extra information for DT_TX_TEXTFLOW_AREA */
typedef struct
{
	const DT_FLOAT* TransformArray; /* An affine or perspective transformation to apply to the textflow area */
	const DT_UBYTE* EdgeArrayLeft;  /* Left edge polyline for text areas with a custom (i.e. user-defined) left edge. Should be used only with custom text areas (Type=TX_CUSTOM or Type=TX_CUSTOM2). */
	const DT_UBYTE* EdgeArrayRight; /* Right edge polyline for text areas with a custom (i.e. user-defined) right edge. Should be used only with custom text areas (Type=TX_CUSTOM or Type=TX_CUSTOM2). */
	DT_SLONG EdgeLenLeft;           /* Length of the EdgeArrayLeft array */
	DT_SLONG EdgeLenRight;          /* Length of the EdgeArrayRight array */
	DT_ID_UBYTE VAlign;             /* Vertical alignment of text inside the text area, when the text flow does not end in this text area. Should be used only with rectangular text areas (Type=TX_RECT). */
	DT_ID_UBYTE VAlignEnd;          /* Vertical alignment of text inside the text area, when the text flow ends in this text area. Should be used only with rectangular text areas (Type=TX_RECT). */

} DT_TX_TEXTFLOW_AREA_EXTRA;


/* Textflow area attributes */
typedef struct
{
	DT_SLONG X, Y, W, H;            /* Textflow area's bounding box in document units */
	DT_ID_UBYTE Type;               /* Type of textflow area and control of empty text lines */
	DT_ID_UBYTE RowType;            /* Row type within the textflow area */
	DT_ID_UBYTE WrapType;           /* Text wrap method */
	DT_ID_UBYTE GlobalDirection;    /* Global text direction and bidirectional text display method for a textflow area */
	DT_ID_UBYTE TargetDeviceID;     /* Textflow area's device mode */
	DT_ID_UBYTE TransformAndExtras; /* Identifies the type of the transformation matrix and/or any extra parameters. Can be one of the following:
	                                   TX_TM_NULL       = no transformation matrix and no extra parameters - TransformArray/Extra is DV_NULL
	                                   TX_TM_1X1        = 1x1 transformation matrix only - TransformArray is a valid pointer to an array containing 1 element
	                                   TX_TM_2X2        = 2x2 transformation matrix only - TransformArray is a valid pointer to an array containing 4 elements
	                                   TX_TM_3X3        = 3x3 transformation matrix only - TransformArray is a valid pointer to an array containing 9 elements
	                                   TX_TM_EXTRA_NULL = extra parameters only - Extra is a valid pointer to the DT_TX_TEXTFLOW_AREA_EXTRA structure, Extra->TransformArray is DV_NULL
	                                   TX_TM_EXTRA_1X1  = 1x1 transformation matrix and extra parameters - Extra is a valid pointer to the DT_TX_TEXTFLOW_AREA_EXTRA structure, Extra->TransformArray is a valid pointer to an array containing 1 element
	                                   TX_TM_EXTRA_2X2  = 2x2 transformation matrix and extra parameters - Extra is a valid pointer to the DT_TX_TEXTFLOW_AREA_EXTRA structure, Extra->TransformArray is a valid pointer to an array containing 4 elements
	                                   TX_TM_EXTRA_3X3  = 3x3 transformation matrix and extra parameters - Extra is a valid pointer to the DT_TX_TEXTFLOW_AREA_EXTRA structure, Extra->TransformArray is a valid pointer to an array containing 9 elements
	                                */
	union
	{
		/* An affine or perspective transformation to apply to the textflow area */
		const DT_FLOAT* TransformArray;
		/* An affine or perspective transformation and any extra parameters to apply to the textflow area */
		const DT_TX_TEXTFLOW_AREA_EXTRA* Extra;
	};

} DT_TX_TEXTFLOW_AREA;


#define TransformArrayID TransformAndExtras


/* Parameters for text documents */
typedef struct
{
	DT_ULONG EnableBits;           /* A set of bit values to enable or disable various text document features */
	DT_ULONG ConfigBits;           /* Various configuration bits */
	const DT_PD_DOC_PARAMS* Extra; /* Optional extra parameters (can be DV_NULL) */

} DT_TX_DOC_PARAMS;


/* Formatting/Styling Value */
typedef DT_CHAR DT_TX_ATTRIBS_VALUE [256];


/* Formatting/Styling Buffer */
typedef struct
{
	const DT_UBYTE* Addr; /* Memory address at which the value of the formatting/styling attribute begins */
	DT_SLONG Len;         /* Length of the value in bytes */

} DT_TX_ATTRIBS_BUFFER;


/* Text formatting/styling structure */
typedef struct
{
	DT_ID_SLONG Id;                  /* Text formatting/styling attribute identifier and indicator if its value is supplied via the Value or Buffer union member */

	union
	{
		DT_TX_ATTRIBS_VALUE Value;   /* Value of the formatting/styling attribute (suitable for null-terminated ASCII strings up to 255 bytes long) */
		DT_TX_ATTRIBS_BUFFER Buffer; /* Buffer holding the value of the formatting/styling attribute (suitable for longer values and/or binary data) */
	};

} DT_TX_ATTRIBS;


/* Text flow (currently we must use both Img and Obj to identify a text flow; ideally only Obj should be required) */
typedef struct
{
	DT_SLONG Img;
	DT_ID_SLONG ParentObj;

} DT_TX_TEXT_FLOW;


/* Types of text files */
#define TX_AUTO       0   /* Auto-detect */
#define TX_ANSI       1   /* Ansi */
#define TX_UCS2_LE    2   /* UCS-2 Little Endian (deprecated in favor of TX_UTF16_LE) */
#define TX_UCS2_BE    3   /* UCS-2 Big Endian (deprecated in favor of TX_UTF16_BE) */
#define TX_UTF8       4   /* UTF-8 */
#define TX_UCS4_LE    5   /* UCS-4 Little Endian */
#define TX_UCS4_BE    6   /* UCS-4 Big Endian */
#define TX_UTF16_LE   7   /* UTF-16 Little Endian */
#define TX_UTF16_BE   8   /* UTF-16 Big Endian */
#define TX_UTF32_LE   9   /* UTF-32 Little Endian */
#define TX_UTF32_BE   10  /* UTF-32 Big Endian */
#define TX_BUFFER3_LE 11  /* Like UCS-2 or UCS-4 Little Endian, but 3 bytes per char */
#define TX_BUFFER3_BE 12  /* Like UCS-2 or UCS-4 Big Endian, but 3 bytes per char */


/* Ways to import Unicode text */
#define TX_IMPORT_UNICODE_BIDI      1   /* Apply Unicode Bidirectional Algorithm */
#define TX_IMPORT_UNICODE_SCRIPT    2   /* Auto-detect Unicode scripts and use complex shapings */
#define TX_IMPORT_OPENTYPE_CJK_VALT 4   /* Assume text is used in vertical layout and apply vertical OpenType glyph forms to 'hani' and 'kana' scripts (when available in font) */
#define TX_IMPORT_POWERDOC_REFORMAT 8   /* Reserved for PowerDoc text */
#define TX_IMPORT_UNICODE_OPTIMIZE  16  /* Optimize Unicode text */
#define TX_IMPORT_UNICODE_COMPRESS  32  /* Compress Unicode text */
#define TX_IMPORT_BASIC             0   /* No special processing */
#define TX_IMPORT_FULL              3   /* Same as (TX_IMPORT_UNICODE_BIDI | TX_IMPORT_UNICODE_SCRIPT) */
#define TX_IMPORT_FULL_COMPACT      51  /* Same as (TX_IMPORT_UNICODE_BIDI | TX_IMPORT_UNICODE_SCRIPT | TX_IMPORT_UNICODE_OPTIMIZE | TX_IMPORT_UNICODE_COMPRESS) */


/* Blend modes */
#define TX_BLEND_INVERT      0    /* Invert background */
#define TX_BLEND_HIGHLIGHT_1 1    /* Highlight over light background */
#define TX_BLEND_HIGHLIGHT_2 2    /* Highlight over dark background */
#define TX_BLEND_RENDER      255  /* Standard rendering */


/* Various types of textflow areas */
#define TX_RECT        0    /* Rectangular */
#define TX_CUSTOM      5    /* Custom */
#define TX_CUSTOM2     6    /* Custom With Extra Precision */
#define TX_CIRC_QRTR_A 10   /* Quarter-Circular A */
#define TX_CIRC_QRTR_B 11   /* Quarter-Circular B */
#define TX_CIRC_QRTR_C 12   /* Quarter-Circular C */
#define TX_CIRC_QRTR_D 13   /* Quarter-Circular D */
#define TX_CIRC_HALF_A 14   /* Half-Circular A */
#define TX_CIRC_HALF_B 15   /* Half-Circular B */
#define TX_CIRC_HALF_C 16   /* Half-Circular C */
#define TX_CIRC_HALF_D 17   /* Half-Circular D */
#define TX_CIRC        18   /* Circular */
#define TX_DIAM_QRTR_A 20   /* Quarter-Diamond A */
#define TX_DIAM_QRTR_B 21   /* Quarter-Diamond B */
#define TX_DIAM_QRTR_C 22   /* Quarter-Diamond C */
#define TX_DIAM_QRTR_D 23   /* Quarter-Diamond D */
#define TX_DIAM_HALF_A 24   /* Half-Diamond A */
#define TX_DIAM_HALF_B 25   /* Half-Diamond B */
#define TX_DIAM_HALF_C 26   /* Half-Diamond C */
#define TX_DIAM_HALF_D 27   /* Half-Diamond D */
#define TX_DIAM        28   /* Diamond */


/* Row types within a textflow area */
#define TX_MATHROW            0    /* mathematically calculate spacing between text rows */
#define TX_TYPOROW            10   /* respect typographic values (i.e. sTypoAscender, sTypoDescender) when calculating spacing between text rows */
#define TX_TYPOROW_LINEGAP    20   /* same as TX_TYPOROW but adds additional linegap (i.e. sTypoLineGap), if available in the font */
#define TX_WINTYPOROW         30   /* respect Windows specific typographic values (i.e. usWinAscent, usWinDescent) when calculating spacing between text rows */
#define TX_HORTYPOROW         110  /* same as TX_TYPOROW but horizontal typographic values are used for text in vertical layout (columns) */
#define TX_HORTYPOROW_LINEGAP 120  /* same as TX_TYPOROW_LINEGAP but horizontal typographic values are used for text in vertical layout (columns) */


/* Text wrap methods */
#define TX_WRAP     0   /* Soft Wrap Enabled */
#define TX_NOWRAP_A 1   /* Soft Wrap Disabled - trim text after any character */
#define TX_NOWRAP_B 2   /* Soft Wrap Disabled - trim text only after space or some other breakable character (e.g. CJK) but not after a hyphen */
#define TX_NOWRAP_C 3   /* Soft Wrap Disabled - trim text only after space or some other breakable character (e.g. CJK) or after a hyphen */
#define TX_NOWRAP_D 4   /* Soft Wrap Disabled - trim text after any character and add a horizontal ellipsis */


/* Global text direction and bidirectional text display method for a textflow area */
/* Horizontal Layout (Text in Rows) and Bidirectional Reordering: */
#define TX_DIR_ROW_LR_TB               0   /* Left-to-Right, Top-to-Bottom (e.g. Latin, Cyrillic, Greek) */
#define TX_DIR_ROW_RL_TB               1   /* Right-to-Left, Top-to-Bottom (e.g. Arabic, Hebrew) */
#define TX_DIR_ROW_LR_BT               2   /* Left-to-Right, Bottom-to-Top */
#define TX_DIR_ROW_RL_BT               3   /* Right-to-Left, Bottom-to-Top */
#define TX_DIR_ROW_LR_TB_SWAP          4   /* Same as TX_DIR_ROW_LR_TB but swap Portrait & Landscape */
#define TX_DIR_ROW_RL_TB_SWAP          5   /* Same as TX_DIR_ROW_RL_TB but swap Portrait & Landscape */
#define TX_DIR_ROW_LR_BT_SWAP          6   /* Same as TX_DIR_ROW_LR_BT but swap Portrait & Landscape */
#define TX_DIR_ROW_RL_BT_SWAP          7   /* Same as TX_DIR_ROW_RL_BT but swap Portrait & Landscape */
/* Vertical Layout (Text in Columns) and Bidirectional Reordering: */
#define TX_DIR_COL_TB_RL               8   /* Top-to-Bottom, Right-to-Left (e.g. Vertical Chinese) */
#define TX_DIR_COL_BT_RL               9   /* Bottom-to-Top, Right-to-Left */
#define TX_DIR_COL_TB_LR              10   /* Top-to-Bottom, Left-to-Right */
#define TX_DIR_COL_BT_LR              11   /* Bottom-to-Top, Left-to-Right */
#define TX_DIR_COL_TB_RL_SWAP         12   /* Same as TX_DIR_COL_TB_RL but swap Portrait & Landscape */
#define TX_DIR_COL_BT_RL_SWAP         13   /* Same as TX_DIR_COL_BT_RL but swap Portrait & Landscape */
#define TX_DIR_COL_TB_LR_SWAP         14   /* Same as TX_DIR_COL_TB_LR but swap Portrait & Landscape */
#define TX_DIR_COL_BT_LR_SWAP         15   /* Same as TX_DIR_COL_BT_LR but swap Portrait & Landscape */
/* Horizontal Layout (Text in Rows) and Bidirectional Rotation: */
#define TX_DIR_ROW_BIDIROT_LR_TB      16   /* Left-to-Right, Top-to-Bottom (e.g. Latin, Cyrillic, Greek) */
#define TX_DIR_ROW_BIDIROT_RL_TB      17   /* Right-to-Left, Top-to-Bottom (e.g. Arabic, Hebrew) */
#define TX_DIR_ROW_BIDIROT_LR_BT      18   /* Left-to-Right, Bottom-to-Top */
#define TX_DIR_ROW_BIDIROT_RL_BT      19   /* Right-to-Left, Bottom-to-Top */
#define TX_DIR_ROW_BIDIROT_LR_TB_SWAP 20   /* Same as TX_DIR_ROW_BIDIROT_LR_TB but swap Portrait & Landscape */
#define TX_DIR_ROW_BIDIROT_RL_TB_SWAP 21   /* Same as TX_DIR_ROW_BIDIROT_RL_TB but swap Portrait & Landscape */
#define TX_DIR_ROW_BIDIROT_LR_BT_SWAP 22   /* Same as TX_DIR_ROW_BIDIROT_LR_BT but swap Portrait & Landscape */
#define TX_DIR_ROW_BIDIROT_RL_BT_SWAP 23   /* Same as TX_DIR_ROW_BIDIROT_RL_BT but swap Portrait & Landscape */
/* Vertical Layout (Text in Columns) and Bidirectional Rotation: */
#define TX_DIR_COL_BIDIROT_TB_RL      24   /* Top-to-Bottom, Right-to-Left (e.g. Vertical Chinese) */
#define TX_DIR_COL_BIDIROT_BT_RL      25   /* Bottom-to-Top, Right-to-Left */
#define TX_DIR_COL_BIDIROT_TB_LR      26   /* Top-to-Bottom, Left-to-Right */
#define TX_DIR_COL_BIDIROT_BT_LR      27   /* Bottom-to-Top, Left-to-Right */
#define TX_DIR_COL_BIDIROT_TB_RL_SWAP 28   /* Same as TX_DIR_COL_BIDIROT_TB_RL but swap Portrait & Landscape */
#define TX_DIR_COL_BIDIROT_BT_RL_SWAP 29   /* Same as TX_DIR_COL_BIDIROT_BT_RL but swap Portrait & Landscape */
#define TX_DIR_COL_BIDIROT_TB_LR_SWAP 30   /* Same as TX_DIR_COL_BIDIROT_TB_LR but swap Portrait & Landscape */
#define TX_DIR_COL_BIDIROT_BT_LR_SWAP 31   /* Same as TX_DIR_COL_BIDIROT_BT_LR but swap Portrait & Landscape */

/* For text in rows - deprecated labels: */
//#define TX_DIR_ROW_UPRIGHT_LR_TB  0
//#define TX_DIR_ROW_UPRIGHT_RL_TB  1
//#define TX_DIR_ROW_UPRIGHT_LR_BT  2
//#define TX_DIR_ROW_UPRIGHT_RL_BT  3
//#define TX_DIR_ROW_SIDEWAY_LR_TB  4
//#define TX_DIR_ROW_SIDEWAY_RL_TB  5
//#define TX_DIR_ROW_SIDEWAY_LR_BT  6
//#define TX_DIR_ROW_SIDEWAY_RL_BT  7
/* For text in columns - deprecated labels: */
//#define TX_DIR_COL_SIDEWAY_TB_RL  8
//#define TX_DIR_COL_SIDEWAY_BT_RL  9
//#define TX_DIR_COL_SIDEWAY_TB_LR 10
//#define TX_DIR_COL_SIDEWAY_BT_LR 11
//#define TX_DIR_COL_UPRIGHT_TB_RL 12
//#define TX_DIR_COL_UPRIGHT_BT_RL 13
//#define TX_DIR_COL_UPRIGHT_TB_LR 14
//#define TX_DIR_COL_UPRIGHT_BT_LR 15


/* Device mode (for text layout) */
#define TX_LAY_FRAC     0   /* Device Independent (Mode 0, Enhanced Fractional Positioning) */
#define TX_LAY_WHOLE    1   /* Device Independent (Mode 0, Enhanced Whole Pixel Positioning) */
#define TX_LAY_DEVICE_A 2   /* Device Dependent (Mode 1) */
#define TX_LAY_DEVICE_B 3   /* Device Dependent (Mode 2) */
#define TX_LAY_MIXED_A  4   /* Mixed (Mode 101) */
#define TX_LAY_MIXED_B  5   /* Mixed (Mode 102) */


/* Types of transformation matrices that an array can hold */
#define TX_TM_NULL       0  /* no transformation matrix and no extra parameters - TransformArray/Extra is DV_NULL */
#define TX_TM_1X1        1  /* 1x1 transformation matrix only - TransformArray is a valid pointer to an array containing 1 element */
#define TX_TM_2X2        2  /* 2x2 transformation matrix only - TransformArray is a valid pointer to an array containing 4 elements */
#define TX_TM_3X3        3  /* 3x3 transformation matrix only - TransformArray is a valid pointer to an array containing 9 elements */
#define TX_TM_EXTRA_NULL 4  /* extra parameters only - Extra is a valid pointer to the DT_TX_TEXTFLOW_AREA_EXTRA structure, Extra->TransformArray is DV_NULL */
#define TX_TM_EXTRA_1X1  5  /* 1x1 transformation matrix and extra parameters - Extra is a valid pointer to the DT_TX_TEXTFLOW_AREA_EXTRA structure. Extra->TransformArray is a valid pointer to an array containing 1 element */
#define TX_TM_EXTRA_2X2  6  /* 2x2 transformation matrix and extra parameters - Extra is a valid pointer to the DT_TX_TEXTFLOW_AREA_EXTRA structure. Extra->TransformArray is a valid pointer to an array containing 4 elements */
#define TX_TM_EXTRA_3X3  7  /* 3x3 transformation matrix and extra parameters - Extra is a valid pointer to the DT_TX_TEXTFLOW_AREA_EXTRA structure. Extra->TransformArray is a valid pointer to an array containing 9 elements */
/* obsolete */
//#define TX_TM0 TX_TM_NULL
//#define TX_TM1 TX_TM_1X1
//#define TX_TM2 TX_TM_2X2
//#define TX_TM3 TX_TM_3X3


/* Vertical text alignment */
#define TX_VALIGN_TOP       0  /* Top */
#define TX_VALING_MIDDLE    1  /* Middle */
#define TX_VALIGN_BOTTOM    2  /* Bottom */
#define TX_VALIGN_JUSTIFIED 3  /* Justified */


/* Text draw flags */
#define TX_DRAW_DEFAULT_A                  0   /* same as TX_DRAW_TEXT */
#define TX_DRAW_DEFAULT_B                  1   /* same as TX_DRAW_TEXT_MARKERS_VISIBLEFRAMES */
#define TX_DRAW_NONE                       4   /* Do not draw anything; do not generate text flows and frames (no output) */
#define TX_DRAW_NONE_ALLFRAMES             6   /* Do not draw anything; generate all text flows and frames */
#define TX_DRAW_TEXT                       8   /* Draw text only; do not generate text flows and frames */
#define TX_DRAW_TEXT_VISIBLEFRAMES         9   /* Draw text only; generate visible text flows and frames */
#define TX_DRAW_TEXT_ALLFRAMES             10  /* Draw text only; generate all text flows and frames */
#define TX_DRAW_MARKERS_ALLFRAMES          14  /* Draw the cursor and selections only; generate all text flows and frames */
#define TX_DRAW_TEXT_MARKERS_VISIBLEFRAMES 17  /* First draw text, then draw the cursor and selections; generate visible text flows and frames */
#define TX_DRAW_TEXT_MARKERS_ALLFRAMES     18  /* First draw text, then draw the cursor and selections; generate all text flows and frames */
#define TX_DRAW_MARKERS_TEXT_ALLFRAMES     22  /* First draw the cursor and selections, then draw text; generate all text flows and frames */


/* Text formatting/styling attribute identifiers */
#define TX_ATTR_BUFFER                     1024
#define TX_ATTR_UNUSED                        0
/* Typography */
#define TX_ATTR_FONT_INDEX                    1
#define TX_ATTR_FONT_UNIQUEID                 2
#define TX_ATTR_FONT_MMCONTRIBUTIONS          3
#define TX_ATTR_FONT_WIDTH                    4
#define TX_ATTR_FONT_HEIGHT                   5
#define TX_ATTR_SKEW_HOR                      6
#define TX_ATTR_SKEW_VER                      7
#define TX_ATTR_ROTATION                      8
#define TX_ATTR_EFFECT                        9
#define TX_ATTR_EFFECT_PARAM0                54
#define TX_ATTR_EFFECT_PARAM1                55
#define TX_ATTR_EFFECT_PARAM2                56
#define TX_ATTR_EFFECT_PARAM3                57
#define TX_ATTR_EFFECT_PARAM4                58
/* Text Body */
#define TX_ATTR_BODY                         38
#define TX_ATTR_BODY_RGBT                    10
#define TX_ATTR_BODY_EP                      11
#define TX_ATTR_BODY_EFFECTS                 60
/* Text Shadow */
#define TX_ATTR_SHADOW                       39
#define TX_ATTR_SHADOW_DX                    12
#define TX_ATTR_SHADOW_DY                    13
#define TX_ATTR_SHADOW_RGBT                  14
#define TX_ATTR_SHADOW_EP                    15
#define TX_ATTR_SHADOW_EFFECTS               61
/* Text Outline */
#define TX_ATTR_OUTLINE                      40
#define TX_ATTR_OUTLINE_TREATMENT            37
#define TX_ATTR_OUTLINE_THICKNESS            16
#define TX_ATTR_OUTLINE_SEGMENT              17
#define TX_ATTR_OUTLINE_DASHSIZE             69
#define TX_ATTR_OUTLINE_RGBT                 18
#define TX_ATTR_OUTLINE_EP                   19
#define TX_ATTR_OUTLINE_EFFECTS              62
/* Text Underline */
#define TX_ATTR_UNDERLINE                    44
#define TX_ATTR_UNDERLINE_THICKNESS          45
#define TX_ATTR_UNDERLINE_SEGMENT            46
#define TX_ATTR_UNDERLINE_DASHSIZE           70
#define TX_ATTR_UNDERLINE_RGBT               47
#define TX_ATTR_UNDERLINE_EP                 48
#define TX_ATTR_UNDERLINE_EFFECTS            63
/* Text Layout */
#define TX_ATTR_TEXTLAYOUT                   41
#define TX_ATTR_SPACING_LETTER               20
#define TX_ATTR_SPACING_ROW                  21
#define TX_ATTR_SPACING_BASELINE             43
#define TX_ATTR_SPACING_FACTOR               49
#define TX_ATTR_DX                           22
#define TX_ATTR_DY                           23
#define TX_ATTR_ALIGN                        24
#define TX_ATTR_ALIGNEND                     25
#define TX_ATTR_VALIGN_ROW                   59
#define TX_ATTR_KERNING                      26
#define TX_ATTR_HINTING                      27
#define TX_ATTR_POSITIONING                  28
#define TX_ATTR_DIRECTION                    29
#define TX_ATTR_ORIENTATION                  30
/* Text Ruler */
#define TX_ATTR_TEXTRULER                    42
#define TX_ATTR_TABSTOP_POSITIONS            31
#define TX_ATTR_TABSTOP_FLAGS                32
#define TX_ATTR_MARGIN_LEFT                  67
#define TX_ATTR_MARGIN_RIGHT                 68
#define TX_ATTR_INDENT_LEFT                  33
#define TX_ATTR_INDENT_RIGHT                 34
/* Miscellaneous */
#define TX_ATTR_ENCODING                     35
#define TX_ATTR_SHAPING                      36
#define TX_ATTR_SHAPING_PARAM0               66
#define TX_ATTR_BREAK_FLAGS                  50
#define TX_ATTR_HYPHENATION                  51
#define TX_ATTR_MISC_ATTRIBS                 64
#define TX_ATTR_LANGUAGE                     65
#define TX_ATTR_POWERFONT_PTR                52 /* not supported */
#define TX_ATTR_USER_SPECIFIC_CONTENT        53
/* Typographic Frame Body */
#define TX_ATTR_TYPOFRAME_BODY               71
#define TX_ATTR_TYPOFRAME_BODY_RGBT          72
#define TX_ATTR_TYPOFRAME_BODY_EP            73
#define TX_ATTR_TYPOFRAME_BODY_EFFECTS       74
/* Typographic Frame Shadow */
#define TX_ATTR_TYPOFRAME_SHADOW             75
#define TX_ATTR_TYPOFRAME_SHADOW_DX          76
#define TX_ATTR_TYPOFRAME_SHADOW_DY          77
#define TX_ATTR_TYPOFRAME_SHADOW_RGBT        78
#define TX_ATTR_TYPOFRAME_SHADOW_EP          79
#define TX_ATTR_TYPOFRAME_SHADOW_EFFECTS     80
/* Typographic Frame Outline */
#define TX_ATTR_TYPOFRAME_OUTLINE            81
#define TX_ATTR_TYPOFRAME_OUTLINE_TREATMENT  82
#define TX_ATTR_TYPOFRAME_OUTLINE_THICKNESS  83
#define TX_ATTR_TYPOFRAME_OUTLINE_SEGMENT    84
#define TX_ATTR_TYPOFRAME_OUTLINE_DASHSIZE   85
#define TX_ATTR_TYPOFRAME_OUTLINE_RGBT       86
#define TX_ATTR_TYPOFRAME_OUTLINE_EP         87
#define TX_ATTR_TYPOFRAME_OUTLINE_EFFECTS    88
/* Text Extras - Row Boundary Body */
#define TX_ATTR_ROWBOUND_BODY                89
#define TX_ATTR_ROWBOUND_BODY_RGBT           90
#define TX_ATTR_ROWBOUND_BODY_EP             91
#define TX_ATTR_ROWBOUND_BODY_EFFECTS        92
/* Text Extras - Row Boundary Shadow */
#define TX_ATTR_ROWBOUND_SHADOW              93
#define TX_ATTR_ROWBOUND_SHADOW_DX           94
#define TX_ATTR_ROWBOUND_SHADOW_DY           95
#define TX_ATTR_ROWBOUND_SHADOW_RGBT         96
#define TX_ATTR_ROWBOUND_SHADOW_EP           97
#define TX_ATTR_ROWBOUND_SHADOW_EFFECTS      98
/* Text Extras - Row Boundary Outline */
#define TX_ATTR_ROWBOUND_OUTLINE             99
#define TX_ATTR_ROWBOUND_OUTLINE_TREATMENT  100
#define TX_ATTR_ROWBOUND_OUTLINE_THICKNESS  101
#define TX_ATTR_ROWBOUND_OUTLINE_SEGMENT    102
#define TX_ATTR_ROWBOUND_OUTLINE_DASHSIZE   103
#define TX_ATTR_ROWBOUND_OUTLINE_RGBT       104
#define TX_ATTR_ROWBOUND_OUTLINE_EP         105
#define TX_ATTR_ROWBOUND_OUTLINE_EFFECTS    106
/* Text Extras - Row Boundary General */
#define TX_ATTR_ROWBOUND_MARGIN_LEFT        107
#define TX_ATTR_ROWBOUND_MARGIN_RIGHT       108
#define TX_ATTR_ROWBOUND_MARGIN_TOP         109
#define TX_ATTR_ROWBOUND_MARGIN_BOTTOM      110
#define TX_ATTR_ROWBOUND_ROUND              111
#define TX_ATTR_ROWBOUND_MERGE              112
/* Text Extras - Baseline Line */
#define TX_ATTR_BASELINE_LINE               113
#define TX_ATTR_BASELINE_LINE_THICKNESS     114
#define TX_ATTR_BASELINE_LINE_SEGMENT       115
#define TX_ATTR_BASELINE_LINE_DASHSIZE      116
#define TX_ATTR_BASELINE_LINE_START         117
#define TX_ATTR_BASELINE_LINE_END           118
#define TX_ATTR_BASELINE_LINE_RGBT          119
#define TX_ATTR_BASELINE_LINE_EP            120
#define TX_ATTR_BASELINE_LINE_EFFECTS       121
/* Text Extras - Baseline General */
#define TX_ATTR_BASELINE_MARGIN_LEFT        122
#define TX_ATTR_BASELINE_MARGIN_RIGHT       123
/* Text Extras - Baseline Shadow */
#define TX_ATTR_BASELINE_SHADOW             124
#define TX_ATTR_BASELINE_SHADOW_DX          125
#define TX_ATTR_BASELINE_SHADOW_DY          126
#define TX_ATTR_BASELINE_SHADOW_RGBT        127
#define TX_ATTR_BASELINE_SHADOW_EP          128
#define TX_ATTR_BASELINE_SHADOW_EFFECTS     129
#define TX_ATTR_FONT_UNIQUEID_RULE          130
#define TX_ATTR_FONT_UNIQUEID_RULE_CHECK    131
/* Count/End */
#define TX_ATTR_COUNT                       132
#define TX_ATTR_END                          -1


/* Text hit actions */
#define TX_HIT_MOVE_ALT  -5   /* Mouse move - alt method */
#define TX_HIT_AREAFOCUS -4   /* Focus textflow area */
#define TX_HIT_RELEASE   -3   /* Mouse button released */
#define TX_HIT_MOVE      -2   /* Mouse move */
#define TX_HIT_NA        -1
#define TX_HIT_NORMAL     0   /* Normal mouse button click */
#define TX_HIT_DOUBLE     1   /* Double mouse button click - selects a word (but only looks for the space character to find the word boundaries) */
#define TX_HIT_TRIPLE     4   /* Triple mouse button click - selects a paragraph (by looking for the newline character to find the paragraph boundaries) */
#define TX_HIT_SHIFT      2   /* Mouse button click with the Shift key */
#define TX_HIT_CTRL       3   /* Mouse button click with the Ctrl key */
#define TX_HIT_ALT        5   /* Mouse button click with the Alt key */
#define TX_HIT_CTRLALT    6   /* Mouse button click with the Ctrl + Alt key */
#define TX_HIT_ENDTEXT    100
#define TX_HIT_SELECTFLOW 101


/* Text commands */

//#define TX_CMD_SELECTFIRSTWORD       -4    /* Select first word (obsolete) */
//#define TX_CMD_SELECTFIRSTLINE       -6    /* Select first line (obsolete) */
//#define TX_CMD_SELECTFIRSTPARAGRAPH  -5    /* Select first paragraph (obsolete) */
//#define TX_CMD_SELECTFIRSTAREA       -7    /* Select first text area (obsolete) */
//#define TX_CMD_SELECTALL             -2    /* Same as TX_CMD_SELECTTEXT (obsolete) */

#define TX_CMD_RESET                 -3    /* Deselect all text and remove cursor */
#define TX_CMD_SELECTFLOW            -1    /* Select the current text flow */
#define TX_CMD_SELECTWORD            -8    /* Select entire word */
#define TX_CMD_SELECTWORDS           -13   /* Select entire words (extend existing selections) */
#define TX_CMD_SELECTLINE            -9    /* Select entire line */
#define TX_CMD_SELECTPARAGRAPH       -10   /* Select entire paragraph */
#define TX_CMD_SELECTPARAGRAPHS      -14   /* Select entire paragraphs (extend existing selections) */
#define TX_CMD_SELECTAREA            -11   /* Select entire text area */
#define TX_CMD_SELECTTEXT            -12   /* Select all text */

#define TX_CMD_DEBUG_DUMPFLOW        -100

#define TX_CMD_LEFT                   3    /* Go to previous visible character */
#define TX_CMD_RIGHT                  4    /* Go to next visible character */
#define TX_CMD_UP                     5    /* Go to previous row */
#define TX_CMD_DOWN                   6    /* Go to next row */
#define TX_CMD_SHIFT_LEFT             7    /* Shift + Go to previous visible character */
#define TX_CMD_SHIFT_RIGHT            8    /* Shift + Go to next visible character */
#define TX_CMD_SHIFT_UP               9    /* Shift + Go to previous row */
#define TX_CMD_SHIFT_DOWN             10   /* Shift + Go to next row */

#define TX_CMD_SET                    11   /* Set cursor to closest visible position */
#define TX_CMD_CURRCHAR               12   /* Go to current character */
#define TX_CMD_CURRTEXT               53   /* Same as TX_CMD_CURRCHAR but safer (checks bounds in case the text is changed externally) */

#define TX_CMD_PREVCHAR               13   /* Go to previous character */
#define TX_CMD_PREVWORD               14   /* Go to previous word */
#define TX_CMD_PREVLINE               15   /* Go to previous text line */
#define TX_CMD_PREVPARAGRAPH          16   /* Go to previous paragraph */
#define TX_CMD_PREVAREA               17   /* Go to previous text area */
#define TX_CMD_NEXTCHAR               18   /* Go to next character */
#define TX_CMD_NEXTWORD               19   /* Go to next word */
#define TX_CMD_NEXTLINE               20   /* Go to next text line */
#define TX_CMD_NEXTPARAGRAPH          21   /* Go to next paragraph */
#define TX_CMD_NEXTAREA               22   /* Go to next text area */

#define TX_CMD_SHIFT_PREVCHAR         23   /* Shift + Go to previous character */
#define TX_CMD_SHIFT_PREVWORD         24   /* Shift + Go to previous word */
#define TX_CMD_SHIFT_PREVLINE         25   /* Shift + Go to previous text line */
#define TX_CMD_SHIFT_PREVPARAGRAPH    26   /* Shift + Go to previous paragraph */
#define TX_CMD_SHIFT_PREVAREA         27   /* Shift + Go to previous text area */
#define TX_CMD_SHIFT_NEXTCHAR         28   /* Shift + Go to next character */
#define TX_CMD_SHIFT_NEXTWORD         29   /* Shift + Go to next word */
#define TX_CMD_SHIFT_NEXTLINE         30   /* Shift + Go to next text line */
#define TX_CMD_SHIFT_NEXTPARAGRAPH    31   /* Shift + Go to next paragraph */
#define TX_CMD_SHIFT_NEXTAREA         32   /* Shift + Go to next text area */

#define TX_CMD_STARTWORD              33   /* Go to start of word */
#define TX_CMD_STARTLINE              34   /* Go to start of text line */
#define TX_CMD_STARTPARAGRAPH         35   /* Go to start of paragraph */
#define TX_CMD_STARTAREA              36   /* Go to start of text area */
#define TX_CMD_STARTTEXT              37   /* Go to start of text */
#define TX_CMD_ENDWORD                38   /* Go to end of word */
#define TX_CMD_ENDLINE                39   /* Go to end of text line */
#define TX_CMD_ENDPARAGRAPH           40   /* Go to end of paragraph */
#define TX_CMD_ENDAREA                41   /* Go to end of text area */
#define TX_CMD_ENDTEXT                42   /* Go to end of text */

#define TX_CMD_SHIFT_STARTWORD        43   /* Shift + Go to start of word */
#define TX_CMD_SHIFT_STARTLINE        44   /* Shift + Go to start of text line */
#define TX_CMD_SHIFT_STARTPARAGRAPH   45   /* Shift + Go to start of paragraph */
#define TX_CMD_SHIFT_STARTAREA        46   /* Shift + Go to start of text area */
#define TX_CMD_SHIFT_STARTTEXT        47   /* Shift + Go to start of text */
#define TX_CMD_SHIFT_ENDWORD          48   /* Shift + Go to end of word */
#define TX_CMD_SHIFT_ENDLINE          49   /* Shift + Go to end of text line */
#define TX_CMD_SHIFT_ENDPARAGRAPH     50   /* Shift + Go to end of paragraph */
#define TX_CMD_SHIFT_ENDAREA          51   /* Shift + Go to end of text area */
#define TX_CMD_SHIFT_ENDTEXT          52   /* Shift + Go to end of text */

//#define TX_CMD_PASTE                  0    /* obsolete */
//#define TX_CMD_BACKSPACE              1    /* obsolete */
//#define TX_CMD_DELETE                 2    /* obsolete */


/* Text paste commands */
#define TX_PASTE_TEXT               0    /* Paste text */
#define TX_PASTE_BACKSPACE          1    /* Paste the BAKSPACE key */
#define TX_PASTE_DELETE             2    /* Paste the DELETE key */
#define TX_PASTE_TEXT_REFORMAT      128  /* Paste text and reformat the paragraph */
#define TX_PASTE_BACKSPACE_REFORMAT 129  /* Paste the BAKSPACE key and reformat the paragraph */
#define TX_PASTE_DELETE_REFORMAT    130  /* Paste the DELETE key and reformat the paragraph */


/* Buffer formats */
#define TX_BUFFER_FORMAT_1   1   /* 1 byte per character - for ANSI */
#define TX_BUFFER_FORMAT_2   2   /* 2 bytes per character - for UCS2 */
#define TX_BUFFER_FORMAT_3   3   /* 3 bytes per character */
#define TX_BUFFER_FORMAT_4   4   /* 4 bytes per character - for UCS4 */

/* Text info identifiers */
#define TX_INFO_TEXTAREA   0  /* Info about a textflow area */
#define TX_INFO_FRAMESET   1  /* Info about a frameset */
#define TX_INFO_CURSOR     2  /* Info about a cursor */
#define TX_INFO_COMMAND    3  /* Info about a text command */
#define TX_INFO_SELECTED   4  /* Info about the selected textflow area */
#define TX_INFO_HIT        5  /* Info about the textflow area that encloses a hit point */

/* Defines - Miscellaneous Numeric Values */
#define TX_NVAL_THICKNESS_TEXTFLOW_TOP    0
#define TX_NVAL_THICKNESS_TEXTFLOW_NESTED 1
#define TX_NVAL_DEFAULT_FONT_WIDTH        2
#define TX_NVAL_DEFAULT_FONT_HEIGHT       3
#define TX_NVAL_DEFAULT_SPACING_ROW       4
#define TX_NVAL_ENABLE_BITS               5
#define TX_NVAL_CONFIG_BITS               6


/* Some of the commonly used text formatting/styling attributes */
const DT_TX_ATTRIBS attrRed[]           = {{TX_ATTR_BODY_RGBT, {"FF000000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrRedDark[]       = {{TX_ATTR_BODY_RGBT, {"A0000000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrGreen[]         = {{TX_ATTR_BODY_RGBT, {"00FF0000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrGreenDark[]     = {{TX_ATTR_BODY_RGBT, {"00A00000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrGreenForest[]   = {{TX_ATTR_BODY_RGBT, {"80800000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrGreenGrass[]    = {{TX_ATTR_BODY_RGBT, {"40808000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrBlue[]          = {{TX_ATTR_BODY_RGBT, {"0000FF00"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrBlueDark[]      = {{TX_ATTR_BODY_RGBT, {"0000A000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrWhite[]         = {{TX_ATTR_BODY_RGBT, {"FFFFFF00"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrBlack[]         = {{TX_ATTR_BODY_RGBT, {"00000000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrGray[]          = {{TX_ATTR_BODY_RGBT, {"A0A0A000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrGrayDark[]      = {{TX_ATTR_BODY_RGBT, {"80808000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrYellow[]        = {{TX_ATTR_BODY_RGBT, {"FFFF0000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrBurgundy[]      = {{TX_ATTR_BODY_RGBT, {"80000000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrBrown[]         = {{TX_ATTR_BODY_RGBT, {"80400000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrOrange[]        = {{TX_ATTR_BODY_RGBT, {"FF804000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrPink[]          = {{TX_ATTR_BODY_RGBT, {"FF00FF00"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrPurpleDark[]    = {{TX_ATTR_BODY_RGBT, {"80008000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrPurpleLight[]   = {{TX_ATTR_BODY_RGBT, {"FF008000"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrTurquoise[]     = {{TX_ATTR_BODY_RGBT, {"00FFFF00"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrArial[]         = {{TX_ATTR_FONT_UNIQUEID, {"F0010_ARIAL__NS0"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrTimes[]         = {{TX_ATTR_FONT_UNIQUEID, {"F0011_TIMES__NS0"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrPalatino[]      = {{TX_ATTR_FONT_UNIQUEID, {"F0011_PALATN_NS0"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrBold[]          = {{TX_ATTR_BODY_EP, {"2202"}}, {TX_ATTR_SPACING_LETTER, {"2"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrItalic[]        = {{TX_ATTR_SKEW_HOR, {"-12"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrBig[]           = {{TX_ATTR_FONT_WIDTH, {"54"}}, {TX_ATTR_FONT_HEIGHT, {"54"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrSmall[]         = {{TX_ATTR_FONT_WIDTH, {"24"}}, {TX_ATTR_FONT_HEIGHT, {"24"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrCondensed[]     = {{TX_ATTR_FONT_WIDTH, {"24"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrExpanded[]      = {{TX_ATTR_FONT_WIDTH, {"54"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrSubscript[]     = {{TX_ATTR_FONT_WIDTH, {"20"}}, {TX_ATTR_FONT_HEIGHT, {"20"}}, {TX_ATTR_DY, {"10"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrSuperscript[]   = {{TX_ATTR_FONT_WIDTH, {"20"}}, {TX_ATTR_FONT_HEIGHT, {"20"}}, {TX_ATTR_DY, {"-10"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrUnderline[]     = {{TX_ATTR_EFFECT, {"1"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrStrikethru[]    = {{TX_ATTR_EFFECT, {"8"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrLeft[]          = {{TX_ATTR_ALIGN, {"0"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrCenter[]        = {{TX_ATTR_ALIGN, {"1"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrRight[]         = {{TX_ATTR_ALIGN, {"2"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrJustified[]     = {{TX_ATTR_ALIGN, {"3"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrLeftFull[]      = {{TX_ATTR_ALIGN, {"0"}}, {TX_ATTR_ALIGNEND, {"0"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrCenterFull[]    = {{TX_ATTR_ALIGN, {"1"}}, {TX_ATTR_ALIGNEND, {"1"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrRightFull[]     = {{TX_ATTR_ALIGN, {"2"}}, {TX_ATTR_ALIGNEND, {"2"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrJustifiedFull[] = {{TX_ATTR_ALIGN, {"3"}}, {TX_ATTR_ALIGNEND, {"3"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrHyphenationEN[] = {{TX_ATTR_HYPHENATION, {"1"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrHyphenationFR[] = {{TX_ATTR_HYPHENATION, {"2"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrHyphenationDE[] = {{TX_ATTR_HYPHENATION, {"3"}}, {TX_ATTR_END, {""}}};
const DT_TX_ATTRIBS attrHyphenationES[] = {{TX_ATTR_HYPHENATION, {"4"}}, {TX_ATTR_END, {""}}};


/* D-Type Text Engine Functions */

/* Create and initialize D-Type Text Engine - via stream */
DT_SWORD DTYPE_APICALL txEngineIniViaStream(DT_TXENGINE* engine, const DT_STREAM_DESC* sd, DT_INIT_INFO* init_info);
/* Deinitialize and destroy D-Type Text Engine */
void DTYPE_APICALL txEngineExt(DT_TXENGINE engine);

/* Get D-Type Text Engine version */
DT_SLONG DTYPE_APICALL txEngineGetVersion(DT_TXENGINE engine, DT_CHAR* ver_name, DT_CHAR* ver_snum, DT_CHAR* reserved);
/* Get associated D-Type Power Engine Instance */
void DTYPE_APICALL txEngineGetPowerEngInst(DT_TXENGINE engine, DT_PDENGINE* power_engine);
/* Get associated D-Type Standard Engine Instance */
void DTYPE_APICALL txEngineGetDTypeEngInst(DT_TXENGINE engine, DT_DTENGINE* dtype_engine);

/* Make cached Fontmap */
DT_SLONG DTYPE_APICALL txEngineMakeCachedFontmap(DT_TXENGINE engine, const DT_STREAM_DESC* input_fontmap_sd, DT_STREAM_DESC* output_fontmap_sd, DT_SWORD reserved);

/* Create and initialize empty text document */
DT_SLONG DTYPE_APICALL txTextIni(DT_TXDOC* text_doc);
/* Create and initialize text document - via PowerDoc and buffer */
DT_SLONG DTYPE_APICALL txTextIniViaPowerDocAndBuffer(DT_TXDOC* text_doc, DT_PDDOC power_doc, DT_SLONG nr_of_chars, const DT_UBYTE* buffer, DT_ID_SWORD buffer_format, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA* area, const DT_TX_DEFAULT_ATTRIBS* attribs, const DT_STREAM_DESC* fontmap_sd, DT_SWORD reserved);
/* Create and initialize text document - via PowerDoc and stream */
DT_SLONG DTYPE_APICALL txTextIniViaPowerDocAndStream(DT_TXDOC* text_doc, DT_PDDOC power_doc, DT_ID_UBYTE text_type, const DT_STREAM_DESC* text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA* area, const DT_TX_DEFAULT_ATTRIBS* attribs, const DT_STREAM_DESC* fontmap_sd, DT_SWORD reserved);
/* Create and initialize text document - via PowerDoc */
DT_SLONG DTYPE_APICALL txTextIniViaPowerDoc(DT_TXDOC* text_doc, DT_PDDOC power_doc);
/* Create and initialize text document - via buffer */
DT_SLONG DTYPE_APICALL txTextIniViaBuffer(DT_TXDOC* text_doc, DT_TXENGINE engine, DT_SLONG nr_of_chars, const DT_UBYTE* buffer, DT_ID_SWORD buffer_format, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA* area, const DT_TX_ATTRIBS text_attribs[], const DT_STREAM_DESC* fontmap_sd);
/* Create and initialize text document - via stream */
DT_SLONG DTYPE_APICALL txTextIniViaStream(DT_TXDOC* text_doc, DT_TXENGINE engine, DT_ID_UBYTE text_type, const DT_STREAM_DESC* text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA* area, const DT_TX_ATTRIBS text_attribs[], const DT_STREAM_DESC* fontmap_sd);
/* Deinitialize and destroy text document */
void DTYPE_APICALL txTextExt(DT_TXDOC text_doc);

/* Make text document - via PowerDoc and buffer */
DT_SLONG DTYPE_APICALL txTextMakeViaPowerDocAndBuffer(DT_TXDOC text_doc, DT_PDDOC power_doc, DT_SLONG nr_of_chars, const DT_UBYTE* buffer, DT_ID_SWORD buffer_format, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA* area, const DT_TX_DEFAULT_ATTRIBS* attribs, const DT_STREAM_DESC* fontmap_sd, DT_SWORD reserved);
/* Make text document - via PowerDoc and stream */
DT_SLONG DTYPE_APICALL txTextMakeViaPowerDocAndStream(DT_TXDOC text_doc, DT_PDDOC power_doc, DT_ID_UBYTE text_type, const DT_STREAM_DESC* text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA* area, const DT_TX_DEFAULT_ATTRIBS* attribs, const DT_STREAM_DESC* fontmap_sd, DT_SWORD reserved);
/* Make text document - via PowerDoc */
DT_SLONG DTYPE_APICALL txTextMakeViaPowerDoc(DT_TXDOC text_doc, DT_PDDOC power_doc);
/* Make text document - via buffer */
DT_SLONG DTYPE_APICALL txTextMakeViaBuffer(DT_TXDOC text_doc, DT_TXENGINE engine, DT_SLONG nr_of_chars, const DT_UBYTE* buffer, DT_ID_SWORD buffer_format, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA* area, const DT_TX_ATTRIBS text_attribs[], const DT_STREAM_DESC* fontmap_sd);
/* Make text document - via stream */
DT_SLONG DTYPE_APICALL txTextMakeViaStream(DT_TXDOC text_doc, DT_TXENGINE engine, DT_ID_UBYTE text_type, const DT_STREAM_DESC* text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA* area, const DT_TX_ATTRIBS text_attribs[], const DT_STREAM_DESC* fontmap_sd);

/* Set text document's numeric value */
void DTYPE_APICALL txTextSetNumericValue(DT_TXDOC text_doc, DT_ID_SWORD numeric_id, DT_SLONG numeric_value);
/* Set text document's Layout Engine */
DT_SWORD DTYPE_APICALL txTextSetLayoutEngine(DT_TXDOC text_doc, DT_ID_UBYTE layout_engine);
/* Set text document's parameters */
DT_SWORD DTYPE_APICALL txTextSetParams(DT_TXDOC text_doc, const DT_TX_DOC_PARAMS* params, DT_SLONG reserved);
/* Set text document's internals */
DT_SWORD DTYPE_APICALL txTextSetInternals(DT_TXDOC text_doc, const DT_UBYTE internals[], DT_SLONG reserved);
/* Set visibility filter */
DT_SWORD DTYPE_APICALL txTextSetFilter(DT_TXDOC text_doc, DT_SWORD reserved, DT_ULONG filter);
/* Set text document's highlighter */
DT_SWORD DTYPE_APICALL txTextSetHighlighter(DT_TXDOC text_doc, const DT_UBYTE cursor_blend_arr[], const DT_UBYTE select_blend_arr[], DT_SWORD reserved);
/* Set text document's highlighter */
DT_SWORD DTYPE_APICALL txTextSetHighlighterPlus(DT_TXDOC text_doc, const DT_UBYTE cursor_blend_arr[], const DT_UBYTE select_blend_arr[], const DT_UBYTE blanks_blend_arr[], DT_SWORD reserved);
/* Set text document's transform - 2x2 transformation matrix */
void DTYPE_APICALL txTextSetTransform(DT_TXDOC text_doc, const DT_TM2X2 transform);
/* Set text document's transform - 3x3 transformation matrix */
void DTYPE_APICALL txTextSetTransformPlus(DT_TXDOC text_doc, const DT_TM3X3 transform);
/* Set text document's transform - simple scale */
void DTYPE_APICALL txTextSetScale(DT_TXDOC text_doc, DT_FLOAT scale);
/* Format (i.e. style) text document */
DT_SWORD DTYPE_APICALL txTextSetAttribs(DT_TXDOC text_doc, DT_SLONG first_char, DT_SLONG last_char, const DT_TX_ATTRIBS text_attribs[], DT_SWORD reserved);
/* Format (i.e. style) text document - via struct */
DT_SLONG DTYPE_APICALL txTextSetAttribsViaStruct(DT_TXDOC text_doc, DT_SLONG first_char, DT_SLONG last_char, const DT_TX_FORMAT* text_format, DT_SWORD reserved);
/* Set textflow areas for the text document */
DT_SWORD DTYPE_APICALL txTextSetFlow(DT_TXDOC text_doc, DT_SLONG nr_of_areas, const DT_TX_TEXTFLOW_AREA areas[], DT_SWORD reserved);
/* Set textflow areas for the text document (enhanced) */
DT_SWORD DTYPE_APICALL txTextSetFlowPlus(DT_TXDOC text_doc, DT_SLONG nr_of_areas, const DT_TX_TEXTFLOW_AREA areas[], const DT_SLONG pages[], DT_SWORD reserved);
/* Add textflow areas for the text document */
DT_SWORD DTYPE_APICALL txTextAddFlow(DT_TXDOC text_doc, DT_ID_SLONG obj, DT_SLONG nr_of_areas, const DT_TX_TEXTFLOW_AREA areas[], DT_SWORD reserved);
/* Add textflow areas for the text document (enhanced) */
DT_SWORD DTYPE_APICALL txTextAddFlowPlus(DT_TXDOC text_doc, DT_ID_SLONG obj, DT_SLONG nr_of_areas, const DT_TX_TEXTFLOW_AREA areas[], const DT_SLONG pages[], DT_SWORD reserved);
/* Set text document's char-to-script callback function */
DT_SWORD DTYPE_APICALL txTextSetScriptCallback(DT_TXDOC text_doc, DT_ID_SLONG (DTYPE_USRCALL *user_script_func)(DT_SLONG index, DT_ID_ULONG char_code, DT_ID_SLONG script_code, DT_ULONG* flag, void* user_param), void* user_param);

/* Get associated Text Engine Instance */
void DTYPE_APICALL txTextGetTextEngInst(DT_TXDOC text_doc, DT_TXENGINE* engine);
/* Get associated PowerDoc document */
void DTYPE_APICALL txTextGetPowerDoc(DT_TXDOC text_doc, DT_PDDOC* power_doc);
/* Get text document's numeric value */
DT_SLONG DTYPE_APICALL txTextGetNumericValue(DT_TXDOC text_doc, DT_ID_SWORD numeric_id);
/* Get text document's Layout Engine */
DT_ID_UBYTE DTYPE_APICALL txTextGetLayoutEngine(DT_TXDOC text_doc);
/* Get text document's parameters */
DT_SWORD DTYPE_APICALL txTextGetParams(DT_TXDOC text_doc, DT_TX_DOC_PARAMS* params, DT_SLONG reserved);
/* Get text document's internals */
DT_SWORD DTYPE_APICALL txTextGetInternals(DT_TXDOC text_doc, DT_UBYTE internals[], DT_SLONG reserved);
/* Get visibility filter */
DT_ULONG DTYPE_APICALL txTextGetFilter(DT_TXDOC text_doc, DT_SWORD reserved);
/* Get text document's highlighter */
DT_SWORD DTYPE_APICALL txTextGetHighlighter(DT_TXDOC text_doc, DT_UBYTE cursor_blend_arr[], DT_UBYTE select_blend_arr[], DT_SWORD reserved);
/* Get text document's highlighter */
DT_SWORD DTYPE_APICALL txTextGetHighlighterPlus(DT_TXDOC text_doc, DT_UBYTE cursor_blend_arr[], DT_UBYTE select_blend_arr[], DT_UBYTE blanks_blend_arr[], DT_SWORD reserved);
/* Get text document's transform - 2x2 transformation matrix */
void DTYPE_APICALL txTextGetTransform(DT_TXDOC text_doc, DT_TM2X2 transform);
/* Get text document's transform - 3x3 transformation matrix */
void DTYPE_APICALL txTextGetTransformPlus(DT_TXDOC text_doc, DT_TM3X3 transform);

/* Draw textflow areas on the specified page of the text document */
DT_SLONG DTYPE_APICALL txTextDraw(DT_TXDOC text_doc, DT_SLONG page, DT_SRAST_L x_off, DT_SRAST_L y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_MDC* memory_surface, DT_PD_DOCDRAW_PARAMS* params, DT_ID_SWORD flags);

/* Register text edit configuration */
void DTYPE_APICALL txTextRegisterConfig(DT_TXDOC text_doc, const DT_TX_TEXTEDIT_CONFIG* config);
/* Query text edit configuration */
const DT_TX_TEXTEDIT_CONFIG* DTYPE_APICALL txTextGetConfig(DT_TXDOC text_doc, DT_SLONG index, DT_SWORD fn_draw);

/* Free memory allocated by the Engine */
void DTYPE_APICALL txFree(void* mem_addr);

/* Auxiliary Functions */

/* Process text hit actions - enhanced version */
DT_SLONG DTYPE_APICALL txTextHitPlus(DT_TXDOC text_doc, DT_ID_SWORD hit_type, DT_SRAST_L x, DT_SRAST_L y, const DT_TX_TEXT_FLOW* text_flow, DT_RECT_SLONG* extent, DT_SLONG reserved);
/* Process text hit actions */
DT_SWORD DTYPE_APICALL txTextHit(DT_TXDOC text_doc, DT_ID_SWORD hit_type, DT_SRAST_L x, DT_SRAST_L y, DT_RECT_SLONG* extent, DT_SLONG reserved);
/* Process text commands */
DT_SWORD DTYPE_APICALL txTextCommand(DT_TXDOC text_doc, DT_ID_SWORD command, DT_RECT_SLONG* extent, DT_SLONG reserved);
/* Refresh state */
DT_SWORD DTYPE_APICALL txTextRefresh(DT_TXDOC text_doc, DT_SLONG area_index);

/* Set text selection(s) */
DT_SWORD DTYPE_APICALL txTextSetSelect(DT_TXDOC text_doc, const DT_SLONG first_char[], const DT_SLONG last_char[], DT_SLONG array_len, DT_RECT_SLONG* extent, DT_SLONG reserved);
/* Get text selection(s) */
DT_SWORD DTYPE_APICALL txTextGetSelect(DT_TXDOC text_doc, DT_SLONG first_char[], DT_SLONG last_char[], DT_SLONG max_array_len);
/* Get total size of text selection(s) */
DT_SLONG DTYPE_APICALL txTextGetSelectSize(DT_TXDOC text_doc);
/* Check text selection(s) */
DT_SWORD DTYPE_APICALL txTextCheckSelect(DT_TXDOC text_doc, DT_SWORD font_index, DT_ULONG problematic_char_arr[], DT_SLONG arr_size);

/* Copy plain text to user-supplied buffer */
DT_SWORD DTYPE_APICALL txTextCopy(DT_TXDOC text_doc, DT_UBYTE* buffer, DT_SLONG max_buffer_len, DT_ID_SWORD buffer_format);
/* Paste text into current text selection - via stream */
DT_SLONG DTYPE_APICALL txTextPasteViaStream(DT_TXDOC text_doc, DT_ID_UBYTE paste_command, DT_ID_UBYTE text_type, const DT_STREAM_DESC* text_sd, DT_SWORD unicode_flags, const DT_STREAM_DESC* fontmap_sd);
/* Paste text into current text selection - via buffer */
DT_SLONG DTYPE_APICALL txTextPasteViaBuffer(DT_TXDOC text_doc, DT_ID_UBYTE paste_command, DT_SLONG nr_of_chars, const DT_UBYTE* buffer, DT_ID_SWORD buffer_format, DT_SWORD unicode_flags, const DT_STREAM_DESC* fontmap_sd);
/* Paste another text document into current text selection */
DT_SLONG DTYPE_APICALL txTextPasteViaDoc(DT_TXDOC text_doc, const DT_TXDOC text_doc2, DT_SLONG paste_flags);
/* Paste a PowerDoc object into current text selection */
DT_SLONG DTYPE_APICALL txTextPasteViaPowerObj(DT_TXDOC text_doc, DT_PDDOC power_doc, DT_SLONG obj, DT_SLONG paste_flags);
/* Paste a PowerDoc object into current text selection with format exclusions */
DT_SLONG DTYPE_APICALL txTextPasteViaPowerObjPlus(DT_TXDOC text_doc, DT_PDDOC power_doc, DT_SLONG obj, DT_SLONG paste_flags, const DT_ULONG exclusions[]);
/* Paste a PowerDoc object into current text selection - obsolete */
/*DT_SWORD DTYPE_APICALL txTextPasteObj(DT_TXDOC text_doc, DT_PDDOC power_doc, DT_SLONG obj, DT_SLONG paste_flags);*/
/* Paste another text document into current text selection - obsolete */
/*DT_SWORD DTYPE_APICALL txTextPaste(DT_TXDOC text_doc, const DT_TXDOC text_doc2, DT_SLONG paste_flags);*/
/* Reflow text */
DT_SLONG DTYPE_APICALL txTextReflow(DT_TXDOC text_doc, DT_SLONG reflow_flags);

/* Get info about various properties, objects or features found in a text document */
DT_SWORD DTYPE_APICALL txTextInfo(DT_TXDOC text_doc, DT_ID_SWORD info_type, DT_SLONG param, DT_SLONG reserved, DT_SLONG* info, DT_SLONG extra[]);

/* For debugging purposes */
DT_SLONG DTYPE_APICALL txTextSaveToFile(DT_TXDOC text_doc, DT_ID_UBYTE file_type, const DT_CHAR* file_name);


#ifdef __cplusplus
}
#endif


#endif /* DTYPE_DTYPETXT_HI_H */
