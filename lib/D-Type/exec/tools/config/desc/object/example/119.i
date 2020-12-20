local obj_0 = o("Poly Outline Style");
local obj_1 = o("Poly Body Style");
local obj_2 = o("Poly Shadow Style");
local obj_3 = o("Poly Outline");
local obj_4 = o("Poly Shadow");
local obj_5 = o("Poly");

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
obj_5 + p(pdPolyArrI, "16, 20, 25, 20, 20");
obj_5 + p(pdPolyArrX_32, "0, 400, 300, 200, -60");
obj_5 + p(pdPolyArrY_32, "0, 100, 350, 200, 100");

/* Image Pinning */

obj_5.Pin(0, 100, 100, pdDrawPoly);