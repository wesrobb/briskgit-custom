/*
 * dtextras.cpp - auxiliary implementation file for D-Type Standard Engine
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#include "dtype.h"
#include "dtextras.h"


#define DF_DTX_SQRT(a) sqrt(static_cast<DT_FLOAT>(a))


void dtxConvertCharsToGlyphs(DT_DTENGINE engine, DT_ID_SWORD font_index, const DT_ID_ULONG chars_arr[], DT_ID_ULONG glyph_arr[])
{
	DT_SLONG i = 0;
	while ((i < DV_MAXLINELENGTH) && (chars_arr[i] != 0)) { glyph_arr[i] = dtFontGetGlyphIndex(engine, font_index, chars_arr[i]); i++; }
	glyph_arr[i] = DV_ENDOFGLYPHARR;
}


void dtxConvertCharsToGlyphs(DT_DTENGINE engine, DT_ID_SWORD font_index, const DT_CHAR chars_arr[], DT_ID_ULONG glyph_arr[])
{
	DT_UBYTE b;
	DT_SLONG i = 0;
	while ((i < DV_MAXLINELENGTH) && (chars_arr[i] != 0)) { b = static_cast<DT_UBYTE>(chars_arr[i]); glyph_arr[i] = dtFontGetGlyphIndex(engine, font_index, b); i++; }
	glyph_arr[i] = DV_ENDOFGLYPHARR;
}


DT_SWORD dtxGlyphsGetInfo(DT_DTENGINE engine, DT_ID_SWORD mode, DT_ID_SWORD font_index, const DT_ID_ULONG glyph_arr[], DT_SLONG glyph_displacements[], DT_SLONG glyph_advances[], DT_SLONG* font_em_h, DT_SLONG* font_em_v, DT_RECT_SLONG* glyphbox_ext)
{
	DT_SLONG i = 0;
	DT_RECT_SWORD fontbox;
	DT_RECT_SWORD glyphbox;
	DT_RECT_SWORD glyphbox_ext_tmp;
	DT_UBYTE transform_params_mul_h, transform_params_mul_v;
	DT_UWORD glyph_advance = 0;
	DT_UWORD em_h, em_v;
	DT_ID_SWORD vert_mode = mode & 128;
	//DT_ID_SWORD kern_mode = mode & 127;

	if (glyph_arr == DV_NULL || glyph_displacements == DV_NULL || glyph_advances == DV_NULL || font_em_h == DV_NULL || font_em_v == DV_NULL || glyphbox_ext == DV_NULL || *glyph_arr == DV_ENDOFGLYPHARR) return 0;

	if (dtFontGetMetrics(engine, font_index, &em_h, &em_v, &fontbox) != 1) return 0;

	dtTypesetterGetSizeSubscale(engine, &transform_params_mul_h, &transform_params_mul_v);

	*font_em_h = em_h * transform_params_mul_h;
	*font_em_v = em_v * transform_params_mul_v;

	glyphbox_ext_tmp = fontbox;

	if (vert_mode)
	{
		DT_SLONG ascender = dtFontGetNumericValue(engine, font_index, DV_NVAL_ASCENDER);
		*glyph_advances = ascender;
	}
	else
	{
		*glyph_advances = 0;
	}

	while ((i < DV_MAXLINELENGTH) && (*glyph_arr != DV_ENDOFGLYPHARR))
	{
		dtGlyphGetMetrics(engine, font_index, *glyph_arr, &glyph_advance, &glyphbox);

		glyphbox_ext_tmp.xmn = DF_MIN(glyphbox_ext_tmp.xmn, glyphbox.xmn);
		glyphbox_ext_tmp.ymn = DF_MIN(glyphbox_ext_tmp.ymn, glyphbox.ymn);
		glyphbox_ext_tmp.xmx = DF_MAX(glyphbox_ext_tmp.xmx, glyphbox.xmx);
		glyphbox_ext_tmp.ymx = DF_MAX(glyphbox_ext_tmp.ymx, glyphbox.ymx);

		glyph_advances++;

		if (vert_mode)
		{
			//*glyph_displacements = (glyphbox.xmn - glyphbox.xmx) / 2;
			*glyph_displacements = -glyph_advance / 2;
			*glyph_advances = em_v;
		}
		else
		{
			*glyph_displacements = 0;
			*glyph_advances = glyph_advance;
		}

		glyph_displacements++;
		glyph_arr++;
		i++;
	}

	*glyph_displacements = 0;

	glyphbox_ext->xmn = glyphbox_ext_tmp.xmn;
	glyphbox_ext->ymn = glyphbox_ext_tmp.ymn;
	glyphbox_ext->xmx = glyphbox_ext_tmp.xmx;
	glyphbox_ext->ymx = glyphbox_ext_tmp.ymx;

	return 1;
}


DT_FLOAT dtxGlyphsPosition(DT_DTENGINE engine, const DT_TYPE_EFFECTS_L* type, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[], const DT_SLONG glyph_displacements[], const DT_SLONG glyph_advances[], DT_SLONG font_em_h, DT_SLONG /*font_em_v*/, const DT_RECT_SLONG* clipbox, DT_POINT_FLOAT glyph_arr_origin[], DT_SLONG* i_min, DT_SLONG* i_max, DT_FLOAT* x, DT_FLOAT* y, DT_FLOAT start_advance)
{
	DT_ID_SWORD font_index = type->font_index;
	DT_TYPE_EFFECTS_L type_tmp = {font_index, 0, 0, {{0, 0, 0, 0, 0}}, {0, 0, 0, 0, 0, DV_SCALE_100}};

	DT_UBYTE transform_params_mul_h, transform_params_mul_v;
	DT_SLONG size_h_abs, size_v_abs;
	DT_SWORD kernx = 0, kerny = 0;
	DT_SWORD precision_h_not_required;

	DT_SLONG i = 0, method = 0;
	DT_SLONG glyphs_advance_fu = 0;
	DT_FLOAT glyphs_advance_flt = 0;
	DT_FLOAT glyphs_advance_flt0 = 0;

	DT_FLOAT delta_pf;
	DT_SLONG delta_px;
	DT_SLONG d_fu, xmn_fu = 0, xmx_fu = 0;
	DT_FLOAT d_pf, xmn_pf = 0, xmx_pf = 0, xmx_pf0;
	DT_FLOAT ymn_dummy, ymx_dummy;

	DT_FLOAT proj_x = 0.0, bx = 0.0, base_x = 0.0;
	DT_FLOAT proj_y = 0.0, by = 0.0, base_y = 0.0;
	DT_FLOAT k = 0.0, fx = 0.0, fx2 = 0.0;
	DT_FLOAT tmp;

	DT_RECT_SWORD glyphbox = {0, 0, 0, 0};

	DT_ID_SWORD vert_mode = mode & 128;
	DT_ID_SWORD kern_mode = mode & 127;

	/* Calculate fx, proj_x and proj_y from the transformation matrix tm */
	/* (for straight lines of glyphs, proj_x is 1.0 and proj_y is 0.0) */

	if (vert_mode)
	{
		bx = tm[1][1];
		by = tm[1][0];
		fx = DF_DTX_SQRT(tm[0][1] * tm[0][1] + tm[1][1] * tm[1][1]);
		if (fx < 0.0 || fx > 0.0) { proj_x = tm[0][1] / fx; proj_y = tm[1][1] / fx; }
	}
	else
	{
		bx = tm[0][1];
		by = tm[1][1];
		fx = DF_DTX_SQRT(tm[0][0] * tm[0][0] + tm[1][0] * tm[1][0]);
		if (fx < 0.0 || fx > 0.0) { proj_x = tm[0][0] / fx; proj_y = tm[1][0] / fx; }
	}

	dtTypesetterGetSizeSubscale(engine, &transform_params_mul_h, &transform_params_mul_v);

	*i_min = 32000;
	*i_max = -32000;

	if (kern_mode == DV_SPACING_DEVICE_2)
	{
		kern_mode = DV_SPACING_DEVICE;
		method = 1;
	}
	else if (kern_mode == DV_SPACING_KERN_DEVICE_2)
	{
		kern_mode = DV_SPACING_KERN_DEVICE;
		method = 1;
	}

	if (kern_mode == DV_SPACING_ROUND_ADD || kern_mode == DV_SPACING_KERN_ROUND_ADD)
	{
		*x = DF_ROUND(*x);
		*y = DF_ROUND(*y);

		glyphs_advance_flt0 = glyphs_advance_flt = DF_ROUND(fx * glyph_advances[0]) + start_advance;

		while (i < DV_MAXLINELENGTH)
		{
			if (glyph_displacements == DV_NULL) { base_x = base_y = 0.0; } else { base_x = bx * glyph_displacements[i]; base_y = by * glyph_displacements[i]; }

			glyph_arr_origin[i].x = DF_ROUND(proj_x * glyphs_advance_flt + base_x);
			glyph_arr_origin[i].y = DF_ROUND(proj_y * glyphs_advance_flt + base_y);

			if (*glyph_arr == DV_ENDOFGLYPHARR) break;

			if ((clipbox != DV_NULL) && (glyph_arr_origin[i].x >= clipbox->xmn) && (glyph_arr_origin[i].x <= clipbox->xmx) && (glyph_arr_origin[i].y >= clipbox->ymn) && (glyph_arr_origin[i].y <= clipbox->ymx))
			{
				*i_min = DF_MIN(i, *i_min);
				*i_max = DF_MAX(i, *i_max);
			}

			if (vert_mode) {} else if (kern_mode == DV_SPACING_KERN_ROUND_ADD) dtGlyphGetKern(engine, font_index, *glyph_arr, *(glyph_arr + 1), &kernx, &kerny);

			i++; glyphs_advance_flt += DF_ROUND(fx * (spacing + glyph_advances[i] + kernx));
			glyph_arr++;
		}
	}

	else if (kern_mode == DV_SPACING_ADD_ROUND || kern_mode == DV_SPACING_KERN_ADD_ROUND)
	{
		*x = DF_ROUND(*x);
		*y = DF_ROUND(*y);

		glyphs_advance_flt0 = glyphs_advance_flt = fx * glyph_advances[0] + start_advance;

		while (i < DV_MAXLINELENGTH)
		{
			if (glyph_displacements == DV_NULL) { base_x = base_y = 0.0; } else { base_x = bx * glyph_displacements[i]; base_y = by * glyph_displacements[i]; }

			glyph_arr_origin[i].x = DF_ROUND(proj_x * glyphs_advance_flt + base_x);
			glyph_arr_origin[i].y = DF_ROUND(proj_y * glyphs_advance_flt + base_y);

			if (*glyph_arr == DV_ENDOFGLYPHARR) break;

			if ((clipbox != DV_NULL) && (glyph_arr_origin[i].x >= clipbox->xmn) && (glyph_arr_origin[i].x <= clipbox->xmx) && (glyph_arr_origin[i].y >= clipbox->ymn) && (glyph_arr_origin[i].y <= clipbox->ymx))
			{
				*i_min = DF_MIN(i, *i_min);
				*i_max = DF_MAX(i, *i_max);
			}

			if (vert_mode) {} else if (kern_mode == DV_SPACING_KERN_ADD_ROUND) dtGlyphGetKern(engine, font_index, *glyph_arr, *(glyph_arr + 1), &kernx, &kerny);

			i++; glyphs_advance_flt += fx * (spacing + glyph_advances[i] + kernx);
			glyph_arr++;
		}
	}

	else if (kern_mode == DV_SPACING_DEVICE || kern_mode == DV_SPACING_KERN_DEVICE)
	{
		*x = DF_ROUND(*x);
		*y = DF_ROUND(*y);

		size_h_abs = type->transform.params.size_h; if (size_h_abs < 0) size_h_abs = 0;
		size_v_abs = type->transform.params.size_v; if (size_v_abs < 0) size_v_abs = 0;

		precision_h_not_required = (vert_mode || size_h_abs < 5 * transform_params_mul_h || size_h_abs > 32 * transform_params_mul_h || (type->transform.params.skew_h) != 0 || (type->transform.params.rotation % 90) != 0);
		/* precision is not required when font size is very small or very big or when glyphs are skewed or rotated */
		type_tmp.transform.params.size_h = size_h_abs;
		type_tmp.transform.params.size_v = size_v_abs;

		tmp = font_em_h;
		fx2 = size_h_abs / tmp;
		k = fx / fx2;

		dtTypesetterSetTypeEffects(engine, &type_tmp, 0);

		glyphs_advance_flt0 = glyphs_advance_flt = fx * glyph_advances[0] + start_advance;

		while (i < DV_MAXLINELENGTH)
		{
			if (vert_mode) {} else { dtGlyphGetMetrics(engine, font_index, *glyph_arr, DV_NULL, &glyphbox); }

			if (precision_h_not_required || (dtGlyphDoMinMax(engine, *glyph_arr, 0, 0, 0, &xmn_pf, &ymn_dummy, &xmx_pf0, &ymx_dummy) != 1))
			{
				/* if we can't get transformed min/max, then just apply linear scale (the result will be very close) */
				xmn_pf = fx2 * glyphbox.xmn;
				xmx_pf0 = fx2 * glyphbox.xmx;
			}

			xmn_fu = glyphs_advance_fu + glyphbox.xmn;

			d_fu = xmn_fu - xmx_fu;
			d_pf = fx2 * d_fu;
			/* if (d_pf < 0.5) d_pf = 0.5; */

			if (method == 0)
			{
				delta_pf = d_pf + xmx_pf - xmn_pf;
				delta_px = DF_ROUND(delta_pf);
				/* if (d_pf - (delta_px + xmn_pf - xmx_pf) > 0.25) delta_px++; */
			}
			else
			{
				if (d_pf > 0 && d_pf < 0.25) d_pf = 0.25;

				delta_pf = d_pf + xmx_pf - xmn_pf;
				if (delta_pf > 0 && delta_pf < 0.5) delta_pf = 0.5;
				delta_px = DF_FLOOR(delta_pf);
				if (delta_pf - delta_px > 0.25) delta_px++;
			}

			glyphs_advance_flt += k * delta_px;
			/* glyphs_advance_flt = fx * glyphs_advance_fu; */

			if (glyph_displacements == DV_NULL) { base_x = base_y = 0.0; } else { base_x = bx * glyph_displacements[i]; base_y = by * glyph_displacements[i]; }

			glyph_arr_origin[i].x = DF_ROUND(proj_x * glyphs_advance_flt + base_x);
			glyph_arr_origin[i].y = DF_ROUND(proj_y * glyphs_advance_flt + base_y);

			if (*glyph_arr == DV_ENDOFGLYPHARR) break;

			if ((clipbox != DV_NULL) && (glyph_arr_origin[i].x >= clipbox->xmn) && (glyph_arr_origin[i].x <= clipbox->xmx) && (glyph_arr_origin[i].y >= clipbox->ymn) && (glyph_arr_origin[i].y <= clipbox->ymx))
			{
				*i_min = DF_MIN(i, *i_min);
				*i_max = DF_MAX(i, *i_max);
			}

			xmx_fu = glyphs_advance_fu + glyphbox.xmx;
			xmx_pf = xmx_pf0;

			if (vert_mode) {} else if (kern_mode == DV_SPACING_KERN_DEVICE) dtGlyphGetKern(engine, font_index, *glyph_arr, *(glyph_arr + 1), &kernx, &kerny);

			i++; glyphs_advance_fu += spacing + glyph_advances[i] + kernx;
			glyph_arr++;
		}

		dtTypesetterSetTypeEffects(engine, type, 0);
	}

	else /* if (kern_mode == DV_SPACING_FRAC || kern_mode == DV_SPACING_KERN_FRAC) */
	{
		glyphs_advance_flt0 = glyphs_advance_flt = fx * glyph_advances[0] + start_advance;

		while (i < DV_MAXLINELENGTH)
		{
			if (glyph_displacements == DV_NULL) { base_x = base_y = 0.0; } else { base_x = bx * glyph_displacements[i]; base_y = by * glyph_displacements[i]; }

			glyph_arr_origin[i].x = proj_x * glyphs_advance_flt + base_x;
			glyph_arr_origin[i].y = proj_y * glyphs_advance_flt + base_y;

			if (*glyph_arr == DV_ENDOFGLYPHARR) break;

			if ((clipbox != DV_NULL) && (glyph_arr_origin[i].x >= clipbox->xmn) && (glyph_arr_origin[i].x <= clipbox->xmx) && (glyph_arr_origin[i].y >= clipbox->ymn) && (glyph_arr_origin[i].y <= clipbox->ymx))
			{
				*i_min = DF_MIN(i, *i_min);
				*i_max = DF_MAX(i, *i_max);
			}

			if (vert_mode) {} else if (kern_mode == DV_SPACING_KERN_FRAC) dtGlyphGetKern(engine, font_index, *glyph_arr, *(glyph_arr + 1), &kernx, &kerny);

			i++; glyphs_advance_flt += fx * (spacing + glyph_advances[i] + kernx);
			glyph_arr++;
		}
	}

	if (clipbox == DV_NULL)
	{
		*i_min = 0;
		*i_max = i - 1;
	}

	return glyphs_advance_flt - glyphs_advance_flt0;
}


