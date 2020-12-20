bool CDTBuilderRast::Img_Autoload(DT_SLONG page, DT_SLONG x, DT_SLONG y, DT_UBYTE quality, const DT_CHAR* file_name)
{
	/* should never happen as Img_Autoload is called only from Img_Internal, which is called only from Img, which always creates a new Atom */
	if (ExternalMemory != DV_NULL) return false; //dtMemFree(ExternalMemory);
	//ExternalMemory = DV_NULL;

	if (file_name == DV_NULL) return false;

	DT_CHAR mime_type[255];
	DT_UBYTE file_head[20];
	DT_SLONG offset = 0;
	DT_SWORD compress_method = 0;
	DT_STREAM_FILE(sd, file_name); DT_DTSTREAM stream = dtStreamOpen(&sd, DV_STREAM_OPEN_MUST); if (stream == DV_NULL) return false;
	DT_SWORD ret = dtStreamRead(stream, file_head, 18, 0); dtStreamClose(stream, DV_STREAM_CLOSE_YES); if (ret != 1) return false;
	DT_ID_SWORD file_type = pdxIsKnownFileType(file_head, 18, "", mime_type, &offset, &compress_method);
	if (file_type == 0) return false; else if (file_type == 2) return (Doc->Append(&sd, page, x, y) == 1);

	DT_AUX_GRAPHICS_ATTRIBS gattribs;

	gattribs.R1 = 255;
	gattribs.G1 = 255;
	gattribs.B1 = 255;
	gattribs.R2 = 0;
	gattribs.G2 = 0;
	gattribs.B2 = 0;
	gattribs.PaletteFlag = 0;
	gattribs.TransparencyFlag = 0;
	gattribs.AlphaFlag = 1;
	gattribs.AlphaValue = 255;
	gattribs.Quality = 0;

	DT_UBYTE misc_flags = 1; /* external bitmap memory */
	DT_SLONG prop_id = 0, len;

	     if (file_type == 3) prop_id = pdxImgLoadPNG(Doc->GetPdDoc(), file_name, page, x, y, -1, -1, &gattribs, misc_flags);
	else if (file_type == 6) prop_id = pdxImgLoadJPG(Doc->GetPdDoc(), file_name, page, x, y, -1, -1, &gattribs, misc_flags);
	else if (file_type == 7) prop_id = pdxImgLoadWindowsBitmap(Doc->GetPdDoc(), file_name, page, x, y, -1, -1, &gattribs, misc_flags);

	if (prop_id == 0) return false;

	DT_ID_SLONG obj = Doc->ImgGetObj(Doc->ImgGetLast()); if (obj < 0) return false;
	Doc->PropApplyAsUByte(obj, pdQuality, quality);
	if (misc_flags & 1) ExternalMemory = Doc->PropGet(Doc->LinkGet(obj, pdImageDataPtr), prop_id, &len);
	return true;
}


CDTBuilderRast& CDTBuilderRast::Img_Internal(const CDTAttribs& a)
{
	if (!Img_Autoload(a.APage, a.AX, a.AY, static_cast<DT_UBYTE>(a.AQuality), a.ASrc)) return *this;

	DT_ID_SLONG obj = Doc->ImgGetObj(Img0);
	DT_SLONG w = a.AW, w_img = Doc->PropGetAsSLong(obj, pdDw); if (w < 0) w = -w * w_img / 16384; /* w is a scale */
	DT_SLONG h = a.AH, h_img = Doc->PropGetAsSLong(obj, pdDh); if (h < 0) h = -h * h_img / 16384; /* h is a scale */
	DT_SLONG xbs = (w ? (a.AFontW * w_img / w) : 0);
	DT_SLONG ybs = (h ? (a.AFontH * h_img / h) : 0);
	DT_SLONG dy = 0; if (a.ABaseAlign == DE_BASEALIGN_BOTTOM) dy = h_img; else if (a.ABaseAlign == DE_BASEALIGN_MIDDLE) dy = h_img / 2;

	//CDTBuilder::ToObj(0, 200, 300, 224, a.AFontW, a.AFontH, a.AFontW, a.AFontH, 300, 0);
	CDTBuilder::ToObj(Img0, Img0, 0 * a.AX, 0 * a.AY + dy, w_img, h_img, xbs, ybs, a.AFontW, a.AFontH, w_img, 0);

	return *this;
}
