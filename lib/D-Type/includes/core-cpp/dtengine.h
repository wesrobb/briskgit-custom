/*
 * engine.h - D-Type Engine header
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#ifndef DTYPE_ENGINECPP_H
#define DTYPE_ENGINECPP_H

/*
 * DEF_ENGINE_LEVEL
 *
 * Level 0: Only C API
 * Level 1: D-Type Standard Engine
 * Level 2: Level 1 + D-Type Text Layout Extension (with a caching subsystem)
 * Level 3: Level 2 + D-Type Power Engine
 * Level 4: Level 3 + D-Type Text Engine
 *
 */


#ifndef DEF_ENGINE_LEVEL
#error "dtengine.h: DEF_ENGINE_LEVEL not defined! It must be 0, 1, 2, 3 or 4."
#elif (DEF_ENGINE_LEVEL == 0)
#else

class CDTEngineV8
{
private: // private copy constructor and assignment operator

	CDTEngineV8(const CDTEngineV8&);
	CDTEngineV8& operator = (const CDTEngineV8&);

public:

	CDTEngineV8() { Reset(); }
	CDTEngineV8(const DT_STREAM_DESC* sd_init, const DT_STREAM_DESC* sd_fontmap = DV_NULL, DT_INIT_INFO* init_info = DV_NULL) { Init(sd_init, sd_fontmap, init_info); }
	CDTEngineV8(const DT_STREAM_DESC& sd_init, const DT_STREAM_DESC& sd_fontmap, DT_INIT_INFO* init_info = DV_NULL) { Init(&sd_init, &sd_fontmap, init_info); }
	CDTEngineV8(const DT_STREAM_DESC& sd_init, DT_INIT_INFO* init_info = DV_NULL) { Init(&sd_init, DV_NULL, init_info); }
	virtual ~CDTEngineV8() { Exit(); }

	bool Init(const DT_STREAM_DESC* sd_init, const DT_STREAM_DESC* sd_fontmap = DV_NULL, DT_INIT_INFO* init_info = DV_NULL);
	bool Init(const DT_STREAM_DESC& sd_init, const DT_STREAM_DESC& sd_fontmap, DT_INIT_INFO* init_info = DV_NULL) { return Init(&sd_init, &sd_fontmap, init_info); }
	bool Init(const DT_STREAM_DESC& sd_init, DT_INIT_INFO* init_info = DV_NULL) { return Init(&sd_init, DV_NULL, init_info); }
	void Reset();
	void Exit();

public: // Public methods

	inline bool InitFailed() { return (Engine == DV_NULL); }

#if (DEF_ENGINE_LEVEL == 1)
	typedef DT_DTENGINE DT_ENGINE;
	inline DT_DTENGINE GetDtEngine() const { return Engine; }
	inline DT_SLONG GetVersion() const { return dtEngineGetVersion(Engine, DV_NULL, DV_NULL, 0); }
	inline void Free(void* mem_addr) { dtFree(mem_addr); }
#elif (DEF_ENGINE_LEVEL == 2)
	typedef DT_DTENGINE DT_ENGINE;
	inline DT_DTENGINE GetDtEngine() const { return Engine; }
	inline DT_LXCACHE GetLayoutCache() const { return LayoutCache; }
	inline DT_SLONG GetVersion() const { return dtEngineGetVersion(Engine, DV_NULL, DV_NULL, 0); }
	inline void Free(void* mem_addr) { dtFree(mem_addr); }
#elif (DEF_ENGINE_LEVEL == 3)
	typedef DT_PDENGINE DT_ENGINE;
	inline DT_PDENGINE GetPdEngine() const { return Engine; }
	inline DT_DTENGINE GetDtEngine() const { DT_DTENGINE dt_engine = DV_NULL; pdEngineGetDTypeEngInst(Engine, &dt_engine); return dt_engine; }
	inline DT_LXCACHE GetLayoutCache() const { DT_LXCACHE layout_cache = DV_NULL; pdEngineGetLayoutCacheInst(Engine, &layout_cache); return layout_cache; }
	inline DT_SLONG GetVersion() const { return pdEngineGetVersion(Engine, DV_NULL, DV_NULL, 0); }
	inline void Free(void* mem_addr) { pdFree(mem_addr); }
#else
	typedef DT_TXENGINE DT_ENGINE;
	inline DT_TXENGINE GetTxEngine() const { return Engine; }
	inline DT_PDENGINE GetPdEngine() const { DT_PDENGINE pd_engine = DV_NULL; txEngineGetPowerEngInst(Engine, &pd_engine); return pd_engine; }
	inline DT_DTENGINE GetDtEngine() const { DT_DTENGINE dt_engine = DV_NULL; txEngineGetDTypeEngInst(Engine, &dt_engine); return dt_engine; }
	inline DT_LXCACHE GetLayoutCache() const { DT_LXCACHE layout_cache = DV_NULL; pdEngineGetLayoutCacheInst(GetPdEngine(), &layout_cache); return layout_cache; }
	inline DT_SLONG GetVersion() const { return txEngineGetVersion(Engine, DV_NULL, DV_NULL, 0); }
	inline void Free(void* mem_addr) { txFree(mem_addr); }
#endif

#ifdef DEF_ENGINE_BUILDER_CLIPS
	inline DT_PDDOC GetClip(DT_SWORD i) const { return Clip[i]; }
#endif

	inline const DT_STREAM_DESC* GetDefaultFontmap() const { return &DefaultFontmap; }
	inline const DT_STREAM_DESC& GetDefaultFontmapRef() const { return DefaultFontmap; }

	/* Output */

	inline DT_SWORD OutputSetMDC(DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface, DT_SRAST_L clip_x, DT_SRAST_L clip_y, DT_SRAST_L clip_w, DT_SRAST_L clip_h) { return dtOutputSetMDC(GetDtEngine(), format, subformat, memory_surface, clip_x, clip_y, clip_w, clip_h); }
	inline DT_SWORD OutputSetMDC(DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC& memory_surface, DT_SRAST_L clip_x, DT_SRAST_L clip_y, DT_SRAST_L clip_w, DT_SRAST_L clip_h) { return dtOutputSetMDC(GetDtEngine(), format, subformat, &memory_surface, clip_x, clip_y, clip_w, clip_h); }

	inline DT_SWORD OutputSetStyleEffects(const DT_STYLE_EFFECTS* style) { return dtOutputSetStyleEffects(GetDtEngine(), style, 0); }
	inline DT_SWORD OutputSetStyleEffects(const DT_STYLE_EFFECTS& style) { return dtOutputSetStyleEffects(GetDtEngine(), &style, 0); }
	inline DT_SWORD OutputGetStyleEffects(DT_STYLE_EFFECTS* style) const { return dtOutputGetStyleEffects(GetDtEngine(), style, 0); }

	/* Rasterizer */

	inline DT_SWORD RasterizerRealloc(DT_SWORD raster_width, DT_SWORD raster_height, DT_SWORD raster_intersect, DT_ID_SWORD raster_id) { return dtRasterizerRealloc(GetDtEngine(), raster_width, raster_height, raster_intersect, raster_id); }

	inline DT_SWORD RasterizerSetFillRule(DT_ID_UBYTE fill_rule) { return dtRasterizerSetFillRule(GetDtEngine(), fill_rule); }
	inline DT_SWORD RasterizerGetFillRule(DT_ID_UBYTE* fill_rule) const { return dtRasterizerGetFillRule(GetDtEngine(), fill_rule); }

	inline DT_SWORD RasterizerSetGrayscale(const DT_UBYTE gsv[]) { return dtRasterizerSetGrayscale(GetDtEngine(), gsv); }
	inline DT_SWORD RasterizerGetGrayscale(DT_UBYTE gsv[]) const { return dtRasterizerGetGrayscale(GetDtEngine(), gsv); }

	/* Stored Style */

	inline DT_SWORD StyleSet(DT_UBYTE r, DT_UBYTE g = 0, DT_UBYTE b = 0, DT_UBYTE t = 0, DT_SLONG effects_len = 0, const DT_SLONG effects_arr[] = DV_NULL) { StoredStyle.rgbt[0] = r; StoredStyle.rgbt[1] = g; StoredStyle.rgbt[2] = b; StoredStyle.rgbt[3] = t; StoredStyle.effects_len = effects_len; for (DT_SLONG i = 0; i < effects_len; i++) StoredStyle.effects_arr[i] = effects_arr[i]; return dtOutputSetStyleEffects(GetDtEngine(), &StoredStyle, 0); }
	inline DT_SWORD StyleSetRGBT(DT_UBYTE r, DT_UBYTE g = 0, DT_UBYTE b = 0, DT_UBYTE t = 0) { StoredStyle.rgbt[0] = r; StoredStyle.rgbt[1] = g; StoredStyle.rgbt[2] = b; StoredStyle.rgbt[3] = t; return dtOutputSetStyleEffects(GetDtEngine(), &StoredStyle, 0); }
	inline DT_SWORD StyleSetEffects(DT_SLONG effects_len, const DT_SLONG effects_arr[] = DV_NULL) { StoredStyle.effects_len = effects_len; for (DT_SLONG i = 0; i < effects_len; i++) StoredStyle.effects_arr[i] = effects_arr[i]; return dtOutputSetStyleEffects(GetDtEngine(), &StoredStyle, 0); }

	/* Stored Type */

