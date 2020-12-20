/*
 * engine.h - D-Type Engine
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

// Main D-Type header file
#include "../core/dtype.h"


//#define DEF_ENGINE_INIT_KEY xxxxxxxxx
//#define DEF_ENGINE_INIT_FLAG yyyyy


#ifndef DEF_ENGINE_LEVEL
#error "dtengine.cpp: DEF_ENGINE_LEVEL not defined! It must be 0, 1, 2, 3 or 4."
#elif (DEF_ENGINE_LEVEL == 0)
#else

// Header for this file
#include "dtengine.h"

#include "../extras/dtextras.cpp"
#include "../extras/dtshapes.cpp"

#ifdef DEF_ENGINE_BUILDER_CLIPS
#include "dtbuilder/dtengine_clips.cpp"
#endif


void CDTEngineV8::Reset()
{
	Engine = DV_NULL;

	StoredStyle.effects_len = 0;
	for (DT_SLONG i = 0; i < 64; i++) StoredStyle.effects_arr[i] = 0;
	StoredStyle.rgbt[0] = StoredStyle.rgbt[1] = StoredStyle.rgbt[2] = StoredStyle.rgbt[3] = 0;
	StoredStyle.palette = DV_NULL;

	StoredType.font_index = 0;
	StoredType.reserved = 0;
	StoredType.descriptor = 0;
	StoredType.transform.params.size_h = StoredType.transform.params.size_v = 100;
	StoredType.transform.params.skew_h = StoredType.transform.params.skew_v = 0;
	StoredType.transform.params.rotation = 0;
	StoredType.linedecor.thickness = 0;
	StoredType.linedecor.segment = 0;
	StoredType.linedecor.shift = 0;
	StoredType.linedecor.dash_size = 0;
	StoredType.linedecor.flags = 0;
	StoredType.linedecor.scale_id = DV_SCALE_100;

	DT_STREAM_MEMORY(fontmap_sd_cached, DV_NULL, 0);
	DefaultFontmap = fontmap_sd_cached; /* OK - copy by value */
}


bool CDTEngineV8::Init(const DT_STREAM_DESC* sd_init, const DT_STREAM_DESC* sd_fontmap, DT_INIT_INFO* init_info)
{
	Reset();

#ifdef DEF_ENGINE_BUILDER_CLIPS
	DT_SLONG i;
	for (i = 0; i < DEF_ENGINE_BUILDER_CLIPS; i++) Clip[i] = DV_NULL;
#endif

#ifdef DEF_ENGINE_INIT_KEY

	DT_INIT_INFO init_info_default = {DEF_ENGINE_INIT_KEY, DEF_ENGINE_INIT_FLAG, 0, 0, 0, DV_NULL, DV_NULL};
	if (init_info == DV_NULL) init_info = &init_info_default; else { init_info->init_key = DEF_ENGINE_INIT_KEY; init_info->init_flag = DEF_ENGINE_INIT_FLAG; }

#endif

	if (sd_init == DV_NULL) return false;

#if (DEF_ENGINE_LEVEL == 1)

	if (dtEngineIniViaStream(&Engine, sd_init, init_info) != 1) return false;
	(void)sd_fontmap;

#elif (DEF_ENGINE_LEVEL == 2)

	if (dtEngineIniViaStream(&Engine, sd_init, init_info) != 1) return false;
	if (lxCacheIni(&LayoutCache, Engine, 0) != 1) { dtEngineExt(Engine); return false; }
	(void)sd_fontmap;

#elif (DEF_ENGINE_LEVEL == 3)

	if (pdEngineIniViaStream(&Engine, sd_init, init_info) != 1) return false;
	(void)sd_fontmap;

#else

	if (txEngineIniViaStream(&Engine, sd_init, init_info) != 1) return false;
	if (sd_fontmap == DV_NULL) goto end;
	if (txEngineMakeCachedFontmap(Engine, sd_fontmap, &DefaultFontmap, 0) != 1) { Exit(); return false; }

end:

#endif

#ifdef DEF_ENGINE_BUILDER_CLIPS
	if (!MakeClips()) { DestroyClips(); return false; }
#endif

	return true;
}


