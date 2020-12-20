local obj_0 = o("Surface Style");
local obj_1 = o("Outline Style");
local obj_2 = o("TextLine Body Style");
local obj_3 = o("Line Definition");
local obj_4 = o("LibShape");
local obj_5 = o("LibShape");
local obj_6 = o("TextLine Typography");
local obj_7 = o("Text Layout");
local obj_8 = o("TextLine");
local obj_9 = o("Group Member #0");
local obj_10 = o("Group Member #1");
local obj_11 = o("Group Member #2");
local obj_12 = o("Group");
local obj_13 = o("PowerFont Record #0");
local obj_14 = o("PowerFont");

/* Object Properties */

obj_0 + s(pdRGBT, "FF004000");

obj_1 + s(pdRGBT, "00000000");

obj_2 + s(pdRGBT, "FFFFFF00");

obj_3 + l(pdStylePtr, obj_1);
obj_3 + i(pdLineThickness, 400);

obj_4 + l(pdOutlinePtr, obj_3);
obj_4 + i(pdID, 56);
obj_4 + i(pdDw, 300);
obj_4 + i(pdDh, 300);

obj_5 + l(pdBodyPtr, obj_0);
obj_5 + i(pdID, 56);
obj_5 + i(pdDw, 270);
obj_5 + i(pdDh, 270);

obj_6 + l(pdBodyPtr, obj_2);
obj_6 + i(pdFontIndex, 0);
obj_6 + i(pdFontDw, 95);
obj_6 + i(pdFontDh, 130);
obj_6 + i(pdSkewHor, 0);
obj_6 + i(pdSkewVer, 0);
obj_6 + i(pdRotation, 0);

obj_7 + i(pdKerning, 1);
obj_7 + i(pdAlign, 0);
obj_7 + i(pdAlignEnd, 0);
obj_7 + i(pdRelativeDirection, 0);

obj_8 + i(pdTypographyPtr, obj_6);
obj_8 + i(pdTextLayoutPtr, obj_7);
obj_8 + s(pdTextString_08, "STOP");

obj_9 + l(pdGroupMemberPtr, obj_10);
obj_9 + l(pdFnDrawObjectPtr, obj_4);
obj_9 + i(pdFnDraw, 18);
obj_9 + i(pdDx, 0);
obj_9 + i(pdDy, -300);

obj_10 + l(pdGroupMemberPtr, obj_11);
obj_10 + l(pdFnDrawObjectPtr, obj_5);
obj_10 + i(pdFnDraw, 18);
obj_10 + i(pdDx, 15);
obj_10 + i(pdDy, -285);

obj_11 + l(pdFnDrawObjectPtr, obj_8);
obj_11 + i(pdFnDraw, 1);
obj_11 + i(pdDx, 19);
obj_11 + i(pdDy, -102);

obj_12 + l(pdGroupMemberPtr, obj_9);

obj_13 + l(pdFnDrawObjectPtr, obj_12);
obj_13 + i(pdDw, 350);

obj_14 + l(pdPowerFontRecordPtr, obj_13);
obj_14 + p(pdBoundingBox, "-50, -50, 350, 350", PD_ARR_LONG_SIGNED);
obj_14 + i(pdDw, 300);
obj_14 + i(pdDh, 300);
