local obj_0 = o("Form Configuration");
local obj_1 = o("Form Radio Button");

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
obj_1 + p(fcValue_08, "YES", PD_ASC);
obj_1 + p(fcName, "IDRADIOBUTTON", PD_ASC);
obj_1 + i(fcDw, 100);
obj_1 + i(fcDh, 100);
obj_1 + i(fcState, 2);
obj_1 + i(fcTabOrderID, 100);
obj_1 + i(fcOnCtrlPush, 1);

/* Image Pinning */

obj_1.Pin(0, 50, 50, fcDrawRadioButton);
