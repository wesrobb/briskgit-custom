/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <iostream>
#include <cstdio>

#include "applet.cpp"


int main(int main_argc, char* main_argv[])
{
	xsys_set_work_dir(main_argc, main_argv);

	std::cout << "------------------------------------------------------\n";
	std::cout << "D-TYPE POWER ENGINE DOCUMENT VIEWER\n";
	std::cout << "Source File: examples/example_powerdoc_viewer/main.cpp\n";
	std::cout << "------------------------------------------------------\n";
	std::cout << "\n";
	std::cout << "This sample program demonstrates how to open PowerDoc documents (.pdc files)\n";
	std::cout << "from disk and display them on the screen.\n";
	std::cout << "\n";
	std::cout << "------------------------------\n";
	std::cout << " KEYBOARD COMMANDS\n";
	std::cout << "------------------------------\n";
	std::cout << " -         ->  Zoom out\n";
	std::cout << " +         ->  Zoom in\n";
	std::cout << " PageUp    ->  Previous page\n";
	std::cout << " PageDown  ->  Next page\n";
	std::cout << " R         ->  Reset display\n";
	std::cout << " Esc       ->  Close window\n";
	std::cout << "------------------------------\n";
	std::cout << "\n";

	CDTEngineV8 engine;

	DT_STREAM_FILE(sd_init, "config/dtype.inf");

	if (!engine.Init(sd_init))
	{
		std::cout << "ERROR: INITIALIZATION FAILED!\nMost likely your current working directory is not the directory in which this\nexecutable resides. Use the command line (Terminal) to change your current\nworking directory to the directory in which this executable resides, then try\nstarting it again.\n";
		return 0;
	}

	CDTDisplay display(0);

	while (1)
	{
		std::cout << "Note: More than 50 demo documents are provided in the ../../files/powerdoc/\n";
		std::cout << "folder. Here are just a few of them to try:\n";
		std::cout << "\n";
		std::cout << "  ../../files/powerdoc/anchored_objects-rich_text_area-perspective.pdc\n";
		std::cout << "  ../../files/powerdoc/gradient_catalogue.pdc\n";
		std::cout << "  ../../files/powerdoc/international_cookbook.pdc\n";
		std::cout << "  ../../files/powerdoc/lion.pdc\n";
		std::cout << "  ../../files/powerdoc/longhorn.pdc\n";
		std::cout << "  ../../files/powerdoc/multiple_master.pdc\n";
		std::cout << "  ../../files/powerdoc/reflection.pdc\n";
		std::cout << "  ../../files/powerdoc/saturn.pdc\n";
		std::cout << "  ../../files/powerdoc/tiger.pdc\n";
		std::cout << "  ../../files/powerdoc/text_area_catalogue.pdc\n";
		std::cout << "  ../../files/powerdoc/text_directions.pdc\n";
		std::cout << "  ../../files/powerdoc/text_flow_in_two_columns.pdc\n";
		std::cout << "  ../../files/powerdoc/text_waterfall.pdc\n";
		std::cout << "  ../../files/powerdoc/traffic_signs.pdc\n";
		std::cout << "\n";
		std::cout << "Enter the path of the .pdc file to open or 0 to exit: ";

		DT_CHAR file_name[1024];
		if (std::scanf("%s", file_name) != 1) break;
		if (file_name[0] == '0' && file_name[1] == 0) break;

		DT_STREAM_FILE(sd_check, file_name); DT_DTSTREAM f = dtStreamOpen(&sd_check, DV_STREAM_OPEN_MUST);
		if (f == DV_NULL) { std::cout << "File Not Found!\n"; continue; }
		dtStreamClose(f, DV_STREAM_CLOSE_YES);

		std::cout << "\n";
		std::cout << "DO NOT CLOSE THIS CONSOLE WINDOW WHILE THE PROGRAM IS RUNNING!\n";
		std::cout << "TO QUIT THE PROGRAM, CLOSE ALL OTHER OPEN WINDOWS.\n";
		std::cout << "\n";

		CDTApplet applet(engine, display, "D-Type Example", display.Dist(1024), display.Dist(800), DV_WINDOW_HINT_RESIZE | DV_WINDOW_HINT_CENTER | DV_WINDOW_HINT_FULLRESOLUTION /* | DV_WINDOW_HINT_RGB */);

		applet.LoadDoc(0, 0, 0, file_name);

		while (applet.GetStatus())
		{
			if (applet.Redraw)
			{
				applet.DrawFrame();
				applet.Redraw = false;
			}

			else display.EventLoop();
		}
	}

	std::cout << "Exit.\n\n";

	return 0;
}
