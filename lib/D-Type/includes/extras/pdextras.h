/*
 * pdextras.h - auxiliary header file for D-Type Power Engine
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#ifndef DTYPE_PDEXTRAS_H
#define DTYPE_PDEXTRAS_H


#define DTYPE_PDEXTRAS_VER_STRING "5.x"

#define PDX_MAXTEXTLENGTH TX_MAXTEXTLENGTH

/* #define DEF_PDX_TXDRAW_HACK */

#ifdef DEF_PDX_TXDRAW_HACK
#define DT_DOC DT_TXDOC
#else
#define DT_DOC DT_PDDOC
#endif


inline DT_ID_SLONG DF_PDX_READ_OBJ(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent)
{
	return (obj_parent < 0 ? pdImgGetObj(power_doc, img) : pdLinkGet(power_doc, obj_parent, pdFnDrawObjectPtr));
}

inline DT_SWORD DF_PDX_READ_FNDRAW(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent)
{
	return (obj_parent < 0 ? pdImgGetFnDraw(power_doc, img) : pdPropGetAsSWord(power_doc, obj_parent, pdFnDraw, pdDrawDefault));
}

inline DT_UBYTE DF_PDX_READ_ATTRIBS(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent)
{
	return (obj_parent < 0 ? pdImgGetAttribs(power_doc, img) : pdPropGetAsUByte(power_doc, obj_parent, pdAttribs, 0));
}

/*
inline void DF_PDX_WRITE_FNDRAW(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent, DT_SWORD fn_draw)
{
	if (obj_parent < 0) pdImgSetFnDraw(power_doc, img, fn_draw);
	else pdxPropApplyAsSWord(power_doc, obj_parent, pdFnDraw, fn_draw, pdDrawDefault, PDX_APPLY_STANDARD);
}
*/

inline void DF_PDX_WRITE_ATTRIBS(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent, DT_UBYTE attribs)
{
	if (obj_parent < 0) pdImgSetAttribs(power_doc, img, attribs);
	else pdPropApplyAsUByte(power_doc, obj_parent, pdAttribs, attribs, 0, PD_APPLY_STANDARD);
}


/* Structures */

/* Point (coordinates expressed as 32-bit signed integers) */
typedef struct
{
	DT_SLONG X, Y;

} DT_PDX_XY_SLONG;


/* Size (coordinates expressed as 32-bit signed integers) */
typedef struct
{
	DT_SLONG W, H;

} DT_PDX_WH_SLONG;


/* Rectangle (coordinates expressed as 32-bit signed integers) */
typedef struct
{
	DT_SLONG X, Y, W, H;

} DT_PDX_XYWH_SLONG;


