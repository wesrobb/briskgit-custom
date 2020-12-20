local obj_0 = o("Line Style");
local obj_1 = o("Line Definition");
local obj_2 = o("Baseline");
local obj_3 = o("Row Boundary Outline Style");
local obj_4 = o("Row Boundary Body Style");
local obj_5 = o("Row Boundary Shadow Style");
local obj_6 = o("Row Boundary Outline");
local obj_7 = o("Row Boundary Shadow");
local obj_8 = o("Row Boundary");
local obj_9 = o("Text Extras");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + l(pdStylePtr, obj_0);
obj_1 + i(pdLineThickness, 1500);
obj_1 + i(pdLineSegment, 10);
obj_1 + i(pdLineStart, 100);
obj_1 + i(pdLineEnd, 100);

obj_2 + l(pdLinePtr, obj_1);
obj_2 + i(pdMarginLeft, 200);
obj_2 + i(pdMarginRight, 50);

obj_3 + s(pdRGBT, "50641400");

obj_4 + s(pdRGBT, "B4961400");

obj_5 + s(pdRGBT, "80808078");

obj_6 + l(pdStylePtr, obj_3);
obj_6 + i(pdLineThickness, 240);

obj_7 + l(pdStylePtr, obj_5);
obj_7 + i(pdShadowDx, 5);
obj_7 + i(pdShadowDy, 5);

obj_8 + l(pdBodyPtr, obj_4);
obj_8 + l(pdOutlinePtr, obj_6);
obj_8 + l(pdShadowPtr, obj_7);
obj_8 + i(pdMarginLeft, -200);
obj_8 + i(pdMarginRight, -200);
obj_8 + i(pdMarginTop, -100);
obj_8 + i(pdMarginBottom, -100);
obj_8 + i(pdRound, 50);
obj_8 + i(pdMerge, 1);

obj_9 + l(pdBaselinePtr, obj_2);
obj_9 + l(pdRowBoundPtr, obj_8);
