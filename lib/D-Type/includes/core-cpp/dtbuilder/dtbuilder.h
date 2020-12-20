#ifndef DTYPE_BUILDERCPP_H
#define DTYPE_BUILDERCPP_H


#define DEF_BUILDER_STRCOPY_FULL


const DT_SLONG DV_FONTW = 100; /*36*/
const DT_SLONG DV_FONTH = 100; /*36*/

const DT_SLONG DV_NESTFONTW = 100;
const DT_SLONG DV_NESTFONTH = 100;
const DT_SWORD DV_NESTFONTINDEX = 0;

const DT_SLONG DV_FORMATTER_MAX_COUNT = 20;

const DT_UWORD DV_BUILDER_FLAG_EXBASIC = 0;
const DT_UWORD DV_BUILDER_FLAG_NEWLINE = 1;
const DT_UWORD DV_BUILDER_FLAG_KEEPALL = 2;
const DT_UWORD DV_BUILDER_FLAG_EXRULER = 4;

#define DV_ES ""
#define DV_SEL_ALL -1
#define DV_SEL_LAST -2


class CDTBuilder; /* forward declaration for CDTMarker::SetPos */


class CDTMarker
{
public:

	CDTMarker(DT_SLONG pos = 0) : Pos(pos) {}
	virtual ~CDTMarker() {}

	virtual void SetPos(CDTBuilder& /*buider*/, DT_SLONG pos, DT_SLONG /*flow_index*/ = 0) { Pos = pos; }

public:

	DT_SLONG Pos;
};



#define DV_FORMATARR_MAX_SIZE 11

class CDTFormatArr
{
public:

	CDTFormatArr(DT_ID_SLONG id0, DT_ID_SLONG id1 = TX_ATTR_END, DT_ID_SLONG id2 = TX_ATTR_END, DT_ID_SLONG id3 = TX_ATTR_END, DT_ID_SLONG id4 = TX_ATTR_END, DT_ID_SLONG id5 = TX_ATTR_END, DT_ID_SLONG id6 = TX_ATTR_END, DT_ID_SLONG id7 = TX_ATTR_END, DT_ID_SLONG id8 = TX_ATTR_END, DT_ID_SLONG id9 = TX_ATTR_END)
	{
		Id[0] = id0; Id[1] = id1; Id[2] = id2; Id[3] = id3; Id[4] = id4; Id[5] = id5; Id[6] = id6; Id[7] = id7; Id[8] = id8; Id[9] = id9; Id[10] = TX_ATTR_END;
	}

	DT_ID_SLONG Id[DV_FORMATARR_MAX_SIZE];
};


class CDTFormatter : public CDTAtom
{
public:

	CDTFormatter() { Reset(); }
	virtual ~CDTFormatter() {}

	void Reset() { Count = 0; Attr[0].Id = TX_ATTR_END; Attr[0].Value[0] = 0; }

protected:

	virtual CDTFormatter& Format_Internal(const DT_TX_ATTRIBS text_attribs[], DT_SLONG first_char = -1, DT_SLONG last_char = -1);

public:

	const DT_TX_ATTRIBS* Get() const { return Attr; }