/* A structure to hold PowerDoc export configuration */
typedef struct
{
	const DT_CHAR* PrologOpen;
	const DT_CHAR* PrologClose;
	const DT_CHAR* Header1;                  /* First Header Line */
	const DT_CHAR* Header2;                  /* Second Header Line */
	const DT_CHAR* Header3;                  /* Third Header Line */
	const DT_CHAR* NewLine;                  /* Newline character sequence */
	const DT_CHAR* Tab;                      /* Tab character sequence */
	const DT_CHAR* CommentOpen;              /* Sequence to open a comment */
	const DT_CHAR* CommentClose;             /* Sequence to close a comment */
	const DT_CHAR* CmdEnd;                   /* Sequence to end a statement */
	const DT_CHAR* MakeHeader;               /* Sequence to open the MyPowerDocMake function */
	const DT_CHAR* MakeFooter;               /* Sequence to close the MyPowerDocMake function */
	const DT_CHAR* ArrObjectDeclaration1;    /* Sequence to declare the PowerDoc Object array */
	const DT_CHAR* ArrObjectDeclaration2;
	const DT_CHAR* ArrObjectDeclaration3;
	DT_UBYTE TransformViaMatrix;             /* Inidicates wheter to use a matrix or factors for transformed objects */
	const DT_CHAR* ArrTransformDeclaration1; /* Sequence to declare the PowerDoc Transform array */
	const DT_CHAR* ArrTransformDeclaration2;
	const DT_CHAR* ArrBracketOpen;           /* Sequence to open an array */
	const DT_CHAR* ArrBracketClose;          /* Sequence to close an array */
	const DT_CHAR* ArrDimSeparator;          /* Sequence between two array dimensions */
	DT_SWORD ArrDimAdd;                      /* A value to add to the array size */
	const DT_CHAR* NullValue;                /* Sequence for null value */
	const DT_CHAR* ObjAddOpen;               /* Sequence to open the pdObjAdd function */
	const DT_CHAR* ObjAddClose;              /* Sequence to close the pdObjAdd function */
	const DT_CHAR* ImgAddOpen;               /* Sequence to open the pdImgAddPlus function */
	const DT_CHAR* ImgAddClose;              /* Sequence to close the pdImgAddPlus function */
	const DT_CHAR* LinkAddOpen;              /* Sequence to open the pdLinkAdd function */
	const DT_CHAR* LinkAddClose;             /* Sequence to close the pdLinkAdd function */
	const DT_CHAR* PropAddOpen;              /* Sequence to open the pdPropAdd function */
	const DT_CHAR* PropAddCast;
	const DT_CHAR* PropAddClose;             /* Sequence to close the pdPropAdd function */
	const DT_CHAR* PropAddAsRefOpen;         /* Sequence to open the pdPropAdd function */
	const DT_CHAR* PropAddAsRefClose;        /* Sequence to close the pdPropAdd function */
	const DT_CHAR* Footer1;                  /* First Footer Line */
	const DT_CHAR* Footer2;                  /* Second Footer Line */
	const DT_CHAR* Footer3;                  /* Third Footer Line */
	DT_UBYTE OmitOptional;                   /* Indicates wheter optional values can be omitted */
	DT_UBYTE OmitTypes;                      /* Indicates wheter data types can be omitted */
	DT_UBYTE NonAsciiChrFormatID;            /* Inidactes wheter to save non-Ascii characters as octal values */
	const DT_CHAR* NonAsciiChrOpen;          /* Sequence before a non-Ascii character */
	const DT_CHAR* NonAsciiChrClose;         /* Sequence after a non-Ascii character */

} DT_PDX_EXPORT_CONFIG;


/* Bitmap */
typedef struct
{
	DT_MDC MDC;
	DT_ID_SWORD Format, Subformat;
	DT_UBYTE LCD;

} DT_PDX_BITMAP;


/* Operations for raster graphics */
typedef struct
{
	DT_UBYTE SwapRGB;
	DT_UBYTE Dither;
	DT_UBYTE GrayToBW;
	DT_UBYTE BWTreshold;

} DT_PDX_GRAPHICS_OPERATIONS;


/* An internal structure to help write PNG and JPEG files to a memory buffer */
typedef struct
{
	DT_SWORD Error;   /* Error flag */
	DT_SLONG CurrPos; /* Current position in the buffer */
	DT_UBYTE* Buffer; /* Points to a buffer that holds the PNG or JPEG bytes */

} DT_PDX_STREAM_BUFFER;


/* Check if file type is known */
DT_ID_SWORD pdxIsKnownFileType(const DT_UBYTE* m, DT_SLONG l, const DT_CHAR* mime_default, DT_CHAR* mime_type, DT_SLONG* offset, DT_ID_SWORD* compress_method);
/* Write string to a stream */
DT_SWORD pdxStringWriteToStream(const DT_CHAR* str, DT_DTSTREAM stream);
/* Remove control codes from a string */
DT_CHAR* pdxStringZap(DT_CHAR* str);
/* Read string from a file in which the pipe character serves as a field delimiter */
DT_SWORD pdxStringInput(DT_CHAR* str, DT_DTSTREAM stream, DT_SLONG max_len);
/* Read a numerical value from a file in which the pipe character serves as a field delimiter */
DT_SWORD pdxIntegerInput(DT_SLONG* number, DT_DTSTREAM stream);
/* Retrieve a string based on its numerical value from a file in which the pipe character serves as a field delimiter */
DT_SWORD pdxStringEnumGet(DT_CHAR* str, const DT_CHAR* file_name, DT_SLONG index);

/* Get description for the specified PowerDoc field */
DT_CHAR* pdxInfoTextAlloc(DT_SLONG name, DT_ID_UBYTE show_details, DT_ID_UBYTE as_ref);
/* Free dynamically allocated description */
void pdxInfoTextFree(DT_CHAR* str);

