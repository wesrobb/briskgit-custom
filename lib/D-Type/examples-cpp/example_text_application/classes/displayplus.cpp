/*
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */


#define DV_WINDOW_MAX 50


class CDTDisplayPlus : public CDTDisplay
{
private: // private copy constructor and assignment operator

	CDTDisplayPlus(const CDTDisplayPlus&);
	CDTDisplayPlus& operator = (const CDTDisplayPlus&);

public:

	CDTDisplayPlus() : CDTDisplay(0), WindowCount(0) {}
	virtual ~CDTDisplayPlus()
	{
		for (DT_SLONG i = 0; i < WindowCount; i++) delete Window[i];
	}

public:

	CDTWindowPlus* RegisterNewWindow(CDTWindowPlus* window, DT_SWORD class_id, bool redraw)
	{
		if (window == DV_NULL) return DV_NULL;
		if (WindowCount >= DV_WINDOW_MAX) return DV_NULL;

		/* register new window */
		Window[WindowCount] = window;
		ClassID[WindowCount] = class_id;
		WindowCount++;

		if (redraw)	window->Redraw();
		return window;
	}

	bool FocusExistingWindow(DT_SWORD class_id)
	{
		if (class_id <= 0) return false;

		/* check if a window of the same class is already open */
		for (DT_SLONG i = 0; i < WindowCount; i++)
		{
			if (!Window[i]->GetStatus()) continue;
			if (class_id != ClassID[i]) continue;
			Window[i]->Modify(1); /* yes, so raise the existing window to the top */
			return true;
		}

		return false;
	}

	void EventLoop()
	{
		DT_SLONG i, j = 0;

		do
		{
			CDTDisplay::EventLoop();

			for (i = 0, j = 0; i < WindowCount; i++)
			{
				if (Window[i]->GetStatus()) j++;
			}

		}
		while (j > 0);
	}

private:

	CDTWindowPlus* Window[DV_WINDOW_MAX];
	DT_SWORD ClassID[DV_WINDOW_MAX];
	DT_SLONG WindowCount;
};