DT_FLOAT dtxGlyphsGetBound(DT_DTENGINE engine, DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[], const DT_SLONG glyph_arr_displacement[], const DT_SLONG glyph_arr_advance[], DT_FLOAT start_advance, DT_BMP* bmp)
{
	DT_TM2X2 m;
	DT_TYPE_EFFECTS_L type;
	DT_SLONG font_em_h, font_em_v;
	DT_RECT_SLONG glyphbox_ext;
	DT_RECT_SWORD glyphbox;

	DT_SLONG bmp_w, bmp_h;
	DT_FLOAT glyphs_advance_flt;
	DT_FLOAT tmp;

	DT_RECT_SLONG clipbox, clipbox1, clipbox2;

	DT_SLONG i = 0, i_min, i_max;
	DT_SLONG glyph_displacements[DV_MAXLINELENGTH + 1];
	DT_SLONG glyph_advances[DV_MAXLINELENGTH + 1];
	DT_POINT_FLOAT glyph_arr_origin[DV_MAXLINELENGTH + 1];
	const DT_SLONG* ptr_glyph_displacements = glyph_displacements;
	const DT_SLONG* ptr_glyph_advances = glyph_advances;

	//DT_ID_SWORD vert_mode = mode & 128;
	DT_ID_SWORD kern_mode = mode & 127;

	DT_UWORD glyph_advance;

	bmp->m = DV_NULL;
	bmp->l = 0;
	bmp->x = 0;
	bmp->y = 0;
	bmp->w = 0;
	bmp->h = 0;

	if (dtTypesetterGetTypeEffects(engine, &type, 0) != 1) return 0; /* unknown error */
	if (type.descriptor != 0) /* transform is specified via matrix */
	{
		/* in this case we have some unsupported cases */
		if (tm == DV_NULL || kern_mode == DV_SPACING_DEVICE || kern_mode == DV_SPACING_KERN_DEVICE || kern_mode == DV_SPACING_DEVICE_2 || kern_mode == DV_SPACING_KERN_DEVICE_2) return 0;
		if (type.transform.params.size_h == 0 || type.transform.params.size_v == 0) return 0;
	}

	if (dtxGlyphsGetInfo(engine, mode, type.font_index, glyph_arr, glyph_displacements, glyph_advances, &font_em_h, &font_em_v, &glyphbox_ext) != 1) return 0;

	if (glyph_arr_displacement != DV_NULL) ptr_glyph_displacements = glyph_arr_displacement;
	if (glyph_arr_advance != DV_NULL) ptr_glyph_advances = glyph_arr_advance;

	if (tm == DV_NULL)
	{
		tm = m;
		tmp = font_em_h; m[0][0] = type.transform.params.size_h / tmp; m[0][1] = 0.0;
		tmp = font_em_v; m[1][1] = type.transform.params.size_v / tmp; m[1][0] = 0.0;
	}

	dtExtentDoTransform(engine, 2, &glyphbox_ext);

	glyphs_advance_flt = dtxGlyphsPosition(engine, &type, spacing, mode, tm, glyph_arr, ptr_glyph_displacements, ptr_glyph_advances, font_em_h, font_em_v, DV_NULL, glyph_arr_origin, &i_min, &i_max, &x, &y, start_advance);

	if (glyph_arr_origin[i_min].x < glyph_arr_origin[i_max].x)
	{
		clipbox1.xmn = DF_ROUND(x + glyph_arr_origin[i_min].x);
		clipbox1.xmx = DF_ROUND(x + glyph_arr_origin[i_max].x);
	}
	else
	{
		clipbox1.xmx = DF_ROUND(x + glyph_arr_origin[i_min].x);
		clipbox1.xmn = DF_ROUND(x + glyph_arr_origin[i_max].x);
	}

	if (glyph_arr_origin[i_min].y < glyph_arr_origin[i_max].y)
	{
		clipbox1.ymn = DF_ROUND(y + glyph_arr_origin[i_min].y);
		clipbox1.ymx = DF_ROUND(y + glyph_arr_origin[i_max].y);
	}
	else
	{
		clipbox1.ymx = DF_ROUND(y + glyph_arr_origin[i_min].y);
		clipbox1.ymn = DF_ROUND(y + glyph_arr_origin[i_max].y);
	}

	clipbox1.xmn += glyphbox_ext.xmn;
	clipbox1.xmx += glyphbox_ext.xmx;
	clipbox1.ymn -= glyphbox_ext.ymx;
	clipbox1.ymx -= glyphbox_ext.ymn;

	if (crop_type == DV_CROP_HMAX_VMAX_PADDED || crop_type == DV_CROP_HMAX_VMAX) clipbox = clipbox1;
	else
	{
		clipbox2.xmn = 0;
		clipbox2.xmx = 0;
		clipbox2.ymn = 0;
		clipbox2.ymx = 0;

		for (i = i_min; i <= i_max; i++)
		{
			dtGlyphGetMetrics(engine, type.font_index, glyph_arr[i], &glyph_advance, &glyphbox);

			glyphbox_ext.xmn = glyphbox.xmn;
			glyphbox_ext.ymn = glyphbox.ymn;
			glyphbox_ext.xmx = glyphbox.xmx;
			glyphbox_ext.ymx = glyphbox.ymx;

			dtExtentDoTransform(engine, 1, &glyphbox_ext);

			if (i == i_min)
			{
				clipbox2.xmn = DF_ROUND(x + glyph_arr_origin[i].x) + glyphbox_ext.xmn;
				clipbox2.xmx = DF_ROUND(x + glyph_arr_origin[i].x) + glyphbox_ext.xmx;
				clipbox2.ymn = DF_ROUND(y + glyph_arr_origin[i].y) - glyphbox_ext.ymx;
				clipbox2.ymx = DF_ROUND(y + glyph_arr_origin[i].y) - glyphbox_ext.ymn;
			}
			else
			{
				clipbox2.xmn = DF_MIN(clipbox2.xmn, DF_ROUND(x + glyph_arr_origin[i].x) + glyphbox_ext.xmn);
				clipbox2.xmx = DF_MAX(clipbox2.xmx, DF_ROUND(x + glyph_arr_origin[i].x) + glyphbox_ext.xmx);
				clipbox2.ymn = DF_MIN(clipbox2.ymn, DF_ROUND(y + glyph_arr_origin[i].y) - glyphbox_ext.ymx);
				clipbox2.ymx = DF_MAX(clipbox2.ymx, DF_ROUND(y + glyph_arr_origin[i].y) - glyphbox_ext.ymn);
			}
		}

		if (crop_type == DV_CROP_HMIN_VMAX)
		{
			clipbox.xmn = clipbox2.xmn;
			clipbox.xmx = clipbox2.xmx;
			clipbox.ymn = clipbox1.ymn;
			clipbox.ymx = clipbox1.ymx;
		}
		else if (crop_type == DV_CROP_HMAX_VMIN)
		{
			clipbox.xmn = clipbox1.xmn;
			clipbox.xmx = clipbox1.xmx;
			clipbox.ymn = clipbox2.ymn;
			clipbox.ymx = clipbox2.ymx;
		}
		else clipbox = clipbox2;
	}

	bmp_w = clipbox.xmx - clipbox.xmn;
	bmp_h = clipbox.ymx - clipbox.ymn;

	if (crop_type == DV_CROP_HMAX_VMAX_PADDED)
	{
		bmp_w >>= 2;
		bmp_w <<= 2;
		bmp_w += 4;
	}

	bmp->w = bmp_w;
	bmp->h = bmp_h;
	bmp->x = clipbox.xmn;
	bmp->y = -clipbox.ymn;

	return glyphs_advance_flt;
}