/* Replace chr(13)+chr(10) and chr(10)+chr(13) with chr(13) */
DT_SLONG pdxTextFixNewline(DT_UBYTE* buffer, DT_SLONG buffer_len, DT_ID_SWORD sz);

/* Multiply two 3X3 matrices */
void pdxMatrixMultiplyPlus(const DT_TM3X3 m, const DT_TM3X3 a, DT_TM3X3 r);
/* Multiply two 2x2 matrices */
void pdxMatrixMultiply(const DT_TM2X2 m, const DT_TM2X2 a, DT_TM2X2 r);
/* Get 3X3 transformation matrix from a library of transformations */
DT_SWORD pdxMatrixGetTransformPlus(DT_SLONG matrix_id, DT_TM3X3 transform);
/* Get 2x2 transformation matrix from a library of transformations */
DT_SWORD pdxMatrixGetTransform(DT_SLONG matrix_id, DT_TM2X2 transform);

/* Delete a PowerDoc object synchronizing all links and PowerDoc images in the document */
void pdxObjDelete(DT_PDDOC power_doc, DT_ID_SLONG obj_del);
/* Compare two PowerDoc objects */
DT_SWORD pdxObjCompare(DT_PDDOC power_doc, DT_ID_SLONG obj1, DT_ID_SLONG obj2);
/* Export a PowerDoc object */
DT_SLONG pdxObjExport(DT_PDDOC power_doc, DT_DTSTREAM stream, DT_ID_SLONG obj, DT_UBYTE use_name_enums, const DT_PDX_EXPORT_CONFIG* conf);

/* Get zoomed origin point for a specific PowerDoc image */
void pdxImgGetXYZoomed(DT_PDDOC power_doc, DT_SLONG img, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SLONG* x_zoom, DT_SLONG* y_zoom);
/* Find PowerDoc image whose origin point is closest to (x, y) */
DT_SLONG pdxImgFindClosest(DT_PDDOC power_doc, DT_SLONG hit_x, DT_SLONG hit_y, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y);

/* Select a single PowerDoc image closest to the selection point on the specified page */
DT_SLONG pdxImgSelSelectByPoint(DT_PDDOC power_doc, DT_SLONG x, DT_SLONG y, DT_SLONG page, DT_FLOAT zoom);
/* Select all PowerDoc images within the selection rectangle on the specified page */
DT_SLONG pdxImgSelSelectByRect(DT_PDDOC power_doc, DT_SLONG x1, DT_SLONG y1, DT_SLONG x2, DT_SLONG y2, DT_SLONG page, DT_FLOAT zoom);
/* Select a range of PowerDoc images */
DT_SLONG pdxImgSelSelectByImgRange(DT_PDDOC power_doc, DT_SLONG img_min, DT_SLONG img_max);
/* Select all PowerDoc images on the specified pages */
DT_SLONG pdxImgSelSelectByPageRange(DT_PDDOC power_doc, DT_SLONG page_from, DT_SLONG page_to);

/* Return the number of selected PowerDoc images */
DT_SLONG pdxImgSelCount(DT_PDDOC power_doc);

