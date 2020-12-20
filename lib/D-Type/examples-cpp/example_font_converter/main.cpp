/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <iostream>
#include <cstdio>

// D-Type header file (level 1)
#include "dtype_l1.h"
// D-Type's extra system utility functions
#include "dtxsystem.h"

// D-Type CPP Engine
#include "dtengine.cpp"


DT_SWORD list_fonts_in_catalog(CDTEngineV8& engine)
{
	DT_CHAR fuid[16];
	DT_CHAR font_name[256];
	DT_SWORD font_format_id;
	DT_SWORD fcnr, cmapnr;
	DT_SWORD i, nr_of_font_slots = engine.FontGetCounter();
	DT_UBYTE caching, hinting, activation_level;

	std::cout << "------------------------------------------------------------------\n";
	std::cout << " The following fonts are currently in D-Type's Font Catalog\n";
	std::cout << "------------------------------------------------------------------\n";

	for (i = 0; i < nr_of_font_slots; i++)
	{
		if (engine.FontGetSysStatus(i, &font_format_id, fuid, &fcnr, &cmapnr, &caching, &hinting, &activation_level) != 0) continue; /* Skip font instances or removed fonts */

		engine.FontGetStringValue(i, DV_SVAL_ASC_FONTNAME, reinterpret_cast<DT_UBYTE*>(font_name), 255, true);

		std::cout << " " << i << " = "; if (font_name[0] == 0) std::cout << "Untitled"; else std::cout << font_name;
		std::cout << " (" << font_format_id << ")\n";

		/* engine.FontMakeInactive(i, 0); */
	}

	std::cout << " " << i << " = *** ADD NEW FONT TO D-TYPE'S FONT CATALOG ***\n";
	std::cout << "------------------------------------------------------------------\n";

	return i;
}


DT_ID_SWORD add_new_font_to_catalog(CDTEngineV8& engine)
{
	DT_CHAR dummy[1024];
	DT_CHAR file_name[1024];

	std::cout << "\n";
	std::cout << "Many existing fonts are provided in the ../../fonts/ folder. Here are just\n";
	std::cout << "a few of them to try:\n";
	std::cout << "\n";
	std::cout << "  ../../fonts/format/ttf/Denmark.ttf\n";
	std::cout << "  ../../fonts/format/otf/cmunbsr.otf\n";
	std::cout << "  ../../fonts/format/pfb/Zombie.pfb\n";
	std::cout << "  ../../fonts/format/ps/Moto.ps\n";
	std::cout << "  ../../fonts/format/cff/ESLGothic.cff\n";
	std::cout << "\n";
	std::cout << "Enter the path of the font file on disk: ";

	if (std::scanf("%s", file_name) != 1) return -1;

	DT_STREAM_FILE(sd_check, file_name); DT_DTSTREAM f = dtStreamOpen(&sd_check, DV_STREAM_OPEN_MUST);
	if (f == DV_NULL) { std::cout << "File Not Found!\n\n"; return -1; }
	dtStreamClose(f, DV_STREAM_CLOSE_YES);

	std::cout << "\n";

	std::cout << "Specify source format id:\n\n";
	std::cout << " -1 = Detect automatically\n\n";
	std::cout << "  1 = TrueType or OpenType with TrueType outlines (.ttf) or TrueType Collection\n      (.ttc) [Apple Mac ASCII Encoding]\n";
	std::cout << "  3 = TrueType or OpenType with TrueType outlines (.ttf) or TrueType Collection\n      (.ttc) [Windows Unicode Encoding]\n\n";
	std::cout << "  6 = OpenType (.otf) with Type 2/CFF outlines [Apple Mac ASCII Encoding]\n";
	std::cout << "  5 = OpenType (.otf) with Type 2/CFF outlines [Windows Unicode Encoding]\n\n";
	std::cout << "  2 = Adobe Type 1 (.pfb or .pfa) [Adobe Standard Encoding]\n";
	std::cout << "  7 = Adobe Type 1 (.pfb or .pfa) [ISO Latin Encoding]\n";
	std::cout << "  8 = Adobe Type 1 (.pfb or .pfa) [Synthesized Windows Unicode Encoding]\n\n";
	std::cout << " 11 = Type 3 PostScript (.ps) [ISO Latin Encoding]\n";
	std::cout << " 12 = Type 3 PostScript (.ps) [Synthesized Windows Unicode Encoding]\n\n";
	std::cout << " 14 = Bare CFF (.cff) [Adobe Standard Encoding]\n";
	std::cout << " 15 = Bare CFF (.cff) [ISO Latin Encoding]\n";
	std::cout << " 16 = Bare CFF (.cff) [Synthesized Windows Unicode Encoding]\n\n";
	std::cout << "Enter your choice [-1, 1, 3, 6, 5, 2, 7, 8, 11, 12, 14, 15 or 16]: ";

	if (std::scanf("%s", dummy) != 1) return -1;

	DT_ID_SWORD font_format_id = static_cast<DT_ID_SWORD>(std::atoi(dummy));

	std::cout << "\n";

	std::cout << "If this file is a font collection (e.g. a TrueType .ttc font), enter \nthe collection number of the font to be converted. If this file is not \na collection, or to convert the first font in the collection, enter 0.\nYour choice [0-99]: ";

	if (std::scanf("%s", dummy) != 1) return -1;

	DT_SWORD fcnr = static_cast<DT_SWORD>(std::atoi(dummy));

	std::cout << "\n";

	std::cout << "Font hinting is an optional process that can improve the quality and \nappearance of fonts on the screen. Which of the hinting technologies\nwould you like to use in the converted font:\n\n";
	std::cout << "  0 = No hinting\n";
	std::cout << "  1 = Native hinting when available, auto-hinting otherwise\n";
	std::cout << "  2 = Auto-hinting\n";
	std::cout << "  3 = Native hinting when available, compact auto-hinting otherwise\n";
	std::cout << "  4 = Compact auto-hinting\n\n";
	std::cout << "Note: Hinted fonts are larger in file size than non-hinted fonts.\nCompact auto-hinting is a good compromise between quality and size.\n\n";
	std::cout << "Enter your choice [0, 1, 2, 3, or 4]: ";

	if (std::scanf("%s", dummy) != 1) return -1;

	DT_ID_UBYTE hinting = static_cast<DT_ID_UBYTE>(std::atoi(dummy));

	std::cout << "\n";

	DT_STREAM_FILE(sd_font, file_name);
	return engine.FontAddViaStream(font_format_id, DV_NULL, fcnr, -1, 0, hinting, sd_font);
}


