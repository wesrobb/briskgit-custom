/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <iostream>
#include <math.h>
//#include <time.h>
#include <string.h>

// D-Type header file (level 4)
#include "dtype_l4.h"
// D-Type's extra system utility functions
#include "dtxsystem.h"

// D-Type CPP Engine
#include "dtengine.cpp"


#define SURFACE_BPP 4
#define SURFACE_FORMAT (SURFACE_BPP * 8)


#include "classes/text_doc.cpp"
#include "classes/text_doc_demo1.cpp"
#include "classes/text_doc_demo2.cpp"
#include "classes/text_doc_demo3.cpp"
#include "classes/text_doc_demo4.cpp"
#include "classes/text_doc_demo5.cpp"
#include "classes/text_doc_demo6.cpp"

#include "classes/panel.cpp"
#include "classes/panel_text.cpp"
#include "classes/panel_static.cpp"
#include "classes/panel_drag.cpp"
#include "classes/panel_form.cpp"

#include "classes/windowplus.cpp"
#include "classes/displayplus.cpp"


/* The only globals for this application */
CDTEngineV8* Engine = DV_NULL;
CDTFormControlsLib* FormControlsLib = DV_NULL;
CDTDisplayPlus* MainDisplay = DV_NULL;


#include "classes/windowplus_info.cpp"
#include "classes/windowplus_settings.cpp"
#include "classes/windowplus_clipboard.cpp"
#include "classes/windowplus_insert.cpp"
#include "classes/windowplus_textdoc.cpp"
#include "classes/windowplus_mainmenu.cpp"


int main(int main_argc, char* main_argv[])
{
	xsys_set_work_dir(main_argc, main_argv);

	std::cout << "-------------------------------------------------------\n";
	std::cout << "D-TYPE TEXT ENGINE DEMO APPLICATION\n";
	std::cout << "Source File: examples/example_text_application/main.cpp\n";
	std::cout << "-------------------------------------------------------\n";
	std::cout << "\n";
	std::cout << "This sample program shows how to use 1) D-Type Text Engine to read,\n";
	std::cout << "format and render interactive Unicode text documents and 2) D-Type\n";
	std::cout << "Power Engine in conjunction with D-Type's form engine helper class\n";
	std::cout << "to render high quality and fully scalable graphical user interface.\n";
	std::cout << "\n";
	std::cout << "Additionally, this program utilizes D-Type's helper library for\n";
	std::cout << "platform independent window display, making its entire source code\n";
	std::cout << "fully portable across Windows, Linux and Macintosh.\n";
	std::cout << "\n";
	std::cout << "DO NOT CLOSE THIS CONSOLE WINDOW WHILE THE PROGRAM IS RUNNING!\n";
	std::cout << "TO QUIT THE PROGRAM, CLOSE ALL OTHER OPEN WINDOWS.\n";
	std::cout << "\n";

	Engine = DV_NULL;
	FormControlsLib = DV_NULL;
	MainDisplay = DV_NULL;

	Engine = new CDTEngineV8(); if (Engine == DV_NULL) goto err;

	DT_STREAM_FILE(sd_init, "config/dtype.inf");
	DT_STREAM_FILE(sd_fontmap, "config/fontmap.inf");

	if (!Engine->Init(sd_init, sd_fontmap)) goto err;

	FormControlsLib = new CDTFormControlsLib(Engine->GetPdEngine()); if (FormControlsLib == DV_NULL) goto err;

	MainDisplay = new CDTDisplayPlus(); if (MainDisplay == DV_NULL) goto err;

	MainDisplay->RegisterNewWindow(new CDTWindowPlus_MainMenu(MainDisplay), 0, true);

	// Main loop

	MainDisplay->EventLoop();

	goto end;

err:

	std::cout << "ERROR: INITIALIZATION FAILED!\nMost likely your current working directory is not the directory in which this\nexecutable resides. Use the command line (Terminal) to change your current\nworking directory to the directory in which this executable resides, then try\nstarting it again.\n";

end:

	if (MainDisplay != DV_NULL) delete MainDisplay;
	if (FormControlsLib != DV_NULL) delete FormControlsLib;
	if (Engine != DV_NULL) delete Engine;

	return 0;
}