	CDTFormatter& Format(const DT_TX_ATTRIBS text_attribs[], const DT_CHAR* /*extra*/ = DV_NULL, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { if (text_attribs == DV_NULL) return *this; return Format_Internal(text_attribs, first_char.Pos, last_char.Pos); }
	CDTFormatter& Format(const CDTFormatter& formatter, const DT_CHAR* /*extra*/ = DV_NULL, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { return Format_Internal(formatter.Get(), first_char.Pos, last_char.Pos); }
	CDTFormatter& Format(const CDTFormatArr& formatarr, const DT_CHAR* value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1))
	{
		DT_SLONG i = 0, l = strlen(value); if (l >= 255) return *this;
		DT_TX_ATTRIBS text_attribs[DV_FORMATARR_MAX_SIZE];
		do { text_attribs[i].Id = formatarr.Id[i]; dtMemCopy(text_attribs[i].Value, value, l + 1); i++; } while (formatarr.Id[i - 1] != TX_ATTR_END);
		return Format_Internal(text_attribs, first_char.Pos, last_char.Pos);
	}
	//CDTFormatter& Format(const CDTFormatter& formatter, DT_SLONG /*value*/, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { return Format_Internal(formatter.Get(), first_char.Pos, last_char.Pos); }
	CDTFormatter& Format(const CDTFormatArr& formatarr, DT_SLONG value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1))
	{
		DT_STRING_CHARS(s, 64, ""); dtStrConcatSLong(&s, value);
		return Format(formatarr, s.str, first_char.Pos, last_char.Pos);
	}
	CDTFormatter& Format(DT_ID_SLONG id, const DT_CHAR* value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1))
	{
		DT_SLONG l = strlen(value); if (l >= 255) return *this;
		DT_TX_ATTRIBS text_attribs[] = {{id, {""}}, {TX_ATTR_END, {""}}}; dtMemCopy(text_attribs[0].Value, value, l + 1);
		return Format_Internal(text_attribs, first_char.Pos, last_char.Pos);
	}
	CDTFormatter& Format(DT_ID_SLONG id, DT_SLONG value, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1))
	{
		DT_STRING_CHARS(s, 64, ""); dtStrConcatSLong(&s, value);
		return Format(id, s.str, first_char.Pos, last_char.Pos);
	}
	CDTFormatter& Format_FontSize(DT_SLONG font_w, DT_SLONG font_h = 0, CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1))
	{
		if (font_h == 0) font_h = font_w;
		DT_TX_ATTRIBS text_attribs[] = {{TX_ATTR_FONT_WIDTH, {""}}, {TX_ATTR_FONT_HEIGHT, {""}}, {TX_ATTR_END, {""}}};
		DT_STRING_PTRWR(s0, 64, text_attribs[0].Value, 0); dtStrConcatSLong(&s0, font_w);
		DT_STRING_PTRWR(s1, 64, text_attribs[1].Value, 0); dtStrConcatSLong(&s1, font_h);
		return Format_Internal(text_attribs, first_char.Pos, last_char.Pos);
	}

	//inline CDTFormatter& F(const DT_TX_ATTRIBS text_attribs[], CDTMarker first_char = CDTMarker(-1), CDTMarker last_char = CDTMarker(-1)) { return Format_Internal(text_attribs, first_char.Pos, last_char.Pos); }

private:

	DT_SLONG Count;
	DT_TX_ATTRIBS Attr[DV_FORMATTER_MAX_COUNT];
};



class CDTBuilder : public CDTFormatter
{
public:

	class CDTAttribs : public CDTAtom
	{
	public:

		CDTAttribs() : APage(DV_PAGE_TEMP), AX(0), AY(0)
		{
			ATM[0][0] = 1.0; ATM[0][1] = 0.0; ATM[0][2] = 0.0;
			ATM[1][0] = 0.0; ATM[1][1] = 1.0; ATM[1][2] = 0.0;
			ATM[2][0] = 0.0; ATM[2][1] = 0.0; ATM[2][2] = 1.0;
		}
		virtual ~CDTAttribs() {}

		static CDTAttribs* New(CDTDocV8& doc) { CDTAttribs* n = new CDTAttribs(); doc.AddAtom(n); return n; }

		inline CDTAttribs& Page(DT_SLONG page) { APage = page; return *this; }
		inline CDTAttribs& X(DT_SLONG x) { AX = x; return *this; }
		inline CDTAttribs& Y(DT_SLONG y) { AY = y; return *this; }
		inline CDTAttribs& Pos(DT_SLONG x, DT_SLONG y) { AX = x; AY = y; return *this; }
		inline CDTAttribs& TM(DT_FLOAT m00, DT_FLOAT m01, DT_FLOAT m02, DT_FLOAT m10, DT_FLOAT m11, DT_FLOAT m12, DT_FLOAT m20, DT_FLOAT m21, DT_FLOAT m22)
		{
			ATM[0][0] = m00; ATM[0][1] = m01; ATM[0][2] = m02;
			ATM[1][0] = m10; ATM[1][1] = m11; ATM[1][2] = m12;
			ATM[2][0] = m20; ATM[2][1] = m21; ATM[2][2] = m22;
			return *this;
		}
		inline CDTAttribs& TM(DT_FLOAT m00, DT_FLOAT m01, DT_FLOAT m10, DT_FLOAT m11) { return TM(m00, m01, 0.0, m10, m11, 0.0, 0.0, 0.0, 1.0); }
		inline CDTAttribs& TM(DT_FLOAT m00, DT_FLOAT m11) { return TM(m00, 0.0, 0.0, m11); }
		inline CDTAttribs& TM(DT_FLOAT m00) { return TM(m00, m00); }

