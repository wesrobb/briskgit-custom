/*
 * pdextras.cpp - auxiliary implementation file for D-Type Power Engine
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#include <time.h>
#include <string.h>

#include "dtype.h"
#include "pdextras.h"


#ifndef DEF_AUX_INIT_KEY
#define DEF_AUX_INIT_KEY 0
#endif
#ifndef DEF_AUX_INIT_FLAG
#define DEF_AUX_INIT_FLAG 0
#endif


#define DF_PDX_MEMSET dtMemSet
#define DF_PDX_MEMCPY dtMemCopy
#define DF_PDX_MEMCMP dtMemCompare
//#define DF_PDX_MEMSET memset
//#define DF_PDX_MEMCPY memcpy
//#define DF_PDX_MEMCMP memcmp
#define DF_PDX_ABS abs
#define DF_PDX_ATOI atoi
#define DF_PDX_ATOL atol
#define DF_PDX_ATOF atof
#define DF_PDX_STRCHR strchr
#define DF_PDX_STRCAT strcat
#define DF_PDX_STRCPY strcpy
#define DF_PDX_STRCMP strcmp
#define DF_PDX_STRLEN(s) static_cast<DT_SLONG>(strlen(s))
//#define DF_PDX_SPRINTF sprintf

#define DF_PDX_SEC_ADD(a, b) dtSecAdd(a, b)
#define DF_PDX_SEC_MUL(a, b) dtSecMul(a, b)

#ifndef DEF_MEMORY
#define DF_PDX_MEM_ALLOC(size) dtMemAlloc(size)
#define DF_PDX_MEM_REALLOC(ptr, size) dtMemRealloc(ptr, size)
#define DF_PDX_MEM_FREE(ptr) dtMemFree(ptr)
#endif

#define DEF_PDX_ZLIB
//#define DEF_PDX_EXPORT_PDF
//#define DEF_PDX_EXPORT_SVG


const struct
{
	DT_ID_UBYTE device_mode;
	DT_ID_UBYTE positioning;
}
DType_ConfigTextLayout[6] =
{
	{  0, 11},
	{  0, 13},
	{  1,  0},
	{  2,  0},
	{101,  0},
	{102,  0}
};


DT_ID_SWORD pdxIsKnownFileType(const DT_UBYTE* m, DT_SLONG l, const DT_CHAR* mime_default, DT_CHAR* mime_type, DT_SLONG* offset, DT_ID_SWORD* compress_method)
{
	*offset = 0;
	*compress_method = 0;

	if (m == DV_NULL || l <= 0) {}
	else if (l > 24 && DF_PDX_MEMCMP(m, "{{INTEGRAL_FORMAT_GZIP}}", 24) == 0) { *offset = 24; *compress_method = 1; }
	else if (l > 17 && DF_PDX_MEMCMP(m, "DENEMKO-DTFONTV1-", 17) == 0) { DF_PDX_STRCPY(mime_type, "font/dtf"); return 1; }
	else if (l > 17 && DF_PDX_MEMCMP(m, "DENEMKO-POWERDOC-", 17) == 0) { DF_PDX_STRCPY(mime_type, "application/powerdoc"); return 2; }
	else if (l > 8 && m[0] == 137 && m[1] ==  80 && m[2] ==  78 && m[3] == 71 && m[4] == 13 && m[5] == 10 && m[6] == 26 && m[7] == 10) { DF_PDX_STRCPY(mime_type, "image/png"); return 3; }
	else if (l > 4 && m[0] == 'G' && m[1] == 'I' && m[2] == 'F' && m[3] == '8') { DF_PDX_STRCPY(mime_type, "image/gif"); return 9; }
	else if (l > 4 && m[0] ==  37 && m[1] ==  80 && m[2] ==  68 && m[3] == 70) { DF_PDX_STRCPY(mime_type, "application/pdf"); return 4; }
	else if (l > 4 && m[0] == '<' && m[1] == 's' && m[2] == 'v' && m[3] =='g') { *compress_method = 1; DF_PDX_STRCPY(mime_type, "image/svg+xml"); /*DF_PDX_STRCPY(mime_type, "text/html")*/ return 5; }
	else if (l > 3 && m[0] == 255 && m[1] == 216 && m[2] == 255) { DF_PDX_STRCPY(mime_type, "image/jpg"); return 6; }
	else if (l > 2 && m[0] ==  66 && m[1] == 77) { *compress_method = 1; DF_PDX_STRCPY(mime_type, "image/bmp"); return 7; }
	else if (l > 2 && m[0] ==  80 && m[1] == 75) { DF_PDX_STRCPY(mime_type, "application/zip"); return 8; }

	DF_PDX_STRCPY(mime_type, mime_default);
	return 0;
}


DT_SWORD pdxStringWriteToStream(const DT_CHAR* str, DT_DTSTREAM stream)
{
	return dtStreamWrite(stream, reinterpret_cast<const DT_UBYTE*>(str), DF_PDX_STRLEN(str), 0);
}


DT_CHAR* pdxStringZap(DT_CHAR* str)
{
	DT_SLONG i = 0, j = 0;

	while (str[i] != 0)
	{
		if ((DT_UBYTE)(str[i]) >= 32)
		{
			str[j] = str[i];
			j++;
		}
		i++;
	}
	str[j] = 0;

	return str;
}


DT_CHAR* pdxStringReplaceChr(DT_CHAR* str, DT_CHAR chr_find, DT_CHAR chr_replace)
{
	DT_SLONG i = 0;

	while (str[i] != 0)
	{
		if (str[i] == chr_find) str[i] = chr_replace;
		i++;
	}

	return str;
}


DT_SWORD pdxStringInputDelimited(DT_CHAR* str, DT_DTSTREAM stream, DT_CHAR delimiter, DT_SLONG max_len)
{
	if (str == DV_NULL || max_len < 1) return 0;

	DT_SLONG i = 0;
	DT_UBYTE b = 0;
	//DT_UBYTE b_min = 0;

	max_len--;

	while (!dtStreamEos(stream))
	{
		dtStreamRead(stream, &b, 1, 0);
		if (b == 0 || b == 10 || b == delimiter) break;
		else if (b == 13)
		{
			dtStreamRead(stream, &b, 1, 0); if (b == 0 || b == 10 || dtStreamEos(stream)) break;
			dtStreamSeek(stream, -1, DV_SEEK_REL);
			break;
		}
		else if (i >= max_len) continue;
		//else if (b < b_min) continue;

		str[i] = b; i++;
	}

	str[i] = 0;
	pdxStringZap(str);

	if (dtStreamEos(stream)) return 0;
	return 1;
}


DT_SWORD pdxStringInput(DT_CHAR* str, DT_DTSTREAM stream, DT_SLONG max_len)
{
	return pdxStringInputDelimited(str, stream, '|', max_len);
}


DT_SWORD pdxIntegerInput(DT_SLONG* number, DT_DTSTREAM stream)
{
	DT_CHAR str[PDX_MAXTEXTLENGTH];

	if (pdxStringInput(str, stream, PDX_MAXTEXTLENGTH) == 1)
	{
		*number = DF_PDX_ATOL(str);
		return 1;
	}

	return 0;
}


DT_SWORD pdxStringEnumGet(DT_CHAR* str, const DT_CHAR* file_name, DT_SLONG index)
{
	DT_DTSTREAM stream;
	DT_SLONG i = 0;
	DT_SWORD ret = 0;
	DT_CHAR v[PDX_MAXTEXTLENGTH];

	DF_PDX_STRCPY(str, "undefined");

	DT_STREAM_FILE(sd, file_name);
	if ((stream = dtStreamOpen(&sd, DV_STREAM_OPEN_MUST)) == DV_NULL) return 0;

	while (!dtStreamEos(stream))
	{
		if (pdxIntegerInput(&i, stream) == 0) break;
		if (pdxStringInput(v, stream, PDX_MAXTEXTLENGTH) == 0) break;
		if (i == index)
		{
			ret = 1;
			DF_PDX_STRCPY(str, v);
			break;
		}
	}

	dtStreamClose(stream, DV_STREAM_CLOSE_YES);
	return ret;
}


DT_SLONG pdxTextFixNewline(DT_UBYTE* buffer, DT_SLONG buffer_len, DT_ID_SWORD sz)
{
	DT_SLONG i, j = 0;
	DT_ULONG c = 0, c0 = 0;
	DT_ULONG c_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);
	DT_ULONG c_newline2 = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE2);
	const DT_UBYTE* addr = buffer;

	if (addr == DV_NULL || buffer_len < 0) return j;

	for (i = 0; i < buffer_len; i += sz, addr += sz)
	{
		     if (sz == DV_SIZE_1) c = DF_READ_UBYTE_LE(addr);
		else if (sz == DV_SIZE_2) c = DF_READ_UWORD_LE(addr);
		else if (sz == DV_SIZE_3) c = DF_READ_UTRIO_LE(addr);
		else if (sz == DV_SIZE_4) c = DF_READ_ULONG_LE(addr);

		if ((c0 == c_newline2 && c == c_newline) || (c0 == c_newline && c == c_newline2)) continue; else if (c == c_newline2) c = c_newline;

		     if (sz == DV_SIZE_1) DF_WRITE_UBYTE_LE(buffer, (DT_UBYTE)c);
		else if (sz == DV_SIZE_2) DF_WRITE_UWORD_LE(buffer, (DT_UWORD)c);
		else if (sz == DV_SIZE_3) DF_WRITE_UTRIO_LE(buffer, c);
		else if (sz == DV_SIZE_4) DF_WRITE_ULONG_LE(buffer, c);

		j += sz;
		buffer += sz;
		c0 = c;
	}

	return j;
}


/* Matrix Operations */


void pdxMatrixMultiplyPlus(const DT_TM3X3 m, const DT_TM3X3 a, DT_TM3X3 r)
{
	DT_FLOAT m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m20 = m[2][0], m21 = m[2][1], m22 = m[2][2];
	DT_FLOAT a00 = a[0][0], a01 = a[0][1], a02 = a[0][2], a10 = a[1][0], a11 = a[1][1], a12 = a[1][2], a20 = a[2][0], a21 = a[2][1], a22 = a[2][2];

	r[0][0] = m00 * a00 + m01 * a10 + m02 * a20;
	r[0][1] = m00 * a01 + m01 * a11 + m02 * a21;
	r[0][2] = m00 * a02 + m01 * a12 + m02 * a22;
	r[1][0] = m10 * a00 + m11 * a10 + m12 * a20;
	r[1][1] = m10 * a01 + m11 * a11 + m12 * a21;
	r[1][2] = m10 * a02 + m11 * a12 + m12 * a22;
	r[2][0] = m20 * a00 + m21 * a10 + m22 * a20;
	r[2][1] = m20 * a01 + m21 * a11 + m22 * a21;
	r[2][2] = m20 * a02 + m21 * a12 + m22 * a22;
}


void pdxMatrixMultiply(const DT_TM2X2 m, const DT_TM2X2 a, DT_TM2X2 r)
{
	DT_FLOAT m00 = m[0][0], m01 = m[0][1], m10 = m[1][0], m11 = m[1][1];
	DT_FLOAT a00 = a[0][0], a01 = a[0][1], a10 = a[1][0], a11 = a[1][1];

	r[0][0] = m00 * a00 + m01 * a10;
	r[0][1] = m00 * a01 + m01 * a11;
	r[1][0] = m10 * a00 + m11 * a10;
	r[1][1] = m10 * a01 + m11 * a11;
}


DT_SWORD pdxMatrixGetTransformPlus(DT_SLONG matrix_id, DT_TM3X3 transform)
{
	transform[0][0] = 1.0; transform[0][1] = 0.0; transform[0][2] = 0.0;
	transform[1][0] = 0.0; transform[1][1] = 1.0; transform[1][2] = 0.0;
	transform[2][0] = 0.0; transform[2][1] = 0.0; transform[2][2] = 1.0;

	switch (matrix_id)
	{
	case 0: /* No Transformation */
		return 1;

	case 1: /* Horizontal Scale 0.5 */
		transform[0][0] = 0.5;
		return 1;

	case 2: /* Horizontal Scale 2.0 */
		transform[0][0] = 2.0;
		return 1;

	case 3: /* Vertical Scale 0.5 */
		transform[1][1] = 0.5;
		return 1;

	case 4: /* Vertical Scale 2.0 */
		transform[1][1] = 2.0;
		return 1;

	case 5: /* Horizontal Flip */
		transform[0][0] = -1.0;
		return 1;

	case 6: /* Horizontal Flip 0.5 */
		transform[0][0] = -0.5;
		return 1;

	case 7: /* Horizontal Flip 2.0 */
		transform[0][0] = -2.0;
		return 1;

	case 8: /* Vertical Flip */
		transform[1][1] = -1.0;
		return 1;

	case 9: /* Vertical Flip 0.5 */
		transform[1][1] = -0.5;
		return 1;

	case 10: /* Vertical Flip 2.0 */
		transform[1][1] = -2.0;
		return 1;

	case 11: /* Horizontal Skew -10 degrees */
		transform[0][1] = 0.17633;
		return 1;

	case 12: /* Horizontal Skew -5 degrees */
		transform[0][1] = 0.08749;
		return 1;

	case 13: /* Horizontal Skew 5 degrees */
		transform[0][1] = -0.08749;
		return 1;

	case 14: /* Horizontal Skew 10 degrees */
		transform[0][1] = -0.17633;
		return 1;

	case 15: /* Vertical Skew -10 degrees */
		transform[1][0] = 0.17633;
		return 1;

	case 16: /* Vertical Skew -5 degrees */
		transform[1][0] = 0.08749;
		return 1;

	case 17: /* Vertical Skew 5 degrees */
		transform[1][0] = -0.08749;
		return 1;

	case 18: /* Vertical Skew 10 degrees */
		transform[1][0] = -0.17633;
		return 1;

	case 19: /* Rotation 45 degrees */
		transform[0][0] = 0.707107; transform[0][1] = -0.707107;
		transform[1][0] = 0.707107; transform[1][1] = 0.707107;
		return 1;

	case 20: /* Rotation 90 degrees */
		transform[0][0] = 0.0; transform[0][1] = -1.0;
		transform[1][0] = 1.0; transform[1][1] = 0.0;
		return 1;

	case 21: /* Rotation 135 degrees */
		transform[0][0] = -0.707107; transform[0][1] = -0.707107;
		transform[1][0] = 0.707107; transform[1][1] = -0.707107;
		return 1;

	case 22: /* Rotation 180 degrees */
		transform[0][0] = -1.0; transform[0][1] = 0.0;
		transform[1][0] = 0.0; transform[1][1] = -1.0;
		return 1;

	case 23: /* Rotation 225 degrees */
		transform[0][0] = -0.707107; transform[0][1] = 0.707107;
		transform[1][0] = -0.707107; transform[1][1] = -0.707107;
		return 1;

	case 24: /* Rotation 270 degrees */
		transform[0][0] = 0.0; transform[0][1] = 1.0;
		transform[1][0] = -1.0; transform[1][1] = 0.0;
		return 1;

	case 25: /* Rotation 315 degrees */
		transform[0][0] = 0.707107; transform[0][1] = 0.707107;
		transform[1][0] = -0.707107; transform[1][1] = 0.707107;
		return 1;

	case 26: /* Perspective 1A */
		transform[2][0] = 0.001;
		return 1;

	case 27: /* Perspective 1B */
		transform[2][1] = 0.001;
		return 1;

	case 28: /* Perspective 1C */
		transform[2][0] = 0.001; transform[2][1] = 0.001;
		return 1;

	case 29: /* Perspective 2A */
		transform[0][0] = 0.5; transform[0][1] = -0.042; transform[0][2] = -200;
		transform[1][0] = 0.0; transform[1][1] = 0.5; transform[1][2] = -100;
		transform[2][0] = 0.0; transform[2][1] = -0.001;
		return 1;

	case 30: /* Perspective 2B */
		transform[0][0] = 0.5; transform[0][1] = -0.042; transform[0][2] = -200;
		transform[1][0] = 0.0; transform[1][1] = 0.5; transform[1][2] = -100;
		transform[2][0] = -0.001; transform[2][1] = 0.0;
		return 1;

	case 31: /* Perspective 2C */
		transform[0][0] = -0.042; transform[0][1] = 0.5; transform[0][2] = -200;
		transform[1][0] = 0.5; transform[1][1] = 0.0; transform[1][2] = -100;
		transform[2][0] = 0.0; transform[2][1] = -0.001;
		return 1;

	case 32: /* Perspective 2D */
		transform[0][0] = -0.042; transform[0][1] = 0.5; transform[0][2] = -200;
		transform[1][0] = 0.5; transform[1][1] = 0.0; transform[1][2] = -100;
		transform[2][0] = -0.001; transform[2][1] = 0.0;
		return 1;
	}

	return 0;
}


DT_SWORD pdxMatrixGetTransform(DT_SLONG matrix_id, DT_TM2X2 transform)
{
	DT_TM3X3 t;
	DT_SWORD ret = pdxMatrixGetTransformPlus(matrix_id, t);

	transform[0][0] = t[0][0];
	transform[0][1] = t[0][1];
	transform[1][0] = t[1][0];
	transform[1][1] = t[1][1];

	return ret;
}


void pdxObjDelete(DT_PDDOC power_doc, DT_ID_SLONG obj_del)
{
	DT_UBYTE type;
	DT_ID_SLONG link;
	DT_ID_SLONG obj, obj_last = pdObjGetLast(power_doc);
	DT_SLONG img_last = pdImgGetLast(power_doc);
	DT_SLONG prop_name, target;
	DT_SLONG i, j, k;

	/* Synchronize Links and References*/

	for (obj = 0; obj <= obj_last; obj++)
	{
		j = pdNameGet(power_doc, obj, -1, &type);

		for (i = 0, k = 0; i < j; i++, k++)
		{
			prop_name = pdNameGet(power_doc, obj, k, &type);

			if (type == PD_FT_LINK)
			{
				/* Link */
				if ((link = pdLinkGet(power_doc, obj, prop_name)) == -1) continue;

				else if (link == obj_del)
				{
					pdLinkDel(power_doc, obj, prop_name);
					k--;
				}
				else if (obj < obj_del && link > obj_del)
				{
					pdLinkSet(power_doc, obj, prop_name, link - 1);
					k--;
				}
				else if (obj > obj_del && link < obj_del)
				{
					pdLinkSet(power_doc, obj, prop_name, link + 1);
					k--;
				}
			}
			else if (type == PD_FT_PROPERTYREFERENCE)
			{
				/* Property Reference */
				if ((link = pdPropGetAsRef(power_doc, obj, prop_name, &target)) == -1) continue;

				else if (link == obj_del)
				{
					pdPropDel(power_doc, obj, prop_name);
					k--;
				}
				else if (obj < obj_del && link > obj_del)
				{
					pdPropSetAsRef(power_doc, obj, prop_name, link - 1, target);
					k--;
				}
				else if (obj > obj_del && link < obj_del)
				{
					pdPropSetAsRef(power_doc, obj, prop_name, link + 1, target);
					k--;
				}
			}
		}
	}

	/* Synchronize Images */

	for (i = 0; i <= img_last; i++)
	{
		obj = pdImgGetObj(power_doc, i);

		if (obj == obj_del)
		{
			pdImgDel(power_doc, i);
			img_last--;
			i--;
		}
		else if (obj > obj_del) pdImgSetObj(power_doc, i, obj - 1);
	}

	/* Finally delete the object */

	pdObjDel(power_doc, obj_del);
}


DT_SWORD pdxObjCompare(DT_PDDOC power_doc, DT_ID_SLONG obj1, DT_ID_SLONG obj2)
{
	const DT_CHAR* desc;
	DT_UBYTE* prop_data1;
	DT_UBYTE* prop_data2;
	DT_UBYTE type, prop_ref = PD_FT_PROPERTYREFERENCE;
	DT_SLONG i = 0, prop_name, name;
	DT_SLONG prop_len1, prop_len2;
	DT_SLONG target1, target2;
	DT_ID_SLONG link1, link2;

	/* If Obj1 has a different number of fields than Obj2, test is not successful */
	if (pdObjGet(power_doc, obj1, &name, &desc) != pdObjGet(power_doc, obj2, &name, &desc)) return 0;

	/* Go through all fields in Obj1 */
	while ((prop_name = pdNameGet(power_doc, obj1, i++, &type)) != 0)
	{
		if (type == PD_FT_PROPERTY || type == PD_FT_EXTPROPERTY_BUFFER || type == PD_FT_EXTPROPERTY_CALLBACK || type == prop_ref)
		{
			/* Regular Property, External Property or Property Reference (depending on prop_ref) */
			if ((prop_data1 = pdPropGet(power_doc, obj1, prop_name, &prop_len1)) == DV_NULL) return 0;
			if ((prop_data2 = pdPropGet(power_doc, obj2, prop_name, &prop_len2)) == DV_NULL) return 0;
			/* If the length of both properies is not the same, test is not successful */
			if (prop_len1 != prop_len2) return 0;
			/* If the content of both properties is not the same, test is not successful */
			if (DF_PDX_MEMCMP(prop_data1, prop_data2, prop_len1) != 0) return 0;
		}
		else if (type == PD_FT_LINK)
		{
			/* Link */
			link1 = pdLinkGet(power_doc, obj1, prop_name);
			link2 = pdLinkGet(power_doc, obj2, prop_name);
			if (link1 != link2) return 0;
		}
		else if (type == PD_FT_PROPERTYREFERENCE)
		{
			/* Property Reference */
			link1 = pdPropGetAsRef(power_doc, obj1, prop_name, &target1);
			link2 = pdPropGetAsRef(power_doc, obj2, prop_name, &target2);
			if (link1 != link2) return 0;
			if (target1 != target2) return 0;
		}
	}

	return 1;
}


