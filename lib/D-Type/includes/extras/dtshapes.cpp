/*
 * dtshapes.h - auxiliary implementation file for D-Type Rasterizer to draw 2D graphics primitives
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#include "dtype.h"
#include "dtshapes.h"


#define DV_FLOAT_FOURBYTHREE 1.3333333333333
#define DV_FLOAT_TWOBYTHREE 0.6666666666666
#define DV_FLOAT_PI 3.141592654
#define DV_FLOAT_TWOPI 6.283185307
#define DV_FLOAT_BEZIERCIRCLE_CONST 0.5522847497 /* 4 * (SQRT(2) - 1) / 3 */

#define DF_SIN(a) sin((DT_FLOAT)(a))
#define DF_COS(a) cos((DT_FLOAT)(a))
#define DF_FABS(a) fabs((DT_FLOAT)(a))
#define DF_SQRT(a) sqrt((DT_FLOAT)(a))
#define DF_DIST(dx, dy) DF_SQRT((dx) * (dx) + (dy) * (dy))

#define DF_SEC_ADD(a, b) dtSecAdd(a, b)
#define DF_SEC_MUL(a, b) dtSecMul(a, b)

#ifndef DEF_MEMORY
#define DF_SHAPE_MEM_ALLOC(size) dtMemAlloc(size)
#define DF_SHAPE_MEM_REALLOC(ptr, size) dtMemRealloc(ptr, size)
#define DF_SHAPE_MEM_FREE(ptr) dtMemFree(ptr)
#endif

#define DF_SHAPE_MEMCPY(dst, src, size) dtMemCopy(dst, src, size)


typedef struct
{
	DT_UBYTE flg;
	DT_FLOAT xo, yo, xi, yi;

} DT_SHAPE_THICK_POINT;


typedef struct
{
	DT_SHAPE_THICK_POINT* a;
	DT_SLONG i;
	DT_SLONG limit;
	DT_SWORD flag;

} DT_SHAPE_THICK_POLYLINE;


/* Write DT_UBYTE to memory and advance memory pointer */
#define WINC_UBYTE(buffer, n) { (*((DT_UBYTE*)(buffer)) = (DT_UBYTE)(n)); (buffer) += sizeof(DT_UBYTE); }

/* Write DT_UWORD to memory and advance memory pointer */
#define WINC_UWORD(buffer, n) { DF_SHAPE_MEMCPY(buffer, &(n), sizeof(DT_UWORD)); (buffer) += sizeof(DT_UWORD); } /* { (*((DT_UWORD*)(buffer)) = (DT_UWORD)(n)); (buffer) += sizeof(DT_UWORD); } */

/* Write DT_FLOAT to memory and advance memory pointer */
#define WINC_FLOAT(buffer, n) { DF_SHAPE_MEMCPY(buffer, &(n), sizeof(DT_FLOAT)); (buffer) += sizeof(DT_FLOAT); } /* { (*((DT_FLOAT*)(buffer)) = (DT_FLOAT)(n)); (buffer) += sizeof(DT_FLOAT); } */

/* Write code for new contour (16) or new line (20) and advance memory pointer */
#define WINC_DELTA(buffer, n) { WINC_UBYTE(buffer, n); WINC_UBYTE(buffer, 0); }

/* Write code for new Bezier curve (24) or B-Spline curve (25) and advance memory pointer */
#define WINC_CURVE(buffer, n, seg) { WINC_UBYTE(buffer, n); WINC_UBYTE(buffer, 0); WINC_UWORD(buffer, seg); }

/* Write point (x, y) to memory and advance memory pointer */
#define WINC_POINT(buffer, x, y, tm) { dtsWriteTransformedPoints((buffer), (x), (y), tm); (buffer) += 2 * sizeof(DT_FLOAT); }

/* Write shape's boundary */
#define MAKE_BOUND(extent, x1, y1, x2, y2, err, tm) { dtsWriteTransformedBoundary((extent), DF_ROUND(x1), DF_ROUND(y1), DF_ROUND(x2), DF_ROUND(y2), DF_ROUND(err), tm); }


/* Function to write point (x, y) to memory */
void dtsWriteTransformedPoints(DT_UBYTE* buffer, DT_FLOAT x, DT_FLOAT y, const DT_TM2X2 tm)
{
	DT_FLOAT xt, yt;

	if (tm == DV_NULL)
	{
		WINC_FLOAT(buffer, x);
		WINC_FLOAT(buffer, y);
	}
	else
	{
		xt = x * tm[0][0] + y * tm[0][1]; WINC_FLOAT(buffer, xt);
		yt = x * tm[1][0] + y * tm[1][1]; WINC_FLOAT(buffer, yt);
	}
}


/* Function to write boundary to memory */
void dtsWriteTransformedBoundary(DT_RECT_SLONG* extent, DT_SLONG x1, DT_SLONG y1, DT_SLONG x2, DT_SLONG y2, DT_SLONG err, const DT_TM2X2 tm)
{
	DT_SLONG xmn, ymn, xmx, ymx;

	err += 2;
	if (tm == DV_NULL)
	{
		extent->xmn = DF_MIN(x1, x2) - err;
		extent->ymn = DF_MIN(y1, y2) - err;
		extent->xmx = DF_MAX(x1, x2) + err;
		extent->ymx = DF_MAX(y1, y2) + err;
	}
	else
	{
		if (x1 < x2) { x1 -= err; x2 += err; } else { x1 += err; x2 -= err; }
		if (y1 < y2) { y1 -= err; y2 += err; } else { y1 += err; y2 -= err; }

		xmn = DF_ROUND(x1 * tm[0][0]);
		xmx = DF_ROUND(x2 * tm[0][0]);
		ymn = DF_ROUND(y1 * tm[0][1]);
		ymx = DF_ROUND(y2 * tm[0][1]);
		extent->xmn = DF_MIN(xmn, xmx) + DF_MIN(ymn, ymx);
		extent->xmx = DF_MAX(xmn, xmx) + DF_MAX(ymn, ymx);

		xmn = DF_ROUND(x1 * tm[1][0]);
		xmx = DF_ROUND(x2 * tm[1][0]);
		ymn = DF_ROUND(y1 * tm[1][1]);
		ymx = DF_ROUND(y2 * tm[1][1]);
		extent->ymn = DF_MIN(xmn, xmx) + DF_MIN(ymn, ymx);
		extent->ymx = DF_MAX(xmn, xmx) + DF_MAX(ymn, ymx);
	}
}


/* Function to output shape and free its buffer */
DT_SWORD dtsShapeOut(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_UBYTE* buf, const DT_RECT_SLONG* extent)
{
	DT_BMP bmp;

	if (buf == DV_NULL) return 0;
	dtShapesDoOutput(engine, DV_NULL, DV_BITMAP_OFF, refx, refy, extent, DV_SHAPE_BUFFER, buf, DV_NULL, DV_NULL, &bmp);
	DF_SHAPE_MEM_FREE(buf);
	return 1;
}


DT_SWORD dtsShapeOut_NZW(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_UBYTE* buf, const DT_RECT_SLONG* extent)
{
	DT_UBYTE rule;
	DT_BMP bmp;

	if (buf == DV_NULL) return 0;
	dtRasterizerGetFillRule(engine, &rule);
	dtRasterizerSetFillRule(engine, DV_NZW_ON);
	dtShapesDoOutput(engine, DV_NULL, DV_BITMAP_OFF, refx, refy, extent, DV_SHAPE_BUFFER, buf, DV_NULL, DV_NULL, &bmp);
	dtRasterizerSetFillRule(engine, rule);
	DF_SHAPE_MEM_FREE(buf);
	return 1;
}


void dtsRelease(DT_UBYTE* buf)
{
	if (buf == DV_NULL) return;
	DF_SHAPE_MEM_FREE(buf);
}


/* Helper Routines */

void dtsThickPolylineInit(DT_SHAPE_THICK_POLYLINE* pl, DT_SWORD flag)
{
	pl->a = DV_NULL;
	pl->i = 0;
	pl->limit = 0;
	pl->flag = flag;
}


DT_SWORD dtsThickPolylineGrow(DT_SHAPE_THICK_POLYLINE* pl, DT_SLONG grow)
{
	DT_SLONG new_limit = grow + pl->limit;
	DT_SHAPE_THICK_POINT* a = (DT_SHAPE_THICK_POINT*)DF_SHAPE_MEM_REALLOC(pl->a, DF_SEC_MUL(new_limit, sizeof(DT_SHAPE_THICK_POINT)));

	if (a == DV_NULL) return 0;
	pl->a = a;
	pl->limit = new_limit;
	return 1;
}


void dtsThickPolylineFree(DT_SHAPE_THICK_POLYLINE* pl)
{
	if (pl->a != DV_NULL) DF_SHAPE_MEM_FREE(pl->a);
	dtsThickPolylineInit(pl, 0);
}


/* Function to thicken a line and copy the resulting coordinates to an inner and outer array */
DT_SWORD dtsThicken2(DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT facdx, DT_FLOAT facdy, DT_UBYTE flg, DT_SHAPE_THICK_POLYLINE* pl)
{
	DT_SLONG i = pl->i;

	if (i + 2 >= pl->limit)
	{
		if (dtsThickPolylineGrow(pl, 512) != 1) return 0;
	}

	pl->a[i].flg = flg;
	pl->a[i].xi = x - facdy; pl->a[i].yi = y + facdx;
	pl->a[i].xo = x + facdy; pl->a[i].yo = y - facdx;

	i++; x += dx; y += dy;

	pl->a[i].flg = flg;
	pl->a[i].xi = x - facdy; pl->a[i].yi = y + facdx;
	pl->a[i].xo = x + facdy; pl->a[i].yo = y - facdx;

	pl->i = pl->i + 2;

	return 1;
}


