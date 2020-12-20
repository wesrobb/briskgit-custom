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

// Header for this file
#include "fontmap.h"


DT_ID_SWORD CDTFontMap::LoadSingleFont(DT_ID_UBYTE hinting, const DT_CHAR* file_name)
{
	DT_STREAM_FILE(sd, file_name);
	DT_ID_SWORD font_index = Engine.FontAddViaStream(DV_FONT_FORMAT_AUTO1, DV_NULL, 0, -1, 128, hinting, sd);
	if (font_index >= 0) std::cout << "Font #" << font_index << " loaded.\n"; else std::cout << "ERROR: Required font " << file_name << " could not be loaded!\n";
	return font_index;
}


DT_SWORD CDTFontMap::LoadRequiredFonts()
{
	DT_SWORD success = 1;

	success &= ((Font_CMUSS = LoadSingleFont(2, "../../fonts/format/otf/cmunss.otf")) >= 0);
	success &= ((Font_BCMPS = LoadSingleFont(1, "../../fonts/script/arab/BCompset.ttf")) >= 0);
	success &= ((Font_THRAB = LoadSingleFont(1, "../../fonts/script/deva/tr.ttf")) >= 0);
	success &= ((Font_UMING = LoadSingleFont(1, "../../fonts/script/hani/uming.ttf")) >= 0);
	success &= ((Font_NORAS = LoadSingleFont(1, "../../fonts/script/thai/Norasi.ttf")) >= 0);
	success &= ((Font_DJVUS = LoadSingleFont(1, "../../fonts/script/DejaVuSans.ttf")) >= 0);

	if (success) std::cout << "All required fonts have been successfully loaded.\n"; else std::cout << "Not all required fonts could be loaded.\n";

	std::cout << "Font_CMUSS=" << Font_CMUSS;
	std::cout << "Font_BCMPS=" << Font_BCMPS;
	std::cout << "Font_THRAB=" << Font_THRAB;
	std::cout << "Font_UMING=" << Font_UMING;
	std::cout << "Font_NORAS=" << Font_NORAS;
	std::cout << "Font_DJVUS=" << Font_DJVUS;

	return success;
}


void CDTFontMap::UnloadSingleFont(DT_ID_SWORD font_index)
{
	if (font_index < 0) return;
	Engine.FontRemove(font_index);
	std::cout << "Font #" << font_index << " unloaded.\n";
}


void CDTFontMap::UnloadRequiredFonts()
{
	UnloadSingleFont(Font_CMUSS); Font_CMUSS = -1;
	UnloadSingleFont(Font_BCMPS); Font_BCMPS = -1;
	UnloadSingleFont(Font_THRAB); Font_THRAB = -1;
	UnloadSingleFont(Font_UMING); Font_UMING = -1;
	UnloadSingleFont(Font_NORAS); Font_NORAS = -1;
	UnloadSingleFont(Font_DJVUS); Font_DJVUS = -1;
}


DT_ID_SWORD CDTFontMap::GetFontForUnicodeScript(DT_ID_SWORD script_code) const
{
	switch (script_code)
	{
	case zzzzScriptCode: return Font_CMUSS;
	case zyyyScriptCode: return Font_CMUSS;
	case latnScriptCode: return Font_CMUSS;
	case cyrlScriptCode: return Font_CMUSS;
	case grekScriptCode: return Font_CMUSS;
	case haniScriptCode: return Font_UMING;
	case hiraScriptCode: return Font_UMING;
	case kanaScriptCode: return Font_UMING;
	case hebrScriptCode: return Font_DJVUS;
	case arabScriptCode: return Font_BCMPS;
	case devaScriptCode: return Font_THRAB;
	case thaiScriptCode: return Font_NORAS;
	}

	return Font_CMUSS; /* any other script */
}