	inline DT_SWORD TypeSet(DT_ID_SWORD font_index, DT_SRAST_L size_h = 0, DT_SRAST_L size_v = 0, DT_SWORD skew_h = 0, DT_SWORD skew_v = 0, DT_SWORD rotation = 0, DT_SRAST_L thickness = 0, DT_SRAST_L segment = 0, DT_UWORD dash_size = 0) { StoredType.font_index = font_index; StoredType.transform.params.size_h = size_h; if (size_v == 0) StoredType.transform.params.size_v = size_h; else StoredType.transform.params.size_v = size_v; StoredType.transform.params.skew_h = skew_h; StoredType.transform.params.skew_v = skew_v; StoredType.transform.params.rotation = rotation; StoredType.linedecor.thickness = thickness; StoredType.linedecor.segment = segment; StoredType.linedecor.dash_size = dash_size; return dtTypesetterSetTypeEffects(GetDtEngine(), &StoredType, 0); }
	inline DT_SWORD TypeSetFont(DT_ID_SWORD font_index) { StoredType.font_index = font_index; return dtTypesetterSetTypeEffects(GetDtEngine(), &StoredType, 0); }
	inline DT_SWORD TypeSetSize(DT_SRAST_L size_h, DT_SRAST_L size_v = 0) { StoredType.transform.params.size_h = size_h; if (size_v == 0) StoredType.transform.params.size_v = size_h; else StoredType.transform.params.size_v = size_v; return dtTypesetterSetTypeEffects(GetDtEngine(), &StoredType, 0); }
	inline DT_SWORD TypeSetSkew(DT_SWORD skew_h, DT_SWORD skew_v = 0) { StoredType.transform.params.skew_h = skew_h; StoredType.transform.params.skew_v = skew_v; return dtTypesetterSetTypeEffects(GetDtEngine(), &StoredType, 0); }
	inline DT_SWORD TypeSetRotation(DT_SWORD rotation) { StoredType.transform.params.rotation = rotation; return dtTypesetterSetTypeEffects(GetDtEngine(), &StoredType, 0); }
	inline DT_SWORD TypeSetOutline(DT_SRAST_L thickness, DT_SRAST_L segment = 0, DT_UWORD dash_size = 0) { StoredType.linedecor.thickness = thickness; StoredType.linedecor.segment = segment; StoredType.linedecor.dash_size = dash_size; return dtTypesetterSetTypeEffects(GetDtEngine(), &StoredType, 0); }

	/* Typesetter */

	inline DT_SWORD TypesetterSetCacheCompression(DT_ID_UBYTE bitmap_compress) { return dtTypesetterSetCacheCompression(GetDtEngine(), bitmap_compress, 0); }
	inline DT_SWORD TypesetterGetCacheCompression(DT_ID_UBYTE* bitmap_compress) const { return dtTypesetterGetCacheCompression(GetDtEngine(), bitmap_compress); }

	inline DT_SWORD TypesetterSetCachePolicy(DT_SLONG max_bitmap_bytes, DT_UBYTE flag = 0) { return dtTypesetterSetCachePolicy(GetDtEngine(), max_bitmap_bytes, -1, -1, -1, -1, flag); }
	inline DT_SWORD TypesetterGetCachePolicy(DT_SLONG* max_bitmap_bytes) const { return dtTypesetterGetCachePolicy(GetDtEngine(), max_bitmap_bytes, DV_NULL, DV_NULL, DV_NULL, DV_NULL); }

	inline DT_SWORD TypesetterSetQuality(DT_ID_UBYTE quality, DT_UBYTE flag = 0) { return dtTypesetterSetQuality(GetDtEngine(), quality, flag); }
	inline DT_SWORD TypesetterGetQuality(DT_ID_UBYTE* quality) const { return dtTypesetterGetQuality(GetDtEngine(), quality); }

	inline DT_SWORD TypesetterSetPositioning(DT_ID_UBYTE positioning, DT_UBYTE flag = 0) { return dtTypesetterSetPositioning(GetDtEngine(), positioning, flag); }
	inline DT_SWORD TypesetterGetPositioning(DT_ID_UBYTE* positioning) const { return dtTypesetterGetPositioning(GetDtEngine(), positioning); }

	inline DT_SWORD TypesetterSetSubpixels(DT_UBYTE subpixels, DT_UBYTE flag = 0) { return dtTypesetterSetSubpixels(GetDtEngine(), subpixels, flag); }
	inline DT_SWORD TypesetterGetSubpixels(DT_UBYTE* subpixels) const { return dtTypesetterGetSubpixels(GetDtEngine(), subpixels); }

	inline DT_SWORD TypesetterSetHinting(DT_ID_UBYTE hinting, DT_UBYTE flag = 0) { return dtTypesetterSetHinting(GetDtEngine(), hinting, flag); }
	inline DT_SWORD TypesetterGetHinting(DT_ID_UBYTE* hinting) const { return dtTypesetterGetHinting(GetDtEngine(), hinting); }

	inline DT_SWORD TypesetterSetHintThreshold(DT_SLONG min_hint_dpx, DT_SLONG min_hint_dpy, DT_ID_UBYTE scale_id = DV_SCALE_256) { return dtTypesetterSetHintThreshold(GetDtEngine(), min_hint_dpx, min_hint_dpy, scale_id, 0); }
	inline DT_SWORD TypesetterGetHintThreshold(DT_SLONG* min_hint_dpx, DT_SLONG* min_hint_dpy, DT_ID_UBYTE scale_id = DV_SCALE_256) const { return dtTypesetterGetHintThreshold(GetDtEngine(), min_hint_dpx, min_hint_dpy, scale_id); }

	inline DT_SWORD TypesetterSetHintRounding(DT_SWORD rounding_x, DT_SWORD rounding_y) { return dtTypesetterSetHintRounding(GetDtEngine(), rounding_x, rounding_y, 0); }
	inline DT_SWORD TypesetterGetHintRounding(DT_SWORD* rounding_x, DT_SWORD* rounding_y) const { return dtTypesetterGetHintRounding(GetDtEngine(), rounding_x, rounding_y); }

	inline DT_SWORD TypesetterSetAlignMethod(DT_ID_UBYTE align_method_x, DT_ID_UBYTE align_method_y) { return dtTypesetterSetAlignMethod(GetDtEngine(), align_method_x, align_method_y, 0); }
	inline DT_SWORD TypesetterGetAlignMethod(DT_ID_UBYTE* align_method_x, DT_ID_UBYTE* align_method_y) const { return dtTypesetterGetAlignMethod(GetDtEngine(), align_method_x, align_method_y); }

	inline DT_SWORD TypesetterSetSizeSubscale(DT_UBYTE transform_params_mul_h, DT_UBYTE transform_params_mul_v) { return dtTypesetterSetSizeSubscale(GetDtEngine(), transform_params_mul_h, transform_params_mul_v, 0); }
	inline DT_SWORD TypesetterGetSizeSubscale(DT_UBYTE* transform_params_mul_h, DT_UBYTE* transform_params_mul_v) const { return dtTypesetterGetSizeSubscale(GetDtEngine(), transform_params_mul_h, transform_params_mul_v); }

	inline DT_SWORD TypesetterSetTypeEffectsTransformed(DT_TYPE_EFFECTS_L* type, const DT_TM2X2 tm_2x2 = DV_NULL, DT_ID_UBYTE flag = 0) { return dtTypesetterSetTypeEffectsTransformed(GetDtEngine(), type, tm_2x2, flag, 0); }
	inline DT_SWORD TypesetterSetTypeEffectsTransformed(DT_TYPE_EFFECTS_L& type, const DT_TM2X2 tm_2x2 = DV_NULL, DT_ID_UBYTE flag = 0) { return dtTypesetterSetTypeEffectsTransformed(GetDtEngine(), &type, tm_2x2, flag, 0); }
	inline DT_SWORD TypesetterSetTypeEffects(const DT_TYPE_EFFECTS_L* type) { return dtTypesetterSetTypeEffects(GetDtEngine(), type, 0); }
	inline DT_SWORD TypesetterSetTypeEffects(const DT_TYPE_EFFECTS_L& type) { return dtTypesetterSetTypeEffects(GetDtEngine(), &type, 0); }
	inline DT_SWORD TypesetterGetTypeEffects(DT_TYPE_EFFECTS_L* type) const { return dtTypesetterGetTypeEffects(GetDtEngine(), type, 0); }

	inline DT_SWORD TypesetterTransformTypeEffects(DT_TYPE_EFFECTS_L* type, const DT_TM2X2 tm_2x2, DT_ID_UBYTE optimize) { return dtTypesetterTransformTypeEffects(GetDtEngine(), type, tm_2x2, optimize, 0); }
	inline DT_SWORD TypesetterTransformTypeEffects(DT_TYPE_EFFECTS_L& type, const DT_TM2X2 tm_2x2, DT_ID_UBYTE optimize) { return dtTypesetterTransformTypeEffects(GetDtEngine(), &type, tm_2x2, optimize, 0); }

	inline DT_SWORD TypesetterRealloc(DT_SLONG cache_bitmap_bytes, DT_SLONG cache_bitmap_items, DT_SLONG cache_transform_items) { return dtTypesetterRealloc(GetDtEngine(), cache_bitmap_bytes, cache_bitmap_items, cache_transform_items, 0); }

	/* Patterns */

	inline DT_SWORD PatternGetNrOfGroups() const { return dtPatternGetNrOfGroups(GetDtEngine()); }
	inline DT_SWORD PatternGetGroupCount(DT_ID_SWORD group_index) const { return dtPatternGetGroupCount(GetDtEngine(), group_index); }
	inline DT_UBYTE* PatternGetBitmap(DT_ID_SWORD group_index, DT_ID_SWORD pattern_index) const { return dtPatternGetBitmap(GetDtEngine(), group_index, pattern_index); }

	/* Font */

	inline DT_SWORD FontGetCounter() const { return dtFontGetCounter(GetDtEngine()); }
	inline DT_ID_SWORD FontTest(DT_ID_SWORD slot_index) const { return dtFontTest(GetDtEngine(), slot_index); }
	inline DT_ID_SWORD FontFind(const DT_CHAR* fuid, DT_ID_SWORD default_font_id = 0) const { DT_ID_SWORD font_id = dtFontFind(GetDtEngine(), fuid); return (font_id < 0 ? default_font_id : font_id); }
	inline DT_ID_SWORD FontFindMMInstance(DT_ID_SWORD font_index, DT_ID_SWORD contribs_flag, DT_SWORD contribs_len, const DT_SLONG contribs[]) const { return dtFontFindMMInstancePlus(GetDtEngine(), font_index, contribs_flag, contribs_len, contribs); }

	inline DT_SWORD FontSetErrorCallback(void (DTYPE_USRCALL *user_error_func)(const DT_CHAR* error_message, void* user_param), void* user_param = DV_NULL) { return dtFontSetErrorCallback(GetDtEngine(), user_error_func, user_param); }