DT_SLONG pdxObjExport(DT_PDDOC power_doc, DT_DTSTREAM stream, DT_ID_SLONG obj, DT_UBYTE use_name_enums, const DT_PDX_EXPORT_CONFIG* conf)
{
	DT_STRING_CHARS(str, PDX_MAXTEXTLENGTH, "");
	DT_STRING_CHARS(buffer, PDX_MAXTEXTLENGTH, "");
	DT_CHAR name_enum[64] = "";
	DT_CHAR format_enum[64] = "";
	DT_CHAR hex_data[8] = "";
	DT_CHAR quote[2] = "";
	DT_CHAR divider[2] = " ";
	DT_CHAR* field_name;
	DT_CHAR* format;
	DT_UBYTE* prop_data;
	DT_UBYTE t, type;
	DT_SLONG i, j;
	DT_SLONG prop_len = 0, compress_len = 0;
	DT_SLONG prop_name, target;
	DT_ID_SLONG link;
	DT_SBYTE format_type;
	DT_UBYTE val_ubyte;
	DT_SLONG val_slong;
	DT_ULONG val_ulong;
	DT_UBYTE state = 0;

	if (use_name_enums == 1)
	{
		DF_PDX_STRCPY(name_enum, "NameEnum.");
		DF_PDX_STRCPY(format_enum, "DataTypeEnum.");
	}
	else if (use_name_enums == 2)
	{
		DF_PDX_STRCPY(name_enum, "doc.");
		DF_PDX_STRCPY(format_enum, "doc.");
	}

	//DF_PDX_SPRINTF(str, "%s%s Properties for object %ld %s%s%s", conf->Tab, conf->CommentOpen, obj, conf->CommentClose, conf->NewLine, conf->NewLine);
	str.len = 0;
	dtStrConcatChars(&str, conf->Tab);
	dtStrConcatChars(&str, conf->CommentOpen);
	dtStrConcatChars(&str, " Properties for object ");
	dtStrConcatSLong(&str, obj);
	dtStrConcatChars(&str, " ");
	dtStrConcatChars(&str, conf->CommentClose);
	dtStrConcatChars(&str, conf->NewLine);
	if (conf->ObjAddOpen[0] != '@') dtStrConcatChars(&str, conf->NewLine);
	pdxStringWriteToStream(str.str, stream);
	str.len = 0;

	t = PD_FT_LINK;

loop:

	i = 0;

	while ((prop_name = pdNameGet(power_doc, obj, i++, &type)) != 0)
	{
		if (t != type) continue;

		if (pdxStringEnumGet(buffer.str, "config/field.inf", prop_name) == 0) { buffer.len = 0; dtStrConcatSLong(&buffer, prop_name); }

		DF_PDX_STRCPY(quote, "\"");
		DF_PDX_STRCPY(divider, ",");
		field_name = buffer.str;
		if (field_name[0] == '>') field_name++;
		if (use_name_enums == 1 && field_name[0] == 'p' && field_name[1] == 'd') field_name[0] = 'P';

		str.len = 0;

		if (type == PD_FT_PROPERTY || type == PD_FT_EXTPROPERTY_BUFFER || type == PD_FT_EXTPROPERTY_CALLBACK)
		{
			/* Regular Property or External Property */
			if (conf->PropAddOpen[0] == '@')
			{
				if (state == 0)
				{
					state = 1;
					dtStrConcatChars(&str, conf->Tab);
					dtStrConcatChars(&str, "obj");
					dtStrConcatChars(&str, conf->ArrBracketOpen);
					dtStrConcatSLong(&str, obj);
					dtStrConcatChars(&str, conf->ArrBracketClose);
				}
				dtStrConcatChars(&str, conf->PropAddOpen + 1);
				//dtStrConcatChars(&str, ", ");
			}
			else if (conf->PropAddOpen[0] != 0)
			{
				//DF_PDX_SPRINTF(str, "%s%sobj%s%ld%s, ", conf->Tab, conf->PropAddOpen, conf->ArrBracketOpen, obj, conf->ArrBracketClose);
				dtStrConcatChars(&str, conf->Tab);
				dtStrConcatChars(&str, conf->PropAddOpen);
				dtStrConcatChars(&str, "obj");
				dtStrConcatChars(&str, conf->ArrBracketOpen);
				dtStrConcatSLong(&str, obj);
				dtStrConcatChars(&str, conf->ArrBracketClose);
				dtStrConcatChars(&str, ", ");
			}
			else
			{
				DF_PDX_STRCPY(quote, "");
				DF_PDX_STRCPY(divider, ":");
			}

			//DF_PDX_SPRINTF(str, "%s%s%s %s", name_enum, field_name, divider, conf->PropAddCast);
			dtStrConcatChars(&str, name_enum);
			dtStrConcatChars(&str, field_name);
			dtStrConcatChars(&str, divider);
			dtStrConcatChars(&str, " ");
			dtStrConcatChars(&str, conf->PropAddCast);
			pdxStringWriteToStream(str.str, stream);

			prop_data = pdPropGetH(power_doc, obj, prop_name, &prop_len, DV_NULL, &compress_len);
			if (compress_len > 0) { prop_len = compress_len; DF_PDX_STRCPY(buffer.str, "PD_HEX_COMPRESSED"); } else pdxStringEnumGet(buffer.str, "config/format.inf", prop_name);

			format = buffer.str;

			/* Simple Types */

			format_type = 0;
			str.len = 0;
			val_slong = 0;
			val_ulong = 0;

			if (DF_PDX_STRCMP(format, "PD_BYTE_SIGNED") == 0)
			{
				/* Signed Byte */
				format_type = -1; if (prop_len == 1) { val_slong = DF_READ_SBYTE_LE(prop_data); } else format_type = 2; /* err */
			}
			else if (DF_PDX_STRCMP(format, "PD_BYTE_UNSIGNED") == 0)
			{
				/* Unsigned Byte */
				format_type = 1; if (prop_len == 1) { val_ulong = DF_READ_UBYTE_LE(prop_data); } else format_type = 2; /* err */
			}
			else if (DF_PDX_STRCMP(format, "PD_WORD_SIGNED") == 0)
			{
				/* Signed Word */
				format_type = -1; if (prop_len == 2) { val_slong = DF_READ_SWORD_LE(prop_data); } else if (prop_len == 1) { val_slong = DF_READ_SBYTE_LE(prop_data); } else format_type = 2; /* err */
			}
			else if (DF_PDX_STRCMP(format, "PD_WORD_UNSIGNED") == 0)
			{
				/* Unsigned Word */
				format_type = 1; if (prop_len == 2) { val_ulong = DF_READ_UWORD_LE(prop_data); } else if (prop_len == 1) { val_ulong = DF_READ_UBYTE_LE(prop_data); } else format_type = 2; /* err */
			}
			else if (DF_PDX_STRCMP(format, "PD_TRIO_SIGNED") == 0)
			{
				/* Signed Long */
				format_type = -1; if (prop_len == 3) { val_slong = DF_READ_STRIO_LE(prop_data); } else if (prop_len == 2) { val_slong = DF_READ_SWORD_LE(prop_data); } else if (prop_len == 1) { val_slong = DF_READ_SBYTE_LE(prop_data); } else format_type = 2; /* err */
			}
			else if (DF_PDX_STRCMP(format, "PD_TRIO_UNSIGNED") == 0)
			{
				/* Unsigned Long */
				format_type = 1; if (prop_len == 3) { val_ulong = DF_READ_UTRIO_LE(prop_data); } else if (prop_len == 2) { val_ulong = DF_READ_UWORD_LE(prop_data); } else if (prop_len == 1) { val_ulong = DF_READ_UBYTE_LE(prop_data); } else format_type = 2; /* err */
			}
			else if (DF_PDX_STRCMP(format, "PD_LONG_SIGNED") == 0)
			{
				/* Signed Long */
				format_type = -1; if (prop_len == 4) { val_slong = DF_READ_SLONG_LE(prop_data); } else if (prop_len == 3) { val_slong = DF_READ_STRIO_LE(prop_data); } else if (prop_len == 2) { val_slong = DF_READ_SWORD_LE(prop_data); } else if (prop_len == 1) { val_slong = DF_READ_SBYTE_LE(prop_data); } else format_type = 2; /* err */
			}
			else if (DF_PDX_STRCMP(format, "PD_LONG_UNSIGNED") == 0)
			{
				/* Unsigned Long */
				format_type = 1; if (prop_len == 4) { val_ulong = DF_READ_ULONG_LE(prop_data); } else if (prop_len == 3) { val_ulong = DF_READ_UTRIO_LE(prop_data); } else if (prop_len == 2) { val_ulong = DF_READ_UWORD_LE(prop_data); } else if (prop_len == 1) { val_ulong = DF_READ_UBYTE_LE(prop_data); } else format_type = 2; /* err */
			}

			if (format_type == 2)
			{
				dtStrConcatChars(&str, "*** ERR ***");
				pdxStringWriteToStream(str.str, stream);
			}
			else if (format_type == 1)
			{
				dtStrConcatChars(&str, quote); dtStrConcatULong(&str, val_ulong); dtStrConcatChars(&str, quote);
				pdxStringWriteToStream(str.str, stream);
			}
			else if (format_type == -1)
			{
				dtStrConcatChars(&str, quote); dtStrConcatSLong(&str, val_slong); dtStrConcatChars(&str, quote);
				pdxStringWriteToStream(str.str, stream);
			}

			/* Arrays */

			str.len = 0;

			if (format_type) {}
			else if (DF_PDX_STRCMP(format, "PD_ARR_BYTE_SIGNED") == 0)
			{
				/* Array of signed bytes */
				pdxStringWriteToStream(quote, stream);
				for (j = 0; j < prop_len; j++)
				{
					str.len = 0; if (j > 0) dtStrConcatChars(&str, ", "); dtStrConcatSLong(&str, DF_READ_SBYTE_LE(prop_data + j));
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}
			else if (DF_PDX_STRCMP(format, "PD_ARR_BYTE_UNSIGNED") == 0)
			{
				/* Array of unsigned bytes */
				pdxStringWriteToStream(quote, stream);
				for (j = 0; j < prop_len; j++)
				{
					str.len = 0; if (j > 0) dtStrConcatChars(&str, ", "); dtStrConcatULong(&str, DF_READ_UBYTE_LE(prop_data + j));
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}
			else if (DF_PDX_STRCMP(format, "PD_ARR_WORD_SIGNED") == 0)
			{
				/* Array of signed words */
				pdxStringWriteToStream(quote, stream);
				prop_len /= 2; prop_len *= 2;
				for (j = 0; j < prop_len; j += 2)
				{
					str.len = 0; if (j > 0) dtStrConcatChars(&str, ", "); dtStrConcatSLong(&str, DF_READ_SWORD_LE(prop_data + j));
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}
			else if (DF_PDX_STRCMP(format, "PD_ARR_WORD_UNSIGNED") == 0)
			{
				/* Array of unsigned words */
				pdxStringWriteToStream(quote, stream);
				prop_len /= 2; prop_len *= 2;
				for (j = 0; j < prop_len; j += 2)
				{
					str.len = 0; if (j > 0) dtStrConcatChars(&str, ", "); dtStrConcatULong(&str, DF_READ_UWORD_LE(prop_data + j));
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}
			else if (DF_PDX_STRCMP(format, "PD_ARR_TRIO_SIGNED") == 0)
			{
				/* Array of signed trios */
				pdxStringWriteToStream(quote, stream);
				prop_len /= 3; prop_len *= 3;
				for (j = 0; j < prop_len; j += 3)
				{
					str.len = 0; if (j > 0) dtStrConcatChars(&str, ", "); dtStrConcatSLong(&str, DF_READ_STRIO_LE(prop_data + j));
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}
			else if (DF_PDX_STRCMP(format, "PD_ARR_TRIO_UNSIGNED") == 0)
			{
				/* Array of unsigned trios */
				pdxStringWriteToStream(quote, stream);
				prop_len /= 3; prop_len *= 3;
				for (j = 0; j < prop_len; j += 3)
				{
					str.len = 0; if (j > 0) dtStrConcatChars(&str, ", "); dtStrConcatULong(&str, DF_READ_UTRIO_LE(prop_data + j));
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}
			else if (DF_PDX_STRCMP(format, "PD_ARR_LONG_SIGNED") == 0)
			{
				/* Array of signed longs */
				pdxStringWriteToStream(quote, stream);
				prop_len /= 4; prop_len *= 4;
				for (j = 0; j < prop_len; j += 4)
				{
					str.len = 0; if (j > 0) dtStrConcatChars(&str, ", "); dtStrConcatSLong(&str, DF_READ_SLONG_LE(prop_data + j));
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}
			else if (DF_PDX_STRCMP(format, "PD_ARR_LONG_UNSIGNED") == 0)
			{
				/* Array of unsigned longs */
				pdxStringWriteToStream(quote, stream);
				prop_len /= 4; prop_len *= 4;
				for (j = 0; j < prop_len; j += 4)
				{
					str.len = 0; if (j > 0) dtStrConcatChars(&str, ", "); dtStrConcatULong(&str, DF_READ_ULONG_LE(prop_data + j));
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}

			else if (DF_PDX_STRCMP(format, "PD_ASC") == 0)
			{
				/* ASCII String */
				DF_PDX_STRCPY(quote, "\"");
				pdxStringWriteToStream(quote, stream);
				for (j = 0; j < prop_len; j++)
				{
					str.len = 0;
					val_ubyte = prop_data[j];
					if (val_ubyte < 32 || val_ubyte > 127 || val_ubyte =='"' || val_ubyte =='\\')
					{
						dtStrConcatChars(&str, conf->NonAsciiChrOpen);
						dtStrConcatULong(&str, val_ubyte, conf->NonAsciiChrFormatID);
						dtStrConcatChars(&str, conf->NonAsciiChrClose);
					}
					else
					{
						//DF_PDX_SPRINTF(str, "%c", val_ubyte);
						dtStrConcatULong(&str, val_ubyte, 5);
					}
					pdxStringWriteToStream(str.str, stream);
				}
				pdxStringWriteToStream(quote, stream);
			}
			else
			{
				/* Hex String */
				DF_PDX_STRCPY(quote, "\"");
				pdxStringWriteToStream(quote, stream);
				for (j = 0; j < prop_len; j++)
				{
					val_ubyte = prop_data[j];
					pdMakeHexFromUByte1(hex_data, val_ubyte, 0);
					pdxStringWriteToStream(hex_data, stream); /*DF_PDX_SPRINTF(str, "%02X", val_ubyte);*/
				}
				pdxStringWriteToStream(quote, stream);
			}

			if (use_name_enums == 1 && format[0] == 'P' && format[1] == 'D') format[1] = 'd';

			if (conf->OmitTypes == 0 || compress_len > 0)
			{
				//DF_PDX_SPRINTF(str, ", %s%s", format_enum, format);
				str.len = 0;
				dtStrConcatChars(&str, ", ");
				dtStrConcatChars(&str, format_enum);
				dtStrConcatChars(&str, format);
				pdxStringWriteToStream(str.str, stream);
			}

			pdxStringWriteToStream(conf->PropAddClose, stream);
			if (state == 0) { pdxStringWriteToStream(conf->CmdEnd, stream); pdxStringWriteToStream(conf->NewLine, stream); }
		}
		else if (type == PD_FT_LINK)
		{
			/* Link */
			if (conf->LinkAddOpen[0] == '@')
			{
				if (state == 0)
				{
					state = 1;
					dtStrConcatChars(&str, conf->Tab);
					dtStrConcatChars(&str, "obj");
					dtStrConcatChars(&str, conf->ArrBracketOpen);
					dtStrConcatSLong(&str, obj);
					dtStrConcatChars(&str, conf->ArrBracketClose);
				}
				dtStrConcatChars(&str, conf->LinkAddOpen + 1);
				//dtStrConcatChars(&str, ", ");
			}
			else if (conf->LinkAddOpen[0] != 0)
			{
				//DF_PDX_SPRINTF(str, "%s%sobj%s%ld%s, ", conf->Tab, conf->LinkAddOpen, conf->ArrBracketOpen, obj, conf->ArrBracketClose);
				dtStrConcatChars(&str, conf->Tab);
				dtStrConcatChars(&str, conf->LinkAddOpen);
				dtStrConcatChars(&str, "obj");
				dtStrConcatChars(&str, conf->ArrBracketOpen);
				dtStrConcatSLong(&str, obj);
				dtStrConcatChars(&str, conf->ArrBracketClose);
				dtStrConcatChars(&str, ", ");
			}
			else
			{
				DF_PDX_STRCPY(quote, "");
				DF_PDX_STRCPY(divider, ":");
			}

			//DF_PDX_SPRINTF(str, "%s%s%s ", name_enum, field_name, divider);
			dtStrConcatChars(&str, name_enum);
			dtStrConcatChars(&str, field_name);
			dtStrConcatChars(&str, divider);
			dtStrConcatChars(&str, " ");

			if ((link = pdLinkGet(power_doc, obj, prop_name)) == -1)
			{
				//DF_PDX_SPRINTF(str, "-1");
				dtStrConcatChars(&str, "-1");
			}
			else
			{
				//DF_PDX_SPRINTF(str, "obj%s%ld%s", conf->ArrBracketOpen, link, conf->ArrBracketClose);
				dtStrConcatChars(&str, "obj");
				dtStrConcatChars(&str, conf->ArrBracketOpen);
				dtStrConcatSLong(&str, link);
				dtStrConcatChars(&str, conf->ArrBracketClose);
			}

			pdxStringWriteToStream(str.str, stream);
			pdxStringWriteToStream(conf->LinkAddClose, stream);
			if (state == 0) { pdxStringWriteToStream(conf->CmdEnd, stream); pdxStringWriteToStream(conf->NewLine, stream); }
		}
		else if (type == PD_FT_PROPERTYREFERENCE)
		{
			/* Property Reference */
			if (conf->PropAddAsRefOpen[0] == '@')
			{
				if (state == 0)
				{
					state = 1;
					dtStrConcatChars(&str, conf->Tab);
					dtStrConcatChars(&str, "obj");
					dtStrConcatChars(&str, conf->ArrBracketOpen);
					dtStrConcatSLong(&str, obj);
					dtStrConcatChars(&str, conf->ArrBracketClose);
				}
				dtStrConcatChars(&str, conf->PropAddAsRefOpen + 1);
				//dtStrConcatChars(&str, ", ");
			}
			else if (conf->PropAddAsRefOpen[0] != 0)
			{
				//DF_PDX_SPRINTF(str, "%s%sobj%s%ld%s, ", conf->Tab, conf->PropAddAsRefOpen, conf->ArrBracketOpen, obj, conf->ArrBracketClose);
				dtStrConcatChars(&str, conf->Tab);
				dtStrConcatChars(&str, conf->PropAddAsRefOpen);
				dtStrConcatChars(&str, "obj");
				dtStrConcatChars(&str, conf->ArrBracketOpen);
				dtStrConcatSLong(&str, obj);
				dtStrConcatChars(&str, conf->ArrBracketClose);
				dtStrConcatChars(&str, ", ");
			}
			else
			{
				DF_PDX_STRCPY(quote, "");
				DF_PDX_STRCPY(divider, ":");
			}

			//DF_PDX_SPRINTF(str, "%s%s%s ", name_enum, field_name, divider);
			dtStrConcatChars(&str, name_enum);
			dtStrConcatChars(&str, field_name);
			dtStrConcatChars(&str, divider);
			dtStrConcatChars(&str, " ");

			link = pdPropGetAsRef(power_doc, obj, prop_name, &target);
			if (pdxStringEnumGet(buffer.str, "config/field.inf", target) == 0) { buffer.len = 0; dtStrConcatSLong(&buffer, target); }

			field_name = buffer.str;
			if (field_name[0] == '>') field_name++;
			if (use_name_enums == 1 && field_name[0] == 'p' && field_name[1] == 'd') field_name[0] = 'P';

			if (link == -1)
			{
				//DF_PDX_SPRINTF(str, "-1");
				dtStrConcatChars(&str, "-1");
			}
			else
			{
				//DF_PDX_SPRINTF(str, "obj%s%ld%s, %s%s", conf->ArrBracketOpen, link, conf->ArrBracketClose, name_enum, field_name);
				dtStrConcatChars(&str, "obj");
				dtStrConcatChars(&str, conf->ArrBracketOpen);
				dtStrConcatSLong(&str, link);
				dtStrConcatChars(&str, conf->ArrBracketClose);
				dtStrConcatChars(&str, ", ");
				dtStrConcatChars(&str, name_enum);
				dtStrConcatChars(&str, field_name);
			}

			pdxStringWriteToStream(str.str, stream);
			pdxStringWriteToStream(conf->PropAddAsRefClose, stream);
			if (state == 0) { pdxStringWriteToStream(conf->CmdEnd, stream); pdxStringWriteToStream(conf->NewLine, stream); }
		}
	}

	if (t == PD_FT_LINK) { t = PD_FT_PROPERTYREFERENCE; goto loop; }
	if (t == PD_FT_PROPERTYREFERENCE) { t = PD_FT_PROPERTY; goto loop; }
	if (t == PD_FT_PROPERTY) { t = PD_FT_EXTPROPERTY_BUFFER; goto loop; }
	if (t == PD_FT_EXTPROPERTY_BUFFER) { t = PD_FT_EXTPROPERTY_CALLBACK; goto loop; }

	if (state == 1) { pdxStringWriteToStream(conf->CmdEnd, stream); pdxStringWriteToStream(conf->NewLine, stream); }

	pdxStringWriteToStream(conf->NewLine, stream);
	return 1;
}


DT_SLONG pdxObjExport2(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_ID_SLONG obj, DT_UBYTE use_name_enums, const DT_PDX_EXPORT_CONFIG* conf)
{
	DT_DTSTREAM stream;
	DT_SLONG len = -1;

	DT_STREAM_FILE(sd, file_name);
	if ((stream = dtStreamOpenW(&sd, DV_STREAM_OPEN_MUST, 0)) == DV_NULL) return len;
	pdxObjExport(power_doc, stream, obj, use_name_enums, conf);
	len = dtStreamLoc(stream);
	dtStreamClose(stream, DV_STREAM_CLOSE_YES);

	return len;
}


DT_SLONG pdxObjAllExport(DT_PDDOC power_doc, DT_DTSTREAM stream, DT_UBYTE use_name_enums, const DT_PDX_EXPORT_CONFIG* conf)
{
	const DT_CHAR* obj_desc = DV_NULL;
	DT_STRING_CHARS(str, PDX_MAXTEXTLENGTH, "");
	DT_SLONG obj, obj_name = 0, obj_last = pdObjGetLast(power_doc);

	//DF_PDX_SPRINTF(str, "%s%s PART 1: Defining objects and their properties %s%s", conf->Tab, conf->CommentOpen, conf->CommentClose, conf->NewLine);
	pdxStringWriteToStream(conf->Tab, stream);
	pdxStringWriteToStream(conf->CommentOpen, stream);
	pdxStringWriteToStream(" PART 1: Defining objects and their properties ", stream);
	pdxStringWriteToStream(conf->CommentClose, stream);
	pdxStringWriteToStream(conf->NewLine, stream);

	if (obj_last < 0)
	{
		//DF_PDX_SPRINTF(str, "%s%s There are no objects in this document %s%s", conf->Tab, conf->CommentOpen, conf->CommentClose, conf->NewLine);
		pdxStringWriteToStream(conf->Tab, stream);
		pdxStringWriteToStream(conf->CommentOpen, stream);
		pdxStringWriteToStream(" There are no objects in this document ", stream);
		pdxStringWriteToStream(conf->CommentClose, stream);
		pdxStringWriteToStream(conf->NewLine, stream);
		return 0;
	}

	//DF_PDX_SPRINTF(str, "%s%s %ld PowerDoc Object(s) %s%s", conf->Tab, conf->CommentOpen, obj_last + 1, conf->CommentClose, conf->NewLine);
	dtStrConcatChars(&str, conf->Tab);
	dtStrConcatChars(&str, conf->CommentOpen);
	dtStrConcatChars(&str, " ");
	dtStrConcatSLong(&str, obj_last + 1);
	dtStrConcatChars(&str, " PowerDoc Object(s) ");
	dtStrConcatChars(&str, conf->CommentClose);
	dtStrConcatChars(&str, conf->NewLine);

	//DF_PDX_SPRINTF(str, "%s%sobj%s%ld%s%s%s%s", conf->Tab, conf->ArrObjectDeclaration1, conf->ArrObjectDeclaration2, obj_last + 1 + conf->ArrDimAdd, conf->ArrObjectDeclaration3, conf->CmdEnd, conf->NewLine, conf->NewLine);
	dtStrConcatChars(&str, conf->Tab);
	dtStrConcatChars(&str, conf->ArrObjectDeclaration1);
	dtStrConcatChars(&str, "obj");
	dtStrConcatChars(&str, conf->ArrObjectDeclaration2);
	dtStrConcatSLong(&str, obj_last + 1 + conf->ArrDimAdd);
	dtStrConcatChars(&str, conf->ArrObjectDeclaration3);
	dtStrConcatChars(&str, conf->CmdEnd);
	dtStrConcatChars(&str, conf->NewLine);
	dtStrConcatChars(&str, conf->NewLine);

	pdxStringWriteToStream(str.str, stream);

	for (obj = 0; obj <= obj_last; obj++)
	{
		str.len = 0;
		pdObjGet(power_doc, obj, &obj_name, &obj_desc);

		//DF_PDX_SPRINTF(str, "%sobj%s%ld%s = %s%ld, \"%s\"%s%s%s", conf->Tab, conf->ArrBracketOpen, obj, conf->ArrBracketClose, conf->ObjAddOpen, obj_name, obj_desc, conf->ObjAddClose, conf->CmdEnd, conf->NewLine);
		dtStrConcatChars(&str, conf->Tab);
		dtStrConcatChars(&str, "obj");
		dtStrConcatChars(&str, conf->ArrBracketOpen);
		dtStrConcatSLong(&str, obj);
		dtStrConcatChars(&str, conf->ArrBracketClose);
		dtStrConcatChars(&str, " = ");

		if (conf->ObjAddOpen[0] == '@')
		{
			dtStrConcatChars(&str, conf->ObjAddOpen + 1);
			if (obj_name != 0 || (obj_desc != DV_NULL && obj_desc[0] != 0))
			{
				dtStrConcatChars(&str, "\"");
				dtStrConcatChars(&str, obj_desc);
				dtStrConcatChars(&str, "\"");
				if (obj_name != 0) { dtStrConcatChars(&str, ", "); dtStrConcatSLong(&str, obj_name); }
			}
		}
		else
		{
			dtStrConcatChars(&str, conf->ObjAddOpen);
			dtStrConcatSLong(&str, obj_name);
			if (obj_desc != DV_NULL)
			{
				dtStrConcatChars(&str, ", \"");
				dtStrConcatChars(&str, obj_desc);
				dtStrConcatChars(&str, "\"");
			}
			else if (conf->OmitOptional == 0)
			{
				dtStrConcatChars(&str, ", ");
				dtStrConcatChars(&str, conf->NullValue);
			}
		}
		dtStrConcatChars(&str, conf->ObjAddClose);

		dtStrConcatChars(&str, conf->CmdEnd);
		dtStrConcatChars(&str, conf->NewLine);
		pdxStringWriteToStream(str.str, stream);
	}

	pdxStringWriteToStream(conf->NewLine, stream);

	for (obj = 0; obj <= obj_last; obj++) pdxObjExport(power_doc, stream, obj, use_name_enums, conf);

	return 1;
}


/* Operations on selected PowerImages */


void pdxImgGetXYZoomed(DT_PDDOC power_doc, DT_SLONG img, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SLONG* x_zoom, DT_SLONG* y_zoom)
{
	DT_SLONG img_x = 0, img_y = 0;

	pdImgGetXY(power_doc, img, &img_x, &img_y);

	if (x_zoom != DV_NULL) *x_zoom = DF_ROUND(img_x * zoom) - x;
	if (y_zoom != DV_NULL) *y_zoom = DF_ROUND(img_y * zoom) - y;
}


DT_SLONG pdxImgFindViaObj(DT_PDDOC power_doc, DT_ID_SLONG obj)
{
	DT_SLONG img, img_last = pdImgGetLast(power_doc);

	for (img = 0; img <= img_last; img++) { if (pdImgGetObj(power_doc, img) == obj) return img; }
	return -1;
}


DT_SLONG pdxImgFindClosest(DT_PDDOC power_doc, DT_SLONG hit_x, DT_SLONG hit_y, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y)
{
	DT_SLONG img, x_zoom, y_zoom;
	DT_SLONG img_last = pdImgGetLast(power_doc);
	DT_SLONG s = pdDocGetNumericValue(power_doc, PD_NVAL_SIZE_ORIGIN);
	//DT_PD_DOC_PARAMS params; pdDocGetParams(power_doc, &params, 0); DT_SLONG s = params.SizeOrigin;

	for (img = img_last; img >= 0; img--)
	{
		if (pdImgGetPage(power_doc, img) != page) continue;

		pdxImgGetXYZoomed(power_doc, img, zoom, x, y, &x_zoom, &y_zoom);

		if (hit_x >= x_zoom - s && hit_x <= x_zoom + s && hit_y >= y_zoom - s && hit_y <= y_zoom + s) return img;
	}

	return -1;
}


DT_SLONG pdxImgSelSelectByPoint(DT_PDDOC power_doc, DT_SLONG x, DT_SLONG y, DT_SLONG page, DT_FLOAT zoom)
{
	DT_SLONG img = pdxImgFindClosest(power_doc, x, y, page, zoom, 0, 0);

	if (img != -1)
	{
		/* Select/Deselect an image */
		if (pdImgGetStatus(power_doc, img) == 0) pdImgSetStatus(power_doc, img, 1);
		else pdImgSetStatus(power_doc, img, 0);
		return img;
	}
	else
	{
		/* Deselect all images */
		for (img = 0; img <= pdImgGetLast(power_doc); img++) pdImgSetStatus(power_doc, img, 0);
		return -1;
	}
}


DT_SLONG pdxImgSelSelectByRect(DT_PDDOC power_doc, DT_SLONG x1, DT_SLONG y1, DT_SLONG x2, DT_SLONG y2, DT_SLONG page, DT_FLOAT zoom)
{
	DT_SLONG img;
	DT_SLONG x, y;
	DT_SLONG tmp, ret = 0;

	if (x2 < x1) { tmp = x1; x1 = x2; x2 = tmp; }
	if (y2 < y1) { tmp = y1; y1 = y2; y2 = tmp; }

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetPage(power_doc, img) != page) continue;

		pdxImgGetXYZoomed(power_doc, img, zoom, 0, 0, &x, &y);
		if (x >= x1 && y >= y1 && x <= x2 && y <= y2)
		{
			pdImgSetStatus(power_doc, img, 1);
			ret = 1;
		}
	}

	return ret;
}


DT_SLONG pdxImgSelSelectByImgRange(DT_PDDOC power_doc, DT_SLONG img_min, DT_SLONG img_max)
{
	DT_SLONG img;

	for (img = img_min; img <= img_max; img++) pdImgSetStatus(power_doc, img, 1);
	return 1;
}


DT_SLONG pdxImgSelSelectByPageRange(DT_PDDOC power_doc, DT_SLONG page_from, DT_SLONG page_to)
{
	DT_SLONG page, img, ret = 0;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if ((page = pdImgGetPage(power_doc, img)) >= page_from && page <= page_to)
		{
			pdImgSetStatus(power_doc, img, 1);
			ret = 1;
		}
	}

	return ret;
}


DT_SLONG pdxImgSelCount(DT_PDDOC power_doc)
{
	DT_SLONG img, count = 0;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetStatus(power_doc, img) == 1) count++;
	}

	return count;
}


DT_SLONG pdxImgSelMove(DT_PDDOC power_doc, DT_SLONG dx, DT_SLONG dy)
{
	DT_SLONG img, x, y;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;

		pdImgGetXY(power_doc, img, &x, &y);
		pdImgSetXY(power_doc, img, x + dx, y + dy);
	}

	return 1;
}


DT_SLONG pdxImgSelResetTransform(DT_PDDOC power_doc)
{
	DT_SLONG img, x, y, x2, y2;
	DT_SLONG x_min = 0, y_min = 0;
	DT_FLOAT a, b, c, d, e, f, r;
	DT_TM3X3 t;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;

		pdImgGetXY(power_doc, img, &x, &y);
		pdImgGetTransformPlus(power_doc, img, t);

		a = /* t[2][1] * X */ - t[0][1];
		b = /* t[2][0] * Y */ - t[1][0];
		c = /* t[2][0] * X */ - t[0][0];
		d = /* t[2][1] * Y */ - t[1][1];
		e = t[2][2] * x /* - t[0][2] */;
		f = t[2][2] * y /* - t[1][2] */;
		r = a * b - c * d;

		if (DF_ISEQUAL(r, 0.0)) {}
		else
		{
			x2 = DF_ROUND((e * d - a * f) / r);
			y2 = DF_ROUND((c * f - e * b) / r);
			pdImgSetXY(power_doc, img, x2, y2);
			if (x2 < x_min) x_min = x2;
			if (y2 < y_min) y_min = y2;
		}

		pdImgSetTransformPlus(power_doc, img, DV_NULL);
	}

	pdxImgSelMove(power_doc, -x_min, -y_min);

	return 1;
}


DT_SLONG pdxImgSelDelete(DT_PDDOC power_doc)
{
	pdImgDelete(power_doc, 1);
	return 1;
}


DT_SLONG pdxImgSelMoveToPage(DT_PDDOC power_doc, DT_SLONG page)
{
	pdImgMoveToPage(power_doc, 1, page);
	return 1;
}


DT_SLONG pdxImgSelHide(DT_PDDOC power_doc)
{
	//pdImgChangeAttribs(power_doc, 1, 1);

	DT_SLONG img, img_last = pdImgGetLast(power_doc);
	for (img = 0; img <= img_last; img++) if (pdImgGetStatus(power_doc, img) == 1) pdImgSetAttribs(power_doc, img, pdImgGetAttribs(power_doc, img) | 1);
	return 1;
}


DT_SLONG pdxImgSelUnhide(DT_PDDOC power_doc)
{
	//pdImgChangeAttribs(power_doc, 1, 0);

	DT_SLONG img, img_last = pdImgGetLast(power_doc);
	for (img = 0; img <= img_last; img++) if (pdImgGetStatus(power_doc, img) == 1) pdImgSetAttribs(power_doc, img, pdImgGetAttribs(power_doc, img) & 254);
	return 1;
}


DT_SLONG pdxImgSelGroup(DT_PDDOC power_doc, DT_SLONG page_group, DT_SLONG x_group, DT_SLONG y_group, DT_SWORD bit_flags)
{
	DT_UBYTE attribs, use_frac = 0;
	DT_SWORD name, fn_draw;
	DT_SLONG img, img_last = pdImgGetLast(power_doc);
	DT_SLONG member_count = 0;
	DT_SLONG mask, x, y;
	DT_ID_SLONG obj, obj_member = -1;
	DT_TM3X3 transform = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
	DT_UBYTE buffer[8 * sizeof(DT_SLONG) + 8 * sizeof(DT_FLOAT) + 1];
	DT_UBYTE* t;

	DT_ID_SLONG obj_group = pdObjAdd(power_doc, 0, "Group");
	DT_ID_SLONG obj_last_member = obj_group;

	/* Make group members from selected images */
	for (img = 0; img <= img_last; img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;

		obj = pdImgGetObj(power_doc, img);
		fn_draw = pdImgGetFnDraw(power_doc, img);
		mask = pdImgGetMask(power_doc, img);
		name = pdImgGetName(power_doc, img);
		attribs = pdImgGetAttribs(power_doc, img);
		pdImgGetXY(power_doc, img, &x, &y);
		pdImgGetTransformPlus(power_doc, img, transform);

		obj_member = pdObjAdd(power_doc, 0, "Group Member");
		pdLinkAdd(power_doc, obj_last_member, pdGroupMemberPtr, obj_member);

		if (member_count == 0 && (bit_flags & 1))
		{
			page_group = pdImgGetPage(power_doc, img);
			x_group = x;
			y_group = y;
		}

		x -= x_group;
		y -= y_group;

		if ((bit_flags & 2))
		{
			transform[0][0] += x * transform[2][0];
			transform[1][0] += y * transform[2][0];
			transform[0][1] += x * transform[2][1];
			transform[1][1] += y * transform[2][1];
			transform[0][2] += x * transform[2][2];
			transform[1][2] += y * transform[2][2];

			x = 0;
			y = 0;
		}

		pdLinkAdd(power_doc, obj_member, pdFnDrawObjectPtr, obj);
		pdPropAddAsSWord(power_doc, obj_member, pdFnDraw, fn_draw);
		if (attribs != 0) pdPropAddAsUByte(power_doc, obj_member, pdAttribs, attribs);
		if (name != 0) pdPropAddAsSWord(power_doc, obj_member, pdName, name);
		if (mask != 0) pdPropAddAsSLong(power_doc, obj_member, pdMask, mask);
		if (x != 0) pdPropAddAsSLong(power_doc, obj_member, pdDx, x);
		if (y != 0) pdPropAddAsSLong(power_doc, obj_member, pdDy, y);

		DT_UBYTE omit_part2 = (DF_ISEQUAL(transform[0][2], 0.0) && DF_ISEQUAL(transform[1][2], 0.0) && DF_ISEQUAL(transform[2][0], 0.0) && DF_ISEQUAL(transform[2][1], 0.0));
		DT_UBYTE omit_trans = omit_part2 && (DF_ISEQUAL(transform[0][0], 1.0) && DF_ISEQUAL(transform[0][1], 0.0) && DF_ISEQUAL(transform[1][0], 0.0) && DF_ISEQUAL(transform[1][1], 1.0));

		if (omit_trans && DF_ISEQUAL(transform[2][2], 1.0)) {}
		else if (use_frac)
		{
			t = buffer;
			DF_WRITE_SLONG_LE(t, DF_ROUND(transform[0][0] * 262144)); t += sizeof(DT_SLONG);
			DF_WRITE_SLONG_LE(t, DF_ROUND(transform[0][1] * 262144)); t += sizeof(DT_SLONG);
			DF_WRITE_SLONG_LE(t, DF_ROUND(transform[1][0] * 262144)); t += sizeof(DT_SLONG);
			DF_WRITE_SLONG_LE(t, DF_ROUND(transform[1][1] * 262144)); t += sizeof(DT_SLONG);

			if (omit_part2) {}
			else
			{
				DF_WRITE_SLONG_LE(t, DF_ROUND(transform[0][2] * 262144)); t += sizeof(DT_SLONG);
				DF_WRITE_SLONG_LE(t, DF_ROUND(transform[1][2] * 262144)); t += sizeof(DT_SLONG);
				DF_WRITE_SLONG_LE(t, DF_ROUND(transform[2][0] * 262144)); t += sizeof(DT_SLONG);
				DF_WRITE_SLONG_LE(t, DF_ROUND(transform[2][1] * 262144)); t += sizeof(DT_SLONG);
			}

			pdPropAdd(power_doc, obj_member, pdTransformMatrix, buffer, static_cast<DT_SLONG>(t - buffer));
		}
		else /* much better precision and range */
		{
			t = buffer;
			DF_WRITE_FLOAT_LE(t, transform[0][0]); t += sizeof(DT_FLOAT);
			DF_WRITE_FLOAT_LE(t, transform[0][1]); t += sizeof(DT_FLOAT);
			DF_WRITE_FLOAT_LE(t, transform[1][0]); t += sizeof(DT_FLOAT);
			DF_WRITE_FLOAT_LE(t, transform[1][1]); t += sizeof(DT_FLOAT);

			if (omit_part2) {}
			else
			{
				DF_WRITE_FLOAT_LE(t, transform[0][2]); t += sizeof(DT_FLOAT);
				DF_WRITE_FLOAT_LE(t, transform[1][2]); t += sizeof(DT_FLOAT);
				DF_WRITE_FLOAT_LE(t, transform[2][0]); t += sizeof(DT_FLOAT);
				DF_WRITE_FLOAT_LE(t, transform[2][1]); t += sizeof(DT_FLOAT);
			}

			*t = 0; t++; /* +1 */

			pdPropAdd(power_doc, obj_member, pdTransformMatrix, buffer, static_cast<DT_SLONG>(t - buffer));
		}

		obj_last_member = obj_member;
		member_count++;
	}

	/* Delete selected images */
	pdxImgSelDelete(power_doc);

	/* Add group image */
	/*img =*/ pdImgAddPlus(power_doc, obj_group, page_group, x_group, y_group, 1, 0, 0, pdDrawGroup, DV_NULL);

	/* Select created group image */
	/* pdxImgSelSelectByImgRange(power_doc, img, img); */

	return 1;
}


DT_SLONG pdxImgSelUngroup(DT_PDDOC power_doc)
{
	DT_UBYTE attribs_member;
	DT_SWORD name_member, fn_draw_member;
	DT_SLONG len;
	DT_SLONG page;
	DT_SLONG img, img_member = -1, img_last = pdImgGetLast(power_doc);
	DT_SLONG x, y, x_member, y_member, mask_member;
	DT_SLONG nr_of_created = 0, nr_of_removed = 0;
	DT_ID_SLONG obj, obj_member;
	DT_FLOAT one_over_262144 = 0.000003814697265625;
	DT_UBYTE* t;

	/* Make images from selected group objects */
	for (img = 0; img <= img_last; img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;
		if (pdImgGetFnDraw(power_doc, img) != pdDrawGroup)
		{
			/* This is not a group object. Deselect and skip this image. */
			pdImgSetStatus(power_doc, img, 0);
			continue;
		}

		pdImgGetXY(power_doc, img, &x, &y);
		page = pdImgGetPage(power_doc, img);
		obj = pdImgGetObj(power_doc, img);

		for (;;)
		{
			if ((obj = pdLinkGet(power_doc, obj, pdGroupMemberPtr)) == -1) break;
			else if ((obj_member = pdLinkGet(power_doc, obj, pdFnDrawObjectPtr)) == -1) continue;

			fn_draw_member = pdPropGetAsSWord(power_doc, obj, pdFnDraw, pdDrawDefault);
			attribs_member = pdPropGetAsUByte(power_doc, obj, pdAttribs, 0);
			name_member = pdPropGetAsSWord(power_doc, obj, pdName, 0);
			mask_member = pdPropGetAsSLong(power_doc, obj, pdMask, 0);
			x_member = pdPropGetAsSLong(power_doc, obj, pdDx, 0);
			y_member = pdPropGetAsSLong(power_doc, obj, pdDy, 0);
			t = pdPropGet(power_doc, obj, pdTransformMatrix, &len);

			DT_TM3X3 transform = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

			if ((len == 1 + 4 * sizeof(DT_FLOAT)) || (len == 1 + 8 * sizeof(DT_FLOAT))) /* much better precision and range */
			{
				transform[0][0] = DF_READ_FLOAT_LE(t); t += sizeof(DT_FLOAT);
				transform[0][1] = DF_READ_FLOAT_LE(t); t += sizeof(DT_FLOAT);
				transform[1][0] = DF_READ_FLOAT_LE(t); t += sizeof(DT_FLOAT);
				transform[1][1] = DF_READ_FLOAT_LE(t); t += sizeof(DT_FLOAT);

				if (len == 1 + 8 * sizeof(DT_FLOAT))
				{
					transform[0][2] = DF_READ_FLOAT_LE(t); t += sizeof(DT_FLOAT);
					transform[1][2] = DF_READ_FLOAT_LE(t); t += sizeof(DT_FLOAT);
					transform[2][0] = DF_READ_FLOAT_LE(t); t += sizeof(DT_FLOAT);
					transform[2][1] = DF_READ_FLOAT_LE(t); t += sizeof(DT_FLOAT);
				}

				transform[2][2] = 1.0;

				img_member = pdImgAddPlus(power_doc, obj_member, page, x + x_member, y + y_member, 0, attribs_member, mask_member, fn_draw_member, transform);
			}
			else if ((len == 4 * sizeof(DT_SLONG)) || (len == 8 * sizeof(DT_SLONG)))
			{
				transform[0][0] = (DF_READ_SLONG_LE(t)) * one_over_262144; t += sizeof(DT_SLONG);
				transform[0][1] = (DF_READ_SLONG_LE(t)) * one_over_262144; t += sizeof(DT_SLONG);
				transform[1][0] = (DF_READ_SLONG_LE(t)) * one_over_262144; t += sizeof(DT_SLONG);
				transform[1][1] = (DF_READ_SLONG_LE(t)) * one_over_262144; t += sizeof(DT_SLONG);

				if (len == 8 * sizeof(DT_SLONG))
				{
					transform[0][2] = (DF_READ_SLONG_LE(t)) * one_over_262144; t += sizeof(DT_SLONG);
					transform[1][2] = (DF_READ_SLONG_LE(t)) * one_over_262144; t += sizeof(DT_SLONG);
					transform[2][0] = (DF_READ_SLONG_LE(t)) * one_over_262144; t += sizeof(DT_SLONG);
					transform[2][1] = (DF_READ_SLONG_LE(t)) * one_over_262144; t += sizeof(DT_SLONG);
				}

				transform[2][2] = 1.0;

				img_member = pdImgAddPlus(power_doc, obj_member, page, x + x_member, y + y_member, 0, attribs_member, mask_member, fn_draw_member, transform);
			}
			else
			{
				img_member = pdImgAddPlus(power_doc, obj_member, page, x + x_member, y + y_member, 0, attribs_member, mask_member, fn_draw_member, DV_NULL);
			}

			pdImgSetName(power_doc, img_member, name_member);
			nr_of_created++;
		}

		nr_of_removed++;
	}

	img_last += nr_of_created;

	/* Delete selected images */
	pdxImgSelDelete(power_doc);
	img_last -= nr_of_removed;

	/* Select created images */
	pdxImgSelSelectByImgRange(power_doc, img_last - nr_of_created + 1, img_last);

	return 1;
}


DT_SLONG pdxImgSelGroupOriginMove(DT_PDDOC power_doc, DT_SLONG dx, DT_SLONG dy)
{
	DT_SLONG img;
	DT_SLONG x_member, y_member;
	DT_ID_SLONG obj, obj_member;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;
		if (pdImgGetFnDraw(power_doc, img) != pdDrawGroup)
		{
			/* This is not a group object. Deselect and skip this image. */
			pdImgSetStatus(power_doc, img, 0);
			continue;
		}

		obj = pdImgGetObj(power_doc, img);

		for (;;)
		{
			if ((obj = pdLinkGet(power_doc, obj, pdGroupMemberPtr)) == -1) break;
			else if ((obj_member = pdLinkGet(power_doc, obj, pdFnDrawObjectPtr)) == -1) continue;

			x_member = dx + pdPropGetAsSLong(power_doc, obj, pdDx, 0);
			y_member = dy + pdPropGetAsSLong(power_doc, obj, pdDy, 0);

			pdPropApplyAsSLong(power_doc, obj, pdDx, x_member, 0, PD_APPLY_DEFVALUE);
			pdPropApplyAsSLong(power_doc, obj, pdDy, y_member, 0, PD_APPLY_DEFVALUE);
		}
	}

	return 1;
}