void CDTEngineV8::Exit()
{
#ifdef DEF_ENGINE_BUILDER_CLIPS
	DestroyClips();
#endif

#if (DEF_ENGINE_LEVEL == 1)

	if (Engine != DV_NULL) dtEngineExt(Engine);

#elif (DEF_ENGINE_LEVEL == 2)

	if (LayoutCache != DV_NULL) lxCacheExt(LayoutCache);
	LayoutCache = DV_NULL;

	if (Engine != DV_NULL) dtEngineExt(Engine);

#elif (DEF_ENGINE_LEVEL == 3)

	if (Engine != DV_NULL) pdEngineExt(Engine);

#else

	if (Engine != DV_NULL) txEngineExt(Engine);
	// release memory that txEngineMakeCachedFontmap allocated for the cached font map
	if (DefaultFontmap.stream_id == DV_STREAM_MEMORY && DefaultFontmap.stream_locator.mem_w.addr != DV_NULL) txFree(DefaultFontmap.stream_locator.mem_w.addr);
	DefaultFontmap.stream_locator.mem_w.addr = DV_NULL;

#endif

	DT_STREAM_MEMORY(fontmap_sd_cached, DV_NULL, 0);
	DefaultFontmap = fontmap_sd_cached; /* OK - copy by value */

	Engine = DV_NULL;
}


#if (DEF_ENGINE_LEVEL == 1 || DEF_ENGINE_LEVEL == 2)
#else

//#define DEF_PDX_ZLIB
//#define DEF_PDX_LIBPNG
//#define DEF_PDX_LIBJPG

#include "../extras/pdextras.cpp"


#ifdef DEF_ENGINE_RENAME_FONTS
#include "dtbuilder/dtengine_rename_fonts.cpp"
#endif


void CDTDocV8::Reset()
{
	NextAtom = DV_NULL;
	Engine = DV_NULL;
	PdDoc = DV_NULL;
	TxDoc = DV_NULL;

	DT_STREAM_MEMORY(fontmap_sd_cached, DV_NULL, 0);
	Fontmap = fontmap_sd_cached; /* OK - copy by value */

	DoTextReflow = true;
}


bool CDTDocV8::HasTextCompression() const
{
	DT_UBYTE internals[4] = {0 /* EnableFormatDescriptions */, 0 /* EnableTextDescriptions */, 0 /* EnableTextOptimization */, 0 /* EnableTextCompression */};

#if (DEF_ENGINE_LEVEL == 3)
#else
	txTextGetInternals(TxDoc, internals, 0);
#endif

	return (internals[3] == 1);
}