	inline DT_ID_SWORD FontAddViaStream(DT_ID_SWORD font_format_id, const DT_CHAR* fuid, DT_SWORD fcnr, DT_ID_SWORD cmap_id, DT_ID_UBYTE caching, DT_ID_UBYTE hinting, const DT_STREAM_DESC* sd) { return dtFontAddViaStream(GetDtEngine(), font_format_id, fuid, fcnr, cmap_id, caching, hinting, sd); }
	inline DT_ID_SWORD FontAddViaStream(DT_ID_SWORD font_format_id, const DT_CHAR* fuid, DT_SWORD fcnr, DT_ID_SWORD cmap_id, DT_ID_UBYTE caching, DT_ID_UBYTE hinting, const DT_STREAM_DESC& sd) { return dtFontAddViaStream(GetDtEngine(), font_format_id, fuid, fcnr, cmap_id, caching, hinting, &sd); }
	inline DT_ID_SWORD FontAddViaStreams(DT_ID_SWORD font_format_id, const DT_CHAR* fuid, DT_SWORD fcnr, DT_ID_SWORD cmap_id, DT_ID_UBYTE caching, DT_ID_UBYTE hinting, DT_UBYTE arr_size, const DT_STREAM_DESC* sd[]) { return dtFontAddViaStreams(GetDtEngine(), font_format_id, fuid, fcnr, cmap_id, caching, hinting, arr_size, sd); }
	inline DT_ID_SWORD FontAddAsMMInstance(DT_ID_SWORD font_index, DT_ID_SWORD contribs_flag, DT_SWORD contribs_len, const DT_SLONG contribs[]) { return dtFontAddAsMMInstancePlus(GetDtEngine(), font_index, contribs_flag, contribs_len, contribs); }
	inline DT_SWORD FontRemove(DT_ID_SWORD font_index) { return dtFontRemove(GetDtEngine(), font_index); }
	inline DT_SWORD FontRemoveAll() { return dtFontRemoveAll(GetDtEngine()); }

	inline DT_SWORD FontMakeActive(DT_ID_SWORD font_index, DT_ID_UBYTE level, DT_SWORD* last_fcnr) { return dtFontMakeActive(GetDtEngine(), font_index, level, last_fcnr); }
	inline DT_SWORD FontMakeInactive(DT_ID_SWORD font_index, DT_ID_UBYTE respect_multireference = 0) { return dtFontMakeInactive(GetDtEngine(), font_index, respect_multireference); }
	inline DT_SWORD FontMakeInactiveAll() { return dtFontMakeInactiveAll(GetDtEngine()); }

	inline DT_SLONG FontSaveToStream(DT_ID_SWORD font_index, DT_STREAM_DESC* sd, DT_SWORD (DTYPE_USRCALL *font_subset_func)(DT_ID_ULONG glyph_index, void* user_param) = DV_NULL, void* user_param = DV_NULL) { return dtFontSaveToStream(GetDtEngine(), font_index, 0, sd, font_subset_func, user_param); }
	inline DT_SLONG FontSaveToStream(DT_ID_SWORD font_index, DT_STREAM_DESC& sd, DT_SWORD (DTYPE_USRCALL *font_subset_func)(DT_ID_ULONG glyph_index, void* user_param) = DV_NULL, void* user_param = DV_NULL) { return dtFontSaveToStream(GetDtEngine(), font_index, 0, &sd, font_subset_func, user_param); }

	inline DT_SWORD FontGetSysStatus(DT_ID_SWORD font_index, DT_ID_SWORD* font_format_id = DV_NULL, DT_CHAR* fuid = DV_NULL, DT_SWORD* fcnr = DV_NULL, DT_ID_SWORD* cmap_id = DV_NULL, DT_ID_UBYTE* caching = DV_NULL, DT_ID_UBYTE* hinting = DV_NULL, DT_ID_UBYTE* active = DV_NULL) const { return dtFontGetSysStatus(GetDtEngine(), font_index, font_format_id, fuid, fcnr, cmap_id, caching, hinting, active); }
	inline DT_SWORD FontGetMMAxisInfo(DT_ID_SWORD font_index, DT_SWORD* dim = DV_NULL, DT_SLONG* inst_count = DV_NULL) const { return dtFontGetMMAxisInfo(GetDtEngine(), font_index, dim, inst_count); }
	inline DT_SLONG FontGetStringValue(DT_ID_SWORD font_index, DT_ID_SWORD string_id, DT_UBYTE* buffer, DT_SLONG max_len, bool null_terminate = true) const
	{
		if (buffer == DV_NULL || max_len < 1) return 0;
		DT_SLONG req_len = dtFontGetStringValue(GetDtEngine(), font_index, string_id, buffer, max_len);
		if (null_terminate && req_len >= 0) { if (req_len < max_len) buffer[req_len] = 0; else buffer[max_len - 1] = 0; }
		return req_len;
	}
	inline DT_SLONG FontGetStringValues(DT_UWORD flags, DT_ID_SWORD font_index, DT_ID_SWORD string_id, DT_UBYTE* buffer, DT_SLONG max_len, DT_SLONG* max_arr_len, DT_ULONG lang_and_platform_id_arr[], DT_SLONG langtag_offset_arr[], DT_SLONG langtag_len_arr[], DT_SLONG string_offset_array[], DT_SLONG string_len_array[], bool null_terminate = true) const
	{
		if (buffer == DV_NULL || max_len < 1) return 0;
		DT_SLONG req_len = dtFontGetStringValues(GetDtEngine(), flags, font_index, string_id, buffer, max_len, max_arr_len, lang_and_platform_id_arr, langtag_offset_arr, langtag_len_arr, string_offset_array, string_len_array);
		if (null_terminate && req_len >= 0) { if (req_len < max_len) buffer[req_len] = 0; else buffer[max_len - 1] = 0; }
		return req_len;
	}
	inline DT_SLONG FontGetNumericValue(DT_ID_SWORD font_index, DT_ID_SWORD numeric_id) const { return dtFontGetNumericValue(GetDtEngine(), font_index, numeric_id); }
	inline DT_ULONG FontGetNrOfGlyphs(DT_ID_SWORD font_index) const { return dtFontGetNrOfGlyphs(GetDtEngine(), font_index); }
	inline DT_SWORD FontGetMetrics(DT_ID_SWORD font_index, DT_UWORD* font_xbs, DT_UWORD* font_ybs, DT_RECT_SWORD* extent = DV_NULL) const { return dtFontGetMetrics(GetDtEngine(), font_index, font_xbs, font_ybs, extent); }
	inline DT_ID_ULONG FontGetGlyphIndex(DT_ID_SWORD font_index, DT_ID_ULONG char_code) const { return dtFontGetGlyphIndex(GetDtEngine(), font_index, char_code); }

	/* Glyph */

	inline DT_SWORD GlyphDoOutput(DT_ID_ULONG glyph_index, DT_FLOAT x, DT_FLOAT y, DT_SWORD reserved = 0, DT_BMP* memory_bitmap = DV_NULL) { return dtGlyphDoOutput(GetDtEngine(), glyph_index, x, y, reserved, memory_bitmap); }
	inline DT_SLONG GlyphDoTransform(DT_ID_ULONG glyph_index, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD flag, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_FLOAT x_arr[], DT_FLOAT y_arr[], DT_BMP* memory_bitmap = DV_NULL) { return dtGlyphDoTransform(GetDtEngine(), glyph_index, x, y, flag, max_points, i_arr, x_arr, y_arr, memory_bitmap); }

	inline DT_SWORD GlyphGetMetrics(DT_ID_SWORD font_index, DT_ID_ULONG glyph_index, DT_UWORD* advance_width, DT_RECT_SWORD* extent = DV_NULL) const { return dtGlyphGetMetrics(GetDtEngine(), font_index, glyph_index, advance_width, extent); }
	inline DT_SWORD GlyphGetMetrics(DT_ID_SWORD font_index, DT_ID_ULONG glyph_index, DT_ADVANCE* advance, DT_RECT_SWORD* extent = DV_NULL) const { return dtGlyphGetMetricsPlus(GetDtEngine(), font_index, glyph_index, advance, extent, 0); }
	inline DT_SLONG GlyphGetOutline(DT_ID_SWORD font_index, DT_ID_ULONG glyph_index, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_SLONG x_arr[], DT_SLONG y_arr[], DT_SLONG reserved = 0) const { return dtGlyphGetOutline(GetDtEngine(), font_index, glyph_index, max_points, i_arr, x_arr, y_arr, reserved); }
	inline DT_SWORD GlyphGetKern(DT_ID_SWORD font_index, DT_ID_ULONG glyph_index_1, DT_ID_ULONG glyph_index_2, DT_SWORD* kerning_x, DT_SWORD* kerning_y = DV_NULL) const { return dtGlyphGetKern(GetDtEngine(), font_index, glyph_index_1, glyph_index_2, kerning_x, kerning_y); }

	/* Char */

	inline DT_SWORD CharDoOutput(DT_ID_ULONG char_code, DT_FLOAT x, DT_FLOAT y, DT_SWORD reserved = 0, DT_BMP* memory_bitmap = DV_NULL) { return dtCharDoOutput(GetDtEngine(), char_code, x, y, reserved, memory_bitmap); }
	inline DT_SLONG CharDoTransform(DT_ID_ULONG char_code, DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD flag, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_FLOAT x_arr[], DT_FLOAT y_arr[], DT_BMP* memory_bitmap = DV_NULL) { return dtCharDoTransform(GetDtEngine(), char_code, x, y, flag, max_points, i_arr, x_arr, y_arr, memory_bitmap); }

	inline DT_SWORD CharGetMetrics(DT_ID_SWORD font_index, DT_ID_ULONG char_code, DT_UWORD* advance_width, DT_RECT_SWORD* extent = DV_NULL) const { return dtCharGetMetrics(GetDtEngine(), font_index, char_code, advance_width, extent); }
	inline DT_SWORD CharGetMetrics(DT_ID_SWORD font_index, DT_ID_ULONG char_code, DT_ADVANCE* advance, DT_RECT_SWORD* extent = DV_NULL) const { return dtCharGetMetricsPlus(GetDtEngine(), font_index, char_code, advance, extent, 0); }
	inline DT_SLONG CharGetOutline(DT_ID_SWORD font_index, DT_ID_ULONG char_code, DT_SLONG max_points, DT_ID_UBYTE i_arr[], DT_SLONG x_arr[], DT_SLONG y_arr[], DT_SLONG reserved = 0) const { return dtCharGetOutline(GetDtEngine(), font_index, char_code, max_points, i_arr, x_arr, y_arr, reserved); }
	inline DT_SWORD CharGetKern(DT_ID_SWORD font_index, DT_ID_ULONG char_code_1, DT_ID_ULONG char_code_2, DT_SWORD* kerning_x, DT_SWORD* kerning_y = DV_NULL) const { return dtCharGetKern(GetDtEngine(), font_index, char_code_1, char_code_2, kerning_x, kerning_y); }

	/* Shapes */

