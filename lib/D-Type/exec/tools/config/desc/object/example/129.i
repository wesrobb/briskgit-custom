local obj_0 = o("LibPolyline Line Style");
local obj_1 = o("LibPolyline Line");
local obj_2 = o("LibPolyline");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + l(pdStylePtr, obj_0);
obj_1 + i(pdLineThickness, 240);

obj_2 + l(pdLinePtr, obj_1);
obj_2 + i(pdDw, 300);
obj_2 + i(pdDh, 300);
obj_2 + i(pdID, 12);

/* Image Pinning */

obj_2.Pin(0, 100, 100, pdDrawLibraryPolyline);