/* Move selected PowerDoc images */
DT_SLONG pdxImgSelMove(DT_PDDOC power_doc, DT_SLONG dx, DT_SLONG dy);
/* Remove a transformation matrix from selected images */
DT_SLONG pdxImgSelResetTransform(DT_PDDOC power_doc);
/* Delete selected PowerDoc images */
DT_SLONG pdxImgSelDelete(DT_PDDOC power_doc);
/* Move selected PowerDoc images to the specified page */
DT_SLONG pdxImgSelMoveToPage(DT_PDDOC power_doc, DT_SLONG page);
/* Hide selected PowerDoc images */
DT_SLONG pdxImgSelHide(DT_PDDOC power_doc);
/* Unhide selected PowerDoc images */
DT_SLONG pdxImgSelUnhide(DT_PDDOC power_doc);
/* Group selected PowerDoc images */
DT_SLONG pdxImgSelGroup(DT_PDDOC power_doc, DT_SLONG page_group, DT_SLONG x_group, DT_SLONG y_group, DT_SWORD bit_flags);
/* Ungroup selected PowerDoc images */
DT_SLONG pdxImgSelUngroup(DT_PDDOC power_doc);
/* Move group origins */
DT_SLONG pdxImgSelGroupOriginMove(DT_PDDOC power_doc, DT_SLONG dx, DT_SLONG dy);
/* Move selected PowerDoc images to the top */
DT_SLONG pdxImgSelToTop(DT_PDDOC power_doc);
/* Move selected PowerDoc images to the bottom */
DT_SLONG pdxImgSelToBottom(DT_PDDOC power_doc);
/* Duplicate selected PowerDoc images */
DT_SLONG pdxImgSelDuplicate(DT_PDDOC power_doc, DT_SLONG dst_page, DT_SLONG dx, DT_SLONG dy, DT_UBYTE src_status, DT_UBYTE dst_status);
/* Convert selected images to PowerFont */
DT_ID_SLONG pdxImgSelToPowerFont(DT_PDDOC power_doc, const DT_RECT_SLONG* extent, const DT_SLONG metrics_x_arr[], const DT_SLONG metrics_y_arr[], DT_SLONG arr_size, DT_SLONG* record_count);
/* Convert selected images to text */
DT_ID_SLONG pdxImgSelToText(DT_PDDOC power_doc, const DT_RECT_SLONG* extent, const DT_SLONG metrics_x_arr[], const DT_SLONG metrics_y_arr[], DT_SLONG arr_size, DT_ID_SWORD sz);
/* Select all non-selected PowerDoc images and deselect all selected PowerDoc images */
DT_SLONG pdxImgSelInvert(DT_PDDOC power_doc);
/* Apply a 3X3 transformation matrix to selected images */
DT_SLONG pdxImgSelTransformPlus(DT_PDDOC power_doc, const DT_TM3X3 transform);
/* Apply a 2x2 transformation matrix to selected images */
DT_SLONG pdxImgSelTransform(DT_PDDOC power_doc, const DT_TM2X2 transform);

/* Find image by name */
DT_SLONG pdxImgAllFindByName(DT_PDDOC power_doc, DT_SWORD name);
/* Delete all PowerDoc images on the specified page range */
DT_SLONG pdxImgAllDeleteOnPageRange(DT_PDDOC power_doc, DT_SLONG page_from, DT_SLONG page_to);
/* Delete all PowerDoc images on the specified page */
DT_SLONG pdxImgAllDeleteOnPage(DT_PDDOC power_doc, DT_SLONG page);
/* Return the number of PowerDoc images when the filter is applied */
DT_SLONG pdxImgAllCountByFilter(DT_PDDOC power_doc, DT_SLONG page, DT_UBYTE attribs, DT_SLONG fn_draw, DT_ID_SLONG obj);
/* Apply a 3X3 transform matrix to all images */
DT_SLONG pdxImgAllSetTransformPlus(DT_PDDOC power_doc, const DT_TM3X3 transform);
/* Apply a 2x2 transform matrix to all images */
DT_SLONG pdxImgAllSetTransform(DT_PDDOC power_doc, const DT_TM2X2 transform);
/* Deselect all PowerDoc images */
DT_SLONG pdxImgAllDeselect(DT_PDDOC power_doc);
/* Export all PowerDoc images */
DT_SLONG pdxImgAllExport(DT_PDDOC power_doc, DT_DTSTREAM stream, DT_UBYTE use_name_enums, const DT_PDX_EXPORT_CONFIG* conf);

