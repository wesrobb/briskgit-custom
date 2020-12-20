local obj_0 = o("Gradient Start Style");
local obj_1 = o("Gradient End Style");
local obj_2 = o("Gradient");

/* Object Properties */

obj_0 + s(pdRGBT, "FF000080");

obj_1 + s(pdRGBT, "0000FF80");

obj_2 + l(pdBodyPtr, obj_0);
obj_2 + l(pdStylePtr, obj_1);
obj_2 + i(pdDh, 400);
obj_2 + i(pdDw, 400);
obj_2 + i(pdGradientType, 18);

/* Image Pinning */

obj_2.Pin(0, 50, 50, pdDrawGradient);