DT_SLONG pdxImgSelToTop(DT_PDDOC power_doc)
{
	pdImgToTop(power_doc, 1);
	return 1;
}


DT_SLONG pdxImgSelToBottom(DT_PDDOC power_doc)
{
	pdImgToBottom(power_doc, 1);
	return 1;
}


DT_SLONG pdxImgSelDuplicate(DT_PDDOC power_doc, DT_SLONG dst_page, DT_SLONG dx, DT_SLONG dy, DT_UBYTE src_status, DT_UBYTE dst_status)
{
	DT_SLONG img, img_new, img_last = pdImgGetLast(power_doc);
	DT_SLONG x, y;

	for (img = 0; img <= img_last; img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;

		pdImgGetXY(power_doc, img, &x, &y);
		pdImgSetStatus(power_doc, img, src_status);

		img_new = pdImgDup(power_doc, img);

		pdImgSetStatus(power_doc, img_new, dst_status);
		pdImgSetPage(power_doc, img_new, dst_page);
		pdImgSetXY(power_doc, img_new, x + dx, y + dy);
	}

	return 1;
}


DT_ID_SLONG pdxImgSelToPowerFont(DT_PDDOC power_doc, const DT_RECT_SLONG* extent, const DT_SLONG metrics_x_arr[], const DT_SLONG metrics_y_arr[], DT_SLONG arr_size, DT_SLONG* record_count)
{
	DT_SWORD fn_draw;
	DT_SLONG img, img_last = pdImgGetLast(power_doc);
	DT_SLONG n = 0, advance_width = 0;
	DT_ID_SLONG obj, obj_root = -1, obj_font_rec0 = -1, obj_font_rec, obj_font;
	DT_UBYTE buffer[4 * sizeof(DT_SLONG)];
	DT_UBYTE* b = buffer;

	if (extent == DV_NULL || metrics_x_arr == DV_NULL || metrics_y_arr == DV_NULL || arr_size < 3) return -1;

	/* PowerFont Records */
	for (img = 0; img <= img_last; img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;

		obj = pdImgGetObj(power_doc, img);
		fn_draw = pdImgGetFnDraw(power_doc, img);

		if (2 + n < arr_size) advance_width = metrics_x_arr[2 + n];

		obj_font_rec = pdObjAdd(power_doc, 0, "PowerFont Record");
		pdPropAddAsSLong(power_doc, obj_font_rec, pdDw, advance_width);
		pdPropAddAsSWord(power_doc, obj_font_rec, pdFnDraw, fn_draw);
		pdLinkAdd(power_doc, obj_font_rec, pdFnDrawObjectPtr, obj);

		if (obj_root == -1) obj_root = obj_font_rec;
		else pdLinkAdd(power_doc, obj_font_rec0, pdPowerFontRecordPtr, obj_font_rec);

		obj_font_rec0 = obj_font_rec;
		n++;
	}

	*record_count = n;

	if (n < 1) return -1;

	/* PowerFont Header */
	obj_font = pdObjAdd(power_doc, 0, "PowerFont");
	pdLinkAdd(power_doc, obj_font, pdPowerFontRecordPtr, obj_root);
	pdPropAddAsSLong(power_doc, obj_font, pdDw, metrics_x_arr[1]);
	pdPropAddAsSLong(power_doc, obj_font, pdDh, metrics_y_arr[1]);
	DF_WRITE_SLONG_LE(b,  extent->xmn); b += sizeof(DT_SLONG);
	DF_WRITE_SLONG_LE(b, -extent->ymx); b += sizeof(DT_SLONG);
	DF_WRITE_SLONG_LE(b,  extent->xmx); b += sizeof(DT_SLONG);
	DF_WRITE_SLONG_LE(b, -extent->ymn); b += sizeof(DT_SLONG);
	pdPropAdd(power_doc, obj_font, pdBoundingBox, buffer, (DT_SLONG)(b - buffer));

	return obj_font;
}


DT_ID_SLONG pdxImgSelToText(DT_PDDOC power_doc, const DT_RECT_SLONG* extent, const DT_SLONG metrics_x_arr[], const DT_SLONG metrics_y_arr[], DT_SLONG arr_size, DT_ID_SWORD sz)
{
	DT_UBYTE buffer[DV_SIZE_4 * 256];
	DT_SLONG i, record_count;
	DT_SLONG p = pd0;
	DT_ID_SLONG obj_0, obj_1, obj_2, obj_3;
	DT_ID_SLONG obj_font = pdxImgSelToPowerFont(power_doc, extent, metrics_x_arr, metrics_y_arr, arr_size, &record_count);
	DT_ULONG j = 0;

	if (record_count < 1) return -1; else if (record_count > 256) record_count = 256;

	if (sz == DV_SIZE_1)
	{
		p = pdTextString_08;
		for (i = 0; i < record_count; i++, j++) DF_WRITE_UBYTE_LE(buffer + i, (DT_UBYTE)j);
	}
	else if (sz == DV_SIZE_2)
	{
		p = pdTextString_16;
		for (i = 0; i < record_count; i++, j++) DF_WRITE_UWORD_LE(buffer + DV_SIZE_2 * i, (DT_UWORD)j);
	}
	else if (sz == DV_SIZE_3)
	{
		p = pdTextString_24;
		for (i = 0; i < record_count; i++, j++) DF_WRITE_UTRIO_LE(buffer + DV_SIZE_3 * i, j);
	}
	else if (sz == DV_SIZE_4)
	{
		p = pdTextString_32;
		for (i = 0; i < record_count; i++, j++) DF_WRITE_ULONG_LE(buffer + DV_SIZE_4 * i, j);
	}
	else return -1;

	obj_0 = pdObjAdd(power_doc, 0, "Text Fragment Body Color");
	pdPropAdd(power_doc, obj_0, pdRGBT, reinterpret_cast<const DT_UBYTE*>("00 00 00 00"), PD_HEX);

	obj_1 = pdObjAdd(power_doc, 0, "Text Fragment Typography");
	pdPropAddAsSLong(power_doc, obj_1, pdFontDw, metrics_x_arr[0]);
	pdPropAddAsSLong(power_doc, obj_1, pdFontDh, metrics_y_arr[0]);
	pdLinkAdd(power_doc, obj_1, pdPowerFontPtr, obj_font);
	pdLinkAdd(power_doc, obj_1, pdBodyPtr, obj_0);

	obj_2 = pdObjAdd(power_doc, 0, "Text Fragment");
	pdPropAdd(power_doc, obj_2, p, buffer, sz * record_count);
	pdPropAddAsUByte(power_doc, obj_2, pdTextEncoding, 3);
	pdLinkAdd(power_doc, obj_2, pdTypographyPtr, obj_1);

	obj_3 = pdObjAdd(power_doc, 0, "Text");
	pdLinkAdd(power_doc, obj_3, pdTextFragmentPtr, obj_2);

	return obj_3;
}


DT_SLONG pdxImgSelInvert(DT_PDDOC power_doc)
{
	DT_SLONG img;
	DT_UBYTE status;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetStatus(power_doc, img)) status = 0; else status = 1;
		pdImgSetStatus(power_doc, img, status);
	}

	return 1;
}


DT_SLONG pdxImgSelTransformPlus(DT_PDDOC power_doc, const DT_TM3X3 transform)
{
	DT_SLONG img, x, y, x2, y2;
	DT_SLONG x_min = 0, y_min = 0;
	DT_TM3X3 t, t2;
	/* DT_FLOAT w; */

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;

		pdImgGetTransformPlus(power_doc, img, t);
		pdImgGetXY(power_doc, img, &x, &y);

		/* w = x * transform[2][0] + y * transform[2][1] + transform[2][2]; */
		x2 = DF_ROUND((x * transform[0][0] + y * transform[0][1] + 0 * transform[0][2]) /* / w */);
		y2 = DF_ROUND((x * transform[1][0] + y * transform[1][1] + 0 * transform[1][2]) /* / w */);
		pdImgSetXY(power_doc, img, x2, y2);
		pdxMatrixMultiplyPlus(transform, t, t2);
		pdImgSetTransformPlus(power_doc, img, t2);
		if (x2 < x_min) x_min = x2;
		if (y2 < y_min) y_min = y2;
	}

	pdxImgSelMove(power_doc, -x_min, -y_min);

	return 1;
}


DT_SLONG pdxImgSelTransform(DT_PDDOC power_doc, const DT_TM2X2 transform)
{
	DT_SLONG img, x, y, x2, y2;
	DT_SLONG x_min = 0, y_min = 0;
	DT_TM2X2 t, t2;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetStatus(power_doc, img) != 1) continue;

		pdImgGetTransform(power_doc, img, t);
		pdImgGetXY(power_doc, img, &x, &y);
		x2 = DF_ROUND(x * transform[0][0] + y * transform[0][1]);
		y2 = DF_ROUND(x * transform[1][0] + y * transform[1][1]);
		pdImgSetXY(power_doc, img, x2, y2);
		pdxMatrixMultiply(transform, t, t2);
		pdImgSetTransform(power_doc, img, t2);
		if (x2 < x_min) x_min = x2;
		if (y2 < y_min) y_min = y2;
	}

	pdxImgSelMove(power_doc, -x_min, -y_min);

	return 1;
}


/* Operations on all PowerImages */


DT_SLONG pdxImgAllFindByName(DT_PDDOC power_doc, DT_SWORD name)
{
	DT_SLONG img, img_last = pdImgGetLast(power_doc);

	for (img = 0; img <= img_last; img++)
	{
		if (pdImgGetName(power_doc, img) != name) continue;
		return img;
	}

	return -1;
}


DT_SLONG pdxImgAllDeleteOnPageRange(DT_PDDOC power_doc, DT_SLONG page_from, DT_SLONG page_to)
{
	DT_SLONG p, img, count = 0, img_last = pdImgGetLast(power_doc);

	for (img = 0; img <= img_last; img++)
	{
		p = pdImgGetPage(power_doc, img); if (p < page_from || p > page_to) continue;
		pdImgDel(power_doc, img);
		count++; img--; img_last--;
	}

	return count;
}


DT_SLONG pdxImgAllDeleteOnPage(DT_PDDOC power_doc, DT_SLONG page)
{
	return pdxImgAllDeleteOnPageRange(power_doc, page, page);
}


DT_SLONG pdxImgAllCountByFilter(DT_PDDOC power_doc, DT_SLONG page, DT_UBYTE attribs, DT_SLONG fn_draw, DT_ID_SLONG obj)
{
	DT_SLONG img;
	DT_SLONG count = 0;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		if (pdImgGetPage(power_doc, img) != page) continue;
		if (pdImgGetAttribs(power_doc, img) != attribs) continue;
		if (pdImgGetFnDraw(power_doc, img) != fn_draw) continue;
		if (pdImgGetObj(power_doc, img) != obj) continue;
		count++;
	}

	return count;
}


DT_SLONG pdxImgAllSetTransformPlus(DT_PDDOC power_doc, const DT_TM3X3 transform)
{
	DT_SLONG img;
	DT_SLONG x, y, x2, y2;
	/* DT_FLOAT w; */

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		pdImgGetXY(power_doc, img, &x, &y);

		/* w = x * transform[2][0] + y * transform[2][1] + transform[2][2]; */

		x2 = DF_ROUND((x * transform[0][0] + y * transform[0][1] + transform[0][2]) /* / w */);
		y2 = DF_ROUND((x * transform[1][0] + y * transform[1][1] + transform[1][2]) /* / w */);
		pdImgSetXY(power_doc, img, x2, y2);
		pdImgSetTransformPlus(power_doc, img, transform);
	}

	return 1;
}


DT_SLONG pdxImgAllSetTransform(DT_PDDOC power_doc, const DT_TM2X2 transform)
{
	DT_SLONG img;
	DT_SLONG x, y, x2, y2;

	for (img = 0; img <= pdImgGetLast(power_doc); img++)
	{
		pdImgGetXY(power_doc, img, &x, &y);
		x2 = DF_ROUND(x * transform[0][0] + y * transform[0][1]);
		y2 = DF_ROUND(x * transform[1][0] + y * transform[1][1]);
		pdImgSetXY(power_doc, img, x2, y2);
		pdImgSetTransform(power_doc, img, transform);
	}

	return 1;
}


DT_SLONG pdxImgAllDeselect(DT_PDDOC power_doc)
{
	DT_SLONG img;

	for (img = 0; img <= pdImgGetLast(power_doc); img++) pdImgSetStatus(power_doc, img, 0);
	return 1;
}


