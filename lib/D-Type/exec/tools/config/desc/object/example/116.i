local obj_0 = o("Arc Style");
local obj_1 = o("Arc Shadow Style");
local obj_2 = o("Arc Line Definition");
local obj_3 = o("Arc Shadow");
local obj_4 = o("Arc");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + s(pdRGBT, "80808078");

obj_2 + l(pdStylePtr, obj_0);
obj_2 + i(pdLineThickness, 240);

obj_3 + l(pdStylePtr, obj_1);
obj_3 + i(pdShadowDx, 5);
obj_3 + i(pdShadowDy, 5);

obj_4 + l(pdLinePtr, obj_2);
obj_4 + l(pdShadowPtr, obj_3);
obj_4 + i(pdRadiusHor, 150);
obj_4 + i(pdRadiusVer, 150);
obj_4 + i(pdAngleAlpha, 210);
obj_4 + i(pdAngleTheta, 120);

/* Image Pinning */

obj_4.Pin(0, 250, 250, pdDrawArc);