DT_SWORD dtsThicken(DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT t, DT_UBYTE flg, DT_SHAPE_THICK_POLYLINE* pl)
{
	DT_FLOAT l = DF_SQRT(dx * dx + dy * dy);
	if (l <= 0) return -1;

	DT_FLOAT k = 0.5 * t / l;
	DT_FLOAT facdx = k * dx;
	DT_FLOAT facdy = k * dy;

	return dtsThicken2(x, y, dx, dy, facdx, facdy, flg, pl);
}


void dtsDashes_Iterate(DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT ld, DT_FLOAT* ld_shift_ptr, DT_FLOAT t, DT_SHAPE_THICK_POLYLINE* pl)
{
	DT_SLONG i;
	DT_FLOAT l0, l1;
	DT_FLOAT u1, v1, u2, v2;
	DT_FLOAT l = DF_SQRT(dx * dx + dy * dy);
	if (l <= 0) return;

	DT_FLOAT t2 = 0.5 * t;
	DT_FLOAT kx = dx / l;
	DT_FLOAT ky = dy / l;
	DT_FLOAT facdx = t2 * kx;
	DT_FLOAT facdy = t2 * ky;

	DT_FLOAT start_len = *ld_shift_ptr;
	DT_SLONG start_idx = DF_FLOOR(start_len / ld);

	DT_FLOAT end_len = start_len + l;
	DT_SLONG end_idx = 1 + DF_FLOOR(end_len / ld);

	for (i = start_idx; i <= end_idx; i++)
	{
		l0 = i * ld - start_len; if (l0 > l) break;
		l1 = l0 + ld * 0.6666666666666; if (l1 < 0) continue;

		if (l0 < 0) l0 = 0;
		if (l1 > l) l1 = l;

		u1 = l0 * kx;
		v1 = l0 * ky;
		u2 = l1 * kx;
		v2 = l1 * ky;

		dtsThicken2(x + u1, y + v1, u2 - u1, v2 - v1, facdx, facdy, (DT_UBYTE)(i & 1), pl);
	}

	*ld_shift_ptr = end_len;
}


DT_SLONG dtsDashes_Adjust(DT_FLOAT* ld, DT_FLOAT l)
{
	DT_SLONG n;

	if ((l <= 0) || (*ld <= 1)) return 0; else if (*ld > l) *ld = l;
	n = DF_ROUND(l / (*ld));
	if (n <= 0) return 0;
	*ld = l / n;

	return n;
}


/* Function to make line's start or end style */
DT_UBYTE* dtsLineStyle_Make(DT_UBYTE* buf, DT_UBYTE type, DT_FLOAT dx, DT_FLOAT dy, DT_SLONG sc_t, DT_UBYTE closing, const DT_TM2X2 tm)
{
	if (type == DV_LINESTYLE_ROUND_OUTSIDE /*1*/)
	{
		/* Curve - outside */
		WINC_CURVE(buf, 24, sc_t);
		WINC_POINT(buf, DV_FLOAT_TWOBYTHREE * dy, -DV_FLOAT_TWOBYTHREE * dx, tm);
		WINC_POINT(buf, dx, dy, tm);
		WINC_POINT(buf, -DV_FLOAT_TWOBYTHREE * dy, DV_FLOAT_TWOBYTHREE * dx, tm);
	}
	else if (type == DV_LINESTYLE_ROUND_INSIDE /*2*/)
	{
		/* Curve - inside */
		WINC_CURVE(buf, 24, sc_t);
		WINC_POINT(buf, -DV_FLOAT_TWOBYTHREE * dy, DV_FLOAT_TWOBYTHREE * dx, tm);
		WINC_POINT(buf, dx, dy, tm);
		WINC_POINT(buf, DV_FLOAT_TWOBYTHREE * dy, -DV_FLOAT_TWOBYTHREE * dx, tm);
	}
	else if (type == DV_LINESTYLE_PEAK_OUTSIDE /*3*/)
	{
		/* Peak - outside */
		WINC_DELTA(buf, 20); WINC_POINT(buf, dy / 2 + dx / 2, dy / 2 - dx / 2, tm);
		WINC_DELTA(buf, 20); WINC_POINT(buf, dx / 2 - dy / 2, dx / 2 + dy / 2, tm);

	}
	else if (type == DV_LINESTYLE_PEAK_INSIDE /*4*/)
	{
		/* Peak - inside */
		WINC_DELTA(buf, 20); WINC_POINT(buf, dx / 2 - dy / 2, dx / 2 + dy / 2, tm);
		WINC_DELTA(buf, 20); WINC_POINT(buf, dy / 2 + dx / 2, dy / 2 - dx / 2, tm);

	}
	else if (type == DV_LINESTYLE_SQUARE_OUTSIDE /*5*/)
	{
		/* Curve - outside */
		WINC_DELTA(buf, 20); WINC_POINT(buf, dy / 2, -dx / 2, tm);
		WINC_DELTA(buf, 20); WINC_POINT(buf, dx, dy, tm);
		WINC_DELTA(buf, 20); WINC_POINT(buf, -dy / 2, dx / 2, tm);
	}
	else if (type == DV_LINESTYLE_SQUARE_INSIDE /*6*/)
	{
		/* Curve - inside */
		WINC_DELTA(buf, 20); WINC_POINT(buf, -dy / 2, dx / 2, tm);
		WINC_DELTA(buf, 20); WINC_POINT(buf, dx, dy, tm);
		WINC_DELTA(buf, 20); WINC_POINT(buf, dy / 2, -dx / 2, tm);
	}
	else if (closing == 0) /* DV_LINESTYLE_FLAT */ /* 0 */
	{
		/* Line */
		WINC_DELTA(buf, 20); WINC_POINT(buf, dx, dy, tm);
	}

	return buf;
}


/* Function to make a thick dashed polyline */
DT_UBYTE* dtsPolylineDashed_Build(DT_SHAPE_THICK_POLYLINE* pl, DT_UBYTE bgn, DT_UBYTE end, DT_SLONG sc_t, const DT_TM2X2 tm)
{
	DT_UBYTE end0 = end;
	DT_SLONG i, i0 = 0, i_cont = -1;
	DT_SLONG ld_flag = 0;
	DT_SLONG pts_per_step = 2; /* 2 x instruction 20 */
	DT_UBYTE* buf0;
	DT_UBYTE* buf;
	DT_SLONG j = pl->i;

	if (j < 2) return DV_NULL;

	if (pl->flag) pts_per_step += 2; /* instruction 16 + first dtsLineStyle_Make */
	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(DF_SEC_ADD(512, DF_SEC_MUL(j, 2 * pts_per_step * (sizeof(DT_UBYTE) + sizeof(DT_FLOAT)))))) == DV_NULL) return DV_NULL;
	buf = buf0;

	for (;;)
	{
		i_cont = -1;
		end0 = end;
		if (pl->flag) ld_flag = pl->a[i0].flg;

		/* Set point */
		WINC_DELTA(buf, 16);
		WINC_POINT(buf, pl->a[i0].xo, pl->a[i0].yo, tm);

		/* outer edge - go forward */
		for (i = i0 + 1; i < j; i++)
		{
			if ((pl->flag) && (pl->a[i].flg != ld_flag)) { i_cont = i; end0 = 0; break; }
			WINC_DELTA(buf, 20);
			WINC_POINT(buf, pl->a[i].xo - pl->a[i - 1].xo, pl->a[i].yo - pl->a[i - 1].yo, tm);
		}

		buf = dtsLineStyle_Make(buf, end0, pl->a[i - 1].xi - pl->a[i - 1].xo, pl->a[i - 1].yi - pl->a[i - 1].yo, sc_t, 0, tm);

		/* inner edge - go backward */
		for (i = i - 1; i > i0; i--)
		{
			WINC_DELTA(buf, 20);
			WINC_POINT(buf, pl->a[i - 1].xi - pl->a[i].xi, pl->a[i - 1].yi - pl->a[i].yi, tm);
		}

		buf = dtsLineStyle_Make(buf, bgn, pl->a[i0].xo - pl->a[i0].xi, pl->a[i0].yo - pl->a[i0].yi, sc_t, 1, tm);

		if (i_cont < 0) break;
		i0 = i_cont;
		bgn = 0;
	}

	/* End */
	WINC_UBYTE(buf, 8);

	return buf0;
}


/* Line, Curve and Arc Drawing Functions */


/*
 * Functions to draw a straight line:
 *
 * dtsLineDashed - for dashed or solid lines
 * dtsLine - only for solid lines, simplified version of dtsLineDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of line's first point, in float-point pixels
 *   dx, dy - line's dx and dy, in float-point pixels
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - line's thickness, in float-point pixels
 *   bgn, end - line's start and end style (0 = flat, 1 = round outside, 2 = round inside, 3 = peak outside, 4 = peak inside)
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_UBYTE* dtsLine_BuildStatic(DT_UBYTE* buf, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm, DT_RECT_SLONG* extent)
{
	DT_SLONG sc_t = 0;
	DT_FLOAT fac, facdx = 0, facdy = 0;

	if ((t <= 0) || (DF_ISEQUAL(dx, 0.0) && DF_ISEQUAL(dy, 0.0))) return DV_NULL;

	//MAKE_BOUND(extent, x, y, x + dx, y + dy, t / 2, tm);
	MAKE_BOUND(extent, x, y, x + dx, y + dy, 0.70710678 * t, tm);

	if ((fac = DF_SQRT(dx * dx + dy * dy)) <= 0) return DV_NULL;

	fac = t / fac;
	facdx = fac * dx;
	facdy = fac * dy;

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x - facdy / 2, y + facdx / 2, tm);

	buf = dtsLineStyle_Make(buf, bgn, facdy, -facdx, sc_t, 0, tm);

	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, dx, dy, tm);

	buf = dtsLineStyle_Make(buf, end, -facdy, facdx, sc_t, 0, tm);

	/* End */
	WINC_UBYTE(buf, 8);

	return buf;
}


