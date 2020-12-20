local obj_0 = o("TextArea Outline Style");
local obj_1 = o("TextArea Body Style");
local obj_2 = o("TextArea Shadow Style");
local obj_3 = o("TextArea Outline");
local obj_4 = o("TextArea Shadow");
local obj_5 = o("TextArea Typography");
local obj_6 = o("TextArea Layout");
local obj_7 = o("TextArea Ruler");
local obj_8 = o("TextArea");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + s(pdRGBT, "B4961400");

obj_2 + s(pdRGBT, "80808078");
obj_2 + s(pdEP, "030A");

obj_3 + l(pdStylePtr, obj_0);
obj_3 + i(pdLineThickness, 240);

obj_4 + l(pdStylePtr, obj_2);
obj_4 + i(pdShadowDx, 5);
obj_4 + i(pdShadowDy, 5);

obj_5 + l(pdBodyPtr, obj_1);
obj_5 + l(pdShadowPtr, obj_4);
obj_5 + l(pdOutlinePtr, obj_3);
obj_5 + i(pdFontIndex, 0);
obj_5 + i(pdFontDw, 90);
obj_5 + i(pdFontDh, 90);

obj_6 + i(pdSpacingRow, 10);
obj_6 + i(pdSpacingLetter, 5);
obj_6 + i(pdKerning, 1);
obj_6 + i(pdAlign, 3);

obj_7 + p(pdTabStopPositions, "40, 80, 150, 220");
obj_7 + p(pdTabStopFlags, "1, 0, 0, 2");

obj_8 + l(pdTypographyPtr, obj_5);
obj_8 + l(pdTextLayoutPtr, obj_6);
obj_8 + l(pdTextRulerHorPtr, obj_7);
obj_8 + i(pdTextAreaDw, 800);
obj_8 + i(pdTextAreaDh, 400);
obj_8 + s(pdTextString_08, "This is a simple text area");

/* Image Pinning */

obj_8.Pin(0, 50, 50, pdDrawTextArea);