DT_FLOAT dtxCharsGetBound(DT_DTENGINE engine, DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[], DT_BMP* bmp)
{
	DT_TYPE_EFFECTS_L type;
	DT_ID_ULONG glyph_arr[DV_MAXLINELENGTH + 1];

	if (dtTypesetterGetTypeEffects(engine, &type, 0) != 1) return 0;
	dtxConvertCharsToGlyphs(engine, type.font_index, chars_arr, glyph_arr);
	return dtxGlyphsGetBound(engine, crop_type, x, y, spacing, mode, tm, glyph_arr, DV_NULL, DV_NULL, 0.0, bmp);
}


DT_FLOAT dtxCharsGetBound(DT_DTENGINE engine, DT_ID_UBYTE crop_type, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_CHAR chars_arr[], DT_BMP* bmp)
{
	DT_TYPE_EFFECTS_L type;
	DT_ID_ULONG glyph_arr[DV_MAXLINELENGTH + 1];

	if (dtTypesetterGetTypeEffects(engine, &type, 0) != 1) return 0;
	dtxConvertCharsToGlyphs(engine, type.font_index, chars_arr, glyph_arr);
	return dtxGlyphsGetBound(engine, crop_type, x, y, spacing, mode, tm, glyph_arr, DV_NULL, DV_NULL, 0.0, bmp);
}


