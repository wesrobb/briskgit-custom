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


#ifndef DTYPE_DTYPESTD_LO_H
#define DTYPE_DTYPESTD_LO_H


#ifdef __cplusplus
DT_EXTERN {
#endif


/* ------------------------- D-Type Standard Engine (Low-Level API) ------------------------- */

typedef struct DT_DASYS_STRUCT* DT_DASYS;

typedef struct DT_DTSTREAM_STRUCT* DT_DTSTREAM;
//typedef void* DT_DTSTREAM;


/* String structure */
typedef struct
{
	DT_CHAR* str;
	DT_SLONG max, len;
	DT_UWORD flags;

} DT_STRING;


/* Stream structure */
typedef struct
{
	DT_ID_UBYTE stream_id;
	DT_UBYTE flags;
	DT_UBYTE reserved1;
	DT_UBYTE reserved2;

	union
	{
		/* File Stream */
		const DT_CHAR* file_name; /* used when stream_id=0 or stream_id=1, for reading and writing */

		/* Memory Stream */
		struct
		{
			const DT_UBYTE* addr;
			DT_SLONG len;

		} mem_r;                  /* used when stream_id=2, for reading */

		struct
		{
			DT_UBYTE* addr;
			DT_SLONG len;

		} mem_w;                  /* used when stream_id=2, for writing */

		/* URL, FURL and MURL Streams */
		const DT_CHAR* url;       /* used when stream_id=3, stream_id=5 or stream_id=6, for reading only */

		/* File-Pointer Stream */
		DT_FILE_PTR file_ptr;     /* used when stream_id=4, for reading and writing */

	} stream_locator;

} DT_STREAM_DESC;


/* Defines - Stream IDs */
#define DV_STREAM_DEFAULT 0
#define DV_STREAM_FILE 1
#define DV_STREAM_MEMORY 2
#define DV_STREAM_URL 3
#define DV_STREAM_FPTR 4
#define DV_STREAM_FURL 5
#define DV_STREAM_MURL 6


/* Defines - Stream Open/Close Flags */
#define DV_STREAM_OPEN_NO 0
#define DV_STREAM_OPEN_TRY 1
#define DV_STREAM_OPEN_MUST 2
#define DV_STREAM_CLOSE_NO 0
#define DV_STREAM_CLOSE_YES 1


/* Defines - Seek IDs */
#define DV_SEEK_ABS 0
#define DV_SEEK_REL 1
#define DV_SEEK_BGN 2
#define DV_SEEK_END 3


/* Defines - Compression/Decompression Methods */
#define DV_METHOD_NONE  0
#define DV_METHOD_ZLIB  1
#define DV_METHOD_HUFF  2
#define DV_METHOD_RANGE 3


/* Defines - String Macros */
#define DT_STRING_CHARS(s, max, chars) char s##str[max] = chars; DT_STRING s = {s##str, max, sizeof(chars) - 1, 0}
#define DT_STRING_PTRWR(s, max, ptrwr, len) DT_STRING s = {ptrwr, max, len, 0}


/* Defines - Stream Macros */
#define DT_STREAM_(sd, _id)                       DT_STREAM_DESC sd; sd.stream_id = _id; sd.flags = 0; sd.reserved1 = 0; sd.reserved2 = 0;
#define DT_STREAM_FILE(sd, _file_name)            DT_STREAM_(sd, DV_STREAM_FILE);   sd.stream_locator.file_name = _file_name;
#define DT_STREAM_MEMORY(sd, _mem_addr, _mem_len) DT_STREAM_(sd, DV_STREAM_MEMORY); sd.stream_locator.mem_r.addr = _mem_addr; sd.stream_locator.mem_r.len = _mem_len;
#define DT_STREAM_URL(sd, _url)                   DT_STREAM_(sd, DV_STREAM_URL);    sd.stream_locator.url = _url;
#define DT_STREAM_FPTR(sd, _file_ptr)             DT_STREAM_(sd, DV_STREAM_FPTR);   sd.stream_locator.file_ptr = _file_ptr;
#define DT_STREAM_FURL(sd, _url)                  DT_STREAM_(sd, DV_STREAM_FURL);   sd.stream_locator.url = _url;
#define DT_STREAM_MURL(sd, _url)                  DT_STREAM_(sd, DV_STREAM_MURL);   sd.stream_locator.url = _url;


/* D-Type Standard Engine Functions */

/* Initialize ASync System */
DT_SWORD DTYPE_APICALL daSystemIni(DT_DASYS* das, DT_UWORD max_aux_thread_count, DT_UWORD flags);
/* Deinitialize (destroy) ASync System */
void DTYPE_APICALL daSystemExt(DT_DASYS das);

/* Allocate memory by the engine */
void* DTYPE_APICALL dtMemAlloc(DT_SLONG size);
/* Reallocate memory by the engine */
void* DTYPE_APICALL dtMemRealloc(void* mem_addr, DT_SLONG size);
/* Free memory allocated by the engine */
void DTYPE_APICALL dtMemFree(void* mem_addr);
/* Set memory */
void DTYPE_APICALL dtMemSet(void* mem_addr, DT_UBYTE value, DT_SLONG size);
/* Copy memory */
void DTYPE_APICALL dtMemCopy(void* mem_addr, const void* mem_buff, DT_SLONG size);
/* Compare memory */
DT_SLONG DTYPE_APICALL dtMemCompare(const void* mem_addr, const void* mem_buff, DT_SLONG size);

/* Set Memory Pool */
DT_SLONG DTYPE_APICALL dtMemPoolSet(DT_SLONG init_size);
/* Get Memory Pool Stats */
DT_SLONG DTYPE_APICALL dtMemPoolGet(DT_SLONG* init_size, DT_SLONG* used_size, DT_SLONG* max_size);

/* Remove file (UTF-8 filename) */
DT_SLONG dtFileRemove(const DT_CHAR* filename);
/* Open file (UTF-8 filename) */
DT_FILE_PTR dtFileOpen(const DT_CHAR* filename, const DT_CHAR* mode);
/* Open file (UTF-8 filename) */
DT_FILE_PTR dtFileOpenX(const DT_CHAR* filename, const DT_CHAR* mode);
/* Close file */
DT_SLONG dtFileClose(DT_FILE_PTR fptr);

/* Open read stream */
DT_DTSTREAM DTYPE_APICALL dtStreamOpen(const DT_STREAM_DESC* sd, DT_UWORD flags);
/* Open write stream */
DT_DTSTREAM DTYPE_APICALL dtStreamOpenW(const DT_STREAM_DESC* sd, DT_UWORD flags, DT_UWORD extra_flags_write);
/* Read from stream */
DT_SWORD DTYPE_APICALL dtStreamRead(DT_DTSTREAM stream, DT_UBYTE* buffer, DT_SLONG len, DT_SWORD reserved);
/* Write to stream */
DT_SWORD DTYPE_APICALL dtStreamWrite(DT_DTSTREAM stream, const DT_UBYTE* buffer, DT_SLONG len, DT_SWORD reserved);
/* */
DT_SWORD DTYPE_APICALL dtStreamFlush(DT_DTSTREAM stream);
/* */
DT_SLONG DTYPE_APICALL dtStreamLoc(DT_DTSTREAM stream);
/* */
DT_SWORD DTYPE_APICALL dtStreamEos(DT_DTSTREAM stream);
/* */
DT_SWORD DTYPE_APICALL dtStreamSeek(DT_DTSTREAM stream, DT_SLONG offset, DT_ID_SWORD mode);
/* Close stream */
void DTYPE_APICALL dtStreamClose(DT_DTSTREAM stream, DT_UWORD flags);

/* Create stream */
DT_DTSTREAM DTYPE_APICALL dtStreamCreate(const DT_CHAR* temp_folder, DT_SWORD flags, DT_CHAR* fname, DT_UBYTE* source_addr, DT_SLONG source_len, DT_STREAM_DESC* sd);
/* Destroy stream */
void DTYPE_APICALL dtStreamDestroy(DT_DTSTREAM stream, DT_SWORD flags);

/* Compress data in buffer */
DT_SWORD DTYPE_APICALL dtBufferCompress(DT_ID_SWORD method, DT_UBYTE* compr_addr, DT_SLONG* compr_len, const DT_UBYTE* raw_addr, DT_SLONG raw_len);
/* Decompress data in buffer */
DT_SWORD DTYPE_APICALL dtBufferDecompress(DT_ID_SWORD method, DT_UBYTE* raw_addr, DT_SLONG* raw_len, const DT_UBYTE* compr_addr, DT_SLONG compr_len);

/* Int Square Root */
DT_ULONG DTYPE_APICALL dtMathISqrt(DT_ULONG val, DT_UWORD flags);
/* Int Square Root XL */
DT_UHUGE DTYPE_APICALL dtMathISqrtXL(DT_SHUGE val, DT_UWORD flags);
/* Int Distance */
DT_ULONG DTYPE_APICALL dtMathIDist(DT_SLONG dx, DT_SLONG dy, DT_UWORD flags);
/* Int Distance XL */
DT_UHUGE DTYPE_APICALL dtMathIDistXL(DT_SHUGE dx, DT_SHUGE dy, DT_UWORD flags);

/* Secure Positive Add */
DT_SLONG DTYPE_APICALL dtSecAdd(DT_SLONG a, DT_SLONG b);
/* Secure Positive Mul */
DT_SLONG DTYPE_APICALL dtSecMul(DT_SLONG a, DT_SLONG b);

/* Sort array(s) */
void DTYPE_APICALL dtArrSort(DT_SLONG arr[], DT_SLONG arr2[], DT_SLONG arr_size);
/* Sort array(s) */
void DTYPE_APICALL dtArrSort2(DT_SLONG arr[], DT_SLONG arr2[], DT_SLONG arr3[], DT_SLONG arr_size);

/* String concatenate chars */
DT_STRING* DTYPE_APICALL dtStrConcatChars(DT_STRING* s, const DT_CHAR* chars, DT_SLONG len);
/* String concatenate SLong */
DT_STRING* DTYPE_APICALL dtStrConcatSLong(DT_STRING* s, DT_SLONG num, DT_UWORD flags);
/* String concatenate ULong */
DT_STRING* DTYPE_APICALL dtStrConcatULong(DT_STRING* s, DT_ULONG num, DT_UWORD flags);
/* String concatenate Float */
DT_STRING* DTYPE_APICALL dtStrConcatFloat(DT_STRING* s, DT_FLOAT num, DT_UWORD flags);

/* Check system status */
DT_SWORD DTYPE_APICALL dtSysPST(DT_SLONG id, DT_SWORD flag, DT_UBYTE param1, DT_UBYTE param2);


#ifdef __cplusplus
}
#endif


#endif /* DTYPE_DTYPESTD_LO_H */