	inline DT_SWORD ShapesDoOutput(DT_ID_SWORD bitmap_flag, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG* extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[], DT_BMP* memory_bitmap = DV_NULL) { return dtShapesDoOutput(GetDtEngine(), DV_NULL, bitmap_flag, x, y, extent, flag, i_arr, x_arr, y_arr, memory_bitmap); }
	inline DT_SWORD ShapesDoOutput(DT_ID_SWORD bitmap_flag, DT_SLONG x, DT_SLONG y, const DT_RECT_SLONG& extent, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[], DT_BMP* memory_bitmap = DV_NULL) { return dtShapesDoOutput(GetDtEngine(), DV_NULL, bitmap_flag, x, y, &extent, flag, i_arr, x_arr, y_arr, memory_bitmap); }

	/* Bitmap */

	inline void BitmapDoOutput(const DT_BMP* memory_bitmap, DT_SLONG x, DT_SLONG y) { dtBitmapDoOutput(GetDtEngine(), memory_bitmap, x, y); }
	inline void BitmapDoOutput(const DT_BMP& memory_bitmap, DT_SLONG x, DT_SLONG y) { dtBitmapDoOutput(GetDtEngine(), &memory_bitmap, x, y); }
	inline void BitmapInvert(DT_BMP* memory_bitmap) { dtBitmapInvert(GetDtEngine(), memory_bitmap); }
	inline void BitmapInvert(DT_BMP& memory_bitmap) { dtBitmapInvert(GetDtEngine(), &memory_bitmap); }

	inline DT_SLONG BitmapCompress(DT_UBYTE* bitmap_addr, DT_SLONG bitmap_len) { return dtBitmapCompress(GetDtEngine(), bitmap_addr, bitmap_len); }
	inline DT_SLONG BitmapCompress(DT_UBYTE& bitmap_addr, DT_SLONG bitmap_len) { return dtBitmapCompress(GetDtEngine(), &bitmap_addr, bitmap_len); }
	inline DT_UBYTE* BitmapDecompress(const DT_UBYTE* bitmap_addr, DT_SLONG bitmap_len) { return dtBitmapDecompress(GetDtEngine(), bitmap_addr, bitmap_len); }
	inline DT_UBYTE* BitmapDecompress(const DT_UBYTE& bitmap_addr, DT_SLONG bitmap_len) { return dtBitmapDecompress(GetDtEngine(), &bitmap_addr, bitmap_len); }

	inline void BitmapFree(DT_UBYTE* bitmap_addr) { dtBitmapFree(GetDtEngine(), bitmap_addr); }
	inline void BitmapFree(DT_UBYTE& bitmap_addr) { dtBitmapFree(GetDtEngine(), &bitmap_addr); }

	/* Glyphs */

	inline DT_FLOAT GlyphsGetBound(DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[], const DT_SLONG glyph_arr_displacement[], const DT_SLONG glyph_arr_advance[], DT_FLOAT start_advance, DT_BMP* bmp) const { return dtxGlyphsGetBound(GetDtEngine(), crop_type, x, y, spacing, kern_mode, tm, glyph_arr, glyph_arr_displacement, glyph_arr_advance, start_advance, bmp); }
	inline DT_FLOAT GlyphsDoOutput(DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[], const DT_SLONG glyph_arr_displacement[], const DT_SLONG glyph_arr_advance[], DT_FLOAT start_advance) { return dtxGlyphsDoOutput(GetDtEngine(), x, y, spacing, kern_mode, tm, glyph_arr, glyph_arr_displacement, glyph_arr_advance, start_advance); }
	inline void GlyphsDoOutput_Arc(DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[]) { dtxGlyphsDoOutput_Arc(GetDtEngine(), xc, yc, rc, alpha0, delta, effects, spacing, kern_mode, tm, glyph_arr); }
	inline void GlyphsDoOutput_Arc(DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L& effects, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[]) { dtxGlyphsDoOutput_Arc(GetDtEngine(), xc, yc, rc, alpha0, delta, &effects, spacing, kern_mode, tm, glyph_arr); }

	/* Chars */

	inline DT_FLOAT CharsGetBound(DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[], DT_BMP* bmp) const { return dtxCharsGetBound(GetDtEngine(), crop_type, x, y, spacing, kern_mode, tm, chars_arr, bmp); }
	inline DT_FLOAT CharsGetBound(DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_CHAR chars_arr[], DT_BMP* bmp) const { return dtxCharsGetBound(GetDtEngine(), crop_type, x, y, spacing, kern_mode, tm, chars_arr, bmp); }
	inline DT_FLOAT CharsDoOutput(DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[]) { return dtxCharsDoOutput(GetDtEngine(), x, y, spacing, kern_mode, tm, chars_arr); }
	inline DT_FLOAT CharsDoOutput(DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_CHAR chars_arr[]) { return dtxCharsDoOutput(GetDtEngine(), x, y, spacing, kern_mode, tm, chars_arr); }
	inline void CharsDoOutput_Arc(DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[]) { dtxCharsDoOutput_Arc(GetDtEngine(), xc, yc, rc, alpha0, delta, effects, spacing, kern_mode, tm, chars_arr); }
	inline void CharsDoOutput_Arc(DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L& effects, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[]) { dtxCharsDoOutput_Arc(GetDtEngine(), xc, yc, rc, alpha0, delta, &effects, spacing, kern_mode, tm, chars_arr); }
	inline void CharsDoOutput_Arc(DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_CHAR chars_arr[]) { dtxCharsDoOutput_Arc(GetDtEngine(), xc, yc, rc, alpha0, delta, effects, spacing, kern_mode, tm, chars_arr); }
	inline void CharsDoOutput_Arc(DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L& effects, DT_SLONG spacing, DT_ID_SWORD kern_mode, DT_TM2X2 tm, const DT_CHAR chars_arr[]) { dtxCharsDoOutput_Arc(GetDtEngine(), xc, yc, rc, alpha0, delta, &effects, spacing, kern_mode, tm, chars_arr); }

	/* Misc */
	inline static void TransformPerspective(const DT_TM3X3 m, DT_SLONG x, DT_SLONG y, DT_SLONG n, const DT_SLONG x_arr_in[], const DT_SLONG y_arr_in[], DT_FLOAT x_arr_out[], DT_FLOAT y_arr_out[]) { dtxTransformPerspective(m, x, y, n, x_arr_in, y_arr_in, x_arr_out, y_arr_out); }

	/* Common Shapes */

	inline DT_SWORD Shape_LineDashed(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsLineDashed(GetDtEngine(), refx, refy, x, y, dx, dy, ld, ld_shift, t, bgn, end, tm); }
	inline DT_SWORD Shape_Line(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsLine(GetDtEngine(), refx, refy, x, y, dx, dy, t, bgn, end, tm); }
	inline DT_SWORD Shape_BSplineCurveDashed(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsBSplineCurveDashed(GetDtEngine(), refx, refy, x, y, dx1, dy1, dx2, dy2, n, ld, ld_shift, t, bgn, end, tm); }
	inline DT_SWORD Shape_BSplineCurve(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsBSplineCurve(GetDtEngine(), refx, refy, x, y, dx1, dy1, dx2, dy2, n, t, bgn, end, tm); }
	inline DT_SWORD Shape_BezierCurveDashed(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_FLOAT dx3, DT_FLOAT dy3, DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsBezierCurveDashed(GetDtEngine(), refx, refy, x, y, dx1, dy1, dx2, dy2, dx3, dy3, n, ld, ld_shift, t, bgn, end, tm); }
	inline DT_SWORD Shape_BezierCurve(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_FLOAT dx3, DT_FLOAT dy3, DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsBezierCurve(GetDtEngine(), refx, refy, x, y, dx1, dy1, dx2, dy2, dx3, dy3, n, t, bgn, end, tm); }
	inline DT_SWORD Shape_ArcDashed(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsArcDashed(GetDtEngine(), refx, refy, x, y, rx, ry, alpha0, theta, ld, ld_shift, t, bgn, end, tm); }
	inline DT_SWORD Shape_Arc(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsArc(GetDtEngine(), refx, refy, x, y, rx, ry, alpha0, theta, t, bgn, end, tm); }
	inline static DT_UBYTE* Shape_SliceFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, const DT_TM2X2 tm, DT_RECT_SLONG* extent) { return dtsSliceFilled_Build(x, y, rx, ry, alpha0, theta, tm, extent); }
	inline DT_SWORD Shape_SliceFilled(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, const DT_TM2X2 tm = DV_NULL) { return dtsSliceFilled(GetDtEngine(), refx, refy, x, y, rx, ry, alpha0, theta, tm); }
	inline DT_SWORD Shape_SliceDashed(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm = DV_NULL) { return dtsSliceDashed(GetDtEngine(), refx, refy, x, y, rx, ry, alpha0, theta, ld, ld_shift, t, tm); }
	inline DT_SWORD Shape_Slice(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT t, const DT_TM2X2 tm = DV_NULL) { return dtsSlice(GetDtEngine(), refx, refy, x, y, rx, ry, alpha0, theta, t, tm); }
	inline static DT_UBYTE* Shape_TrueEllipseFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, const DT_TM2X2 tm, DT_RECT_SLONG* extent) { return dtsTrueEllipseFilled_Build(x, y, rx, ry, tm, extent); }
	inline DT_SWORD Shape_TrueEllipseFilled(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, const DT_TM2X2 tm = DV_NULL) { return dtsTrueEllipseFilled(GetDtEngine(), refx, refy, x, y, rx, ry, tm); }
	inline DT_SWORD Shape_TrueEllipse(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_FLOAT t, const DT_TM2X2 tm = DV_NULL) { return dtsTrueEllipse(GetDtEngine(), refx, refy, x, y, rx, ry, t, tm); }
	inline DT_SWORD Shape_TrueEllipseDashed(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm = DV_NULL) { return dtsTrueEllipseDashed(GetDtEngine(), refx, refy, x, y, rx, ry, ld, ld_shift, t, tm); }
	inline static DT_UBYTE* Shape_EllipseFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, const DT_TM2X2 tm, DT_RECT_SLONG* extent) { return dtsEllipseFilled_Build(x, y, w, h, tm, extent); }
	inline DT_SWORD Shape_EllipseFilled(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, const DT_TM2X2 tm = DV_NULL) { return dtsEllipseFilled(GetDtEngine(), refx, refy, x, y, w, h, tm); }
	inline DT_SWORD Shape_Ellipse(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT t, const DT_TM2X2 tm = DV_NULL) { return dtsEllipse(GetDtEngine(), refx, refy, x, y, w, h, t, tm); }
	inline DT_SWORD Shape_EllipseDashed(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm = DV_NULL) { return dtsEllipseDashed(GetDtEngine(), refx, refy, x, y, w, h, ld, ld_shift, t, tm); }
	inline static DT_UBYTE* Shape_RectangleFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, const DT_TM2X2 tm, DT_RECT_SLONG* extent) { return dtsRectangleFilled_Build(x, y, w, h, r, tm, extent); }
	inline DT_SWORD Shape_RectangleFilled(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, const DT_TM2X2 tm = DV_NULL) { return dtsRectangleFilled(GetDtEngine(), refx, refy, x, y, w, h, r, tm); }
	inline DT_SWORD Shape_Rectangle(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, DT_FLOAT t, const DT_TM2X2 tm = DV_NULL) { return dtsRectangle(GetDtEngine(), refx, refy, x, y, w, h, r, t, tm); }
	inline DT_SWORD Shape_RectangleDashed(DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm = DV_NULL) { return dtsRectangleDashed(GetDtEngine(), refx, refy, x, y, w, h, r, ld, ld_shift, t, tm); }
	inline static DT_UBYTE* Shape_PolyFilled_Build(const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, const DT_RECT_SLONG* bound_box, const DT_TM2X2 tm, DT_RECT_SLONG* extent) { return dtsPolyFilled_Build(ct, cx, cy, n, bound_box, tm, extent); }
	inline DT_SWORD Shape_PolyFilled(DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, const DT_RECT_SLONG* bound_box = DV_NULL, const DT_TM2X2 tm = DV_NULL) { return dtsPolyFilled(GetDtEngine(), refx, refy, ct, cx, cy, n, bound_box, tm); }
	inline DT_SWORD Shape_PolyDashed(DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsPolyDashed(GetDtEngine(), refx, refy, ct, cx, cy, n, ld, ld_shift, t, bgn, end, tm); }
	inline DT_SWORD Shape_Poly(DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm = DV_NULL) { return dtsPoly(GetDtEngine(), refx, refy, ct, cx, cy, n, t, bgn, end, tm); }
	inline static void Shape_Release(DT_UBYTE* buf) { dtsRelease(buf); }