DT_FLOAT dtxGlyphsDoOutput(DT_DTENGINE engine, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG glyph_arr[], const DT_SLONG glyph_arr_displacement[], const DT_SLONG glyph_arr_advance[], DT_FLOAT start_advance)
{
	DT_TM2X2 m;
	DT_TYPE_EFFECTS_L type;
	DT_SLONG font_em_h, font_em_v;
	DT_RECT_SLONG glyphbox_ext;

	DT_FLOAT glyphs_advance_flt;
	DT_FLOAT tmp;

	DT_SLONG i = 0, i_min, i_max;
	DT_SLONG glyph_displacements[DV_MAXLINELENGTH + 1];
	DT_SLONG glyph_advances[DV_MAXLINELENGTH + 1];
	DT_POINT_FLOAT glyph_arr_origin[DV_MAXLINELENGTH + 1];
	const DT_SLONG* ptr_glyph_displacements = glyph_displacements;
	const DT_SLONG* ptr_glyph_advances = glyph_advances;

	//DT_ID_SWORD vert_mode = mode & 128;
	DT_ID_SWORD kern_mode = mode & 127;

	DT_BMP glyph_bmp;

	if (dtTypesetterGetTypeEffects(engine, &type, 0) != 1) return 0; /* unknown error */
	if (type.descriptor != 0) /* transform is specified via matrix */
	{
		/* in this case we have some unsupported cases */
		if (tm == DV_NULL || kern_mode == DV_SPACING_DEVICE || kern_mode == DV_SPACING_KERN_DEVICE || kern_mode == DV_SPACING_DEVICE_2 || kern_mode == DV_SPACING_KERN_DEVICE_2) return 0;
		if (type.transform.params.size_h == 0 || type.transform.params.size_v == 0) return 0;
	}

	if (dtxGlyphsGetInfo(engine, mode, type.font_index, glyph_arr, glyph_displacements, glyph_advances, &font_em_h, &font_em_v, &glyphbox_ext) != 1) return 0;

	if (glyph_arr_displacement != DV_NULL) ptr_glyph_displacements = glyph_arr_displacement;
	if (glyph_arr_advance != DV_NULL) ptr_glyph_advances = glyph_arr_advance;

	if (tm == DV_NULL)
	{
		tm = m;
		tmp = font_em_h; m[0][0] = type.transform.params.size_h / tmp; m[0][1] = 0.0;
		tmp = font_em_v; m[1][1] = type.transform.params.size_v / tmp; m[1][0] = 0.0;
	}

	glyphs_advance_flt = dtxGlyphsPosition(engine, &type, spacing, mode, tm, glyph_arr, ptr_glyph_displacements, ptr_glyph_advances, font_em_h, font_em_v, DV_NULL, glyph_arr_origin, &i_min, &i_max, &x, &y, start_advance);

	if (i_max < i_min) return glyphs_advance_flt;

	for (i = i_min; i <= i_max; i++)
	{
		dtGlyphDoOutput(engine, glyph_arr[i], x + glyph_arr_origin[i].x, y + glyph_arr_origin[i].y, 0, &glyph_bmp);
	}

	return glyphs_advance_flt;
}