DT_SWORD dtsLine(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	DT_UBYTE buf0[512];
	DT_BMP bmp;

	if (dtsLine_BuildStatic(buf0, x, y, dx, dy, t, bgn, end, tm, &extent) == DV_NULL) return 0;
	dtShapesDoOutput(engine, DV_NULL, DV_BITMAP_OFF, refx, refy, &extent, DV_SHAPE_BUFFER, buf0, DV_NULL, DV_NULL, &bmp);
	return 1;
}


DT_SWORD dtsLineDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	DT_SHAPE_THICK_POLYLINE pl;
	DT_UBYTE* buf;

	DT_RECT_SLONG extent;

	DT_FLOAT ld_shift0 = 0;
	DT_FLOAT* ld_shift_ptr = &ld_shift0;

	if (ld_shift != DV_NULL) ld_shift_ptr = ld_shift;

	if ((t <= 0) || (DF_ISEQUAL(dx, 0.0) && DF_ISEQUAL(dy, 0.0))) return 0;

	if (ld < 1) return dtsLine(engine, refx, refy, x, y, dx, dy, t, bgn, end, tm);

	dtsThickPolylineInit(&pl, 1);
	dtsDashes_Iterate(x, y, dx, dy, ld, ld_shift_ptr, t, &pl);
	buf = dtsPolylineDashed_Build(&pl, bgn, end, 0, tm);
	dtsThickPolylineFree(&pl);

	MAKE_BOUND(&extent, x, y, x + dx, y + dy, t, tm);
	return dtsShapeOut(engine, refx, refy, buf, &extent);
}


/*
 * Functions to draw a B-Spline curve:
 *
 * dtsBSplineCurveDashed - for dashed or solid curves
 * dtsBSplineCurve - only for solid curves, simplified version of dtsBSplineCurveDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of curve's first point, in float-point pixels
 *   dx1, dy1, dx2, dy2 - curve's dx1, dy1, dx2, dy2, in float-point pixels
 *   n - number of curve segments
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - curve's thickness, in float-point pixels
 *   bgn, end - curve's start and end style (0 = flat, 1 = round outside, 2 = round inside, 3 = peak outside, 4 = peak inside)
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_SLONG dtsBSplineCurve_Dist(DT_FLOAT x0, DT_FLOAT y0, DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_SLONG n_max, const DT_TM2X2 tm)
{
	DT_FLOAT xt0 = x0, xt1 = x1, xt2 = x2;
	DT_FLOAT yt0 = y0, yt1 = y1, yt2 = y2;

	if (tm != DV_NULL)
	{
		xt0 = x0 * tm[0][0] + y0 * tm[0][1]; yt0 = x0 * tm[1][0] + y0 * tm[1][1];
		xt1 = x1 * tm[0][0] + y1 * tm[0][1]; yt1 = x1 * tm[1][0] + y1 * tm[1][1];
		xt2 = x2 * tm[0][0] + y2 * tm[0][1]; yt2 = x2 * tm[1][0] + y2 * tm[1][1];
	}

	DT_FLOAT dx1 = xt1 - xt0, dx2 = xt2 - xt1;
	DT_FLOAT dy1 = yt1 - yt0, dy2 = yt2 - yt1;
	DT_FLOAT f = DF_DIST(dx1, dy1) + DF_DIST(dx2, dy2);
	DT_SLONG l = DF_ROUND(f);
	DT_SLONG d, n_min = 3, n = n_min;

	if (n_max <= 0) n += (l / 8); /* no limit */ else { d = n_max - n_min; n += ((l * d) / (l + 8 * d)); } /* limit is n_max as l approaches infinity */

	return n;
}


void dtsBSplineCurve_Iterate(DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_SHAPE_THICK_POLYLINE* pl, DT_SLONG n, const DT_TM2X2 tm)
{
	DT_SLONG n2, ni;
	DT_FLOAT vx, bx;
	DT_FLOAT vy, by;
	DT_FLOAT dx3 = dx1 + dx2;
	DT_FLOAT dy3 = dy1 + dy2;
	DT_FLOAT xp = x, ddx1, ddx2;
	DT_FLOAT yp = y, ddy1, ddy2;

	DT_FLOAT ld_shift0 = 0;
	DT_FLOAT* ld_shift_ptr = &ld_shift0;

	if (ld_shift != DV_NULL) ld_shift_ptr = ld_shift;

	if (n < 1) n = dtsBSplineCurve_Dist(x, y, x + dx1, y + dy1, x + dx3, y + dy3, 500, tm);
	if (n > 500) n = 500;

	n2 = n * n;
	vx = 2 * dx1; bx = dx3 - vx;
	vy = 2 * dy1; by = dy3 - vy;

	ddx1 = (bx + n * vx) / (DT_FLOAT)n2; ddx2 = (2 * bx) / (DT_FLOAT)n2;
	ddy1 = (by + n * vy) / (DT_FLOAT)n2; ddy2 = (2 * by) / (DT_FLOAT)n2;

	if (pl->flag)
	{
		for (ni = 1; ni < n; ni++)
		{
			dtsDashes_Iterate(xp, yp, ddx1, ddy1, ld, ld_shift_ptr, t, pl);
			xp += ddx1; ddx1 += ddx2;
			yp += ddy1; ddy1 += ddy2;
		}

		/* dtsDashes_Iterate(xp, yp, ddx1, ddy1, ld, ld_shift_ptr, t, pl); */
		dtsDashes_Iterate(xp, yp, x + dx3 - xp, y + dy3 - yp, ld, ld_shift_ptr, t, pl);
		return;
	}

	for (ni = 1; ni < n; ni++)
	{
		dtsThicken(xp, yp, ddx1, ddy1, t, 0, pl);
		xp += ddx1; ddx1 += ddx2;
		yp += ddy1; ddy1 += ddy2;
	}

	/* dtsThicken(xp, yp, ddx1, ddy1, t, 0, pl); */
	dtsThicken(xp, yp, x + dx3 - xp, y + dy3 - yp, t, 0, pl);
}


DT_SWORD dtsBSplineCurveDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	if (t <= 0) return 0;

	DT_SHAPE_THICK_POLYLINE pl;
	DT_RECT_SLONG extent;
	DT_UBYTE* buf;

	DT_FLOAT X0 = DF_MIN(DF_MIN(x + dx1 + dx2, x + dx1), x);
	DT_FLOAT Y0 = DF_MIN(DF_MIN(y + dy1 + dy2, y + dy1), y);
	DT_FLOAT X1 = DF_MAX(DF_MAX(x + dx1 + dx2, x + dx1), x);
	DT_FLOAT Y1 = DF_MAX(DF_MAX(y + dy1 + dy2, y + dy1), y);

	dtsThickPolylineInit(&pl, ld >= 1);
	dtsBSplineCurve_Iterate(x, y, dx1, dy1, dx2, dy2, ld, ld_shift, t, &pl, n, tm);
	buf = dtsPolylineDashed_Build(&pl, bgn, end, 0, tm);
	dtsThickPolylineFree(&pl);

	MAKE_BOUND(&extent, X0, Y0, X1, Y1, t, tm);
	return dtsShapeOut_NZW(engine, refx, refy, buf, &extent);
}


DT_SWORD dtsBSplineCurve(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	return dtsBSplineCurveDashed(engine, refx, refy, x, y, dx1, dy1, dx2, dy2, n, -1, DV_NULL, t, bgn, end, tm);
}


/*
 * Functions to draw a Bezier curve:
 *
 * dtsBezierCurveDashed - for dashed or solid curves
 * dtsBezierCurve - only for solid curves, simplified version of dtsBezierCurveDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of curve's first point, in float-point pixels
 *   dx1, dy1, dx2, dy2, dx3, dy3 - curve's dx1, dy1, dx2, dy2, dx3, dy3, in float-point pixels
 *   n - number of curve segments
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - curve's thickness, in float-point pixels
 *   bgn, end - curve's start and end style (0 = flat, 1 = round outside, 2 = round inside, 3 = peak outside, 4 = peak inside)
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_SLONG dtsBezierCurve_Dist(DT_FLOAT x0, DT_FLOAT y0, DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_FLOAT x3, DT_FLOAT y3, DT_SLONG n_max, const DT_TM2X2 tm)
{
	DT_FLOAT xt0 = x0, xt1 = x1, xt2 = x2, xt3 = x3;
	DT_FLOAT yt0 = y0, yt1 = y1, yt2 = y2, yt3 = y3;

	if (tm != DV_NULL)
	{
		xt0 = x0 * tm[0][0] + y0 * tm[0][1]; yt0 = x0 * tm[1][0] + y0 * tm[1][1];
		xt1 = x1 * tm[0][0] + y1 * tm[0][1]; yt1 = x1 * tm[1][0] + y1 * tm[1][1];
		xt2 = x2 * tm[0][0] + y2 * tm[0][1]; yt2 = x2 * tm[1][0] + y2 * tm[1][1];
		xt3 = x3 * tm[0][0] + y3 * tm[0][1]; yt3 = x3 * tm[1][0] + y3 * tm[1][1];
	}

	DT_FLOAT dx1 = xt1 - xt0, dx2 = xt2 - xt1, dx3 = xt3 - xt2;
	DT_FLOAT dy1 = yt1 - yt0, dy2 = yt2 - yt1, dy3 = yt3 - yt2;
	DT_FLOAT f = DF_DIST(dx1, dy1) + DF_DIST(dx2, dy2) + DF_DIST(dx3, dy3);
	DT_SLONG l = DF_ROUND(f);
	DT_SLONG d, n_min = 6, n = n_min;

	if (n_max <= 0) n += (l / 4); /* no limit */ else { d = n_max - n_min; n += ((l * d) / (l + 4 * d)); } /* limit is n_max as l approaches infinity */

	return n;
}