		DT_SLONG APage;
		DT_SLONG AX, AY;
		DT_TM3X3 ATM;
	};

public:

	CDTBuilder(CDTDocV8* doc) : Doc(doc), Img0(0), FlowIndex(0), LastCharCount(0), FormatFirstChar(-1), FormatLastChar(-1), Flags(DV_BUILDER_FLAG_EXBASIC) { Reset(); }
	CDTBuilder(CDTDocV8& doc) : Doc(&doc), Img0(0), FlowIndex(0), LastCharCount(0), FormatFirstChar(-1), FormatLastChar(-1), Flags(DV_BUILDER_FLAG_EXBASIC) { Reset(); }
	virtual ~CDTBuilder() {}

	CDTBuilder* New() const { CDTBuilder* n = new CDTBuilder(Doc); /*CDTAtom* a = Doc->NextAtom; Doc->NextAtom = n; n->NextAtom = a;*/ Doc->AddAtom(n); return n; }

	void Reset() { if (Doc == DV_NULL) return; Doc->Deselect(); Img0 = 1 + Doc->ImgGetLast(); }

	inline DT_SLONG GetFlowIndex() const { return FlowIndex; }
	inline void SetFlowIndex(DT_SLONG flow_index) { FlowIndex = flow_index; }

	virtual bool IsNil() const { return false; }
	virtual DT_STR_UTF8 IsPlainText() const { return DV_NULL; }
	//virtual DT_UWORD GetFlags() { return 0; }

	void VCopy(const CDTAttribs& a, const CDTBuilder& b) { Doc->ImgAdd(Doc->ImgGetObj(b.Img0), a.APage, a.AX, a.AY, 0, 0, 0, Doc->ImgGetFnDraw(b.Img0), a.ATM); }

	inline CDTBuilder::CDTAttribs operator () () { return CDTAttribs(); }

protected:

	inline const DT_STREAM_DESC* GetFontmap() const { return Doc->GetFontmap(); }

	DT_ID_SLONG ToObj(DT_SLONG img_min, DT_SLONG img_max, DT_SLONG x, DT_SLONG y, DT_SLONG w, DT_SLONG h, DT_SLONG xbs, DT_SLONG ybs, DT_SLONG font_w, DT_SLONG font_h, DT_SLONG aw, DT_SLONG ah) const;

protected:

	CDTDocV8* Doc;
	DT_SLONG Img0;
	DT_SLONG FlowIndex;
	DT_SLONG LastCharCount;
	DT_SLONG FormatFirstChar, FormatLastChar;
	DT_UWORD Flags;

#if (DEF_ENGINE_LEVEL == 3)
#else

private:

	bool AddRun_UTF8(DT_STR_UTF8 str_utf8, DT_SLONG* last_char_count);
	bool AddRun_Builder(const CDTBuilder& b, DT_SLONG img, DT_ID_SLONG parent_obj, DT_ID_SWORD command, DT_SWORD flags, DT_SLONG* last_char_count);

protected:

	DT_SLONG TxCmd_Internal(DT_SLONG img, DT_ID_SLONG parent_obj, DT_ID_SWORD command);

	bool AddRun_Internal(const CDTBuilder& b, DT_SLONG img, DT_ID_SLONG parent_obj, DT_ID_SWORD command, DT_SWORD flags = 0 /*, bool update_last_char = true*/)
	{
		return AddRun_Builder(b, img, parent_obj, command, flags, &LastCharCount);
	}