DT_FLOAT dtxCharsDoOutput(DT_DTENGINE engine, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[])
{
	DT_TYPE_EFFECTS_L type;
	DT_ID_ULONG glyph_arr[DV_MAXLINELENGTH + 1];

	if (dtTypesetterGetTypeEffects(engine, &type, 0) != 1) return 0;
	dtxConvertCharsToGlyphs(engine, type.font_index, chars_arr, glyph_arr);
	return dtxGlyphsDoOutput(engine, x, y, spacing, mode, tm, glyph_arr, DV_NULL, DV_NULL, 0.0);
}


DT_FLOAT dtxCharsDoOutput(DT_DTENGINE engine, DT_FLOAT x, DT_FLOAT y, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_CHAR chars_arr[])
{
	DT_TYPE_EFFECTS_L type;
	DT_ID_ULONG glyph_arr[DV_MAXLINELENGTH + 1];

	if (dtTypesetterGetTypeEffects(engine, &type, 0) != 1) return 0;
	dtxConvertCharsToGlyphs(engine, type.font_index, chars_arr, glyph_arr);
	return dtxGlyphsDoOutput(engine, x, y, spacing, mode, tm, glyph_arr, DV_NULL, DV_NULL, 0.0);
}


/*
 * dtxGlyphsDoOutput_Arc() - draws a line of glyphs in a circular arc.
 *
 * xc - X coordinate of the arc's center point, in pixels.
 * yc - Y coordinate of the arc's center point, in pixels
 * rc - Radius of the arc, in pixels. Must be positive.
 * alpha0 - Angle of the arc's starting point, in degrees. Must be between 0 and 360.
 * delta - Length of the arc, in degrees. Must be positive for clockwise direction.
 * effects - Typographic effects of the line of glyphs to be drawn.
 * glyph_arr[] - an array of glyphs, terminated with DV_ENDOFGLYPHARR.
 *
 * The radius of the arc is rc while its center point is located at coordinates
 * (xc, yc). The starting point of the arc is defined by the alpha0 angle and its
 * length by the delta angle. Therefore, the ending point of the arc is defined by
 * the alpha0+delta angle. When delta>0, the arc is moving in a clockwise direction.
 *
 * The line of glyphs is drawn glyph by glyph using the typographic attributes supplied
 * via effects. If delta<>0, the length of the arc is predefined, therefore the supplied
 * line of glyphs will need to be adjusted to ensure that the length of the arc and the
 * length of the line of glyphs (when drawn in this arc) are the same.
 */

