/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <iostream>

// Main D-Type header file
#include "dtype.h"

// D-Type CPP Engine header
#define DEF_ENGINE_LEVEL 2
#include "dtengine.h"

// FontMap header file
#include "fontmap.h"

// Header for this file
#include "textline.h"


DT_SWORD CDTTextLine::LoadUnicodeTextFromFile(const DT_STREAM_DESC* text_sd, DT_ID_UBYTE text_type)
{
	if (text_sd == DV_NULL) return 0;

	DT_SLONG buffer_len = 0;
	UnicodeText_Buffer = lxTextAllocViaStream(text_sd, text_type, 0, &buffer_len, &UnicodeText_BytesPerChar);

	if (buffer_len < 0 || UnicodeText_BytesPerChar < 1 || UnicodeText_BytesPerChar > 4) return 0; // this never happens (if it did it would be an error)
	if (buffer_len > 0 && UnicodeText_Buffer == DV_NULL) return 0; // this never happens (if it did it would be an error)

	UnicodeText_CharCount = buffer_len / UnicodeText_BytesPerChar;

	std::cout << "Unicode text successfully loaded.\n";
	std::cout << "UnicodeText_CharCount=" << UnicodeText_CharCount << ", UnicodeText_BytesPerChar=" << UnicodeText_BytesPerChar << "\n";

	return 1;
}


void CDTTextLine::UnloadUnicodeText()
{
	if (UnicodeText_Buffer != DV_NULL)
	{
		lxTextFree(UnicodeText_Buffer);
		std::cout << "Unicode text unloaded.\n";
	}

	UnicodeText_Buffer = DV_NULL;
	UnicodeText_CharCount = 0;
	UnicodeText_BytesPerChar = 0;

	TextRunCount = 0; // once the text is unloaded any associated text runs must be cleared
}


const DT_CHAR* CDTTextLine::GetUnicodeScriptString(DT_ID_SWORD script_code)
{
	DT_SLONG i = 0;

	while (ConfigScriptMap[i].script != DV_NULL)
	{
		if (ConfigScriptMap[i].code == script_code) return ConfigScriptMap[i].script;
		i++;
	}

	return ConfigScriptMap[0].script; // if script_code is not found return the first script which is "zyyy" (Common)
}


DT_SWORD CDTTextLine::BiDiAlloc(TEXT_LAYOUT_BIDI& bidi, const DT_UBYTE* text_buffer, DT_SLONG nr_of_chars, DT_ID_SWORD bytes_per_char, DT_SWORD unicode_flags)
{
	DT_SLONG l = dtSecAdd(nr_of_chars, 1);
	DT_UBYTE* memory;

	bidi.Types = DV_NULL;
	bidi.Levels = DV_NULL;

	if (nr_of_chars < 0) return 0;
	if ((memory = static_cast<DT_UBYTE*>(dtMemAlloc(dtSecMul(l, sizeof(DT_UBYTE) + sizeof(DT_UBYTE))))) == DV_NULL) return 0;

	bidi.Types = memory; memory += l * sizeof(DT_UBYTE);
	bidi.Levels = memory;

	lxBidiApplyPlus(nr_of_chars, text_buffer, bytes_per_char, bidi.Types, bidi.Levels, 255, unicode_flags);
	//lxBidiApplyPlus(nr_of_chars, text_buffer, bytes_per_char, bidi.Types, bidi.Levels, 0, unicode_flags);

	return 1;
}


void CDTTextLine::BiDiFree(TEXT_LAYOUT_BIDI& bidi)
{
	if (bidi.Types != DV_NULL) dtMemFree(bidi.Types);

	bidi.Types = bidi.Levels = DV_NULL;
}