void dtsBezierCurve_Iterate(DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_FLOAT dx3, DT_FLOAT dy3, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_SHAPE_THICK_POLYLINE* pl, DT_SLONG n, const DT_TM2X2 tm)
{
	DT_SLONG n2, n3, ni;
	DT_FLOAT ux, vx, ax, bx, cx;
	DT_FLOAT uy, vy, ay, by, cy;
	DT_FLOAT dx4 = dx1 + dx2 + dx3;
	DT_FLOAT dy4 = dy1 + dy2 + dy3;
	DT_FLOAT xp = x, ddx1, ddx2, ddx3;
	DT_FLOAT yp = y, ddy1, ddy2, ddy3;

	DT_FLOAT ld_shift0 = 0;
	DT_FLOAT* ld_shift_ptr = &ld_shift0;

	if (ld_shift != DV_NULL) ld_shift_ptr = ld_shift;

	if (n < 1) n = dtsBezierCurve_Dist(x, y, x + dx1, y + dy1, x + dx1 + dx2, y + dy1 + dy2, x + dx4, y + dy4, 500, tm);
	if (n > 500) n = 500;

	n2 = n * n; n3 = n2 * n;
	ux = 3 * dx1; vx = 3 * dx2; ax = n * (vx - ux); bx = dx4 - vx; cx = 6 * bx;
	uy = 3 * dy1; vy = 3 * dy2; ay = n * (vy - uy); by = dy4 - vy; cy = 6 * by;

	ddx1 = (ax + bx + n2 * ux) / (DT_FLOAT)n3; ddx2 = (cx + 2 * ax) / (DT_FLOAT)n3; ddx3 = cx / (DT_FLOAT)n3;
	ddy1 = (ay + by + n2 * uy) / (DT_FLOAT)n3; ddy2 = (cy + 2 * ay) / (DT_FLOAT)n3; ddy3 = cy / (DT_FLOAT)n3;

	if (pl->flag)
	{
		for (ni = 1; ni < n; ni++)
		{
			dtsDashes_Iterate(xp, yp, ddx1, ddy1, ld, ld_shift_ptr, t, pl);
			xp += ddx1; ddx1 += ddx2; ddx2 += ddx3;
			yp += ddy1; ddy1 += ddy2; ddy2 += ddy3;
		}

		/* dtsDashes_Iterate(xp, yp, ddx1, ddy1, ld, ld_shift_ptr, t, pl); */
		dtsDashes_Iterate(xp, yp, x + dx4 - xp, y + dy4 - yp, ld, ld_shift_ptr, t, pl);
		return;
	}

	for (ni = 1; ni < n; ni++)
	{
		dtsThicken(xp, yp, ddx1, ddy1, t, 0, pl);
		xp += ddx1; ddx1 += ddx2; ddx2 += ddx3;
		yp += ddy1; ddy1 += ddy2; ddy2 += ddy3;
	}

	/* dtsThicken(xp, yp, ddx1, ddy1, t, 0, pl); */
	dtsThicken(xp, yp, x + dx4 - xp, y + dy4 - yp, t, 0, pl);
}


DT_SWORD dtsBezierCurveDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_FLOAT dx3, DT_FLOAT dy3, DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	if (t <= 0) return 0;

	DT_SHAPE_THICK_POLYLINE pl;
	DT_RECT_SLONG extent;
	DT_UBYTE* buf;

	DT_FLOAT X0 = DF_MIN(DF_MIN(DF_MIN(x + dx1 + dx2 + dx3, x + dx1 + dx2), x + dx1), x);
	DT_FLOAT Y0 = DF_MIN(DF_MIN(DF_MIN(y + dy1 + dy2 + dy3, y + dy1 + dy2), y + dy1), y);
	DT_FLOAT X1 = DF_MAX(DF_MAX(DF_MAX(x + dx1 + dx2 + dx3, x + dx1 + dx2), x + dx1), x);
	DT_FLOAT Y1 = DF_MAX(DF_MAX(DF_MAX(y + dy1 + dy2 + dy3, y + dy1 + dy2), y + dy1), y);

	dtsThickPolylineInit(&pl, ld >= 1);
	dtsBezierCurve_Iterate(x, y, dx1, dy1, dx2, dy2, dx3, dy3, ld, ld_shift, t, &pl, n, tm);
	buf = dtsPolylineDashed_Build(&pl, bgn, end, 0, tm);
	dtsThickPolylineFree(&pl);

	MAKE_BOUND(&extent, X0, Y0, X1, Y1, t, tm);
	return dtsShapeOut_NZW(engine, refx, refy, buf, &extent);
}


DT_SWORD dtsBezierCurve(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_FLOAT dx3, DT_FLOAT dy3, DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	return dtsBezierCurveDashed(engine, refx, refy, x, y, dx1, dy1, dx2, dy2, dx3, dy3, n, -1, DV_NULL, t, bgn, end, tm);
}


/*
 * Functions to draw a circular arc:
 *
 * dtsArcDashed - for dashed or solid arcs
 * dtsArc - only for solid arcs, simplified version of dtsArcDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of arc's center point, in float-point pixels
 *   rx, ry - arc's horizontal and vertical radius, in float-point pixels
 *   alpha0 - arc's starting angle in degrees
 *   theta - arc's length in degrees
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - arc's thickness, in float-point pixels
 *   bgn, end - arc's start and end style (0 = flat, 1 = round outside, 2 = round inside, 3 = peak outside, 4 = peak inside)
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_SLONG dtsArc_Dist(DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_SLONG n_max, const DT_TM2X2 tm)
{
	DT_FLOAT x0 = x - rx, y0 = y, xt0 = x0, yt0 = y0;
	DT_FLOAT x1 = x, y1 = y + ry, xt1 = x1, yt1 = y1;
	DT_FLOAT x2 = x + rx, y2 = y, xt2 = x2, yt2 = y2;
	DT_FLOAT x3 = x, y3 = y - ry, xt3 = x3, yt3 = y3;

	if (tm != DV_NULL)
	{
		xt0 = x0 * tm[0][0] + y0 * tm[0][1]; yt0 = x0 * tm[1][0] + y0 * tm[1][1];
		xt1 = x1 * tm[0][0] + y1 * tm[0][1]; yt1 = x1 * tm[1][0] + y1 * tm[1][1];
		xt2 = x2 * tm[0][0] + y2 * tm[0][1]; yt2 = x2 * tm[1][0] + y2 * tm[1][1];
		xt3 = x3 * tm[0][0] + y3 * tm[0][1]; yt3 = x3 * tm[1][0] + y3 * tm[1][1];
	}

	DT_FLOAT dx1 = xt1 - xt0, dx2 = xt2 - xt1, dx3 = xt3 - xt2;
	DT_FLOAT dy1 = yt1 - yt0, dy2 = yt2 - yt1, dy3 = yt3 - yt2;
	DT_FLOAT f = DF_DIST(dx1, dy1) + DF_DIST(dx2, dy2) + DF_DIST(dx3, dy3);
	DT_SLONG l = DF_ROUND(f);
	DT_SLONG d, n_min = 6, n = n_min;

	if (n_max <= 0) n += (l / 4); /* no limit */ else { d = n_max - n_min; n += ((l * d) / (l + 4 * d)); } /* limit is n_max as l approaches infinity */

	return n;
}


void dtsArc_Iterate(DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_SHAPE_THICK_POLYLINE* pl, DT_SLONG n, const DT_TM2X2 tm)
{
	DT_SLONG i, i0, i1;
	DT_FLOAT f, j, xc, yc, x0, y0;
	DT_UWORD alpha1;

	DT_FLOAT ld_shift0 = 0;
	DT_FLOAT* ld_shift_ptr = &ld_shift0;
	if (ld_shift != DV_NULL) ld_shift_ptr = ld_shift;

	if (n < 1) n = dtsArc_Dist(x, y, rx, ry, 500, tm);

	alpha1 = (DT_UWORD)(alpha0 + theta);
	i0 = (alpha0 * n) / 360;
	i1 = (alpha1 * n) / 360;
	f = DV_FLOAT_TWOPI / (DT_FLOAT)n;

	j = DF_RAD(alpha0);
	xc = rx * DF_COS(j);
	yc = ry * DF_SIN(j);

	x0 = xc;
	y0 = yc;
	i0++;

	if (pl->flag)
	{
		for (i = i0; i <= i1; i++)
		{
			j = i * f;
			xc = rx * DF_COS(j);
			yc = ry * DF_SIN(j);

			dtsDashes_Iterate(x + x0, y + y0, xc - x0, yc - y0, ld, ld_shift_ptr, t, pl);
			x0 = xc;
			y0 = yc;
		}

		j = DF_RAD(alpha1);
		xc = rx * DF_COS(j);
		yc = ry * DF_SIN(j);
		dtsDashes_Iterate(x + x0, y + y0, xc - x0, yc - y0, ld, ld_shift_ptr, t, pl);
		return;
	}

	for (i = i0; i <= i1; i++)
	{
		j = i * f;
		xc = rx * DF_COS(j);
		yc = ry * DF_SIN(j);

		dtsThicken(x + x0, y + y0, xc - x0, yc - y0, t, 0, pl);
		x0 = xc;
		y0 = yc;
	}

	j = DF_RAD(alpha1);
	xc = rx * DF_COS(j);
	yc = ry * DF_SIN(j);
	dtsThicken(x + x0, y + y0, xc - x0, yc - y0, t, 0, pl);
}


DT_SWORD dtsArcDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	DT_SHAPE_THICK_POLYLINE pl;
	DT_RECT_SLONG extent;
	DT_UBYTE* buf;

	if (rx <= 0 || ry <= 0 || t <= 0) return 0;

	alpha0 = (DT_UWORD)(alpha0 % 360);
	if (theta > 360) theta = 360;

	dtsThickPolylineInit(&pl, ld >= 1);
	dtsArc_Iterate(x, y, rx - t / 2, ry - t / 2, alpha0, theta, ld, ld_shift, t, &pl, 0, tm);
	buf = dtsPolylineDashed_Build(&pl, bgn, end, 0, tm);
	dtsThickPolylineFree(&pl);

	MAKE_BOUND(&extent, x - rx, y - ry, x + rx, y + ry, 0 /*t*/, tm);
	return dtsShapeOut_NZW(engine, refx, refy, buf, &extent);
}