	static DT_SWORD GetTextFormatFlags(bool standard_mode)
	{
		if (standard_mode) return TX_IMPORT_FULL_COMPACT;

		DT_SWORD unicode_flags = TX_IMPORT_POWERDOC_REFORMAT;
		/*if (true TextFormat_Font)*/ unicode_flags |= TX_IMPORT_UNICODE_SCRIPT | 2048;
		/*if (true TextFormat_Shaping)*/ unicode_flags |= TX_IMPORT_UNICODE_SCRIPT | (1024 | 4096);
		/*if (true TextFormat_Orientation)*/ unicode_flags |= TX_IMPORT_UNICODE_SCRIPT | 512;
		/*if (true TextFormat_Direction)*/ unicode_flags |= TX_IMPORT_UNICODE_BIDI | 256;
		return unicode_flags;
	}

	static DT_ID_SWORD Val(DT_ID_SWORD v, DT_ID_SWORD v_max, DT_ID_SWORD v_add = 0)
	{
		if (v < 0) v = 0; else if (v > v_max) v = v_max;
		return (DT_ID_SWORD)(v + v_add);
	}

	CDTFormatter& Format_Internal(const DT_TX_ATTRIBS text_attribs[], DT_SLONG first_char = -1, DT_SLONG last_char = -1);

public:

	//bool AddRun(CDTBuilder& b);
	inline bool AddRun(const CDTBuilder& b, DT_SWORD flags = 0) { return AddRun_Internal(b, Img0, -1, TX_CMD_ENDTEXT, flags); }
	inline bool AddMarker(CDTMarker& m, DT_SWORD /*flags*/ = 0) { m.SetPos(*this, LastCharCount, FlowIndex); return true; }
	//inline CDTBuilder& operator + (CDTMarker& char_count_ref) { char_count_ref.SetPos(*this, LastCharCount, FlowIndex); /*char_count_ref.Pos = this->LastCharCount;*/ return *this; }

#endif
};


class CDTString : public CDTAtom
{
public:

	// https://docs.microsoft.com/en-us/cpp/cpp/move-constructors-and-move-assignment-operators-cpp
	// https://isocpp.org/wiki/faq/const-correctness#ref-to-const
	// CDTString const& str is equivalent to const CDTString& str, and CDTString const* str is equivalent to const CDTString* str

	CDTString(CDTDocV8* doc) : Doc(doc), MemStr(DV_NULL), MemLen(0) {}
	CDTString(CDTDocV8& doc) : Doc(&doc), MemStr(DV_NULL), MemLen(0) {}
	CDTString(DT_FLOAT num) : Doc(DV_NULL), MemStr(DV_NULL), MemLen(0) { Concat(num); }
	CDTString(DT_STR_UTF8 str_utf8 = DV_NULL) : Doc(DV_NULL), MemStr(DV_NULL), MemLen(0) { Concat(str_utf8); }
	CDTString(const CDTString& str) : Doc(DV_NULL), MemStr(DV_NULL), MemLen(0) { Concat(str.MemStr); } // copy constructor
#if (_MSC_VER >= 1910)
	CDTString(CDTString&& str) : Doc(DV_NULL) { MemStr = str.MemStr; MemLen = str.MemLen; str.MemStr = DV_NULL; str.MemLen = 0; } // move constructor
#endif
	virtual ~CDTString() { Clear(); }

	CDTString& operator = (const CDTString& str) { if (this == &str) return *this; Clear(); Concat(str.MemStr); return *this; } // copy assignment operator
#if (_MSC_VER >= 1910)
	CDTString& operator = (CDTString&& str) { if (this == &str) return *this; Clear(); MemStr = str.MemStr; MemLen = str.MemLen; str.MemStr = DV_NULL; str.MemLen = 0; return *this; } // move assignment operator
#endif

public:

	inline DT_STR_UTF8 GetStr() const { return MemStr; }
	inline DT_SLONG GetLen() const { return MemLen; }

	bool Concat(DT_STR_UTF8 str, DT_SLONG len = -1)
	{
		if (str == DV_NULL || str[0] == 0) return true; else if (len < 0) len = strlen(str);
		DT_SLONG str_len = 1 + len;
		DT_SLONG mem_len = MemLen + str_len;
		//DT_CHAR* mem_str = static_cast<DT_CHAR*>(dtMemRealloc(MemStr, mem_len)); if (mem_str == DV_NULL) return false; /* can't use dtMemRealloc as somebody might want to append a string to itself */
		DT_CHAR* mem_str = static_cast<DT_CHAR*>(dtMemAlloc(mem_len)); if (mem_str == DV_NULL) return false;
		dtMemCopy(mem_str, MemStr, MemLen);
		dtMemCopy(mem_str + MemLen, str, str_len);
		dtMemFree(MemStr);
		MemStr = mem_str;
		MemLen = mem_len - 1;
		return true;
	}

