local obj_0 = o("Line Style");
local obj_1 = o("Line Definition");
local obj_2 = o("Baseline");

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