DT_SLONG pdxImgAllExport(DT_PDDOC power_doc, DT_DTSTREAM stream, DT_UBYTE use_name_enums, const DT_PDX_EXPORT_CONFIG* conf)
{
	DT_STRING_CHARS(str, PDX_MAXTEXTLENGTH, "");
	DT_STRING_CHARS(buffer, PDX_MAXTEXTLENGTH, "");
	DT_CHAR function_enum[64] = "";
	DT_CHAR* func_name;
	DT_TM3X3 transform = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
	DT_TM3X3 transform_prev = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
	DT_UBYTE flag_transform = 0;
	DT_UBYTE flag_transform_prev = 0;
	DT_SLONG x, y;
	DT_SLONG fn_draw;
	DT_SLONG img, img_max = pdImgGetLast(power_doc);
	DT_SWORD tn, ti, tj;

	if (use_name_enums == 1)
	{
		DF_PDX_STRCPY(function_enum, "FunctionEnum.");
	}
	else if (use_name_enums == 2)
	{
		DF_PDX_STRCPY(function_enum, "doc.");
	}

	if (conf->ImgAddOpen[0] != '@') pdxStringWriteToStream(conf->NewLine, stream);

	//DF_PDX_SPRINTF(str, "%s%s PART 2: Image placement %s%s", conf->Tab, conf->CommentOpen, conf->CommentClose, conf->NewLine);
	pdxStringWriteToStream(conf->Tab, stream);
	pdxStringWriteToStream(conf->CommentOpen, stream);
	pdxStringWriteToStream(" PART 2: Image placement ", stream);
	pdxStringWriteToStream(conf->CommentClose, stream);
	pdxStringWriteToStream(conf->NewLine, stream);

	if (img_max < 0)
	{
		//DF_PDX_SPRINTF(str, "%s%s There are no PowerDoc images in this document %s%s", conf->Tab, conf->CommentOpen, conf->CommentClose, conf->NewLine);
		pdxStringWriteToStream(conf->Tab, stream);
		pdxStringWriteToStream(conf->CommentOpen, stream);
		pdxStringWriteToStream("There are no PowerDoc images in this document ", stream);
		pdxStringWriteToStream(conf->CommentClose, stream);
		pdxStringWriteToStream(conf->NewLine, stream);
		return 0;
	}

	//DF_PDX_SPRINTF(str, "%s%s %ld PowerDoc Image(s) %s%s", conf->Tab, conf->CommentOpen, img_max + 1, conf->CommentClose, conf->NewLine);
	dtStrConcatChars(&str, conf->Tab);
	dtStrConcatChars(&str, conf->CommentOpen);
	dtStrConcatChars(&str, " ");
	dtStrConcatSLong(&str, img_max + 1);
	dtStrConcatChars(&str, " PowerDoc Image(s)");
	dtStrConcatChars(&str, conf->CommentClose);
	dtStrConcatChars(&str, conf->NewLine);
	pdxStringWriteToStream(str.str, stream);

	for (img = 0; img <= img_max; img++)
	{
		pdImgGetXY(power_doc, img, &x, &y);
		pdImgGetTransformPlus(power_doc, img, transform);

		if (DF_ISEQUAL(transform[0][0], 1.0) && DF_ISEQUAL(transform[0][1], 0.0) && DF_ISEQUAL(transform[0][2], 0.0) && DF_ISEQUAL(transform[1][0], 0.0) && DF_ISEQUAL(transform[1][1], 1.0) && DF_ISEQUAL(transform[1][2], 0.0) && DF_ISEQUAL(transform[2][0], 0.0) && DF_ISEQUAL(transform[2][1], 0.0) && DF_ISEQUAL(transform[2][2], 1.0))
		{
			flag_transform = 0; /* no transformation matrix for this image */
		}
		else if (flag_transform_prev == 1 && DF_ISEQUAL(transform[0][0], transform_prev[0][0]) && DF_ISEQUAL(transform[0][1], transform_prev[0][1]) && DF_ISEQUAL(transform[0][2], transform_prev[0][2]) && DF_ISEQUAL(transform[1][0], transform_prev[1][0]) && DF_ISEQUAL(transform[1][1], transform_prev[1][1]) && DF_ISEQUAL(transform[1][2], transform_prev[1][2]) && DF_ISEQUAL(transform[2][0], transform_prev[2][0]) && DF_ISEQUAL(transform[2][1], transform_prev[2][1]) && DF_ISEQUAL(transform[2][2], transform_prev[2][2]))
		{
			flag_transform = 1; /* re-use previous transformation matrix */
		}
		else
		{
			flag_transform = 2; /* make new transformation matrix */
		}

		fn_draw = pdImgGetFnDraw(power_doc, img);
		if (pdxStringEnumGet(buffer.str, "config/fndraw.inf", fn_draw) == 0) { buffer.len = 0; dtStrConcatSLong(&buffer, fn_draw); }

		func_name = buffer.str;
		if (use_name_enums == 1 && func_name[0] == 'p' && func_name[1] == 'd') func_name[0] = 'P';

		pdxStringWriteToStream(conf->Tab, stream);

		if (flag_transform == 2 && conf->TransformViaMatrix == 1)
		{
			for (tn = 0, ti = 0; ti <= 2; ti++) for (tj = 0; tj <= 2; tj++, tn++)
			{
				//DF_PDX_SPRINTF(str, "transform%s%d%s%d%s = %f%s%s%s", conf->ArrBracketOpen, ti, conf->ArrDimSeparator, tj, conf->ArrBracketClose, transform[ti][tj], conf->CmdEnd, conf->NewLine, conf->Tab);
				str.len = 0;
				dtStrConcatChars(&str, "transform");
				dtStrConcatChars(&str, conf->ArrBracketOpen);

				if (conf->ArrDimSeparator == DV_NULL) dtStrConcatSLong(&str, tn);
				else
				{
					dtStrConcatSLong(&str, ti);
					dtStrConcatChars(&str, conf->ArrDimSeparator);
					dtStrConcatSLong(&str, tj);
				}

				dtStrConcatChars(&str, conf->ArrBracketClose);
				dtStrConcatChars(&str, " = ");
				dtStrConcatFloat(&str, transform[ti][tj]);
				dtStrConcatChars(&str, conf->CmdEnd);
				dtStrConcatChars(&str, conf->NewLine);
				dtStrConcatChars(&str, conf->Tab);
				pdxStringWriteToStream(str.str, stream);
				transform_prev[ti][tj] = transform[ti][tj];
			}
			flag_transform_prev = 1;
		}

		str.len = 0;

		if (conf->ImgAddOpen[0] == '@')
		{
				dtStrConcatChars(&str, "obj");
				dtStrConcatChars(&str, conf->ArrBracketOpen);
				dtStrConcatSLong(&str, pdImgGetObj(power_doc, img));
				dtStrConcatChars(&str, conf->ArrBracketClose);
				dtStrConcatChars(&str, conf->ImgAddOpen + 1);
		}
		else
		{
			dtStrConcatChars(&str, conf->ImgAddOpen);

			//DF_PDX_SPRINTF(str, "obj%s%ld%s, %ld, %ld, %ld, %d, %d, %ld, %s%s", conf->ArrBracketOpen, pdImgGetObj(power_doc, img), conf->ArrBracketClose, pdImgGetPage(power_doc, img), x, y, pdImgGetStatus(power_doc, img), pdImgGetAttribs(power_doc, img), pdImgGetMask(power_doc, img), function_enum, func_name);
			dtStrConcatChars(&str, "obj");
			dtStrConcatChars(&str, conf->ArrBracketOpen);
			dtStrConcatSLong(&str, pdImgGetObj(power_doc, img));
			dtStrConcatChars(&str, conf->ArrBracketClose);
			dtStrConcatChars(&str, ", ");
		}

		dtStrConcatSLong(&str, pdImgGetPage(power_doc, img));
		dtStrConcatChars(&str, ", ");
		dtStrConcatSLong(&str, x);
		dtStrConcatChars(&str, ", ");
		dtStrConcatSLong(&str, y);

		if (conf->ImgAddOpen[0] == '@')
		{
			dtStrConcatChars(&str, ", ");
			dtStrConcatChars(&str, function_enum);
			dtStrConcatChars(&str, func_name);

			DT_UBYTE status_attrib_mask = 1;

			if (flag_transform == 1 || flag_transform == 2) {} else if (conf->OmitOptional == 0) {}
			else if (pdImgGetStatus(power_doc, img) == 0 && pdImgGetAttribs(power_doc, img) == 0 && pdImgGetMask(power_doc, img) == 0) status_attrib_mask = 0;

			if (status_attrib_mask)
			{
				dtStrConcatChars(&str, ", ");
				dtStrConcatSLong(&str, pdImgGetStatus(power_doc, img));
				dtStrConcatChars(&str, ", ");
				dtStrConcatSLong(&str, pdImgGetAttribs(power_doc, img));
				dtStrConcatChars(&str, ", ");
				dtStrConcatSLong(&str, pdImgGetMask(power_doc, img));
			}
		}
		else
		{
			dtStrConcatChars(&str, ", ");
			dtStrConcatSLong(&str, pdImgGetStatus(power_doc, img));
			dtStrConcatChars(&str, ", ");
			dtStrConcatSLong(&str, pdImgGetAttribs(power_doc, img));
			dtStrConcatChars(&str, ", ");
			dtStrConcatSLong(&str, pdImgGetMask(power_doc, img));

			dtStrConcatChars(&str, ", ");
			dtStrConcatChars(&str, function_enum);
			dtStrConcatChars(&str, func_name);
		}

		if (flag_transform == 1 || flag_transform == 2)
		{
			if (conf->TransformViaMatrix == 1)
			{
				//DF_PDX_SPRINTF(str, ", transform");
				dtStrConcatChars(&str, ", transform");
			}
			else
			{
				//DF_PDX_SPRINTF(str, ", %f, %f, %f, %f, %f, %f, %f, %f, %f", transform[0][0], transform[0][1], transform[0][2], transform[1][0], transform[1][1], transform[1][2], transform[2][0], transform[2][1], transform[2][2]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[0][0]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[0][1]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[0][2]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[1][0]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[1][1]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[1][2]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[2][0]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[2][1]);
				dtStrConcatChars(&str, ", ");
				dtStrConcatFloat(&str, transform[2][2]);
			}
		}
		else if (conf->OmitOptional == 0)
		{
			if (conf->TransformViaMatrix == 1)
			{
				//DF_PDX_SPRINTF(str, ", %s", conf->NullValue);
				dtStrConcatChars(&str, ", ");
				dtStrConcatChars(&str, conf->NullValue);
			}
			else
			{
				//DF_PDX_SPRINTF(str, ", 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0");
				dtStrConcatChars(&str, ", 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0");
			}
		}

		pdxStringWriteToStream(str.str, stream);
		pdxStringWriteToStream(conf->ImgAddClose, stream);
		pdxStringWriteToStream(conf->CmdEnd, stream);
		pdxStringWriteToStream(conf->NewLine, stream);
	}

	return 1;
}


/* Operations on a single PowerImage */


DT_SLONG pdxImgMakeBitmap(DT_PDDOC power_doc, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_SLONG prop_id, DT_SLONG bmp_w, DT_SLONG bmp_h, const DT_UBYTE* bmp_ptr, DT_SLONG bmp_data_size, DT_UBYTE misc_flags)
{
	if (bmp_w <= 0 || bmp_h <= 0 || bmp_w > 32000 || bmp_h > 32000) return 0;

	DT_UWORD nw = (DT_UWORD)bmp_w;
	DT_UWORD nh = (DT_UWORD)bmp_h;
	DT_ID_SLONG obj1, obj2;
	DT_SLONG len = 0;
	DT_SLONG w0 = w, h0 = h;
	DT_FLOAT k = 1.0;

	/* Create PoweDoc Image */
	obj1 = pdObjAdd(power_doc, 0, "Bitmap Image Data");

	if (misc_flags & 1)
	{
		pdPropAddAsExtBuffer(power_doc, obj1, prop_id, bmp_ptr, bmp_data_size);
		if ((pdPropGet(power_doc, obj1, prop_id, &len) != bmp_ptr) || (len != bmp_data_size)) return 0;
	}
	else
	{
		pdPropAdd(power_doc, obj1, prop_id, bmp_ptr, bmp_data_size);
		pdPropGet(power_doc, obj1, prop_id, &len); if (len != bmp_data_size) return 0;
	}

	pdPropAddAsUWord(power_doc, obj1, pdNw, nw);
	pdPropAddAsUWord(power_doc, obj1, pdNh, nh);

	if (w >= 0 && h >= 0) k = DF_MIN(w / (DT_FLOAT)nw, h / (DT_FLOAT)nh);
	else if (w >= 0) k = w / (DT_FLOAT)nw;
	else if (h >= 0) k = h / (DT_FLOAT)nh;

	if (w < 0) w0 = w = nw; else w = DF_ROUND(k * nw);
	if (h < 0) h0 = h = nh; else h = DF_ROUND(k * nh);

	obj2 = pdObjAdd(power_doc, 0, "Bitmap Image");
	pdPropAddAsSLong(power_doc, obj2, pdDw, w);
	pdPropAddAsSLong(power_doc, obj2, pdDh, h);

	pdPropAddAsUByte(power_doc, obj2, pdQuality, 2);
	pdPropAddAsUByte(power_doc, obj2, pdT, 0);
	pdLinkAdd(power_doc, obj2, pdImageDataPtr, obj1);

	if (prop_id == pdImageDataGray || prop_id == pdImageDataGrayAlpha)
	{
		pdPropAdd(power_doc, obj2, pdRangeRR, reinterpret_cast<const DT_UBYTE*>("00 FF"), PD_HEX);
		pdPropAdd(power_doc, obj2, pdRangeGG, reinterpret_cast<const DT_UBYTE*>("00 FF"), PD_HEX);
		pdPropAdd(power_doc, obj2, pdRangeBB, reinterpret_cast<const DT_UBYTE*>("00 FF"), PD_HEX);
	}

	if (prop_id == pdImageDataRGBAlpha || prop_id == pdImageDataGrayAlpha)
	{
		pdPropAddAsUByte(power_doc, obj2, pdAlphaTreatment, 1);
	}

	pdImgAddPlus(power_doc, obj2, page, x + (w0 - w) / 2, y + (h0 - h) / 2, 0, 0, 0, pdDrawImage, DV_NULL);
	return prop_id;
}


DT_SLONG pdxImgLoadCompressedImage(DT_UWORD compression_type, DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_UBYTE misc_flags)
{
	DT_AUX_INFO info = {DEF_AUX_INIT_KEY, DEF_AUX_INIT_FLAG, 0, 0, compression_type, DV_NULL};
	DT_AUX_USER_STREAM user_stream = {DV_NULL, DV_NULL, DV_NULL, DV_NULL};
	DT_UBYTE* bmp_ptr = DV_NULL;
	DT_SLONG ret = 0, bmp_w = 0, bmp_h = 0, prop_id = 0;
	DT_SWORD format = 0, subformat = 0;

	if ((user_stream.FilePtr = dtFileOpen(file_name, "rb")) == DV_NULL) return 0;

	ret = fxImageRead(&info, &bmp_ptr, &format, &subformat, &bmp_w, &bmp_h, 0, gattribs, &user_stream);

	dtFileClose(user_stream.FilePtr);

	if (bmp_ptr == DV_NULL) return 0;

	//if (ret != 0 && bmp_w > 0 && bmp_h > 0 && bmp_w <= 32000 && bmp_h <= 32000) ret = 1; else { format = 0; ret = 0; }
	if (ret == 0 || bmp_w <= 0 || bmp_h <= 0 || bmp_w > 32000 || bmp_h > 32000) format = 0;

	if (format == 32)
		prop_id = pdxImgMakeBitmap(power_doc, page, x, y, w, h, pdImageDataRGBAlpha, bmp_w, bmp_h, bmp_ptr, 4 * bmp_w * bmp_h, misc_flags);
	else if (format == 24)
		prop_id = pdxImgMakeBitmap(power_doc, page, x, y, w, h, pdImageDataRGB, bmp_w, bmp_h, bmp_ptr, 3 * bmp_w * bmp_h, misc_flags);
	else if (format == 16)
		prop_id = pdxImgMakeBitmap(power_doc, page, x, y, w, h, pdImageDataGrayAlpha, bmp_w, bmp_h, bmp_ptr, 2 * bmp_w * bmp_h, misc_flags);
	else if (format == 8)
		prop_id = pdxImgMakeBitmap(power_doc, page, x, y, w, h, pdImageDataGray, bmp_w, bmp_h, bmp_ptr, bmp_w * bmp_h, misc_flags);

	if ((prop_id != 0) && (misc_flags & 1)) return prop_id;
	DF_PDX_MEM_FREE(bmp_ptr);
	return prop_id;
}


DT_SLONG pdxImgLoadWindowsBitmap(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, const DT_AUX_GRAPHICS_ATTRIBS* /*gattribs*/, DT_UBYTE misc_flags)
{
	#define PDX_WINBMP_DIB_HEADER_MARKER ((DT_UWORD) ('M' << 8) | 'B')
	#define PDX_WINBMP_RGBQUAD_SIZE 4
	#define PDX_WINBMP_RGBTRIPLE_SIZE 3
	#define PDX_WINBMP_BITMAPFILEHEADER_SIZE 14
	#define PDX_WINBMP_BITMAPINFOHEADER_SIZE 40
	#define PDX_WINBMP_BITMAPCOREHEADER_SIZE 12

	DT_UBYTE buffer[255];
	DT_DTSTREAM stream = DV_NULL;
	DT_UBYTE* bmp_ptr = DV_NULL;
	DT_UBYTE* tmp_ptr;
	DT_ULONG bmp_size, bmp_data_size, bmp_row_size, bmp_row_size_padded, header_size = 0, palette_size = 0;
	DT_SLONG i, prop_id = 0, bmp_w = 0, bmp_h = 0, clr_used = 0;
	DT_UWORD bit_count = 0;
	DT_UWORD type;

	/*
	struct bmpBitmapFileHeader //BITMAPFILEHEADER
	{
		DT_UWORD bfType;
		DT_ULONG bfSize;
		DT_UWORD bfReserved1;
		DT_UWORD bfReserved2;
		DT_ULONG bfOffBits;

	} bmp_head;

	struct bmpBitmapInfoHeader //BITMAPINFOHEADER
	{
		DT_ULONG biSize;
		DT_SLONG biWidth;
		DT_SLONG biHeight;
		DT_UWORD biPlanes;
		DT_UWORD biBitCount;
		DT_ULONG biCompression;
		DT_ULONG biSizeImage;
		DT_SLONG biXPelsPerMeter;
		DT_SLONG biYPelsPerMeter;
		DT_ULONG biClrUsed;
		DT_ULONG biClrImportant;
	};

	struct bmpBitmapCoreHeader //BITMAPCOREHEADER
	{
		DT_ULONG bcSize; //used to get to color table
		DT_UWORD bcWidth;
		DT_UWORD bcHeight;
		DT_UWORD bcPlanes;
		DT_UWORD bcBitCount;
	};

	bmpBitmapInfoHeader* bmp_info; //pointer to a Win 3.0-style DIB
	bmpBitmapCoreHeader* bmp_core; //pointer to an other-style DIB
	*/

	DT_STREAM_FILE(sd, file_name);
	if ((stream = dtStreamOpen(&sd, DV_STREAM_OPEN_MUST)) == DV_NULL) goto end;

	dtStreamSeek(stream, 0, DV_SEEK_END);
	bmp_size = dtStreamLoc(stream) - PDX_WINBMP_BITMAPFILEHEADER_SIZE;
	if (bmp_size < 14 || bmp_size > 268435456) goto end;
	dtStreamSeek(stream, 0, DV_SEEK_BGN);

	/* Read the BMP file header and check if it's valid */
	if (dtStreamRead(stream, buffer, PDX_WINBMP_BITMAPFILEHEADER_SIZE, 0) != 1) goto end;
	type = DF_READ_UWORD_LE(buffer);
	if (type != PDX_WINBMP_DIB_HEADER_MARKER) goto end;
	/* Allocate memory for the bitmap */
	if ((bmp_ptr = static_cast<DT_UBYTE*>(DF_PDX_MEM_ALLOC(bmp_size))) == DV_NULL) goto end;

	/* Read BMP header */
	if (dtStreamRead(stream, bmp_ptr, 4, 0) != 1) goto end;
	header_size = DF_READ_ULONG_LE(bmp_ptr);

	if (header_size == PDX_WINBMP_BITMAPINFOHEADER_SIZE)
	{
		/* Win30 DIB */
		if (dtStreamRead(stream, bmp_ptr + 4, header_size - 4, 0) != 1) goto end;
		/* bmp_info = (bmpBitmapInfoHeader*)bmp_ptr; */
		bmp_w = DF_READ_SLONG_LE(bmp_ptr + 4); /* bmp_info->biWidth; */
		bmp_h = DF_READ_SLONG_LE(bmp_ptr + 8); /* bmp_info->biHeight; */
		bit_count = DF_READ_UWORD_LE(bmp_ptr + 14); /* bmp_info->biBitCount; */
		clr_used = DF_READ_ULONG_LE(bmp_ptr + 32); /* bmp_info->biClrUsed; */
		palette_size = PDX_WINBMP_RGBQUAD_SIZE;
	}
	else if (header_size == PDX_WINBMP_BITMAPCOREHEADER_SIZE)
	{
		/* Other style DIB */
		if (dtStreamRead(stream, bmp_ptr + 4, header_size - 4, 0) != 1) goto end;
		/* bmp_core = (bmpBitmapCoreHeader*)bmp_ptr; */
		bmp_w = DF_READ_UWORD_LE(bmp_ptr + 4); /* bmp_core->bcWidth; */
		bmp_h = DF_READ_UWORD_LE(bmp_ptr + 6); /* bmp_core->bcHeight; */
		bit_count = DF_READ_UWORD_LE(bmp_ptr + 10); /* bmp_core->bcBitCount; */
		palette_size = PDX_WINBMP_RGBTRIPLE_SIZE;
	}
	else goto end;

	if (bmp_w <= 0 || bmp_h <= 0 || bmp_w > 32000 || bmp_h > 32000) goto end;

	if (bit_count == 32) /* 32-bpp RGBA bitmap */
	{
		bmp_row_size = 4 * bmp_w;
		prop_id = pdImageDataRGBAlpha;
		palette_size = 0;
	}
	else if (bit_count == 24) /* 24-bpp RGB bitmap */
	{
		bmp_row_size = 3 * bmp_w;
		prop_id = pdImageDataRGB;
		palette_size = 0;
	}
	else if (bit_count == 8) /* 256-colour bitmap */
	{
		bmp_row_size = bmp_w;
		prop_id = pdImageDataGray;
		if (clr_used == 0) palette_size *= 256; else palette_size *= clr_used;
	}
	else goto end;

	bmp_data_size = bmp_row_size * bmp_h;
	if (bmp_data_size > bmp_size) goto end;

	bmp_row_size_padded = (bmp_row_size >> 2) << 2;
	if (bmp_row_size != bmp_row_size_padded) bmp_row_size_padded += 4;

	/* Skip palette */
	dtStreamSeek(stream, palette_size, DV_SEEK_REL);

	/* Read BMP data (row by row, getting rid of extra byte padding) */
	tmp_ptr = bmp_ptr + bmp_row_size * bmp_h;
	for (i = 0; i < bmp_h; i++)
	{
		tmp_ptr -= bmp_row_size;
		if (dtStreamRead(stream, tmp_ptr, bmp_row_size, 0) != 1) goto end;
		dtStreamSeek(stream, bmp_row_size_padded - bmp_row_size, DV_SEEK_REL); /* Skip padding */
	}

	prop_id = pdxImgMakeBitmap(power_doc, page, x, y, w, h, prop_id, bmp_w, bmp_h, bmp_ptr, bmp_data_size, misc_flags);

end:

	if (stream != DV_NULL) dtStreamClose(stream, DV_STREAM_CLOSE_YES);
	if (bmp_ptr == DV_NULL) return prop_id;
	if ((prop_id != 0) && (misc_flags & 1)) return prop_id;
	DF_PDX_MEM_FREE(bmp_ptr);
	return prop_id;
}


DT_SLONG pdxImgLoadPNG(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_UBYTE misc_flags)
{
	return pdxImgLoadCompressedImage(DV_AUX_FILEFORMAT_PNG, power_doc, file_name, page, x, y, w, h, gattribs, misc_flags);
}


DT_SLONG pdxImgLoadJPG(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, DT_UBYTE misc_flags)
{
	return pdxImgLoadCompressedImage(DV_AUX_FILEFORMAT_JPG, power_doc, file_name, page, x, y, w, h, gattribs, misc_flags);
}


DT_ID_SLONG pdxImgAddShape(DT_PDDOC power_doc, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG n, const DT_UBYTE* buff_i, const DT_UBYTE* buff_x, const DT_UBYTE* buff_y, DT_UBYTE fill_rule, DT_UBYTE body, DT_UBYTE shadow, DT_UBYTE outline, const DT_CHAR* body_rgbt, const DT_CHAR* shadow_rgbt, const DT_CHAR* outline_rgbt, DT_SLONG outline_thickness, DT_SWORD shadow_dx, DT_SWORD shadow_dy, DT_ID_SWORD sz)
{
	DT_ID_SLONG obj1 = -1, obj2 = -1, obj3 = -1, obj4 = -1, obj5 = -1, obj6 = -1;

	if (n < 2) return -1;
	if (buff_i == DV_NULL) return -1;
	if (buff_x == DV_NULL) return -1;
	if (buff_y == DV_NULL) return -1;

	if (shadow == 0 && outline == 0) body = 1;

	if (body == 1)
	{
		obj1 = pdObjAdd(power_doc, 0, "Poly Body Color");
		pdPropAdd(power_doc, obj1, pdRGBT, reinterpret_cast<const DT_UBYTE*>(body_rgbt), PD_HEX);
	}

	if (shadow == 1)
	{
		obj2 = pdObjAdd(power_doc, 0, "Poly Shadow Color");
		pdPropAdd(power_doc, obj2, pdRGBT, reinterpret_cast<const DT_UBYTE*>(shadow_rgbt), PD_HEX);

		obj3 = pdObjAdd(power_doc, 0, "Poly Shadow");
		pdLinkAdd(power_doc, obj3, pdStylePtr, obj2);
		pdPropAddAsSWord(power_doc, obj3, pdShadowDx, shadow_dx);
		pdPropAddAsSWord(power_doc, obj3, pdShadowDy, shadow_dy);
	}

	if (outline == 1)
	{
		obj4 = pdObjAdd(power_doc, 0, "Poly Outline Color");
		pdPropAdd(power_doc, obj4, pdRGBT, reinterpret_cast<const DT_UBYTE*>(outline_rgbt), PD_HEX);

		obj5 = pdObjAdd(power_doc, 0, "Poly Outline");
		pdLinkAdd(power_doc, obj5, pdStylePtr, obj4);
		pdPropAddAsSLong(power_doc, obj5, pdLineThickness, outline_thickness);
	}

	obj6 = pdObjAdd(power_doc, 0, "Poly");
	pdPropAdd(power_doc, obj6, pdPolyArrI, buff_i, n);

	if (sz == 1)
	{
		pdPropAdd(power_doc, obj6, pdPolyArrX_8, buff_x, n);
		pdPropAdd(power_doc, obj6, pdPolyArrY_8, buff_y, n);
	}
	else if (sz == 2)
	{
		pdPropAdd(power_doc, obj6, pdPolyArrX_16, buff_x, n * 2);
		pdPropAdd(power_doc, obj6, pdPolyArrY_16, buff_y, n * 2);
	}
	else if (sz == 3)
	{
		pdPropAdd(power_doc, obj6, pdPolyArrX_24, buff_x, n * 3);
		pdPropAdd(power_doc, obj6, pdPolyArrY_24, buff_y, n * 3);
	}
	else
	{
		pdPropAdd(power_doc, obj6, pdPolyArrX_32, buff_x, n * 4);
		pdPropAdd(power_doc, obj6, pdPolyArrY_32, buff_y, n * 4);
	}

	pdPropAddAsUByte(power_doc, obj6, pdPolyFillRule, fill_rule);
	if (body == 1) pdLinkAdd(power_doc, obj6, pdBodyPtr, obj1);
	if (shadow == 1) pdLinkAdd(power_doc, obj6, pdShadowPtr, obj3);
	if (outline == 1) pdLinkAdd(power_doc, obj6, pdOutlinePtr, obj5);

	pdImgAddPlus(power_doc, obj6, page, x, y, 0, 0, 0, pdDrawPoly, DV_NULL);

	return obj6;
}


