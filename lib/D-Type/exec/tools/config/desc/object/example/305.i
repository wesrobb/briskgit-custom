local obj_0 = o("Form Configuration");
local obj_1 = o("Form List Box");
local obj_2 = o("Form List Box - Second Column");

/* Object Properties */

obj_0 + i(fcCnfLib, 0);
obj_0 + i(fcCnfCtrlStyleOnFocus, 0);
obj_0 + i(fcCnfCharBox, 6);
obj_0 + i(fcCnfButtonNormal, 14);
obj_0 + i(fcCnfButtonPushed, 13);
obj_0 + i(fcCnfSliderNormal, 2);
obj_0 + i(fcCnfSliderPushed, 3);
obj_0 + i(fcCnfRadioButtonNormal, 6);
obj_0 + i(fcCnfRadioButtonPushed, 7);
obj_0 + i(fcCnfRadioButtonSelected, 8);
obj_0 + i(fcCnfCheckBoxNormal, 3);
obj_0 + i(fcCnfCheckBoxPushed, 4);
obj_0 + i(fcCnfCheckBoxSelected, 5);

obj_1 + l(fcConfigPtr, obj_0);
obj_1 + l(fcColumnNextPtr, obj_2);
obj_1 + p(fcValue_08, "Item 0\rItem 1\rItem 2\rItem 3\rItem 4\rItem 5", PD_ASC);
obj_1 + p(fcName, "IDLISTBOX", PD_ASC);
obj_1 + i(fcDw, 800);
obj_1 + i(fcDh, 500);
obj_1 + i(fcListMultiSelect, 1);
obj_1 + i(fcItemFirst, 0);
obj_1 + p(fcListSelectedItems, "0, 2", PD_ARR_LONG_SIGNED);
obj_1 + i(fcTabOrderID, 100);
obj_1 + i(fcOnCtrlChange, 1);
obj_1 + i(fcOnCtrlScroll, 1);

obj_2 + p(fcValue_08, "$100.00\r$5,500.00\r$9,850.00\r$800.00\r$200.00\r$8,550.00", PD_ASC);
obj_2 + i(fcAlign, 2);
obj_2 + i(fcListColOffset, 400);

/* Image Pinning */

obj_1.Pin(0, 50, 50, fcDrawListBox);
