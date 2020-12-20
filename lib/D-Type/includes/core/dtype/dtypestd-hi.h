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


#ifndef DTYPE_DTYPESTD_HI_H
#define DTYPE_DTYPESTD_HI_H


#ifdef __cplusplus
DT_EXTERN {
#endif


/* ------------------------- D-Type Standard Engine (High-Level API) ------------------------- */

typedef struct DT_DTENGINE_STRUCT* DT_DTENGINE;


/* Structures */


/* D-Type Style (standard version) */
typedef struct
{
	DT_UBYTE ep[2];          /* Effect and effect parameter */
	DT_UBYTE rgbt[4];        /* Color (Red, Green, Blue) and Transparency */
	DT_SWORD reserved;       /* Must be set to 0 */
	const DT_UBYTE* palette; /* Either DV_NULL or a valid pointer to a 256-element array */

} DT_STYLE_ATTRIBS;


/* D-Type Style (advanced version) */
typedef struct
{
	DT_SLONG effects_len;     /* Number of effect values */
	DT_SLONG effects_arr[64]; /* Array of effect values */
	DT_UBYTE rgbt[4];         /* Colour (Red, Green, Blue) and Transparency */
	const DT_UBYTE* palette;  /* Either DV_NULL or a valid pointer to a 256-element array */

} DT_STYLE_EFFECTS;


/* Transformation specified using raw transformation parameters */
typedef struct
{
	DT_SRAST_L size_h, size_v; /* Horizontal and vertical size in pixels and multiplied by TRANSFORM_PARAMS_MUL_H and TRANSFORM_PARAMS_MUL_V (for extra precision) */
	DT_SWORD skew_h, skew_v;   /* Horizontal and vertical skew in degrees */
	DT_SWORD rotation;         /* Rotation in degrees, must be between 0 and 360 */

} DT_TRANSFORM_PARAMS_L;

/* Alias */
typedef DT_TRANSFORM_PARAMS_L DT_TRANSFORM_PARAMS;


/* Transformation specified using a 2-dimensional transformation matrix */
typedef struct
{
	DT_SRAST_L tm_00, tm_01; /* m[0][0] and m[0][1] multiplied by 262144 */
	DT_SRAST_L tm_10, tm_11; /* m[1][0] and m[1][1] multiplied by 262144 */

} DT_TRANSFORM_MATRIX_L;

/* Alias */
typedef DT_TRANSFORM_MATRIX_L DT_TRANSFORM_MATRIX;


/* Transformation to be applied to a glyph or character */
typedef union
{
	DT_TRANSFORM_PARAMS_L params; /* Transformation via raw params */
	DT_TRANSFORM_MATRIX_L matrix; /* Transformation via 2x2 matrix */

} DT_TRANSFORM_L;

/* Alias */
typedef DT_TRANSFORM_L DT_TRANSFORM;


/* Type (typographical properties of a glyph or character) - advanced */
typedef struct
{
	DT_ID_SWORD font_index;   /* Font index from Font Catalog */
	DT_UBYTE reserved;        /* Must be set to 0 */
	DT_ID_UBYTE descriptor;   /* 0=transformation via raw params, 1=transformation via 2x2 matrix */
	DT_TRANSFORM_L transform;
	DT_LINEDECOR_L linedecor;

} DT_TYPE_EFFECTS_L;

/* Alias */
typedef DT_TYPE_EFFECTS_L DT_TYPE_EFFECTS;


/* Transformation specified using raw transformation parameters - XL */
typedef struct
{
	DT_SRAST_XL size_h, size_v; /* Horizontal and vertical size in pixels and multiplied by TRANSFORM_PARAMS_MUL_H and TRANSFORM_PARAMS_MUL_V (for extra precision) */
	DT_SWORD skew_h, skew_v;    /* Horizontal and vertical skew in degrees */
	DT_SWORD rotation;          /* Rotation in degrees, must be between 0 and 360 */

} DT_TRANSFORM_PARAMS_XL;


/* Transformation specified using a 2-dimensional transformation matrix - XL */
typedef struct
{
	DT_SRAST_XL tm_00, tm_01; /* m[0][0] and m[0][1] multiplied by 262144 */
	DT_SRAST_XL tm_10, tm_11; /* m[1][0] and m[1][1] multiplied by 262144 */

} DT_TRANSFORM_MATRIX_XL;


/* Transformation to be applied to a glyph or character - XL */
typedef union
{
	DT_TRANSFORM_PARAMS_XL params; /* Transformation via raw params */
	DT_TRANSFORM_MATRIX_XL matrix; /* Transformation via 2x2 matrix */

} DT_TRANSFORM_XL;


/* Type (typographical properties of a glyph or character) - advanced - XL */
typedef struct
{
	DT_ID_SWORD font_index;    /* Font index from Font Catalog */
	DT_UBYTE reserved;         /* Must be set to 0 */
	DT_ID_UBYTE descriptor;    /* 0=transformation via raw params, 1=transformation via 2x2 matrix */
	DT_TRANSFORM_XL transform;
	DT_LINEDECOR_XL linedecor;

} DT_TYPE_EFFECTS_XL;


/* Type (typographical properties of a glyph or character) - legacy */
typedef struct
{
	DT_ID_SWORD font_index;   /* Font index from Font Catalog */
	DT_SRAST_L thickness;     /* 0=normal, >0=outline, <0=expansion */
	DT_SRAST_L segment;       /* Segment len - used for dashing when thickness<>0 */
	DT_UBYTE reserved;        /* Must be set to 0 */
	DT_ID_UBYTE descriptor;   /* 0=transformation via raw params, 1=transformation via 2x2 matrix */
	DT_TRANSFORM_L transform;

} DT_TYPE_ATTRIBS;


/* Advanced Typography */
typedef DT_ID_ULONG DT_AT_GLYPH_INDEX;


/* A single N-to-M glyph substitution instruction */
typedef struct
{
	DT_SLONG glyph_arr_offset; /* start location in the array of glyphs */
	DT_UBYTE count_in;         /* number of in-glyphs to read (N) */
	DT_UBYTE count_out;        /* number of out-glyphs to read (M) */

} DT_AT_GLYPH_SUBSTS;


/* A set of glyph substitutions for the specified script and feature in a font */
typedef struct
{
	DT_AT_GLYPH_SUBSTS* subst_arr; /* array of substitution instructions */
	DT_AT_GLYPH_INDEX* glyph_arr;  /* array of glyphs */
	DT_SLONG nr_of_substs;         /* number of substitution instructions in the array */
	DT_SLONG nr_of_glyphs;         /* number of glyphs in the array */
	DT_ID_UBYTE script_id;         /* specified script */
	DT_ID_UBYTE feature_id;        /* specified feature */

} DT_AT_GLYPH_SUBSTS_SET;


/* Additional initialization information */
typedef struct
{
	DT_SLONG init_key;    /* Initialization key (0=demo) */
	DT_SWORD init_flag;   /* Initialization flag (0=demo) */
	DT_SWORD init_status; /* Set by D-Type Engine, provides extended initialization information */
	DT_SWORD reserved1;   /* Must be set to 0 */
	DT_SWORD reserved2;   /* Must be set to 0 */
	void* reserved3;      /* Must be set to DV_NULL */
	void* reserved4;      /* Must be set to DV_NULL */

} DT_INIT_INFO;


/* Defines - Font Formats */
#define DV_FONT_FORMAT_AUTO1 -1
#define DV_FONT_FORMAT_AUTO2 -2

#define DV_FONT_DTYPE 0
#define DV_FONT_OPENTYPE_TTF 3
#define DV_FONT_OPENTYPE_CFF 5
#define DV_FONT_TYPE1 8
#define DV_FONT_PSTYPE3 12
#define DV_FONT_BARECFF 16

#define DV_FONT_OPENTYPE_TTF_MACASCII 1
#define DV_FONT_OPENTYPE_TTF_WINUNICODE 3
#define DV_FONT_OPENTYPE_CFF_MACASCII 6
#define DV_FONT_OPENTYPE_CFF_WINUNICODE 5
#define DV_FONT_TYPE1_ADOBE 2
#define DV_FONT_TYPE1_ISOLATIN 7
#define DV_FONT_TYPE1_WINUNICODE 8
#define DV_FONT_PSTYPE3_ISOLATIN 11
#define DV_FONT_PSTYPE3_WINUNICODE 12
#define DV_FONT_BARECFF_ADOBE 14
#define DV_FONT_BARECFF_ISOLATIN 15
#define DV_FONT_BARECFF_WINUNICODE 16


/* Defines - Font Blending Options */
#define DV_MMCONTRIB_TYPE_NONE           0
#define DV_MMCONTRIB_TYPE_DIM_NORM1024   1
#define DV_MMCONTRIB_TYPE_DIM_NORM16384  2
#define DV_MMCONTRIB_TYPE_AXIS_NORM1024  3
#define DV_MMCONTRIB_TYPE_AXIS_NORM16384 4
#define DV_MMCONTRIB_TYPE_AXIS_USER1024  5
#define DV_MMCONTRIB_TYPE_AXIS_USER16384 6


/* Defines - Style Effects */
#define DV_EFFECT_NONE 0

#define DV_EFFECT_HBLUR_FUZZY 1
#define DV_EFFECT_VBLUR_FUZZY 2
#define DV_EFFECT_HVBLUR_FUZZY 3

#define DV_EFFECT_HBLUR_GAUSS 4
#define DV_EFFECT_VBLUR_GAUSS 5
#define DV_EFFECT_HVBLUR_GAUSS 6

#define DV_EFFECT_EMBOSS_50_NW 7
#define DV_EFFECT_EMBOSS_50_NE 8
#define DV_EFFECT_EMBOSS_50_SW 9
#define DV_EFFECT_EMBOSS_50_SE 10

#define DV_EFFECT_PATTERN_PHOTO 11
#define DV_EFFECT_PATTERN_SHADES 12
#define DV_EFFECT_PATTERN_STRIPES 13
#define DV_EFFECT_PATTERN_ORNAMENTAL 14

#define DV_EFFECT_EDGE_50_NW 15
#define DV_EFFECT_EDGE_50_NE 16
#define DV_EFFECT_EDGE_50_SW 17
#define DV_EFFECT_EDGE_50_SE 18

#define DV_EFFECT_BODY_50_NW 19
#define DV_EFFECT_BODY_50_NE 20
#define DV_EFFECT_BODY_50_SW 21
#define DV_EFFECT_BODY_50_SE 22

#define DV_EFFECT_EDGE_100_NW 23
#define DV_EFFECT_EDGE_100_NE 24
#define DV_EFFECT_EDGE_100_SW 25
#define DV_EFFECT_EDGE_100_SE 26

#define DV_EFFECT_BODY_100_NW 27
#define DV_EFFECT_BODY_100_NE 28
#define DV_EFFECT_BODY_100_SW 29
#define DV_EFFECT_BODY_100_SE 30

#define DV_EFFECT_SHADE_INNER_A 31
#define DV_EFFECT_SHADE_INNER_B 32
#define DV_EFFECT_SHADE_INNER_C 33

#define DV_EFFECT_HBOLD 34
#define DV_EFFECT_VBOLD 35
#define DV_EFFECT_HVBOLD 36

#define DV_EFFECT_HBOLD_VBLUR_FUZZY 37
#define DV_EFFECT_VBOLD_HBLUR_FUZZY 38

#define DV_EFFECT_PATTERN_INVPHOTO 39
#define DV_EFFECT_PATTERN_INVSHADES 40
#define DV_EFFECT_PATTERN_INVSTRIPES 41
#define DV_EFFECT_PATTERN_INVORNAMENTAL 42

#define DV_EFFECT_PATTERN_CUSTOM 43

#define DV_EFFECT_RESERVED_A 44
#define DV_EFFECT_RESERVED_B 45

#define DV_EFFECT_HBLUR_STACK 46
#define DV_EFFECT_VBLUR_STACK 47
#define DV_EFFECT_HVBLUR_STACK 48

#define DV_EFFECT_HBOLD_VBLUR_GAUSS 49
#define DV_EFFECT_VBOLD_HBLUR_GAUSS 50

#define DV_EFFECT_HBOLD_VBLUR_STACK 51
#define DV_EFFECT_VBOLD_HBLUR_STACK 52

#define DV_EFFECT_RESERVED_C 53
#define DV_EFFECT_RESERVED_D 54

#define DV_EFFECT_HOUTLINE 55
#define DV_EFFECT_VOUTLINE 56
#define DV_EFFECT_HVOUTLINE 57


/* Defines - Sizes */
#define DV_SIZE_1 1
#define DV_SIZE_2 2
#define DV_SIZE_3 3
#define DV_SIZE_4 4


/* Defines - Surface Formats */
#define DV_FORMAT_8 8
#define DV_FORMAT_16 16
#define DV_FORMAT_24 24
#define DV_FORMAT_32 32


/* Defines - Font Rendering Quality */
#define DV_QUALITY_MEDIUM 0
#define DV_QUALITY_HIGH 1
#define DV_QUALITY_VERYHIGH 2
#define DV_QUALITY_DISPERSION_FILTER_1 3
#define DV_QUALITY_DISPERSION_FILTER_2 4
#define DV_QUALITY_HINT_FILTER_1 5
#define DV_QUALITY_HINT_FILTER_2 6
#define DV_QUALITY_HINT_FILTER_3 7
#define DV_QUALITY_HINT_FILTER_4 8
#define DV_QUALITY_LCD_RGB_SUB_1 9
#define DV_QUALITY_LCD_RGB_INT_1 10
#define DV_QUALITY_LCD_BGR_SUB_1 11
#define DV_QUALITY_LCD_BGR_INT_1 12
#define DV_QUALITY_LCD_RGB_SUB_2 13
#define DV_QUALITY_LCD_RGB_INT_2 14
#define DV_QUALITY_LCD_BGR_SUB_2 15
#define DV_QUALITY_LCD_BGR_INT_2 16
#define DV_QUALITY_LCD_RGB_SUB_3 17
#define DV_QUALITY_LCD_RGB_INT_3 18
#define DV_QUALITY_LCD_BGR_SUB_3 19
#define DV_QUALITY_LCD_BGR_INT_3 20
#define DV_QUALITY_LCD2_RGB_SUB_1 21
#define DV_QUALITY_LCD2_RGB_INT_1 22
#define DV_QUALITY_LCD2_BGR_SUB_1 23
#define DV_QUALITY_LCD2_BGR_INT_1 24
#define DV_QUALITY_LCD2_RGB_SUB_2 25
#define DV_QUALITY_LCD2_RGB_INT_2 26
#define DV_QUALITY_LCD2_BGR_SUB_2 27
#define DV_QUALITY_LCD2_BGR_INT_2 28


/* Defines - Rendering Rules */
#define DV_NZW_OFF 0
#define DV_NZW_ON 1


/* Defines - Shape Definition Flags */
#define DV_SHAPE_SUPPRESS_XL 128
#define DV_SHAPE_BUFFER_MIXED 0
#define DV_SHAPE_BUFFER_ABSOLUTE 2
#define DV_SHAPE_BUFFER_RELATIVE 4
#define DV_SHAPE_ARRAY_MIXED 1
#define DV_SHAPE_ARRAY_ABSOLUTE 3
#define DV_SHAPE_ARRAY_RELATIVE 5
#define DV_SHAPE_BUFFER 0
#define DV_SHAPE_ARRAY 1


/* Defines - Bitmap Cache Compression Flags */
#define DV_COMPRESSION_OFF 0
#define DV_COMPRESSION_ON 1


/* Defines - Bitmap Creation Flags */
#define DV_BITMAP_OFF 0
#define DV_BITMAP_ON 1
#define DV_BITMAP_ON_ONLYIFOPAQUEANDINSIDE -1
#define DV_BITMAPPART_OFF 2
#define DV_BITMAPPART_ON 3
#define DV_BITMAPPART_ON_ONLYIFOPAQUEANDINSIDE -3


/* Defines - Miscellaneous Numeric Font Values */
#define DV_NVAL_PLATFORM_TYPE   2001

#define DV_NVAL_EMBED           2101
#define DV_NVAL_CPAGE_A         2102
#define DV_NVAL_CPAGE_B         2103
#define DV_NVAL_SELECTION_FLAGS 2104
#define DV_NVAL_WEIGHT_CLASS    2105
#define DV_NVAL_WIDTH_CLASS     2106
#define DV_NVAL_FAMILY_CLASS    2107

#define DV_NVAL_ASCENDER        2201
#define DV_NVAL_DESCENDER       2202
#define DV_NVAL_LINEGAP         2203

#define DV_NVAL_WIN_ASCENT      2301
#define DV_NVAL_WIN_DESCENT     2302

#define DV_NVAL_VER_ASCENDER    2401
#define DV_NVAL_VER_DESCENDER   2402
#define DV_NVAL_VER_LINEGAP     2403

#define DV_NVAL_MMAXIS_TAG_000  3000
#define DV_NVAL_MMAXIS_DEF_000  3500
#define DV_NVAL_MMAXIS_MIN_000  4000
#define DV_NVAL_MMAXIS_MAX_000  4500


/* Defines - Miscellaneous String Font Values */
#define DV_SVAL_ASC_FONTNAME            1001 /* Full Font Name - ASCII */
#define DV_SVAL_ASC_FAMILYNAME          1002 /* Font Family Name - ASCII */
#define DV_SVAL_ASC_SUBFAMILYNAME       1003 /* Font Subfamily Name - ASCII */
#define DV_SVAL_ASC_COPYRIGHT           1004 /* Copyright Notice - ASCII */
#define DV_SVAL_ASC_VERSION             1005 /* Version String - ASCII */
#define DV_SVAL_ASC_POSTSCRIPTNAME      1006 /* PostScript Font Name - ASCII */
#define DV_SVAL_ASC_UNIQUEID            1007 /* Unique Font ID String - ASCII */
#define DV_SVAL_ASC_TRADEMARK           1008 /* Font Trademark - ASCII */
#define DV_SVAL_ASC_MANUFACTURER        1009 /* Font Manufacturer - ASCII */
#define DV_SVAL_ASC_DESIGNER            1010 /* Font Designer - ASCII */
#define DV_SVAL_ASC_DESCRIPTION         1011 /* Font Description - ASCII */
#define DV_SVAL_ASC_SAMPLE              1012 /* Font Sample Text - ASCII */
#define DV_SVAL_ASC_TYPOFAMILYNAME      1013 /* Tyographic Family Name (Preferred Family) - ASCII */
#define DV_SVAL_ASC_TYPOSUBFAMILYNAME   1014 /* Tyographic Subfamily Name (Preferred Subfamily) - ASCII */
#define DV_SVAL_ASC_WWSFAMILYNAME       1015 /* WWS Family Name */
#define DV_SVAL_ASC_WWSSUBFAMILYNAME    1016 /* WWS Subfamily Name */

#define DV_SVAL_UNI_FONTNAME            1101 /* Full Font Name - UNICODE */
#define DV_SVAL_UNI_FAMILYNAME          1102 /* Font Family Name - UNICODE */
#define DV_SVAL_UNI_SUBFAMILYNAME       1103 /* Font Subfamily Name - UNICODE */
#define DV_SVAL_UNI_COPYRIGHT           1104 /* Copyright Notice - UNICODE */
#define DV_SVAL_UNI_VERSION             1105 /* Version String - UNICODE */
#define DV_SVAL_UNI_POSTSCRIPTNAME      1106 /* PostScript Font Name - UNICODE */
#define DV_SVAL_UNI_UNIQUEID            1107 /* Unique Font ID String - UNICODE */
#define DV_SVAL_UNI_TRADEMARK           1108 /* Font Trademark - UNICODE */
#define DV_SVAL_UNI_MANUFACTURER        1109 /* Font Manufacturer - ASCII */
#define DV_SVAL_UNI_DESIGNER            1110 /* Font Designer - ASCII */
#define DV_SVAL_UNI_DESCRIPTION         1111 /* Font Description - ASCII */
#define DV_SVAL_UNI_SAMPLE              1112 /* Font Sample Text - ASCII */
#define DV_SVAL_UNI_TYPOFAMILYNAME      1113 /* Tyographic Family Name (Preferred Family) - ASCII */
#define DV_SVAL_UNI_TYPOSUBFAMILYNAME   1114 /* Tyographic Subfamily Name (Preferred Subfamily) - ASCII */
#define DV_SVAL_UNI_WWSFAMILYNAME       1115 /* WWS Family Name */
#define DV_SVAL_UNI_WWSSUBFAMILYNAME    1116 /* WWS Subfamily Name */

#define DV_SVAL_ASC_MMDESIGNNAMES       1201 /* Multiple-Master Design Names - ASCII */
#define DV_SVAL_ASC_FONTMATRIX          1202 /* Type 1 Font Matrix - ASCII */
#define DV_SVAL_ASC_MMAXISTYPES         1203 /* Type 1 MM Axis Types - ASCII */
#define DV_SVAL_ASC_MMDESIGNPOSITIONS   1204 /* Type 1 MM Design Positions - ASCII */
#define DV_SVAL_ASC_MMDESIGNMAP         1205 /* Type 1 MM Design Map - ASCII */
#define DV_SVAL_ASC_FONTMATRIXGROUPS    1206 /* CID-Keyed Font Matrix Groups - ASCII */

#define DV_SVAL_ASC_MMAXIS_NAME_000     2000
#define DV_SVAL_UNI_MMAXIS_NAME_000     2500
#define DV_SVAL_ASC_MMINSTANCE_NAME_000 3000
#define DV_SVAL_UNI_MMINSTANCE_NAME_000 3500


/* Defines - Hinting */
#define DV_HINTING_ENGINE_DEFAULT_A 0
#define DV_HINTING_ENGINE_DEFAULT_B 1
#define DV_HINTING_USER_DEFAULT_A 2
#define DV_HINTING_USER_DEFAULT_B 3
#define DV_HINTING_XON_YON 4
#define DV_HINTING_XON_YOFF 5
#define DV_HINTING_XOFF_YON 6
#define DV_HINTING_XOFF_YOFF 7
#define DV_HINTING_AUTO_A 8
#define DV_HINTING_AUTO_B 9


/* Defines - Positioning */
#define DV_POSITIONING_ENGINE_DEFAULT_A 0
#define DV_POSITIONING_ENGINE_DEFAULT_B 1
#define DV_POSITIONING_USER_DEFAULT_A 2
#define DV_POSITIONING_USER_DEFAULT_B 3
#define DV_POSITIONING_FRACX_FRACY 4
#define DV_POSITIONING_FRACX_INTY 5
#define DV_POSITIONING_INTX_FRACY 6
#define DV_POSITIONING_INTX_INTY 7
#define DV_POSITIONING_AUTO_A 8
#define DV_POSITIONING_AUTO_B 9


/* Defines - Advanced Typography: Scripts */
#define DV_AT_SCRIPT_NONE 0
#define DV_AT_SCRIPT_LATN 1
#define DV_AT_SCRIPT_CYRL 2
#define DV_AT_SCRIPT_GREK 3
#define DV_AT_SCRIPT_HEBR 4
#define DV_AT_SCRIPT_ARAB 5
#define DV_AT_SCRIPT_HANI 6
#define DV_AT_SCRIPT_KANA 7


/* Defines - Advanced Typography: Glyph Substitution Features */
#define DV_AT_GSUBST_NONE 0
#define DV_AT_GSUBST_INIT 1
#define DV_AT_GSUBST_MEDI 2
#define DV_AT_GSUBST_FINA 3
#define DV_AT_GSUBST_LIGA 4
#define DV_AT_GSUBST_RLIG 5
#define DV_AT_GSUBST_DLIG 6
#define DV_AT_GSUBST_HLIG 7
#define DV_AT_GSUBST_VERT 8
#define DV_AT_GSUBST_VRT2 9


/* D-Type Standard Engine Functions */

/* Create and initialize D-Type Standard Engine - via stream */
DT_SWORD DTYPE_APICALL dtEngineIniViaStream(DT_DTENGINE* engine, const DT_STREAM_DESC* sd, DT_INIT_INFO* init_info);
/* Deinitialize and destroy D-Type Standard Engine */
void DTYPE_APICALL dtEngineExt(DT_DTENGINE engine);

/* Get D-Type Standard Engine version */
DT_SLONG DTYPE_APICALL dtEngineGetVersion(DT_DTENGINE engine, DT_CHAR* ver_name, DT_CHAR* ver_snum, DT_CHAR* reserved);
/* Get user string */
const DT_CHAR* DTYPE_APICALL dtEngineGetUserString(DT_DTENGINE engine, DT_SLONG string_id);
/* Get user params */
DT_SLONG DTYPE_APICALL dtEngineGetUserParam(DT_DTENGINE engine, DT_SLONG index, DT_CHAR* buffer);
/* Get user stream */
DT_DTSTREAM DTYPE_APICALL dtEngineGetUserStream(DT_DTENGINE engine, DT_SLONG index);
/* Get user stream info */
DT_SWORD DTYPE_APICALL dtEngineGetUserStreamInfo(DT_DTENGINE engine, DT_SLONG index, const DT_CHAR** label, DT_SWORD* category, DT_UBYTE* type);

/* Set Output to memory surface (MDC) */
DT_SWORD DTYPE_APICALL dtOutputSetMDC(DT_DTENGINE engine, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface, DT_SRAST_L clip_x, DT_SRAST_L clip_y, DT_SRAST_L clip_w, DT_SRAST_L clip_h);
/* Set Output to memory surface (MDC) - deprecated form */
DT_SWORD DTYPE_APICALL dtOutputSetAsMDC(DT_DTENGINE engine, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface, DT_SWORD clip_x, DT_SWORD clip_y, DT_SWORD clip_w, DT_SWORD clip_h);
/* Set Output to Windows Device Context (HDC) - Windows only */
DT_SWORD DTYPE_APICALL dtOutputSetAsHDC(DT_DTENGINE engine, DT_HDC hdc, DT_SWORD clip_x, DT_SWORD clip_y, DT_SWORD clip_w, DT_SWORD clip_h);
/* Set Output style (color, transparency and special effect such as shadow, pattern fill, emboss, emboldening etc.) */
DT_SWORD DTYPE_APICALL dtOutputSetStyleAttribs(DT_DTENGINE engine, const DT_STYLE_ATTRIBS* style, DT_UBYTE reserved);
/* Set Output style (advanced version) */
DT_SWORD DTYPE_APICALL dtOutputSetStyleEffects(DT_DTENGINE engine, const DT_STYLE_EFFECTS* style, DT_UBYTE reserved);
/* Invert output */
DT_SWORD DTYPE_APICALL dtOutputSetInv(DT_DTENGINE engine, DT_UBYTE flag);

/* Get Output style (color, transparency and special effect such as shadow, pattern fill, emboss, emboldening etc.) */
DT_SWORD DTYPE_APICALL dtOutputGetStyleAttribs(DT_DTENGINE engine, DT_STYLE_ATTRIBS* style, DT_UBYTE reserved);
/* Get Output style (advanced version) */
DT_SWORD DTYPE_APICALL dtOutputGetStyleEffects(DT_DTENGINE engine, DT_STYLE_EFFECTS* style, DT_UBYTE reserved);

/* Get Rasterizer info */
DT_SLONG DTYPE_APICALL dtRasterizerGetInfo(DT_DTENGINE engine, DT_SWORD reserved);

/* Set Rasterizer's fill rule */
DT_SWORD DTYPE_APICALL dtRasterizerSetFillRule(DT_DTENGINE engine, DT_ID_UBYTE fill_rule);
/* Set Rasterizer's ASync System */
DT_SWORD DTYPE_APICALL dtRasterizerSetASys(DT_DTENGINE engine, DT_DASYS das);
/* Set Rasterizer's alpha levels (grayscale) */
DT_SWORD DTYPE_APICALL dtRasterizerSetGrayscale(DT_DTENGINE engine, const DT_UBYTE gsv[]);

/* Get Rasterizer's fill rule */
DT_SWORD DTYPE_APICALL dtRasterizerGetFillRule(DT_DTENGINE engine, DT_ID_UBYTE* fill_rule);
/* Get Rasterizer's ASync System */
DT_SWORD DTYPE_APICALL dtRasterizerGetASys(DT_DTENGINE engine, DT_DASYS* das);
/* Get Rasterizer's alpha levels (grayscale) */
DT_SWORD DTYPE_APICALL dtRasterizerGetGrayscale(DT_DTENGINE engine, DT_UBYTE gsv[]);

/* Reallocate and/or switch Rasterizer */
DT_SWORD DTYPE_APICALL dtRasterizerRealloc(DT_DTENGINE engine, DT_SWORD raster_width, DT_SWORD raster_height, DT_SWORD raster_intersect, DT_ID_SWORD raster_id);

/* Set Typesetter's hinting mode */
DT_SWORD DTYPE_APICALL dtTypesetterSetHinting(DT_DTENGINE engine, DT_ID_UBYTE hinting, DT_UBYTE flag);
/* Set Typesetter's positioning mode */
DT_SWORD DTYPE_APICALL dtTypesetterSetPositioning(DT_DTENGINE engine, DT_ID_UBYTE positioning, DT_UBYTE flag);
/* Set Typesetter's subpixel levels */
DT_SWORD DTYPE_APICALL dtTypesetterSetSubpixels(DT_DTENGINE engine, DT_UBYTE subpixels, DT_UBYTE flag);
/* Set Typesetter's bitmap cache compression */
DT_SWORD DTYPE_APICALL dtTypesetterSetCacheCompression(DT_DTENGINE engine, DT_ID_UBYTE bitmap_compress, DT_UBYTE reserved);
/* Set Typesetter's bitmap cache policy */
DT_SWORD DTYPE_APICALL dtTypesetterSetCachePolicy(DT_DTENGINE engine, DT_SLONG max_bitmap_bytes, DT_SWORD reserved1, DT_SWORD reserved2, DT_SWORD reserved3, DT_SWORD reserved4, DT_UBYTE flag);
/* Set Typesetter's quality */
DT_SWORD DTYPE_APICALL dtTypesetterSetQuality(DT_DTENGINE engine, DT_ID_UBYTE quality, DT_UBYTE flag);
/* Set minimum pixel size when Typesetter scales narrow horizontal and vertical glyph features in hinted fonts to small sizes */
DT_SWORD DTYPE_APICALL dtTypesetterSetHintThreshold(DT_DTENGINE engine, DT_SLONG min_hint_dpx, DT_SLONG min_hint_dpy, DT_ID_UBYTE scale_id, DT_UBYTE flag);
/* Set minimum pixel size when Typesetter scales narrow horizontal and vertical glyph features in hinted fonts to small sizes */
DT_SWORD DTYPE_APICALL dtTypesetterSetHintMin(DT_DTENGINE engine, DT_SWORD min_hint_dpx, DT_SWORD min_hint_dpy, DT_UBYTE flag);
/* Set rounding flag for scaled hinted features */
DT_SWORD DTYPE_APICALL dtTypesetterSetHintRounding(DT_DTENGINE engine, DT_SWORD rounding_x, DT_SWORD rounding_y, DT_UBYTE reserved);
/* Set Typesetter's size subscale */
DT_SWORD DTYPE_APICALL dtTypesetterSetSizeSubscale(DT_DTENGINE engine, DT_UBYTE transform_params_mul_h, DT_UBYTE transform_params_mul_v, DT_UBYTE reserved);
/* Set Typesetter's angle subscale */
DT_SWORD DTYPE_APICALL dtTypesetterSetAngleSubscale(DT_DTENGINE engine, DT_UBYTE transform_params_mul_a, DT_UBYTE reserved);
/* Set Typesetter's type (advanced version) */
DT_SWORD DTYPE_APICALL dtTypesetterSetTypeEffects(DT_DTENGINE engine, const DT_TYPE_EFFECTS_L* type, DT_UBYTE reserved);
/* Set Typesetter's type (advanced version) - XL */
DT_SWORD DTYPE_APICALL dtTypesetterSetTypeEffectsXL(DT_DTENGINE engine, const DT_TYPE_EFFECTS_XL* type, DT_UBYTE reserved);
/* Set Typesetter's type */
DT_SWORD DTYPE_APICALL dtTypesetterSetTypeAttribs(DT_DTENGINE engine, const DT_TYPE_ATTRIBS* type, DT_UBYTE reserved);
/* Set Typesetter's type using an additional transformation matrix (advanced version) */
DT_SWORD DTYPE_APICALL dtTypesetterSetTypeEffectsTransformed(DT_DTENGINE engine, DT_TYPE_EFFECTS_L* type, const DT_TM2X2 transform, DT_ID_UBYTE flag, DT_UBYTE reserved);
/* Set Typesetter's type using an additional transformation matrix (advanced version) - XL */
DT_SWORD DTYPE_APICALL dtTypesetterSetTypeEffectsTransformedXL(DT_DTENGINE engine, DT_TYPE_EFFECTS_XL* type, const DT_TM2X2 transform, DT_ID_UBYTE flag, DT_UBYTE reserved);
/* Set Typesetter's type using an additional transformation matrix */
/*DT_SWORD DTYPE_APICALL dtTypesetterSetTypeAttribsTransformed(DT_DTENGINE engine, DT_TYPE_ATTRIBS* type, const DT_TM2X2 transform, DT_ID_UBYTE flag, DT_UBYTE reserved);*/
/* Set Typesetter's align method */
DT_SWORD DTYPE_APICALL dtTypesetterSetAlignMethod(DT_DTENGINE engine, DT_ID_UBYTE align_method_x, DT_ID_UBYTE align_method_y, DT_UBYTE reserved);

/* Get Typesetter's hinting mode */
DT_SWORD DTYPE_APICALL dtTypesetterGetHinting(DT_DTENGINE engine, DT_ID_UBYTE* hinting);
/* Get Typesetter's positioning mode */
DT_SWORD DTYPE_APICALL dtTypesetterGetPositioning(DT_DTENGINE engine, DT_ID_UBYTE* positioning);
/* Get Typesetter's subpixel levels */
DT_SWORD DTYPE_APICALL dtTypesetterGetSubpixels(DT_DTENGINE engine, DT_UBYTE* subpixels);
/* Get Typesetter's bitmap cache compression */
DT_SWORD DTYPE_APICALL dtTypesetterGetCacheCompression(DT_DTENGINE engine, DT_ID_UBYTE* bitmap_compress);
/* Get Typesetter's bitmap cache policy */
DT_SWORD DTYPE_APICALL dtTypesetterGetCachePolicy(DT_DTENGINE engine, DT_SLONG* max_bitmap_bytes, DT_SWORD* reserved1, DT_SWORD* reserved2, DT_SWORD* reserved3, DT_SWORD* reserved4);
/* Get Typesetter's quality */
DT_SWORD DTYPE_APICALL dtTypesetterGetQuality(DT_DTENGINE engine, DT_ID_UBYTE* quality);
/* Get minimum pixel size when Typesetter scales narrow horizontal and vertical glyph features in hinted fonts to small sizes */
DT_SWORD DTYPE_APICALL dtTypesetterGetHintThreshold(DT_DTENGINE engine, DT_SLONG* min_hint_dpx, DT_SLONG* min_hint_dpy, DT_ID_UBYTE scale_id);
/* Get minimum pixel size when Typesetter scales narrow horizontal and vertical glyph features in hinted fonts to small sizes */
DT_SWORD DTYPE_APICALL dtTypesetterGetHintMin(DT_DTENGINE engine, DT_SWORD* min_hint_dpx, DT_SWORD* min_hint_dpy);
/* Get rounding flag for scaled hinted features */
DT_SWORD DTYPE_APICALL dtTypesetterGetHintRounding(DT_DTENGINE engine, DT_SWORD* rounding_x, DT_SWORD* rounding_y);
/* Get Typesetter's size subscale */
DT_SWORD DTYPE_APICALL dtTypesetterGetSizeSubscale(DT_DTENGINE engine, DT_UBYTE* transform_params_mul_h, DT_UBYTE* transform_params_mul_v);
/* Get Typesetter's angle subscale */
DT_SWORD DTYPE_APICALL dtTypesetterGetAngleSubscale(DT_DTENGINE engine, DT_UBYTE* transform_params_mul_a);
/* Get Typesetter's current type (advanced version) */
DT_SWORD DTYPE_APICALL dtTypesetterGetTypeEffects(DT_DTENGINE engine, DT_TYPE_EFFECTS_L* type, DT_UBYTE reserved);
/* Get Typesetter's current type (advanced version) - XL */
DT_SWORD DTYPE_APICALL dtTypesetterGetTypeEffectsXL(DT_DTENGINE engine, DT_TYPE_EFFECTS_XL* type, DT_UBYTE reserved);
/* Get Typesetter's current type */
DT_SWORD DTYPE_APICALL dtTypesetterGetTypeAttribs(DT_DTENGINE engine, DT_TYPE_ATTRIBS* type, DT_UBYTE reserved);
/* Get Typesetter's align method */
DT_SWORD DTYPE_APICALL dtTypesetterGetAlignMethod(DT_DTENGINE engine, DT_ID_UBYTE* align_method_x, DT_ID_UBYTE* align_method_y);

/* Transform type using a transformation matrix (advanced version) */
DT_SWORD DTYPE_APICALL dtTypesetterTransformTypeEffects(DT_DTENGINE engine, DT_TYPE_EFFECTS_L* type, const DT_TM2X2 transform, DT_ID_UBYTE optimize, DT_UBYTE reserved);
/* Transform type using a transformation matrix (advanced version) - XL */
DT_SWORD DTYPE_APICALL dtTypesetterTransformTypeEffectsXL(DT_DTENGINE engine, DT_TYPE_EFFECTS_XL* type, const DT_TM2X2 transform, DT_ID_UBYTE optimize, DT_UBYTE reserved);
/* Transform type using a transformation matrix */
/*DT_SWORD DTYPE_APICALL dtTypesetterTransformTypeAttribs(DT_DTENGINE engine, DT_TYPE_ATTRIBS* type, const DT_TM2X2 transform, DT_ID_UBYTE optimize, DT_UBYTE reserved);*/

/* Reallocate Typesetter */
DT_SWORD DTYPE_APICALL dtTypesetterRealloc(DT_DTENGINE engine, DT_SLONG cache_bitmap_bytes, DT_SLONG cache_bitmap_items, DT_SLONG cache_transform_items, DT_SWORD reserved);

/* Get number of pattern groups in Pattern Catalog */
DT_SWORD DTYPE_APICALL dtPatternGetNrOfGroups(DT_DTENGINE engine);
/* Get number of pattern fills in a pattern group */
DT_SWORD DTYPE_APICALL dtPatternGetGroupCount(DT_DTENGINE engine, DT_ID_SWORD group_index);
/* Get pattern fill's bitmap data */
DT_UBYTE* DTYPE_APICALL dtPatternGetBitmap(DT_DTENGINE engine, DT_ID_SWORD group_index, DT_ID_SWORD pattern_index);
/* Get pattern fill's bitmap data */
DT_UBYTE* DTYPE_APICALL dtPatternGetBitmapPlus(DT_DTENGINE engine, DT_ID_SWORD group_index, DT_ID_SWORD pattern_index, DT_ID_SWORD* pattern_format, DT_ID_SWORD* pattern_subformat, DT_SLONG* pattern_w, DT_SLONG* pattern_h);
/* Set pattern fill's bitmap data */
DT_SWORD DTYPE_APICALL dtPatternSetBitmap(DT_DTENGINE engine, DT_ID_SWORD group_index, DT_ID_SWORD pattern_index, DT_UBYTE* pattern_addr);
/* Set pattern fill's bitmap data */
DT_SWORD DTYPE_APICALL dtPatternSetBitmapPlus(DT_DTENGINE engine, DT_ID_SWORD group_index, DT_ID_SWORD pattern_index, DT_ID_SWORD pattern_format, DT_ID_SWORD pattern_subformat, DT_UBYTE* pattern_addr, DT_SLONG pattern_w, DT_SLONG pattern_h);

/* Get count of font slots in Font Catalog */
DT_SWORD DTYPE_APICALL dtFontGetCounter(DT_DTENGINE engine);
/* Test font slot and return its font index */
DT_ID_SWORD DTYPE_APICALL dtFontTest(DT_DTENGINE engine, DT_ID_SWORD slot_index);
/* Find font index using unique font identification string */
DT_ID_SWORD DTYPE_APICALL dtFontFind(DT_DTENGINE engine, const DT_CHAR* fuid);
/* Find font index of a Multiple-Master font instance */
DT_ID_SWORD DTYPE_APICALL dtFontFindMMInstance(DT_DTENGINE engine, DT_ID_SWORD font_index, const DT_SWORD contribs[]);
/* Find font index of a Multiple-Master font instance */
DT_ID_SWORD DTYPE_APICALL dtFontFindMMInstancePlus(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SWORD contribs_flag, DT_SWORD contribs_len, const DT_SLONG contribs[]);
/* Detect font format automatically via a single stream */
DT_ID_SWORD DTYPE_APICALL dtFontDetectFormatViaStream(DT_DTENGINE engine, DT_ID_SWORD font_format_ids[], const DT_STREAM_DESC* sd, DT_ID_UBYTE level, DT_SWORD* last_fcnr, DT_SWORD* axis_count, DT_SWORD* dim, DT_SLONG* i_next);
/* Detect font format automatically via a set of streams */
DT_ID_SWORD DTYPE_APICALL dtFontDetectFormatViaStreams(DT_DTENGINE engine, DT_ID_SWORD font_format_ids[], DT_UBYTE arr_size, const DT_STREAM_DESC* sd[], DT_ID_UBYTE level, DT_SWORD* last_fcnr, DT_SWORD* axis_count, DT_SWORD* dim, DT_SLONG* i_next);

/* Add new font to Font Catalog via a single stream and return its font index */
DT_ID_SWORD DTYPE_APICALL dtFontAddViaStream(DT_DTENGINE engine, DT_ID_SWORD font_format_id, const DT_CHAR* fuid, DT_SWORD fcnr, DT_ID_SWORD cmap_id, DT_ID_UBYTE caching, DT_ID_UBYTE hinting, const DT_STREAM_DESC* sd);
/* Add new font to Font Catalog via a set of streams and return its font index */
DT_ID_SWORD DTYPE_APICALL dtFontAddViaStreams(DT_DTENGINE engine, DT_ID_SWORD font_format_id, const DT_CHAR* fuid, DT_SWORD fcnr, DT_ID_SWORD cmap_id, DT_ID_UBYTE caching, DT_ID_UBYTE hinting, DT_UBYTE arr_size, const DT_STREAM_DESC* sd[]);
/* Add new font as a Multiple-Master font instance and return its font index */
DT_ID_SWORD DTYPE_APICALL dtFontAddAsMMInstance(DT_DTENGINE engine, DT_ID_SWORD font_index, const DT_SWORD contribs[]);
/* Add new font as a Multiple-Master font instance and return its font index */
DT_ID_SWORD DTYPE_APICALL dtFontAddAsMMInstancePlus(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SWORD contribs_flag, DT_SWORD contribs_len, const DT_SLONG contribs[]);
/* Remove font from Font Catalog */
DT_SWORD DTYPE_APICALL dtFontRemove(DT_DTENGINE engine, DT_ID_SWORD font_index);
/* Remove all fonts in Font Catalog */
DT_SWORD DTYPE_APICALL dtFontRemoveAll(DT_DTENGINE engine);

/* Activate font */
DT_SWORD DTYPE_APICALL dtFontMakeActive(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_UBYTE level, DT_SWORD* last_fcnr);
/* Deactivate font */
DT_SWORD DTYPE_APICALL dtFontMakeInactive(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_UBYTE respect_multireference);
/* Deactivate all fonts in Font Catalog */
DT_SWORD DTYPE_APICALL dtFontMakeInactiveAll(DT_DTENGINE engine);

/* Save font to stream with optional glyph subsetting, in the D-Type format */
DT_SLONG DTYPE_APICALL dtFontSaveToStream(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_SWORD reserved, DT_STREAM_DESC* sd, DT_SWORD (DTYPE_USRCALL *font_subset_func)(DT_ID_ULONG glyph_index, void* user_param), void* user_param);

/* Get font's system status */
DT_SWORD DTYPE_APICALL dtFontGetSysStatus(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SWORD* font_format_id, DT_CHAR* fuid, DT_SWORD* fcnr, DT_ID_SWORD* cmap_id, DT_ID_UBYTE* caching, DT_ID_UBYTE* hinting, DT_ID_UBYTE* active);
/* Get number of base designs in font */
DT_UBYTE DTYPE_APICALL dtFontGetDim(DT_DTENGINE engine, DT_ID_SWORD font_index);
/* Get number of axes, base designs and instances in font */
DT_SWORD DTYPE_APICALL dtFontGetMMAxisInfo(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_SWORD* dim, DT_SLONG* inst_count);
/* Get default axis values for all named font instances in font */
DT_SWORD DTYPE_APICALL dtFontGetMMInstData(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_SLONG inst_data[], DT_SWORD reserved);
/* Get a string value from font */
DT_SLONG DTYPE_APICALL dtFontGetStringValue(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SWORD string_id, DT_UBYTE* string_buffer, DT_SLONG max_string_len);
/* Get a string value from font in multiple languages */
DT_SLONG DTYPE_APICALL dtFontGetStringValuePlus(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SWORD string_id, DT_UBYTE* buffer, DT_SLONG max_buffer_len, DT_SLONG* max_arr_len, DT_ULONG langid_arr[], DT_SLONG langtag_offset_arr[], DT_SLONG langtag_len_arr[], DT_SLONG string_offset_array[], DT_SLONG string_len_array[]);
/* Get string values from font in multiple languages */
DT_SLONG DTYPE_APICALL dtFontGetStringValues(DT_DTENGINE engine, DT_UWORD flags, DT_ID_SWORD font_index, DT_ID_SWORD string_id, DT_UBYTE* buffer, DT_SLONG max_buffer_len, DT_SLONG* max_arr_len, DT_ULONG lang_and_platform_id_arr[], DT_SLONG langtag_offset_arr[], DT_SLONG langtag_len_arr[], DT_SLONG string_offset_array[], DT_SLONG string_len_array[]);
/* Get a numeric value from font */
DT_SLONG DTYPE_APICALL dtFontGetNumericValue(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SWORD numeric_id);
/* Get glyph substitutions as defined in font */
DT_SWORD DTYPE_APICALL dtFontGetGlyphSubsts(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_SWORD script_id, DT_ID_SWORD feature_id, DT_SWORD reserved, DT_AT_GLYPH_SUBSTS_SET* substs_set);
/* Get number of glyphs in font */
DT_ULONG DTYPE_APICALL dtFontGetNrOfGlyphs(DT_DTENGINE engine, DT_ID_SWORD font_index);
/* Get global font metrics */
DT_SWORD DTYPE_APICALL dtFontGetMetrics(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_UWORD* font_xbs, DT_UWORD* font_ybs, DT_RECT_SWORD* extent);
/* Get font dependent glyph index based on given character code */
DT_ID_ULONG DTYPE_APICALL dtFontGetGlyphIndex(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG char_code);
/* Get pointer to the font's main stream object (reserved for internal use) */
DT_DTSTREAM DTYPE_APICALL dtFontGetStream(DT_DTENGINE engine, DT_ID_SWORD font_index);

/* Instruct font drivers to send human-readable descriptions of errors encountered in bad fonts to your own function(s). Call this function before adding a new font to Font Catalog. */
DT_SWORD DTYPE_APICALL dtFontSetErrorCallback(DT_DTENGINE engine, void (DTYPE_USRCALL *user_error_func)(const DT_CHAR* error_message, void* user_param), void* user_param);

/* Add new fonts to Font Catalog using a font list */
DT_SWORD DTYPE_APICALL dtFontsAddViaList(DT_DTENGINE engine, DT_SWORD flags, const DT_STREAM_DESC* sd);

/* Get bounding box of a scaled extent */
void DTYPE_APICALL dtExtentDoTransform(DT_DTENGINE engine, DT_UBYTE crop_err, DT_RECT_L* rect_and_extent);
/* Get bounding box of a scaled extent - XL */
void DTYPE_APICALL dtExtentDoTransformXL(DT_DTENGINE engine, DT_UBYTE crop_err, const DT_RECT_SLONG* rect, DT_RECT_XL* extent);

/* Render glyph to Output - floating point version */
DT_SWORD DTYPE_APICALL dtGlyphDoOutput(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_FLOAT x, DT_FLOAT y, DT_SWORD reserved, DT_BMP* memory_bitmap);
/* Render glyph to Output - fractional version */
DT_SWORD DTYPE_APICALL dtGlyphDoOutputFrac(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_SFRAC_L x, DT_SFRAC_L y, DT_SWORD reserved, DT_BMP* memory_bitmap);
/* Render glyph to Output - fractional version - XL */
DT_SWORD DTYPE_APICALL dtGlyphDoOutputFracXL(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_SWORD reserved, DT_BMP* memory_bitmap);
/* Scale glyph - floating point version */
DT_SLONG DTYPE_APICALL dtGlyphDoTransform(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD flag, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_FLOAT x_arr[], DT_FLOAT y_arr[], DT_BMP* memory_bitmap);
/* Scale glyph - fractional version */
DT_SLONG DTYPE_APICALL dtGlyphDoTransformFrac(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_SFRAC_L x, DT_SFRAC_L y, DT_ID_SWORD flag, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_SFRAC_L x_arr[], DT_SFRAC_L y_arr[], DT_BMP* memory_bitmap);
/* Scale glyph - floating point version - XL */
DT_SLONG DTYPE_APICALL dtGlyphDoTransformFracXL(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_ID_SWORD flag, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_SFRAC_XL x_arr[], DT_SFRAC_XL y_arr[], DT_BMP* memory_bitmap);
/* Scale glyph extent - floating point version */
DT_SWORD DTYPE_APICALL dtGlyphDoMinMax(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD reserved, DT_FLOAT* xmn, DT_FLOAT* ymn, DT_FLOAT* xmx, DT_FLOAT* ymx);
/* Scale glyph extent - fractional version */
DT_SWORD DTYPE_APICALL dtGlyphDoMinMaxFrac(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_SFRAC_L x, DT_SFRAC_L y, DT_ID_SWORD reserved, DT_SFRAC_L* xmn, DT_SFRAC_L* ymn, DT_SFRAC_L* xmx, DT_SFRAC_L* ymx);
/* Scale glyph extent - fractional version - XL */
DT_SWORD DTYPE_APICALL dtGlyphDoMinMaxFracXL(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_ID_SWORD reserved, DT_SFRAC_XL* xmn, DT_SFRAC_XL* ymn, DT_SFRAC_XL* xmx, DT_SFRAC_XL* ymx);
/* Scale glyph position - floating point version */
DT_SWORD DTYPE_APICALL dtGlyphDoPosition(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD flag, DT_FLOAT* x_out, DT_FLOAT* y_out);
/* Scale glyph position - fractional version */
DT_SWORD DTYPE_APICALL dtGlyphDoPositionFrac(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_SFRAC_L x, DT_SFRAC_L y, DT_ID_SWORD flag, DT_SFRAC_L* x_out, DT_SFRAC_L* y_out);
/* Scale glyph position - fractional version - XL */
DT_SWORD DTYPE_APICALL dtGlyphDoPositionFracXL(DT_DTENGINE engine, DT_ID_ULONG glyph_index, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_ID_SWORD flag, DT_SFRAC_XL* x_out, DT_SFRAC_XL* y_out);

/* Get glyph metrics */
DT_SWORD DTYPE_APICALL dtGlyphGetMetrics(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG glyph_index, DT_UWORD* advance_width, DT_RECT_SWORD* extent);
/* Get glyph metrics - horizontal and vertical */
DT_SWORD DTYPE_APICALL dtGlyphGetMetricsPlus(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG glyph_index, DT_ADVANCE* advance, DT_RECT_SWORD* extent, DT_SWORD reserved);
/* Get glyph outline */
DT_SLONG DTYPE_APICALL dtGlyphGetOutline(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG glyph_index, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_SLONG x_arr[], DT_SLONG y_arr[], DT_SLONG reserved);
/* Get kerning for two adjacent glyphs */
DT_SWORD DTYPE_APICALL dtGlyphGetKern(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG glyph_index_1, DT_ID_ULONG glyph_index_2, DT_SWORD* kerning_x, DT_SWORD* kerning_y);
/* Get extra data that might be associated with a glyph */
DT_UBYTE* DTYPE_APICALL dtGlyphGetExtras(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG glyph_index, DT_UWORD flags, const DT_SLONG params[], DT_SLONG* len, DT_UWORD* xtra_bits);

/* Render character to Output - floating point version */
DT_SWORD DTYPE_APICALL dtCharDoOutput(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_FLOAT x, DT_FLOAT y, DT_SWORD reserved, DT_BMP* memory_bitmap);
/* Render character to Output - fractional version */
DT_SWORD DTYPE_APICALL dtCharDoOutputFrac(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_SFRAC_L x, DT_SFRAC_L y, DT_SWORD reserved, DT_BMP* memory_bitmap);
/* Render character to Output - fractional version - XL */
DT_SWORD DTYPE_APICALL dtCharDoOutputFracXL(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_SWORD reserved, DT_BMP* memory_bitmap);
/* Scale character - floating point version */
DT_SLONG DTYPE_APICALL dtCharDoTransform(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD flag, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_FLOAT x_arr[], DT_FLOAT y_arr[], DT_BMP* memory_bitmap);
/* Scale character - fractional version */
DT_SLONG DTYPE_APICALL dtCharDoTransformFrac(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_SFRAC_L x, DT_SFRAC_L y, DT_ID_SWORD flag, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_SFRAC_L x_arr[], DT_SFRAC_L y_arr[], DT_BMP* memory_bitmap);
/* Scale character - fractional version - XL */
DT_SLONG DTYPE_APICALL dtCharDoTransformFracXL(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_ID_SWORD flag, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_SFRAC_XL x_arr[], DT_SFRAC_XL y_arr[], DT_BMP* memory_bitmap);
/* Scale character extent - floating point version */
DT_SWORD DTYPE_APICALL dtCharDoMinMax(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD reserved, DT_FLOAT* xmn, DT_FLOAT* ymn, DT_FLOAT* xmx, DT_FLOAT* ymx);
/* Scale character extent - fractional version */
DT_SWORD DTYPE_APICALL dtCharDoMinMaxFrac(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_SFRAC_L x, DT_SFRAC_L y, DT_ID_SWORD reserved, DT_SFRAC_L* xmn, DT_SFRAC_L* ymn, DT_SFRAC_L* xmx, DT_SFRAC_L* ymx);
/* Scale character extent - fractional version - XL */
DT_SWORD DTYPE_APICALL dtCharDoMinMaxFracXL(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_ID_SWORD reserved, DT_SFRAC_XL* xmn, DT_SFRAC_XL* ymn, DT_SFRAC_XL* xmx, DT_SFRAC_XL* ymx);
/* Scale character position - floating point version */
DT_SWORD DTYPE_APICALL dtCharDoPosition(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD flag, DT_FLOAT* x_out, DT_FLOAT* y_out);
/* Scale character position - fractional version */
DT_SWORD DTYPE_APICALL dtCharDoPositionFrac(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_SFRAC_L x, DT_SFRAC_L y, DT_ID_SWORD flag, DT_SFRAC_L* x_out, DT_SFRAC_L* y_out);
/* Scale character position - fractional version - XL */
DT_SWORD DTYPE_APICALL dtCharDoPositionFracXL(DT_DTENGINE engine, DT_ID_ULONG char_code, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_ID_SWORD flag, DT_SFRAC_XL* x_out, DT_SFRAC_XL* y_out);

/* Get character metrics */
DT_SWORD DTYPE_APICALL dtCharGetMetrics(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG char_code, DT_UWORD* advance_width, DT_RECT_SWORD* extent);
/* Get character metrics - horizontal and vertical */
DT_SWORD DTYPE_APICALL dtCharGetMetricsPlus(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG char_code, DT_ADVANCE* advance, DT_RECT_SWORD* extent, DT_SWORD reserved);
/* Get character outline */
DT_SLONG DTYPE_APICALL dtCharGetOutline(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG char_code, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_SLONG x_arr[], DT_SLONG y_arr[], DT_SLONG reserved);
/* Get kerning for two adjacent characters */
DT_SWORD DTYPE_APICALL dtCharGetKern(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG char_code_1, DT_ID_ULONG char_code_2, DT_SWORD* kerning_x, DT_SWORD* kerning_y);
/* Get extra data that might be associated with a character */
DT_UBYTE* DTYPE_APICALL dtCharGetExtras(DT_DTENGINE engine, DT_ID_SWORD font_index, DT_ID_ULONG char_code, DT_UWORD flags, const DT_SLONG params[], DT_SLONG* len, DT_UWORD* xtra_bits);

/* Render custom shapes to Output */
DT_SWORD DTYPE_APICALL dtShapesDoOutput(DT_DTENGINE engine, const DT_LINEDECOR_L* linedecor, DT_ID_SWORD bitmap_flag, DT_SRAST_L x, DT_SRAST_L y, const DT_RECT_L* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[], DT_BMP* memory_bitmap);
/* Render custom shapes to Output - XL */
DT_SWORD DTYPE_APICALL dtShapesDoOutputXL(DT_DTENGINE engine, const DT_LINEDECOR_XL* linedecor, DT_ID_SWORD bitmap_flag, DT_SRAST_XL x, DT_SRAST_XL y, const DT_RECT_XL* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[], DT_BMP* memory_bitmap);
/* Render custom shapes to Output - fractional version */
DT_SWORD DTYPE_APICALL dtShapesDoOutputFrac(DT_DTENGINE engine, const DT_LINEDECOR_L* linedecor, DT_ID_SWORD bitmap_flag, DT_SRAST_L x, DT_SRAST_L y, const DT_RECT_L* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_SFRAC_L x_arr[], const DT_SFRAC_L y_arr[], DT_BMP* memory_bitmap);
/* Render custom shapes to Output - fractional version - XL */
DT_SWORD DTYPE_APICALL dtShapesDoOutputFracXL(DT_DTENGINE engine, const DT_LINEDECOR_XL* linedecor, DT_ID_SWORD bitmap_flag, DT_SRAST_XL x, DT_SRAST_XL y, const DT_RECT_XL* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_SFRAC_XL x_arr[], const DT_SFRAC_XL y_arr[], DT_BMP* memory_bitmap);

/* Render custom shape to Output - floating point version - deprecated form */
DT_SWORD DTYPE_APICALL dtShapeDoOutput(DT_DTENGINE engine, DT_ID_SWORD bitmap_flag, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[], DT_BMP* memory_bitmap);
/* Render custom shape to Output (decorated) - floating point version - deprecated */
DT_SWORD DTYPE_APICALL dtShapeDoOutputDecorated(DT_DTENGINE engine, const DT_LINEDECOR_L* linedecor, DT_ID_SWORD bitmap_flag, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[], DT_BMP* memory_bitmap);
/* Render custom shape to Output (enhanced) - floating point version - deprecated */
DT_SWORD DTYPE_APICALL dtShapeDoOutputPlus(DT_DTENGINE engine, DT_SLONG thickness, DT_SLONG segment, DT_ID_SWORD bitmap_flag, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[], DT_BMP* memory_bitmap);
/* Render custom shape to Output - fractional version - deprecated */
DT_SWORD DTYPE_APICALL dtShapeDoOutputFrac(DT_DTENGINE engine, DT_ID_SWORD bitmap_flag, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_SFRAC_L x_arr[], const DT_SFRAC_L y_arr[], DT_BMP* memory_bitmap);
/* Render custom shape to Output (decorated) - fractional version - deprecated */
DT_SWORD DTYPE_APICALL dtShapeDoOutputFracDecorated(DT_DTENGINE engine, const DT_LINEDECOR_L* linedecor, DT_ID_SWORD bitmap_flag, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_SFRAC_L x_arr[], const DT_SFRAC_L y_arr[], DT_BMP* memory_bitmap);
/* Render custom shape to Output (enhanced) - fractional version - deprecated */
DT_SWORD DTYPE_APICALL dtShapeDoOutputFracPlus(DT_DTENGINE engine, DT_SLONG thickness, DT_SLONG segment, DT_ID_SWORD bitmap_flag, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_SFRAC_L x_arr[], const DT_SFRAC_L y_arr[], DT_BMP* memory_bitmap);

/* Render grayscale bitmap to Output */
void DTYPE_APICALL dtBitmapDoOutput(DT_DTENGINE engine, const DT_BMP* memory_bitmap, DT_SRAST_L x, DT_SRAST_L y);
/* Render grayscale bitmap to Output - XL */
void DTYPE_APICALL dtBitmapDoOutputXL(DT_DTENGINE engine, const DT_BMP* memory_bitmap, DT_SRAST_XL x, DT_SRAST_XL y);
/* Invert grayscale bitmap */
void DTYPE_APICALL dtBitmapInvert(DT_DTENGINE engine, DT_BMP* memory_bitmap);

/* Operation on grayscale bitmap */
DT_SWORD DTYPE_APICALL dtBitmapOperation(DT_DTENGINE engine, DT_UBYTE* bitmap_addr, DT_SLONG bitmap_w, DT_SLONG bitmap_h, DT_ID_SWORD operation_id);
/* Compress grayscale bitmap data */
DT_SLONG DTYPE_APICALL dtBitmapCompress(DT_DTENGINE engine, DT_UBYTE* bitmap_addr, DT_SLONG bitmap_len);
/* Compress grayscale bitmap memory */
DT_SLONG DTYPE_APICALL dtBitmapCompressPlus(DT_DTENGINE engine, const DT_UBYTE* bitmap_addr, DT_UBYTE* compr_addr, DT_SLONG bitmap_len, DT_UBYTE fill_byte, DT_UBYTE fill_unused, DT_UBYTE bpp);
/* Decompress grayscale bitmap data */
DT_UBYTE* DTYPE_APICALL dtBitmapDecompress(DT_DTENGINE engine, const DT_UBYTE* bitmap_addr, DT_SLONG bitmap_len);
/* Decompress grayscale bitmap memory */
DT_UBYTE* DTYPE_APICALL dtBitmapDecompressPlus(DT_DTENGINE engine, const DT_UBYTE* bitmap_addr, DT_SLONG bitmap_len, DT_SLONG compr_len, DT_UBYTE fill_byte, DT_UBYTE ignore_error, DT_UBYTE bpp);

/* Probe style effects */
DT_SWORD DTYPE_APICALL dtProbeStyleEffects(const DT_STYLE_EFFECTS* style_effects_1, const DT_STYLE_EFFECTS* style_effects_2, DT_SWORD reserved);
/* Probe type effects */
DT_SWORD DTYPE_APICALL dtProbeTypeEffects(const DT_TYPE_EFFECTS_L* type_effects_1, const DT_TYPE_EFFECTS_L* type_effects_2, DT_SWORD reserved);
/* Probe type effects - XL */
DT_SWORD DTYPE_APICALL dtProbeTypeEffectsXL(const DT_TYPE_EFFECTS_XL* type_effects_1, const DT_TYPE_EFFECTS_XL* type_effects_2, DT_SWORD reserved);

/* Free grayscale bitmap */
void DTYPE_APICALL dtBitmapFree(DT_DTENGINE engine, DT_UBYTE* bitmap_addr);

/* Free memory allocated by the engine */
void DTYPE_APICALL dtFree(void* mem_addr);

/* Render G, GA, RGB or RGBA bitmap image to Output */
DT_SWORD DTYPE_APICALL dtImageDoOutput(DT_DTENGINE engine, const DT_UBYTE* img_m, DT_UWORD img_w, DT_UWORD img_h, DT_SLONG img_pitch, DT_UBYTE img_bits_per_pixel, DT_UBYTE reserved, DT_UBYTE transparency, DT_UBYTE quality, DT_UBYTE alpha_treatment, DT_UBYTE edge_smoothing, const DT_UBYTE background_arr[], const DT_UBYTE channel_ordering_arr[], const DT_UBYTE rr[], const DT_UBYTE gg[], const DT_UBYTE bb[], DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, const DT_TM2X2 transform);


/* ------------------------- D-Type Direct Color RGBA Rasterizer ------------------------- */

typedef struct DT_DCRASTER_STRUCT* DT_DCRASTER;


/* Structures */


typedef struct
{
	DT_SFRAC_L cx1, cx2;
	DT_SFRAC_L cy1, cy2;
	DT_ULONG contour_id;
	DT_UBYTE g;
	DT_UBYTE reserved1;
	DT_UBYTE reserved2;
	DT_UBYTE debug_id;

} DT_SUBSCAN_INTERSECT;


typedef struct
{
	DT_SFRAC_L x, y;

	union
	{
		DT_UBYTE rgbt[4];
		DT_ULONG color;
	};

} DT_COLOR_SOURCE;


/* D-Type Direct Color Rasterizer Functions */

/* Create and initialize D-Type Direct Color Rasterizer */
DT_SWORD DTYPE_APICALL dcRasterizerIni(DT_DCRASTER* dcr, const DT_MDC* memory_surface, DT_SLONG raster_intersect, DT_ID_SLONG raster_id);
/* Create and initialize D-Type Direct Color Rasterizer */
DT_SWORD DTYPE_APICALL dcRasterizerIniPlus(DT_DCRASTER* dcr, const DT_MDC* memory_surface, DT_SLONG raster_intersect, DT_ID_SLONG raster_id, DT_DASYS das, DT_UWORD flags);
/* Deinitialize and destroy D-Type Direct Color Rasterizer */
void DTYPE_APICALL dcRasterizerExt(DT_DCRASTER dcr);

/* Set rasterizer callback */
void DTYPE_APICALL dcRasterizerSetSubscanCallback(DT_DCRASTER dcr, void (DTYPE_USRCALL *user_subscan_func)(DT_SRAST_L x, DT_SRAST_L y, const DT_SUBSCAN_INTERSECT* is, void* user_data), void* user_data);

/* Set parameters */
void DTYPE_APICALL dcRasterizerSetParams(DT_DCRASTER dcr, const DT_UBYTE color[], DT_ID_UBYTE fill_rule, DT_ID_UBYTE reserved1, DT_SWORD reserved2);
/* Set fill rule */
void DTYPE_APICALL dcRasterizerSetFillRule(DT_DCRASTER dcr, DT_ID_UBYTE fill_rule);
/* Get fill rule */
void DTYPE_APICALL dcRasterizerGetFillRule(DT_DCRASTER dcr, DT_ID_UBYTE* fill_rule);

/* Set ASync System */
void DTYPE_APICALL dcRasterizerSetASys(DT_DCRASTER dcr, DT_DASYS das);
/* Get ASync System */
void DTYPE_APICALL dcRasterizerGetASys(DT_DCRASTER dcr, DT_DASYS* das);

/* MoveTo command */
void DTYPE_APICALL dcRasterizerMoveTo(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1, DT_UBYTE r, DT_UBYTE g, DT_UBYTE b, DT_UBYTE t);
/* MoveTo2 command */
void DTYPE_APICALL dcRasterizerMoveTo2(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1, const DT_UBYTE color[]);
/* MoveToFrac command */
void DTYPE_APICALL dcRasterizerMoveToFrac(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1, DT_UBYTE r, DT_UBYTE g, DT_UBYTE b, DT_UBYTE t);
/* MoveToFrac command - XL */
void DTYPE_APICALL dcRasterizerMoveToFracXL(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1, DT_UBYTE r, DT_UBYTE g, DT_UBYTE b, DT_UBYTE t);
/* MoveToFrac2 command */
void DTYPE_APICALL dcRasterizerMoveToFrac2(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1, const DT_UBYTE color[]);
/* MoveToFrac2 command - XL */
void DTYPE_APICALL dcRasterizerMoveToFracXL2(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1, const DT_UBYTE color[]);

/* LineTo command */
void DTYPE_APICALL dcRasterizerLineTo(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1);
/* LineTo2 command */
void DTYPE_APICALL dcRasterizerLineTo2(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1, const DT_UBYTE color[]);
/* LineTo3 command */
void DTYPE_APICALL dcRasterizerLineTo3(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1, DT_UBYTE r, DT_UBYTE g, DT_UBYTE b, DT_UBYTE t);
/* LineToFrac command */
void DTYPE_APICALL dcRasterizerLineToFrac(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1);
/* LineToFrac command - XL */
void DTYPE_APICALL dcRasterizerLineToFracXL(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1);
/* LineToFrac2 command */
void DTYPE_APICALL dcRasterizerLineToFrac2(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1, const DT_UBYTE color[]);
/* LineToFrac2 command - XL */
void DTYPE_APICALL dcRasterizerLineToFracXL2(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1, const DT_UBYTE color[]);
/* LineToFrac3 command */
void DTYPE_APICALL dcRasterizerLineToFrac3(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1, DT_UBYTE r, DT_UBYTE g, DT_UBYTE b, DT_UBYTE t);
/* LineToFrac3 command - XL */
void DTYPE_APICALL dcRasterizerLineToFracXL3(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1, DT_UBYTE r, DT_UBYTE g, DT_UBYTE b, DT_UBYTE t);

/* BSplineTo command */
void DTYPE_APICALL dcRasterizerBSplineTo(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_SLONG seg);
/* BSplineTo2 command */
void DTYPE_APICALL dcRasterizerBSplineTo2(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_SLONG seg, const DT_UBYTE color1[], const DT_UBYTE color2[]);
/* BSplineToFrac command */
void DTYPE_APICALL dcRasterizerBSplineToFrac(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1, DT_SFRAC_L x2, DT_SFRAC_L y2, DT_SLONG seg);
/* BSplineToFrac command - XL */
void DTYPE_APICALL dcRasterizerBSplineToFracXL(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1, DT_SFRAC_XL x2, DT_SFRAC_XL y2, DT_SLONG seg);
/* BSplineToFrac2 command */
void DTYPE_APICALL dcRasterizerBSplineToFrac2(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1, DT_SFRAC_L x2, DT_SFRAC_L y2, DT_SLONG seg, const DT_UBYTE color1[], const DT_UBYTE color2[]);
/* BSplineToFrac2 command - XL */
void DTYPE_APICALL dcRasterizerBSplineToFracXL2(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1, DT_SFRAC_XL x2, DT_SFRAC_XL y2, DT_SLONG seg, const DT_UBYTE color1[], const DT_UBYTE color2[]);

/* BezierTo command */
void DTYPE_APICALL dcRasterizerBezierTo(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_FLOAT x3, DT_FLOAT y3, DT_SLONG seg);
/* BezierTo2 command */
void DTYPE_APICALL dcRasterizerBezierTo2(DT_DCRASTER dcr, DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_FLOAT x3, DT_FLOAT y3, DT_SLONG seg, const DT_UBYTE color1[], const DT_UBYTE color2[], const DT_UBYTE color3[]);
/* BezierToFrac command */
void DTYPE_APICALL dcRasterizerBezierToFrac(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1, DT_SFRAC_L x2, DT_SFRAC_L y2, DT_SFRAC_L x3, DT_SFRAC_L y3, DT_SLONG seg);
/* BezierToFrac command - XL */
void DTYPE_APICALL dcRasterizerBezierToFracXL(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1, DT_SFRAC_XL x2, DT_SFRAC_XL y2, DT_SFRAC_XL x3, DT_SFRAC_XL y3, DT_SLONG seg);
/* BezierToFrac2 command */
void DTYPE_APICALL dcRasterizerBezierToFrac2(DT_DCRASTER dcr, DT_SFRAC_L x1, DT_SFRAC_L y1, DT_SFRAC_L x2, DT_SFRAC_L y2, DT_SFRAC_L x3, DT_SFRAC_L y3, DT_SLONG seg, const DT_UBYTE color1[], const DT_UBYTE color2[], const DT_UBYTE color3[]);
/* BezierToFrac2 command - XL */
void DTYPE_APICALL dcRasterizerBezierToFracXL2(DT_DCRASTER dcr, DT_SFRAC_XL x1, DT_SFRAC_XL y1, DT_SFRAC_XL x2, DT_SFRAC_XL y2, DT_SFRAC_XL x3, DT_SFRAC_XL y3, DT_SLONG seg, const DT_UBYTE color1[], const DT_UBYTE color2[], const DT_UBYTE color3[]);

/* MultiSegment command */
DT_SWORD DTYPE_APICALL dcRasterizerMultiSegment(DT_DCRASTER dcr, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_UBYTE c_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[]);
/* MultiSegmentFrac command */
DT_SWORD DTYPE_APICALL dcRasterizerMultiSegmentFrac(DT_DCRASTER dcr, DT_SFRAC_L x, DT_SFRAC_L y, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_UBYTE c_arr[], const DT_SFRAC_L x_arr[], const DT_SFRAC_L y_arr[]);
/* MultiSegment command - XL */
DT_SWORD DTYPE_APICALL dcRasterizerMultiSegmentFracXL(DT_DCRASTER dcr, DT_SFRAC_XL x, DT_SFRAC_XL y, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_UBYTE c_arr[], const DT_SFRAC_XL x_arr[], const DT_SFRAC_XL y_arr[]);

/* Picture input */
DT_SWORD DTYPE_APICALL dcRasterizerVectorPicture(DT_DCRASTER dcr, const DT_COLOR_SOURCE color_sources_val[], const DT_UBYTE color_sources_flg[], const DT_SLONG color_sources_off[], DT_SLONG color_sources_n, DT_SWORD color_model);

/* Render scene */
void DTYPE_APICALL dcRasterizerDoOutput(DT_DCRASTER dcr, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_SWORD reserved);


#ifdef __cplusplus
}
#endif


#endif /* DTYPE_DTYPESTD_HI_H */
