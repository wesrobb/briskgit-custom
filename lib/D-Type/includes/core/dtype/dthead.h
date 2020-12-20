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


#ifndef DTYPE_DTHEAD_H
#define DTYPE_DTHEAD_H


/* ------------------------- D-Type Common Header ------------------------- */

#define DTYPE_ENGINE_VER_STRING "5.x"


#ifdef DEF_STDLIB
	#include <stdlib.h>
#endif


/* D-Type's raster and fractional types */
typedef DT_SLONG DT_SRAST_L;
typedef DT_SLONG DT_SFRAC_L;
typedef DT_SHUGE DT_SRAST_XL;
typedef DT_SHUGE DT_SFRAC_XL;
/* Aliases */
typedef DT_SRAST_L DT_SRAST;
typedef DT_SFRAC_L DT_SFRAC;

/* D-Type's matrices */
typedef DT_FLOAT DT_TM2X2 [2][2];
typedef DT_FLOAT DT_TM3X3 [3][3];

/* D-Type's char type alias */
typedef char DT_CHAR;

/* D-Type's standard file pointer alias */

#ifdef DEF_STDIO_FILE
	#include <stdio.h> /* for FILE* */
	typedef FILE* DT_FILE_PTR;
#else
	typedef struct DT_FILE_STRUCT* DT_FILE_PTR;
#endif


/* Basic macros */

#ifdef DEF_MACROS_FLOAT_BASIC
	#include <math.h> /* for floor */
#ifdef DEF_FLOAT_IEEE754
	#define DF_ROUND(a) DF_F2I_SLONG(floor((a) + 0.5))
	#define DF_FLOOR(a) DF_F2I_SLONG(floor(a))
#else
	#define DF_ROUND(a) ((DT_SLONG)floor((a) + 0.5))
	#define DF_FLOOR(a) ((DT_SLONG)floor(a))
#endif
#endif

#ifdef DEF_MACROS_FLOAT_OTHER
	#define DV_FLOAT_EPSILON 0.0000001 /* tolerance */
	#define DV_FLOAT_PI_OVER_180 0.01745329251994 /* pi divided by 180 */
	#define DV_FLOAT_180_OVER_PI 57.29577951309314 /* 180 divided by pi */
	#define DF_RAD(a) ((a) * DV_FLOAT_PI_OVER_180)
	#define DF_DEG(a) ((DT_SWORD)((DF_ROUND((a) * DV_FLOAT_180_OVER_PI)) % 360))
	#define DF_ISEQUAL(a, b) (((b) - DV_FLOAT_EPSILON < (a)) && ((b) + DV_FLOAT_EPSILON) > (a))
#endif

#ifdef DEF_MACROS_CONVENIENCE
	#define DF_MIN(a, b) (((a) < (b)) ? (a) : (b))
	#define DF_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif


/* Structures */


/* Point */
typedef struct
{
	DT_FLOAT x, y;

} DT_POINT_FLOAT;


/* D-Type Rectangle (coordinates expressed as 16-bit signed integers) */
typedef struct
{
	DT_SWORD xmn, ymn, xmx, ymx;

} DT_RECT_SWORD;


/* D-Type Rectangle (coordinates expressed as 32-bit signed integers) */
typedef struct
{
	DT_SLONG xmn, ymn, xmx, ymx;

} DT_RECT_SLONG;

/* Alias */
typedef DT_RECT_SLONG DT_RECT_L;


/* D-Type Rectangle (coordinates expressed as 64-bit signed integers) */
typedef struct
{
	DT_SHUGE xmn, ymn, xmx, ymx;

} DT_RECT_SHUGE;

/* Alias */
typedef DT_RECT_SHUGE DT_RECT_XL;


/* D-Type's 8-bit grayscale memory bitmap */
typedef struct
{
	DT_SRAST_L x, y; /* Bitmap's origin point in pixels */
	DT_SRAST_L w, h; /* Bitmap's dimensions in pixels */
	DT_UBYTE* m;     /* Pointer to bitmap's memory buffer */
	DT_SLONG l;      /* Length of the buffer in bytes */

} DT_BMP;


