local obj_0 = o("TextLine Outline Style");
local obj_1 = o("TextLine Body Style");
local obj_2 = o("TextLine Shadow Style");
local obj_3 = o("TextLine Outline");
local obj_4 = o("TextLine Shadow");
local obj_5 = o("TextLine Typography");

/* Object Properties */

obj_0 + s(pdRGBT, "50641400");

obj_1 + s(pdRGBT, "B4961400");
obj_1 + s(pdEP, "030A");

obj_2 + s(pdRGBT, "80808078");
obj_2 + s(pdEP, "030A");

obj_3 + l(pdStylePtr, obj_0);
obj_3 + i(pdLineThickness, 240);

obj_4 + l(pdStylePtr, obj_2);
obj_4 + i(pdShadowDx, 5);
obj_4 + i(pdShadowDy, 5);

obj_5 + l(pdBodyPtr, obj_1);
obj_5 + l(pdShadowPtr, obj_4);
obj_5 + l(pdOutlinePtr, obj_3);
obj_5 + i(pdFontIndex, 0);
obj_5 + i(pdFontDw, 90);
obj_5 + i(pdFontDh, 90);