DT_SWORD CDTTextLine::ProcessRuns()
{
	TextRunCount = 0;

	if (FontMap == DV_NULL) return 0;

	TEXT_LAYOUT_BIDI bidi;
	DT_UWORD unicode_flags = 3; // apply BiDi and Unicode Script analysis
	DT_SLONG buffer_start = 0;
	DT_SLONG len = 0;
	DT_SLONG letter_count = 0, prev_letter_count = 0;
	DT_ULONG /*prevent_break = 0,*/ flag = 0;
	DT_UBYTE current_level = 0, next_level = 0;
	DT_UBYTE c_is_newline = 0, c_prev_is_newline = 1;
	DT_ID_SWORD current_font_index = 0;
	DT_ID_SWORD current_script = 0, next_script = 0, this_script = 0;
	DT_ID_ULONG c_newline = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE);
	DT_ID_ULONG c_newline2 = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWLINE2);
	DT_ID_ULONG c_newarea = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_NEWAREA);
	DT_ID_ULONG c_vertab = lxCharGetAttrib(0, LX_ATTRIB_CTRLCHAR_VERTAB);
	DT_ID_ULONG c = 0;
	//DT_ID_ULONG c_prev = 0;

	if (UnicodeText_Buffer == DV_NULL || UnicodeText_CharCount < 1) return 1; // nothing to process (empty text file)

	if (BiDiAlloc(bidi, UnicodeText_Buffer, UnicodeText_CharCount, UnicodeText_BytesPerChar, unicode_flags) != 1) return 0;

	/* Using the returned BiDi data, split text into runs of the same directionality and script (more or less) */

loop:

	len = 0;

	/* Respect BiDi levels and Unicode scripts */
	while (letter_count < UnicodeText_CharCount)
	{
		//prevent_break = 0;

			 if (UnicodeText_BytesPerChar == 4) c = DF_READ_ULONG_LE(UnicodeText_Buffer + len);
		else if (UnicodeText_BytesPerChar == 3) c = DF_READ_UTRIO_LE(UnicodeText_Buffer + len);
		else if (UnicodeText_BytesPerChar == 2) c = DF_READ_UWORD_LE(UnicodeText_Buffer + len);
		else if (UnicodeText_BytesPerChar == 1) c = DF_READ_UBYTE_LE(UnicodeText_Buffer + len);
		else c = 0; // this never happens (if it did it would be an error)

		next_level = bidi.Levels[letter_count];
		this_script = bidi.Types[letter_count];

		if (this_script < 0 || this_script >= scriptCodeCount) this_script = 0; // invalid or unsupported script detected

		c_is_newline = (c == c_newline || c == c_newline2 || c == c_newarea || c == c_vertab);

		if (c_prev_is_newline && (!c_is_newline))
		{
			/* We have a first letter in a paragraph */
			next_script = this_script;
		}
		else if (this_script == qaaiScriptCode) // Inherited Script
		{
			/* Don't change the script but inherit from the preceding character */
		}
		else if (this_script == zyyyScriptCode) // Common Script
		{
			/* Don't change the script but inherit from the preceding character. */
			/* However, exceptions are paired punctuations and other characters with a mirror. */

			flag = lxCharGetAttrib(c, LX_ATTRIB_MIRROR_AND_BREAK_FLAG);

				 if (flag == 2 /* mirror and don't allow break */) { next_script = this_script; /*prevent_break = 1;*/ /* change script and prevent break */ }
			else if (flag == 1 /* mirror and allow break */) { next_script = this_script; /* change script */ }
			//else if (flag == 0) { /* not a mirror, so no change of script */ }

			/* oversimplified and crude implementation */
			/*
			if (c == '(' || c == ')') next_script = this_script;
			if (c == '[' || c == ']') next_script = this_script;
			if (c == '{' || c == '}') next_script = this_script;
			if (c == '<' || c == '>') next_script = this_script;
			*/
		}
		else next_script = this_script;

		//c_prev = c;
		c_prev_is_newline = c_is_newline;

		/* Initialize current_level and current_script when letter_count == 0 (this will ensure that the first text fragment is never empty) */
		if (letter_count == 0) { current_level = next_level; current_script = next_script; }

		if (next_level != current_level || next_script != current_script) break; // exit when script or level changes

		/* Advance to the next letter */
		letter_count++;
		len += UnicodeText_BytesPerChar;
	}

	current_font_index = FontMap->GetFontForUnicodeScript(current_script);

	std::cout << "Text run " << TextRunCount << " [" << buffer_start << ".." << (buffer_start + len - 1) << "] ends at character pos " << letter_count << " and has direction " << current_level << " (" << ((current_level % 2) ? "OPPOSITE" : "NORMAL") << "), script " << current_script << " (" << GetUnicodeScriptString(current_script) << "), font index " << current_font_index << "\n";

	if (TextRunCount >= DV_MAX_RUNS_PER_LINE) goto done; // too many runs

	TextRunArr[TextRunCount].CharPosFrom = prev_letter_count;
	TextRunArr[TextRunCount].CharPosTo = letter_count;
	TextRunArr[TextRunCount].Script = current_script;
	TextRunArr[TextRunCount].Language = 0;
	TextRunArr[TextRunCount].FontIndex = current_font_index;
	TextRunArr[TextRunCount].Level = TextRunArr[TextRunCount].ReorderingLevel = current_level;
	prev_letter_count = letter_count;

	TextRunCount++;

	buffer_start += len;
	current_level = next_level;
	current_script = next_script;

	if (letter_count < UnicodeText_CharCount) goto loop;

