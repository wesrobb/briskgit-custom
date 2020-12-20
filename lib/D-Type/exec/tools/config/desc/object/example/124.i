local obj_0 = o("Polyline Line Style");
local obj_1 = o("Polyline Shadow Style");
local obj_2 = o("Polyline Line");
local obj_3 = o("Polyline Shadow");
local obj_4 = o("Polyline");

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
obj_4 + p(pdPolyArrI, "17, 20, 25, 20, 20");
obj_4 + p(pdPolyArrX_32, "0, 400, 300, 200, -60");
obj_4 + p(pdPolyArrY_32, "0, 100, 350, 200, 100");

/* Image Pinning */

obj_4.Pin(0, 100, 100, pdDrawPolyline);