	bool Concat(DT_FLOAT num)
	{
		DT_CHAR buff[64];
		DT_STRING_PTRWR(str_utf8, 64, buff, 0); dtStrConcatFloat(&str_utf8, num, 10);
		return Concat(str_utf8.str);
	}

	inline bool Concat(const CDTString& str) { return Concat(str.GetStr(), str.GetLen()); }

	inline void Clear() { if (MemStr != DV_NULL) dtMemFree(MemStr); MemStr = DV_NULL; MemLen = 0; }

	CDTString& Str() { CDTString* n = new CDTString(); if (Doc != DV_NULL) Doc->AddAtom(n); return *n; }

	inline CDTString& operator () () { return Str(); }

private:

	CDTDocV8* Doc;
	DT_CHAR* MemStr;
	DT_SLONG MemLen;
};


inline CDTString& operator ~ (CDTString& str) { str.Clear(); return str; }
inline CDTString& operator + (CDTString& str, DT_FLOAT num) { str.Concat(num); return str; }
inline CDTString& operator + (CDTString& str, DT_STR_UTF8 str_utf8) { str.Concat(str_utf8); return str; }
inline CDTString& operator + (CDTString& str, const CDTString& str2) { str.Concat(str2); return str; }


class CDTBuilderUTF8 : public CDTBuilder
{
public:

	CDTBuilderUTF8(const CDTString& str) : CDTBuilder(DV_NULL), StrUTF8(str.GetStr()) { *Buff = 0; }
	CDTBuilderUTF8(const CDTString* str) : CDTBuilder(DV_NULL), StrUTF8(str->GetStr()) { *Buff = 0; }
	CDTBuilderUTF8(DT_STR_UTF8 str_utf8) : CDTBuilder(DV_NULL), StrUTF8(str_utf8) { *Buff = 0; }
	CDTBuilderUTF8(DT_FLOAT num) : CDTBuilder(DV_NULL) { *Buff = 0; DT_STRING_PTRWR(str_utf8, 64, Buff, 0); dtStrConcatFloat(&str_utf8, num, 10); StrUTF8 = str_utf8.str; }
	CDTBuilderUTF8(bool /*dummy*/, CDTDocV8* doc) : CDTBuilder(doc), StrUTF8(DV_NULL) { *Buff = 0; }
	CDTBuilderUTF8(bool /*dummy*/, CDTDocV8& doc) : CDTBuilder(doc), StrUTF8(DV_NULL) { *Buff = 0; }
	virtual ~CDTBuilderUTF8() {}

	bool IsNil() const { return (StrUTF8 != DV_NULL && StrUTF8[0] == 8 /* Nil */ && StrUTF8[1] == 0); }
	DT_STR_UTF8 IsPlainText() const { return StrUTF8; }

protected:

	DT_CHAR Buff[64];

private:

	DT_STR_UTF8 StrUTF8;
};


typedef CDTBuilderUTF8 CDTBuilderHead;
typedef const CDTBuilderHead& DT_BUILDREF;
//typedef const CDTBuilder& DT_BUILDREF;
typedef CDTBuilder CDTBuilderBase;
//typedef CDTBuilder::CDTAttribs CDTAttribsBase;


#if (DEF_ENGINE_LEVEL == 3)
#else

//.inline CDTBuilderUTF8& operator + (CDTBuilderUTF8& run, DT_STR_UTF8 s) { CDTBuilderUTF8 b(s); run.AddRun(b); return run; }
inline CDTBuilderUTF8& operator + (CDTBuilderUTF8& run, DT_BUILDREF b) { run.AddRun(b); return run; }
inline CDTBuilderUTF8& operator + (CDTBuilderUTF8& run, CDTMarker& m) { run.AddMarker(m); return run; }

//inline CDTBuilder& operator + (CDTBuilder& run, DT_STR_UTF8 s) { run.AddRun(s); return run; }
//inline CDTBuilder& operator - (CDTBuilder& run, DT_BUILDREF b) { run.AddObj(b); return run; }