void dtxGlyphsDoOutput_Arc(DT_DTENGINE engine, DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 /*tm*/, const DT_ID_ULONG glyph_arr[])
{
	DT_UWORD font_em_h, font_em_v;
	DT_SLONG glyph_advances[DV_MAXLINELENGTH + 1];
	DT_SWORD glyph_kernxs[DV_MAXLINELENGTH + 1];

	DT_SWORD kerny = 0;
	DT_SWORD rot;

	DT_FLOAT tmp;
	DT_FLOAT xr, yr;
	DT_FLOAT angle = DF_RAD(alpha0), theta;
	DT_FLOAT f, k, glyph_advance_flt, total_advance_flt;
	DT_SLONG i, l = 0, rc2 = 2 * rc, total_advance = 0;
	DT_UBYTE transform_params_mul_h, transform_params_mul_v;
	DT_UWORD glyph_advance = 0;

	//DT_ID_SWORD vert_mode = mode & 128;
	DT_ID_SWORD kern_mode = mode & 127;

	DT_TYPE_EFFECTS_L type;
	DT_BMP glyph_bmp;

	if (effects->descriptor != 0) return; /* transform is specified via matrix - not supported */

	type = *effects;

	/* Get font info */
	if (dtFontGetMetrics(engine, effects->font_index, &font_em_h, &font_em_v, DV_NULL) != 1) return;

	dtTypesetterGetSizeSubscale(engine, &transform_params_mul_h, &transform_params_mul_v);

	//font_em_h *= transform_params_mul_h;
	//font_em_v *= transform_params_mul_v;
	tmp = font_em_h * transform_params_mul_h;

	f = effects->transform.params.size_h / tmp;

	/* Determine the total length of our line of glyphs ... */
	while (l < DV_MAXLINELENGTH)
	{
		if (glyph_arr[l] == DV_ENDOFGLYPHARR) break;
		dtGlyphGetMetrics(engine, effects->font_index, glyph_arr[l], &glyph_advance, DV_NULL);
		glyph_advances[l] = glyph_advance;
		glyph_kernxs[l] = 0;
		if (kern_mode == DV_SPACING_KERN_FRAC) dtGlyphGetKern(engine, effects->font_index, glyph_arr[l], glyph_arr[l + 1], &glyph_kernxs[l], &kerny);
		total_advance += spacing + glyph_advances[l] + glyph_kernxs[l];
		l++;
	}

	total_advance_flt = total_advance * f;

	if (total_advance_flt <= 0.0001) return;

	if (delta != 0)
	{
		/* ... and, if delta is specified, adjust it */
		k = rc * DF_RAD(delta) / total_advance_flt;
		type.transform.params.size_h = DF_ROUND(k * effects->transform.params.size_h);
		type.transform.params.size_v = DF_ROUND(k * effects->transform.params.size_v);
		f *= k;
	}

	angle += 1.570796327;

	/* Draw arc */
	for (i = 0; i < l; i++)
	{
		xr = xc + rc * sin(angle);
		yr = yc - rc * cos(angle);

		glyph_advance_flt = f * (spacing + glyph_advances[i] + glyph_kernxs[i]);

		theta = glyph_advance_flt / rc2;
		rot = DF_DEG(angle + theta);
		rot >>= 2;
		rot <<= 2;

		type.transform.params.rotation = rot;
		dtTypesetterSetTypeEffects(engine, &type, 0);

		dtGlyphDoOutput(engine, glyph_arr[i], xr, yr, 0, &glyph_bmp);

		angle += 2 * theta;
	}
}


