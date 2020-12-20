local obj_0 = o("True Ellipse Outline Style");
local obj_1 = o("True Ellipse Body Style");
local obj_2 = o("True Ellipse Shadow Style");
local obj_3 = o("True Ellipse Outline");
local obj_4 = o("True Ellipse Shadow");
local obj_5 = o("True Ellipse");

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
obj_5 + i(pdRadiusHor, 300);
obj_5 + i(pdRadiusVer, 125);

/* Image Pinning */

obj_5.Pin(0, 350, 150, pdDrawTrueEllipse);