DT_SLONG pdxImgLoadShapeDefTable(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG /*w*/, DT_SLONG /*h*/)
{
	DT_DTSTREAM stream;
	DT_CHAR buffer[PDX_MAXTEXTLENGTH + 1];
	DT_CHAR* bp;
	DT_UBYTE* buff_c = DV_NULL;
	DT_UBYTE* buff_i = DV_NULL;
	DT_UBYTE* buff_x = DV_NULL;
	DT_UBYTE* buff_y = DV_NULL;
	DT_UBYTE* tmp = DV_NULL;
	DT_SLONG len;
	DT_SLONG n = 0, nmax = 0;
	DT_SLONG c = 0, cmax = 0;
	DT_SLONG outline_thickness = 0;
	DT_SWORD sz = 4, shadow_dx = 0, shadow_dy = 0;
	DT_CHAR body_rgbt[] = "00000000", shadow_rgbt[] = "00000000", outline_rgbt[] = "00000000";
	DT_UBYTE fill_rule = 0, body = 0, shadow = 0, outline = 0;
	DT_FLOAT x_scale = 1.0, y_scale = 1.0, x_offset = 0.0, y_offset = 0.0;
	DT_SLONG cx = 0, cy = 0;
	DT_UBYTE grayscale_rasterizer = 1, dynamic = 0, data = 0;
	DT_ID_SLONG obj = -1;

	DT_STREAM_FILE(sd, file_name);
	if ((stream = dtStreamOpen(&sd, DV_STREAM_OPEN_MUST)) == DV_NULL) return 0;

	while (!dtStreamEos(stream))
	{
		if (pdxStringInputDelimited(buffer, stream, ',', PDX_MAXTEXTLENGTH) == 0) break;

		if (buffer[0] == 0 || buffer[0] == '#')
		{
			/* Comments */
		}
		else if (buffer[0] == '!')
		{
			if (grayscale_rasterizer)
			{
				/* add previously processed shape */
				if (n > 0) pdxImgAddShape(power_doc, page, x, y, n, buff_i, buff_x, buff_y, fill_rule, body, shadow, outline, body_rgbt, shadow_rgbt, outline_rgbt, outline_thickness, shadow_dx, shadow_dy, sz);
				if (buff_i != DV_NULL) DF_PDX_MEM_FREE(buff_i); buff_i = DV_NULL;
				if (buff_x != DV_NULL) DF_PDX_MEM_FREE(buff_x); buff_x = DV_NULL;
				if (buff_y != DV_NULL) DF_PDX_MEM_FREE(buff_y); buff_y = DV_NULL;
				n = 0;
				nmax = 0;
			}
			else if (data && n > 0)
			{
				/* add previously processed color */

				if (c >= cmax)
				{
					/* Reallocate memory for the color array */
					cmax = DF_PDX_SEC_ADD(cmax, 100);
					if ((tmp = static_cast<DT_UBYTE*>(DF_PDX_MEM_REALLOC(buff_c, DF_PDX_SEC_MUL(cmax, 4)))) == DV_NULL) break;
					buff_c = tmp;
				}

				len = 0; dynamic = 0;
				tmp = pdMakeBinary(body_rgbt, PD_HEX, DV_NULL, &len, &dynamic);
				if (tmp) { DF_PDX_MEMCPY(buff_c + c * 4, tmp, 4); DF_PDX_MEM_FREE(tmp); } else { DF_PDX_MEMSET(buff_c + c * 4, 0, 4); }
				c++;
			}

			data = 0;

			/* Special Commands */

			if ((bp = DF_PDX_STRCHR(buffer, '=')) == DV_NULL) continue; /* ignore error */
			*bp = 0;
			bp++;

			if (DF_PDX_STRCMP(buffer, "!RESET") == 0)
			{
				if (DF_PDX_ATOI(bp) != 0)
				{
					x_scale = 1.0;
					y_scale = 1.0;
					x_offset = 0.0;
					y_offset = 0.0;
					outline_thickness = 0;
					shadow_dx = 0;
					shadow_dy = 0;
					fill_rule = 0;
					body = 0;
					shadow = 0;
					outline = 0;
					DF_PDX_STRCPY(body_rgbt, "00000000");
					DF_PDX_STRCPY(shadow_rgbt, "00000000");
					DF_PDX_STRCPY(outline_rgbt, "00000000");
				}
			}
			else if (DF_PDX_STRCMP(buffer, "!FILL_RULE") == 0)
			{
				fill_rule = (DT_UBYTE)DF_PDX_ATOI(bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!X_SCALE") == 0)
			{
				x_scale = DF_PDX_ATOF(bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!Y_SCALE") == 0)
			{
				y_scale = DF_PDX_ATOF(bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!X_OFFSET") == 0)
			{
				x_offset = DF_PDX_ATOF(bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!Y_OFFSET") == 0)
			{
				y_offset = DF_PDX_ATOF(bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!BODY_RGBT") == 0)
			{
				body = 1;
				if (DF_PDX_STRLEN(bp) == 8) DF_PDX_STRCPY(body_rgbt, bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!SHADOW_RGBT") == 0)
			{
				shadow = 1;
				if (DF_PDX_STRLEN(bp) == 8) DF_PDX_STRCPY(shadow_rgbt, bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!OUTLINE_RGBT") == 0)
			{
				outline = 1;
				if (DF_PDX_STRLEN(bp) == 8) DF_PDX_STRCPY(outline_rgbt, bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!SHADOW_DX") == 0)
			{
				shadow = 1;
				shadow_dx = (DT_SWORD)DF_PDX_ATOI(bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!SHADOW_DY") == 0)
			{
				shadow = 1;
				shadow_dy = (DT_SWORD)DF_PDX_ATOI(bp);
			}
			else if (DF_PDX_STRCMP(buffer, "!OUTLINE_THICKNESS") == 0)
			{
				outline = 1;
				outline_thickness = DF_PDX_ATOL(bp);
			}
		}
		else
		{
			/* Coordinates */

			if (n >= nmax)
			{
				/* Reallocate memory for the arrays */
				nmax = DF_PDX_SEC_ADD(nmax, 100);
				if ((tmp = static_cast<DT_UBYTE*>(DF_PDX_MEM_REALLOC(buff_i, nmax))) == DV_NULL) break;
				buff_i = tmp;
				if ((tmp = static_cast<DT_UBYTE*>(DF_PDX_MEM_REALLOC(buff_x, DF_PDX_SEC_MUL(nmax, sz)))) == DV_NULL) break;
				buff_x = tmp;
				if ((tmp = static_cast<DT_UBYTE*>(DF_PDX_MEM_REALLOC(buff_y, DF_PDX_SEC_MUL(nmax, sz)))) == DV_NULL) break;
				buff_y = tmp;
			}

			/* Store coordinates */
			DF_WRITE_UBYTE_LE(buff_i + n, (DT_UBYTE)DF_PDX_ATOI(buffer));

			if (pdxStringInputDelimited(buffer, stream, ',', PDX_MAXTEXTLENGTH) == 0) break;
			cx = DF_ROUND(DF_PDX_ATOF(buffer) * x_scale + x_offset);

			if (pdxStringInputDelimited(buffer, stream, ',', PDX_MAXTEXTLENGTH) == 0) break;
			cy = DF_ROUND(DF_PDX_ATOF(buffer) * y_scale + y_offset);

			if (sz == 1)
			{
				DF_WRITE_SBYTE_LE(buff_x + n, (DT_SBYTE)cx);
				DF_WRITE_SBYTE_LE(buff_y + n, (DT_SBYTE)cy);
			}
			else if (sz == 2)
			{
				DF_WRITE_SWORD_LE(buff_x + n * 2, (DT_SWORD)cx);
				DF_WRITE_SWORD_LE(buff_y + n * 2, (DT_SWORD)cy);
			}
			else if (sz == 3)
			{
				DF_WRITE_STRIO_LE(buff_x + n * 3, cx);
				DF_WRITE_STRIO_LE(buff_y + n * 3, cy);
			}
			else
			{
				DF_WRITE_SLONG_LE(buff_x + n * 4, cx);
				DF_WRITE_SLONG_LE(buff_y + n * 4, cy);
			}

			n++;
			data = 1;
		}
	}

	dtStreamClose(stream, DV_STREAM_CLOSE_YES);

	if (!grayscale_rasterizer) { body = 1; shadow = outline = 0; }

	/* add previously processed shape */
	if (n > 0) obj = pdxImgAddShape(power_doc, page, x, y, n, buff_i, buff_x, buff_y, fill_rule, body, shadow, outline, body_rgbt, shadow_rgbt, outline_rgbt, outline_thickness, shadow_dx, shadow_dy, sz);
	if (buff_i != DV_NULL) DF_PDX_MEM_FREE(buff_i); buff_i = DV_NULL;
	if (buff_x != DV_NULL) DF_PDX_MEM_FREE(buff_x); buff_x = DV_NULL;
	if (buff_y != DV_NULL) DF_PDX_MEM_FREE(buff_y); buff_y = DV_NULL;

	if (obj >= 0 && (!grayscale_rasterizer))
	{
		pdPropAdd(power_doc, obj, pdPolyArrC, buff_c, c * 4);
		pdPropAddAsUByte(power_doc, obj, pdRasterizer, 0);
	}

	if (buff_c != DV_NULL) DF_PDX_MEM_FREE(buff_c); buff_c = DV_NULL;

	return 1;
}


DT_SLONG pdxImgLoadVectorPicture(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_SWORD pcnr)
{
	DT_DTSTREAM stream = DV_NULL;
	DT_UBYTE* ptr = DV_NULL;
	DT_SLONG size;
	DT_SLONG ret = 0;

	DT_ID_SLONG obj;
	DT_SLONG w0 = w, h0 = h;
	DT_SLONG nw = w, nh = h;
	DT_SLONG values_in_out[4] = {pcnr, 0, 0, 0};
	DT_FLOAT k = 1.0;

	DT_STREAM_FILE(sd, file_name);
	if ((stream = dtStreamOpen(&sd, DV_STREAM_OPEN_MUST)) == DV_NULL) goto end;

	dtStreamSeek(stream, 0, DV_SEEK_END);
	size = dtStreamLoc(stream);
	if (size < 14 || size > 268435456) goto end;
	dtStreamSeek(stream, 0, DV_SEEK_BGN);

	/* Allocate memory for the vector picture */
	if ((ptr = static_cast<DT_UBYTE*>(DF_PDX_MEM_ALLOC(size))) == DV_NULL) goto end;
	if (dtStreamRead(stream, ptr, size, 0) != 1) goto end;

	pdBufferExtractValues(ptr, size, 0, values_in_out);
	nw = values_in_out[2];
	nh = values_in_out[3];

	/* Make vector picture */
	if (w >= 0 && h >= 0) k = DF_MIN(w / (DT_FLOAT)nw, h / (DT_FLOAT)nh);
	else if (w >= 0) k = w / (DT_FLOAT)nw;
	else if (h >= 0) k = h / (DT_FLOAT)nh;

	if (w < 0) w0 = w = nw; else w = DF_ROUND(k * nw);
	if (h < 0) h0 = h = nh; else h = DF_ROUND(k * nh);

	obj = pdObjAdd(power_doc, 0, "Vector Picture");
	pdPropAddAsSLong(power_doc, obj, pdDw, w);
	pdPropAddAsSLong(power_doc, obj, pdDh, h);
	pdPropAddAsSWord(power_doc, obj, pdPictureNumber, pcnr);
	pdPropAdd(power_doc, obj, pdPolyArrI, ptr, size);
	pdImgAddPlus(power_doc, obj, page, x + (w0 - w) / 2, y + (h0 - h) / 2, 0, 0, 0, pdDrawPoly, DV_NULL);
	ret = 1;

end:

	if (stream != DV_NULL) dtStreamClose(stream, DV_STREAM_CLOSE_YES);
	if (ptr != DV_NULL) DF_PDX_MEM_FREE(ptr);
	return ret;
}


DT_SLONG pdxImgMakeTextFlow(DT_PDDOC power_doc, DT_UBYTE global_direction, DT_UBYTE text_area_type, DT_UBYTE text_area_row_type, DT_UBYTE text_area_valign, DT_UBYTE text_area_valign_end, DT_UBYTE text_layout_id, DT_SLONG page, DT_SLONG nr_of_pages, DT_SLONG nr_of_columns, DT_SLONG col_dist, DT_SLONG col_x, DT_SLONG col_y, DT_SLONG col_w, DT_SLONG col_h, DT_ID_SLONG obj)
{
	DT_SLONG i, j, n = 0 /*, k = 0*/;
	DT_SLONG x, y = col_y, w = col_w, h = col_h;
	DT_ID_SLONG obj_new = -1;
	DT_ID_SLONG obj_old = -1;
	DT_ID_SLONG obj_first = -1;

	// Make flow
	for (i = 0; i < nr_of_pages; i++)
	{
		x = col_x;
		for (j = 0; j < nr_of_columns; j++)
		{
			obj_new = pdObjAdd(power_doc, 0, "RichTextArea");

			if (obj_old == -1)
			{
				obj_first = obj_new;
				if (obj != -1) pdLinkAdd(power_doc, obj_new, pdTextFragmentPtr, obj);
			}
			else
			{
				pdLinkAdd(power_doc, obj_new, txRichTextFirstPtr, obj_first);
				pdLinkAdd(power_doc, obj_old, txRichTextNextPtr, obj_new);
			}

			pdPropAddAsSLong(power_doc, obj_new, pdTextAreaDw, w);
			pdPropAddAsSLong(power_doc, obj_new, pdTextAreaDh, h);
			pdPropAddAsUByte(power_doc, obj_new, pdTextAreaRowType, text_area_row_type);
			pdPropAddAsUByte(power_doc, obj_new, pdTextAreaType, text_area_type);
			pdPropAddAsUByte(power_doc, obj_new, pdTextAreaVAlign, text_area_valign);
			pdPropAddAsUByte(power_doc, obj_new, pdTextAreaVAlignEnd, text_area_valign_end);
			pdPropAddAsUByte(power_doc, obj_new, pdDeviceMode, DType_ConfigTextLayout[text_layout_id].device_mode);
			pdPropAddAsUByte(power_doc, obj_new, pdGlobalDirection, global_direction);

			/*k =*/ pdImgAddPlus(power_doc, obj_new, page + i, x, y, 0, 0, 0, pdDrawRichTextArea, DV_NULL);

			obj_old = obj_new;
			x += w + col_dist;
			n++;
		}
	}

	//txImgTextReflow(power_doc, k, -1, DV_NULL, enable_compression);

	return n;
}


DT_SLONG pdxImgLoadSimpleText(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_ID_SWORD font_index, DT_SLONG font_width, DT_SLONG font_height, DT_UBYTE global_direction, DT_UBYTE text_area_type, DT_UBYTE text_area_valign, DT_UBYTE text_area_valign_end, DT_UBYTE text_layout_id, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h)
{
	DT_UBYTE* buffer_ptr = DV_NULL;
	DT_ID_SWORD sz = 0;
	DT_ID_SLONG obj1, obj2, obj3, obj4;
	DT_SLONG buffer_len = 0;
	DT_SLONG p = pd0;

	if (text_layout_id > 5) text_layout_id = 0;

	DT_STREAM_FILE(sd, file_name);

	if ((buffer_ptr = lxTextAllocViaStream(&sd, textAuto, 0, &buffer_len, &sz)) == DV_NULL) return 0;
	buffer_len = pdxTextFixNewline(buffer_ptr, buffer_len, sz);

	/* Create PoweDoc Text Area */
	obj1 = pdObjAdd(power_doc, 0, "TextArea Body Color");
	pdPropAdd(power_doc, obj1, pdRGBT, reinterpret_cast<const DT_UBYTE*>("00 00 00 00"), PD_HEX);

	obj2 = pdObjAdd(power_doc, 0, "TextArea Typography");
	pdPropAddAsSWord(power_doc, obj2, pdFontIndex, font_index);
	pdPropAddAsSLong(power_doc, obj2, pdFontDw, font_width);
	pdPropAddAsSLong(power_doc, obj2, pdFontDh, font_height);
	pdPropAddAsSWord(power_doc, obj2, pdSkewHor, 0);
	pdPropAddAsSWord(power_doc, obj2, pdSkewVer, 0);
	pdPropAddAsSWord(power_doc, obj2, pdRotation, 0);
	pdLinkAdd(power_doc, obj2, pdBodyPtr, obj1);

	obj3 = pdObjAdd(power_doc, 0, "TextArea Layout");
	pdPropAddAsSWord(power_doc, obj3, pdSpacingRow, 10);
	pdPropAddAsSWord(power_doc, obj3, pdSpacingLetter, 0);
	pdPropAddAsUByte(power_doc, obj3, pdPositioning, DType_ConfigTextLayout[text_layout_id].positioning);
	pdPropAddAsUByte(power_doc, obj3, pdHinting, 0);
	pdPropAddAsUByte(power_doc, obj3, pdAlign, 0);
	pdPropAddAsUByte(power_doc, obj3, pdAlignEnd, 0);
	pdPropAddAsUByte(power_doc, obj3, pdKerning, 1);

	obj4 = pdObjAdd(power_doc, 0, "TextArea");
	pdLinkAdd(power_doc, obj4, pdTypographyPtr, obj2);
	pdLinkAdd(power_doc, obj4, pdTextLayoutPtr, obj3);
	pdPropAddAsSLong(power_doc, obj4, pdTextAreaDw, w);
	pdPropAddAsSLong(power_doc, obj4, pdTextAreaDh, h);
	pdPropAddAsUByte(power_doc, obj4, pdTextAreaRowType, 20);
	pdPropAddAsUByte(power_doc, obj4, pdTextAreaType, text_area_type);
	pdPropAddAsUByte(power_doc, obj4, pdTextAreaVAlign, text_area_valign);
	pdPropAddAsUByte(power_doc, obj4, pdTextAreaVAlignEnd, text_area_valign_end);
	pdPropAddAsUByte(power_doc, obj4, pdDeviceMode, DType_ConfigTextLayout[text_layout_id].device_mode);
	pdPropAddAsUByte(power_doc, obj4, pdGlobalDirection, global_direction);

	     if (sz == DV_SIZE_1) p = pdTextString_08;
	else if (sz == DV_SIZE_2) p = pdTextString_16;
	else if (sz == DV_SIZE_3) p = pdTextString_24;
	else if (sz == DV_SIZE_4) p = pdTextString_32;

	pdPropAdd(power_doc, obj4, p, buffer_ptr, buffer_len);

	pdImgAddPlus(power_doc, obj4, page, x, y, 0, 0, 0, pdDrawTextArea, DV_NULL);

	lxTextFree(buffer_ptr);
	return 1;
}


DT_ID_SLONG pdxImgTextToRichText(DT_PDDOC power_doc, DT_SLONG img, DT_ID_SLONG obj_parent)
{
	DT_SLONG len = 0;
	DT_UBYTE* data;
	DT_SWORD fn_draw, fn_draw_new;
	DT_ID_SLONG obj_new, obj = pdImgGetObjAndFnDraw(power_doc, img, obj_parent, &fn_draw);
	DT_ID_SLONG link;
	DT_CHAR desc[255];

	if (fn_draw == pdDrawTextArea)
	{
		fn_draw_new = pdDrawRichTextArea;
		DF_PDX_STRCPY(desc, "RichTextArea");
	}
	else if (fn_draw == pdDrawTextLine)
	{
		fn_draw_new = pdDrawRichTextLine;
		DF_PDX_STRCPY(desc, "RichTextLine");
	}
	else if (fn_draw == pdDrawTextArc)
	{
		fn_draw_new = pdDrawRichTextArc;
		DF_PDX_STRCPY(desc, "RichTextArc");
	}
	else if (fn_draw == pdDrawTextPath)
	{
		fn_draw_new = pdDrawRichTextPath;
		DF_PDX_STRCPY(desc, "RichTextPath");
	}
	else if (fn_draw == pdDrawTextDesign)
	{
		fn_draw_new = pdDrawRichTextDesign;
		DF_PDX_STRCPY(desc, "RichTextDesign");
	}
	else return -1;

	if ((obj_new = pdObjAdd(power_doc, 0, desc)) == -1) return -1;

	obj = pdObjDup(power_doc, obj);
	pdObjSet(power_doc, obj, 0, "Text Fragment");

	// For Text Area

	if ((data = pdPropGet(power_doc, obj, pdTextAreaDw, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaDw, data, len);
	pdPropDel(power_doc, obj, pdTextAreaDw);

	if ((data = pdPropGet(power_doc, obj, pdTextAreaDh, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaDh, data, len);
	pdPropDel(power_doc, obj, pdTextAreaDh);

	if ((data = pdPropGet(power_doc, obj, pdTextAreaType, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaType, data, len);
	pdPropDel(power_doc, obj, pdTextAreaType);

	if ((data = pdPropGet(power_doc, obj, pdTextAreaRowType, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaRowType, data, len);
	pdPropDel(power_doc, obj, pdTextAreaRowType);

	if ((data = pdPropGet(power_doc, obj, pdTextAreaWrapType, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaWrapType, data, len);
	pdPropDel(power_doc, obj, pdTextAreaWrapType);

	if ((data = pdPropGet(power_doc, obj, pdTextAreaEdgeLeft, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaEdgeLeft, data, len);
	pdPropDel(power_doc, obj, pdTextAreaEdgeLeft);

	if ((data = pdPropGet(power_doc, obj, pdTextAreaEdgeRight, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaEdgeRight, data, len);
	pdPropDel(power_doc, obj, pdTextAreaEdgeRight);

	if ((data = pdPropGet(power_doc, obj, pdTextAreaVAlign, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaVAlign, data, len);
	pdPropDel(power_doc, obj, pdTextAreaVAlign);

	if ((data = pdPropGet(power_doc, obj, pdTextAreaVAlignEnd, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdTextAreaVAlignEnd, data, len);
	pdPropDel(power_doc, obj, pdTextAreaVAlignEnd);

	// For Text Line

	if ((data = pdPropGet(power_doc, obj, pdAngleRotation, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdAngleRotation, data, len);
	pdPropDel(power_doc, obj, pdAngleRotation);

	// For Text Arc

	if ((data = pdPropGet(power_doc, obj, pdRadius, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdRadius, data, len);
	pdPropDel(power_doc, obj, pdRadius);

	if ((data = pdPropGet(power_doc, obj, pdAngleAlpha, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdAngleAlpha, data, len);
	pdPropDel(power_doc, obj, pdAngleAlpha);

	// For Text Path

	if ((data = pdPropGet(power_doc, obj, pdPolyArrI, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdPolyArrI, data, len);
	pdPropDel(power_doc, obj, pdPolyArrI);

	if ((data = pdPropGet(power_doc, obj, pdPolyArrX_32, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdPolyArrX_32, data, len);
	pdPropDel(power_doc, obj, pdPolyArrX_32);

	if ((data = pdPropGet(power_doc, obj, pdPolyArrY_32, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdPolyArrY_32, data, len);
	pdPropDel(power_doc, obj, pdPolyArrY_32);

	// For All

	if ((data = pdPropGet(power_doc, obj, pdGlobalDirection, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdGlobalDirection, data, len);
	pdPropDel(power_doc, obj, pdGlobalDirection);

	if ((data = pdPropGet(power_doc, obj, pdGlyphStart, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdGlyphStart, data, len);
	pdPropDel(power_doc, obj, pdGlyphStart);

	if ((data = pdPropGet(power_doc, obj, pdDeviceMode, &len)) != DV_NULL) pdPropAdd(power_doc, obj_new, pdDeviceMode, data, len);
	pdPropDel(power_doc, obj, pdDeviceMode);

	if ((link = pdLinkGet(power_doc, obj, pdBaselinePtr)) >= 0) pdLinkAdd(power_doc, obj_new, pdBaselinePtr, link);
	pdLinkDel(power_doc, obj, pdBaselinePtr);

	pdLinkAdd(power_doc, obj_new, pdTextFragmentPtr, obj);

	if (obj_parent < 0)
	{
		pdImgSetFnDraw(power_doc, img, fn_draw_new);
		pdImgSetObj(power_doc, img, obj_new);
	}
	else
	{
		pdLinkDel(power_doc, obj_parent, pdFnDrawObjectPtr);
		pdPropDel(power_doc, obj_parent, pdFnDraw);

		pdPropAddAsSWord(power_doc, obj_parent, pdFnDraw, fn_draw_new);
		pdLinkAdd(power_doc, obj_parent, pdFnDrawObjectPtr, obj_new);
	}

	return obj_new;
}


/* Page operations */


void pdxPagesMove(DT_PDDOC power_doc, DT_SLONG start_page, DT_SLONG page_offset)
{
	DT_SLONG page, mask;
	DT_SLONG img, img_last = pdImgGetLast(power_doc);

	for (img = 0; img <= img_last; img++)
	{
		if ((page = pdImgGetPage(power_doc, img)) >= start_page) pdImgSetPage(power_doc, img, page + page_offset);
		if ((mask = pdImgGetMask(power_doc, img)) != 0 && mask >= start_page) pdImgSetMask(power_doc, img, mask + page_offset);
	}
}


DT_SWORD pdxPageIsEmpty(DT_PDDOC power_doc, DT_SLONG page)
{
	DT_SLONG img, img_last = pdImgGetLast(power_doc);

	if (power_doc == DV_NULL) return 1;

	for (img = 0; img <= img_last; img++)
	{
		if (page == pdImgGetPage(power_doc, img)) return 0;
	}

	return 1;
}


DT_ID_SLONG pdxPageToText(DT_PDDOC power_doc, DT_SLONG page, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, const DT_SLONG metrics_x_array[], const DT_SLONG metrics_y_array[], DT_SLONG array_len)
{
	if (extent == DV_NULL || metrics_x_array == DV_NULL || metrics_y_array == DV_NULL || array_len < 3) return -1;
	pdxImgSelSelectByPageRange(power_doc, page, page);
	pdxImgSelGroup(power_doc, page, x, y, 2);
	return pdxImgSelToText(power_doc, extent, metrics_x_array, metrics_y_array, array_len, 2);
}


/* Document operations */


DT_SLONG pdxDocDiscardDescriptions(DT_PDDOC power_doc, const DT_CHAR* important_desc_prefix)
{
	DT_SLONG n =0, name = 0;
	DT_SLONG desc_len, prefix_len = 0;
	DT_ID_SLONG obj, obj_last = pdObjGetLast(power_doc);
	const DT_CHAR* desc;

	if (important_desc_prefix != DV_NULL) prefix_len = DF_PDX_STRLEN(important_desc_prefix);

	for (obj = 0; obj <= obj_last; obj++)
	{
		pdObjGet(power_doc, obj, &name, &desc);
		if (desc == DV_NULL) continue;

		if (prefix_len < 1) goto set;
		desc_len = DF_PDX_STRLEN(desc); if (desc_len < prefix_len) goto set;
		if (DF_PDX_MEMCMP(desc, important_desc_prefix, prefix_len) == 0) continue;

	set:

		pdObjSet(power_doc, obj, name, DV_NULL);
		if (desc != DV_NULL) n++;
	}

	return n;
}


DT_SLONG pdxDocReconnectExternal(DT_PDDOC power_doc)
{
	DT_UBYTE* data;
	DT_SLONG i, n = 0, len = 0, prop;
	DT_ID_SLONG obj, obj_last = pdObjGetLast(power_doc);
	DT_UBYTE type = 0;

	if (obj_last < 0) return 0;

	for (obj = 0; obj <= obj_last; obj++)
	{
		i = 0;
		while ((prop = pdNameGet(power_doc, obj, i++, &type)) != 0)
		{
			if (type != PD_FT_EXTPROPERTY_BUFFER) continue;
			data = pdPropGet(power_doc, obj, prop, &len); if (data == DV_NULL || len < 0) continue;
			pdPropSet(power_doc, obj, prop, data, len);
			//if (free_memory) { DF_PDX_MEM_FREE(data); }
			i = 0;
			n++;
		}
	}

	return n;
}


DT_SLONG pdxDocReconnectFonts(DT_PDDOC power_doc)
{
	DT_PDENGINE engine;
	DT_DTENGINE dtype_inst;

	DT_SWORD font_format_id;
	DT_CHAR fuid[16 + 1];
	DT_ID_SWORD font_index;
	DT_SWORD fcnr, cmapnr;
	DT_UBYTE caching, hinting, activation_level;
	DT_SLONG n = 0;
	DT_ID_SLONG obj, obj_last = pdObjGetLast(power_doc);

	if (obj_last < 0) return 0;

	pdDocGetPowerEngInst(power_doc, &engine); if (engine == DV_NULL) return 0;
	pdEngineGetDTypeEngInst(engine, &dtype_inst); if (dtype_inst == DV_NULL) return 0;

	for (obj = 0; obj <= obj_last; obj++)
	{
		if ((font_index = pdPropGetAsSWord(power_doc, obj, pdFontIndex, -1)) < 0) continue;
		if (dtFontGetSysStatus(dtype_inst, font_index, &font_format_id, fuid, &fcnr, &cmapnr, &caching, &hinting, &activation_level) != 0) continue; /* Skip font instances or removed fonts */

		fuid[16] = 0; if (DF_PDX_STRLEN(fuid) != 16) continue;

		pdPropApply(power_doc, obj, pdFontUniqueID, reinterpret_cast<DT_UBYTE*>(fuid), PD_ASC, PD_APPLY_STANDARD);
		pdPropDel(power_doc, obj, pdFontIndex);
		n++;
	}

	return n;
}


DT_SLONG pdxDocNormalizeLinks(DT_PDDOC power_doc, DT_UBYTE multi_pass)
{
	DT_SLONG i, n = 0, m, prop_name, target;
	DT_ID_SLONG obj1, obj2, obj_last = pdObjGetLast(power_doc);
	DT_ID_SLONG link;
	DT_UBYTE type;

	do
	{
		m = 0;

		/* Go through all objects in document */
		for (obj1 = 0; obj1 <= obj_last; obj1++)
		{
			/* Find all links and property references */
			i = 0;
			while ((prop_name = pdNameGet(power_doc, obj1, i++, &type)) != 0)
			{
				target = 0;
				if (type == PD_FT_LINK) link = pdLinkGet(power_doc, obj1, prop_name); /* Link */
				else if (type == PD_FT_PROPERTYREFERENCE) link = pdPropGetAsRef(power_doc, obj1, prop_name, &target); /* Property Reference */
				else continue;

				/* Try to find an object the contains all the same fields as the linked object, but before it */
				for (obj2 = 0; obj2 < link; obj2++)
				{
					if (pdxObjCompare(power_doc, link, obj2) != 1) continue;

					/* If successful, repoint the link to obj2 */
					if (type == PD_FT_LINK) pdLinkSet(power_doc, obj1, prop_name, obj2);
					else if (type == PD_FT_PROPERTYREFERENCE) pdPropSetAsRef(power_doc, obj1, prop_name, obj2, target);
					n++;
					m++;
					break;
				}
			}
		}

	}
	while (m > 0 && multi_pass);

	return n;
}


DT_SLONG pdxDocAppendFromStream(DT_PDDOC power_doc, DT_ID_SWORD flag, DT_SLONG page, DT_SLONG x, DT_SLONG y, const DT_STREAM_DESC* sd)
{
	/* Note that with this method all data is loaded and decompressed in memory */

	/* flag=0 means read only uncompressed files, flag=1 means read only compressed files, flag=2 means read both compressed and uncompressed files */

	DT_AUX_INFO info = {DEF_AUX_INIT_KEY, DEF_AUX_INIT_FLAG, 0, 0, DV_AUX_FILEFORMAT_ZLIB, DV_NULL};
	DT_DTSTREAM stream = DV_NULL;
	const DT_STREAM_DESC* sd2 = sd;
	DT_UBYTE buffer[32];
	DT_UBYTE* mem_raw = DV_NULL;
	DT_UBYTE* mem_compressed = DV_NULL;
	DT_SLONG len;
	DT_SLONG len_raw;
	DT_SLONG len_compressed;
	DT_SLONG ret = 0, res = -1;

	if (flag == 0) goto doc_append;

	if ((stream = dtStreamOpen(sd2, DV_STREAM_OPEN_MUST)) == DV_NULL) return -1;

	if (dtStreamRead(stream, buffer, 20, 0) != 1) { dtStreamClose(stream, DV_STREAM_CLOSE_YES); return -1; }
	if (DF_PDX_MEMCMP(buffer, "DENEMKO-POWERDOC-CMP", 20) != 0) /* this is not compressed PowerDoc format */
	{
		dtStreamClose(stream, DV_STREAM_CLOSE_YES);

		if (flag == 1) return 0;
		goto doc_append; /* try uncompressed PowerDoc format */
	}

	/* try compressed PowerDoc format */

	if (dtStreamRead(stream, buffer, 2 * sizeof(DT_SLONG), 0) != 1) { dtStreamClose(stream, DV_STREAM_CLOSE_YES); return -1; }
	len_raw = len = DF_READ_SLONG_LE(buffer);
	len_compressed = DF_READ_SLONG_LE(buffer + sizeof(DT_SLONG));
	if (len_raw < 1 || len_compressed < 1) { dtStreamClose(stream, DV_STREAM_CLOSE_YES); return -1; }

	if ((mem_raw = static_cast<DT_UBYTE*>(DF_PDX_MEM_ALLOC(len_raw))) == DV_NULL) { dtStreamClose(stream, DV_STREAM_CLOSE_YES); return -2; }
	if ((mem_compressed = static_cast<DT_UBYTE*>(DF_PDX_MEM_ALLOC(len_compressed))) == DV_NULL) { DF_PDX_MEM_FREE(mem_raw); dtStreamClose(stream, DV_STREAM_CLOSE_YES); return -2; }

	if (dtStreamRead(stream, mem_compressed, len_compressed, 0) != 1) { DF_PDX_MEM_FREE(mem_compressed); DF_PDX_MEM_FREE(mem_raw); dtStreamClose(stream, DV_STREAM_CLOSE_YES); return -1; }

	dtStreamClose(stream, DV_STREAM_CLOSE_YES);

#ifdef DEF_PDX_ZLIB
	ret = fxBufferDecompress(&info, mem_raw, &len_raw, mem_compressed, len_compressed);
#else
	ret = dtBufferDecompress(DV_METHOD_ZLIB, mem_raw, &len_raw, mem_compressed, len_compressed);
#endif

	DF_PDX_MEM_FREE(mem_compressed);

	if (ret != 1 || len_raw != len)
	{
		DF_PDX_MEM_FREE(mem_raw);
		return -3;
	}

	DT_STREAM_MEMORY(sd_memory, mem_raw, len_raw);
	sd2 = &sd_memory;

doc_append:

	if (pdDocAppendFromStream(power_doc, page, x, y, sd2) == 1) res = 1;
	if (mem_raw != DV_NULL) DF_PDX_MEM_FREE(mem_raw);

	return res;
}


DT_SLONG pdxDocAppend(DT_PDDOC power_doc, DT_ID_SWORD flag, DT_SLONG page, DT_SLONG x, DT_SLONG y, const DT_CHAR* file_name)
{
	DT_STREAM_FILE(sd, file_name);
	return pdxDocAppendFromStream(power_doc, flag, page, x, y, &sd);
}


DT_SLONG pdxDocSaveToStream(DT_PDDOC power_doc, DT_ID_SWORD flag, DT_STREAM_DESC* sd)
{
	/* Note that with this method all data is saved and compressed in memory */

	/* flag=1 means compress file */

	DT_AUX_INFO info = {DEF_AUX_INIT_KEY, DEF_AUX_INIT_FLAG, 0, 0, DV_AUX_FILEFORMAT_ZLIB, DV_NULL};
	DT_DTSTREAM stream = DV_NULL;
	DT_STREAM_DESC* sd2 = sd;
	DT_UBYTE buffer[32];
	DT_UBYTE* mem_raw = DV_NULL;
	DT_UBYTE* mem_compressed = DV_NULL;
	DT_SLONG len_raw;
	DT_SLONG len_compressed;
	DT_SLONG ret = 0, res = -1;

	DT_STREAM_MEMORY(sd_memory, DV_NULL, 0);

	if (flag == 1) sd2 = &sd_memory;
	if (pdDocSaveToStream(power_doc, sd2) < 0) return -1;
	if (flag != 1) return 1;

	if (sd_memory.stream_locator.mem_w.len < 0) return -1;
	len_raw = sd_memory.stream_locator.mem_w.len;
	mem_raw = sd_memory.stream_locator.mem_w.addr;

#ifdef DEF_PDX_ZLIB
	len_compressed = fxBufferGetBound(&info, len_raw); if (len_compressed < 1) return -2;
	if ((mem_compressed = static_cast<DT_UBYTE*>(DF_PDX_MEM_ALLOC(len_compressed))) == DV_NULL) { pdFree(mem_raw); return -2; }
	ret = fxBufferCompress(&info, mem_compressed, &len_compressed, mem_raw, len_raw, -1);
#else
	len_compressed = len_raw;
#endif

	pdFree(mem_raw);

	if (ret != 1)
	{
		if (mem_compressed != DV_NULL) DF_PDX_MEM_FREE(mem_compressed);
		return -3;
	}

	if ((stream = dtStreamOpenW(sd, DV_STREAM_OPEN_MUST, 0)) == DV_NULL) goto end;

	DF_PDX_MEMCPY(buffer, "DENEMKO-POWERDOC-CMP", 20);
	if (dtStreamWrite(stream, buffer, 20, 0) != 1) goto end;

	DF_WRITE_SLONG_LE(buffer, len_raw);
	DF_WRITE_SLONG_LE(buffer + sizeof(DT_SLONG), len_compressed);
	if (dtStreamWrite(stream, buffer, 2 * sizeof(DT_SLONG), 0) != 1) goto end;
	if (dtStreamWrite(stream, mem_compressed, len_compressed, 0) != 1) goto end;
	res = 1;

end:

	if (stream != DV_NULL) dtStreamClose(stream, DV_STREAM_CLOSE_YES);
	DF_PDX_MEM_FREE(mem_compressed);

	return res;
}


DT_SLONG pdxDocSave(DT_PDDOC power_doc, DT_ID_SWORD flag, const DT_CHAR* file_name)
{
	DT_STREAM_FILE(sd, file_name);
	return pdxDocSaveToStream(power_doc, flag, &sd);
}


DT_PDDOC pdxDocMakeCopy(DT_PDDOC power_doc, DT_PDENGINE engine)
{
	DT_PDDOC power_doc_copy;

	if (pdDocIni(&power_doc_copy, engine) == 0) return DV_NULL;
	if (pdDocAppend(power_doc_copy, 0, 0, 0, power_doc) == 1) return power_doc_copy;

	pdDocExt(power_doc_copy);
	return DV_NULL;
}


DT_SLONG pdxDocExport(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_UBYTE use_name_enums, const DT_PDX_EXPORT_CONFIG* conf, const DT_CHAR* body_str)
{
	const DT_CHAR* info[7] =
	{
		"                   D-Type PowerDoc (TM)                   ",
		"        Portable graphics library for high-quality        ",
		"             resolution independent documents             ",
		"         Copyright (C) 1996-2020 D-Type Solutions         ",
		"                                                          ",
		"             Web Site: http://www.d-type.com              ",
		"                 E-mail: info@d-type.com                  "
	};
	DT_CHAR t[64];
	DT_SLONG i, len = -1;
	DT_DTSTREAM stream;
	time_t now = time(DV_NULL);

	DF_PDX_STRCPY(t, ctime(&now));
	pdxStringZap(t);

	DT_STREAM_FILE(sd, file_name);
	if ((stream = dtStreamOpenW(&sd, DV_STREAM_OPEN_MUST, 0)) == DV_NULL) return len;

	pdxStringWriteToStream(conf->PrologOpen, stream);

	for (i = 0; i < 7; i++)
	{
		pdxStringWriteToStream(conf->CommentOpen, stream);
		pdxStringWriteToStream(info[i], stream);
		pdxStringWriteToStream(conf->CommentClose, stream);
		pdxStringWriteToStream(conf->NewLine, stream);
	}

	pdxStringWriteToStream(conf->NewLine, stream);

	//DF_PDX_SPRINTF(str, "%s This file was auto-generated on %s %s%s%s", conf->CommentOpen, t, conf->CommentClose, conf->NewLine, conf->NewLine);
	pdxStringWriteToStream(conf->CommentOpen, stream);
	pdxStringWriteToStream(" This file was auto-generated on ", stream);
	pdxStringWriteToStream(t, stream);
	pdxStringWriteToStream(" ", stream);
	pdxStringWriteToStream(conf->CommentClose, stream);
	pdxStringWriteToStream(conf->NewLine, stream);
	pdxStringWriteToStream(conf->NewLine, stream);

	pdxStringWriteToStream(conf->PrologClose, stream);

	pdxStringWriteToStream(conf->Header1, stream);
	pdxStringWriteToStream(conf->Header2, stream);
	pdxStringWriteToStream(conf->Header3, stream);
	pdxStringWriteToStream(conf->MakeHeader, stream);

	if (body_str != DV_NULL)
	{
		pdxStringWriteToStream(conf->Tab, stream);
		pdxStringWriteToStream(body_str, stream);
		pdxStringWriteToStream(conf->NewLine, stream);
	}
	else
	{
		if (conf->TransformViaMatrix == 1)
		{
			//DF_PDX_SPRINTF(str, "%s%s Affine Transformation Matrix %s%s", conf->Tab, conf->CommentOpen, conf->CommentClose, conf->NewLine);
			pdxStringWriteToStream(conf->Tab, stream);
			pdxStringWriteToStream(conf->CommentOpen, stream);
			pdxStringWriteToStream(" Affine Transformation Matrix", stream);
			pdxStringWriteToStream(conf->CommentClose, stream);
			pdxStringWriteToStream(conf->NewLine, stream);

			//DF_PDX_SPRINTF(str, "%s%stransform%s%s%s%s", conf->Tab, conf->ArrTransformDeclaration1, conf->ArrTransformDeclaration2, conf->CmdEnd, conf->NewLine, conf->NewLine);
			pdxStringWriteToStream(conf->Tab, stream);
			pdxStringWriteToStream(conf->ArrTransformDeclaration1, stream);
			pdxStringWriteToStream("transform", stream);
			pdxStringWriteToStream(conf->ArrTransformDeclaration2, stream);
			pdxStringWriteToStream(conf->CmdEnd, stream);
			pdxStringWriteToStream(conf->NewLine, stream);
			pdxStringWriteToStream(conf->NewLine, stream);
		}

		pdxObjAllExport(power_doc, stream, use_name_enums, conf);
		pdxImgAllExport(power_doc, stream, use_name_enums, conf);
	}

	pdxStringWriteToStream(conf->MakeFooter, stream);
	pdxStringWriteToStream(conf->Footer1, stream);
	pdxStringWriteToStream(conf->Footer2, stream);
	pdxStringWriteToStream(conf->Footer3, stream);

	len = dtStreamLoc(stream);
	dtStreamClose(stream, DV_STREAM_CLOSE_YES);

	return len;
}


DT_SLONG pdxDocExportAsText(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"",
		"---",
		"---",
		"",

		"START OF DOCUMENT SPECIFICATION\r\n" \
		"\r\n",

		"\r\n" \
		"" \
		"END OF DOCUMENT SPECIFICATION\r\n",

		"", " = Array(", ")",
		1, "", "(2, 2)",
		"(", ")", ",", -1,
		"0",

		"OBJECT ", "",
		"IMAGE ", "",
		"", "",
		"", "", "",
		"", "",

		"",
		"",
		"",

		1,
		1,

		3, "\" + chr(", ") + \""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\")");
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsC(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"/* See readme.txt for additional information. */\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"/*",
		"*/",
		";",

		"void MyPowerDocMake(DT_PDDOC doc)\r\n" \
		"{\r\n" \
		"\t/* PowerDoc Make Script v1.0 */\r\n\r\n",

		"\r\n" \
		"}\r\n",

		"DT_ID_SLONG ", "[", "]",
		1, "DT_FLOAT ", "[3][3]",
		"[", "]", "][", 0,
		"DV_NULL",

		"pdObjAdd(doc, ", ")",
		"pdImgAddPlus(doc, ", ")",
		"pdLinkAdd(doc, ", ")",
		"pdPropAdd(doc, ", "(DT_UBYTE*)", ")",
		"pdPropAddAsRef(doc, ", ")",

		"",
		"",
		"",

		0,
		0,

		10, "\\", ""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "pdDocAppendFromFile(doc, 0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\");");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsJava(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"// See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"int PowerDocMake(DTypeDoc doc)\r\n" \
		"{\r\n" \
		"\t// PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\treturn 1;\r\n" \
		"}\r\n",

		"int ", "[] = new int[", "]",
		0, "double ", " = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]",
		"[", "]", "][", 0,
		"null",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd3(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"",
		"",
		"",

		0,
		1,

		10, "\\", ""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 2, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "if (doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") != 1) return 0;");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 2, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsINTEGRAL(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"// See readme.txt for additional information.\r\n\r\n",
		"::my.include(\"../../common/includes/integral-basic.i\");\r\n\r\n",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"function CMyDocumentBasic::Make()\r\n" \
		"{\r\n" \
		"\t/* Lambda shortcuts */\r\n\tlocal o = @(label = \"\") CDTObj(this, label); /* to make object */\r\n\tlocal p = @(id, str, len = PD_DEFAULT) CDTProp(id, str, len); /* to add property */\r\n\tlocal r = @(id, obj, id2) CDTPropRef(id, obj, id2); /* to add property reference */\r\n\tlocal l = @(id, obj) CDTLink(id, obj); /* to add link */\r\n\r\n",

		"}\r\n",

		"local ", " = array(", ")",
		1, "local ", " = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]",
		"[", "]", DV_NULL, 0,
		"null",

		"@o(", ")",
		"@.Pin(", ")",
		"@ + l(", ")",
		"@ + p(", "", ")",
		"@ + r(", ")",

		"\r\n\r\nlocal format = ::my.app.GetQueryVar(\"format\", DX_FORMAT_PNG).tointeger();\r\nlocal zoom = ::my.app.GetQueryVar(\"zoom\", 1.2).tofloat();\r\nlocal page = ::my.app.GetQueryVar(\"page\", 0).tointeger();\r\n\r\n",
		"local doc = CMyDocumentBasic();\r\ndoc.Make();\r\ndoc.Cleanup();\r\n\r\n",
		"::my.app.ReZoom(zoom);::my.app.Export(format, 680 * zoom, 700 * zoom, 0, page, DX_TRANSP_ALPHA);\r\n",

		1,
		1,

		13, "\\x", ""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "Append(\""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\", 0, 0, 0);");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsCPP(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"// See readme.txt for additional information.\r\n\r\n",
		"class CMyDocumentBasic : public CDTDocV7\r\n{\r\npublic:\r\n\r\n\tvoid Make();\r\n};\r\n\r\n",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"void CMyDocumentBasic::Make()\r\n" \
		"{\r\n" \
		"\t/* Lambda shortcuts */\r\n\tauto o = [this](const DT_CHAR* label = \"\") { return CDTObj(*this, label); }; /* to make object */\r\n\tauto p = [](DT_SLONG id, const DT_CHAR* str, DT_SLONG len = PD_DEFAULT) { return CDTProp(id, str, len); }; /* to add property */\r\n\tauto r = [](DT_SLONG id, CDTObj& obj, DT_SLONG id2) { return CDTPropRef(id, obj, id2); }; /* to add property reference */\r\n\tauto l = [](DT_SLONG id, CDTObj& obj) { return CDTLink(id, obj); }; /* to add link */\r\n\r\n",

		"}\r\n",

		"CDTObj ", "[", "]",
		1, "DT_FLOAT ", "[3][3]",
		"[", "]", "][", 0,
		"DV_NULL",

		"@o(", ")",
		"@.Pin(", ")",
		"@ + l(", ")",
		"@ + p(", "", ")",
		"@ + r(", ")",

		"",
		"",
		"",

		1,
		1,

		10, "\\", ""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "Append(\""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\", 0, 0, 0);");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsJScript(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"// See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"function PowerDocMake(doc)\r\n" \
		"{\r\n" \
		"\t// PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\treturn 1;\r\n" \
		"}\r\n",

		"var ", " = new Array(", ")",
		0, "var ", " = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]",
		"[", "]", "][", 0,
		"null",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd3(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"",
		"",
		"",

		1,
		1,

		10, "\\", ""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "if (doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") != 1) return 0;");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsVBScript(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"' See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"'",
		"",
		"",

		"Function PowerDocMake(doc)\r\n\r\n" \
		"\t' PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\tPowerDocMake = 1\r\n\r\n" \
		"End Function\r\n",

		"Dim ", "(", ")",
		1, "Dim ", "(2, 2)",
		"(", ")", ",", -1,
		"0",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd ", "",
		"doc.LinkAdd ", "",
		"doc.PropAdd ", "", "",
		"doc.PropAddAsRef ", "",

		"",
		"",
		"",

		1,
		1,

		3, "\" & Chr(", ") & \""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "PowerDocMake = 0\r\n\tIf doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") <> 1 Then Exit Function");
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsVB(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"' See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"'",
		"",
		"",

		"Function PowerDocMake(doc)\r\n\r\n" \
		"\t' PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\tPowerDocMake = 1\r\n\r\n" \
		"End Function\r\n",

		"Dim ", "(", ")",
		1, "Dim ", "(2, 2)",
		"(", ")", ",", -1,
		"0",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"",
		"",
		"",

		1,
		1,

		3, "\" & Chr(", ") & \""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "PowerDocMake = 0\r\n\tIf doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") <> 1 Then Exit Function");
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsCS(DT_PDDOC power_doc, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"",
		"",

		"// See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"public object PowerDocMake(PowerDocClass doc)\r\n" \
		"{\r\n" \
		"\t// PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\treturn 1;\r\n" \
		"}\r\n",

		"int[] ", " = new int[", "]",
		0, "double[,] ", " = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]",
		"[", "]", "][", 0,
		"null",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd3(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"",
		"",
		"",

		0,
		0,

		10, "\\", ""
	};

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 1, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "if (doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") != 1) return 0;");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 1, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsJSPInJava(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"<%@ page import = \"dtype.DTypeDoc\" %>\r\n\r\n<%\r\n\r\n",
		"%>\r\n\r\n",

		"<%\r\n// See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"class MyDTypeDoc { int PowerDocMake(DTypeDoc doc)\r\n" \
		"{\r\n" \
		"\t// PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\treturn 1;\r\n" \
		"}}\r\n",

		"int ", "[] = new int[", "]",
		0, "double ", " = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]",
		"[", "]", "][", 0,
		"null",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd3(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"\r\n\r\n" \
		"// Create and initialize PowerDoc object\r\n" \
		"DTypeDoc doc = new DTypeDoc();\r\n" \
		"doc.Initialize(application.getInitParameter(\"DTypeEngineInitPath\"));\r\n",

		"",

		"// Set zoom factor\r\n" \
		"doc.Zoom = 1;\r\n" \
		"// Set clipping area\r\n" \
		"doc.ClipX = 0; doc.ClipY = 0; doc.ClipW = 400; doc.ClipH = 400;\r\n" \
		"// Make document\r\n" \
		"new MyDTypeDoc().PowerDocMake(doc);\r\n" \
		"\r\n" \
		"// Render the page as a PNG image\r\n" \
		"byte[] image = doc.MakePNG();\r\n" \
		"doc.Terminate();\r\n" \
		"response.setContentType(\"image/png\");\r\n" \
		"response.setContentLength(image.length);\r\n" \
		"response.getOutputStream().write(image);\r\n" \
		"// If you wish to render the page as a JPEG image, remove the above 5 lines and uncomment the following 5 lines\r\n" \
		"// byte[] image = doc.MakeJPG();\r\n" \
		"// doc.Terminate();\r\n" \
		"// response.setContentType(\"image/png\");\r\n" \
		"// response.setContentLength(image.length);\r\n" \
		"// response.getOutputStream().write(image);\r\n" \
		"\r\n\r\n%>\r\n",

		0,
		1,

		10, "\\", ""
	};

	//DF_PDX_SPRINTF(footer_str, "// Specify the page to render\r\ndoc.Page = %ld;\r\n", curr_page);
	DT_STRING_CHARS(footer_str, PDX_MAXTEXTLENGTH, "// Specify the page to render\r\ndoc.Page = "); dtStrConcatSLong(&footer_str, curr_page); dtStrConcatChars(&footer_str, ";\r\n");
	conf.Footer2 = footer_str.str;

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 2, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "if (doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") != 1) return 0;");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 2, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsASPInJScript(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"<%@ LANGUAGE = JavaScript %>\r\n\r\n<%\r\n\r\n",
		"%>\r\n\r\n",

		"<%\r\n// See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"function PowerDocMake(doc)\r\n" \
		"{\r\n" \
		"\t// PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\treturn 1;\r\n" \
		"}\r\n",

		"var ", " = new Array(", ")",
		0, "var ", " = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]",
		"[", "]", "][", 0,
		"null",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd3(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"\r\n\r\n" \
		"// Create and initialize PowerDoc object\r\n" \
		"var doc = Server.CreateObject(\"PDOC.PowerDoc\");\r\n" \
		"doc.Initialize(PD_INI_SHARE, Application(\"pdEngine\"));\r\n",

		"",

		"// Set zoom factor\r\n" \
		"doc.Zoom = 1;\r\n" \
		"// Set clipping area\r\n" \
		"doc.Clip(0, 0, 400, 400);\r\n" \
		"// Make document\r\n" \
		"PowerDocMake(doc);\r\n" \
		"\r\n" \
		"// Render the page as a PNG image\r\n" \
		"Response.ContentType = \"image/png\";\r\n" \
		"Response.BinaryWrite(doc.MakePNG());\r\n" \
		"// If you wish to render the page as a JPEG image, remove the above 2 lines and uncomment the following 2 lines\r\n" \
		"// Response.ContentType = \"image/jpeg\";\r\n" \
		"// Response.BinaryWrite(doc.MakeJPG());\r\n" \
		"\r\n\r\n%>\r\n",

		1,
		1,

		10, "\\", ""
	};

	//DF_PDX_SPRINTF(footer_str, "// Specify the page to render\r\ndoc.Page = %ld;\r\n", curr_page);
	DT_STRING_CHARS(footer_str, PDX_MAXTEXTLENGTH, "// Specify the page to render\r\ndoc.Page = "); dtStrConcatSLong(&footer_str, curr_page); dtStrConcatChars(&footer_str, ";\r\n");
	conf.Footer2 = footer_str.str;

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "if (doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") != 1) return 0;");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsASPInVBScript(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"<%@ LANGUAGE = VBScript %>\r\n\r\n<%\r\n\r\n",
		"%>\r\n\r\n",

		"<%\r\n' See readme.txt for additional information.\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"'",
		"",
		"",

		"Function PowerDocMake(doc)\r\n\r\n" \
		"\t' PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\tPowerDocMake = 1\r\n\r\n" \
		"End Function\r\n",

		"Dim ", "(", ")",
		1, "Dim ", "(2, 2)",
		"(", ")", ",", -1,
		"0",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd ", "",
		"doc.LinkAdd ", "",
		"doc.PropAdd ", "", "",
		"doc.PropAddAsRef ", "",

		"\r\n\r\n" \
		"' Create and initialize PowerDoc object\r\n" \
		"Set doc = Server.CreateObject(\"PDOC.PowerDoc\")\r\n" \
		"doc.Initialize PD_INI_SHARE, Application(\"pdEngine\")\r\n",

		"",

		"' Set zoom factor\r\n" \
		"doc.Zoom = 1\r\n" \
		"' Set clipping area\r\n" \
		"doc.Clip 0, 0, 400, 400\r\n" \
		"' Make document\r\n" \
		"PowerDocMake doc\r\n" \
		"\r\n" \
		"' Render the page as a PNG image\r\n" \
		"Response.ContentType = \"image/png\"\r\n" \
		"Response.BinaryWrite doc.MakePNG\r\n" \
		"' If you wish to render the page as a JPEG image, remove the above 2 lines and uncomment the following 2 lines\r\n" \
		"' Response.ContentType = \"image/jpeg\"\r\n" \
		"' Response.BinaryWrite doc.MakeJPG\r\n" \
		"\r\n\r\n%>\r\n",

		1,
		1,

		3, "\" & Chr(", ") & \""
	};

	//DF_PDX_SPRINTF(footer_str, "' Specify the page to render\r\ndoc.Page = %ld\r\n", curr_page);
	DT_STRING_CHARS(footer_str, PDX_MAXTEXTLENGTH, "' Specify the page to render\r\ndoc.Page = "); dtStrConcatSLong(&footer_str, curr_page); dtStrConcatChars(&footer_str, "\r\n");
	conf.Footer2 = footer_str.str;

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 0, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "PowerDocMake = 0\r\n\tIf doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") <> 1 Then Exit Function");
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 0, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsASPNETInJScript(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"<% @Page Language=\"JScript\" %>\r\n" \
		"<% @Import Namespace=\"PDOC_dotnetproxy\" %>\r\n" \
		"<%\r\n\r\n",
		"%>\r\n\r\n",

		"<%\r\n// See readme.txt for additional information.\r\n",
		"%>\r\n\r\n",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"<script language=\"JScript\" runat=\"server\">\r\n\r\n" \
		"function PowerDocMake(doc)\r\n" \
		"{\r\n" \
		"\t// PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\treturn 1;\r\n" \
		"}\r\n" \
		"</script>\r\n",

		"var ", " = new Array(", ")",
		0, "var ", " = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]",
		"[", "]", "][", 0,
		"null",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd3(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"\r\n<%\r\n\r\n" \
		"// Create and initialize PowerDoc object\r\n" \
		"var doc = new PowerDocClass();\r\n" \
		"doc.Initialize(InitializerEnum.Pd_INI_SHARE, Application(\"pdEngine\"));\r\n",

		"",

		"// Set zoom factor\r\n" \
		"doc.Zoom = 1;\r\n" \
		"// Set clipping area\r\n" \
		"doc.Clip(0, 0, 400, 400);\r\n" \
		"// Make document\r\n" \
		"PowerDocMake(doc);\r\n" \
		"\r\n" \
		"// Render the page as a PNG image\r\n" \
		"Response.ContentType = \"image/png\";\r\n" \
		"Response.BinaryWrite(doc.MakePNG());\r\n" \
		"// If you wish to render the page as a JPEG image, remove the above 2 lines and uncomment the following 2 lines\r\n" \
		"// Response.ContentType = \"image/jpeg\";\r\n" \
		"// Response.BinaryWrite(doc.MakeJPG());\r\n" \
		"\r\n\r\n%>\r\n",

		1,
		1,

		10, "\\", ""
	};

	//DF_PDX_SPRINTF(footer_str, "// Specify the page to render\r\ndoc.Page = %ld;\r\n", curr_page);
	DT_STRING_CHARS(footer_str, PDX_MAXTEXTLENGTH, "// Specify the page to render\r\ndoc.Page = "); dtStrConcatSLong(&footer_str, curr_page); dtStrConcatChars(&footer_str, ";\r\n");
	conf.Footer2 = footer_str.str;

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 1, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "if (doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") != 1) return 0;");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 1, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsASPNETInVB(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{

		"<% @Page Language=\"VB\" %>\r\n" \
		"<% @Import Namespace=\"PDOC_dotnetproxy\" %>\r\n" \
		"<%\r\n\r\n",
		"%>\r\n\r\n",

		"<%\r\n' See readme.txt for additional information.\r\n" \
		"%>\r\n\r\n",
		"",
		"",

		"\r\n",
		"\t",
		"'",
		"",
		"",

		"<script language=\"VB\" runat=\"server\">\r\n\r\n" \
		"Function PowerDocMake(doc)\r\n\r\n" \
		"\t' PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\tPowerDocMake = 1\r\n\r\n" \
		"End Function\r\n\r\n" \
		"</script>\r\n",

		"Dim ", "(", ")",
		1, "Dim ", "(2, 2)",
		"(", ")", ",", -1,
		"0",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"\r\n<%\r\n\r\n" \
		"' Create and initialize PowerDoc object\r\n" \
		"Dim doc As PowerDocClass\r\ndoc = new PowerDocClass()\r\n" \
		"doc.Initialize(InitializerEnum.Pd_INI_SHARE, Application(\"pdEngine\"))\r\n",

		"",

		"' Set zoom factor\r\n" \
		"doc.Zoom = 1\r\n" \
		"' Set clipping area\r\n" \
		"doc.Clip(0, 0, 400, 400)\r\n" \
		"' Make document\r\n" \
		"PowerDocMake(doc)\r\n" \
		"\r\n" \
		"' Render the page as a PNG image\r\n" \
		"Response.ContentType = \"image/png\"\r\n" \
		"Response.BinaryWrite(doc.MakePNG())\r\n" \
		"' If you wish to render the page as a JPEG image, remove the above 2 lines and uncomment the following 2 lines\r\n" \
		"' Response.ContentType = \"image/jpeg\"\r\n" \
		"' Response.BinaryWrite(doc.MakeJPG())\r\n" \
		"\r\n\r\n%>\r\n",

		1,
		1,

		3, "\" & Chr(", ") & \""
	};

	//DF_PDX_SPRINTF(footer_str, "' Specify the page to render\r\ndoc.Page = %ld\r\n", curr_page);
	DT_STRING_CHARS(footer_str, PDX_MAXTEXTLENGTH, "' Specify the page to render\r\ndoc.Page = "); dtStrConcatSLong(&footer_str, curr_page); dtStrConcatChars(&footer_str, "\r\n");
	conf.Footer2 = footer_str.str;

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 1, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "PowerDocMake = 0\r\n\tIf doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\") <> 1 Then Exit Function");
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 1, &conf, body_str_ptr);
}


DT_SLONG pdxDocExportAsASPNETInCS(DT_PDDOC power_doc, DT_SLONG curr_page, const DT_CHAR* file_name, DT_SLONG obj)
{
	DT_CHAR* body_str_ptr = DV_NULL;
	DT_STRING_CHARS(body_str, PDX_MAXTEXTLENGTH, "");
	DT_PDX_EXPORT_CONFIG conf =
	{
		"<% @Page Language=\"C#\" %>\r\n" \
		"<% @Import Namespace=\"PDOC_dotnetproxy\" %>\r\n" \
		"<%\r\n\r\n",
		"%>\r\n\r\n",

		"<%\r\n// See readme.txt for additional information.\r\n",
		"%>\r\n\r\n",
		"",

		"\r\n",
		"\t",
		"//",
		"",
		";",

		"<script language=\"C#\" runat=\"server\">\r\n\r\n" \
		"public object PowerDocMake(PowerDocClass doc)\r\n" \
		"{\r\n" \
		"\t// PowerDoc Make Script v1.0\r\n\r\n",

		"\r\n" \
		"\treturn 1;\r\n" \
		"}\r\n" \
		"</script>\r\n",

		"int[] ", " = new int[", "]",
		0, "double[,] ", " = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]",
		"[", "]", "][", 0,
		"null",

		"doc.ObjAdd(", ")",
		"doc.ImgAdd3(", ")",
		"doc.LinkAdd(", ")",
		"doc.PropAdd(", "", ")",
		"doc.PropAddAsRef(", ")",

		"\r\n<%\r\n\r\n" \
		"// Create and initialize PowerDoc object\r\n" \
		"PowerDocClass doc;\r\ndoc = new PowerDocClass();\r\n" \
		"doc.Initialize(InitializerEnum.Pd_INI_SHARE, Application[\"pdEngine\"]);\r\n",

		"",

		"// Set zoom factor\r\n" \
		"doc.Zoom = 1;\r\n" \
		"// Set clipping area\r\n" \
		"doc.Clip(0, 0, 400, 400);\r\n" \
		"// Make document\r\n" \
		"PowerDocMake(doc);\r\n" \
		"\r\n" \
		"// Render the page as a PNG image\r\n" \
		"Response.ContentType = \"image/png\";\r\n" \
		"Response.BinaryWrite((byte[])doc.MakePNG(ColorModeEnum.Pd_COLOR, 255,255,255, 0,0,0, CropModeEnum.Pd_NO_CROP, TransparencyEnum.Pd_NO_TRANSPARENCY));\r\n" \
		"// If you wish to render the page as a JPEG image, remove the above 2 lines and uncomment the following 2 lines\r\n" \
		"// Response.ContentType = \"image/jpeg\";\r\n" \
		"// Response.BinaryWrite((byte[])doc.MakeJPG(ColorModeEnum.Pd_COLOR, 255,255,255, CropModeEnum.Pd_NO_CROP, 90));\r\n" \
		"\r\n\r\n%>\r\n",

		0,
		0,

		10, "\\", ""
	};

	//DF_PDX_SPRINTF(footer_str, "// Specify the page to render\r\ndoc.Page = %ld;\r\n", curr_page);
	DT_STRING_CHARS(footer_str, PDX_MAXTEXTLENGTH, "// Specify the page to render\r\ndoc.Page = "); dtStrConcatSLong(&footer_str, curr_page); dtStrConcatChars(&footer_str, ";\r\n");
	conf.Footer2 = footer_str.str;

	if (obj >= 0)
	{
		conf.Tab = "";
		return pdxObjExport2(power_doc, file_name, obj, 1, &conf);
	}

	else if (obj == -2)
	{
		dtStrConcatChars(&body_str, "doc.AppendFromFile(0, 0, 0, \""); dtStrConcatChars(&body_str, file_name); dtStrConcatChars(&body_str, ".pdc\");");
		pdxStringReplaceChr(body_str.str, '\\', '/');
		body_str_ptr = body_str.str;
	}

	return pdxDocExport(power_doc, file_name, 1, &conf, body_str_ptr);
}


/* Rendering parameters */


void pdxDrawParamsReset(DT_PD_DOCDRAW_PARAMS* params)
{
	params->Flags = 0;
	params->Origins = 0;
	params->Boundaries = 0;
	params->Background = 0;
	params->Flows = 0;
	params->Reserved = 0;
	params->R = params->G = params->B = params->A = 255;
	params->FlowsArr.Len = 0;
	params->FlowsArr.Ptr = DV_NULL;
	params->UserData = DV_NULL;
}


DT_SWORD pdxBitmapCrop(const DT_PDX_BITMAP* bitmap, DT_UBYTE crop_type, const DT_UBYTE rgbt[], DT_SLONG* cx, DT_SLONG* cy, DT_SLONG* cw, DT_SLONG* ch)
{
	if (bitmap == DV_NULL) return 0;

	const DT_UBYTE* pixels = bitmap->MDC.m;
	DT_SLONG w = bitmap->MDC.w;
	DT_SLONG h = bitmap->MDC.h; if (h < 0) h = -h;

	if (pixels == DV_NULL || w < 0) return 0;

	DT_ID_SWORD format = bitmap->Format;
	DT_SLONG xmin = w - 1, ymin = h - 1, xmax = 0, ymax = 0;
	DT_SLONG i, j;
	DT_SWORD bpp;
	DT_UBYTE r = rgbt[0], g = rgbt[1], b = rgbt[2], t = rgbt[3];

	*cx = 0;
	*cy = 0;
	*cw = w;
	*ch = h;

	if (format == 24)
	{
		bpp = 3; if (crop_type < 1) return bpp;

		for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
		{
			if (*pixels != b || *(pixels + 1) != g || *(pixels + 2) != r)
			{
				if (j <= xmin) xmin = j; else if (j >= xmax) xmax = j;
				if (i <= ymin) ymin = i; else if (i >= ymax) ymax = i;
			}

			pixels += 3;
		}
	}
	else if (format == 8)
	{
		bpp = 1; if (crop_type < 1) return bpp;

		for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
		{
			if (*pixels != t)
			{
				if (j <= xmin) xmin = j; else if (j >= xmax) xmax = j;
				if (i <= ymin) ymin = i; else if (i >= ymax) ymax = i;
			}

			pixels++;
		}
	}
	else return 0;

	if (xmax <= xmin)
	{
		xmin = 0;
		xmax = 0;
	}
	if (ymax <= ymin)
	{
		ymin = 0;
		ymax = 0;
	}

	if (crop_type != 1)
	{
		*cx = xmin;
		*cw = xmax - xmin + 1;
	}
	if (crop_type != 2)
	{
		*cy = ymin;
		*ch = ymax - ymin + 1;
	}

	return bpp;
}


void pdxBitmapDrawGrid(const DT_PDX_BITMAP* bitmap, DT_SLONG x_off, DT_SLONG y_off, DT_SLONG dw, DT_SLONG dh, DT_FLOAT zoom)
{
	if (bitmap == DV_NULL || dw <= 0 || dh <= 0) return;

	DT_UBYTE* pixels = bitmap->MDC.m;
	DT_SLONG w = bitmap->MDC.w;
	DT_SLONG h = bitmap->MDC.h; if (h < 0) h = -h;

	if (pixels == DV_NULL || w < 0) return;

	DT_SLONG bpp = bitmap->Format / 8;
	DT_SLONG x_pix, y_pix;
	DT_SLONG x, y, x0, y0, x1, y1;
	DT_SLONG pitch = DF_CALC_PITCH(bpp, &bitmap->MDC);
	DT_UBYTE* m0;
	DT_UBYTE* m;

	if (zoom * dw < 5) dw *= 1 + DF_ROUND(5 / (zoom * dw));
	if (zoom * dh < 5) dh *= 1 + DF_ROUND(5 / (zoom * dh));

	x0 = ((DF_ROUND(x_off / zoom)) / dw - 1) * dw;
	y0 = ((DF_ROUND(y_off / zoom)) / dh - 1) * dh;
	x1 = ((DF_ROUND((w + x_off) / zoom)) / dw + 1) * dw;
	y1 = ((DF_ROUND((h + y_off) / zoom)) / dh + 1) * dh;

	if (bitmap->Format == 24 || bitmap->Format == 32)
	{
		for (y = y0; y < y1; y += dh)
		{
			y_pix = DF_ROUND(y * zoom) - y_off; if (y_pix < 0 || y_pix >= h) continue;

			if (bitmap->MDC.h < 0) m0 = pixels + pitch * y_pix; else m0 = pixels + pitch * (h - 1 - y_pix);

			for (x = x0; x < x1; x += dw)
			{
				x_pix = DF_ROUND(x * zoom) - x_off;
				if (x_pix < 0 || x_pix >= w) continue;

				m = m0 + bpp * x_pix;
				*m = ~(*m); m++;
				*m = ~(*m); m++;
				*m = ~(*m);
			}
		}
	}
	else if (bitmap->Format == 8)
	{
		for (y = y0; y < y1; y += dh)
		{
			y_pix = DF_ROUND(y * zoom) - y_off; if (y_pix < 0 || y_pix >= h) continue;

			if (bitmap->MDC.h < 0) m0 = pixels + pitch * y_pix; else m0 = pixels + pitch * (h - 1 - y_pix);

			for (x = x0; x < x1; x += dw)
			{
				x_pix = DF_ROUND(x * zoom) - x_off;
				if (x_pix < 0 || x_pix >= w) continue;

				m = m0 + x_pix;
				*m = ~(*m);
			}
		}
	}
}


void pdxBitmapFree(DT_PDX_BITMAP* bitmap)
{
	if (bitmap == DV_NULL) return;
	if (bitmap->LCD) DF_PDX_MEM_FREE(bitmap->MDC.m); else pdFree(bitmap->MDC.m);
}


/* Rendering and Output */


void pdxMakeLCD2OptimizedColorBitmap(const DT_UBYTE* src, DT_UBYTE* dst, DT_SLONG w_clip, DT_SLONG h_clip, const DT_UWORD contrib[][3], DT_ID_UBYTE rgb_flag, DT_SWORD format)
{
	DT_SLONG i, ix, iy;
	DT_SLONG c0, c1, c2;
	DT_UWORD c00[256], c01[256], c02[256];
	DT_UWORD c10[256], c11[256], c12[256];
	DT_UWORD c20[256], c21[256], c22[256];
	DT_SWORD d = -1, a1 = 0, b1 = 8, a2, a3, b2, b3; /* for BGR ordering */

	if (rgb_flag)
	{
		d = 1;
		a1 = 2;
		b1 = 6; /* for RGB ordering */
	}

	a2 = a1 + 3;
	a3 = a1 + 6;
	b2 = b1 - 3;
	b3 = b1 - 6;

	for (i = 0; i < 256; i++)
	{
		c00[i] = (DT_UWORD)(i * contrib[0][0]);
		c01[i] = (DT_UWORD)(i * contrib[0][1]);
		c02[i] = (DT_UWORD)(i * contrib[0][2]);

		c10[i] = (DT_UWORD)(i * contrib[1][0]);
		c11[i] = (DT_UWORD)(i * contrib[1][1]);
		c12[i] = (DT_UWORD)(i * contrib[1][2]);

		c20[i] = (DT_UWORD)(i * contrib[2][0]);
		c21[i] = (DT_UWORD)(i * contrib[2][1]);
		c22[i] = (DT_UWORD)(i * contrib[2][2]);
	}

	dst++;

	if (format == 24)
	{
		for (iy = 0; iy < h_clip; iy++, src += 3)
		for (ix = 0; ix < w_clip; ix++, dst += 3)
		{
			/* R or B */
			c0 = c00[*(src + a1)]; /**/
			c1 = c01[*(src + a2)];
			c2 = c02[*(src + a3)];
			*(dst + d) = (DT_UBYTE)((c0 + c1 + c2) >> 8);

			/* G */
			c0 = c10[*(src + 1)];
			c1 = c11[*(src + 4)]; /**/
			c2 = c12[*(src + 7)];
			*(dst    ) = (DT_UBYTE)((c0 + c1 + c2) >> 8);

			/* B or R */
			c2 = c20[*(src + b3)];
			c1 = c21[*(src + b2)];
			c0 = c22[*(src + b1)]; /**/
			*(dst - d) = (DT_UBYTE)((c0 + c1 + c2) >> 8);

			src += 6;
		}
	}
	else if (format == 8)
	{
		for (iy = 0; iy < h_clip; iy++, src++)
		for (ix = 0; ix < w_clip; ix++, dst += 3)
		{
			/* R or B */
			c0 = c00[*(src    )]; /**/
			c1 = c01[*(src + 1)];
			c2 = c02[*(src + 2)];
			*(dst + d) = (DT_UBYTE)((c0 + c1 + c2) >> 8);

			/* G */
			c0 = c10[*(src    )];
			c1 = c11[*(src + 1)]; /**/
			c2 = c12[*(src + 2)];
			*(dst    ) = (DT_UBYTE)((c0 + c1 + c2) >> 8);

			/* B or R */
			c2 = c20[*(src    )];
			c1 = c21[*(src + 1)];
			c0 = c22[*(src + 2)]; /**/
			*(dst - d) = (DT_UBYTE)((c0 + c1 + c2) >> 8);

			src += 2;
		}
	}
}


void pdxMakeLCD1OptimizedColorBitmap(const DT_UBYTE* src, DT_UBYTE* dst, DT_SLONG w_clip, DT_SLONG h_clip, const DT_UWORD contrib[][5], DT_ID_UBYTE rgb_flag, DT_SWORD format)
{
	/* Note: The width of the source bitmap must be exactly 3 * WClip + 2 so that the width of the destination bitmap is WClip */

	DT_SLONG i, ix, iy;
	DT_SLONG c0, c1, c2, c3, c4;
	DT_UWORD c00[256], c01[256], c02[256], c03[256], c04[256];
	DT_UWORD c10[256], c11[256], c12[256], c13[256], c14[256];
	DT_UWORD c20[256], c21[256], c22[256], c23[256], c24[256];
	DT_SWORD d = -1, n2 = 4, n1 = 1; /* for BGR ordering */

	if (rgb_flag)
	{
		d = 1;
		n2 = 2;
		n1 = 5; /* for RGB ordering */
		if (format == 24) src += 2;
	}

	for (i = 0; i < 256; i++)
	{
		c00[i] = (DT_UWORD)(i * contrib[0][0]);
		c01[i] = (DT_UWORD)(i * contrib[0][1]);
		c02[i] = (DT_UWORD)(i * contrib[0][2]);
		c03[i] = (DT_UWORD)(i * contrib[0][3]);
		c04[i] = (DT_UWORD)(i * contrib[0][4]);

		c10[i] = (DT_UWORD)(i * contrib[1][0]);
		c11[i] = (DT_UWORD)(i * contrib[1][1]);
		c12[i] = (DT_UWORD)(i * contrib[1][2]);
		c13[i] = (DT_UWORD)(i * contrib[1][3]);
		c14[i] = (DT_UWORD)(i * contrib[1][4]);

		c20[i] = (DT_UWORD)(i * contrib[2][0]);
		c21[i] = (DT_UWORD)(i * contrib[2][1]);
		c22[i] = (DT_UWORD)(i * contrib[2][2]);
		c23[i] = (DT_UWORD)(i * contrib[2][3]);
		c24[i] = (DT_UWORD)(i * contrib[2][4]);
	}


	dst++;

	if (format == 24)
	{
		for (iy = 0; iy < h_clip; iy++, src += 6)
		for (ix = 0; ix < w_clip; ix++, dst += 3)
		{
			/* R or B */
			c0 = c00[*(src     )];
			c1 = c01[*(src +  3)];
			c2 = c02[*(src +  6)];
			c3 = c03[*(src +  9)];
			c4 = c04[*(src + 12)];
			*(dst + d) = (DT_UBYTE)((c0 + c1 + c2 + c3 + c4) >> 8);
			src += n2;

			/* G */
			c0 = c10[*(src -  3)];
			c1 = c11[*(src     )];
			c2 = c12[*(src +  3)];
			c3 = c13[*(src +  6)];
			c4 = c14[*(src +  9)];
			*(dst    ) = (DT_UBYTE)((c0 + c1 + c2 + c3 + c4) >> 8);
			src += n2;

			/* B or R */
			c0 = c20[*(src -  6)];
			c1 = c21[*(src -  3)];
			c2 = c22[*(src     )];
			c3 = c23[*(src +  3)];
			c4 = c24[*(src +  6)];
			*(dst - d) = (DT_UBYTE)((c0 + c1 + c2 + c3 + c4) >> 8);
			src += n1;
		}
	}
	else if (format == 8)
	{
		for (iy = 0; iy < h_clip; iy++, src += 2)
		for (ix = 0; ix < w_clip; ix++, dst += 3)
		{
			/* R or B */
			c0 = c00[*(src    )];
			c1 = c01[*(src + 1)];
			c2 = c02[*(src + 2)];
			c3 = c03[*(src + 3)];
			c4 = c04[*(src + 4)];
			*(dst + d) = (DT_UBYTE)((c0 + c1 + c2 + c3 + c4) >> 8);
			src++;

			/* G */
			c0 = c10[*(src - 1)];
			c1 = c11[*(src    )];
			c2 = c12[*(src + 1)];
			c3 = c13[*(src + 2)];
			c4 = c14[*(src + 3)];
			*(dst    ) = (DT_UBYTE)((c0 + c1 + c2 + c3 + c4) >> 8);
			src++;

			/* B or R */
			c0 = c20[*(src - 2)];
			c1 = c21[*(src - 1)];
			c2 = c22[*(src    )];
			c3 = c23[*(src + 1)];
			c4 = c24[*(src + 2)];
			*(dst - d) = (DT_UBYTE)((c0 + c1 + c2 + c3 + c4) >> 8);
			src++;
		}
	}
}


DT_SLONG pdxDocDraw(DT_DOC doc, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x_off, DT_SLONG y_off, DT_SLONG width, DT_SLONG height, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, DT_PDX_BITMAP* bitmap)
{
	DT_SLONG diff;
	DT_SLONG ret = 0;
	DT_SLONG x = x_off, y = y_off;
	DT_UBYTE* rgb_decimated_memory = DV_NULL;

	DT_UWORD gray_contrib_lib[11] = {0, 26, 51, 77, 102, 128, 154, 180, 205, 230, 256};

	DT_SWORD lib_id_gray = (DT_SWORD)(DF_PDX_ABS(lcd_mode) / 100);
	DT_SWORD lib_id_rgb = (DT_SWORD)(DF_PDX_ABS(lcd_mode) - lib_id_gray * 100 - 1);

	DT_UWORD final_contrib_lcd1[3][5];
	DT_UWORD final_contrib_lcd2[3][3];

	//DT_TM3X3 tm = {{zoom, 0.0, 0.0}, {0.0, zoom, 0.0}, {0.001, 0.0, 1.0}}; /* for testing purposes */
	DT_TM3X3 tm =
	{
		{zoom, 0.0, 0.0},
		{0.0, zoom, 0.0},
		{0.0, 0.0, 1.0}
	};
	DT_FLOAT tm00 = tm[0][0];


	bitmap->LCD = 0;
	bitmap->Format = format;
	bitmap->Subformat = subformat;
	bitmap->MDC.w = width;
	bitmap->MDC.h = height;
	bitmap->MDC.m = DV_NULL;
	bitmap->MDC.l = 0;


	if (lib_id_gray < 0 || lib_id_gray > 10) {}

	else if (lib_id_rgb >= 0 && lib_id_rgb <= 3)
	{
		/* LCD1 Mode */

		/* The sum of all 3 color component contributions in all 3 arrays must equal 256 to preserve color balance */
		DT_UWORD rgb_contrib_lib[4][3][3] =
		{
			{
				{85, 85, 86},
				{85, 86, 85},
				{86, 85, 85}
			},
			{
				{64, 64, 128},
				{64, 128, 64},
				{128, 64, 64}
			},
			{
				{42, 86, 128},
				{86, 128, 42},
				{128, 42, 86}
			},
			{
				{128, 128, 0},
				{85, 86, 85},
				{0, 128, 128}
			}
		};

		/* kg = 179; */ /* 0.7 */ /* kc = 256 - kg; */ /* 1 - kg */
		DT_SLONG kg = gray_contrib_lib[lib_id_gray], kc = 256 - kg;
		DT_SLONG kc_m9 = kc * 9;
		DT_SLONG kg_m256_m1 = kg * 256, kg_m256_m2 = kg_m256_m1 * 2, kg_m256_m3 = kg_m256_m1 * 3;
		DT_SLONG kg_m2_d9 = (kg * 2) / 9, kg_d9 = kg / 9;
		DT_SLONG m256_m9 = 256 * 9;

		final_contrib_lcd1[0][0] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][0][0] + kg_m256_m1) / m256_m9);
		final_contrib_lcd1[0][1] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][0][1] + kg_m256_m2) / m256_m9);
		final_contrib_lcd1[0][2] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][0][2] + kg_m256_m3) / m256_m9);
		final_contrib_lcd1[0][3] = (DT_UWORD)kg_m2_d9;
		final_contrib_lcd1[0][4] = (DT_UWORD)kg_d9;
		if ((diff = 256 - (final_contrib_lcd1[0][0] + final_contrib_lcd1[0][1] + final_contrib_lcd1[0][2] + final_contrib_lcd1[0][3] + final_contrib_lcd1[0][4])) != 0) final_contrib_lcd1[0][2] = (DT_UWORD)(final_contrib_lcd1[0][2] + diff);

		final_contrib_lcd1[1][0] = (DT_UWORD)kg_d9;
		final_contrib_lcd1[1][1] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][1][0] + kg_m256_m2) / m256_m9);
		final_contrib_lcd1[1][2] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][1][1] + kg_m256_m3) / m256_m9);
		final_contrib_lcd1[1][3] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][1][2] + kg_m256_m2) / m256_m9);
		final_contrib_lcd1[1][4] = (DT_UWORD)kg_d9;
		if ((diff = 256 - (final_contrib_lcd1[1][0] + final_contrib_lcd1[1][1] + final_contrib_lcd1[1][2] + final_contrib_lcd1[1][3] + final_contrib_lcd1[1][4])) != 0) final_contrib_lcd1[1][2] = (DT_UWORD)(final_contrib_lcd1[1][2] + diff);

		final_contrib_lcd1[2][0] = (DT_UWORD)kg_d9;
		final_contrib_lcd1[2][1] = (DT_UWORD)kg_m2_d9;
		final_contrib_lcd1[2][2] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][2][0] + kg_m256_m3) / m256_m9);
		final_contrib_lcd1[2][3] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][2][1] + kg_m256_m2) / m256_m9);
		final_contrib_lcd1[2][4] = (DT_UWORD)((kc_m9 * rgb_contrib_lib[lib_id_rgb][2][2] + kg_m256_m1) / m256_m9);
		if ((diff = 256 - (final_contrib_lcd1[2][0] + final_contrib_lcd1[2][1] + final_contrib_lcd1[2][2] + final_contrib_lcd1[2][3] + final_contrib_lcd1[2][4])) != 0) final_contrib_lcd1[2][2] = (DT_UWORD)(final_contrib_lcd1[2][2] + diff);


		if ((rgb_decimated_memory = static_cast<DT_UBYTE*>(DF_PDX_MEM_ALLOC(DF_PDX_SEC_MUL(DF_PDX_SEC_MUL(width, height), 3)))) != DV_NULL)
		{
			/* If rendering in LCDMode 1, the width of the original bitmap must be exactly 3*Width+2 */
			bitmap->LCD = 1;
			bitmap->MDC.w = 3 * width + 2;
			tm[0][0] *= 3;
			x *= 3;
		}
	}
	else if (lib_id_rgb >= 10 && lib_id_rgb <= 13)
	{
		/* LCD2 Mode */

		lib_id_rgb -= 10;

		/* The sum of all 3 color component contributions in all 3 arrays must equal 256 to preserve color balance */
		DT_UWORD rgb_contrib_lib[4][3][3] =
		{
			{
				{160, 64, 32},
				{85, 86, 85},
				{32, 64, 160}
			},
			//{{128, 86, 42}, {77, 102, 77}, {42, 86, 128}},
			//{{128, 96, 32}, {48, 160, 48}, {32, 96, 128}},
			{
				{192, 21, 18},
				{77, 102, 77},
				{18, 21, 128}
			},
			{
				{224, 10, 6},
				{48, 160, 48},
				{6, 10, 224}
			},
			{
				{256, 0, 0},
				{0, 256, 0},
				{0, 0, 256}
			}
		};

		/* kg = 179; */ /* 0.7 */ /* kc = 256 - kg; */ /* 1 - kg */
		DT_SLONG kg = gray_contrib_lib[lib_id_gray], kc = 256 - kg;
		DT_SLONG kc_m4 = kc * 4;
		DT_SLONG kg_m256_m1 = kg * 256, kg_m256_m2 = kg_m256_m1 * 2;
		DT_SLONG m256_m4 = 256 * 4;

		final_contrib_lcd2[0][0] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][0][0] + kg_m256_m1) / m256_m4);
		final_contrib_lcd2[0][1] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][0][1] + kg_m256_m2) / m256_m4);
		final_contrib_lcd2[0][2] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][0][2] + kg_m256_m1) / m256_m4);
		if ((diff = 256 - (final_contrib_lcd2[0][0] + final_contrib_lcd2[0][1] + final_contrib_lcd2[0][2])) != 0) final_contrib_lcd2[0][1] = (DT_UWORD)(final_contrib_lcd2[0][1] + diff);

		final_contrib_lcd2[1][0] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][1][0] + kg_m256_m1) / m256_m4);
		final_contrib_lcd2[1][1] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][1][1] + kg_m256_m2) / m256_m4);
		final_contrib_lcd2[1][2] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][1][2] + kg_m256_m1) / m256_m4);
		if ((diff = 256 - (final_contrib_lcd2[1][0] + final_contrib_lcd2[1][1] + final_contrib_lcd2[1][2])) != 0) final_contrib_lcd2[1][1] = (DT_UWORD)(final_contrib_lcd2[1][1] + diff);

		final_contrib_lcd2[2][0] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][2][0] + kg_m256_m1) / m256_m4);
		final_contrib_lcd2[2][1] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][2][1] + kg_m256_m2) / m256_m4);
		final_contrib_lcd2[2][2] = (DT_UWORD)((kc_m4 * rgb_contrib_lib[lib_id_rgb][2][2] + kg_m256_m1) / m256_m4);
		if ((diff = 256 - (final_contrib_lcd2[2][0] + final_contrib_lcd2[2][1] + final_contrib_lcd2[2][2])) != 0) final_contrib_lcd2[2][1] = (DT_UWORD)(final_contrib_lcd2[2][1] + diff);


		if ((rgb_decimated_memory = static_cast<DT_UBYTE*>(DF_PDX_MEM_ALLOC(DF_PDX_SEC_MUL(DF_PDX_SEC_MUL(width, height), 3)))) != DV_NULL)
		{
			/* If rendering in LCDMode 2, the width of the original bitmap must be exactly 2*Width+1 */
			bitmap->LCD = 2;
			bitmap->MDC.w = 2 * width + 1;
			tm[0][0] *= 2;
			x *= 2;
		}
	}


	/* Set LCD transform and render the page */

