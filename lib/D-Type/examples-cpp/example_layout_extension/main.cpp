/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <iostream>

#include "applet.cpp"


int main(int main_argc, char* main_argv[])
{
	xsys_set_work_dir(main_argc, main_argv);

	std::cout << "---------------------------------------------------\n";
	std::cout << "D-TYPE STANDARD ENGINE + LAYOUT EXTENSION OUTPUT DEMO\n";
	std::cout << "Source File: examples/example_layout_extension/main.cpp\n";
	std::cout << "---------------------------------------------------\n";
	std::cout << "\n";
	std::cout << "This sample program shows how to render text lines using D-Type Font Engine\n";
	std::cout << "and D-Type Text Layout Extension. It shows how to perform text segmentation\n";
	std::cout << "and, at the same time, demonstartes Unicode Bidirecional (BiDi) algorithm and\n";
	std::cout << "complex text shaping (for Arabic and Indic).\n";
	std::cout << "\n";
	std::cout << "Additionally, this program utilizes D-Type's helper library for platform\n";
	std::cout << "independent window display, making its entire source code fully portable\n";
	std::cout << "across Windows, Linux and Macintosh.\n";
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
	std::cout << "DO NOT CLOSE THIS CONSOLE WINDOW WHILE THE PROGRAM IS RUNNING!\n";
	std::cout << "TO QUIT THE PROGRAM, CLOSE ALL OTHER OPEN WINDOWS.\n";
	std::cout << "\n";

	CDTEngineV8 engine;

	DT_STREAM_FILE(sd_init, "config/dtype.inf");

	if (!engine.Init(sd_init))
	{
		std::cout << "ERROR: INITIALIZATION FAILED!\nMost likely your current working directory is not the directory in which this\nexecutable resides. Use the command line (Terminal) to change your current\nworking directory to the directory in which this executable resides, then try\nstarting it again.\n";
		return 0;
	}

	CDTDisplay display(0);
	CDTApplet applet(engine, display, "D-Type Example", display.Dist(1024), display.Dist(800), DV_WINDOW_HINT_RESIZE | DV_WINDOW_HINT_CENTER | DV_WINDOW_HINT_FULLRESOLUTION /* | DV_WINDOW_HINT_RGB */);

	while (applet.GetStatus())
	{
		if (applet.Redraw)
		{
			applet.DrawFrame();
			applet.Redraw = false;
		}

		else display.EventLoop();
	}

	std::cout << "Exit.\n\n";

	return 0;
}