/* D-Type's memory (off-screen) surface */
typedef struct
{
	DT_SRAST_L w, h; /* Surface's dimensions in pixels */
	DT_UBYTE* m;     /* Pointer to surface's memory buffer */
	DT_SLONG l;      /* Length of the buffer in bytes (defines pitch) */

} DT_MDC;


/* D-Type's glyph/char origins and advances */
typedef struct
{
	DT_SWORD x, y; /* horizontal and vertical origin in font units */
	DT_UWORD w, h; /* advance width and height in font units */

} DT_ADVANCE;


/* D-Type Line Decoration (SRAST_L) */
typedef struct
{
	DT_SRAST_L thickness; /* 0=normal, >0=outline, <0=expansion */
	DT_SRAST_L segment;   /* Segment len - used for dashing when thickness<>0 */
	DT_SRAST_L shift;     /* Must be set to 0 */
	DT_UWORD dash_size;   /* Dash size as a percentage of the segment len (0=default, 65535=100%) */
	DT_UBYTE flags;       /* Must be set to 0 */
	DT_ID_UBYTE scale_id; /* Multiplier ID for thickness, segment and shift (0=multiply by 256, 1=multiply by 100) */

} DT_LINEDECOR_L;

/* Alias */
typedef DT_LINEDECOR_L DT_LINEDECOR;


/* D-Type Line Decoration (SRAST_XL) */
typedef struct
{
	DT_SRAST_XL thickness; /* 0=normal, >0=outline, <0=expansion */ /* DT_PIX256 */
	DT_SRAST_XL segment;   /* Segment len - used for dashing when thickness<>0 */ /* DT_PIX256 */
	DT_SRAST_XL shift;     /* Must be set to 0 */ /* DT_PIX256 */
	DT_UWORD dash_size;    /* Dash size as a percentage of the segment len (0=default, 65535=100%) */
	DT_UBYTE flags;        /* Must be set to 0 */
	DT_ID_UBYTE scale_id;  /* Multiplier ID for thickness, segment and shift (0=multiply by 256, 1=multiply by 100) */

} DT_LINEDECOR_XL;


/* Basic return values */
typedef DT_ID_SWORD DT_RET;
#define DV_RET_NOTHING -2
#define DV_RET_WARN -1
#define DV_RET_ERR 0
#define DV_RET_OK 1

/* Defines - D-Type Scale IDs */
#define DV_SCALE_256 0
#define DV_SCALE_100 1
#define DV_SCALE_1   2


/* Macros to read data from memory in Intel x86 little-endian format */

inline DT_UBYTE DF_READ_UBYTE_LE(const DT_UBYTE* b)
{
	return (DT_UBYTE)(b[0]);
}

inline DT_UWORD DF_READ_UWORD_LE(const DT_UBYTE* b)
{
	return (DT_UWORD)((b[1] << 8) | b[0]);
}

inline DT_ULONG DF_READ_ULONG_LE(const DT_UBYTE* b)
{
	return (DT_ULONG)((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0]);
}

inline DT_SBYTE DF_READ_SBYTE_LE(const DT_UBYTE* b) { return (DT_SBYTE)DF_READ_UBYTE_LE(b); }
inline DT_SWORD DF_READ_SWORD_LE(const DT_UBYTE* b) { return (DT_SWORD)DF_READ_UWORD_LE(b); }
inline DT_SLONG DF_READ_SLONG_LE(const DT_UBYTE* b) { return (DT_SLONG)DF_READ_ULONG_LE(b); }

inline DT_ULONG DF_READ_UTRIO_LE(const DT_UBYTE* b)
{
	return (DT_ULONG)((b[2] << 16) | (b[1] << 8) | b[0]);
}

