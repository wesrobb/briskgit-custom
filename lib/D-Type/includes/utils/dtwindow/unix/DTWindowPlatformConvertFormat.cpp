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


class CDTFormat
{
public:

	enum Enumeration
	{
		Unknown,  // unknown pixel format.
		XRGB8888, // 32 bit truecolor. this is the native pixel format in Mode::TrueColor.
		XBGR8888, // 32 bit truecolor in BGR order.
		RGB888,   // 24 bit truecolor.
		BGR888,   // 24 bit truecolor in BGR order.
		RGB565,   // 16 bit hicolor.
		BGR565,   // 16 bit hicolor in BGR order.
		XRGB1555, // 15 bit hicolor.
		XBGR1555  // 15 bit hicolor in BGR order.
	};

	CDTFormat() { Enumr = Unknown; }
	CDTFormat(Enumeration enumr) { Enumr = enumr; }

	operator Enumeration() const { return Enumr; }

private:

	Enumeration Enumr;
};



// integer to integer converters

inline void Convert_XRGB8888_to_XBGR8888(const DT_ULONG src[], DT_ULONG dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG r = (src[i] & 0x00FF0000) >> 16;
		DT_ULONG g = (src[i] & 0x0000FF00);
		DT_ULONG b = (src[i] & 0x000000FF) << 16;
		dst[i] = r | g | b;
	}
}


inline void Convert_XBGR8888_to_XRGB8888(const DT_ULONG src[], DT_ULONG dst[], DT_SLONG count)
{
	Convert_XRGB8888_to_XBGR8888(src, dst, count);
}


inline void Convert_XRGB8888_to_RGB888(const DT_ULONG src[], DT_UBYTE dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		dst[0] = (DT_UBYTE)((src[i] & 0x00FF0000) >> 16);
		dst[1] = (DT_UBYTE)((src[i] & 0x0000FF00) >> 8);
		dst[2] = (DT_UBYTE)((src[i] & 0x000000FF));
		dst += 3;
	}
}


inline void Convert_RGB888_to_XRGB8888(const DT_UBYTE src[], DT_ULONG dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG r = src[0];
		DT_ULONG g = src[1];
		DT_ULONG b = src[2];

		dst[i] = (r << 16) | (g << 8) | b;
		src += 3;
	}
}


inline void Convert_XRGB8888_to_BGR888(const DT_ULONG src[], DT_UBYTE dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		dst[0] = (DT_UBYTE)((src[i] & 0x000000FF));
		dst[1] = (DT_UBYTE)((src[i] & 0x0000FF00) >> 8);
		dst[2] = (DT_UBYTE)((src[i] & 0x00FF0000) >> 16);
		dst += 3;
	}
}


inline void Convert_BGR888_to_XRGB8888(const DT_UBYTE src[], DT_ULONG dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG r = src[0];
		DT_ULONG g = src[1];
		DT_ULONG b = src[2];

		dst[i] = (b << 16) | (g << 8) | r;
		src += 3;
	}
}


inline void Convert_XRGB8888_to_RGB565(const DT_ULONG src[], DT_UWORD dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG r = (src[i] & 0x00F80000) >> 8;
		DT_ULONG g = (src[i] & 0x0000FC00) >> 5;
		DT_ULONG b = (src[i] & 0x000000F8) >> 3;

		dst[i] = (DT_UWORD)(r | g | b);
	}
}


inline void Convert_RGB565_to_XRGB8888(const DT_UWORD src[], DT_ULONG dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG color = (DT_ULONG)src[i];

		DT_ULONG r = (color & 0x0000F800) << 8;
		DT_ULONG g = (color & 0x000007E0) << 5;
		DT_ULONG b = (color & 0x0000001F) << 3;

		dst[i] = r | g | b;
	}
}


inline void Convert_XRGB8888_to_BGR565(const DT_ULONG src[], DT_UWORD dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG r = (src[i] & 0x00F80000) >> 19;
		DT_ULONG g = (src[i] & 0x0000FC00) >> 5;
		DT_ULONG b = (src[i] & 0x000000F8) << 8;

		dst[i] = (DT_UWORD)(r | g | b);
	}
}


inline void Convert_BGR565_to_XRGB8888(const DT_UWORD src[], DT_ULONG dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG color = (DT_ULONG)src[i];

		DT_ULONG r = ((color & 0x0000F800) << 8) >> 16;
		DT_ULONG g = ((color & 0x000007E0) << 5);
		DT_ULONG b = ((color & 0x0000001F) << 3) << 16;

		dst[i] = r | g | b;
	}
}


inline void Convert_XRGB8888_to_XRGB1555(const DT_ULONG src[], DT_UWORD dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG r = (src[i] & 0x00F80000) >> 9;
		DT_ULONG g = (src[i] & 0x0000F800) >> 6;
		DT_ULONG b = (src[i] & 0x000000F8) >> 3;

		dst[i] = (DT_UWORD)(r | g | b);
	}
}


inline void Convert_XRGB1555_to_XRGB8888(const DT_UWORD src[], DT_ULONG dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG color = (DT_ULONG)src[i];

		DT_ULONG r = (color & 0x00007C00) << 9;
		DT_ULONG g = (color & 0x000003E0) << 6;
		DT_ULONG b = (color & 0x0000001F) << 3;

		dst[i] = r | g | b;
	}
}