private:

	DT_ENGINE Engine;
	DT_STREAM_DESC DefaultFontmap;
#if (DEF_ENGINE_LEVEL == 2)
	DT_LXCACHE LayoutCache;
#endif

private:

	DT_STYLE_EFFECTS StoredStyle;
	DT_TYPE_EFFECTS_L StoredType;

#ifdef DEF_ENGINE_BUILDER_CLIPS
	bool MakeClips();
	void DestroyClips();
	DT_PDDOC Clip[DEF_ENGINE_BUILDER_CLIPS];
#endif
};

#endif /* DEF_ENGINE_LEVEL */



#if (DEF_ENGINE_LEVEL == 1 || DEF_ENGINE_LEVEL == 2)
#else

#ifndef DTYPE_DOCCPP_H
#define DTYPE_DOCCPP_H


const DT_SLONG DV_PAGE_TEMP = 999997; /*-1*/
const DT_SLONG DV_PAGE_NEST = 999998; /*-2*/


class CDTAtom
{
public:

	CDTAtom() : NextAtom(DV_NULL) {}
	virtual ~CDTAtom() {}

public:

	CDTAtom* NextAtom;
};


class CDTDocV8 : public CDTAtom
{
private: // private copy constructor and assignment operator

	CDTDocV8(const CDTDocV8&);
	CDTDocV8& operator = (const CDTDocV8&);

public:

	CDTDocV8() { Reset(); }
	CDTDocV8(CDTEngineV8* engine) { Init(engine); }
	CDTDocV8(CDTEngineV8& engine) { Init(&engine); }
	virtual ~CDTDocV8() { Exit(); }

	bool Init(CDTEngineV8* engine = DV_NULL);
	bool Init(CDTEngineV8& engine) { return Init(&engine); }
	void Reset();
	void Exit();

	bool HasTextCompression() const;

public: // Public methods

	inline bool InitFailed()
	{
#if (DEF_ENGINE_LEVEL == 3)
		return (PdDoc == DV_NULL);
#else
		return (TxDoc == DV_NULL);
#endif
	}

	inline CDTEngineV8* GetEngine() const { return Engine; }
	inline DT_PDDOC GetPdDoc() const { return PdDoc; }
	inline DT_TXDOC GetTxDoc() const { return TxDoc; }

#ifdef DEF_ENGINE_BUILDER_CLIPS
	inline DT_PDDOC GetClip(DT_SWORD i) const { return Engine->GetClip(i); }
#endif

	inline void Set(CDTEngineV8* engine, DT_PDDOC doc, DT_TXDOC text_doc) { Engine = engine; PdDoc = doc; TxDoc = text_doc; }

	inline void SetInternalFlags(DT_UWORD flags) { pdDocSetInternalFlags(PdDoc, flags); }

	bool SetFontmap(const DT_STREAM_DESC* sd_fontmap);
	inline bool SetFontmap(const DT_STREAM_DESC& sd_fontmap) { return SetFontmap(&sd_fontmap); }
	inline bool SetFontmap(const DT_CHAR* fontmap) { DT_STREAM_FILE(sd_fontmap, fontmap); return SetFontmap(&sd_fontmap); }

	const DT_STREAM_DESC* GetFontmap() const
	{
		if (Fontmap.stream_id == DV_STREAM_MEMORY && Fontmap.stream_locator.mem_w.addr == DV_NULL) return Engine->GetDefaultFontmap();
		return &Fontmap;
	}
	const DT_STREAM_DESC& GetFontmapRef() const
	{
		if (Fontmap.stream_id == DV_STREAM_MEMORY && Fontmap.stream_locator.mem_w.addr == DV_NULL) return Engine->GetDefaultFontmapRef();
		return Fontmap;
	}

	/* Document */

	inline void Erase() { pdDocErase(PdDoc); }
	void Deselect();
	void Cleanup(DT_SLONG cleanup_maxpass = 50, DT_UWORD cleanup_flags = 3);

	DT_SLONG PageFindViaCharIndex(DT_SLONG img, DT_ID_SLONG obj_parent, DT_SLONG char_index);
	DT_SLONG PagesInsert(DT_SLONG start_page, DT_SLONG n = 1);

	DT_SLONG Save(DT_STREAM_DESC* sd, DT_SWORD save_mode = 0, DT_SLONG cleanup_maxpass = 50, DT_UWORD cleanup_flags = 3);
	inline DT_SLONG Save(DT_STREAM_DESC& sd, DT_SWORD save_mode = 0, DT_SLONG cleanup_maxpass = 50, DT_UWORD cleanup_flags = 3) { return Save(&sd, save_mode, cleanup_maxpass, cleanup_flags); }
	inline DT_SLONG Save(const DT_CHAR* filename, DT_SWORD save_mode = 0, DT_SLONG cleanup_maxpass = 50, DT_UWORD cleanup_flags = 3) { DT_STREAM_FILE(sd, filename); return Save(&sd, save_mode, cleanup_maxpass, cleanup_flags); }

	DT_SLONG Append(DT_PDDOC doc, DT_SLONG page = 0, DT_SLONG x = 0, DT_SLONG y = 0, bool deselect = false, bool select = false);
	DT_SLONG Append(const DT_STREAM_DESC* sd, DT_SLONG page = 0, DT_SLONG x = 0, DT_SLONG y = 0, bool group = false, bool deselect = false, bool select = false, DT_ID_SWORD flag = 2);
	inline DT_SLONG Append(const DT_STREAM_DESC& sd, DT_SLONG page = 0, DT_SLONG x = 0, DT_SLONG y = 0, bool group = false, bool deselect = false, bool select = false, DT_ID_SWORD flag = 2) { return Append(&sd, page, x, y, group, deselect, select, flag); }
	inline DT_SLONG Append(const DT_CHAR* filename, DT_SLONG page = 0, DT_SLONG x = 0, DT_SLONG y = 0, bool group = false, bool deselect = false, bool select = false, DT_ID_SWORD flag = 2) { DT_STREAM_FILE(sd, filename); return Append(&sd, page, x, y, group, deselect, select, flag); }
	//inline DT_SLONG AppendFromStream(const DT_STREAM_DESC* sd, DT_SLONG page, DT_SLONG x, DT_SLONG) { return pdDocAppendFromStream(sd, PdDoc, page, x, y); }

	inline DT_SLONG Transfer(const DT_PDDOC pd_doc, DT_SLONG page = 0, DT_SLONG x = 0, DT_SLONG y = 0, bool erase = false) { DT_SLONG ret = pdDocAppend(PdDoc, page, x, y, pd_doc); if (erase) pdDocErase(pd_doc); return ret; }
	DT_SLONG TransferWithCleanup(DT_PDDOC pd_doc, DT_UWORD cleanup_flags = 0, bool erase = false);

	inline DT_SLONG ObjGetLast() { return pdObjGetLast(PdDoc); }
	inline DT_SLONG ImgGetLast() { return pdImgGetLast(PdDoc); }

	inline DT_SWORD SetParams(const DT_PD_DOC_PARAMS* params, DT_SLONG reserved = 0) { return pdDocSetParams(PdDoc, params, reserved); }
	inline DT_SWORD SetParams(const DT_PD_DOC_PARAMS& params, DT_SLONG reserved = 0) { return pdDocSetParams(PdDoc, &params, reserved); }

	inline void SetNumericValue(DT_ID_SWORD numeric_id, DT_SLONG numeric_value) { pdDocSetNumericValue(PdDoc, numeric_id, numeric_value); }

	inline void SetScale(DT_FLOAT zoom) { pdDocSetScale(PdDoc, zoom); }
	inline void SetTransform(const DT_TM2X2 transform) { pdDocSetTransform(PdDoc, transform); }
	inline void SetTransform(const DT_TM3X3 transform) { pdDocSetTransformPlus(PdDoc, transform); }

