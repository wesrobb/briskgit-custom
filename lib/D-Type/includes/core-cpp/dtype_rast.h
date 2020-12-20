/*
 * raster.h - D-Type Rasterizer header
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#ifndef DTYPE_RASTERCPP_H
#define DTYPE_RASTERCPP_H


#include "../core/dtype.h"


const DT_UBYTE DV_RGBT[4] = {0, 0, 0, 0};


class CDTRasterizerV8
{
private: // private copy constructor and assignment operator

	CDTRasterizerV8(const CDTRasterizerV8&);
	CDTRasterizerV8& operator = (const CDTRasterizerV8&);

public:

	CDTRasterizerV8() : Rast(DV_NULL) {}
	CDTRasterizerV8(const DT_MDC* memory_surface, DT_SLONG raster_intersect = 0, DT_ID_SLONG raster_id = 0) : Rast(DV_NULL) { Init(memory_surface, raster_intersect, raster_id); }
	CDTRasterizerV8(const DT_MDC& memory_surface, DT_SLONG raster_intersect = 0, DT_ID_SLONG raster_id = 0) : Rast(DV_NULL) { Init(memory_surface, raster_intersect, raster_id); }
	virtual ~CDTRasterizerV8() { Exit(); }

	bool Init(const DT_MDC* memory_surface, DT_SLONG raster_intersect = 0, DT_ID_SLONG raster_id = 0, DT_DASYS das = DV_NULL, DT_UWORD flags = 0) { Rast = DV_NULL; if (dcRasterizerIniPlus(&Rast, memory_surface, raster_intersect, raster_id, das, flags) != 1) return false; return true; }
	bool Init(const DT_MDC& memory_surface, DT_SLONG raster_intersect = 0, DT_ID_SLONG raster_id = 0, DT_DASYS das = DV_NULL, DT_UWORD flags = 0) { Rast = DV_NULL; if (dcRasterizerIniPlus(&Rast, &memory_surface, raster_intersect, raster_id, das, flags) != 1) return false; return true; }
	void Exit() { if (Rast != DV_NULL) dcRasterizerExt(Rast); Rast = DV_NULL; }

public: // Public methods

	bool InitFailed() { return (Rast == DV_NULL); }

	inline void SetParams(const DT_UBYTE color[] = DV_RGBT, DT_ID_UBYTE fill_rule = DV_NZW_OFF, DT_ID_UBYTE reserved1 = 0, DT_SWORD reserved2 = 0) { dcRasterizerSetParams(Rast, color, fill_rule, reserved1, reserved2); }
	inline void SetFillRule(DT_ID_UBYTE fill_rule) { dcRasterizerSetFillRule(Rast, fill_rule); }
	inline void GetFillRule(DT_ID_UBYTE* fill_rule) const { dcRasterizerGetFillRule(Rast, fill_rule); }
	inline void SetASys(DT_DASYS das) { dcRasterizerSetASys(Rast, das); }
	inline void GetASys(DT_DASYS* das) const { dcRasterizerGetASys(Rast, das); }
	inline void MoveTo(DT_FLOAT x1, DT_FLOAT y1, DT_UBYTE r = 0, DT_UBYTE g = 0, DT_UBYTE b = 0, DT_UBYTE t = 0) { dcRasterizerMoveTo(Rast, x1, y1, r, g, b, t); }
	inline void MoveTo2(DT_FLOAT x1, DT_FLOAT y1, const DT_UBYTE color[] = DV_RGBT) { dcRasterizerMoveTo2(Rast, x1, y1, color); }
	inline void MoveToFrac(DT_SFRAC_L x1, DT_SFRAC_L y1, DT_UBYTE r = 0, DT_UBYTE g = 0, DT_UBYTE b = 0, DT_UBYTE t = 0) { dcRasterizerMoveToFrac(Rast, x1, y1, r, g, b, t); }
	inline void MoveToFrac2(DT_SFRAC_L x1, DT_SFRAC_L y1, const DT_UBYTE color[] = DV_RGBT) { dcRasterizerMoveToFrac2(Rast, x1, y1, color); }
	inline void LineTo(DT_FLOAT x1, DT_FLOAT y1) { dcRasterizerLineTo(Rast, x1, y1); }
	inline void LineTo2(DT_FLOAT x1, DT_FLOAT y1, const DT_UBYTE color[] = DV_RGBT) { dcRasterizerLineTo2(Rast, x1, y1, color); }
	inline void LineTo3(DT_FLOAT x1, DT_FLOAT y1, DT_UBYTE r = 0, DT_UBYTE g = 0, DT_UBYTE b = 0, DT_UBYTE t = 0) { dcRasterizerLineTo3(Rast, x1, y1, r, g, b, t); }
	inline void LineToFrac(DT_SFRAC_L x1, DT_SFRAC_L y1) { dcRasterizerLineToFrac(Rast, x1, y1); }
	inline void LineToFrac2(DT_SFRAC_L x1, DT_SFRAC_L y1, const DT_UBYTE color[] = DV_RGBT) { dcRasterizerLineToFrac2(Rast, x1, y1, color); }
	inline void LineToFrac3(DT_SFRAC_L x1, DT_SFRAC_L y1, DT_UBYTE r = 0, DT_UBYTE g = 0, DT_UBYTE b = 0, DT_UBYTE t = 0) { dcRasterizerLineToFrac3(Rast, x1, y1, r, g, b, t); }
	inline void BSplineTo(DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_SLONG seg = 0) { dcRasterizerBSplineTo(Rast, x1, y1, x2, y2, seg); }
	inline void BSplineTo2(DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_SLONG seg = 0, const DT_UBYTE color1[] = DV_RGBT, const DT_UBYTE color2[] = DV_RGBT) { dcRasterizerBSplineTo2(Rast, x1, y1, x2, y2, seg, color1, color2); }
	inline void BSplineToFrac(DT_SFRAC_L x1, DT_SFRAC_L y1, DT_SFRAC_L x2, DT_SFRAC_L y2, DT_SLONG seg = 0) { dcRasterizerBSplineToFrac(Rast, x1, y1, x2, y2, seg); }
	inline void BSplineToFrac2(DT_SFRAC_L x1, DT_SFRAC_L y1, DT_SFRAC_L x2, DT_SFRAC_L y2, DT_SLONG seg = 0, const DT_UBYTE color1[] = DV_RGBT, const DT_UBYTE color2[] = DV_RGBT) { dcRasterizerBSplineToFrac2(Rast, x1, y1, x2, y2, seg, color1, color2); }
	inline void BezierTo(DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_FLOAT x3, DT_FLOAT y3, DT_SLONG seg = 0) { dcRasterizerBezierTo(Rast, x1, y1, x2, y2, x3, y3, seg); }
	inline void BezierTo2(DT_FLOAT x1, DT_FLOAT y1, DT_FLOAT x2, DT_FLOAT y2, DT_FLOAT x3, DT_FLOAT y3, DT_SLONG seg = 0, const DT_UBYTE color1[] = DV_RGBT, const DT_UBYTE color2[] = DV_RGBT, const DT_UBYTE color3[] = DV_RGBT) { dcRasterizerBezierTo2(Rast, x1, y1, x2, y2, x3, y3, seg, color1, color2, color3); }
	inline void BezierToFrac(DT_SFRAC_L x1, DT_SFRAC_L y1, DT_SFRAC_L x2, DT_SFRAC_L y2, DT_SFRAC_L x3, DT_SFRAC_L y3, DT_SLONG seg = 0) { dcRasterizerBezierToFrac(Rast, x1, y1, x2, y2, x3, y3, seg); }
	inline void BezierToFrac2(DT_SFRAC_L x1, DT_SFRAC_L y1, DT_SFRAC_L x2, DT_SFRAC_L y2, DT_SFRAC_L x3, DT_SFRAC_L y3, DT_SLONG seg = 0, const DT_UBYTE color1[] = DV_RGBT, const DT_UBYTE color2[] = DV_RGBT, const DT_UBYTE color3[] = DV_RGBT) { dcRasterizerBezierToFrac2(Rast, x1, y1, x2, y2, x3, y3, seg, color1, color2, color3); }
	inline DT_SWORD MultiSegment(DT_FLOAT x, DT_FLOAT y, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_UBYTE c_arr[], const DT_FLOAT x_arr[], const DT_FLOAT y_arr[]) { return dcRasterizerMultiSegment(Rast, x, y, flag, i_arr, c_arr, x_arr, y_arr); }
	inline DT_SWORD MultiSegmentFrac(DT_SFRAC_L x, DT_SFRAC_L y, DT_ID_SWORD flag, const DT_UBYTE i_arr[], const DT_UBYTE c_arr[], const DT_SFRAC_L x_arr[], const DT_SFRAC_L y_arr[]) { return dcRasterizerMultiSegmentFrac(Rast, x, y, flag, i_arr, c_arr, x_arr, y_arr); }
	inline void DoOutput(DT_ID_SWORD format, DT_ID_SWORD subformat = 0, DT_SWORD reserved = 0) { dcRasterizerDoOutput(Rast, format, subformat, reserved); }

private:

	DT_DCRASTER Rast;
};


#endif /* DTYPE_RASTERCPP_H */