#ifdef DEF_PDX_TXDRAW_HACK
	txTextSetTransformPlus(doc, tm);
	if ((ret = txTextDraw(doc, page, x, y, format, subformat, &bitmap->MDC, params, 1)) != 1) return 0;
#else
	pdDocSetTransformPlus(doc, tm);
	if ((ret = pdDocDraw(doc, page, x, y, format, subformat, &bitmap->MDC, params)) != 1) return 0;
#endif


	/* Optimize for LCD screens */
	if (bitmap->LCD)
	{
		/* Convert the original bitmap to LCD optimized (stretched) bitmap */
		if (bitmap->LCD == 2)
			pdxMakeLCD2OptimizedColorBitmap(bitmap->MDC.m, rgb_decimated_memory, width, height, final_contrib_lcd2, lcd_mode > 0, bitmap->Format);
		else
			pdxMakeLCD1OptimizedColorBitmap(bitmap->MDC.m, rgb_decimated_memory, width, height, final_contrib_lcd1, lcd_mode > 0, bitmap->Format);

		/* Discard the original bitmap */
		pdFree(bitmap->MDC.m);

		/* This is now our LCD optimized bitmap */
		bitmap->Format = 24; /* This is always an RGB bitmap */
		bitmap->MDC.m = rgb_decimated_memory;
		bitmap->MDC.w = width;
		bitmap->MDC.h = height;
		bitmap->MDC.l = 3 * width * height; /* the length is 3*Width*Height because this is always an RGB bitmap */
	}


	/* Set normal transform */
	tm[0][0] = tm00;