	DT_SLONG Draw(DT_SLONG page, DT_SRAST_L x_off, DT_SRAST_L y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_MDC* memory_surface, DT_PD_DOCDRAW_PARAMS* params = DV_NULL, DT_ID_SWORD flags = 0); // { return pdDocDraw(PdDoc, page, x_off, y_off, format, subformat, memory_surface, params); }
	//inline DT_SLONG Draw(DT_SLONG page, DT_SRAST_L x_off, DT_SRAST_L y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_MDC* memory_surface, DT_PD_DOCDRAW_PARAMS* params, CDTEngineV8* engine) { return pdDocDraw2(PdDoc, page, x_off, y_off, format, subformat, memory_surface, params, engine->GetPdEngine()); }
	inline DT_SLONG Draw(DT_SLONG page, DT_SRAST_L x_off, DT_SRAST_L y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_MDC& memory_surface, DT_PD_DOCDRAW_PARAMS* params = DV_NULL, DT_ID_SWORD flags = 0) { return Draw(page, x_off, y_off, format, subformat, &memory_surface, params, flags); }

	inline void InvertLine(DT_SRAST_L x, DT_SRAST_L y, DT_SRAST_L w, DT_SRAST_L h, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface) { pdDocInvertLine(PdDoc, x, y, w, h, format, subformat, memory_surface); }
	inline void InvertBox(DT_SRAST_L x, DT_SRAST_L y, DT_SRAST_L w, DT_SRAST_L h, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface) { pdDocInvertBox(PdDoc, x, y, w, h, format, subformat, memory_surface); }
	inline void InvertQuad(DT_SRAST_L x1, DT_SRAST_L y1, DT_SRAST_L x2, DT_SRAST_L y2, DT_SRAST_L x3, DT_SRAST_L y3, DT_SRAST_L x4, DT_SRAST_L y4, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface) { pdDocInvertQuad(PdDoc, x1, y1, x2, y2, x3, y3, x4, y4, format, subformat, memory_surface); }
	inline void InvertFrames(DT_ID_SWORD frames_format, DT_PD_FRAMES frames, DT_SLONG frames_len, DT_UBYTE* frames_flags, DT_SLONG x_off, DT_SLONG y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, const DT_MDC* memory_surface) { pdDocInvertFrames(PdDoc, frames_format, frames, frames_len, frames_flags, x_off, y_off, format, subformat, memory_surface); }

	/* Objects */

	inline void ObjDelAll() { pdObjDelAll(PdDoc); }
	inline void ObjDel(DT_ID_SLONG obj) { pdObjDel(PdDoc, obj); }

	inline DT_ID_SLONG ObjAdd(DT_SLONG name = 0, const DT_CHAR* desc = DV_NULL, DT_SLONG size = 0) { return pdObjAddPlus(PdDoc, name, desc, size); }
	inline DT_ID_SLONG ObjDup(DT_ID_SLONG obj) { return pdObjDup(PdDoc, obj); }

	inline DT_SWORD ObjSet(DT_ID_SLONG obj, DT_SLONG name, const DT_CHAR* desc = DV_NULL) { return pdObjSet(PdDoc, obj, name, desc); }
	inline DT_SLONG ObjGet(DT_ID_SLONG obj, DT_SLONG* name, const DT_CHAR** desc) const { return pdObjGet(PdDoc, obj, name, desc); }

	inline DT_ID_SLONG ObjFindByNameAndDesc(DT_SLONG name, const DT_CHAR* desc = DV_NULL) { return pdObjFindByNameAndDesc(PdDoc, name, desc); }

	/* Fields */

	inline DT_SLONG NameGet(DT_ID_SLONG obj, DT_SLONG idx, DT_UBYTE* type) const { return pdNameGet(PdDoc, obj, idx, type); }

	/* Properties */

	inline DT_SWORD PropDel(DT_ID_SLONG obj, DT_SLONG name) { return pdPropDel(PdDoc, obj, name); }

	inline DT_SWORD PropAdd(DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len) { return pdPropAdd(PdDoc, obj, name, value, len); }
	inline DT_SWORD PropAdd(DT_ID_SLONG obj, DT_SLONG name, const DT_CHAR* value, DT_SLONG len) { return pdPropAdd(PdDoc, obj, name, reinterpret_cast<const DT_UBYTE*>(value), len); }
	inline DT_SWORD PropAddAsRef(DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG ptr, DT_SLONG target) { return pdPropAddAsRef(PdDoc, obj, name, ptr, target); }
	inline DT_SWORD PropAddAsExtBuffer(DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE* extbuffer, DT_SLONG extlen) { return pdPropAddAsExtBuffer(PdDoc, obj, name, extbuffer, extlen); }
	inline DT_SWORD PropAddAsUByte(DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE value) { return pdPropAddAsUByte(PdDoc, obj, name, value); }
	inline DT_SWORD PropAddAsSByte(DT_ID_SLONG obj, DT_SLONG name, DT_SBYTE value) { return pdPropAddAsSByte(PdDoc, obj, name, value); }
	inline DT_SWORD PropAddAsUWord(DT_ID_SLONG obj, DT_SLONG name, DT_UWORD value) { return pdPropAddAsUWord(PdDoc, obj, name, value); }
	inline DT_SWORD PropAddAsSWord(DT_ID_SLONG obj, DT_SLONG name, DT_SWORD value) { return pdPropAddAsSWord(PdDoc, obj, name, value); }
	inline DT_SWORD PropAddAsUTrio(DT_ID_SLONG obj, DT_SLONG name, DT_ULONG value) { return pdPropAddAsUTrio(PdDoc, obj, name, value); }
	inline DT_SWORD PropAddAsSTrio(DT_ID_SLONG obj, DT_SLONG name, DT_SLONG value) { return pdPropAddAsSTrio(PdDoc, obj, name, value); }
	inline DT_SWORD PropAddAsULong(DT_ID_SLONG obj, DT_SLONG name, DT_ULONG value) { return pdPropAddAsULong(PdDoc, obj, name, value); }
	inline DT_SWORD PropAddAsSLong(DT_ID_SLONG obj, DT_SLONG name, DT_SLONG value) { return pdPropAddAsSLong(PdDoc, obj, name, value); }

	inline DT_SWORD PropApply(DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len, DT_UBYTE apply_flags = PD_APPLY_STANDARD) { return pdPropApply(PdDoc, obj, name, value, len, apply_flags); }
	inline DT_SWORD PropApplyAsUByte(DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE value, DT_UBYTE default_value = 0, DT_UBYTE apply_flags = PD_APPLY_STANDARD) { return pdPropApplyAsUByte(PdDoc, obj, name, value, default_value, apply_flags); }
	inline DT_SWORD PropApplyAsSByte(DT_ID_SLONG obj, DT_SLONG name, DT_SBYTE value, DT_SBYTE default_value = 0, DT_UBYTE apply_flags = PD_APPLY_STANDARD) { return pdPropApplyAsSByte(PdDoc, obj, name, value, default_value, apply_flags); }
	inline DT_SWORD PropApplyAsUWord(DT_ID_SLONG obj, DT_SLONG name, DT_UWORD value, DT_UWORD default_value = 0, DT_UBYTE apply_flags = PD_APPLY_STANDARD) { return pdPropApplyAsUWord(PdDoc, obj, name, value, default_value, apply_flags); }
	inline DT_SWORD PropApplyAsSWord(DT_ID_SLONG obj, DT_SLONG name, DT_SWORD value, DT_SWORD default_value = 0, DT_UBYTE apply_flags = PD_APPLY_STANDARD) { return pdPropApplyAsSWord(PdDoc, obj, name, value, default_value, apply_flags); }
	inline DT_SWORD PropApplyAsULong(DT_ID_SLONG obj, DT_SLONG name, DT_ULONG value, DT_ULONG default_value = 0, DT_UBYTE apply_flags = PD_APPLY_STANDARD) { return pdPropApplyAsULong(PdDoc, obj, name, value, default_value, apply_flags); }
	inline DT_SWORD PropApplyAsSLong(DT_ID_SLONG obj, DT_SLONG name, DT_SLONG value, DT_SLONG default_value = 0, DT_UBYTE apply_flags = PD_APPLY_STANDARD) { return pdPropApplyAsSLong(PdDoc, obj, name, value, default_value, apply_flags); }

	inline DT_SWORD PropSet(DT_ID_SLONG obj, DT_SLONG name, const DT_UBYTE* value, DT_SLONG len) { return pdPropSet(PdDoc, obj, name, value, len); }
	inline DT_SWORD PropSetAsRef(DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG ptr, DT_SLONG target) { return pdPropSetAsRef(PdDoc, obj, name, ptr, target); }
	inline DT_SWORD PropSetAsExtBuffer(DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE* extbuffer, DT_SLONG extlen) { return pdPropSetAsExtBuffer(PdDoc, obj, name, extbuffer, extlen); }
	inline DT_UBYTE* PropGet(DT_ID_SLONG obj, DT_SLONG name, DT_SLONG* len) const { return pdPropGet(PdDoc, obj, name, len); }
	inline DT_ID_SLONG PropGetAsRef(DT_ID_SLONG obj, DT_SLONG name, DT_SLONG* target) const { return pdPropGetAsRef(PdDoc, obj, name, target); }

	inline DT_UBYTE PropGetAsUByte(DT_ID_SLONG obj, DT_SLONG name, DT_UBYTE default_value = 0) const { return pdPropGetAsUByte(PdDoc, obj, name, default_value); }
	inline DT_SBYTE PropGetAsSByte(DT_ID_SLONG obj, DT_SLONG name, DT_SBYTE default_value = 0) const { return pdPropGetAsSByte(PdDoc, obj, name, default_value); }
	inline DT_UWORD PropGetAsUWord(DT_ID_SLONG obj, DT_SLONG name, DT_UWORD default_value = 0) const { return pdPropGetAsUWord(PdDoc, obj, name, default_value); }
	inline DT_SWORD PropGetAsSWord(DT_ID_SLONG obj, DT_SLONG name, DT_SWORD default_value = 0) const { return pdPropGetAsSWord(PdDoc, obj, name, default_value); }
	inline DT_ULONG PropGetAsUTrio(DT_ID_SLONG obj, DT_SLONG name, DT_ULONG default_value = 0) const { return pdPropGetAsUTrio(PdDoc, obj, name, default_value); }
	inline DT_SLONG PropGetAsSTrio(DT_ID_SLONG obj, DT_SLONG name, DT_SLONG default_value = 0) const { return pdPropGetAsSTrio(PdDoc, obj, name, default_value); }
	inline DT_ULONG PropGetAsULong(DT_ID_SLONG obj, DT_SLONG name, DT_ULONG default_value = 0) const { return pdPropGetAsULong(PdDoc, obj, name, default_value); }
	inline DT_SLONG PropGetAsSLong(DT_ID_SLONG obj, DT_SLONG name, DT_SLONG default_value = 0) const { return pdPropGetAsSLong(PdDoc, obj, name, default_value); }