// https://stackoverflow.com/questions/5532991/what-is-the-difference-between-overloading-an-operator-inside-or-outside-a-class
//struct Bar
//{
//	Bar() {}
//	Bar(int) {}
//};
//
//Bar operator + (Bar const & L, Bar const & R) { return L; }

//inline CDTBuilderUTF8 operator + (CDTBuilderUTF8 run, CDTBuilderUTF8 b) { run.AddRun(b); return run; }


//CDTBuilderUTF8 operator + (CDTBuilderUTF8 const & L, Bar const & R) { return CDTBuilderUTF8(); }


class CDTAttribsBase : public CDTAtom
{
public:

	CDTAttribsBase() : APage(DV_PAGE_TEMP), AX(0), AY(0), AW(100), AH(100), AFontW(DV_FONTW), AFontH(DV_FONTH) {}
	virtual ~CDTAttribsBase() {}

	DT_SLONG APage;
	DT_SLONG AX, AY;
	DT_SLONG AW, AH;
	DT_SLONG AFontW, AFontH;

protected:

#ifdef DEF_BUILDER_STRCOPY_FULL

	typedef DT_CHAR* DT_BUILDER_STR;

	static DT_BUILDER_STR StrDup(const DT_CHAR* s)
	{
		if (s == DV_NULL) return DV_NULL;
		DT_SLONG l = 1 + strlen(s);
		DT_BUILDER_STR p = static_cast<DT_BUILDER_STR>(dtMemAlloc(l)); if (p != DV_NULL) dtMemCopy(p, s, l);
		return p;
	}

	inline static void StrCopy(DT_BUILDER_STR& dst, const DT_CHAR* src) { if (dst) dtMemFree(dst); dst = StrDup(src); }
	inline static void StrFree(DT_BUILDER_STR s) { if (s) dtMemFree(s); }

#else

	typedef const DT_CHAR* DT_BUILDER_STR;

	inline static void StrCopy(DT_BUILDER_STR& dst, const DT_CHAR* src) { dst = src; }
	inline static void StrFree(DT_BUILDER_STR /*s*/) {}

#endif
};

#define DEF_ATTRIBS_BASE_METHODS \
static CDTAttribs* New(CDTDocV8& doc) { CDTAttribs* n = new CDTAttribs(); doc.AddAtom(n); return n; } \
inline CDTAttribs& Page(DT_SLONG page) { APage = page; return *this; } \
inline CDTAttribs& X(DT_SLONG x) { AX = x; return *this; } \
inline CDTAttribs& Y(DT_SLONG y) { AY = y; return *this; } \
inline CDTAttribs& Pos(DT_SLONG x, DT_SLONG y) { AX = x; AY = y; return *this; } \
inline CDTAttribs& W(DT_SLONG w) { AW = w; return *this; } \
inline CDTAttribs& WScale(DT_SLONG w) { AW = -16 * w; return *this; } \
inline CDTAttribs& H(DT_SLONG h) { AH = h; return *this; } \
inline CDTAttribs& HScale(DT_SLONG h) { AH = -16 * h; return *this; } \
inline CDTAttribs& Size(DT_SLONG w, DT_SLONG h = 0) { if (h == 0) h = w; AW = w; AH = h; return *this; } \
inline CDTAttribs& SizeScale(DT_SLONG w, DT_SLONG h = 0) { if (h == 0) h = w; AW = -16 * w; AH = -16 * h; return *this; } \
inline CDTAttribs& FontW(DT_SLONG font_w) { AFontW = font_w; return *this; } \
inline CDTAttribs& FontH(DT_SLONG font_h) { AFontH = font_h; return *this; } \
inline CDTAttribs& FontSize(DT_SLONG font_w, DT_SLONG font_h = 0) { if (font_h == 0) font_h = font_w; AFontW = font_w; AFontH = font_h; return *this; }


#include "dtbuilder_autoformat.h"
#include "dtbuilder_rast.h"
#include "dtbuilder_text.h"
#include "dtbuilder_ctrl.h"
#include "dtbuilder_math.h"

#endif


#endif /* DTYPE_BUILDERCPP_H */
