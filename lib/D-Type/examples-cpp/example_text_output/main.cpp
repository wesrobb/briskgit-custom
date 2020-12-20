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

	std::cout << "--------------------------------------------------\n";
	std::cout << "D-TYPE TEXT ENGINE OUTPUT DEMO\n";
	std::cout << "Source File: examples/example_text_output/main.cpp\n";
	std::cout << "--------------------------------------------------\n";
	std::cout << "\n";
	std::cout << "This sample program shows how to render Unicode text using D-Type\n";
	std::cout << "Text Engine. The text is stored in UTF-8 format in several languages\n";
	std::cout << "(English, Russian, Greek, Vietnamese, Hebrew, Arabic, Chinese).\n";
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
	DT_STREAM_FILE(sd_fontmap, "config/fontmap.inf");

	if (!engine.Init(sd_init, sd_fontmap))
	{
		std::cout << "ERROR: INITIALIZATION FAILED!\nMost likely your current working directory is not the directory in which this\nexecutable resides. Use the command line (Terminal) to change your current\nworking directory to the directory in which this executable resides, then try\nstarting it again.\n";
		return 0;
	}

	CDTDisplay display(0);
	CDTApplet applet(engine, display, "D-Type Example", display.Dist(1024), display.Dist(760), DV_WINDOW_HINT_RESIZE | DV_WINDOW_HINT_CENTER | DV_WINDOW_HINT_FULLRESOLUTION /* | DV_WINDOW_HINT_RGB */);

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