	inline DT_SWORD PropApplyAsRef(DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG ref_obj, DT_SLONG ref_name, DT_UBYTE apply_flags = PD_APPLY_STANDARD) { return pdPropApplyAsRef(PdDoc, obj, name, ref_obj, ref_name, apply_flags); }

	/* Links */

	inline DT_SWORD LinkDel(DT_ID_SLONG obj, DT_SLONG name) { return pdLinkDel(PdDoc, obj, name); }
	inline DT_SWORD LinkAdd(DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG link) { return pdLinkAdd(PdDoc, obj, name, link); }
	inline DT_SWORD LinkApply(DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG link, DT_UBYTE apply_flags) { return pdLinkApply(PdDoc, obj, name, link, apply_flags); }
	inline DT_SWORD LinkSet(DT_ID_SLONG obj, DT_SLONG name, DT_ID_SLONG link) { return pdLinkSet(PdDoc, obj, name, link); }
	inline DT_ID_SLONG LinkGet(DT_ID_SLONG obj, DT_SLONG name) const { return pdLinkGet(PdDoc, obj, name); }

	/* Images */

	inline void ImgDelAll() { pdImgDelAll(PdDoc); }
	inline void ImgDel(DT_SLONG img) { pdImgDel(PdDoc, img); }
	inline void ImgDelete(DT_UBYTE status) { pdImgDelete(PdDoc, status); }

	inline DT_SLONG ImgAdd(DT_ID_SLONG obj, DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_UBYTE status, DT_UBYTE attribs, DT_SLONG mask, DT_ID_SWORD fn_draw, const DT_TM3X3 transform = DV_NULL) { return pdImgAddPlus(PdDoc, obj, page, x, y, status, attribs, mask, fn_draw, transform); }
	inline DT_SLONG ImgDup(DT_SLONG img) { return pdImgDup(PdDoc, img); }

	inline void ImgSetX(DT_SLONG img, DT_SLONG x) { DT_SLONG t, y; pdImgGetXY(PdDoc, img, &t, &y); pdImgSetXY(PdDoc, img, x, y); }
	inline void ImgSetY(DT_SLONG img, DT_SLONG y) { DT_SLONG x, t; pdImgGetXY(PdDoc, img, &x, &t); pdImgSetXY(PdDoc, img, x, y); }
	inline void ImgSetXY(DT_SLONG img, DT_SLONG x, DT_SLONG y) { pdImgSetXY(PdDoc, img, x, y); }

	inline DT_SLONG ImgGetX(DT_SLONG img) const { DT_SLONG x, t; pdImgGetXY(PdDoc, img, &x, &t); return x; }
	inline DT_SLONG ImgGetY(DT_SLONG img) const { DT_SLONG t, y; pdImgGetXY(PdDoc, img, &t, &y); return y; }
	inline void ImgGetXY(DT_SLONG img, DT_SLONG* x, DT_SLONG* y) const { pdImgGetXY(PdDoc, img, x, y); }

	inline void ImgSetObj(DT_SLONG img, DT_ID_SLONG obj) { pdImgSetObj(PdDoc, img, obj); }
	inline DT_ID_SLONG ImgGetObj(DT_SLONG img) const { return pdImgGetObj(PdDoc, img); }

	inline void ImgSetFnDraw(DT_SLONG img, DT_ID_SWORD fn_draw) { pdImgSetFnDraw(PdDoc, img, fn_draw); }
	inline DT_ID_SWORD ImgGetFnDraw(DT_SLONG img) const { return pdImgGetFnDraw(PdDoc, img); }

	inline void ImgSetStatus(DT_SLONG img, DT_UBYTE status) { pdImgSetStatus(PdDoc, img, status); }
	inline DT_UBYTE ImgGetStatus(DT_SLONG img) const { return pdImgGetStatus(PdDoc, img); }

	inline void ImgSetAttribs(DT_SLONG img, DT_UBYTE attribs) { pdImgSetAttribs(PdDoc, img, attribs); }
	inline DT_UBYTE ImgGetAttribs(DT_SLONG img) const { return pdImgGetAttribs(PdDoc, img); }

	inline void ImgSetPage(DT_SLONG img, DT_SLONG page) { pdImgSetPage(PdDoc, img, page); }
	inline DT_SLONG ImgGetPage(DT_SLONG img) const { return pdImgGetPage(PdDoc, img); }

	inline void ImgSetMask(DT_SLONG img, DT_SLONG mask) { pdImgSetMask(PdDoc, img, mask); }
	inline DT_SLONG ImgGetMask(DT_SLONG img) const { return pdImgGetMask(PdDoc, img); }

	inline void ImgSetName(DT_SLONG img, DT_SWORD name) { pdImgSetName(PdDoc, img, name); }
	inline DT_SWORD ImgGetName(DT_SLONG img) const { return pdImgGetName(PdDoc, img); }

	inline void ImgSetTransform(DT_SLONG img, const DT_TM2X2 transform) { pdImgSetTransform(PdDoc, img, transform); }
	inline void ImgSetTransform(DT_SLONG img, const DT_TM3X3 transform) { pdImgSetTransformPlus(PdDoc, img, transform); }
	inline void ImgGetTransform(DT_SLONG img, DT_TM2X2 transform) const { pdImgGetTransform(PdDoc, img, transform); }
	inline void ImgGetTransform(DT_SLONG img, DT_TM3X3 transform) const { pdImgGetTransformPlus(PdDoc, img, transform); }

	inline DT_ID_SLONG ImgGetObjAndFnDraw(DT_SLONG img, DT_ID_SLONG obj_parent, DT_SWORD* fn_draw) const { return pdImgGetObjAndFnDraw(PdDoc, img, obj_parent, fn_draw); }

	inline void ImgToTop(DT_UBYTE status) { pdImgToTop(PdDoc, status); }
	inline void ImgToBottom(DT_UBYTE status) { pdImgToBottom(PdDoc, status); }

	inline void ImgMoveToPage(DT_UBYTE status, DT_SLONG page) { pdImgMoveToPage(PdDoc, status, page); }
	inline void ImgChangeAttribs(DT_UBYTE status, DT_UBYTE attribs) { pdImgChangeAttribs(PdDoc, status, attribs); }

public:

	bool AddAtom(CDTAtom* n) { if (n == DV_NULL) return false; CDTAtom* a = NextAtom; NextAtom = n; n->NextAtom = a; return true; }

private:

	void LogError(const DT_CHAR* /*msg*/) { /*printf("%s\n", msg);*/ }

private:

	CDTAtom* NextAtom;
	CDTEngineV8* Engine;
	DT_PDDOC PdDoc;
	DT_TXDOC TxDoc;
	DT_STREAM_DESC Fontmap;

public:

	bool DoTextReflow;

#if (DEF_ENGINE_LEVEL == 3)
#else
public:

	class CDTText
	{
	private: // private copy constructor and assignment operator

		CDTText(const CDTText&);
		CDTText& operator = (const CDTText&);

	protected:

		CDTText() : PdDoc(DV_NULL), TxDoc(DV_NULL), MustFree(0) {}

	public:

		/* inheritable (global) text doc (PdDoc is DV_NULL) */
		CDTText(CDTDocV8& doc) : PdDoc(DV_NULL), TxDoc(doc.GetTxDoc()), MustFree(0) {}
		CDTText(CDTDocV8* doc) : PdDoc(DV_NULL), TxDoc(doc->GetTxDoc()), MustFree(0) {}
		virtual ~CDTText() { if (MustFree && TxDoc != DV_NULL) txTextExt(TxDoc); }

		inline DT_TXDOC GetTxDoc() const { return TxDoc; }

	public:

		inline DT_SWORD SetAttribs(DT_SLONG first_char, DT_SLONG last_char, const DT_TX_ATTRIBS text_attribs[], DT_SWORD reserved = 0) { return txTextSetAttribs(TxDoc, first_char, last_char, text_attribs, reserved); }

		inline DT_SWORD SetParams(const DT_TX_DOC_PARAMS* params, DT_SLONG reserved = 0) { return txTextSetParams(TxDoc, params, reserved); }
		inline DT_SWORD SetParams(const DT_TX_DOC_PARAMS& params, DT_SLONG reserved = 0) { return txTextSetParams(TxDoc, &params, reserved); }

		inline void SetNumericValueTX(DT_ID_SWORD numeric_id, DT_SLONG numeric_value) { txTextSetNumericValue(TxDoc, numeric_id, numeric_value); }
		void SetNumericValuePD(DT_ID_SWORD numeric_id, DT_SLONG numeric_value) { DT_PDDOC power_doc = DV_NULL; txTextGetPowerDoc(TxDoc, &power_doc); if (power_doc != DV_NULL) pdDocSetNumericValue(power_doc, numeric_id, numeric_value); }

		inline DT_SWORD SetHighlighter(const DT_UBYTE cursor_blend_arr[], const DT_UBYTE select_blend_arr[], DT_SWORD reserved = 0) { return txTextSetHighlighter(TxDoc, cursor_blend_arr, select_blend_arr, reserved); }

		inline DT_SWORD SetFlow(DT_SLONG nr_of_areas, const DT_TX_TEXTFLOW_AREA areas[], const DT_SLONG pages[] = DV_NULL, DT_SWORD reserved = 0) { return txTextSetFlowPlus(TxDoc, nr_of_areas, areas, pages, reserved); }
		inline DT_SWORD AddFlow(DT_ID_SLONG obj, DT_SLONG nr_of_areas, const DT_TX_TEXTFLOW_AREA areas[], const DT_SLONG pages[] = DV_NULL, DT_SWORD reserved = 0) { return txTextAddFlowPlus(TxDoc, obj, nr_of_areas, areas, pages, reserved); }

		inline DT_SWORD SetSelect(const DT_SLONG first_char[], const DT_SLONG last_char[], DT_SLONG array_len = 1, DT_RECT_SLONG* extent = DV_NULL, DT_SLONG reserved = 0) { return txTextSetSelect(TxDoc, first_char, last_char, array_len, extent, reserved); }
		inline DT_SWORD SetSelect(DT_SLONG first_char, DT_SLONG last_char, DT_RECT_SLONG* extent = DV_NULL, DT_SLONG reserved = 0) { return txTextSetSelect(TxDoc, &first_char, &last_char, 1, extent, reserved); }
		inline DT_SWORD SetSelect(DT_SLONG first_char, DT_RECT_SLONG* extent = DV_NULL, DT_SLONG reserved = 0) { return txTextSetSelect(TxDoc, &first_char, &first_char, 1, extent, reserved); }