bool CDTDocV8::Init(CDTEngineV8* engine)
{
	Reset();

	Engine = engine;

	if (pdDocIni(&PdDoc, (Engine == DV_NULL ? DV_NULL : Engine->GetPdEngine())) != 1) return false;
	if (PdDoc == DV_NULL) return false;

	pdDocSetNumericValue(PdDoc, PD_NVAL_TEXT_BLANKS_SIZE, 512);

#if (DEF_ENGINE_LEVEL == 3)
#else
	//txTextIniViaPowerDocAndBuffer(&TxDoc, PdDoc, 0, DV_NULL, 0, 0 /*TX_IMPORT_POWERDOC_REFORMAT*/, DV_NULL, DV_NULL, Engine->GetDefaultFontmap(), 0);
	if (txTextIniViaPowerDoc(&TxDoc, PdDoc) < 0) return false; /* same as txTextIniViaPowerDocAndBuffer(&TxDoc, PdDoc, 0, DV_NULL, 0, 0, DV_NULL, DV_NULL, DV_NULL, 0); */
	if (TxDoc == DV_NULL) return false;

	pdDocSetNumericValue(PdDoc, PD_NVAL_CONTROL_GLYPHS, 1);
	pdDocSetNumericValue(PdDoc, PD_NVAL_CONTROL_ALGORITHMS, 2 | 8);

	txTextSetNumericValue(TxDoc, TX_NVAL_ENABLE_BITS, 15);
	txTextSetNumericValue(TxDoc, TX_NVAL_CONFIG_BITS, 1 | 16 | 256 | 512);

	//DT_PD_DOC_PARAMS pd_params = {4 /*runtime->ImageOriginsNormalSize*/, 6 /*runtime->ImageOriginsSelectedSize*/, 10, 10, 2, 0, 1, 2 | 8};
	//DT_TX_DOC_PARAMS tx_params = {15, 1 | 16 | 256 | 512, &pd_params};
	//txTextSetParams(TxDoc, &tx_params, 0);

	DT_UBYTE internals[4] = {0 /* EnableFormatDescriptions */, 0 /* EnableTextDescriptions */, 0 /* EnableTextOptimization */, 0 /* EnableTextCompression */};
	txTextSetInternals(TxDoc, internals, 0);

	txTextCommand(TxDoc, TX_CMD_RESET, DV_NULL, 0);
#endif

	//BlendBlanks[0] = TX_BLEND_HIGHLIGHT_1; BlendBlanks[1] = 235; BlendBlanks[2] = 235; BlendBlanks[3] = 235; BlendBlanks[4] = 0;
	//txTextSetHighlighterPlus(TxDoc, extra->BlendCursor, extra->BlendSelect, extra->BlendBlanks, 0);

	return true;
}


void CDTDocV8::Exit()
{
	CDTAtom* a = NextAtom; while (a != DV_NULL) { CDTAtom* n = a->NextAtom; delete(a); a = n; /*printf("delete atom\n");*/ } /* delete all Atoms */

#if (DEF_ENGINE_LEVEL == 3)
#else

	if (TxDoc != DV_NULL) txTextExt(TxDoc);
	TxDoc = DV_NULL;

	if (Fontmap.stream_id == DV_STREAM_MEMORY && Fontmap.stream_locator.mem_w.addr != DV_NULL) txFree(Fontmap.stream_locator.mem_w.addr);
	Fontmap.stream_locator.mem_w.addr = DV_NULL;

#endif

	if (PdDoc != DV_NULL) pdDocExt(PdDoc);
	PdDoc = DV_NULL;

	DT_STREAM_MEMORY(fontmap_sd_cached, DV_NULL, 0);
	Fontmap = fontmap_sd_cached; /* OK - copy by value */
}


bool CDTDocV8::SetFontmap(const DT_STREAM_DESC* sd_fontmap)
{
#if (DEF_ENGINE_LEVEL == 3)
	(void)sd_fontmap;
	return false;
#else

	if (Fontmap.stream_id == DV_STREAM_MEMORY && Fontmap.stream_locator.mem_w.addr != DV_NULL) txFree(Fontmap.stream_locator.mem_w.addr);
	Fontmap.stream_locator.mem_w.addr = DV_NULL;

	DT_STREAM_MEMORY(fontmap_sd_cached, DV_NULL, 0);
	Fontmap = fontmap_sd_cached; /* OK - copy by value */

	if (sd_fontmap == DV_NULL) return true;
	if (txEngineMakeCachedFontmap(Engine->GetTxEngine(), sd_fontmap, &Fontmap, 0) != 1) return false;
	return true;

#endif
}


DT_SLONG CDTDocV8::Draw(DT_SLONG page, DT_SLONG x_off, DT_SLONG y_off, DT_ID_SWORD format, DT_ID_SWORD subformat, DT_MDC* memory_surface, DT_PD_DOCDRAW_PARAMS* params, DT_ID_SWORD flags)
{
#if (DEF_ENGINE_LEVEL == 3)
	(void)flags;
	return pdDocDraw(PdDoc, page, x_off, y_off, format, subformat, memory_surface, params);
#else
	return txTextDraw(TxDoc, page, x_off, y_off, format, subformat, memory_surface, params, flags);
#endif
}


