local obj_0 = o("Pattern Style");
local obj_1 = o("Pattern");

/* Object Properties */

obj_0 + s(pdRGBT, "B4961400");

obj_1 + l(pdBodyPtr, obj_0);
obj_1 + i(pdDw, 400);
obj_1 + i(pdDh, 400);
obj_1 + i(pdPatternGroup, 2);
obj_1 + i(pdPatternItem, 14);

/* Image Pinning */

obj_1.Pin(0, 50, 50, pdDrawPattern);