/* Make bitmap image */
DT_SLONG pdxImgMakeBitmap(DT_PDDOC power_doc, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_SLONG prop_id, DT_SLONG bmp_w, DT_SLONG bmp_h, const DT_UBYTE* bmp_ptr, DT_SLONG bmp_data_size, DT_UBYTE misc_flags);
/* Make bitmap image from a Windows Bitmap file */
DT_SLONG pdxImgLoadWindowsBitmap(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_UBYTE misc_flags);
/* Make bitmap image from a JPEG file */
DT_SLONG pdxImgLoadPNG(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_UBYTE misc_flags);
/* Make bitmap image from a PNG file */
DT_SLONG pdxImgLoadJPG(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_UBYTE misc_flags);
/* Make vector shape from a Shape Definition Table */
DT_SLONG pdxImgLoadShapeDefTable(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h);
/* Make vector picture */
DT_SLONG pdxImgLoadVectorPicture(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_SWORD pcnr);
/* Make text area, import text from a file */
DT_SLONG pdxImgLoadSimpleText(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_ID_SWORD font_index, DT_SLONG font_width, DT_SLONG font_height, DT_UBYTE global_direction, DT_UBYTE text_area_type, DT_UBYTE text_area_valign, DT_UBYTE text_area_valign_end, DT_UBYTE text_layout_id, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h);
/* Make text flow */
DT_SLONG pdxImgMakeTextFlow(DT_PDDOC power_doc, DT_UBYTE global_direction, DT_UBYTE text_area_type, DT_UBYTE text_area_row_type, DT_UBYTE text_area_valign, DT_UBYTE text_area_valign_end, DT_UBYTE text_layout_id, DT_SLONG page, DT_SLONG nr_of_pages, DT_SLONG nr_of_columns, DT_SLONG col_dist, DT_SLONG col_x, DT_SLONG col_y, DT_SLONG col_w, DT_SLONG col_h, DT_ID_SLONG obj);
/* Convert image with plain text to rich text */
DT_ID_SLONG pdxImgTextToRichText(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent);

/* Move PowerDoc pages */
void pdxPagesMove(DT_PDDOC power_doc, DT_SLONG start_page, DT_SLONG page_offset);
/* Test if page is empty */
DT_SWORD pdxPageIsEmpty(DT_PDDOC power_doc, DT_SLONG page);
/* Utility function to make a Text Fragment */
DT_ID_SLONG pdxPageToText(DT_PDDOC power_doc, DT_SLONG page, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, const DT_SLONG metrics_x_array[], const DT_SLONG metrics_y_array[], DT_SLONG array_len);

/* Discard object descriptions */
DT_SLONG pdxDocDiscardDescriptions(DT_PDDOC power_doc, const DT_CHAR* important_desc_prefix);
/* Replace font indexes in a document with unique font identifiers */
DT_SLONG pdxDocReconnectFonts(DT_PDDOC power_doc);
/* Replace external objects in a document with internal objects */
DT_SLONG pdxDocReconnectExternal(DT_PDDOC power_doc);
/* Normalize links in a document */
DT_SLONG pdxDocNormalizeLinks(DT_PDDOC power_doc, DT_UBYTE multi_pass);

/* Open compressed Power Engine document from stream */
DT_SLONG pdxDocAppendFromStream(DT_PDDOC power_doc, DT_ID_SWORD flag, DT_SLONG page, DT_SLONG x, DT_SLONG y, const DT_STREAM_DESC* sd);
/* Open compressed Power Engine document from disk */
DT_SLONG pdxDocAppend(DT_PDDOC power_doc, DT_ID_SWORD flag, DT_SLONG page, DT_SLONG x, DT_SLONG y, const DT_CHAR* file_name);
/* Save compressed Power Engine document to stream */
DT_SLONG pdxDocSaveToStream(DT_PDDOC power_doc, DT_ID_SWORD flag, DT_STREAM_DESC* sd);
/* Save compressed Power Engine document to disk */
DT_SLONG pdxDocSave(DT_PDDOC power_doc, DT_ID_SWORD flag, const DT_CHAR* file_name);

/* Save specified page as an 8-bpp or 24-bpp raw bitmap file */
DT_SLONG pdxDocSaveAsRAW(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip);
/* Save specified page as Windows bitmap file */
DT_SLONG pdxDocSaveAsBMP(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip);
/* Save specified page as a PNG file */
DT_SLONG pdxDocSaveAsPNG(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_UBYTE close_crop, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip);
/* Save specified page as a JPEG file */
DT_SLONG pdxDocSaveAsJPG(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_UBYTE close_crop, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip);