DT_SWORD dtsArc(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	return dtsArcDashed(engine, refx, refy, x, y, rx, ry, alpha0, theta, -1, DV_NULL, t, bgn, end, tm);
}


/*
 * Functions to draw a slice (a shape commonly used in pie-charts):
 *
 * dtsSliceFilled - for filled slices
 * dtsSliceDashed - for dashed or solid slice outlines
 * dtsSlice - only for solid slice outlines, simplified version of dtsSliceDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of slice's center point, in float-point pixels
 *   rx, ry - slice's horizontal and vertical radius, in float-point pixels
 *   alpha0 - slice's starting angle in degrees
 *   theta - length of slice's arc in degrees
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - thickness of slice's outline, in float-point pixels
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_UBYTE* dtsSliceFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, const DT_TM2X2 tm, DT_RECT_SLONG* extent)
{
	DT_UBYTE *buf, *buf0;
	DT_SLONG n;
	DT_SLONG nmax;

	DT_SLONG i, i0, i1;
	DT_FLOAT f, j, xc, yc, x0, y0;
	DT_UWORD alpha1;

	if (rx <= 0 || ry <= 0) return DV_NULL;

	alpha0 = (DT_UWORD)(alpha0 % 360);
	if (theta > 360) theta = 360;

	nmax = dtsArc_Dist(x, y, rx, ry, 500, tm);

	alpha1 = (DT_UWORD)(alpha0 + theta);
	i0 = (alpha0 * nmax) / 360;
	i1 = (alpha1 * nmax) / 360;
	n = i1 - i0;
	f = DV_FLOAT_TWOPI / (DT_FLOAT)nmax;

	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(DF_SEC_ADD(512, DF_SEC_MUL(n, 2 * (sizeof(DT_UBYTE) + sizeof(DT_FLOAT)))))) == DV_NULL) return DV_NULL;
	buf = buf0;

	MAKE_BOUND(extent, x - rx, y - ry, x + rx, y + ry, 0, tm);

	/* Set point */
	x0 = 0;
	y0 = 0;
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + x0, y + y0, tm);

	j = DF_RAD(alpha0);
	xc = rx * DF_COS(j);
	yc = ry * DF_SIN(j);
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, xc - x0, yc - y0, tm);
	x0 = xc;
	y0 = yc;
	i0++;

	for (i = i0; i <= i1; i++)
	{
		j = i*f;
		xc = rx * DF_COS(j);
		yc = ry * DF_SIN(j);
		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, xc - x0, yc - y0, tm);
		x0 = xc;
		y0 = yc;
	}

	j = DF_RAD(alpha1);
	xc = rx * DF_COS(j);
	yc = ry * DF_SIN(j);
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, xc - x0, yc - y0, tm);

	/* End */
	WINC_UBYTE(buf, 8);

	return buf0;
}


DT_SWORD dtsSliceFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	return dtsShapeOut(engine, refx, refy, dtsSliceFilled_Build(x, y, rx, ry, alpha0, theta, tm, &extent), &extent);
}


DT_SWORD dtsSliceDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm)
{
	DT_FLOAT j, k;
	DT_FLOAT rxc, ryc;
	DT_FLOAT dx, dy;

	DT_FLOAT ld_shift0 = 0;
	DT_FLOAT* ld_shift_ptr = &ld_shift0;

	if (ld_shift != DV_NULL) ld_shift_ptr = ld_shift;

	if (rx <= 0 || ry <= 0 || t <= 0) return 0;

	k = t / 2;
	rxc = rx - k;
	ryc = ry - k;

	j = DF_RAD(alpha0);
	dx = rxc * DF_COS(j);
	dy = ryc * DF_SIN(j);

	if (dtsLineDashed(engine, refx, refy, x, y, dx, dy, ld, ld_shift_ptr, t, 1, 1, tm) != 1) return 0;

	if (theta > 360) theta = 360;
	j = DF_RAD(alpha0 + theta);
	dx = rxc * DF_COS(j);
	dy = ryc * DF_SIN(j);

	if (dtsArcDashed(engine, refx, refy, x, y, rx, ry, alpha0, theta, ld, ld_shift_ptr, t, 1, 1, tm) != 1) return 0;
	return dtsLineDashed(engine, refx, refy, x + dx, y + dy, -dx, -dy, ld, ld_shift_ptr, t, 1, 1, tm);
}


DT_SWORD dtsSlice(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT t, const DT_TM2X2 tm)
{
	return dtsSliceDashed(engine, refx, refy, x, y, rx, ry, alpha0, theta, -1, DV_NULL, t, tm);
}


/*
 * Functions to draw a true mathematical circle or ellipse:
 *
 * dtsTrueEllipseFilled - for filled ellipses
 * dtsTrueEllipseDashed - for dashed or solid ellipse outlines
 * dtsTrueEllipse - only for solid ellipse outlines, simplified version of dtsTrueEllipseDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of ellipse's center point, in float-point pixels
 *   rx, ry - ellipse's horizontal and vertical radius, in float-point pixels
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - thickness of ellipse's outline, in float-point pixels
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_UBYTE* dtsTrueEllipseFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, const DT_TM2X2 tm, DT_RECT_SLONG* extent)
{
	DT_UBYTE *buf, *buf0;
	DT_SLONG i;
	DT_FLOAT j, xc, yc, x0, y0;
	DT_SLONG n = dtsArc_Dist(x, y, rx, ry, 500, tm);
	DT_FLOAT f = DV_FLOAT_TWOPI / (DT_FLOAT)n;

	if (rx <= 0 || ry <= 0) return DV_NULL;

	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(DF_SEC_ADD(512, DF_SEC_MUL(n, 2 * (sizeof(DT_UBYTE) + sizeof(DT_FLOAT)))))) == DV_NULL) return DV_NULL;
	buf = buf0;

	MAKE_BOUND(extent, x - rx, y - ry, x + rx, y + ry, 0, tm);

	/* Set point */
	x0 = 0;
	y0 = ry;
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + x0, y + y0, tm);

	for (i = 0; i < n; i++)
	{
		j = i * f;
		xc = rx * DF_SIN(j);
		yc = ry * DF_COS(j);
		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, xc - x0, yc - y0, tm);
		x0 = xc;
		y0 = yc;
	}

	/* End */
	WINC_UBYTE(buf, 8);

	return buf0;
}


DT_SWORD dtsTrueEllipseFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	return dtsShapeOut(engine, refx, refy, dtsTrueEllipseFilled_Build(x, y, rx, ry, tm, &extent), &extent);
}


DT_SWORD dtsTrueEllipse(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_FLOAT t, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	DT_UBYTE *buf0, *buf;
	DT_SLONG i;
	DT_FLOAT j, xc, yc, x0, y0;
	DT_SLONG n = dtsArc_Dist(x, y, rx, ry, 500, tm);
	DT_FLOAT f = DV_FLOAT_TWOPI / (DT_FLOAT)n;
	DT_FLOAT rx2, ry2;
	DT_BMP bmp;

	if (rx <= 0 || ry <= 0 || t <= 0) return 0;

	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(DF_SEC_ADD(512, DF_SEC_MUL(n, 4 * (sizeof(DT_UBYTE) + sizeof(DT_FLOAT)))))) == DV_NULL) return 0;
	buf = buf0;

	MAKE_BOUND(&extent, x - rx, y - ry, x + rx, y + ry, 0, tm);

	/* Set point */
	x0 = 0;
	y0 = ry;
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + x0, y + y0, tm);

	for (i = 0; i < n; i++)
	{
		j = i * f;
		xc = rx * DF_SIN(j);
		yc = ry * DF_COS(j);
		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, xc - x0, yc - y0, tm);
		x0 = xc;
		y0 = yc;
	}

	rx2 = rx - t;
	ry2 = ry - t;

	x0 = 0;
	y0 = ry2;
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + x0, y + y0, tm);

	for (i = n - 1; i >= 0; i--)
	{
		j = i * f;
		xc = rx2 * DF_SIN(j);
		yc = ry2 * DF_COS(j);
		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, xc - x0, yc - y0, tm);
		x0 = xc;
		y0 = yc;
	}

	/* End */
	WINC_UBYTE(buf, 8);

	dtShapesDoOutput(engine, DV_NULL, DV_BITMAP_OFF, refx, refy, &extent, DV_SHAPE_BUFFER, buf0, DV_NULL, DV_NULL, &bmp);
	DF_SHAPE_MEM_FREE(buf0);

	return 1;
}


DT_SWORD dtsTrueEllipseDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm)
{
	if (rx <= 0 || ry <= 0 || t <= 0) return 0;
	if (ld < 1) return dtsTrueEllipse(engine, refx, refy, x, y, rx, ry, t, tm);

	DT_FLOAT a = rx - t / 2, b = ry - t / 2;
	dtsDashes_Adjust(&ld, DV_FLOAT_PI * (3 * (a + b) - DF_SQRT((3 * a + b) * (a + 3 * b)))); /* Ramanujan's approximative method to calculate circumference of an ellipse */

	return dtsArcDashed(engine, refx, refy, x, y, rx, ry, 0, 360, ld, ld_shift, t, 0, 0, tm);
}


