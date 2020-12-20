local obj_0 = o("Outline Style");
local obj_1 = o("Line Definition");
local obj_2 = o("LibShape");
local obj_3 = o("Group Member");

obj_0 + s(pdRGBT, "00000000");

obj_1 + l(pdStylePtr, obj_0);
obj_1 + i(pdLineThickness, 400);

obj_2 + l(pdOutlinePtr, obj_1);
obj_2 + i(pdID, 56);
obj_2 + i(pdDw, 300);
obj_2 + i(pdDh, 300);

obj_3 + l(pdFnDrawObjectPtr, obj_2);
obj_3 + p(pdTransformMatrix, "262144, -46223, 0, 262144");
obj_3 + i(pdFnDraw, 18);
obj_3 + i(pdDx, 0);
obj_3 + i(pdDy, -300);