#ifdef DEF_PDX_TXDRAW_HACK
	txTextSetTransformPlus(doc, tm);
#else
	pdDocSetTransformPlus(doc, tm);
#endif

	return ret;
}


DT_SLONG pdxDocSaveAsRawImage(DT_UBYTE bmp_format, const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* /*gattribs*/, const DT_PDX_XYWH_SLONG* clip)
{
	DT_UBYTE buff[128];
	DT_UBYTE* tmp = buff;
	DT_DTSTREAM stream = DV_NULL;
	DT_ULONG bpr, bpr_padded, len, l, padding = 0, palette_size = 0;
	DT_SLONG i;
	DT_SLONG ret = 0;
	DT_PDX_BITMAP bitmap;

	if (pdxDocDraw(doc, lcd_mode, page, zoom, x + clip->X, y + clip->Y, clip->W, clip->H, format, subformat, params, &bitmap) != 1) return 0;

	bpr = clip->W * (bitmap.Format / 8);
	len = bpr * clip->H;

	DT_STREAM_FPTR(sd_1, stdout);
	DT_STREAM_FILE(sd_2, file_name);

	const DT_STREAM_DESC* sd = &sd_1; if (file_name != DV_NULL) sd = &sd_2;

	if ((stream = dtStreamOpenW(sd, DV_STREAM_OPEN_MUST, 0)) == DV_NULL) goto end;

	if (bmp_format)
	{
		bpr_padded = (bpr >> 2) << 2;
		if (bpr != bpr_padded) bpr_padded += 4;
		padding = bpr_padded - bpr;
		if (bitmap.Format == 8) palette_size = 256 * 4;

		/* Windows BMP Header */
		DF_WRITE_UBYTE_LE(tmp, 'B'); tmp++; /* bmp_head->bfType (66) */
		DF_WRITE_UBYTE_LE(tmp, 'M'); tmp++; /* bmp_head->bfType (77) */
		DF_WRITE_ULONG_LE(tmp, bpr_padded * clip->H + 54 + palette_size + 2); tmp += 4; /* bmp_head->bfSize */
		DF_WRITE_UWORD_LE(tmp, 0); tmp += 2; /* bmp_head->bfReserved1 */
		DF_WRITE_UWORD_LE(tmp, 0); tmp += 2; /* bmp_head->bfReserved2 */
		DF_WRITE_ULONG_LE(tmp, 54 + palette_size); tmp += 4; /* bmp_head->bfOffBits */
		DF_WRITE_ULONG_LE(tmp, 40); tmp += 4; /* bmp_info->biSize (BITMAPINFOHEADER_SIZE) */
		DF_WRITE_SLONG_LE(tmp, clip->W); tmp += 4; /* bmp_info->biWidth */
		DF_WRITE_SLONG_LE(tmp, clip->H); tmp += 4; /* bmp_info->biHeight */
		DF_WRITE_UWORD_LE(tmp, 1); tmp += 2; /* bmp_info->biPlanes */
		DF_WRITE_UWORD_LE(tmp, bitmap.Format); tmp += 2; /* bmp_info->biBitCount */
		DF_WRITE_ULONG_LE(tmp, 0); tmp += 4; /* bmp_info->biCompression */
		DF_WRITE_ULONG_LE(tmp, 0); tmp += 4; /* bmp_info->biSizeImage */
		DF_WRITE_SLONG_LE(tmp, 2834); tmp += 4; /* bmp_info->biXPelsPerMeter */
		DF_WRITE_SLONG_LE(tmp, 2834); tmp += 4; /* bmp_info->biYPelsPerMeter */
		DF_WRITE_ULONG_LE(tmp, 0); tmp += 4; /* bmp_info->biClrUsed */
		DF_WRITE_ULONG_LE(tmp, 0); tmp += 4; /* bmp_info->biClrImportant */

		l = static_cast<DT_SLONG>(tmp - buff);
		if (dtStreamWrite(stream, buff, l, 0) != 1) goto end;

		if (bitmap.Format == 8) /* write palette for grayscale images */
		{
			for (i = 0; i <= 255; i++)
			{
				DF_PDX_MEMSET(buff, static_cast<DT_UBYTE>(i), 4);
				if (dtStreamWrite(stream, buff, 4, 0) != 1) goto end;
			}
		}
	}


	DF_PDX_MEMSET(buff, 0, 128);

	if (orientation == 0)
	{
		tmp = bitmap.MDC.m;
		for (i = 0; i < clip->H; i++)
		{
			if (dtStreamWrite(stream, tmp, bpr, 0) != 1) goto end;
			tmp += bpr;

			if (padding <= 0) continue;
			if (dtStreamWrite(stream, buff, padding, 0) != 1) goto end;
		}
	}
	else
	{
		tmp = bitmap.MDC.m + len;
		for (i = 0; i < clip->H; i++)
		{
			tmp -= bpr;
			if (dtStreamWrite(stream, tmp, bpr, 0) != 1) goto end;

			if (padding <= 0) continue;
			if (dtStreamWrite(stream, buff, padding, 0) != 1) goto end;
		}
	}


	if (bmp_format)
	{
		/* Windows BMP Footer */
		if (dtStreamWrite(stream, buff, 2, 0) != 1) goto end;
	}


	/* Everything was OK */
	ret = 1;


end:

	if (stream != DV_NULL) dtStreamClose(stream, DV_STREAM_CLOSE_YES);
	pdxBitmapFree(&bitmap);
	return ret;
}