/*
 * Functions to draw a circle or ellipse (approximative method using Bezier curves)
 *
 * dtsEllipseFilled - for filled ellipses
 * dtsEllipseDashed - for dashed or solid ellipse outlines
 * dtsEllipse - only for solid ellipse outlines, simplified version of dtsEllipseDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of ellipse's center point, in float-point pixels
 *   w, h - ellipse's bounding box, in float-point pixels
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - thickness of ellipse's outline, in float-point pixels
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_UBYTE* dtsEllipseFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, const DT_TM2X2 tm, DT_RECT_SLONG* extent)
{
	DT_UBYTE *buf, *buf0;
	DT_SLONG n = 0;
	DT_FLOAT rx, ry;
	DT_FLOAT hx, hy;

	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(512)) == DV_NULL) return DV_NULL;
	buf = buf0;

	MAKE_BOUND(extent, x, y, x + w, y + h, 0, tm);

	rx = w / 2; hx = rx * DV_FLOAT_BEZIERCIRCLE_CONST;
	ry = h / 2; hy = ry * DV_FLOAT_BEZIERCIRCLE_CONST;

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x, y + ry, tm);

	/* Curve - outer 1 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, 0, -hy, tm);
	WINC_POINT(buf, rx - hx, hy - ry, tm);
	WINC_POINT(buf, hx, 0, tm);

	/* Curve - outer 2 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, hx, 0, tm);
	WINC_POINT(buf, rx - hx, ry - hy, tm);
	WINC_POINT(buf, 0, hy, tm);

	/* Curve - outer 3 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, 0, hy, tm);
	WINC_POINT(buf, hx - rx, ry - hy, tm);
	WINC_POINT(buf, -hx, 0, tm);

	/* Curve - outer 4 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, -hx, 0, tm);
	WINC_POINT(buf, hx - rx, hy - ry, tm);
	WINC_POINT(buf, 0, -hy, tm);

	/* End */
	WINC_UBYTE(buf, 8);

	return buf0;
}


DT_SWORD dtsEllipseFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	return dtsShapeOut(engine, refx, refy, dtsEllipseFilled_Build(x, y, w, h, tm, &extent), &extent);
}


DT_SWORD dtsEllipse(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT t, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	DT_UBYTE buf0[512];
	DT_UBYTE* buf = buf0;
	DT_SLONG n = 0;
	DT_FLOAT rx, ry;
	DT_FLOAT hx, hy;
	DT_BMP bmp;

	if (t <= 0) return 0;

	MAKE_BOUND(&extent, x, y, x + w, y + h, 0, tm);

	rx = w / 2; hx = rx * DV_FLOAT_BEZIERCIRCLE_CONST;
	ry = h / 2; hy = ry * DV_FLOAT_BEZIERCIRCLE_CONST;

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x, y + ry, tm);

	/* Curve - outer 1 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, 0, -hy, tm);
	WINC_POINT(buf, rx - hx, hy - ry, tm);
	WINC_POINT(buf, hx, 0, tm);

	/* Curve - outer 2 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, hx, 0, tm);
	WINC_POINT(buf, rx - hx, ry - hy, tm);
	WINC_POINT(buf, 0, hy, tm);

	/* Curve - outer 3 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, 0, hy, tm);
	WINC_POINT(buf, hx - rx, ry - hy, tm);
	WINC_POINT(buf, -hx, 0, tm);

	/* Curve - outer 4 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, -hx, 0, tm);
	WINC_POINT(buf, hx - rx, hy - ry, tm);
	WINC_POINT(buf, 0, -hy, tm);

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + t, y + ry, tm);

	rx -= t; hx = rx * DV_FLOAT_BEZIERCIRCLE_CONST;
	ry -= t; hy = ry * DV_FLOAT_BEZIERCIRCLE_CONST;

	/* Curve - inner 1 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, 0, hy, tm);
	WINC_POINT(buf, rx - hx, ry - hy, tm);
	WINC_POINT(buf, hx, 0, tm);

	/* Curve - inner 2 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, hx, 0, tm);
	WINC_POINT(buf, rx - hx, hy - ry, tm);
	WINC_POINT(buf, 0, -hy, tm);

	/* Curve - inner 3 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, 0, -hy, tm);
	WINC_POINT(buf, hx - rx, hy - ry, tm);
	WINC_POINT(buf, -hx, 0, tm);

	/* Curve - inner 4 */
	WINC_CURVE(buf, 24, n);
	WINC_POINT(buf, -hx, 0, tm);
	WINC_POINT(buf, hx - rx, ry - hy, tm);
	WINC_POINT(buf, 0, hy, tm);

	/* End */
	WINC_UBYTE(buf, 8);

	dtShapesDoOutput(engine, DV_NULL, DV_BITMAP_OFF, refx, refy, &extent, DV_SHAPE_BUFFER, buf0, DV_NULL, DV_NULL, &bmp);

	return 1;
}


DT_SWORD dtsEllipseDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm)
{
	return dtsTrueEllipseDashed(engine, refx, refy, x + w / 2, y + h / 2, w / 2, h / 2, ld, ld_shift, t, tm);
}


/*
 * dtsRectangleFilled - Functions to draw a rectangle with optional rounded corners:
 *
 * dtsRectangleFilled - for filled rectangles
 * dtsRectangleDashed - for dashed or solid rectangle outlines
 * dtsRectangle - only for solid rectangle outlines, simplified version of dtsRectangleDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of rectangle's top-left point, in float-point pixels
 *   w, h - rectangle's bounding box, in float-point pixels
 *   r - rectangle's roundness factor (0.0 = DF_MINimum roundness, 1.0 = maximum roundness)
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - thickness of rectangle's outline, in float-point pixels
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_UBYTE* dtsRectangleFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, const DT_TM2X2 tm, DT_RECT_SLONG* extent)
{
	DT_UBYTE *buf, *buf0;
	DT_FLOAT dx = 0, dy = 0;
	DT_SLONG n = 0;

	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(512)) == DV_NULL) return DV_NULL;
	buf = buf0;

	if (r > 0)
	{
		dx = dy = (DF_MIN(w, h) * DF_MIN(DF_MAX(0, r), 1) / 2);
	}

	MAKE_BOUND(extent, x, y, x + w, y + h, 0, tm);

	w -= 2 * dx;
	h -= 2 * dy;

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x, y + dy, tm);

	if (dx > 0) /* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, -dy, tm);
		WINC_POINT(buf, dx, 0, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, w, 0, tm);
	if (dx > 0) /* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, dx, 0, tm);
		WINC_POINT(buf, 0, dy, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, 0, h, tm);
	if (dx > 0) /* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, dy, tm);
		WINC_POINT(buf, -dx, 0, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, -w, 0, tm);
	if (dx > 0) /* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, -dx, 0, tm);
		WINC_POINT(buf, 0, -dy, tm);
	}
	/* Line */
	/* WINC_DELTA(buf, 20); WINC_POINT(buf, 0, -h, tm); */

	/* End */
	WINC_UBYTE(buf, 8);

	return buf0;
}


DT_SWORD dtsRectangleFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	return dtsShapeOut(engine, refx, refy, dtsRectangleFilled_Build(x, y, w, h, r, tm, &extent), &extent);
}


DT_SWORD dtsRectangle(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, DT_FLOAT t, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	DT_UBYTE buf0[512];
	DT_UBYTE* buf = buf0;
	DT_SLONG n = 0;
	DT_BMP bmp;
	DT_FLOAT w2 = w - 2 * t, h2 = h - 2 * t;
	DT_FLOAT dx = 0, dy = 0;
	DT_FLOAT dx2 = 0, dy2 = 0;
	DT_UBYTE method = 1;

	if (t <= 0) return 0;

	if (r > 0)
	{
		dx = dy = (DF_MIN(w, h) * DF_MIN(DF_MAX(0, r), 1) / 2);
	}

	MAKE_BOUND(&extent, x, y, x + w, y + h, 0, tm);

	w -= 2 * dx;
	h -= 2 * dy;

	if (method == 1)
	{
		if (r > 0)
		{
			dx2 = dy2 = (DF_MIN(w2, h2) * DF_MIN(DF_MAX(0, r), 1) / 2);
		}
		w2 -= 2 * dx2;
		h2 -= 2 * dy2;

	}
	else
	{
		dx2 = dx - t;
		dy2 = dy - t;
		w2 = w;
		h2 = h;
	}

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x, y + dy, tm);
	if (dx > 0) /* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, -dy, tm);
		WINC_POINT(buf, dx, 0, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, w, 0, tm);
	if (dx > 0) /* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, dx, 0, tm);
		WINC_POINT(buf, 0, dy, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, 0, h, tm);
	if (dx > 0) /* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, dy, tm);
		WINC_POINT(buf, -dx, 0, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, -w, 0, tm);
	if (dx > 0) /* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, -dx, 0, tm);
		WINC_POINT(buf, 0, -dy, tm);
	}
	/* Line */
	/* WINC_DELTA(buf, 20); WINC_POINT(buf, 0, -h, tm); */

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + t + dx2, y + t, tm);
	/* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, -dx2, 0, tm);
		WINC_POINT(buf, 0, dy2, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, 0, h2, tm);
	/* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, dy2, tm);
		WINC_POINT(buf, dx2, 0, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, w2, 0, tm);
	/* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, dx2, 0, tm);
		WINC_POINT(buf, 0, -dy2, tm);
	}
	/* Line */
	WINC_DELTA(buf, 20);
	WINC_POINT(buf, 0, -h2, tm);
	/* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, -dy2, tm);
		WINC_POINT(buf, -dx2, 0, tm);
	}
	/* Line */
	/* WINC_DELTA(buf, 20); WINC_POINT(buf, -w2, 0, tm); */

	/* End */
	WINC_UBYTE(buf, 8);

	dtShapesDoOutput(engine, DV_NULL, DV_BITMAP_OFF, refx, refy, &extent, DV_SHAPE_BUFFER, buf0, DV_NULL, DV_NULL, &bmp);

	return 1;
}


