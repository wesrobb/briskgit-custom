local obj_0 = o("Grayscale Image Data");
local obj_1 = o("Grayscale Image");

/* Object Properties */

obj_0 + i(pdNw, 4);
obj_0 + i(pdNh, 4);
obj_0 + p(pdImageDataGray, "88880000 88880000 00008888 00008888");

obj_1 + l(pdImageDataPtr, obj_0);
obj_1 + s(pdRangeRR, "77FF");
obj_1 + s(pdRangeGG, "44FF");
obj_1 + s(pdRangeBB, "66AA");
obj_1 + i(pdQuality, 0);
obj_1 + i(pdDw, 312);
obj_1 + i(pdDh, 120);
obj_1 + i(pdT, 80);

/* Image Pinning */

obj_1.Pin(0, 50, 50, pdDrawImage);
