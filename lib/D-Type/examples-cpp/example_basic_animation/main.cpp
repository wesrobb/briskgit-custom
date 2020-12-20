/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#include <iostream>
#include <ctime>

#include "applet.cpp"


int main(int main_argc, char* main_argv[])
{
	xsys_set_work_dir(main_argc, main_argv);

	std::cout << "------------------------------------------------------\n";
	std::cout << "D-TYPE FONT ENGINE ANIMATION DEMO\n";
	std::cout << "Source File: examples/example_basic_animation/main.cpp\n";
	std::cout << "------------------------------------------------------\n";
	std::cout << "\n";
	std::cout << "This sample program shows how to render animated scalable text using D-Type\n";
	std::cout << "Font Engine.\n";
	std::cout << "\n";
	std::cout << "------------------------------\n";
	std::cout << " KEYBOARD COMMANDS\n";
	std::cout << "------------------------------\n";
	std::cout << " -         ->  Zoom out\n";
	std::cout << " +         ->  Zoom in\n";
	std::cout << " R         ->  Reset display\n";
	std::cout << " SPACE     ->  Pause/Continue\n";
	std::cout << " Esc       ->  Close window\n";
	std::cout << "------------------------------\n";
	std::cout << "\n";
	std::cout << "DO NOT CLOSE THIS CONSOLE WINDOW WHILE THE PROGRAM IS RUNNING!\n";
	std::cout << "TO QUIT THE PROGRAM, CLOSE ALL OTHER OPEN WINDOWS.\n";
	std::cout << "\n";
	std::cout << "\n";

	CDTEngineV8 engine;

	DT_STREAM_FILE(sd_init, "config/dtype.inf");

	if (!engine.Init(sd_init))
	{
		std::cout << "ERROR: INITIALIZATION FAILED!\nMost likely your current working directory is not the directory in which this\nexecutable resides. Use the command line (Terminal) to change your current\nworking directory to the directory in which this executable resides, then try\nstarting it again.\n";
		return 0;
	}

	CDTDisplay display(0);
	CDTApplet applet(engine, display, "D-Type Example", display.Dist(640), display.Dist(400), DV_WINDOW_HINT_RESIZE | DV_WINDOW_HINT_CENTER | DV_WINDOW_HINT_FULLRESOLUTION /* | DV_WINDOW_HINT_RGB */);

	DT_SLONG counter = 0;
	std::clock_t clock_start = std::clock();

	while (applet.GetStatus())
	{
		if (!applet.Pause)
		{
			applet.NextFrame();
			applet.Redraw = true;
		}
		else
		{
			clock_start = clock();
			counter = 0;
		}

		if (applet.Redraw)
		{
			applet.DrawFrame();
			applet.Redraw = false;

			counter++;

			if (counter % 50 != 0) continue;
			DT_FLOAT duration = (std::clock() - clock_start); duration /= CLOCKS_PER_SEC;
			std::cout << "frames = " << counter << ", duration = " << duration << ", fps = " << (counter / duration) << "\n";
		}

		else display.EventLoop();
	}

	std::cout << "Exit.\n\n";

	return 0;
}