DT_SWORD dtsRectangleDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, DT_FLOAT ld, DT_FLOAT* /*ld_shift*/, DT_FLOAT t, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	DT_UBYTE buf0[1024];
	DT_UBYTE* buf = buf0;
	DT_SLONG n = 0;
	DT_BMP bmp;
	DT_SLONG nw, nh;
	DT_FLOAT w0 = w, h0 = h;
	DT_FLOAT w2 = w - 2 * t, h2 = h - 2 * t;
	DT_FLOAT x2, y2;
	DT_FLOAT dx = 0, dy = 0;
	DT_FLOAT dx2 = 0, dy2 = 0;
	DT_FLOAT l_w, l_h;
	DT_FLOAT ld_w = ld, ld_h = ld;
	DT_FLOAT ld_shift_tmp;
	DT_UBYTE method = 1;

	if (t <= 0) return 0;

	if (ld < 1) return dtsRectangle(engine, refx, refy, x, y, w, h, r, t, tm);

	if (r > 0)
	{
		dx = dy = (DF_MIN(w, h) * DF_MIN(DF_MAX(0, r), 1) / 2);
	}

	MAKE_BOUND(&extent, x, y, x + w, y + h, 0, tm);

	w -= 2 * dx;
	h -= 2 * dy;

	if (method == 1)
	{
		if (r > 0)
		{
			dx2 = dy2 = (DF_MIN(w2, h2) * DF_MIN(DF_MAX(0, r), 1) / 2);
		}
		//w2 -= 2 * dx2;
		//h2 -= 2 * dy2;

	}
	else
	{
		dx2 = dx - t;
		dy2 = dy - t;
		//w2 = w;
		//h2 = h;
	}

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x, y + dy, tm);
	/* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, -dy, tm);
		WINC_POINT(buf, dx, 0, tm);

		WINC_DELTA(buf, 20);
		WINC_POINT(buf, t + dx2 - dx, 0, tm);
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, t, tm);

		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, -dx2, 0, tm);
		WINC_POINT(buf, 0, dy2, tm);

		WINC_DELTA(buf, 20);
		WINC_POINT(buf, -t, 0, tm);
	}

	/* Line */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + dx + w, y, tm);
	/* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, dx, 0, tm);
		WINC_POINT(buf, 0, dy, tm);

		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, t + dy2 - dy, tm);
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, -t, 0, tm);

		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, -dy2, tm);
		WINC_POINT(buf, -dx2, 0, tm);

		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, -t, tm);
	}

	/* Line */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + 2 * dx + w, y + dy + h, tm);
	/* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, dy, tm);
		WINC_POINT(buf, -dx, 0, tm);

		WINC_DELTA(buf, 20);
		WINC_POINT(buf, dx - t - dx2, 0, tm);
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, -t, tm);

		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, dx2, 0, tm);
		WINC_POINT(buf, 0, -dy2, tm);

		WINC_DELTA(buf, 20);
		WINC_POINT(buf, t, 0, tm);
	}

	/* Line */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, x + dx, y + 2 * dy + h, tm);
	/* rounded corner */
	{
		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, -dx, 0, tm);
		WINC_POINT(buf, 0, -dy, tm);

		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, -t - dy2 + dy, tm);
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, t, 0, tm);

		/* Curve */
		WINC_CURVE(buf, 25, n);
		WINC_POINT(buf, 0, dy2, tm);
		WINC_POINT(buf, dx2, 0, tm);

		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, t, tm);
	}

	/* End */
	WINC_UBYTE(buf, 8);

	dtShapesDoOutput(engine, DV_NULL, DV_BITMAP_OFF, refx, refy, &extent, DV_SHAPE_BUFFER, buf0, DV_NULL, DV_NULL, &bmp);

	l_w = w - 2 * (t + dx2 - dx);
	l_h = h - 2 * (t + dy2 - dy);

	nw = dtsDashes_Adjust(&ld_w, l_w);
	nh = dtsDashes_Adjust(&ld_h, l_h);

	if (nw > 0) ld_w = (3 * l_w) / (3 * nw + 1);
	if (nh > 0) ld_h = (3 * l_h) / (3 * nh + 1);

	x2 = x + t + dx2;
	y2 = y + t + dy2;

	ld_shift_tmp = -ld_w / 3; dtsLineDashed(engine, refx, refy, x2, y + t / 2, l_w, 0, ld_w, &ld_shift_tmp, t, 0, 0, tm);
	ld_shift_tmp = -ld_w / 3; dtsLineDashed(engine, refx, refy, x2, y + h0 - t / 2, l_w, 0, ld_w, &ld_shift_tmp, t, 0, 0, tm);

	ld_shift_tmp = -ld_h / 3; dtsLineDashed(engine, refx, refy, x + t / 2, y2, 0, l_h, ld_h, &ld_shift_tmp, t, 0, 0, tm);
	ld_shift_tmp = -ld_h / 3; dtsLineDashed(engine, refx, refy, x + w0 - t / 2, y2, 0, l_h, ld_h, &ld_shift_tmp, t, 0, 0, tm);

	return 1;
}


DT_UBYTE* dtsRectFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT aw, DT_FLOAT ah, DT_FLOAT t, const DT_TM2X2 tm, DT_RECT_SLONG* extent)
{
	DT_UBYTE* buf;
	DT_UBYTE* buf0;
	DT_SLONG n = 0;
	DT_FLOAT rx, ry;
	DT_FLOAT hx, hy;

	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(512)) == DV_NULL) return DV_NULL;
	buf = buf0;

	if (t < 1) t = 1;

	MAKE_BOUND(extent, x, y, x + w, y + h, 0, tm);

	if (aw > 0 && ah > 0)
	{
		/* adjust arc size to take into account the thickness which makes the Rect very close to the mac quickdraw version */
		aw = aw + t - 1;
		ah = ah + t - 1;

		rx = aw / 2; hx = rx * DV_FLOAT_BEZIERCIRCLE_CONST;
		ry = ah / 2; hy = ry * DV_FLOAT_BEZIERCIRCLE_CONST;

		/* Set point */
		WINC_DELTA(buf, 16);
		WINC_POINT(buf, x, y + ah / 2, tm);

		/* Curve - outer 1 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, 0, -hy, tm);
		WINC_POINT(buf, rx - hx, hy - ry, tm);
		WINC_POINT(buf, hx, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, w - aw, 0, tm);

		/* Curve - outer 2 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, hx, 0, tm);
		WINC_POINT(buf, rx - hx, ry - hy, tm);
		WINC_POINT(buf, 0, hy, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, h - ah, tm);

		/* Curve - outer 3 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, 0, hy, tm);
		WINC_POINT(buf, hx - rx, ry - hy, tm);
		WINC_POINT(buf, -hx, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, aw - w, 0, tm);

		/* Curve - outer 4 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, -hx, 0, tm);
		WINC_POINT(buf, hx - rx, hy - ry, tm);
		WINC_POINT(buf, 0, -hy, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, ah - h, tm);
	}
	else
	{
		/* Set point */
		WINC_DELTA(buf, 16);
		WINC_POINT(buf, x, y, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, w, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, h, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, -w, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, -h, tm);
	}

	/* End */
	WINC_UBYTE(buf, 8);

	return buf0;
}


DT_SWORD dtsRectFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT aw, DT_FLOAT ah, DT_FLOAT t, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	return dtsShapeOut(engine, refx, refy, dtsRectFilled_Build(x, y, w, h, aw, ah, t, tm, &extent), &extent);
}


DT_UBYTE* dtsRect_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT aw, DT_FLOAT ah, DT_FLOAT t, const DT_TM2X2 tm, DT_RECT_SLONG* extent)
{
	DT_UBYTE* buf;
	DT_UBYTE* buf0;
	DT_SLONG n = 0;
	DT_FLOAT rx, ry;
	DT_FLOAT hx, hy;

	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(1024)) == DV_NULL) return DV_NULL;
	buf = buf0;

	if (t < 1) t = 1;

	MAKE_BOUND(extent, x, y, x + w, y + h, 0, tm);

	if (aw > 0 && ah > 0)
	{
		/* adjust arc size to take into account the thickness which makes the Rect very close to the mac quickdraw version */
		aw = aw + t - 1;
		ah = ah + t - 1;

		rx = aw / 2; hx = rx * DV_FLOAT_BEZIERCIRCLE_CONST;
		ry = ah / 2; hy = ry * DV_FLOAT_BEZIERCIRCLE_CONST;

		/* Set point */
		WINC_DELTA(buf, 16);
		WINC_POINT(buf, x, y + ah / 2, tm);

		/* Curve - outer 1 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, 0, -hy, tm);
		WINC_POINT(buf, rx - hx, hy - ry, tm);
		WINC_POINT(buf, hx, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, w - aw, 0, tm);

		/* Curve - outer 2 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, hx, 0, tm);
		WINC_POINT(buf, rx - hx, ry - hy, tm);
		WINC_POINT(buf, 0, hy, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, h - ah, tm);

		/* Curve - outer 3 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, 0, hy, tm);
		WINC_POINT(buf, hx - rx, ry - hy, tm);
		WINC_POINT(buf, -hx, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, aw - w, 0, tm);

		/* Curve - outer 4 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, -hx, 0, tm);
		WINC_POINT(buf, hx - rx, hy - ry, tm);
		WINC_POINT(buf, 0, -hy, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, ah - h, tm);

		/* Set point */
		WINC_DELTA(buf, 16);
		WINC_POINT(buf, x + t, y + h - ry, tm);

		rx -= t; hx = rx * DV_FLOAT_BEZIERCIRCLE_CONST;
		ry -= t; hy = ry * DV_FLOAT_BEZIERCIRCLE_CONST;

		/* Curve - inner 1 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, 0, hy, tm);
		WINC_POINT(buf, rx - hx, ry - hy, tm);
		WINC_POINT(buf, hx, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, w - aw, 0, tm);

		/* Curve - inner 2 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, hx, 0, tm);
		WINC_POINT(buf, rx - hx, hy - ry, tm);
		WINC_POINT(buf, 0, -hy, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, ah - h, tm);

		/* Curve - inner 3 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, 0, -hy, tm);
		WINC_POINT(buf, hx - rx, hy - ry, tm);
		WINC_POINT(buf, -hx, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, aw - w, 0, tm);

		/* Curve - inner 4 */
		WINC_CURVE(buf, 24, n);
		WINC_POINT(buf, -hx, 0, tm);
		WINC_POINT(buf, hx - rx, ry - hy, tm);
		WINC_POINT(buf, 0, hy, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, h - ah, tm);
	}
	else
	{
		/* Set point */
		WINC_DELTA(buf, 16);
		WINC_POINT(buf, x, y, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, w, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, h, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, -w, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, -h, tm);

		/* Set point */
		WINC_DELTA(buf, 16);
		WINC_POINT(buf, x + t, y + h - t, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, w - 2 * t, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, 2 * t - h, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 2 * t - w, 0, tm);

		/* Line */
		WINC_DELTA(buf, 20);
		WINC_POINT(buf, 0, h - 2 * t, tm);
	}

	/* End */
	WINC_UBYTE(buf, 8);

	return buf0;
}