		inline DT_SLONG PasteViaPowerObj(DT_PDDOC power_doc, DT_SLONG obj, DT_SLONG paste_flags = 0) { return txTextPasteViaPowerObj(TxDoc, power_doc, obj, paste_flags); }

		inline DT_SLONG Save(DT_ID_UBYTE file_type, const DT_CHAR* file_name) { return txTextSaveToFile(TxDoc, file_type, file_name); }

	protected:

		DT_PDDOC PdDoc;
		DT_TXDOC TxDoc;
		DT_UBYTE MustFree;
	};

	class CDTTextStandalone : public CDTText
	{
	public:

		/* standalone */
		CDTTextStandalone() { txTextIni(&TxDoc); MustFree = 1; }
		virtual ~CDTTextStandalone() {}

	public:

		inline DT_ID_SLONG Make(CDTEngineV8* engine, DT_ID_UBYTE text_type, const DT_STREAM_DESC* text_sd, DT_SWORD unicode_flags = TX_IMPORT_FULL_COMPACT, const DT_TX_TEXTFLOW_AREA* area = DV_NULL, const DT_TX_ATTRIBS attribs[] = DV_NULL, const DT_STREAM_DESC* fontmap_sd = DV_NULL)
		{
			return txTextMakeViaStream(TxDoc, engine->GetTxEngine(), text_type, text_sd, unicode_flags, area, attribs, fontmap_sd);
		}

		inline DT_ID_SLONG Make(CDTEngineV8& engine, DT_ID_UBYTE text_type, const DT_STREAM_DESC& text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA& area, const DT_TX_ATTRIBS attribs[], const DT_STREAM_DESC& fontmap_sd) { return Make(&engine, text_type, &text_sd, unicode_flags, &area, attribs, &fontmap_sd); }

		inline DT_ID_SLONG Make(CDTEngineV8& engine, DT_ID_UBYTE text_type, const DT_STREAM_DESC& text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA& area, const DT_TX_ATTRIBS attribs[] = DV_NULL) { return Make(&engine, text_type, &text_sd, unicode_flags, &area, attribs, DV_NULL); }

		inline DT_ID_SLONG Make(CDTEngineV8& engine, DT_ID_UBYTE text_type, const DT_STREAM_DESC& text_sd, DT_SWORD unicode_flags = TX_IMPORT_FULL_COMPACT) { return Make(&engine, text_type, &text_sd, unicode_flags, DV_NULL, DV_NULL, DV_NULL); }
	};

	class CDTTextEmbeddable: public CDTText
	{
	public:

		/* embeddable (local) text doc */
		CDTTextEmbeddable(CDTDocV8& doc) { txTextIni(&TxDoc); PdDoc = doc.GetPdDoc(); MustFree = 1; }
		CDTTextEmbeddable(CDTDocV8* doc) { txTextIni(&TxDoc); PdDoc = doc->GetPdDoc(); MustFree = 1; }
		CDTTextEmbeddable(DT_PDDOC doc) { txTextIni(&TxDoc); PdDoc = doc; MustFree = 1; }
		virtual ~CDTTextEmbeddable() {}

	public:

		inline DT_ID_SLONG Make(DT_ID_UBYTE text_type, const DT_STREAM_DESC* text_sd, DT_SWORD unicode_flags = TX_IMPORT_FULL_COMPACT, const DT_TX_TEXTFLOW_AREA* area = DV_NULL, const DT_TX_DEFAULT_ATTRIBS* attribs = DV_NULL, const DT_STREAM_DESC* fontmap_sd = DV_NULL, DT_SWORD flags = 1)
		{
			//if (PdDoc == DV_NULL) return -1; else if (TxDoc != DV_NULL) { txTextExt(TxDoc); TxDoc = DV_NULL; }
			//return txTextIniViaPowerDocAndStream(&TxDoc, PdDoc, text_type, text_sd, unicode_flags, area, attribs, fontmap_sd, flags);
			return txTextMakeViaPowerDocAndStream(TxDoc, PdDoc, text_type, text_sd, unicode_flags, area, attribs, fontmap_sd, flags);
		}

		inline DT_ID_SLONG Make(DT_ID_UBYTE text_type, const DT_STREAM_DESC& text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA& area, const DT_TX_DEFAULT_ATTRIBS& attribs, const DT_STREAM_DESC& fontmap_sd, DT_SWORD flags = 1) { return Make(text_type, &text_sd, unicode_flags, &area, &attribs, &fontmap_sd, flags); }

		inline DT_ID_SLONG Make(DT_ID_UBYTE text_type, const DT_STREAM_DESC& text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA& area, const DT_STREAM_DESC& fontmap_sd, DT_SWORD flags = 1) { return Make(text_type, &text_sd, unicode_flags, &area, DV_NULL, &fontmap_sd, flags); }

		inline DT_ID_SLONG Make(DT_ID_UBYTE text_type, const DT_STREAM_DESC& text_sd, DT_SWORD unicode_flags, const DT_TX_TEXTFLOW_AREA& area, DT_SWORD flags = 1) { return Make(text_type, &text_sd, unicode_flags, &area, DV_NULL, DV_NULL, flags); }

		inline DT_ID_SLONG Make(DT_ID_UBYTE text_type, const DT_STREAM_DESC& text_sd, DT_SWORD unicode_flags = TX_IMPORT_FULL_COMPACT, DT_SWORD flags = 1) { return Make(text_type, &text_sd, unicode_flags, DV_NULL, DV_NULL, DV_NULL, flags); }
	};
#endif
};


class CDTField
{
public:

	union
	{
		const DT_UBYTE* ValueB;
		DT_SLONG ValueI;
	};

	DT_SLONG Name;
	DT_SLONG Name2;
	DT_SLONG Len;
	//DT_SWORD Flags;
};


class CDTObj
{
public:

	CDTObj() { PdDoc = DV_NULL; Obj = -1; }
	CDTObj(CDTDocV8& doc, const DT_CHAR* obj_desc = DV_NULL, DT_SLONG obj_name = 0, DT_SLONG obj_size = 0) : PdDoc(&doc) { Obj = pdObjAddPlus(doc.GetPdDoc(), obj_name, obj_desc, obj_size); }

	CDTObj& Del(DT_SLONG name)
	{
		pdPropDel(PdDoc->GetPdDoc(), Obj, name);
		return *this;
	}

	CDTObj& Apply(CDTField field)
	{
		if (field.Len == PD_LINK)
		{
			if (field.Name2 < 0) pdLinkApply(PdDoc->GetPdDoc(), Obj, field.Name, field.ValueI, PD_APPLY_STANDARD); else pdPropApplyAsRef(PdDoc->GetPdDoc(), Obj, field.Name, field.ValueI, field.Name2, PD_APPLY_STANDARD);
		}
		else if (field.Len == PD_OTHER) pdPropApplyAsSLong(PdDoc->GetPdDoc(), Obj, field.Name, field.ValueI, 0, PD_APPLY_STANDARD);
		else if (field.Len != 0)        pdPropApply(PdDoc->GetPdDoc(), Obj, field.Name, field.ValueB, field.Len, PD_APPLY_STANDARD);

		return *this;
	}

	inline DT_SLONG Pin(DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_ID_SWORD fn_draw, DT_UBYTE status = 0, DT_UBYTE attribs = 0, DT_SLONG mask = 0, const DT_TM3X3 transform = DV_NULL)
	{
		return pdImgAddPlus(PdDoc->GetPdDoc(), Obj, page, x, y, status, attribs, mask, fn_draw, transform);
	}

public:

	CDTDocV8* PdDoc;
	DT_ID_SLONG Obj;
};


class CDTLink : public CDTField
{
public:

	CDTLink(DT_SLONG name, CDTObj& obj) { Name = name; Name2 = -1; ValueI = obj.Obj; Len = PD_LINK; /*Flags = 0;*/ }
	CDTLink(DT_SLONG name, DT_ID_SLONG obj_id) { Name = name; Name2 = -1; ValueI = obj_id; Len = PD_LINK; /*Flags = 0;*/ }
};


class CDTProp : public CDTField
{
public:

	CDTProp(DT_SLONG name, const DT_CHAR* value, DT_SLONG len = PD_DEFAULT) { Name = name; Name2 = -1; ValueB = reinterpret_cast<const DT_UBYTE*>(value); Len = len; /*Flags = 0;*/ }
	CDTProp(DT_SLONG name, const DT_UBYTE* value, DT_SLONG len = PD_DEFAULT) { Name = name; Name2 = -1; ValueB = value; Len = len; /*Flags = 0;*/ }
	CDTProp(DT_SLONG name, CDTObj& obj) { Name = name; Name2 = -1; ValueI = obj.Obj; Len = PD_LINK; /*Flags = 0;*/ }
	//CDTProp& Flag(DT_SWORD flag) { Flags |= flag; return *this; }
};


class CDTPropRef : public CDTField
{
public:

	CDTPropRef(DT_SLONG name, CDTObj& obj, DT_SLONG name2) { Name = name; Name2 = name2; ValueI = obj.Obj; Len = PD_LINK; /*Flags = 0;*/ }
	CDTPropRef(DT_SLONG name, DT_ID_SLONG obj_id, DT_SLONG name2) { Name = name; Name2 = name2; ValueI = obj_id; Len = PD_LINK; /*Flags = 0;*/ }
};


class CDTPropInt : public CDTField
{
public:

	CDTPropInt(DT_SLONG name, DT_SLONG value_int) { Name = name; Name2 = -1; ValueI = value_int; Len = PD_OTHER; /*Flags = 0;*/ }
};


class CDTPropStr : public CDTField
{
public:

	CDTPropStr(DT_SLONG name, const DT_CHAR* value_str, DT_SLONG len = PD_DEFAULT) { Name = name; Name2 = -1; ValueB = reinterpret_cast<const DT_UBYTE*>(value_str); Len = len; /*Flags = 0;*/ }
};


inline CDTObj& operator + (CDTObj& obj, const CDTField& field) { return obj.Apply(field); }
inline CDTObj& operator - (CDTObj& obj, DT_SLONG name) { return obj.Del(name); }
//inline CDTProp operator ! (CDTProp prop) { return prop.Flag(1); }


#endif /* DTYPE_DOCCPP_H */

#endif /* DEF_ENGINE_LEVEL */

typedef const DT_CHAR* DT_STR_UTF8;

#endif /* DTYPE_ENGINECPP_H */
