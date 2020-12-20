local obj_0 = o("Grid Style");
local obj_1 = o("Grid Border");
local obj_2 = o("Grid Lines");
local obj_3 = o("Grid");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + l(pdStylePtr, obj_0);
obj_1 + i(pdLineThickness, 300);

obj_2 + l(pdStylePtr, obj_0);
obj_2 + i(pdLineThickness, 100);

obj_3 + l(pdOutlinePtr, obj_1);
obj_3 + l(pdLinePtr, obj_2);
obj_3 + i(pdDw, 600);
obj_3 + i(pdDh, 250);
obj_3 + i(pdNw, 5);
obj_3 + i(pdNh, 3);

/* Image Pinning */

obj_3.Pin(0, 50, 50, pdDrawGrid);
