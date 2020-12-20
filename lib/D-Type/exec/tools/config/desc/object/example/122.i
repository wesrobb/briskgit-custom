/* This example illustrates how Generic Object can be used to represent a Library Shape object */

local obj_0 = o("LibShape Style");
local obj_1 = o("LibShape");
local obj_2 = o("Generic Object");

/* Object Properties */

obj_0 + s(pdRGBT, "B4961400");

obj_1 + l(pdBodyPtr, obj_0);
obj_1 + i(pdDw, 300);
obj_1 + i(pdDh, 300);
obj_1 + i(pdID, 8);

obj_2 + l(pdFnDrawObjectPtr, obj_1);
obj_2 + i(pdFnDraw, 18);

/* Image Pinning */

obj_2.Pin(0, 50, 50, pdDrawGenericObject);