/* Save specified page as an animated GIF file */
DT_SLONG pdxDocSaveAsAnimatedGIF(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, const DT_SLONG pages[], const DT_SLONG delays[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip);
/* Save specified page as an animated PNG file */
DT_SLONG pdxDocSaveAsAnimatedPNG(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, const DT_SLONG pages[], const DT_SLONG delays[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip);

/* Save specified page as a PDF file */
DT_SWORD pdxDocSaveAsPDF_Bitmap(const DT_CHAR* filename, DT_DOC doc, const DT_SLONG pages[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_PDX_GRAPHICS_OPERATIONS* ops, const DT_PDX_XYWH_SLONG* clip, DT_FLOAT scale, DT_UWORD var_flags);
/* Save specified page as a PDF file */
DT_SWORD pdxDocSaveAsPDF_Vector(const DT_CHAR* filename, DT_DOC doc, const DT_SLONG pages[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_PDX_GRAPHICS_OPERATIONS* ops, const DT_PDX_XYWH_SLONG* clip, DT_FLOAT scale, const DT_FLOAT render_factors[], DT_UWORD var_flags);
/* Save specified page as a PDF file */
DT_SWORD pdxDocSaveAsPDF_Wireframe(const DT_CHAR* filename, DT_DOC doc, const DT_SLONG pages[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_PDX_GRAPHICS_OPERATIONS* ops, const DT_PDX_XYWH_SLONG* clip, DT_FLOAT scale, const DT_FLOAT render_factors[], DT_UWORD var_flags);
/* Save specified page as a SVG file */
DT_SWORD pdxDocSaveAsSVG(const DT_CHAR* filename, DT_DOC doc, const DT_SLONG pages[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_PDX_GRAPHICS_OPERATIONS* ops, const DT_PDX_XYWH_SLONG* clip, const DT_FLOAT render_factors[], DT_UWORD var_flags);

/* Copy Power Engine document */
DT_PDDOC pdxDocMakeCopy(DT_PDDOC power_doc, DT_PDENGINE engine);

/* Low-level configurable document export function */
DT_SLONG pdxDocExport(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_UBYTE use_name_enums, const DT_PDX_EXPORT_CONFIG* conf, const DT_CHAR* body_str);
/* Save document as Text */
DT_SLONG pdxDocExportAsText(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as C/C++ source */
DT_SLONG pdxDocExportAsC(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as Java */
DT_SLONG pdxDocExportAsJava(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as INTEGRAL */
DT_SLONG pdxDocExportAsINTEGRAL(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as C++ */
DT_SLONG pdxDocExportAsCPP(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as JScript/JavaScript */
DT_SLONG pdxDocExportAsJScript(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as VB Script */
DT_SLONG pdxDocExportAsVBScript(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as VB */
DT_SLONG pdxDocExportAsVB(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as C# */
DT_SLONG pdxDocExportAsCS(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as JSP page using Java */
DT_SLONG pdxDocExportAsJSPInJava(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as ASP page using JScript/JavaScript */
DT_SLONG pdxDocExportAsASPInJScript(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as ASP page using VBScript */
DT_SLONG pdxDocExportAsASPInVBScript(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as ASP.NET page using JScript */
DT_SLONG pdxDocExportAsASPNETInJScript(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as ASP.NET page using VB */
DT_SLONG pdxDocExportAsASPNETInVB(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj);
/* Save document as ASP.NET page using C# */
DT_SLONG pdxDocExportAsASPNETInCS(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj);

/* Draw grid inside bitmap */
void pdxBitmapDrawGrid(const DT_PDX_BITMAP* bitmap, DT_SLONG x_off, DT_SLONG y_off, DT_SLONG dw, DT_SLONG dh, DT_FLOAT zoom);
/* Crop bitmap */
DT_SWORD pdxBitmapCrop(const DT_PDX_BITMAP* bitmap, DT_UBYTE crop_type, const DT_UBYTE rgbt[], DT_SLONG* cx, DT_SLONG* cy, DT_SLONG* cw, DT_SLONG* ch);
/* Bitmap graphic operations */
DT_SWORD pdxBitmapOperations(const DT_PDX_BITMAP* bitmap, const DT_PDX_GRAPHICS_OPERATIONS* ops, DT_UBYTE clr_byte, const DT_UBYTE* old_m, DT_SLONG old_w, DT_UBYTE* has_pixels);

/* Rendering parameters */
void pdxDrawParamsReset(DT_PD_DOCDRAW_PARAMS* params);


#endif /* DTYPE_PDEXTRAS_H */