void dtxCharsDoOutput_Arc(DT_DTENGINE engine, DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_ID_ULONG chars_arr[])
{
	DT_ID_ULONG glyph_arr[DV_MAXLINELENGTH + 1];

	dtxConvertCharsToGlyphs(engine, effects->font_index, chars_arr, glyph_arr);
	dtxGlyphsDoOutput_Arc(engine, xc, yc, rc, alpha0, delta, effects, spacing, mode, tm, glyph_arr);
}


void dtxCharsDoOutput_Arc(DT_DTENGINE engine, DT_SLONG xc, DT_SLONG yc, DT_SLONG rc, DT_SWORD alpha0, DT_SWORD delta, const DT_TYPE_EFFECTS_L* effects, DT_SLONG spacing, DT_ID_SWORD mode, DT_TM2X2 tm, const DT_CHAR chars_arr[])
{
	DT_ID_ULONG glyph_arr[DV_MAXLINELENGTH + 1];

	dtxConvertCharsToGlyphs(engine, effects->font_index, chars_arr, glyph_arr);
	dtxGlyphsDoOutput_Arc(engine, xc, yc, rc, alpha0, delta, effects, spacing, mode, tm, glyph_arr);
}


void dtxTransformPerspective(const DT_TM3X3 tm, DT_SLONG x, DT_SLONG y, DT_SLONG n, const DT_SLONG x_arr_in[], const DT_SLONG y_arr_in[], DT_FLOAT x_arr_out[], DT_FLOAT y_arr_out[])
{
	DT_SLONG i;
	DT_SLONG xi, yi;
	DT_FLOAT wi;

	for (i = 0; i < n; i++)
	{
		xi = x + x_arr_in[i];
		yi = y + y_arr_in[i];
		wi = tm[2][0] * xi + tm[2][1] * yi + tm[2][2];
		if (DF_ISEQUAL(wi, 0.0)) continue;
		wi = 1.0 / wi;
		x_arr_out[i] = (tm[0][0] * xi + tm[0][1] * yi + tm[0][2]) * wi;
		y_arr_out[i] = (tm[1][0] * xi + tm[1][1] * yi + tm[1][2]) * wi;
	}
}

