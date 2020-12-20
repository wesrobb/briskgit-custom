local obj_0 = o("Text Fragment Outline Style");
local obj_1 = o("Text Fragment Body Style");
local obj_2 = o("Text Fragment Shadow Style");
local obj_3 = o("Text Fragment Outline");
local obj_4 = o("Text Fragment Shadow");
local obj_5 = o("Text Fragment Typography");
local obj_6 = o("Text Fragment Layout");
local obj_7 = o("Text Fragment Ruler");
local obj_8 = o("Text Fragment");
local obj_9 = o("Form Configuration");
local obj_10 = o("Form Rich List Box");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + s(pdRGBT, "B4961400");

obj_2 + s(pdRGBT, "80808078");
obj_2 + s(pdEP, "030A");

obj_3 + l(pdStylePtr, obj_0);
obj_3 + i(pdLineThickness, 150);

obj_4 + l(pdStylePtr, obj_2);
obj_4 + i(pdShadowDx, 5);
obj_4 + i(pdShadowDy, 5);

obj_5 + l(pdBodyPtr, obj_1);
obj_5 + l(pdShadowPtr, obj_4);
obj_5 + l(pdOutlinePtr, obj_3);
obj_5 + i(pdFontIndex, 0);
obj_5 + i(pdFontDw, 60);
obj_5 + i(pdFontDh, 60);

obj_6 + i(pdSpacingRow, 10);
obj_6 + i(pdSpacingLetter, 5);
obj_6 + i(pdKerning, 1);
obj_6 + i(pdAlign, 0);
obj_6 + i(pdAlignEnd, 0);

obj_7 + p(pdTabStopPositions, "40, 80, 150, 220");
obj_7 + p(pdTabStopFlags, "1, 0, 0, 2");

obj_8 + l(pdTypographyPtr, obj_5);
obj_8 + l(pdTextLayoutPtr, obj_6);
obj_8 + l(pdTextRulerHorPtr, obj_7);
obj_8 + s(pdTextString_08, "Item 0\rItem 1\rItem 2\rItem 3\rItem 4\rItem 5");

obj_9 + i(fcCnfLib, 0);
obj_9 + i(fcCnfCtrlStyleOnFocus, 0);
obj_9 + i(fcCnfCharBox, 6);
obj_9 + i(fcCnfButtonNormal, 14);
obj_9 + i(fcCnfButtonPushed, 13);
obj_9 + i(fcCnfSliderNormal, 2);
obj_9 + i(fcCnfSliderPushed, 3);
obj_9 + i(fcCnfRadioButtonNormal, 6);
obj_9 + i(fcCnfRadioButtonPushed, 7);
obj_9 + i(fcCnfRadioButtonSelected, 8);
obj_9 + i(fcCnfCheckBoxNormal, 3);
obj_9 + i(fcCnfCheckBoxPushed, 4);
obj_9 + i(fcCnfCheckBoxSelected, 5);

obj_10 + l(pdTextFragmentPtr, obj_8);
obj_10 + l(fcConfigPtr, obj_9);
obj_10 + p(fcName, "IDRICHLISTBOX", PD_ASC);
obj_10 + i(fcDw, 800);
obj_10 + i(fcDh, 500);
obj_10 + i(fcListMultiSelect, 1);
obj_10 + i(fcItemFirst, 0);
obj_10 + p(fcListSelectedItems, "0, 2", PD_ARR_LONG_SIGNED);
obj_10 + i(fcTabOrderID, 100);
obj_10 + i(fcOnCtrlChange, 1);
obj_10 + i(fcOnCtrlScroll, 1);

/* Image Pinning */

obj_10.Pin(0, 50, 50, fcDrawRichListBox);