void CDTDocV8::Deselect()
{
	pdxImgAllDeselect(PdDoc);
}


DT_SLONG CDTDocV8::PageFindViaCharIndex(DT_SLONG img, DT_ID_SLONG obj_parent, DT_SLONG char_index)
{
	/* Utility function to find a page on which a character from a text flow is */

#if (DEF_ENGINE_LEVEL == 3)
	(void)img;
	(void)obj_parent;
	(void)char_index;
#else
	DT_SWORD fn_draw;
	DT_SLONG page;
	DT_SLONG char_min, char_max;
	DT_SLONG char_prev = 0, char_next = 0;
	DT_ID_SLONG obj = txImgGetRootRichTextArea(PdDoc, img, obj_parent, &fn_draw); if (obj < 0) return -1;

	if (fn_draw == pdDrawRichTextArea || fn_draw == pdDrawRichTextLine || fn_draw == pdDrawRichTextArc || fn_draw == pdDrawRichTextPath) {} else return -1;

	while (1)
	{
		img = pdxImgFindViaObj(PdDoc, obj); if (img < 0) return -1;

		page = pdImgGetPage(PdDoc, img); if (page < 0) return -1;
		fn_draw = pdImgGetFnDraw(PdDoc, img); if (fn_draw == pdDrawRichTextArea || fn_draw == pdDrawRichTextLine || fn_draw == pdDrawRichTextArc || fn_draw == pdDrawRichTextPath) {} else return -1;

		char_min = pdPropGetAsSLong(PdDoc, obj, txStatsMinCharIndex, 0);
		char_max = pdPropGetAsSLong(PdDoc, obj, txStatsMaxCharIndex, 0);
		char_next = char_prev + char_max - char_min;

		if (char_index >= char_prev && char_index <= char_next) return page;

		char_prev = char_next;
		obj = pdLinkGet(PdDoc, obj, txRichTextNextPtr); if (obj < 0) return -1;
	}
#endif

	return -1;
}


DT_SLONG CDTDocV8::PagesInsert(DT_SLONG start_page, DT_SLONG n /* = 1 */)
{
	if (start_page < 0 || n < 1) return 0;
	pdxImgAllDeleteOnPageRange(PdDoc, DV_PAGE_TEMP, DV_PAGE_NEST);
	pdxPagesMove(PdDoc, start_page, n);
	return n;
}


void CDTDocV8::Cleanup(DT_SLONG cleanup_maxpass /* = 50 */, DT_UWORD cleanup_flags /* = 3 */)
{
	if (cleanup_maxpass < 0) return;

	pdxImgAllDeleteOnPageRange(PdDoc, DV_PAGE_TEMP, DV_PAGE_NEST); /* always needed */
#ifdef DEF_ENGINE_RENAME_FONTS
	if (cleanup_flags & 32768) pdxDocRenameFonts(PdDoc);
#endif
	if (cleanup_flags & 4) pdxDocDiscardDescriptions(PdDoc, "!"); /* no longer critical since Text Engine 7.0.1.2 uses pdObjFindByNameAndDescPlus */
	if (cleanup_flags & 1) pdxDocReconnectFonts(PdDoc);
	if (cleanup_flags & 2) pdxDocReconnectExternal(PdDoc);

	DT_SLONG i = 0;

	while (i < cleanup_maxpass)
	{
		DT_SLONG n1 = ObjGetLast(); pdDocDiscardOrphansPlus(PdDoc, 0, "!");
		DT_SLONG n2 = ObjGetLast(); if (n1 == n2) break;
		i++;
	}
}


