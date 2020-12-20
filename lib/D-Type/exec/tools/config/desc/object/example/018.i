local obj_0 = o("Pattern Fill Definition - First");
local obj_1 = o("Pattern Fill Definition - Second");
local obj_2 = o("Pattern Fill Definition - Third");

/* Object Properties */

obj_0 + l(pdPatternFillPtr, obj_1);
obj_0 + i(pdNw, 3);
obj_0 + i(pdNh, 3);
obj_0 + p(pdImageDataGray, "880088 888888 008800");

obj_1 + l(pdPatternFillPtr, obj_2);
obj_1 + i(pdNw, 4);
obj_1 + i(pdNh, 4);
obj_1 + p(pdImageDataGray, "88880000 88880000 00008888 00008888");

obj_2 + i(pdNw, 5);
obj_2 + i(pdNh, 5);
obj_2 + p(pdImageDataGray, "8888008888 8888008888 8888888888 0000880000 0000880000");
