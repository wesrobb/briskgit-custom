local obj_0 = o("LibShape Body Style");
local obj_1 = o("LibShape");

/* Object Properties */

obj_0 + s(pdRGBT, "B4961400");

obj_1 + l(pdBodyPtr, obj_0);
obj_1 + i(pdDw, 300);
obj_1 + i(pdDh, 300);
obj_1 + i(pdID, 7);

/* Image Pinning */

obj_1.Pin(0, 50, 50, pdDrawLibraryShape);
