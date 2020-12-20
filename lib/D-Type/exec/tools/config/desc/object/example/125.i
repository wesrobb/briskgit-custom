local obj_0 = o("Fast Rect Style");
local obj_1 = o("Fast Rect");

/* Object Properties */

obj_0 + s(pdRGBT, "FF000080");

obj_1 + l(pdBodyPtr, obj_0);
obj_1 + i(pdDh, 400);
obj_1 + i(pdDw, 400);
obj_1 + i(pdGradientType, 0);

/* Image Pinning */

obj_1.Pin(0, 50, 50, pdDrawGradient);
