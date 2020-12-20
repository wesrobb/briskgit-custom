local obj_0 = o("Row Boundary Outline Style");
local obj_1 = o("Row Boundary Body Style");
local obj_2 = o("Row Boundary Shadow Style");
local obj_3 = o("Row Boundary Outline");
local obj_4 = o("Row Boundary Shadow");
local obj_5 = o("Row Boundary");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + s(pdRGBT, "B4961400");

obj_2 + s(pdRGBT, "80808078");

obj_3 + l(pdStylePtr, obj_0);
obj_3 + i(pdLineThickness, 240);

obj_4 + l(pdStylePtr, obj_2);
obj_4 + i(pdShadowDx, 5);
obj_4 + i(pdShadowDy, 5);

obj_5 + l(pdBodyPtr, obj_1);
obj_5 + l(pdOutlinePtr, obj_3);
obj_5 + l(pdShadowPtr, obj_4);
obj_5 + i(pdMarginLeft, -200);
obj_5 + i(pdMarginRight, -200);
obj_5 + i(pdMarginTop, -100);
obj_5 + i(pdMarginBottom, -100);
obj_5 + i(pdRound, 50);
obj_5 + i(pdMerge, 1);
