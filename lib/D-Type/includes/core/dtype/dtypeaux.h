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


#ifndef DTYPE_DTYPEAUX_H
#define DTYPE_DTYPEAUX_H


#ifdef __cplusplus
DT_EXTERN {
#endif


/* ------------------------- D-Type External Format Plugin ------------------------- */

#define DV_AUX_FILEFORMAT_JPG  1
#define DV_AUX_FILEFORMAT_PNG  2
#define DV_AUX_FILEFORMAT_APNG 2
#define DV_AUX_FILEFORMAT_TIFF 3
#define DV_AUX_FILEFORMAT_GIF  100
#define DV_AUX_FILEFORMAT_ZLIB 200


/* Initialization information */
typedef struct
{
	DT_SLONG InitKey;    /* Initialization key (0=demo) */
	DT_SWORD InitFlag;   /* Initialization flag (0=demo) */
	DT_SWORD Reserved1;  /* Must be set to 0 */
	DT_SWORD Reserved2;  /* Must be set to 0 */
	DT_UWORD FileFormat; /* File format value (DV_AUX_FILEFORMAT_...) */
	void* Internal;      /* Internal use - must be set to DV_NULL */

} DT_AUX_INFO;


/* Attributes for raster graphics - (e.g. color gradient for grayscale images, compression quality, transparency treatment etc.) */
typedef struct
{
	DT_UBYTE R1, G1, B1;       /* RGB color of the background */
	DT_UBYTE R2, G2, B2;       /* RGB color of the foreground */
	DT_UBYTE PaletteFlag;      /* Set to 0 for RGB, 1 for palette (PNG 8-bpp mode only) */
	DT_UBYTE TransparencyFlag; /* 0=no transparent background, 1=basic transparency, 2=reserved, 3=alpha transparency */
	DT_UBYTE AlphaFlag;        /* Set to 1 to respect AlphaValue, otherwise set to 0 */
	DT_UBYTE AlphaValue;       /* Supplies alpha or gray value when AlphaFlag is nonzero */
	DT_SLONG AnimDelay;        /* Animation delay in 1/100 seconds */
	DT_SLONG AnimLoops;        /* Number of animation loops (0=play indefinitely) */
	DT_SWORD Quality;          /* Graphics quality (0-100 for JPEG; for GIF quality over 75 activates dithering) */

} DT_AUX_GRAPHICS_ATTRIBS;


/* User defined stream */
typedef struct
{
	void* UserStruct;     /* User supplied data (will be passed to UserFuncR and UserFuncW) */
	DT_SLONG (*UserFuncR)(void* user_struct, DT_UBYTE* buffer, DT_SLONG length);       /* user function to read data from stream */
	DT_SLONG (*UserFuncW)(void* user_struct, const DT_UBYTE* buffer, DT_SLONG length); /* user function to write data to stream */
	DT_FILE_PTR FilePtr;  /* For use with standard file-pointer streams */

} DT_AUX_USER_STREAM;


/* Get maximum size of buffer to store compressed data */
DT_SLONG DTYPE_APICALL fxBufferGetBound(DT_AUX_INFO* info, DT_SLONG raw_len);
/* Compress data in buffer */
DT_SWORD DTYPE_APICALL fxBufferCompress(DT_AUX_INFO* info, DT_UBYTE* compr_addr, DT_SLONG* compr_len, const DT_UBYTE* raw_addr, DT_SLONG raw_len, DT_ID_SWORD level);
/* Decompress data in buffer */
DT_SWORD DTYPE_APICALL fxBufferDecompress(DT_AUX_INFO* info, DT_UBYTE* raw_addr, DT_SLONG* raw_len, const DT_UBYTE* compr_addr, DT_SLONG compr_len);


/* Read bitmap image from user stream */
DT_SLONG DTYPE_APICALL fxImageRead(DT_AUX_INFO* info, DT_UBYTE** bitmap_ptr, DT_ID_SWORD* bitmap_format, DT_ID_SWORD* bitmap_subformat, DT_SLONG* width, DT_SLONG* height, DT_UWORD flags, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_AUX_USER_STREAM* user_stream);
/* Write bitmap image to user stream */
DT_SLONG DTYPE_APICALL fxImageWrite(DT_AUX_INFO* info, const DT_UBYTE* bitmap_ptr, const DT_UBYTE* alpha_ptr, DT_SLONG bitmap_padding, DT_SLONG alpha_padding, DT_ID_SWORD bitmap_format, DT_SWORD bitmap_subformat, DT_SLONG width, DT_SLONG height, DT_UWORD flags, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_AUX_USER_STREAM* user_stream);


/* Open new animated image */
DT_SWORD DTYPE_APICALL fxAnimStart(DT_AUX_INFO* info, DT_ID_SWORD bitmap_format, DT_ID_SWORD bitmap_subformat, DT_SLONG width, DT_SLONG height, DT_UWORD flags, DT_SLONG frames_count, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_AUX_USER_STREAM* user_stream);
/* Write animation frame */
DT_SLONG DTYPE_APICALL fxAnimWrite(DT_AUX_INFO* info, const DT_UBYTE* bitmap_ptr, const DT_UBYTE* alpha_ptr, DT_SLONG bitmap_padding, DT_SLONG alpha_padding, DT_SLONG cx, DT_SLONG cy, DT_SLONG cw, DT_SLONG ch, DT_SLONG quality, DT_UWORD flags, DT_SLONG delay);
/* Close animated image */
void DTYPE_APICALL fxAnimEnd(DT_AUX_INFO* info);


#ifdef __cplusplus
}
#endif


#endif /* DTYPE_DTYPEAUX_H */
