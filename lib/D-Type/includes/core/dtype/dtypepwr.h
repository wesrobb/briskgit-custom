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


#ifndef DTYPE_DTYPEPWR_H
#define DTYPE_DTYPEPWR_H


#ifdef __cplusplus
DT_EXTERN {
#endif


/* ------------------------- D-Type Power Engine ------------------------- */

typedef struct DT_PDENGINE_STRUCT* DT_PDENGINE;
typedef struct DT_PDDOC_STRUCT* DT_PDDOC;
typedef struct DT_PDDOCDRAW_R1_STRUCT* DT_PDDOCDRAW_R1;
typedef struct DT_PDDOCDRAW_R2_STRUCT* DT_PDDOCDRAW_R2;


/* Structures */


/* The Canvas structure */
typedef struct
{
	DT_PDDOC PowerDocInst;      /* Document Instance */
	DT_PDENGINE PowerEngInst;   /* Associated Power Engine Instance */
	DT_DTENGINE DTypeEngInst;   /* Associated Standard Engine Instance */
	DT_SLONG Page;              /* Current page */
	DT_SLONG PageParams[2];     /* Reserved page parameters */
	DT_SRAST_L Width;           /* Surface's width in pixels */
	DT_SRAST_L Height;          /* Surface's height in pixels */
	DT_ID_SWORD Format;         /* Surface's format (8, 16, 24 or 32) */
	DT_ID_SWORD Subformat;      /* Surface's subformat (see the manual) */
	DT_UBYTE* Memory;           /* Pointer to surface's memory buffer */
	DT_SLONG Length;            /* Length of the buffer */
	DT_SLONG Img;               /* Current PowerDoc image */
	DT_SLONG ImgParams[2];      /* Reserved image parameters */
	DT_ID_SLONG Obj;            /* Associated PowerDoc object */
	DT_ID_SLONG ObjParams[2];   /* Reserved object parameters */
	DT_SLONG Misc1;             /* Reserved */
	DT_SWORD Misc2;             /* Reserved */
	DT_UBYTE MiscBits;          /* Reserved */
	DT_UBYTE MaskType;          /* Reserved */
	DT_SRAST_L CX, CY, CW, CH;  /* Image's clipped bounding box in pixels */
	DT_SRAST_L BX, BY, BW, BH;  /* Reserved for internal use (alpha-masking) */
	DT_SLONG Mask;              /* Page for alpha-masking (reserved for internal use) */
	DT_UBYTE* Buffer;           /* Buffer for alpha-masking (reserved for internal use) */
	void* UserData;             /* User supplied data */
	DT_PDDOCDRAW_R1 Reserved1;  /* Reserved for internal use */
	DT_PDDOCDRAW_R2 Reserved2;  /* Reserved for internal use */
	DT_UWORD Flags;             /* Reserved for internal use */
	DT_UBYTE Status;            /* Image's status */
	DT_UBYTE Attribs;           /* Image's attributes */
	DT_ID_SLONG ParentObj;      /* Image's parent PowerDoc object */
	DT_SRAST_XL EX, EY, EW, EH; /* Image's extent in pixels (if known) */
	DT_SRAST_XL ImgX, ImgY;     /* Image's origin point in pixels */
	DT_SRAST_XL XOff, YOff;     /* Surface's offset in pixels */

} DT_PD_DOCDRAW_CANVAS;


/* The Frame structure - Format 1 (parallelogram) */
typedef struct
{
	DT_SLONG Idx;                                  /* Frame's Logical ID */
	DT_SLONG Row;                                  /* Frame's Row */
	DT_SRAST_L XMin, YMin, XMax, YMax, XMid, YMid; /* Frame's corner points in pixels (parallelogram) */
	DT_UBYTE Misc, Rsvd;

} DT_PD_FRAME_FORMAT1;


/* The Frame structure - Format 2 (arbitrary 4-point polygon with fractional pixel coordinates) */
typedef struct
{
	DT_SLONG Idx;                              /* Frame's Logical ID */
	DT_SLONG Row;                              /* Frame's Row */
	DT_SFRAC_L X1, Y1, X2, Y2, X3, Y3, X4, Y4; /* Frame's corner points in fractional pixels (4-point polygon) */
	DT_UBYTE Misc, Rsvd;

} DT_PD_FRAME_FORMAT2;


/* The Frame structure - Format 3 (parallelogram) - XL */
typedef struct
{
	DT_SLONG Idx;                                   /* Frame's Logical ID */
	DT_SLONG Row;                                   /* Frame's Row */
	DT_SRAST_XL XMin, YMin, XMax, YMax, XMid, YMid; /* Frame's corner points in pixels (parallelogram) */
	DT_UBYTE Misc, Rsvd;

} DT_PD_FRAME_FORMAT3;


/* The Frame structure - Format 4 (arbitrary 4-point polygon with fractional pixel coordinates) - XL */
typedef struct
{
	DT_SLONG Idx;                               /* Frame's Logical ID */
	DT_SLONG Row;                               /* Frame's Row */
	DT_SFRAC_XL X1, Y1, X2, Y2, X3, Y3, X4, Y4; /* Frame's corner points in fractional pixels (4-point polygon) */
	DT_UBYTE Misc, Rsvd;

} DT_PD_FRAME_FORMAT4;


/* The Frames union */
typedef union
{
	DT_PD_FRAME_FORMAT1* Format1Ptr; /* Frames in Format 1 */
	DT_PD_FRAME_FORMAT2* Format2Ptr; /* Frames in Format 2 */
	DT_PD_FRAME_FORMAT3* Format3Ptr; /* Frames in Format 3 */
	DT_PD_FRAME_FORMAT4* Format4Ptr; /* Frames in Format 4 */

} DT_PD_FRAMES;


/* The Text Flow structure */
typedef struct
{
	DT_SLONG ParentFlow;     /* Flow's parent Flow index */
	DT_SLONG ParentFrame;    /* Flow's parent Frame index */
	DT_SLONG LocalNumber;    /* Flow's local identifier */
	DT_SLONG Img;            /* Flow's PowerDoc Image */
	DT_ID_SLONG ParentObj;   /* Flow's parent PowerDoc Object */
	DT_PD_FRAMES Frames;     /* Flow's array of Frames (named union) */
	DT_SLONG FramesLen;      /* Number of Frames in the array */

} DT_PD_FLOW;


/* An array of Text Flows */
typedef struct
{
	DT_PD_FLOW* Ptr;  /* Points to the first Flow in the array */
	DT_SLONG Len;     /* Number of Flows in the array */

} DT_PD_FLOWS;


/* Optional parameters for rendering */
typedef struct
{
	DT_SWORD Flags;        /*  Bit 0: If unset, perform text layout operations only for text objects
	                                  and characters that are within the visible portion of the page.
	                                  If set, perform text layout operations for any and all text
	                                  objects and characters on the page, even if they are outside
	                                  the visible portion of the page.
	                           Bit 1: If unset, disable processing of critical object-specific errors.
	                                  If set, enable processing of critical object-specific errors. */

	DT_SWORD Origins;      /* -1 = Never draw origins
	                           0 = Draw origins only for selected PowerDoc objects
	                           1 = Draw origins for all PowerDoc objects */

	DT_SWORD Boundaries;   /* -1 = Never draw boundaries
	                           0 = Draw boundaries only for selected PowerDoc objects
	                               (not all PowerDoc objects have boundaries)
	                           1 = Draw boundaries for all PowerDoc objects
	                               (not all PowerDoc objects have boundaries) */

	DT_SWORD Background;   /*  0 = Do not clear surface's background
	                           1 = Clear surface's background using the R, G, B, A values below */

	DT_SWORD Flows;        /*  0 = Do not return text flows and frames resulting from text layout
	                               operations and do not pass them to the glyph callback function.
	                           1 = Return text flows and frames resulting from text layout
	                               operations via the FlowsPtr and FlowsLen structure members
	                               and also pass them to the glyph callback function.
	                               Use DT_PD_FRAME_FORMAT1 for all frames.
	                           2 = Return text flows and frames resulting from text layout
	                               operations via the FlowsPtr and FlowsLen structure members
	                               and also pass them to the glyph callback function.
	                               Use DT_PD_FRAME_FORMAT2 for all frames.
	                           3 = Return text flows and frames resulting from text layout
	                               operations via the FlowsPtr and FlowsLen structure members
	                               and also pass them to the glyph callback function.
	                               Use DT_PD_FRAME_FORMAT3 for all frames.
	                           4 = Return text flows and frames resulting from text layout
	                               operations via the FlowsPtr and FlowsLen structure members
	                               and also pass them to the glyph callback function.
	                               Use DT_PD_FRAME_FORMAT4 for all frames.
	                          10 = Only pass text flows and frames resulting from text layout
	                               operations to the glyph callback function.
	                               Use DT_PD_FRAME_FORMAT1 for all frames.
	                          20 = Only pass text flows and frames resulting from text layout
	                               operations to the glyph callback function.
	                               Use DT_PD_FRAME_FORMAT2 for all frames.
	                          30 = Only pass text flows and frames resulting from text layout
	                               operations to the glyph callback function.
	                               Use DT_PD_FRAME_FORMAT3 for all frames.
	                          40 = Only pass text flows and frames resulting from text layout
	                               operations to the glyph callback function.
	                               Use DT_PD_FRAME_FORMAT4 for all frames. */

	DT_SWORD Reserved;     /* Reserved for future use. Must be set to 0. */

	DT_UBYTE R, G, B, A;   /* R, G, B, A values of the surface's background */

	DT_PD_FLOWS FlowsArr;  /* Text flows generated during text layout operations
	                          (this is an input/output variable) */

	void* UserData;        /* User supplied data (will be passed to DT_PD_DOCDRAW_CANVAS) */

} DT_PD_DOCDRAW_PARAMS;


/* Style */
typedef struct
{
	DT_UBYTE Initialized;
	DT_UBYTE SingleChannelFilter;
	DT_UBYTE Reserved1;
	DT_UBYTE Reserved2;
	DT_ID_SLONG PatternFillDataPtr;
	DT_STYLE_EFFECTS SE;

} DT_PD_STYLE_EFFECTS;


/* Extra information for DT_PD_ORIGIN_L (legacy) */
typedef struct
{
	DT_UBYTE GlyphFlag;          /* Flag to identify the type of glyph */
	DT_UBYTE GlyphMisc;          /* Reserved for future use and presently set to 0 */
	DT_UWORD Reserved;           /* Reserved for future use and presently set to 0 */
	const DT_TYPE_ATTRIBS* Type; /* Typographical attributes of a glyph or character (will be DV_NULL for PowerFonts) */
	DT_TM3X3 Transform;          /* A 2D perspective (or projective) transformation matrix */
	DT_SRAST_L XOff, YOff;       /* Offset in whole pixels */

} DT_PD_ORIGIN_EXTRA;


/* Origin point - L */
typedef struct
{
	DT_SFRAC_L XA, YA;               /* Accurate origin point */
	DT_SFRAC_L XS, YS;               /* Snapped origin point */
	const DT_PD_ORIGIN_EXTRA* Extra; /* Optional extra info (will be DV_NULL if no extra info is available) */

} DT_PD_ORIGIN_L;

/* Alias */
typedef DT_PD_ORIGIN_L DT_PD_ORIGIN;


/* Extra glyph information for DT_PD_ORIGIN_XL */
typedef struct
{
	const DT_TYPE_EFFECTS_XL* Type; /* Typographical attributes of a glyph or character (will be DV_NULL for PowerFonts) */
	DT_SRAST_XL XOff, YOff;         /* Surface's offset in pixels */
	DT_UBYTE Reserved1;             /* Reserved for future use and presently set to 0 */
	DT_UBYTE Reserved2;             /* Reserved for future use and presently set to 0 */
	DT_UBYTE GlyphFlag;             /* Flag to identify the type of glyph */
	DT_UBYTE GlyphMisc;             /* Reserved for future use and presently set to 0 */
	DT_TM3X3 Transform;             /* A 2D perspective (or projective) transformation matrix */

} DT_PD_ORIGIN_GLYPH_XL;


/* Origin point - XL */
typedef struct
{
	DT_SFRAC_XL XA, YA;                 /* Accurate origin point - used for underline/strike-through effect */
	DT_SFRAC_XL XS, YS;                 /* Snapped origin point - used for positioning the glyphs */
	const DT_PD_ORIGIN_GLYPH_XL* Glyph; /* Optional glyph info (will be DV_NULL if no extra info is available) */

} DT_PD_ORIGIN_XL;


/* Parameters for Power Engine */
typedef struct
{
	DT_SLONG CacheBytes; /* Size limit for Power Engine's caching subsystem in bytes */
	DT_SLONG CacheItems; /* The maximum number of items for Power Engine's caching subsystem */
	DT_UBYTE Reserved1;  /* Reserved for future use */
	DT_UBYTE Reserved2;  /* Reserved for future use */

} DT_PD_ENGINE_PARAMS;


/* Parameters for Power Engine documents */
typedef struct
{
	DT_SRAST_L SizeOrigin;        /* Box size for origin points of non-selected PowerDoc images */
	DT_SRAST_L SizeSelect;        /* Box size for origin points of selected PowerDoc images */
	DT_SRAST_L SizeClipExtendX;   /* An internal library parameter. Must be set to 10. */
	DT_SRAST_L SizeClipExtendY;   /* An internal library parameter. Must be set to 10. */
	DT_UBYTE CacheFlagForMasking; /* Specifies which alpha-masking algorithm to use */
	DT_UBYTE CacheFlagForGlyphs;  /* Specifies which caching algorithm to use */
	DT_UBYTE Reserved1;           /* A set of bit values for missing (undefined) glyphs and tabs */
	DT_UBYTE Reserved2;           /* Miscellaneous bits */

} DT_PD_DOC_PARAMS;


/* Nest configuration */
typedef struct
{
	DT_SLONG InternalPage;   /* 999998 */
	DT_SLONG Params[4];
	DT_SLONG FontW;
	DT_SLONG FontH;
	DT_ID_SWORD FontIndex;
	DT_ID_SWORD MiscParam;   /* unused */
	DT_UBYTE RGBT[4];
	DT_UBYTE RGBT_Flag;
	DT_UBYTE Reserved1;      /* unused */
	DT_UBYTE Reserved2;      /* unused */
	DT_ID_SLONG PropName_Dw; /* fcDw */
	DT_ID_SLONG PropName_Dh; /* fcDh */

} DT_PD_NEST_CONFIG;


/* Power Engine Format Identifiers */
#define PD_BYTE -1
#define PD_WORD -2
#define PD_TRIO -3
#define PD_LONG -4
#define PD_BYTE_SIGNED PD_BYTE
#define PD_WORD_SIGNED PD_WORD
#define PD_TRIO_SIGNED PD_TRIO
#define PD_LONG_SIGNED PD_LONG
#define PD_BYTE_UNSIGNED -10
#define PD_WORD_UNSIGNED -20
#define PD_TRIO_UNSIGNED -30
#define PD_LONG_UNSIGNED -40

#define PD_ARR_BYTE -5
#define PD_ARR_WORD -6
#define PD_ARR_TRIO -7
#define PD_ARR_LONG -8
#define PD_ARR_BYTE_SIGNED PD_ARR_BYTE
#define PD_ARR_WORD_SIGNED PD_ARR_WORD
#define PD_ARR_TRIO_SIGNED PD_ARR_TRIO
#define PD_ARR_LONG_SIGNED PD_ARR_LONG
#define PD_ARR_BYTE_UNSIGNED -50
#define PD_ARR_WORD_UNSIGNED -60
#define PD_ARR_TRIO_UNSIGNED -70
#define PD_ARR_LONG_UNSIGNED -80

#define PD_ASC -100
#define PD_HEX -101
#define PD_DEFAULT -128
#define PD_LINK -129
#define PD_OTHER -130

#define PD_ARR_BYTE_SIGNED_COMPRESS -51
#define PD_ARR_WORD_SIGNED_COMPRESS -61
#define PD_ARR_TRIO_SIGNED_COMPRESS -71
#define PD_ARR_LONG_SIGNED_COMPRESS -81
#define PD_ARR_BYTE_UNSIGNED_COMPRESS -52
#define PD_ARR_WORD_UNSIGNED_COMPRESS -62
#define PD_ARR_TRIO_UNSIGNED_COMPRESS -72
#define PD_ARR_LONG_UNSIGNED_COMPRESS -82

#define PD_ARR_COMPRESSED -110
#define PD_HEX_COMPRESSED -111

#define PD_HEX_COMPRESS_METHOD_FIRST -200
#define PD_HEX_COMPRESS_METHOD_000 -200
#define PD_HEX_COMPRESS_METHOD_002 -202
#define PD_HEX_COMPRESS_METHOD_004 -204
#define PD_HEX_COMPRESS_METHOD_006 -206
#define PD_HEX_COMPRESS_METHOD_008 -208
#define PD_HEX_COMPRESS_METHOD_009 -209
#define PD_HEX_COMPRESS_METHOD_010 -210
#define PD_HEX_COMPRESS_METHOD_011 -211
#define PD_HEX_COMPRESS_METHOD_012 -212
#define PD_HEX_COMPRESS_METHOD_015 -215
#define PD_HEX_COMPRESS_METHOD_LAST -217

#define PD_ARR_COMPRESS_METHOD_FIRST -500
#define PD_ARR_COMPRESS_METHOD_000 -500
#define PD_ARR_COMPRESS_METHOD_002 -502
#define PD_ARR_COMPRESS_METHOD_004 -504
#define PD_ARR_COMPRESS_METHOD_006 -506
#define PD_ARR_COMPRESS_METHOD_008 -508
#define PD_ARR_COMPRESS_METHOD_009 -509
#define PD_ARR_COMPRESS_METHOD_010 -510
#define PD_ARR_COMPRESS_METHOD_011 -511
#define PD_ARR_COMPRESS_METHOD_012 -512
#define PD_ARR_COMPRESS_METHOD_015 -515
#define PD_ARR_COMPRESS_METHOD_LAST -517


/* Defines - Miscellaneous Numeric Values */
#define PD_NVAL_THICKNESS_INVERT_LINE        0
#define PD_NVAL_THICKNESS_IMG_BOUNDARY       1
#define PD_NVAL_THICKNESS_IMG_ORIGIN_VISIBLE 2
#define PD_NVAL_THICKNESS_IMG_ORIGIN_HIDDEN  3
#define PD_NVAL_COLOR_IMG_ORIGIN_VISIBLE     4
#define PD_NVAL_COLOR_IMG_ORIGIN_HIDDEN      5
#define PD_NVAL_COLOR_IMG_BOUNDARY           6
#define PD_NVAL_COLOR_IMG_ORIGIN_FILL        7
#define PD_NVAL_TEXT_BLANKS_SIZE             8
#define PD_NVAL_TEXT_PRINT_MODE              9
#define PD_NVAL_FONT_DISPLAY_LAYERS_GLYPHS  10
#define PD_NVAL_FONT_DISPLAY_BIMAGE_GLYPHS  11
#define PD_NVAL_FONT_DISPLAY_SVGDOC_GLYPHS  12
#define PD_NVAL_SELECT_EXTENSION            13
#define PD_NVAL_SIZE_ORIGIN                 14
#define PD_NVAL_SIZE_SELECT                 15
#define PD_NVAL_SIZE_CLIP_EXTEND_X          16
#define PD_NVAL_SIZE_CLIP_EXTEND_Y          17
#define PD_NVAL_CACHE_FLAG_FOR_MASKING      18
#define PD_NVAL_CACHE_FLAG_FOR_GLYPHS       19
#define PD_NVAL_CONTROL_GLYPHS              20
#define PD_NVAL_CONTROL_ALGORITHMS          21


/* Power Engine Field Types */
#define PD_FT_UNKNOWN 0
#define PD_FT_PROPERTY 1
#define PD_FT_LINK 2
#define PD_FT_PROPERTYREFERENCE 3
#define PD_FT_EXTPROPERTY_BUFFER 4
#define PD_FT_EXTPROPERTY_CALLBACK 5


/* Critical error codes while rendering objects */
#define PD_RET_OBJ_ERR_MDC        -8 /* memory surface not supported by object */
#define PD_RET_OBJ_ERR_PATTERN    -7 /* pattern file access error */
#define PD_RET_OBJ_ERR_BITMAP     -6 /* bitmap file access error */
#define PD_RET_OBJ_ERR_VECTOR     -5 /* vector file access error */
#define PD_RET_OBJ_ERR_FONT       -4 /* font file access error */
#define PD_RET_OBJ_ERR_MEMORY     -3 /* not enough memory to process the object */
#define PD_RET_OBJ_ERR_DEFINITION -2 /* object not properly defined */


/* Blend modes */
#define PD_BLEND_INVERT      0   /* Invert background */
#define PD_BLEND_HIGHLIGHT_1 1   /* Highlight over light background */
#define PD_BLEND_HIGHLIGHT_2 2   /* Highlight over dark background */
#define PD_BLEND_RENDER      255 /* Standard rendering */


/* Power Engine Field Names - Basic */
enum
{
	pd0 = 0, pdNull = pd0,
	pdReserved1 = 999,
	pdT,
	pdRGBT,
	pdStylePtr, pdSurfaceDefPtr = pdStylePtr,
	pdLineThickness,
	pdShadowDx,
	pdShadowDy,
	pdFontIndex,
	pdFontDw, pdFontWidth = pdFontDw,
	pdFontDh, pdFontHeight = pdFontDh,
	pdSkewHor,
	pdSkewVer,
	pdRotation,
	pdUnderline,
	pdBodyPtr, pdBodyDefPtr = pdBodyPtr,
	pdOutlinePtr, pdOutlineDefPtr = pdOutlinePtr,
	pdLinePtr, pdLineDefPtr = pdLinePtr,
	pdShadowPtr, pdShadowDefPtr = pdShadowPtr,
	pdID, pdIndex = pdID, pdCode = pdID,
	pdTypographyPtr,
	pdAlign,
	pdKerning,
	pdSpacingLetter, pdSpacingChar = pdSpacingLetter, pdSpacingGlyph = pdSpacingLetter,
	pdSpacingRow,
	pdTextString_08, pdTextString_8 = pdTextString_08, pdTextASCII = pdTextString_08,
	pdTextString_16, pdTextUnicode = pdTextString_16,
	pdTextAreaDw, pdTextAreaW = pdTextAreaDw,
	pdTextAreaDh, pdTextAreaH = pdTextAreaDh,
	pdDx,
	pdDy,
	pdDw,
	pdDh,
	pdRadius, pdRc = pdRadius,
	pdAngleAlpha, pdAlpha = pdAngleAlpha, pdAlpha0 = pdAngleAlpha,
	pdRoundX, pdRound = pdRoundX,
	pdAngleRotation, pdAngle = pdAngleRotation,
	pdRadiusHor, pdRx = pdRadiusHor,
	pdRadiusVer, pdRy = pdRadiusVer,
	pdAngleTheta, pdTheta = pdAngleTheta,
	pdTextFragmentPtr,
	pdNw,
	pdNh,
	pdPolyArrI, pdPolyIArr = pdPolyArrI,
	pdPolyArrX_32, pdPolyXArr = pdPolyArrX_32,
	pdPolyArrY_32, pdPolyYArr = pdPolyArrY_32,
	pdGradientType,
	pdImageDataRGB, pdBitmapDataRGB = pdImageDataRGB,
	pdImageDataGray, pdBitmapDataGray = pdImageDataGray,
	pdQuality,
	pdImageDataPtr, pdBitmapDataPtr = pdImageDataPtr,
	pdRangeRR, pdRR = pdRangeRR,
	pdRangeGG, pdGG = pdRangeGG,
	pdRangeBB, pdBB = pdRangeBB,
	pdLineSegment,
	pdPositioning, pdPrecisionType = pdPositioning,
	pdPatternGroup,
	pdPatternItem,
	pdLineStart,
	pdLineEnd,
	pdLineStartTheta,
	pdLineEndTheta,
	pdLineStartSize,
	pdLineEndSize,
	pdAlignEnd, pdAlignBreak = pdAlignEnd,
	pdEP,
	pdPolyFillRule,
	pdTextAreaRowType,
	pdWrapType, pdTextAreaWrapType = pdWrapType,
	pdMMContributions,
	pdFontUniqueID,
	pdFontDataPtr,
	pdFontDataFormat,
	pdFontDataBytes,
	pdFontDataNumber,
	pdTextAreaType,
	pdRelativeDirection,
	pdGlobalDirection,
	pdGlyphStart, pdStartPosition= pdGlyphStart,
	pdTextAreaEdgeLeft,
	pdTextAreaEdgeRight,
	pdTextEncoding,
	pdTextShaping,
	pdTextRulerPtr, /* deprecated */
	pdTabStopPositions,
	pdTabStopFlags,
	pdTextLayoutPtr,
	pdFnDrawObjectPtr,
	pdFnDraw,
	pdGroupMemberPtr,
	pdTransformMatrix,
	pdBoundingBox,
	pdSpacingFont,
	pdPowerFontPtr,
	pdPowerFontRecordPtr,
	pdMask, pdMaskPage = pdMask,
	pdAttribs,
	pdImageDataRGBAlpha,
	pdImageDataGrayAlpha,
	pdAlphaTreatment,
	pdFontDataHinting,
	pdFontDataCaching,
	pdHinting,
	pdDeviceMode,
	pdPosArrI, pdPosIArr = pdPosArrI,
	pdPosArrX_32, pdPosXArr = pdPosArrX_32,
	pdPosArrY_32, pdPosYArr = pdPosArrY_32,
	pdRelativeOrientation,
	pdIndentLeft,
	pdIndentRight,
	pdLineTreatment,
	pdTextAreaVAlign, pdVAlign = pdTextAreaVAlign,
	pdTextAreaVAlignEnd, pdVAlignEnd = pdTextAreaVAlignEnd, pdVAlignBreak = pdTextAreaVAlignEnd,
	pdTextRulerHorPtr,
	pdSpacingBaseline,
	pdBaselinePtr,
	pdUnderlinePtr,
	pdSpacingFactor,
	pdTextBreakFlags,
	pdTextHyphenBits,
	pdTextString_24,
	pdTextString_32,
	pdUserSpecificContent,
	pdUnderlineParam0,
	pdUnderlineParam1,
	pdUnderlineParam2,
	pdUnderlineParam3,
	pdUnderlineParam4,
	pdVAlignRow,
	pdEffectsArr,
	pdTextMiscAttribs,
	pdImageDataMono,
	pdPatternFillPtr,
	pdPatternROP,
	pdEdges,
	pdPitch,
	pdOrderingArr,
	pdTextLanguage,
	pdTextShapingParam0,
	pdSmoothing,
	pdBackground,
	pdName,
	pdPolyArrX_24,
	pdPolyArrX_16,
	pdPolyArrX_08, pdPolyArrX_8 = pdPolyArrX_08,
	pdPolyArrY_24,
	pdPolyArrY_16,
	pdPolyArrY_08, pdPolyArrY_8 = pdPolyArrY_08,
	pdPolyArrC, pdPolyCArr = pdPolyArrC,
	pdPosArrX_24,
	pdPosArrX_16,
	pdPosArrX_08, pdPosArrX_8 = pdPosArrX_08,
	pdPosArrY_24,
	pdPosArrY_16,
	pdPosArrY_08, pdPosArrY_8 = pdPosArrY_08,
	pdStitching,
	pdRasterizer,
	pdChannelFilter,
	pdLineDashSize,
	pdLineShift,
	pdPictureNumber,
	pdMarginLeft,
	pdMarginRight,
	pdPowerGlyphAttribs,
	pdTextMiscAttribs2,
	pdDynamicDw,
	pdDynamicDh,
	pdDynamicDx,
	pdDynamicDy,
	pdNwHead,
	pdNwFoot,
	pdNhHead,
	pdNhFoot,
	pdPowerGlyphStatus,
	pdTextExtrasPtr,
	pdRowBoundPtr,
	pdMarginTop,
	pdMarginBottom,
	pdMerge,
	pdTypoFramePtr,
	pdLinePerimeterGrow,
	pdRowInvertArr,
	pdRoundY,
	pdMaskPtr, pdMaskObjPtr = pdMaskPtr,
	pdReserved2
};


/* Power Engine Field Names - Extra */
enum
{
	pdxVAlignCell = 2099,
	pdxVAlignRow,
	pdxNw,
	pdxNh,
	pdxSourceCode,
	pdxMediaDescriptor,
	pdxMediaClipID,
	pdxMediaPlayState,
	pdxMediaInfoFPS,
	pdxMediaInfoDuration,
	pdxMediaInfoFramesCount,
	pdxMediaInfoCurrentTime,
	pdxMediaInfoCurrentFrame,
	pdxMediaReserved1,
	pdxMediaReserved2,
	pdxMediaStartTime,
	pdxMediaDataPtr,
	pdxMediaDataBytes,
	pdxMediaDataFormat,
	pdxMediaNextTime,
	pdxMediaNextFrame,
	pdxMediaNextCommand,
	pdxByteCode,
	pdxReserved1,
	pdxOriginLocation,
	pdxMagnitude
};


/* Power Engine Rendering Functions */
enum
{
	pdDrawSysGuideDashed = -3,
	pdDrawSysGuide = -2,
	pdDrawError = -1,
	pdDrawNothing = 0,
	pdDrawTextLine,
	pdDrawTextArc,
	pdDrawTextArea,
	pdDrawLine,
	pdDrawEllipse, pdDrawCircle = pdDrawEllipse,
	pdDrawRect,
	pdDrawTrueEllipse, pdDrawTrueCircle = pdDrawTrueEllipse,
	pdDrawSlice,
	pdDrawArc,
	pdDrawRichTextArea,
	pdDrawPattern,
	pdDrawGrid,
	pdDrawPoly, pdDrawPolygon = pdDrawPoly, pdDrawShape = pdDrawPoly,
	pdDrawGradient, pdDrawInverter = pdDrawGradient, pdDrawFastRect = pdDrawGradient,
	pdDrawImage, pdDrawBitmap = pdDrawImage,
	pdDrawChar,
	pdDrawRichTextLine,
	pdDrawLibraryPoly, pdDrawLibraryPolygon = pdDrawLibraryPoly, pdDrawLibraryShape = pdDrawLibraryPoly,
	pdDrawRichTextArc,
	pdDrawGlyph,
	pdDrawGenericObject,
	pdDrawGroup, pdDrawDefault = pdDrawGroup,
	pdDrawPowerGlyph,
	pdDrawPolyline,
	pdDrawLibraryPolyline,
	pdDrawTextDesign,
	pdDrawRichTextDesign,
	pdDrawTextPath,
	pdDrawRichTextPath,
	pdDrawLibraryImage, pdDrawLibraryBitmap = pdDrawLibraryImage,
	pdDrawBackground,
	pdDrawUser = 9999
};


/* D-Type Power Engine Functions */

/* Create and initialize D-Type Power Engine - via stream */
DT_SWORD DTYPE_APICALL pdEngineIniViaStream(DT_PDENGINE* engine, const DT_STREAM_DESC* sd, DT_INIT_INFO* init_info);
/* Deinitialize and destroy D-Type Power Engine */
void DTYPE_APICALL pdEngineExt(DT_PDENGINE engine);

/* Get D-Type Power Engine version */
DT_SLONG DTYPE_APICALL pdEngineGetVersion(DT_PDENGINE engine, DT_CHAR* ver_name, DT_CHAR* ver_snum, DT_CHAR* reserved);
/* Get associated D-Type Standard Engine Instance */
void DTYPE_APICALL pdEngineGetDTypeEngInst(DT_PDENGINE engine, DT_DTENGINE* dtype_engine);
/* Get associated hyphenator instance */
void DTYPE_APICALL pdEngineGetHyphenatorInst(DT_PDENGINE engine, DT_LXHYPHEN* hyphenator);
/* Get associated layout cache instance */
void DTYPE_APICALL pdEngineGetLayoutCacheInst(DT_PDENGINE engine, DT_LXCACHE* layout_cache);
/* Get Engine parameters */
DT_SWORD DTYPE_APICALL pdEngineGetParams(DT_PDENGINE engine, DT_PD_ENGINE_PARAMS* params, DT_SLONG reserved);
/* Set Engine parameters */
DT_SWORD DTYPE_APICALL pdEngineSetParams(DT_PDENGINE engine, const DT_PD_ENGINE_PARAMS* params, DT_SLONG reserved);

/* Create new stream */
DT_DTSTREAM DTYPE_APICALL pdEngineAddStream(DT_PDENGINE engine, DT_SWORD flags, DT_CHAR* fname, DT_UBYTE* source_addr, DT_SLONG source_len, DT_STREAM_DESC* sd);
/* Destroy last stream */
void DTYPE_APICALL pdEngineDelStream(DT_PDENGINE engine);

/* Create and initialize document */
DT_SWORD DTYPE_APICALL pdDocIni(DT_PDDOC* power_doc, DT_PDENGINE engine);
/* Append another document */
DT_SLONG DTYPE_APICALL pdDocAppend(DT_PDDOC power_doc, DT_SLONG page_off, DT_SLONG x_off, DT_SLONG y_off, const DT_PDDOC power_doc2);
/* Append another document */
DT_SLONG DTYPE_APICALL pdDocAppendPlus(DT_PDDOC power_doc, DT_SLONG page_off, DT_SLONG x_off, DT_SLONG y_off, const DT_PDDOC power_doc2, DT_UBYTE ignore_desc, DT_UBYTE ignore_img, DT_SWORD reserved);
/* Append document from stream */
DT_SLONG DTYPE_APICALL pdDocAppendFromStream(DT_PDDOC power_doc, DT_SLONG page_off, DT_SLONG x_off, DT_SLONG y_off, const DT_STREAM_DESC* sd);
/* Append document from stream */
DT_SLONG DTYPE_APICALL pdDocAppendFromStreamPlus(DT_PDDOC power_doc, DT_SLONG page_off, DT_SLONG x_off, DT_SLONG y_off, const DT_STREAM_DESC* sd, DT_UBYTE ignore_desc, DT_UBYTE ignore_img, DT_SWORD reserved);
DT_SLONG DTYPE_APICALL pdDocAppendSVG(DT_PDDOC power_doc, const DT_UBYTE* svg_content, DT_SLONG svg_len, const DT_CHAR* svg_object, DT_SLONG page_off, DT_UWORD flags, DT_FLOAT width, DT_FLOAT height);
/* Save document to stream */
DT_SLONG DTYPE_APICALL pdDocSaveToStream(DT_PDDOC power_doc, DT_STREAM_DESC* sd);
/* Transfer another document's settings */
DT_SWORD DTYPE_APICALL pdDocTransfer(DT_PDDOC power_doc, const DT_PDDOC power_doc2, DT_ULONG flags, DT_SWORD reserved);
/* Erase document */
void DTYPE_APICALL pdDocErase(DT_PDDOC power_doc);
/* Deinitialize and destroy document */
void DTYPE_APICALL pdDocExt(DT_PDDOC power_doc);

/* Set document's internal flags */
void DTYPE_APICALL pdDocSetInternalFlags(DT_PDDOC power_doc, DT_UWORD flags);
/* Set document's numeric value */
void DTYPE_APICALL pdDocSetNumericValue(DT_PDDOC power_doc, DT_ID_SWORD numeric_id, DT_SLONG numeric_value);
/* Set document's Layout Engine */
DT_SWORD DTYPE_APICALL pdDocSetLayoutEngine(DT_PDDOC power_doc, DT_ID_UBYTE layout_engine);
/* Set document parameters */
DT_SWORD DTYPE_APICALL pdDocSetParams(DT_PDDOC power_doc, const DT_PD_DOC_PARAMS* params, DT_SLONG reserved);
/* Set visibility filter */
DT_SWORD DTYPE_APICALL pdDocSetFilter(DT_PDDOC power_doc, DT_SWORD reserved, DT_ULONG filter);
/* Set the parameters of the inversion effect */
DT_SWORD DTYPE_APICALL pdDocSetInvert(DT_PDDOC power_doc, const DT_UBYTE invert_blend_arr[], DT_SLONG reserved);
/* Set document's transform - 2X2 transformation matrix */
void DTYPE_APICALL pdDocSetTransform(DT_PDDOC power_doc, const DT_TM2X2 transform);
/* Set document's transform - 3X3 transformation matrix */
void DTYPE_APICALL pdDocSetTransformPlus(DT_PDDOC power_doc, const DT_TM3X3 transform);
/* Set document's transform - simple scale */
void DTYPE_APICALL pdDocSetScale(DT_PDDOC power_doc, DT_FLOAT scale);
/* Set document's factors */
void DTYPE_APICALL pdDocSetFactors(DT_PDDOC power_doc, DT_SWORD reserved, const DT_FLOAT factors[]);
/* Set document cache */
DT_SWORD DTYPE_APICALL pdDocSetCache(DT_PDDOC power_doc, DT_ID_UBYTE action);
/* Set document's edge callback function */
DT_SWORD DTYPE_APICALL pdDocSetEdgeCallback(DT_PDDOC power_doc, DT_UBYTE (DTYPE_USRCALL *user_edge_func)(DT_SLONG line_nr, DT_SLONG y, DT_SLONG width, DT_SLONG height, DT_SLONG* edge_x, DT_SLONG* edge_w, void* user_param), void* user_param);
/* Set document's property callback function */
DT_SWORD DTYPE_APICALL pdDocSetPropCallback(DT_PDDOC power_doc, DT_UBYTE* (DTYPE_USRCALL *user_prop_func)(DT_ID_SLONG obj, DT_SLONG name, DT_SLONG* len, DT_SLONG user_id, void* user_param));
/* Set document's draw callback function */
DT_SWORD DTYPE_APICALL pdDocSetDrawCallback(DT_PDDOC power_doc, DT_SWORD (DTYPE_USRCALL *user_draw_func)(DT_PD_DOCDRAW_CANVAS* canvas, DT_SLONG fn_draw, const DT_TM3X3 tm, DT_SWORD flag));
/* Set document's render callback function */
DT_SWORD DTYPE_APICALL pdDocSetRenderCallback(DT_PDDOC power_doc, DT_SWORD (DTYPE_USRCALL *user_render_func)(DT_PD_DOCDRAW_CANVAS* canvas, DT_SLONG fn_draw, DT_SWORD flag, DT_SWORD reserved, DT_SLONG width, DT_SLONG height, const DT_TM3X3 tm, void* user_param), void* user_param);
/* Set document's glyph callback function - L */
DT_SWORD DTYPE_APICALL pdDocSetGlyphCallback(DT_PDDOC power_doc, DT_SWORD (DTYPE_USRCALL *user_glyph_func)(DT_ID_ULONG glyph_index, const DT_PD_ORIGIN_L* glyph_origin, const DT_PD_FLOW* glyph_flow, void* user_param), void* user_param);
/* Set document's glyph callback function - XL */
DT_SWORD DTYPE_APICALL pdDocSetGlyphCallbackXL(DT_PDDOC power_doc, DT_SWORD (DTYPE_USRCALL *user_glyph_func)(DT_ID_ULONG glyph_index, const DT_PD_ORIGIN_XL* glyph_origin, const DT_PD_FLOW* glyph_flow, void* user_param), void* user_param);
/* Set document's vector callback function - L */
DT_SWORD DTYPE_APICALL pdDocSetVectorCallback(DT_PDDOC power_doc, DT_ULONG misc_bits, DT_SWORD (DTYPE_USRCALL *user_vector_func)(DT_ULONG flags, DT_SRAST_L x, DT_SRAST_L y, const DT_UBYTE* instr_buffer, const DT_RECT_L* extent, DT_ID_SWORD bitmap_flag, DT_ID_UWORD layer, const DT_PD_DOCDRAW_CANVAS* canvas, void* user_param), void* user_param);
/* Set document's vector callback function - XL */
DT_SWORD DTYPE_APICALL pdDocSetVectorCallbackXL(DT_PDDOC power_doc, DT_ULONG misc_bits, DT_SWORD (DTYPE_USRCALL *user_vector_func)(DT_ULONG flags, DT_SRAST_XL x, DT_SRAST_XL y, const DT_UBYTE* instr_buffer, const DT_RECT_XL* extent, DT_ID_SWORD bitmap_flag, DT_ID_UWORD layer, const DT_PD_DOCDRAW_CANVAS* canvas, void* user_param), void* user_param);
/* Set document's callback function to be called if critical errors occur while rendering objects */
DT_SWORD DTYPE_APICALL pdDocSetErrorCallback(DT_PDDOC power_doc, void (DTYPE_USRCALL *user_error_func)(DT_PD_DOCDRAW_CANVAS* canvas, DT_SLONG fn_draw, DT_SWORD error_code));
/* Set document's style callback function - L */
DT_SWORD DTYPE_APICALL pdDocSetStyleCallback(DT_PDDOC power_doc, DT_SWORD (DTYPE_USRCALL *user_style_func)(const DT_PD_STYLE_EFFECTS* stl, DT_SWORD flag, DT_SFRAC_L dx, DT_SFRAC_L dy, const DT_LINEDECOR_L* linedecor, void* user_param), void* user_param);
/* Set document's style callback function - XL */
DT_SWORD DTYPE_APICALL pdDocSetStyleCallbackXL(DT_PDDOC power_doc, DT_SWORD (DTYPE_USRCALL *user_style_func)(const DT_PD_STYLE_EFFECTS* stl, DT_SWORD flag, DT_SFRAC_XL dx, DT_SFRAC_XL dy, const DT_LINEDECOR_XL* linedecor, void* user_param), void* user_param);

/* Get associated Power Engine Instance */
void DTYPE_APICALL pdDocGetPowerEngInst(DT_PDDOC power_doc, DT_PDENGINE* engine);
/* Get document size in bytes */
DT_SLONG DTYPE_APICALL pdDocGetSize(DT_PDDOC power_doc);
/* Get document size in bytes */
DT_SLONG DTYPE_APICALL pdDocGetSizePlus(DT_PDDOC power_doc, DT_SLONG* size_max, DT_SWORD reserved);
/* Get document's internal flags */
DT_UWORD DTYPE_APICALL pdDocGetInternalFlags(DT_PDDOC power_doc);
/* Get document's numeric value */
DT_SLONG DTYPE_APICALL pdDocGetNumericValue(DT_PDDOC power_doc, DT_ID_SWORD numeric_id);
/* Get document's Layout Engine */
DT_ID_UBYTE DTYPE_APICALL pdDocGetLayoutEngine(DT_PDDOC power_doc);
/* Get document parameters */
DT_SWORD DTYPE_APICALL pdDocGetParams(DT_PDDOC power_doc, DT_PD_DOC_PARAMS* params, DT_SLONG reserved);
/* Get visibility filter */
DT_ULONG DTYPE_APICALL pdDocGetFilter(DT_PDDOC power_doc, DT_SWORD reserved);
/* Get the parameters of the inversion effect */
DT_SWORD DTYPE_APICALL pdDocGetInvert(DT_PDDOC power_doc, DT_UBYTE invert_blend_arr[], DT_SLONG reserved);
/* Get document's transform - 2X2 transformation matrix */
void DTYPE_APICALL pdDocGetTransform(DT_PDDOC power_doc, DT_TM2X2 transform);
/* Get document's transform - 3X3 transformation matrix */
void DTYPE_APICALL pdDocGetTransformPlus(DT_PDDOC power_doc, DT_TM3X3 transform);

/* Draw document area */
DT_SLONG DTYPE_APICALL pdDocDraw(DT_PDDOC power_doc, DT_SLONG page, DT_SRAST_L x_off, DT_SRAST_L y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_MDC* memory_surface, DT_PD_DOCDRAW_PARAMS* params);
/* Draw document area */
DT_SLONG DTYPE_APICALL pdDocDrawXL(DT_PDDOC power_doc, DT_SLONG page, DT_SRAST_XL x_off, DT_SRAST_XL y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_MDC* memory_surface, DT_PD_DOCDRAW_PARAMS* params);
/* Invert document area (line) */
void DTYPE_APICALL pdDocInvertLine(DT_PDDOC power_doc, DT_SRAST_L x, DT_SRAST_L y, DT_SRAST_L w, DT_SRAST_L h, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface);
/* Invert document area (line) - XL */
void DTYPE_APICALL pdDocInvertLineXL(DT_PDDOC power_doc, DT_SRAST_XL x, DT_SRAST_XL y, DT_SRAST_XL w, DT_SRAST_XL h, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface);
/* Invert document area (rectangle) */
void DTYPE_APICALL pdDocInvertBox(DT_PDDOC power_doc, DT_SRAST_L x, DT_SRAST_L y, DT_SRAST_L w, DT_SRAST_L h, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface);
/* Invert document area (rectangle) - XL */
void DTYPE_APICALL pdDocInvertBoxXL(DT_PDDOC power_doc, DT_SRAST_XL x, DT_SRAST_XL y, DT_SRAST_XL w, DT_SRAST_XL h, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface);
/* Invert document area (quad) */
void DTYPE_APICALL pdDocInvertQuad(DT_PDDOC power_doc, DT_SRAST_L x1, DT_SRAST_L y1, DT_SRAST_L x2, DT_SRAST_L y2, DT_SRAST_L x3, DT_SRAST_L y3, DT_SRAST_L x4, DT_SRAST_L y4, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface);
/* Invert document area (quad) - XL */
void DTYPE_APICALL pdDocInvertQuadXL(DT_PDDOC power_doc, DT_SRAST_XL x1, DT_SRAST_XL y1, DT_SRAST_XL x2, DT_SRAST_XL y2, DT_SRAST_XL x3, DT_SRAST_XL y3, DT_SRAST_XL x4, DT_SRAST_XL y4, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface);
/* Invert document area (frames) */
void DTYPE_APICALL pdDocInvertFrames(DT_PDDOC power_doc, DT_ID_SWORD frames_format, DT_PD_FRAMES frames, DT_SLONG frames_len, DT_UBYTE* frames_flags, DT_SRAST_L x_off, DT_SRAST_L y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface);
/* Invert document area (frames) */
void DTYPE_APICALL pdDocInvertFramesXL(DT_PDDOC power_doc, DT_ID_SWORD frames_format, DT_PD_FRAMES frames, DT_SLONG frames_len, DT_UBYTE* frames_flags, DT_SRAST_XL x_off, DT_SRAST_XL y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface);

/* Get last PowerDoc object in document */
DT_SLONG DTYPE_APICALL pdObjGetLast(DT_PDDOC power_doc);
/* Add new object to document */
DT_ID_SLONG DTYPE_APICALL pdObjAdd(DT_PDDOC power_doc, DT_SLONG name, const DT_CHAR* desc);
/* Add new object to document */
DT_ID_SLONG DTYPE_APICALL pdObjAddPlus(DT_PDDOC power_doc, DT_SLONG name, const DT_CHAR* desc, DT_SLONG size);
/* Duplicate existing object */
DT_ID_SLONG DTYPE_APICALL pdObjDup(DT_PDDOC power_doc, DT_ID_SLONG obj);
/* Get object info */
DT_SLONG DTYPE_APICALL pdObjGet(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG* name, const DT_CHAR** desc);
/* Get object info */
DT_SLONG DTYPE_APICALL pdObjGetPlus(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG* name, const DT_CHAR** desc, DT_SLONG* size);
/* Change existing object info */
DT_SWORD DTYPE_APICALL pdObjSet(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_CHAR* desc);
/* Delete existing object */
void DTYPE_APICALL pdObjDel(DT_PDDOC power_doc, DT_ID_SLONG obj);
/* Delete all objects in document */
void DTYPE_APICALL pdObjDelAll(DT_PDDOC power_doc);

/* Get property or link */
DT_SLONG DTYPE_APICALL pdNameGet(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG idx, DT_UBYTE* type);

/* Add property */
DT_SWORD DTYPE_APICALL pdPropAddPlus(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len, DT_ULONG flags);
/* Add property */
DT_SWORD DTYPE_APICALL pdPropAdd(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len);
/* Add property */
DT_SWORD DTYPE_APICALL pdPropAddC(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len, DT_UBYTE compress);
/* Add property by reference */
DT_SWORD DTYPE_APICALL pdPropAddAsRef(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG ref_obj, DT_SLONG ref_name);
/* Add property - external buffer */
DT_SWORD DTYPE_APICALL pdPropAddAsExtBuffer(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* extbuffer, DT_SLONG extlen);
/* Add property - external callback */
DT_SWORD DTYPE_APICALL pdPropAddAsExtCallback(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG user_id, void* user_param);
/* Get property */
DT_UBYTE* DTYPE_APICALL pdPropGet(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG* len);
/* Get property */
DT_UBYTE* DTYPE_APICALL pdPropGetH(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG* len, DT_UBYTE* compress, DT_SLONG* compress_len);
/* Get property */
DT_UBYTE* DTYPE_APICALL pdPropGetC(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG* len, DT_UBYTE* must_free);
/* Get property by reference */
DT_ID_SLONG DTYPE_APICALL pdPropGetAsRef(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG* ref_name);
/* Change existing property */
DT_SWORD DTYPE_APICALL pdPropSet(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len);
/* Change existing property */
DT_SWORD DTYPE_APICALL pdPropSetC(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len, DT_UBYTE compress);
/* Change existing property by reference */
DT_SWORD DTYPE_APICALL pdPropSetAsRef(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG ref_obj, DT_SLONG ref_name);
/* Change existing property - external buffer */
DT_SWORD DTYPE_APICALL pdPropSetAsExtBuffer(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* extbuffer, DT_SLONG extlen);
/* Change existing property - external callback */
DT_SWORD DTYPE_APICALL pdPropSetAsExtCallback(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG user_id, void* user_param);
/* Delete existing property */
DT_SWORD DTYPE_APICALL pdPropDel(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name);

/* Add link */
DT_SWORD DTYPE_APICALL pdLinkAdd(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG link);
/* Get link */
DT_ID_SLONG DTYPE_APICALL pdLinkGet(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name);
/* Change existing link */
DT_SWORD DTYPE_APICALL pdLinkSet(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG link);
/* Delete existing link */
DT_SWORD DTYPE_APICALL pdLinkDel(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name);

/* Get last image in document */
DT_SLONG DTYPE_APICALL pdImgGetLast(DT_PDDOC power_doc);
/* Add new image with an optional 2x2 transformation matrix */
DT_SLONG DTYPE_APICALL pdImgAdd(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_UBYTE status, DT_UBYTE attribs, DT_SLONG mask, DT_ID_SWORD fn_draw, const DT_TM2X2 transform);
/* Add new image with an optional 3X3 transformation matrix */
DT_SLONG DTYPE_APICALL pdImgAddPlus(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_UBYTE status, DT_UBYTE attribs, DT_SLONG mask, DT_ID_SWORD fn_draw, const DT_TM3X3 transform);
/* Duplicate existing image */
DT_SLONG DTYPE_APICALL pdImgDup(DT_PDDOC power_doc, DT_SLONG img);
/* Get image's object */
DT_ID_SLONG DTYPE_APICALL pdImgGetObj(DT_PDDOC power_doc, DT_SLONG img);
/* Change image's object */
void DTYPE_APICALL pdImgSetObj(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj);
/* Get image's rendering function */
DT_SWORD DTYPE_APICALL pdImgGetFnDraw(DT_PDDOC power_doc, DT_SLONG img);
/* Change image's rendering function */
void DTYPE_APICALL pdImgSetFnDraw(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SWORD fn_draw);
/* Get image's page */
DT_SLONG DTYPE_APICALL pdImgGetPage(DT_PDDOC power_doc, DT_SLONG img);
/* Change image's page */
void DTYPE_APICALL pdImgSetPage(DT_PDDOC power_doc, DT_SLONG img, DT_SLONG page);
/* Get image's masking page */
DT_SLONG DTYPE_APICALL pdImgGetMask(DT_PDDOC power_doc, DT_SLONG img);
/* Change image's masking page */
void DTYPE_APICALL pdImgSetMask(DT_PDDOC power_doc, DT_SLONG img, DT_SLONG mask);
/* Get image's object origin point */
void DTYPE_APICALL pdImgGetXY(DT_PDDOC power_doc, DT_SLONG img, DT_SLONG* x, DT_SLONG* y);
/* Change image's object origin point */
void DTYPE_APICALL pdImgSetXY(DT_PDDOC power_doc, DT_SLONG img, DT_SLONG x, DT_SLONG y);
/* Get image's 2x2 transformation matrix */
void DTYPE_APICALL pdImgGetTransform(DT_PDDOC power_doc, DT_SLONG img, DT_TM2X2 transform);
/* Get image's 3X3 transformation matrix */
void DTYPE_APICALL pdImgGetTransformPlus(DT_PDDOC power_doc, DT_SLONG img, DT_TM3X3 transform);
/* Change image's 2x2 transformation matrix */
void DTYPE_APICALL pdImgSetTransform(DT_PDDOC power_doc, DT_SLONG img, const DT_TM2X2 transform);
/* Change image's 3X3 transformation matrix */
void DTYPE_APICALL pdImgSetTransformPlus(DT_PDDOC power_doc, DT_SLONG img, const DT_TM3X3 transform);
/* Get image's status */
DT_UBYTE DTYPE_APICALL pdImgGetStatus(DT_PDDOC power_doc, DT_SLONG img);
/* Change image's status */
void DTYPE_APICALL pdImgSetStatus(DT_PDDOC power_doc, DT_SLONG img, DT_UBYTE status);
/* Get image's attributes */
DT_UBYTE DTYPE_APICALL pdImgGetAttribs(DT_PDDOC power_doc, DT_SLONG img);
/* Change image's attributes */
void DTYPE_APICALL pdImgSetAttribs(DT_PDDOC power_doc, DT_SLONG img, DT_UBYTE attribs);
/* Get image's name */
DT_SWORD DTYPE_APICALL pdImgGetName(DT_PDDOC power_doc, DT_SLONG img);
/* Change image's name */
void DTYPE_APICALL pdImgSetName(DT_PDDOC power_doc, DT_SLONG img, DT_SWORD name);
/* Delete existing image */
void DTYPE_APICALL pdImgDel(DT_PDDOC power_doc, DT_SLONG img);
/* Delete all images in document */
void DTYPE_APICALL pdImgDelAll(DT_PDDOC power_doc);

/* Change image's attributes depending on its status */
void DTYPE_APICALL pdImgChangeAttribs(DT_PDDOC power_doc, DT_UBYTE status, DT_UBYTE attribs);
/* Send image to top of page */
void DTYPE_APICALL pdImgToTop(DT_PDDOC power_doc, DT_UBYTE status);
/* Send image to bottom of page */
void DTYPE_APICALL pdImgToBottom(DT_PDDOC power_doc, DT_UBYTE status);
/* Move image to another page */
void DTYPE_APICALL pdImgMoveToPage(DT_PDDOC power_doc, DT_UBYTE status, DT_SLONG page);
/* Delete image depending on its status */
void DTYPE_APICALL pdImgDelete(DT_PDDOC power_doc, DT_UBYTE status);

/* Adjust canvas */
DT_SWORD DTYPE_APICALL pdCanvasAdjust(DT_PD_DOCDRAW_CANVAS* canvas, DT_SWORD force_clip);
/* Draw in canvas using 2x2 transformation matrix */
DT_SWORD DTYPE_APICALL pdCanvasObjDraw(DT_PD_DOCDRAW_CANVAS* canvas, DT_ID_SWORD fn_draw, const DT_TM2X2 transform);
/* Draw in canvas using 3x3 transformation matrix */
DT_SWORD DTYPE_APICALL pdCanvasObjDrawPlus(DT_PD_DOCDRAW_CANVAS* canvas, DT_ID_SWORD fn_draw, const DT_TM3X3 transform);
/* Add frames to canvas */
DT_SWORD DTYPE_APICALL pdCanvasAddFrames(DT_PD_DOCDRAW_CANVAS* canvas, DT_SRAST_L x_off, DT_SRAST_L y_off, DT_SLONG idx_off, DT_ID_SLONG parent_obj, DT_SLONG local_number, DT_ID_SWORD frames_format, DT_PD_FRAMES frames, DT_SLONG frames_len);
/* Add frames to canvas */
DT_SWORD DTYPE_APICALL pdCanvasAddFramesXL(DT_PD_DOCDRAW_CANVAS* canvas, DT_SRAST_XL x_off, DT_SRAST_XL y_off, DT_SLONG idx_off, DT_ID_SLONG parent_obj, DT_SLONG local_number, DT_ID_SWORD frames_format, DT_PD_FRAMES frames, DT_SLONG frames_len);

/* Make a decimal string value using the DT_SLONG type as input */
void DTYPE_APICALL pdMakeDecFromSLong(DT_CHAR* value, DT_SLONG n, DT_CHAR x);
/* Make a decimal string value using the DT_ULONG type as input */
void DTYPE_APICALL pdMakeDecFromULong(DT_CHAR* value, DT_ULONG n, DT_CHAR x);
/* Make a decimal string value using the DT_UBYTE type as input */
void DTYPE_APICALL pdMakeDecFromUByte1(DT_CHAR* value, DT_UBYTE b1, DT_CHAR x);
/* Make a hexadecimal string value using 1 x DT_UBYTE type as input */
void DTYPE_APICALL pdMakeHexFromUByte1(DT_CHAR* value, DT_UBYTE b1, DT_CHAR x);
/* Make a hexadecimal string value using 2 x DT_UBYTE types as input */
void DTYPE_APICALL pdMakeHexFromUByte2(DT_CHAR* value, DT_UBYTE b1, DT_UBYTE b2, DT_CHAR x);
/* Make a hexadecimal string value using 3 x DT_UBYTE types as input */
void DTYPE_APICALL pdMakeHexFromUByte3(DT_CHAR* value, DT_UBYTE b1, DT_UBYTE b2, DT_UBYTE b3, DT_CHAR x);
/* Make a hexadecimal string value using 4 x DT_UBYTE types as input */
void DTYPE_APICALL pdMakeHexFromUByte4(DT_CHAR* value, DT_UBYTE b1, DT_UBYTE b2, DT_UBYTE b3, DT_UBYTE b4, DT_CHAR x);
/* Make binary buffer */
DT_UBYTE* DTYPE_APICALL pdMakeBinary(const DT_CHAR* value, DT_ID_SLONG format, DT_UBYTE* buffer, DT_SLONG* len, DT_UBYTE* dynamic);

/* Compress data in buffer */
DT_UBYTE* DTYPE_APICALL pdBufferCompress(const DT_UBYTE* arr, DT_ID_UBYTE version, DT_SLONG arr_size, DT_SLONG* len, DT_ID_UBYTE* best_sz);
/* Decompress data in buffer */
DT_UBYTE* DTYPE_APICALL pdBufferDecompress(const DT_UBYTE* addr, DT_SLONG len, DT_UBYTE head_only, DT_ID_UBYTE* version, DT_SLONG* arr_size);
/* Extract values in buffer */
DT_SWORD DTYPE_APICALL pdBufferExtractValues(const DT_UBYTE* addr, DT_SLONG len, DT_ID_SWORD extract_id, DT_SLONG values_in_out[]);

/* Text reflow function */
DT_ID_SLONG DTYPE_APICALL pdTextReflow(DT_PDDOC power_doc, DT_ID_SWORD fn_draw, DT_SWORD reserved, DT_ID_SLONG obj, DT_SLONG start_position[], DT_SLONG stats_in[]);
/* Text reflow function - advanced version */
DT_ID_SLONG DTYPE_APICALL pdTextReflowPlus(DT_PDDOC power_doc, DT_ID_SWORD fn_draw, DT_SWORD reserved, DT_ID_SLONG obj, DT_SLONG start_position[], DT_SLONG* stats_in_out[]);

/* Apply 2x2 transformation matrix to a rectangle and return its bounding box */
DT_SWORD DTYPE_APICALL pdTransformBox(DT_SRAST_L* x, DT_SRAST_L* y, DT_SRAST_L* w, DT_SRAST_L* h, const DT_TM2X2 transform);
/* Apply 3X3 transformation matrix to a rectangle */
DT_SWORD DTYPE_APICALL pdTransformBoxPlus(DT_SRAST_L* x, DT_SRAST_L* y, DT_SRAST_L* w, DT_SRAST_L* h, const DT_TM3X3 transform);
/* Apply 3X3 transformation matrix to a rectangle - XL */
DT_SWORD DTYPE_APICALL pdTransformBoxPlusXL(DT_SRAST_XL* x, DT_SRAST_XL* y, DT_SRAST_XL* w, DT_SRAST_XL* h, const DT_TM3X3 transform);

/* Free memory allocated by the engine */
void DTYPE_APICALL pdFree(void* mem_addr);

/* Check system status */
DT_SWORD DTYPE_APICALL pdSysPST(DT_SLONG id, DT_SWORD flag, DT_UBYTE param1, DT_UBYTE param2);


/* ------------------------- D-Type Power Engine Extra ------------------------- */


/* Apply flags */
#define PD_APPLY_STANDARD 0
#define PD_APPLY_NODELETE 1
#define PD_APPLY_DEFVALUE 2

/* Object status flags for internal object manipulation routines */
#define PD_COPY_NOTIMPORTANT 0
#define PD_COPY_MARKED       1
#define PD_COPY_IMPORTANT    2
#define PD_COPY_IGNORED      3


/* Property and Link Operations */

/* Set property values as DT_SBYTE */
DT_SWORD DTYPE_APICALL pdPropAddAsSByte(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SBYTE value);
/* Set property values DT_UBYTE */
DT_SWORD DTYPE_APICALL pdPropAddAsUByte(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE value);
/* Set property values as DT_SWORD */
DT_SWORD DTYPE_APICALL pdPropAddAsSWord(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SWORD value);
/* Set property values as DT_UWORD */
DT_SWORD DTYPE_APICALL pdPropAddAsUWord(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_UWORD value);
/* Set property values as DT_STRIO */
DT_SWORD DTYPE_APICALL pdPropAddAsSTrio(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG value);
/* Set property values as DT_UTRIO */
DT_SWORD DTYPE_APICALL pdPropAddAsUTrio(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ULONG value);
/* Set property values as DT_SLONG */
DT_SWORD DTYPE_APICALL pdPropAddAsSLong(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG value);
/* Set property values as DT_ULONG */
DT_SWORD DTYPE_APICALL pdPropAddAsULong(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ULONG value);

/* Get property values as DT_SBYTE */
DT_SBYTE DTYPE_APICALL pdPropGetAsSByte(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SBYTE default_value);
/* Get property values as DT_UBYTE */
DT_UBYTE DTYPE_APICALL pdPropGetAsUByte(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE default_value);
/* Get property values as DT_SWORD */
DT_SWORD DTYPE_APICALL pdPropGetAsSWord(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SWORD default_value);
/* Get property values as DT_UWORD */
DT_UWORD DTYPE_APICALL pdPropGetAsUWord(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_UWORD default_value);
/* Get property values as DT_STRIO */
DT_SLONG DTYPE_APICALL pdPropGetAsSTrio(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG default_value);
/* Get property values as DT_UTRIO */
DT_ULONG DTYPE_APICALL pdPropGetAsUTrio(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ULONG default_value);
/* Get property values as DT_SLONG */
DT_SLONG DTYPE_APICALL pdPropGetAsSLong(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG default_value);
/* Get property values as DT_ULONG */
DT_ULONG DTYPE_APICALL pdPropGetAsULong(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ULONG default_value);

/* Delete property */
DT_SWORD DTYPE_APICALL pdPropDelete(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE apply_flags);

/* Apply property */
DT_SWORD DTYPE_APICALL pdPropApply(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len, DT_UBYTE apply_flags);
/* Apply property As DT_SBYTE */
DT_SWORD DTYPE_APICALL pdPropApplyAsSByte(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SBYTE value, DT_SBYTE default_value, DT_UBYTE apply_flags);
/* Apply property As DT_UBYTE */
DT_SWORD DTYPE_APICALL pdPropApplyAsUByte(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE value, DT_UBYTE default_value, DT_UBYTE apply_flags);
/* Apply property As DT_SWORD */
DT_SWORD DTYPE_APICALL pdPropApplyAsSWord(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SWORD value, DT_SWORD default_value, DT_UBYTE apply_flags);
/* Apply property As DT_UWORD */
DT_SWORD DTYPE_APICALL pdPropApplyAsUWord(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_UWORD value, DT_UWORD default_value, DT_UBYTE apply_flags);
/* Apply property As DT_STRIO */
DT_SWORD DTYPE_APICALL pdPropApplyAsSTrio(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG value, DT_SLONG default_value, DT_UBYTE apply_flags);
/* Apply property As DT_UTRIO */
DT_SWORD DTYPE_APICALL pdPropApplyAsUTrio(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ULONG value, DT_ULONG default_value, DT_UBYTE apply_flags);
/* Apply property As DT_SLONG */
DT_SWORD DTYPE_APICALL pdPropApplyAsSLong(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_SLONG value, DT_SLONG default_value, DT_UBYTE apply_flags);
/* Apply property As DT_ULONG */
DT_SWORD DTYPE_APICALL pdPropApplyAsULong(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ULONG value, DT_ULONG default_value, DT_UBYTE apply_flags);
/* Apply property As Reference */
DT_SWORD DTYPE_APICALL pdPropApplyAsRef(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG ref_obj, DT_SLONG ref_name, DT_UBYTE apply_flags);

/* Applying link */
DT_SWORD DTYPE_APICALL pdLinkApply(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG link, DT_UBYTE apply_flags);

/* Get aggregate's object and rendering function */
DT_ID_SLONG DTYPE_APICALL pdImgGetObjAndFnDraw(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent, DT_SWORD* fn_draw);
/* Find object by name and, optionally, description */
DT_ID_SLONG DTYPE_APICALL pdObjFindByNameAndDesc(DT_PDDOC power_doc, DT_SLONG name, const DT_CHAR* desc);
/* Find object by name and, optionally, description - advanced */
DT_ID_SLONG DTYPE_APICALL pdObjFindByNameAndDescPlus(DT_PDDOC power_doc, DT_SLONG obj_start, DT_SLONG name, const DT_CHAR* desc, DT_SWORD reserved);
/* Copy PowerDoc objects marked as PD_COPY_MARKED with its dependencies to a new document */
DT_SLONG DTYPE_APICALL pdObjCopyMarked(DT_PDDOC power_doc1, DT_PDDOC power_doc2, DT_SLONG obj_code, DT_ID_SLONG temp_arr[]);
/* Copy a single PowerDoc object with its dependencies to a new document */
DT_SLONG DTYPE_APICALL pdObjCopySimple(DT_PDDOC power_doc1, DT_PDDOC power_doc2, DT_SLONG obj_code, DT_ID_SLONG obj_copy);
/* Copy selected PowerDoc objects to a new document */
DT_SLONG DTYPE_APICALL pdDocCopySelected(DT_PDDOC power_doc1, DT_PDDOC power_doc2, DT_SLONG ref_page, DT_SWORD copy_all);
/* Copy selected PowerDoc objects to a new document */
DT_SLONG DTYPE_APICALL pdDocCopySelectedPlus(DT_PDDOC power_doc1, DT_PDDOC power_doc2, DT_SLONG ref_page, DT_SWORD copy_all, DT_SWORD reserved, const DT_CHAR* important_desc_prefix);
/* Discard orphan objects in a PowerDoc document */
DT_SWORD DTYPE_APICALL pdDocDiscardOrphans(DT_PDDOC power_doc);
/* Discard orphan objects in a PowerDoc document */
DT_SWORD DTYPE_APICALL pdDocDiscardOrphansPlus(DT_PDDOC power_doc, DT_SWORD reserved, const DT_CHAR* important_desc_prefix);

/* Free text flows */
void DTYPE_APICALL pdFlowsFree(DT_PD_FLOWS* flows, DT_ID_SWORD frames_format, DT_SLONG img);
/* Dump text flows (debug function) */
void DTYPE_APICALL pdFlowsDump(const DT_PD_FLOWS* flows);

/* Calculate and return frames extent for a PowerDoc object obj when rendered by rendering function fn_draw */
DT_SLONG DTYPE_APICALL pdFramesExtent(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SWORD fn_draw, DT_SWORD flags, DT_SLONG page, DT_RECT_L* extent);
/* Calculate and return frames extent for a PowerDoc object obj when rendered by rendering function fn_draw - XL */
DT_SLONG DTYPE_APICALL pdFramesExtentXL(DT_PDDOC power_doc, DT_ID_SLONG obj, DT_SWORD fn_draw, DT_SWORD flags, DT_SLONG page, DT_RECT_XL* extent);
/* Nest refresh function */
DT_SWORD DTYPE_APICALL pdNestRefresh(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent, DT_ID_SWORD command, DT_SWORD reserved, DT_PD_NEST_CONFIG* config);


#ifdef __cplusplus
}
#endif


#endif /* DTYPE_DTYPEPWR_H */