inline void Convert_XRGB8888_to_XBGR1555(const DT_ULONG src[], DT_UWORD dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG r = (src[i] & 0x00F80000) >> 19;
		DT_ULONG g = (src[i] & 0x0000F800) >> 6;
		DT_ULONG b = (src[i] & 0x000000F8) << 7;

		dst[i] = (DT_UWORD)(r | g | b);
	}
}


inline void Convert_XBGR1555_to_XRGB8888(const DT_UWORD src[], DT_ULONG dst[], DT_SLONG count)
{
	for (DT_SLONG i = 0; i < count; ++i)
	{
		DT_ULONG color = (DT_ULONG)src[i];

		DT_ULONG r = (color & 0x00007C00) >> 7;
		DT_ULONG g = (color & 0x000003E0) << 6;
		DT_ULONG b = (color & 0x0000001F) << 19;

		dst[i] = r | g | b;
	}
}


// copy converters


inline void Convert_XRGB8888_to_XRGB8888(const DT_ULONG src[], DT_ULONG dst[], DT_SLONG count)
{
	dtMemCopy(dst, src, count * 4);
	//for (DT_SLONG i = 0; i < count; ++i) dst[i] = src[i];
}


// declare set of converter classes

class CDTColorConverter
{
public:

	virtual void convert(const void* src, void* dst, DT_SLONG count) = 0;
};

class CDTConverter_XRGB8888_to_XRGB8888 : public CDTColorConverter { void convert(const void* src, void* dst, DT_SLONG count) { Convert_XRGB8888_to_XRGB8888((const DT_ULONG*)src, (DT_ULONG*)dst, count); } };
class CDTConverter_XRGB8888_to_XBGR8888 : public CDTColorConverter { void convert(const void* src, void* dst, DT_SLONG count) { Convert_XRGB8888_to_XBGR8888((const DT_ULONG*)src, (DT_ULONG*)dst, count); } };
class CDTConverter_XRGB8888_to_RGB888   : public CDTColorConverter { void convert(const void* src, void* dst, DT_SLONG count) { Convert_XRGB8888_to_RGB888((const DT_ULONG*)src, (DT_UBYTE*)dst, count); } };
class CDTConverter_XRGB8888_to_BGR888   : public CDTColorConverter { void convert(const void* src, void* dst, DT_SLONG count) { Convert_XRGB8888_to_BGR888((const DT_ULONG*)src, (DT_UBYTE*)dst, count); } };
class CDTConverter_XRGB8888_to_RGB565   : public CDTColorConverter { void convert(const void* src, void* dst, DT_SLONG count) { Convert_XRGB8888_to_RGB565((const DT_ULONG*)src, (DT_UWORD*)dst, count); } };
class CDTConverter_XRGB8888_to_BGR565   : public CDTColorConverter { void convert(const void* src, void* dst, DT_SLONG count) { Convert_XRGB8888_to_BGR565((const DT_ULONG*)src, (DT_UWORD*)dst, count); } };
class CDTConverter_XRGB8888_to_XRGB1555 : public CDTColorConverter { void convert(const void* src, void* dst, DT_SLONG count) { Convert_XRGB8888_to_XRGB1555((const DT_ULONG*)src, (DT_UWORD*)dst, count); } };
class CDTConverter_XRGB8888_to_XBGR1555 : public CDTColorConverter { void convert(const void* src, void* dst, DT_SLONG count) { Convert_XRGB8888_to_XBGR1555((const DT_ULONG*)src, (DT_UWORD*)dst, count); } };


CDTConverter_XRGB8888_to_XRGB8888 Converter_XRGB8888_to_XRGB8888;
CDTConverter_XRGB8888_to_XBGR8888 Converter_XRGB8888_to_XBGR8888;
CDTConverter_XRGB8888_to_RGB888   Converter_XRGB8888_to_RGB888;
CDTConverter_XRGB8888_to_BGR888   Converter_XRGB8888_to_BGR888;
CDTConverter_XRGB8888_to_RGB565   Converter_XRGB8888_to_RGB565;
CDTConverter_XRGB8888_to_BGR565   Converter_XRGB8888_to_BGR565;
CDTConverter_XRGB8888_to_XRGB1555 Converter_XRGB8888_to_XRGB1555;
CDTConverter_XRGB8888_to_XBGR1555 Converter_XRGB8888_to_XBGR1555;


CDTColorConverter* RequestConverter(CDTFormat src, CDTFormat dst)
{
	if (src == CDTFormat::XRGB8888)
	{
		switch (dst)
		{
		case CDTFormat::XRGB8888: return &Converter_XRGB8888_to_XRGB8888;
		case CDTFormat::XBGR8888: return &Converter_XRGB8888_to_XBGR8888;
		case CDTFormat::RGB888:   return &Converter_XRGB8888_to_RGB888;
		case CDTFormat::BGR888:   return &Converter_XRGB8888_to_BGR888;
		case CDTFormat::RGB565:   return &Converter_XRGB8888_to_RGB565;
		case CDTFormat::BGR565:   return &Converter_XRGB8888_to_BGR565;
		case CDTFormat::XRGB1555: return &Converter_XRGB8888_to_XRGB1555;
		case CDTFormat::XBGR1555: return &Converter_XRGB8888_to_XBGR1555;
		default:             return DV_NULL;
		}
	}

	return DV_NULL;
}
