local obj_0 = o("Text Fragment Body Style");
local obj_1 = o("Text Fragment Typography");
local obj_2 = o("Text Fragment Layout");
local obj_3 = o("Text Fragment");
local obj_4 = o("RichTextDesign");

/* Object Properties */

obj_0 + s(pdRGBT, "B4961400");

obj_1 + l(pdBodyPtr, obj_0);
obj_1 + i(pdFontIndex, 2);
obj_1 + i(pdFontDw, 90);
obj_1 + i(pdFontDh, 90);

obj_2 + p(pdPosArrX_32, "15600, 22600, 18600, 15600, 10000, 11000, 8000, 10000, 4100, 3000");
obj_2 + p(pdPosArrY_32, "0, -1000, -1000, -1000, -1000, -3000, -5000, -11000, -14000, -15000");
obj_2 + i(pdHinting, 1);
obj_2 + i(pdPositioning, 1);

obj_3 + l(pdTypographyPtr, obj_1);
obj_3 + l(pdTextLayoutPtr, obj_2);
obj_3 + s(pdTextString_08, "Positioned");

obj_4 + l(pdTextFragmentPtr, obj_3);

/* Image Pinning */

obj_4.Pin(0, 124, 425, pdDrawRichTextDesign);