DT_SLONG pdxDocSaveAsRAW(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	return pdxDocSaveAsRawImage(0, file_name, doc, orientation, lcd_mode, page, zoom, x, y, format, subformat, params, gattribs, clip);
}


DT_SLONG pdxDocSaveAsBMP(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	return pdxDocSaveAsRawImage(1, file_name, doc, orientation, lcd_mode, page, zoom, x, y, format, subformat, params, gattribs, clip);
}


DT_SLONG pdxStreamWriteFn(void* user_struct, const DT_UBYTE* data, DT_SLONG length)
{
	DT_PDX_STREAM_BUFFER* buff_ptr = static_cast<DT_PDX_STREAM_BUFFER*>(user_struct);

	if (buff_ptr->Error) return 0;

	DT_SLONG new_len = DF_PDX_SEC_ADD(buff_ptr->CurrPos, length);
	DT_UBYTE* new_addr = static_cast<DT_UBYTE*>(DF_PDX_MEM_REALLOC(buff_ptr->Buffer, new_len)); if (new_addr == DV_NULL) { buff_ptr->Error = 1; return 0; }

	buff_ptr->Buffer = new_addr;

	new_addr += buff_ptr->CurrPos;
	DF_PDX_MEMCPY(new_addr, data, length);

	buff_ptr->CurrPos = new_len;
	return length;
}


DT_SLONG pdxDocRenderAsCompressedImage(DT_UWORD compression_type, DT_UBYTE** img_memory, DT_DOC doc, DT_UBYTE orientation, DT_UBYTE close_crop, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	DT_AUX_INFO info = {DEF_AUX_INIT_KEY, DEF_AUX_INIT_FLAG, 0, 0, compression_type, DV_NULL};
	DT_PDX_STREAM_BUFFER user_struct = {0, 0, DV_NULL};
	DT_AUX_USER_STREAM user_stream = {&user_struct, DV_NULL, pdxStreamWriteFn, DV_NULL};
	DT_PDX_BITMAP bitmap, mask;
	DT_SLONG ret = 0, img_len = 0;
	DT_SLONG cx = 0, cy = 0, cw = clip->W, ch = clip->H;
	DT_SLONG bitmap_padding = 0;
	DT_SLONG mask_padding = 0;
	DT_SLONG bpp = 1;
	DT_SWORD use_mask = 0;

	*img_memory = DV_NULL;

	if (compression_type == DV_AUX_FILEFORMAT_PNG && gattribs->TransparencyFlag == 3) { use_mask = 1; params->R = params->G = params->B = params->A = 255; }

	if (pdxDocDraw(doc, lcd_mode, page, zoom, x + clip->X, y + clip->Y, clip->W, clip->H, format, subformat, params, &bitmap) != 1) return 0;

	DT_UBYTE* bitmap_memory = bitmap.MDC.m;
	DT_UBYTE* mask_memory = DV_NULL;

	if (use_mask && pdxDocDraw(doc, lcd_mode, page, zoom, x + clip->X, y + clip->Y, clip->W, clip->H, 8, 1, params, &mask) == 1) mask_memory = mask.MDC.m;

	if (close_crop > 0)
	{
		DT_UBYTE rgbt[4] = {gattribs->R1, gattribs->G1, gattribs->B1, 255};
		if ((bpp = pdxBitmapCrop(&bitmap, close_crop, /*clip->W, clip->H,*/ rgbt, &cx, &cy, &cw, &ch)) > 0)
		{
			bitmap_memory += (clip->W * cy + cx) * bpp; bitmap_padding = (clip->W - cw) * bpp;
			if (mask_memory) { mask_memory += (clip->W * cy + cx); mask_padding = (clip->W - cw); }
		}
	}

	ret = fxImageWrite(&info, bitmap_memory, mask_memory, bitmap_padding, mask_padding, bitmap.Format, bitmap.Subformat, cw, ch, orientation, gattribs, &user_stream);

	if (user_struct.Error || user_struct.Buffer == DV_NULL) ret = 0;

	if (ret == 1) /* there were no errors */
	{
		*img_memory = user_struct.Buffer;
		img_len = user_struct.CurrPos;
	}
	else /* an error occured */
	{
		if (user_struct.Buffer != DV_NULL) DF_PDX_MEM_FREE(user_struct.Buffer);
		*img_memory = DV_NULL;
		img_len = 0;
	}

	if (mask_memory) pdxBitmapFree(&mask);
	pdxBitmapFree(&bitmap);

	return img_len;
}


DT_SLONG pdxDocSaveAsCompressedImage(DT_UWORD compression_type, const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_UBYTE close_crop, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	DT_AUX_INFO info = {DEF_AUX_INIT_KEY, DEF_AUX_INIT_FLAG, 0, 0, compression_type, DV_NULL};
	DT_AUX_USER_STREAM user_stream = {DV_NULL, DV_NULL, DV_NULL, DV_NULL};
	DT_PDX_BITMAP bitmap, mask;
	DT_SLONG ret = 0;
	DT_SLONG cx = 0, cy = 0, cw = clip->W, ch = clip->H;
	DT_SLONG bitmap_padding = 0;
	DT_SLONG mask_padding = 0;
	DT_SLONG bpp = 1;
	DT_UBYTE* bitmap_memory = DV_NULL;
	DT_UBYTE* mask_memory = DV_NULL;
	DT_SWORD use_mask = 0;

	if (compression_type == DV_AUX_FILEFORMAT_PNG && gattribs->TransparencyFlag == 3) { use_mask = 1; params->R = params->G = params->B = params->A = 255; }

	if (pdxDocDraw(doc, lcd_mode, page, zoom, x + clip->X, y + clip->Y, clip->W, clip->H, format, subformat, params, &bitmap) != 1) return 0;

	bitmap_memory = bitmap.MDC.m;
	mask_memory = DV_NULL;

	if (use_mask && pdxDocDraw(doc, lcd_mode, page, zoom, x + clip->X, y + clip->Y, clip->W, clip->H, 8, 1, params, &mask) == 1) mask_memory = mask.MDC.m;

	if (close_crop > 0)
	{
		DT_UBYTE rgbt[4] = {gattribs->R1, gattribs->G1, gattribs->B1, 255};
		if ((bpp = pdxBitmapCrop(&bitmap, close_crop, /*clip->W, clip->H,*/ rgbt, &cx, &cy, &cw, &ch)) > 0)
		{
			bitmap_memory += (clip->W * cy + cx) * bpp; bitmap_padding = (clip->W - cw) * bpp;
			if (mask_memory) { mask_memory += (clip->W * cy + cx); mask_padding = (clip->W - cw); }
		}
	}

	user_stream.FilePtr = stdout; if (file_name != DV_NULL) user_stream.FilePtr = dtFileOpen(file_name, "wb"); if (user_stream.FilePtr == DV_NULL) goto end;

	ret = fxImageWrite(&info, bitmap_memory, mask_memory, bitmap_padding, mask_padding, bitmap.Format, bitmap.Subformat, cw, ch, orientation, gattribs, &user_stream);

	dtFileClose(user_stream.FilePtr);

end:

	if (mask_memory) pdxBitmapFree(&mask);
	pdxBitmapFree(&bitmap);

	return ret;
}


DT_SLONG pdxDocSaveAsPNG(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_UBYTE close_crop, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	return pdxDocSaveAsCompressedImage(DV_AUX_FILEFORMAT_PNG, file_name, doc, orientation, close_crop, lcd_mode, page, zoom, x, y, format, subformat, params, gattribs, clip);
}


DT_SLONG pdxDocSaveAsJPG(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_UBYTE close_crop, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	return pdxDocSaveAsCompressedImage(DV_AUX_FILEFORMAT_JPG, file_name, doc, orientation, close_crop, lcd_mode, page, zoom, x, y, format, subformat, params, gattribs, clip);
}


DT_SLONG pdxDocRenderAsPNG(DT_UBYTE** png_memory, DT_DOC doc, DT_UBYTE orientation, DT_UBYTE close_crop, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	return pdxDocRenderAsCompressedImage(DV_AUX_FILEFORMAT_PNG, png_memory, doc, orientation, close_crop, lcd_mode, page, zoom, x, y, format, subformat, params, gattribs, clip);
}


DT_SLONG pdxDocRenderAsJPG(DT_UBYTE** jpg_memory, DT_DOC doc, DT_UBYTE orientation, DT_UBYTE close_crop, DT_SWORD lcd_mode, DT_SLONG page, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	return pdxDocRenderAsCompressedImage(DV_AUX_FILEFORMAT_JPG, jpg_memory, doc, orientation, close_crop, lcd_mode, page, zoom, x, y, format, subformat, params, gattribs, clip);
}


DT_SLONG pdxDocSaveAsAnimatedImage(DT_UWORD compression_type, const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, const DT_SLONG pages[], const DT_SLONG delays[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	if (pages == DV_NULL || n < 1) return 0;

	DT_SLONG page = pages[0], page_counter = 0;
	DT_SLONG delay = ((delays == DV_NULL) ? gattribs->AnimDelay : delays[0]);

	//DT_UBYTE close_crop = 3;
	DT_UBYTE close_crop = 0;

	DT_AUX_INFO info = {DEF_AUX_INIT_KEY, DEF_AUX_INIT_FLAG, 0, 0, compression_type, DV_NULL};
	DT_AUX_USER_STREAM user_stream = {DV_NULL, DV_NULL, DV_NULL, DV_NULL};
	DT_PDX_BITMAP bitmap, mask;
	DT_SLONG ret = 0;
	DT_SLONG cx = 0, cy = 0, cw = clip->W, ch = clip->H;
	DT_SLONG bitmap_padding = 0;
	DT_SLONG mask_padding = 0;
	DT_SLONG bpp = 1;
	DT_UBYTE* bitmap_memory = DV_NULL;
	DT_UBYTE* mask_memory = DV_NULL;
	DT_SWORD use_mask = 0;

	if (compression_type == DV_AUX_FILEFORMAT_PNG && gattribs->TransparencyFlag == 3) { use_mask = 1; params->R = params->G = params->B = params->A = 255; }

	user_stream.FilePtr = stdout; if (file_name != DV_NULL) user_stream.FilePtr = dtFileOpen(file_name, "wb"); if (user_stream.FilePtr == DV_NULL) return 0;

	ret = fxAnimStart(&info, format, subformat, clip->W, clip->H, use_mask, n, gattribs, &user_stream);

	if (ret != 1) { dtFileClose(user_stream.FilePtr); return 0; }

loop:

	if (pdxDocDraw(doc, lcd_mode, page, zoom, x + clip->X, y + clip->Y, clip->W, clip->H, format, subformat, params, &bitmap) != 1) goto end;

	bitmap_memory = bitmap.MDC.m;
	mask_memory = DV_NULL;

	if (use_mask && pdxDocDraw(doc, lcd_mode, page, zoom, x + clip->X, y + clip->Y, clip->W, clip->H, 8, 1, params, &mask) == 1) mask_memory = mask.MDC.m;

	if (close_crop > 0 && page_counter > 0)
	{
		DT_UBYTE rgbt[4] = {gattribs->R1, gattribs->G1, gattribs->B1, 255};
		if ((bpp = pdxBitmapCrop(&bitmap, close_crop, /*clip->W, clip->H,*/ rgbt, &cx, &cy, &cw, &ch)) > 0)
		{
			bitmap_memory += (clip->W * cy + cx) * bpp; bitmap_padding = (clip->W - cw) * bpp;
			if (mask_memory) { mask_memory += (clip->W * cy + cx); mask_padding = (clip->W - cw); }
		}
	}

	ret = fxAnimWrite(&info, bitmap_memory, mask_memory, bitmap_padding, mask_padding, cx, clip->H - cy - ch, cw, ch, gattribs->Quality, orientation, delay);

	pdxBitmapFree(&bitmap);
	page_counter++; if (ret == 1 && page_counter < n) { page = pages[page_counter]; if (delays != DV_NULL) delay = delays[page_counter]; goto loop; }

end:

	fxAnimEnd(&info);

	dtFileClose(user_stream.FilePtr);
	return ret;
}


DT_SLONG pdxDocSaveAsAnimatedPNG(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, const DT_SLONG pages[], const DT_SLONG delays[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	return pdxDocSaveAsAnimatedImage(DV_AUX_FILEFORMAT_PNG, file_name, doc, orientation, lcd_mode, pages, delays, n, zoom, x, y, format, subformat, params, gattribs, clip);
}


DT_SLONG pdxDocSaveAsAnimatedGIF(const DT_CHAR* file_name, DT_DOC doc, DT_UBYTE orientation, DT_SWORD lcd_mode, const DT_SLONG pages[], const DT_SLONG delays[], DT_SLONG n, DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SWORD format, DT_SWORD subformat, DT_PD_DOCDRAW_PARAMS* params, const DT_AUX_GRAPHICS_ATTRIBS* gattribs, const DT_PDX_XYWH_SLONG* clip)
{
	return pdxDocSaveAsAnimatedImage(DV_AUX_FILEFORMAT_GIF, file_name, doc, orientation, lcd_mode, pages, delays, n, zoom, x, y, format, subformat, params, gattribs, clip);
}


#if (defined(DEF_PDX_EXPORT_PDF) || defined(DEF_PDX_EXPORT_SVG))
#include "pdextras/export.cpp"
#endif