done:

	BiDiFree(bidi);

	std::cout << "ProcessRuns done. Unicode text split into " << TextRunCount << " runs.\n";

	return 1;
}


DT_SLONG CDTTextLine::ReverseRuns(DT_SLONG first, DT_SLONG last, DT_SWORD decrease_and_count)
{
	DT_SLONG nr_of_nonzero_dir_numbers = 0;

	while (first < last)
	{
		TEXT_LAYOUT_RUN tmp = TextRunArr[first]; TextRunArr[first] = TextRunArr[last]; TextRunArr[last] = tmp; // swap the two runs

		if (decrease_and_count)
		{
			TextRunArr[first].ReorderingLevel--; if (TextRunArr[first].ReorderingLevel > 0) nr_of_nonzero_dir_numbers++;
			TextRunArr[last].ReorderingLevel--; if (TextRunArr[last].ReorderingLevel > 0) nr_of_nonzero_dir_numbers++;
		}

		first++; last--;
	}

	if (first == last && decrease_and_count)
	{
		TextRunArr[first].ReorderingLevel--; if (TextRunArr[first].ReorderingLevel > 0) nr_of_nonzero_dir_numbers++;
	}

	return nr_of_nonzero_dir_numbers;
}


DT_SWORD CDTTextLine::ReorderRuns()
{
	if (TextRunCount < 1) return 0;

	DT_SLONG i, i0, nr_of_nonzero_dir_numbers;

loop:

	nr_of_nonzero_dir_numbers = 0;
	i0 = -1;

	for (i = 0; i < TextRunCount; i++)
	{
		if (TextRunArr[i].ReorderingLevel == 0)
		{
			if (i0 > -1)
			{
				nr_of_nonzero_dir_numbers += ReverseRuns(i0, i - 1, 1);
				i0 = -1;
			}
		}
		else if (i0 == -1) i0 = i;
	}

	if (i0 > -1) nr_of_nonzero_dir_numbers += ReverseRuns(i0, i - 1, 1);

	if (nr_of_nonzero_dir_numbers > 0) goto loop;

	return 1;
}


DT_SLONG CDTTextLine::ShapeSingleRun_Hor(DT_SLONG offset, DT_SLONG count, DT_UBYTE level, DT_ID_SWORD script_code, DT_ID_SWORD language, DT_ID_SWORD font_index, LX_GLYPH glyph_arr[], DT_SLONG max_glyphs)
{
	if (glyph_arr == DV_NULL) return -1;

	DT_LXLAYOUT layout = DV_NULL;

	if (lxCacheObtainLayoutPlus(FontMap->GetEngine().GetLayoutCache(), font_index, script_code, language, TypoFlags, &layout) != 1) return -1;

	DT_SLONG tmp = level;
	DT_ID_UBYTE direction = LX_DIRECTION_LTR; if (tmp % 2) direction = LX_DIRECTION_RTL;

	return lxLayoutApplyPlus(layout, offset, count, UnicodeText_CharCount, UnicodeText_Buffer, UnicodeText_BytesPerChar, direction, glyph_arr, max_glyphs);
}