inline DT_SLONG DF_READ_STRIO_LE(const DT_UBYTE* b)
{
	return (DT_SLONG)(((b[2] & 128) ? 0xFF000000 : 0) | (b[2] << 16) | (b[1] << 8) | b[0]);
}

inline DT_FLOAT DF_READ_FLOAT_LE(const DT_UBYTE* b) /* new in 8.0.1.2 */
{
	//DT_FLOAT n; dtMemCopy(&n, b, sizeof(DT_FLOAT)); return n;

	DT_BIT64 p = DF_READ_ULONG_LE(b), q = DF_READ_ULONG_LE(b + 4);
	union { DT_FLOAT f; DT_BIT64 i; } u; u.i = p | (q << 32);
	return u.f;
}


/* Macros to write data to memory in Intel x86 little-endian format */

inline void DF_WRITE_UBYTE_LE(DT_UBYTE* b, DT_UBYTE n)
{
	b[0] = (DT_UBYTE)n;
}

inline void DF_WRITE_UWORD_LE(DT_UBYTE* b, DT_UWORD n)
{
	b[0] = (DT_UBYTE)(n & 255);
	b[1] = (DT_UBYTE)(n >> 8);
}

inline void DF_WRITE_UTRIO_LE(DT_UBYTE* b, DT_ULONG n)
{
	b[0] = (DT_UBYTE)(n & 255);
	b[1] = (DT_UBYTE)((n >> 8) & 255);
	b[2] = (DT_UBYTE)((n >> 16) & 255);
}

inline void DF_WRITE_ULONG_LE(DT_UBYTE* b, DT_ULONG n)
{
	b[0] = (DT_UBYTE)(n & 255);
	b[1] = (DT_UBYTE)((n >> 8) & 255);
	b[2] = (DT_UBYTE)((n >> 16) & 255);
	b[3] = (DT_UBYTE)(n >> 24);
}

inline void DF_WRITE_SBYTE_LE(DT_UBYTE* b, DT_SBYTE n) { DF_WRITE_UBYTE_LE(b, (DT_UBYTE)n); }
inline void DF_WRITE_SWORD_LE(DT_UBYTE* b, DT_SWORD n) { DF_WRITE_UWORD_LE(b, (DT_UWORD)n); }
inline void DF_WRITE_STRIO_LE(DT_UBYTE* b, DT_SLONG n) { DF_WRITE_UTRIO_LE(b, (DT_ULONG)n); }
inline void DF_WRITE_SLONG_LE(DT_UBYTE* b, DT_SLONG n) { DF_WRITE_ULONG_LE(b, (DT_ULONG)n); }
inline void DF_WRITE_FLOAT_LE(DT_UBYTE* b, DT_FLOAT n) /* new in 8.0.1.2 */
{
	//dtMemCopy(b, &n, sizeof(DT_FLOAT)); return;

	union { DT_FLOAT f; DT_BIT64 i; } u; u.f = n;
	DF_WRITE_ULONG_LE(b, (DT_ULONG)(u.i & 4294967295)); DF_WRITE_ULONG_LE(b + 4, (DT_ULONG)(u.i >> 32));
}


/* Other macros */

inline DT_SLONG DF_CALC_PITCH(DT_SLONG bpp, const DT_MDC* mdc)
{
	DT_SLONG abs_mdc_w = (mdc->w < 0 ? -mdc->w : mdc->w);
	DT_SLONG abs_mdc_h = (mdc->h < 0 ? -mdc->h : mdc->h);
	DT_SLONG mdc_pitch = bpp * abs_mdc_w;

	if (abs_mdc_h != 0 && mdc->l > mdc_pitch * abs_mdc_h)
	{
		DT_SLONG pitch = mdc->l / abs_mdc_h;
		if (mdc->l == pitch * abs_mdc_h) return pitch;
	}

	return mdc_pitch;
}


#ifdef __cplusplus
	#define DV_NULL nullptr
#else
	#define DV_NULL NULL
#endif


#endif /* DTYPE_DTHEAD_H */
