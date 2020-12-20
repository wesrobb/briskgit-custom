local obj_0 = o("Form Configuration");
local obj_1 = o("Form Text Box");

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
obj_1 + p(fcValue_08, "Enter your text here", PD_ASC);
obj_1 + p(fcName, "IDTEXTBOX", PD_ASC);
obj_1 + i(fcDw, 800);
obj_1 + i(fcDh, 300);
obj_1 + i(fcTabOrderID, 100);
obj_1 + i(fcOnCtrlChange, 1);

/* Image Pinning */

obj_1.Pin(0, 50, 50, fcDrawTextBox);