void my_font_error_func(const DT_CHAR* error_message, void* /*user_param*/)
{
	std::cout << "Error message returned by D-Type Engine: " << error_message << "\n";
}


DT_SWORD my_font_subset_func(DT_ID_ULONG /*glyph_index*/, void* /*user_param*/)
{
	/*

	You can use this callback function to subset the font and/or show
	the progress of the font conversion. This function is called by
	D-Type Font Engine for each glyph in the font.

	Depending on the glyph_index, the function should return 1 if that
	glyph is to be included in the converted font and 0 otherwise.
	Note that glyph_index is font dependent.

	For example:

	if (glyph_index == 34 || glyph_index == 36 || glyph_index == 38) return 1;
	return 0;

	*/

	return 1; /* in our case we simply say that we want to include all glyphs (no subsetting) */
}


int main(int main_argc, char* main_argv[])
{
	xsys_set_work_dir(main_argc, main_argv);

	std::cout << "-----------------------------------------------------\n";
	std::cout << "D-TYPE FONT CONVERTER\n";
	std::cout << "Source File: examples/example_font_converter/main.cpp\n";
	std::cout << "-----------------------------------------------------\n";
	std::cout << "\n";
	std::cout << "This program allows you to convert TrueType, Type 1 and OpenType fonts\n";
	std::cout << "to the D-Type format.\n";
	std::cout << "\n";
	std::cout << "Please remember that existing TrueType, Type 1 and OpenType fonts may\n";
	std::cout << "be protected under copyright law. The unauthorized use or modifications\n";
	std::cout << "of any existing font files could be a violation of the rights of the\n";
	std::cout << "author. Be sure you obtain any permission required from such authors.\n";
	std::cout << "\n";

	DT_CHAR buffer[1024];
	DT_SLONG ret = 0;

	CDTEngineV8 engine;

	DT_STREAM_FILE(sd1, "config/dtype.inf");

	if (!engine.Init(sd1))
	{
		std::cout << "ERROR: INITIALIZATION FAILED!\nMost likely your current working directory is not the directory in which this\nexecutable resides. Use the command line (Terminal) to change your current\nworking directory to the directory in which this executable resides, then try\nstarting it again.\n";
		return 0;
	}

	engine.FontSetErrorCallback(my_font_error_func);

	while (1)
	{
		DT_SWORD last_index = list_fonts_in_catalog(engine);

		std::cout << "\n";
		std::cout << "Enter the index of the font to convert, " << last_index << " to add new font or -1 to exit: ";

		if (std::scanf("%s", buffer) != 1) break;

		DT_ID_SWORD font_index = static_cast<DT_ID_SWORD>(std::atoi(buffer));

		if (font_index < 0) break;
		else if (font_index >= last_index)
		{
			font_index = add_new_font_to_catalog(engine);
			continue;
		}

		DT_STREAM_FILE(sd2, "output.dtf");
		ret = engine.FontSaveToStream(font_index, sd2, my_font_subset_func);

		std::cout << "\n";

		std::cout << "******************************************************************\n";
		std::cout << "*                                                                *\n";

		if (ret == 1)
		{
			std::cout << "*  THE FONT WAS CONVERTED SUCCESSFULLY AND SAVED AS output.dtf   *\n";
			std::cout << "*                                                                *\n";
			std::cout << "*  It is possible to further reduce the file size of this font.  *\n";
			std::cout << "*  To do so, run the font_optimizer.exe program.                 *\n";
		}
		else
		{
			std::cout << "*  AN ERROR HAS OCCURRED WHILE CONVERTING THE FONT.              *\n";
		}

		std::cout << "*                                                                *\n";
		std::cout << "******************************************************************\n";

		std::cout << "\n";
	}

	std::cout << "Exit.\n\n";

	return 0;
}
