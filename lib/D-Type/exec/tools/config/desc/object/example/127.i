local obj_0 = o("TextDesign Body Style");
local obj_1 = o("TextDesign Typography");
local obj_2 = o("TextDesign Layout");
local obj_3 = o("TextDesign");

/* Object Properties */

obj_0 + s(pdRGBT, "B4961400");

obj_1 + l(pdBodyPtr, obj_0);
obj_1 + i(pdFontIndex, 0);
obj_1 + i(pdFontDw, 90);
obj_1 + i(pdFontDh, 90);

obj_2 + p(pdPosArrI, "0, 0, 0, 1, 0, 0, 0, 0, 7");
obj_2 + p(pdPosArrY_32, "0, 6400, -6400, 30600, 0, 0, 0, 0, 55600, 0, -1000, -1000, -1000, -1000, -3000, -5000, -11000, -14000, -15000");
obj_2 + p(pdPosArrX_32, "2000, 21800, 15600, -16000, 15360, 15360, 5360, 9360, 1360, 15600, 22600, 18600, 15600, 10000, 11000, 8000, 10000, 4100, 3000");
obj_2 + i(pdHinting, 1);
obj_2 + i(pdPositioning, 1);

obj_3 + l(pdTypographyPtr, obj_1);
obj_3 + l(pdTextLayoutPtr, obj_2);
obj_3 + s(pdTextString_08, "Manually Positioned");

/* Image Pinning */

obj_3.Pin(0, 41, 110, pdDrawTextDesign);