DT_SWORD dtsRect(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT aw, DT_FLOAT ah, DT_FLOAT t, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	return dtsShapeOut(engine, refx, refy, dtsRect_Build(x, y, w, h, aw, ah, t, tm, &extent), &extent);
}


/*
 * Functions to draw a polygon (polyline) or custom shape:
 *
 * dtsPolyFilled - for filled polygons
 * dtsPolyDashed - for dashed or solid polygon outlines
 * dtsPoly - only for solid polygon outlines, simplified version of dtsPolyDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   ct[] - an array of instructions
 *          16 or 17 = new contour
 *          20 = new line segment
 *          24 = new Bezier curve segment
 *          25 = new B-Spline curve segment
 *   cx[] - an array of polygon X coordinates, in float-point pixels
 *   cy[] - an array of polygon Y coordinates, in float-point pixels
 *
 *                                   ct[]          cx[]        cy[]
 *          =======================================================
 *          New Closed Contour        16            X            Y
 *          -------------------------------------------------------
 *          New Open Contour          17            X            Y
 *          -------------------------------------------------------
 *          Line                      20            X            Y
 *          -------------------------------------------------------
 *          Bezier Curve              24           X1           Y1
 *                                    cs           X2           Y2
 *                                     0           X3           Y3
 *          -------------------------------------------------------
 *          B-Spline Curve            25           X1           Y1
 *                                    cs           X2           Y2
 *          =======================================================
 *          cs = number of curve segments
 *
 *   n - size of the ct[], cx[] and cy[] array
 *   bound_box - polygon's bounding box in pixels, if DV_NULL it will be auto-calculated
 *   ld - length of a single dashed segment in pixels
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - thickness of polygon's outline, in float-point pixels
 *   tm - optional Affine transform matrix (DV_NULL = no transform)
 */


DT_UBYTE* dtsPolyFilled_Build(const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, const DT_RECT_SLONG* bound_box, const DT_TM2X2 tm, DT_RECT_SLONG* extent)
{
	DT_SLONG i, seg;
	DT_FLOAT dx1, dy1, dx2, dy2, dx3, dy3;
	DT_UBYTE *buf, *buf0;
	DT_RECT_SLONG bound_box_tmp;

	if (n < 2) return DV_NULL;

	DT_SLONG l_max = 2 * sizeof(DT_UBYTE) + sizeof(DT_UWORD) + 6 * sizeof(DT_FLOAT);

	if ((buf0 = (DT_UBYTE*)DF_SHAPE_MEM_ALLOC(DF_SEC_ADD(512, DF_SEC_MUL(n, l_max)))) == DV_NULL) return DV_NULL;
	buf = buf0;

	if (bound_box == DV_NULL) /* bound box not provided so calculate it */
	{
		bound_box_tmp.xmn = bound_box_tmp.xmx = DF_ROUND(cx[0]);
		bound_box_tmp.ymn = bound_box_tmp.ymx = DF_ROUND(cy[0]);

		for (i = 1; i < n; i++)
		{
			bound_box_tmp.xmn = DF_MIN(bound_box_tmp.xmn, DF_ROUND(cx[i]));
			bound_box_tmp.xmx = DF_MAX(bound_box_tmp.xmx, DF_ROUND(cx[i]));
			bound_box_tmp.ymn = DF_MIN(bound_box_tmp.ymn, DF_ROUND(cy[i]));
			bound_box_tmp.ymx = DF_MAX(bound_box_tmp.ymx, DF_ROUND(cy[i]));
		}

		bound_box = &bound_box_tmp;
	}

	MAKE_BOUND(extent, bound_box->xmn, bound_box->ymn, bound_box->xmx, bound_box->ymx, 0, tm);

	/* Set point */
	WINC_DELTA(buf, 16);
	WINC_POINT(buf, cx[0], cy[0], tm);
	for (i = 1; i < n; i++)
	{
		if (ct[i] == 16 || ct[i] == 17)
		{
			/* New contour */
			WINC_DELTA(buf, 16);
			WINC_POINT(buf, cx[i], cy[i], tm);
		}
		else if (ct[i] == 20)
		{
			/* Line */
			WINC_DELTA(buf, 20);
			WINC_POINT(buf, cx[i] - cx[i - 1], cy[i] - cy[i - 1], tm);
		}
		else if (ct[i] == 24)
		{
			/* Bezier Curve */
			dx1 = cx[i] - cx[i - 1];
			dy1 = cy[i] - cy[i - 1];
			i++;
			seg = ct[i];
			dx2 = cx[i] - cx[i - 1];
			dy2 = cy[i] - cy[i - 1];
			i++;
			dx3 = cx[i] - cx[i - 1];
			dy3 = cy[i] - cy[i - 1];

			WINC_CURVE(buf, 24, seg);
			WINC_POINT(buf, dx1, dy1, tm);
			WINC_POINT(buf, dx2, dy2, tm);
			WINC_POINT(buf, dx3, dy3, tm);

		}
		else if (ct[i] == 25)
		{
			/* B-Spline Curve */
			dx1 = cx[i] - cx[i - 1];
			dy1 = cy[i] - cy[i - 1];
			i++;
			seg = ct[i];
			dx2 = cx[i] - cx[i - 1];
			dy2 = cy[i] - cy[i - 1];

			WINC_CURVE(buf, 25, seg);
			WINC_POINT(buf, dx1, dy1, tm);
			WINC_POINT(buf, dx2, dy2, tm);
		}
	}
	/* End */
	WINC_UBYTE(buf, 8);

	return buf0;
}


DT_SWORD dtsPolyFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, const DT_RECT_SLONG* bound_box, const DT_TM2X2 tm)
{
	DT_RECT_SLONG extent;
	return dtsShapeOut(engine, refx, refy, dtsPolyFilled_Build(ct, cx, cy, n, bound_box, tm, &extent), &extent);
}


DT_SWORD dtsPolyDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	DT_SLONG i;
	DT_FLOAT x0, y0;
	DT_FLOAT dx, dy;
	DT_UBYTE ClosePath = 1;
	DT_UBYTE bgn0 = bgn, end0 = 1;

	DT_FLOAT ld_shift0 = 0;
	DT_FLOAT* ld_shift_ptr = &ld_shift0;

	if (ld_shift != DV_NULL) ld_shift_ptr = ld_shift;

	if (n < 2 || t <= 0) return 0;

	x0 = cx[0];
	y0 = cy[0];
	if (ct[0] == 17) ClosePath = 0;
	else ClosePath = 1;

	for (i = 0; i < n - 1; i++)
	{
		if (ct[i + 1] == 16 || ct[i + 1] == 17)
		{
			if ((ClosePath == 0) && (i + 2 == n || ct[i + 2] == 16 || ct[i + 2] == 17)) end0 = end;

			dx = x0 - cx[i];
			dy = y0 - cy[i];

			if (ClosePath == 1) dtsLineDashed(engine, refx, refy, cx[i], cy[i], dx, dy, ld, ld_shift_ptr, t, bgn, end, tm);

			end0 = 1;
			bgn = bgn0;

			x0 = cx[i + 1];
			y0 = cy[i + 1];
			if (ct[i + 1] == 17) ClosePath = 0;
			else ClosePath = 1;
		}
		else if (ct[i + 1] == 25)
		{
			if ((ClosePath == 0) && (i + 3 == n || ct[i + 3] == 16 || ct[i + 3] == 17)) end0 = end;

			dtsBSplineCurveDashed(engine, refx, refy, cx[i], cy[i], cx[i + 1] - cx[i], cy[i + 1] - cy[i], cx[i + 2] - cx[i + 1], cy[i + 2] - cy[i + 1], ct[i + 2], ld, ld_shift_ptr, t, bgn, end0, tm);
			bgn = 1;
			i++;
		}
		else if (ct[i + 1] == 24)
		{
			if ((ClosePath == 0) && (i + 4 == n || ct[i + 4] == 16 || ct[i + 4] == 17)) end0 = end;

			dtsBezierCurveDashed(engine, refx, refy, cx[i], cy[i], cx[i + 1] - cx[i], cy[i + 1] - cy[i], cx[i + 2] - cx[i + 1], cy[i + 2] - cy[i + 1], cx[i + 3] - cx[i + 2], cy[i + 3] - cy[i + 2], ct[i + 2], ld, ld_shift_ptr, t, bgn, end0, tm);
			bgn = 1;
			i += 2;
		}
		else
		{
			if ((ClosePath == 0) && (i + 2 == n || ct[i + 2] == 16 || ct[i + 2] == 17)) end0 = end;

			dx = cx[i + 1] - cx[i];
			dy = cy[i + 1] - cy[i];
			dtsLineDashed(engine, refx, refy, cx[i], cy[i], dx, dy, ld, ld_shift_ptr, t, bgn, end0, tm);
			bgn = 1;
		}

	}
	dx = x0 - cx[i];
	dy = y0 - cy[i];
	if (ClosePath == 1) dtsLineDashed(engine, refx, refy, cx[i], cy[i], dx, dy, ld, ld_shift_ptr, t, bgn, end, tm);

	return 1;
}


DT_SWORD dtsPoly(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm)
{
	return dtsPolyDashed(engine, refx, refy, ct, cx, cy, n, -1, DV_NULL, t, bgn, end, tm);
}
