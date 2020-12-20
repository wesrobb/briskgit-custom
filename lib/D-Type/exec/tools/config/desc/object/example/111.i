local obj_0 = o("Line Style");
local obj_1 = o("Line Definition");
local obj_2 = o("Line");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + l(pdStylePtr, obj_0);
obj_1 + i(pdLineThickness, 1500);
obj_1 + i(pdLineSegment, 10);
obj_1 + i(pdLineStart, 100);
obj_1 + i(pdLineEnd, 100);
obj_1 + i(pdLineStartSize, 10);
obj_1 + i(pdLineStartTheta, 40);
obj_1 + i(pdLineEndSize, 20);
obj_1 + i(pdLineEndTheta, 165);

obj_2 + l(pdLinePtr, obj_1);
obj_2 + i(pdDw, 600);
obj_2 + i(pdDh, 250);

/* Image Pinning */

obj_2.Pin(0, 50, 50, pdDrawLine);
