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

	std::cout << "-------------------------------------------------------\n";
	std::cout << "D-TYPE COLOR RASTERIZER OUTPUT DEMO\n";
	std::cout << "Source File: examples/example_color_rasterizer/main.cpp\n";
	std::cout << "-------------------------------------------------------\n";
	std::cout << "\n";
	std::cout << "This sample program shows how to render scalable 2D graphics using\n";
	std::cout << "D-Type Color Rasterizer.\n";
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

	CDTDisplay display(0);
	CDTApplet applet(display, "D-Type Example", display.Dist(1024), display.Dist(800), DV_WINDOW_HINT_RESIZE | DV_WINDOW_HINT_CENTER | DV_WINDOW_HINT_FULLRESOLUTION /* | DV_WINDOW_HINT_RGB */);

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
