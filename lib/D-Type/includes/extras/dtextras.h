/*
 * dtextras.h - auxiliary header file for D-Type Standard Engine
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#ifndef DTYPE_DTEXTRAS_H
#define DTYPE_DTEXTRAS_H

#define DTYPE_DTEXTRAS_VER_STRING "7.x"

#define DV_MAXLINELENGTH 1024
#define DV_ENDOFGLYPHARR 0xFFFFFFFF


/* Defines - Text Round and Kern Modes */
#define DV_SPACING_ROUND_ADD 0
#define DV_SPACING_KERN_ROUND_ADD 1
#define DV_SPACING_ADD_ROUND 2
#define DV_SPACING_KERN_ADD_ROUND 3
#define DV_SPACING_FRAC 4
#define DV_SPACING_KERN_FRAC 5
#define DV_SPACING_DEVICE 6
#define DV_SPACING_KERN_DEVICE 7
#define DV_SPACING_DEVICE_2 8
#define DV_SPACING_KERN_DEVICE_2 9

#define DV_SPACING_HORIZONTAL 0
#define DV_SPACING_VERTICAL 128


/* Defines - Bitmap Crop Modes */
#define DV_CROP_HMAX_VMAX_PADDED 0
#define DV_CROP_HMAX_VMAX 1
#define DV_CROP_HMIN_VMAX 2
#define DV_CROP_HMAX_VMIN 3
#define DV_CROP_HMIN_VMIN 4


/* Get bounding box of a straight line of glyphs */
DT_FLOAT dtxGlyphsGetBound(DT_DTENGINE engine, DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[], const DT_SLONG glyph_arr_displacement[], const DT_SLONG glyph_arr_advance[], DT_FLOAT start_advance, DT_BMP* bmp);
/* Get bounding box of a straight line of characters */
DT_FLOAT dtxCharsGetBound(DT_DTENGINE engine, DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[], DT_BMP* bmp);
/* Get bounding box of a straight line of characters */
DT_FLOAT dtxCharsGetBound(DT_DTENGINE engine, DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_CHAR chars_arr[], DT_BMP* bmp);

/* Render a sequence of glyphs to Output as a straight line */
DT_FLOAT dtxGlyphsDoOutput(DT_DTENGINE engine, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[], const DT_SLONG glyph_arr_displacement[], const DT_SLONG glyph_arr_advance[], DT_FLOAT start_advance);
/* Render a sequence of characters to Output as a straight line */
DT_FLOAT dtxCharsDoOutput(DT_DTENGINE engine, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[]);
/* Render a sequence of characters to Output as a straight line */
DT_FLOAT dtxCharsDoOutput(DT_DTENGINE engine, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_CHAR chars_arr[]);

/* Render a sequence of glyphs to Output as a circular arc */
void dtxGlyphsDoOutput_Arc(DT_DTENGINE engine, DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[]);
/* Render a sequence of characters to Output as a circular arc */
void dtxCharsDoOutput_Arc(DT_DTENGINE engine, DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[]);
/* Render a sequence of characters to Output as a circular arc */
void dtxCharsDoOutput_Arc(DT_DTENGINE engine, DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_CHAR chars_arr[]);


/* Apply 2D perspective transformation to an array of points */
void dtxTransformPerspective(const DT_TM3X3 m, DT_SLONG x, DT_SLONG y, DT_SLONG n, const DT_SLONG x_arr_in[], const DT_SLONG y_arr_in[], DT_FLOAT x_arr_out[], DT_FLOAT y_arr_out[]);


#endif /* DTYPE_DTEXTRAS_H */
