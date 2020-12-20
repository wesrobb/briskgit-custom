/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#ifndef DTYPE_EXAMPLE_TEXTLINE_H
#define DTYPE_EXAMPLE_TEXTLINE_H


#define DV_MAX_GLYPHS_PER_LINE DV_MAXLINELENGTH
#define DV_MAX_RUNS_PER_LINE DV_MAXLINELENGTH


class CDTTextLine
{
private: // private copy constructor and assignment operator

	CDTTextLine(const CDTTextLine&);
	CDTTextLine& operator = (const CDTTextLine&);

public: // Constructor and Destructor

	CDTTextLine(const CDTFontMap* font_map = DV_NULL, const DT_STREAM_DESC* text_sd = DV_NULL, DT_ID_UBYTE text_type = textAuto) : FontMap(font_map), TypoFlags(3) /* kerning and ligatures */, UnicodeText_Buffer(DV_NULL), UnicodeText_CharCount(0), UnicodeText_BytesPerChar(0), TextRunCount(0)
	{
		if (text_sd != DV_NULL) LoadUnicodeTextFromFile(text_sd, text_type);

		DT_STYLE_EFFECTS style = {1, {0}, {0, 0, 0, 0}, DV_NULL};
		Style_Text = Style_Label = style;
	}

	virtual ~CDTTextLine()
	{
		UnloadUnicodeText();
	}

public: // Public methods

	inline void SetFontMap(const CDTFontMap* font_map) { FontMap = font_map; }
	inline void SetFontMap(const CDTFontMap& font_map) { FontMap = &font_map; }
	inline const CDTFontMap* GetFontMap() const { return FontMap; }
	//inline const CDTFontMap& GetFontMap() const { return *FontMap; }

	inline void SetTypoFlags(DT_ID_SLONG typo_flags) { TypoFlags = typo_flags; }
	inline DT_ID_SLONG GetTypoFlags() const { return TypoFlags; }

	DT_SWORD LoadUnicodeTextFromFile(const DT_STREAM_DESC* text_sd, DT_ID_UBYTE text_type);
	DT_SWORD LoadUnicodeTextFromFile(const DT_STREAM_DESC& text_sd, DT_ID_UBYTE text_type) { return LoadUnicodeTextFromFile(&text_sd, text_type); }
	void UnloadUnicodeText();

	DT_SWORD ProcessRuns();
	DT_SWORD ReorderRuns();

	void SetStyle_Text(const DT_STYLE_EFFECTS& style) { Style_Text = style; }
	void SetStyle_Label(const DT_STYLE_EFFECTS& style) { Style_Label = style; }

	void DrawRuns_Hor(DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SLONG dx, DT_SLONG dy, DT_SLONG font_size_w, DT_SLONG font_size_h, DT_ID_SWORD display_mode, DT_SWORD debug_flags, const DT_CHAR* debug_label);
	void DrawRuns_Ver(DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SLONG dx, DT_SLONG dy, DT_SLONG font_size_w, DT_SLONG font_size_h, DT_ID_SWORD display_mode, DT_SWORD debug_flags, const DT_CHAR* debug_label);

private:

	/* The Text Run Structure */
	struct TEXT_LAYOUT_RUN
	{
		DT_SLONG CharPosFrom;
		DT_SLONG CharPosTo;
		DT_ID_SWORD Script;
		DT_ID_SWORD Language;
		DT_ID_SWORD FontIndex;
		DT_UBYTE Level;
		DT_UBYTE ReorderingLevel;
	};

	/* The Unicode BiDi Structure */
	struct TEXT_LAYOUT_BIDI
	{
		DT_UBYTE* Types;
		DT_UBYTE* Levels;
	};

	const CDTFontMap* FontMap; // Pointer to FontMap

	DT_ID_SLONG TypoFlags;

	DT_UBYTE* UnicodeText_Buffer;
	DT_SLONG UnicodeText_CharCount;
	DT_ID_SWORD UnicodeText_BytesPerChar;

	TEXT_LAYOUT_RUN TextRunArr[DV_MAX_RUNS_PER_LINE];
	DT_SLONG TextRunCount;

	DT_STYLE_EFFECTS Style_Text;
	DT_STYLE_EFFECTS Style_Label;

private: // Private methods

	static const DT_CHAR* GetUnicodeScriptString(DT_ID_SWORD script_code);

	DT_SWORD BiDiAlloc(TEXT_LAYOUT_BIDI& bidi, const DT_UBYTE* text_buffer, DT_SLONG nr_of_chars, DT_ID_SWORD bytes_per_char, DT_SWORD unicode_flags);
	void BiDiFree(TEXT_LAYOUT_BIDI& bidi);

	DT_SLONG ReverseRuns(DT_SLONG first, DT_SLONG last, DT_SWORD decrease_and_count);

	DT_SLONG ShapeSingleRun_Hor(DT_SLONG offset, DT_SLONG count, DT_UBYTE level, DT_ID_SWORD script_code, DT_ID_SWORD language, DT_ID_SWORD font_index, LX_GLYPH glyph_arr[], DT_SLONG max_glyphs);
	DT_SLONG ShapeSingleRun_Ver(DT_SLONG offset, DT_SLONG count, DT_UBYTE level, DT_ID_SWORD script_code, DT_ID_SWORD language, DT_ID_SWORD font_index, LX_GLYPH glyph_arr[], DT_SLONG max_glyphs);

	DT_FLOAT DrawSingleRun_Hor(DT_FLOAT zoom, DT_SLONG tx, DT_SLONG ty, DT_ID_SWORD display_mode, DT_SWORD flags, DT_ID_SWORD font_index, DT_SLONG font_size_w, DT_SLONG font_size_h, DT_SWORD angle, const LX_GLYPH glyph_arr[], DT_SLONG glyph_arr_len, DT_FLOAT start_width);
	DT_FLOAT DrawSingleRun_Ver(DT_FLOAT zoom, DT_SLONG tx, DT_SLONG ty, DT_ID_SWORD display_mode, DT_SWORD flags, DT_ID_SWORD font_index, DT_SLONG font_size_w, DT_SLONG font_size_h, DT_SWORD angle, const LX_GLYPH glyph_arr[], DT_SLONG glyph_arr_len, DT_FLOAT start_width);

	inline static DT_SLONG ScaleA(DT_SLONG d, DT_FLOAT zoom)
	{
		zoom = DF_ROUND(zoom * 1000) * 0.001;
		return DF_ROUND(d * zoom);
	}

	inline static DT_SLONG ScaleB(DT_SLONG d, DT_FLOAT zoom)
	{
		return DF_ROUND(d * zoom);
	}
};


#endif /* DTYPE_EXAMPLE_TEXTLINE_H */