DT_SLONG CDTDocV8::TransferWithCleanup(DT_PDDOC pd_doc, DT_UWORD cleanup_flags /* = 0 */, bool erase /* = false */)
{
	pdxImgAllDeleteOnPageRange(PdDoc, DV_PAGE_TEMP, DV_PAGE_NEST); /* always needed */
	pdxImgAllDeleteOnPageRange(pd_doc, DV_PAGE_TEMP, DV_PAGE_NEST); /* always needed */
	if (cleanup_flags & 4) pdxDocDiscardDescriptions(pd_doc, "!"); /* no longer critical since Text Engine 7.0.1.2 uses pdObjFindByNameAndDescPlus */
	if (cleanup_flags & 1) pdxDocReconnectFonts(pd_doc);
	if (cleanup_flags & 2) pdxDocReconnectExternal(pd_doc);

	DT_SLONG ref_page = 0;
	DT_SWORD copy_all = 1;
	DT_SLONG ret = pdDocCopySelectedPlus(pd_doc, PdDoc, ref_page, copy_all, 0, "!");
	if (erase) pdDocErase(pd_doc);
	return ret;
}


DT_SLONG CDTDocV8::Save(DT_STREAM_DESC* sd, DT_SWORD save_mode /* = 0 */, DT_SLONG cleanup_maxpass /* = 50 */, DT_UWORD cleanup_flags /* = 3 */)
{
	//printf("^^^ SAVE USING %d\n", cleanup_flags);
	Cleanup(cleanup_maxpass, cleanup_flags);

	DT_SLONG res = pdxDocSaveToStream(PdDoc, save_mode, sd);

	     if (res == -3) LogError("Compression error!\n");
	else if (res == -2) LogError("Not enough memory to save compressed document!\n");
	else if (res !=  1) LogError("Error saving document!\n");

	return res;
}


DT_SLONG CDTDocV8::Append(DT_PDDOC doc, DT_SLONG page /* = 0 */, DT_SLONG x /* = 0 */, DT_SLONG y /* = 0 */, bool deselect /* = false */, bool select /* = false */)
{
	DT_SLONG img_last = pdImgGetLast(PdDoc);
	if (deselect) pdxImgAllDeselect(PdDoc);
	DT_SLONG ret = pdDocAppend(PdDoc, page, x, y, doc);
	if (select) pdxImgSelSelectByImgRange(PdDoc, img_last + 1, ImgGetLast());
	return ret;
}


DT_SLONG CDTDocV8::Append(const DT_STREAM_DESC* sd, DT_SLONG page /* = 0 */, DT_SLONG x /* = 0 */, DT_SLONG y /* = 0 */, bool group /* = false */, bool deselect /* = false */, bool select /* = false */, DT_ID_SWORD flag /* = 2 */)
{
	DT_PDDOC power_doc_temp = DV_NULL;
	if (pdDocIni(&power_doc_temp, Engine->GetPdEngine()) == 0) return -1;

	DT_SLONG img_last = pdImgGetLast(PdDoc);
	DT_SLONG ret = 0;

	ret = pdxDocAppendFromStream(power_doc_temp, flag, 0, 0, 0, sd);

	     if (ret == -3) { LogError("Decompression error!\n"); goto end; }
	else if (ret == -2) { LogError("Not enough memory to open compressed document!\n"); goto end; }
	else if (ret !=  1) { LogError("Error opening PowerDoc document or the file does not exist!\n"); goto end; }

	if (group)
	{
		pdxImgSelSelectByPageRange(power_doc_temp, 0, 0);
		pdxImgSelGroup(power_doc_temp, 0, 0, 0, 1);
	}

	if (deselect) pdxImgAllDeselect(PdDoc);

	pdDocAppend(PdDoc, page, x, y, power_doc_temp);

	if (select) pdxImgSelSelectByImgRange(PdDoc, img_last + 1, ImgGetLast());

end:

	pdDocExt(power_doc_temp);
	return ret;
}


#endif

#endif