DT_SLONG CDTTextLine::ShapeSingleRun_Ver(DT_SLONG offset, DT_SLONG count, DT_UBYTE level, DT_ID_SWORD script_code, DT_ID_SWORD language, DT_ID_SWORD font_index, LX_GLYPH glyph_arr[], DT_SLONG max_glyphs)
{
	if (glyph_arr == DV_NULL) return -1;

	DT_LXLAYOUT layout = DV_NULL;

	if (lxCacheObtainLayoutPlus(FontMap->GetEngine().GetLayoutCache(), font_index, script_code, language, TypoFlags, &layout) != 1) return -1;

	DT_SLONG tmp = level;
	DT_ID_UBYTE direction = LX_DIRECTION_TTB; if (tmp % 2) direction = LX_DIRECTION_BTT;

	return lxLayoutApplyPlus(layout, offset, count, UnicodeText_CharCount, UnicodeText_Buffer, UnicodeText_BytesPerChar, direction, glyph_arr, max_glyphs);
}


DT_FLOAT CDTTextLine::DrawSingleRun_Hor(DT_FLOAT zoom, DT_SLONG tx, DT_SLONG ty, DT_ID_SWORD display_mode, DT_SWORD debug_flags, DT_ID_SWORD font_index, DT_SLONG font_size_w, DT_SLONG font_size_h, DT_SWORD angle, const LX_GLYPH glyph_arr[], DT_SLONG glyph_arr_len, DT_FLOAT start_width)
{
	if (glyph_arr == DV_NULL || glyph_arr_len < 1) return 0.0;
	if (display_mode == 0) angle = 0; // the simple glyph-by-glyph drawing approach does not support rotation

	DT_SLONG i;
	DT_UWORD font_em_h, font_em_v;
	DT_RECT_SWORD fontbox;
	CDTEngineV8& engine = FontMap->GetEngine();

	engine.FontGetMetrics(font_index, &font_em_h, &font_em_v, &fontbox);

	// Compute font scaling factors
	DT_FLOAT kh = ScaleA(font_size_w, zoom); kh /= font_em_h;
	DT_FLOAT kv = ScaleA(font_size_h, zoom); kv /= font_em_v;

	DT_TYPE_EFFECTS_L type = {font_index, 0, 0, {{ScaleA(font_size_w, zoom), ScaleA(font_size_h, zoom), 0, 0, angle}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	engine.OutputSetStyleEffects(Style_Text);
	engine.TypesetterSetTypeEffects(type);

	DT_SWORD origin_at_zero = 1;
	DT_SLONG glyph_arr_x = origin_at_zero ? 0 : glyph_arr[0].x;
	DT_SLONG glyph_arr_ybase[DV_MAX_GLYPHS_PER_LINE + 1];
	DT_SLONG glyph_arr_width[DV_MAX_GLYPHS_PER_LINE + 1];
	DT_ID_ULONG glyph_arr_index[DV_MAX_GLYPHS_PER_LINE + 1];

	glyph_arr_width[0] = glyph_arr_x;

	for (i = 0; i < glyph_arr_len; i++)
	{
		DT_ID_ULONG glyph_index = glyph_arr[i].glyph_index;

		glyph_arr_index[i] = glyph_index;
		glyph_arr_ybase[i] = glyph_arr[i].y;
		glyph_arr_width[i + 1] = glyph_arr[i + 1].x - glyph_arr[i].x;

		if (display_mode != 0) continue;

 		// Draw text run using a simple glyph-by-glyph approach

		DT_FLOAT glyph_x = kh * glyph_arr_x + start_width; glyph_arr_x += glyph_arr_width[i + 1];
		DT_FLOAT glyph_y = kv * glyph_arr[i].y;

		engine.GlyphDoOutput(glyph_index, tx + glyph_x, ty + glyph_y);
	}

	// Last (phantom) glyph
	glyph_arr_index[i] = DV_ENDOFGLYPHARR;
	glyph_arr_ybase[i] = 0;

	DT_FLOAT c = cos(DF_RAD(angle));
	DT_FLOAT s = sin(DF_RAD(angle));
	DT_FLOAT /*width0 = kh * glyph_arr[0].x,*/ text_width = 0.0;
	DT_TM2X2 tm = {{c * kh, -s * kv}, {s * kh, c * kv}}; // compute transform matrix if rotation is specified
	DT_ID_SWORD mode = DV_SPACING_FRAC; if (display_mode == 1) mode = DV_SPACING_DEVICE; else if (display_mode == 2) mode = DV_SPACING_DEVICE_2;

	if (display_mode == 0)
	{
		text_width = kh * (glyph_arr[i].x - glyph_arr[0].x);
	}
	else // draw line of glyphs using the GlyphsDoOutput function
	{
		text_width = engine.GlyphsDoOutput(tx, ty, 0, mode, tm, glyph_arr_index, glyph_arr_ybase, glyph_arr_width, start_width);
	}

	engine.OutputSetStyleEffects(Style_Label);

	if (debug_flags < 1) // debug only - draw the corresponding bounding box (useful when debugging since we can easily see the bound of each individual text run)
	{
		DT_TM2X2 rect_tm = {{c, -s}, {s, c}};
		DT_FLOAT rect_mx = (fontbox.ymn - fontbox.ymx) * kv, rect_mn = -fontbox.ymn * kv;
		engine.Shape_RectangleDashed(tx, ty, start_width /*+ width0*/, rect_mn, text_width, rect_mx, 0, 5.0, DV_NULL, 0.5, rect_tm);

		//DT_BMP bmp;
		//engine.GlyphsGetBound(DV_CROP_HMIN_VMIN /*DV_CROP_HMIN_VMAX*/, tx, ty, 0, mode, tm, glyph_arr_index, glyph_arr_ybase, glyph_arr_width, start_width, &bmp);
		//engine.Shape_RectangleDashed(0, 0, bmp.x, -bmp.y, bmp.w, bmp.h, 0, 5.0, DV_NULL, 0.5, DV_NULL);
	}

	return text_width; // return the width of this text run in pixels
}


DT_FLOAT CDTTextLine::DrawSingleRun_Ver(DT_FLOAT zoom, DT_SLONG tx, DT_SLONG ty, DT_ID_SWORD display_mode, DT_SWORD debug_flags, DT_ID_SWORD font_index, DT_SLONG font_size_w, DT_SLONG font_size_h, DT_SWORD angle, const LX_GLYPH glyph_arr[], DT_SLONG glyph_arr_len, DT_FLOAT start_height)
{
	if (glyph_arr == DV_NULL || glyph_arr_len < 1) return 0.0;
	if (display_mode == 0) angle = 0; // the simple glyph-by-glyph drawing approach does not support rotation

	DT_SLONG i;
	DT_UWORD font_em_h, font_em_v;
	DT_RECT_SWORD fontbox;
	CDTEngineV8& engine = FontMap->GetEngine();

	engine.FontGetMetrics(font_index, &font_em_h, &font_em_v, &fontbox);
	DT_SLONG ascender = engine.FontGetNumericValue(font_index, DV_NVAL_ASCENDER);

	// Compute font scaling factors
	DT_FLOAT kh = ScaleA(font_size_w, zoom); kh /= font_em_h;
	DT_FLOAT kv = ScaleA(font_size_h, zoom); kv /= font_em_v;

	DT_TYPE_EFFECTS_L type = {font_index, 0, 0, {{ScaleA(font_size_w, zoom), ScaleA(font_size_h, zoom), 0, 0, angle}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	engine.OutputSetStyleEffects(Style_Text);
	engine.TypesetterSetTypeEffects(type);

	DT_SWORD origin_at_zero = 1;
	DT_SLONG glyph_arr_y = origin_at_zero ? ascender : -glyph_arr[0].y;
	DT_SLONG glyph_arr_xbase[DV_MAX_GLYPHS_PER_LINE + 1];
	DT_SLONG glyph_arr_height[DV_MAX_GLYPHS_PER_LINE + 1];
	DT_ID_ULONG glyph_arr_index[DV_MAX_GLYPHS_PER_LINE + 1];

	glyph_arr_height[0] = glyph_arr_y;

	for (i = 0; i < glyph_arr_len; i++)
	{
		DT_ID_ULONG glyph_index = glyph_arr[i].glyph_index;

		glyph_arr_index[i] = glyph_index;
		glyph_arr_xbase[i] = glyph_arr[i].x;
		glyph_arr_height[i + 1] = glyph_arr[i].y - glyph_arr[i + 1].y;

		if (display_mode != 0) continue;

 		// Draw text run using a simple glyph-by-glyph approach

		DT_FLOAT glyph_x = kh * glyph_arr[i].x;
		DT_FLOAT glyph_y = kv * glyph_arr_y + start_height; glyph_arr_y += glyph_arr_height[i + 1];

		engine.GlyphDoOutput(glyph_index, tx + glyph_x, ty + glyph_y);
	}

	// Last (phantom) glyph
	glyph_arr_index[i] = DV_ENDOFGLYPHARR;
	glyph_arr_xbase[i] = 0;

	DT_FLOAT c = cos(DF_RAD(angle));
	DT_FLOAT s = sin(DF_RAD(angle));
	DT_FLOAT /*height0 = kv * glyph_arr[0].y,*/ text_height = 0.0;
	DT_TM2X2 tm = {{c * kh, -s * kv}, {s * kh, c * kv}}; // compute transform matrix if rotation is specified
	DT_ID_SWORD mode = DV_SPACING_VERTICAL | DV_SPACING_FRAC; if (display_mode == 1) mode = DV_SPACING_VERTICAL | DV_SPACING_DEVICE; else if (display_mode == 2) mode = DV_SPACING_VERTICAL | DV_SPACING_DEVICE_2;

	if (display_mode == 0)
	{
		text_height = kv * (glyph_arr[0].y - glyph_arr[i].y);
	}
	else // draw line of glyphs using the GlyphsDoOutput function
	{
		text_height = engine.GlyphsDoOutput(tx, ty, 0, mode, tm, glyph_arr_index, glyph_arr_xbase, glyph_arr_height, start_height);
	}

	engine.OutputSetStyleEffects(Style_Label);

	if (debug_flags < 1) // debug only - draw the corresponding bounding box (useful when debugging since we can easily see the bound of each individual text run)
	{
		DT_TM2X2 rect_tm = {{c, -s}, {s, c}};
		DT_FLOAT rect_mx = font_em_h * kh, rect_mn = -0.5 * rect_mx;
		engine.Shape_RectangleDashed(tx, ty, rect_mn, start_height /*- height0*/, rect_mx, text_height, 0, 5.0, DV_NULL, 0.5, rect_tm);

		//DT_BMP bmp;
		//engine.GlyphsGetBound(DV_CROP_HMIN_VMIN /*DV_CROP_HMIN_VMAX*/, tx, ty, 0, mode, tm, glyph_arr_index, glyph_arr_xbase, glyph_arr_height, start_height, &bmp);
		//engine.Shape_RectangleDashed(0, 0, bmp.x, -bmp.y, bmp.w, bmp.h, 0, 5.0, DV_NULL, 0.5, DV_NULL);
	}

	return text_height; // return the height of this text run in pixels
}


void CDTTextLine::DrawRuns_Hor(DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SLONG dx, DT_SLONG dy, DT_SLONG font_size_w, DT_SLONG font_size_h, DT_ID_SWORD display_mode, DT_SWORD debug_flags, const DT_CHAR* debug_label)
{
	if (FontMap == DV_NULL) return;

	CDTEngineV8& engine = FontMap->GetEngine();

	if (engine.GetLayoutCache() == DV_NULL) return;

	LX_GLYPH glyph_arr[DV_MAX_GLYPHS_PER_LINE];

	DT_SLONG i;
	DT_SLONG glyph_arr_len;
	DT_SLONG tx = ScaleB(dx, zoom) - x;
	DT_SLONG ty = ScaleB(dy, zoom) - y;
	DT_SWORD angle = 0; /* in degrees */
	DT_FLOAT start_width = 0.0;

	for (i = 0; i < TextRunCount; i++)
	{
		const TEXT_LAYOUT_RUN* r = TextRunArr + i;

		glyph_arr_len = ShapeSingleRun_Hor(r->CharPosFrom, r->CharPosTo - r->CharPosFrom, r->Level, r->Script, r->Language, r->FontIndex, glyph_arr, DV_MAX_GLYPHS_PER_LINE);
		start_width += DrawSingleRun_Hor(zoom, tx, ty, display_mode, debug_flags, r->FontIndex, font_size_w, font_size_h, angle, glyph_arr, glyph_arr_len, start_width);
	}

	if (debug_flags < 2) // debug only - draw the corresponding label
	{
		tx = ScaleB(dx - 0, zoom) - x;
		ty = ScaleB(dy - 40, zoom) - y;

		DT_TYPE_EFFECTS_L type = {FontMap->GetFontForUnicodeScript(latnScriptCode), 0, 0, {{ScaleA(font_size_w - 4, zoom), ScaleA(font_size_h - 4, zoom), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

		engine.TypesetterSetTypeEffects(type);
		engine.CharsDoOutput(tx, ty, 0, DV_SPACING_DEVICE_2, DV_NULL, debug_label);
	}
}


void CDTTextLine::DrawRuns_Ver(DT_FLOAT zoom, DT_SLONG x, DT_SLONG y, DT_SLONG dx, DT_SLONG dy, DT_SLONG font_size_w, DT_SLONG font_size_h, DT_ID_SWORD display_mode, DT_SWORD debug_flags, const DT_CHAR* debug_label)
{
	if (FontMap == DV_NULL) return;

	CDTEngineV8& engine = FontMap->GetEngine();

	if (engine.GetLayoutCache() == DV_NULL) return;

	LX_GLYPH glyph_arr[DV_MAX_GLYPHS_PER_LINE];

	DT_SLONG i;
	DT_SLONG glyph_arr_len;
	DT_SLONG tx = ScaleB(dx, zoom) - x;
	DT_SLONG ty = ScaleB(dy, zoom) - y;
	DT_SWORD angle = 0; /* in degrees */
	DT_FLOAT start_height = 0.0;

	for (i = 0; i < TextRunCount; i++)
	{
		const TEXT_LAYOUT_RUN* r = TextRunArr + i;

		glyph_arr_len = ShapeSingleRun_Ver(r->CharPosFrom, r->CharPosTo - r->CharPosFrom, r->Level, r->Script, r->Language, r->FontIndex, glyph_arr, DV_MAX_GLYPHS_PER_LINE);
		start_height += DrawSingleRun_Ver(zoom, tx, ty, display_mode, debug_flags, r->FontIndex, font_size_w, font_size_h, angle, glyph_arr, glyph_arr_len, start_height);
	}

	if (debug_flags < 2) // debug only - draw the corresponding label
	{
		tx = ScaleB(dx - 40, zoom) - x;
		ty = ScaleB(dy - 0, zoom) - y;

		DT_TYPE_EFFECTS_L type = {FontMap->GetFontForUnicodeScript(latnScriptCode), 0, 0, {{ScaleA(font_size_w - 4, zoom), ScaleA(font_size_h - 4, zoom), 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

		engine.TypesetterSetTypeEffects(type);
		engine.CharsDoOutput(tx, ty, 0, DV_SPACING_VERTICAL | DV_SPACING_DEVICE_2, DV_NULL, debug_label);
	}
}
