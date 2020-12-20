/*
 * dtshapes.h - auxiliary header file for the D-Type Rasterizer to draw 2D graphics primitives
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#ifndef DTYPE_DTSHAPES_H
#define DTYPE_DTSHAPES_H

#define DTYPE_DTSHAPES_VER_STRING "7.x"

#define DV_LINESTYLE_FLAT 0
#define DV_LINESTYLE_ROUND_OUTSIDE 1
#define DV_LINESTYLE_ROUND_INSIDE 2
#define DV_LINESTYLE_PEAK_OUTSIDE 3
#define DV_LINESTYLE_PEAK_INSIDE 4
#define DV_LINESTYLE_SQUARE_OUTSIDE 5
#define DV_LINESTYLE_SQUARE_INSIDE 6


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
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_SWORD dtsLineDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);
DT_SWORD dtsLine(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx, DT_FLOAT dy, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);


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
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_SWORD dtsBSplineCurveDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);
DT_SWORD dtsBSplineCurve(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);


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
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_SWORD dtsBezierCurveDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_FLOAT dx3, DT_FLOAT dy3, DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);
DT_SWORD dtsBezierCurve(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT dx1, DT_FLOAT dy1, DT_FLOAT dx2, DT_FLOAT dy2, DT_FLOAT dx3, DT_FLOAT dy3, DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);


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
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_SWORD dtsArcDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);
DT_SWORD dtsArc(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);



/* Shape and Shape Outline Drawing Functions */


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
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_UBYTE* dtsSliceFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, const DT_TM2X2 tm, DT_RECT_SLONG* extent);
DT_SWORD dtsSliceFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, const DT_TM2X2 tm);
DT_SWORD dtsSliceDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm);
DT_SWORD dtsSlice(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_UWORD alpha0, DT_UWORD theta, DT_FLOAT t, const DT_TM2X2 tm);


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
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_UBYTE* dtsTrueEllipseFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, const DT_TM2X2 tm, DT_RECT_SLONG* extent);
DT_SWORD dtsTrueEllipseFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, const DT_TM2X2 tm);
DT_SWORD dtsTrueEllipse(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_FLOAT t, const DT_TM2X2 tm);
DT_SWORD dtsTrueEllipseDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT rx, DT_FLOAT ry, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm);


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
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_UBYTE* dtsEllipseFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, const DT_TM2X2 tm, DT_RECT_SLONG* extent);
DT_SWORD dtsEllipseFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, const DT_TM2X2 tm);
DT_SWORD dtsEllipse(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT t, const DT_TM2X2 tm);
DT_SWORD dtsEllipseDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm);


/*
 * Functions to draw a rectangle with optional rounded corners:
 *
 * dtsRectangleFilled - for filled rectangles
 * dtsRectangleDashed - for dashed or solid rectangle outlines
 * dtsRectangle - only for solid rectangle outlines, simplified version of dtsRectangleDashed
 *
 *   engine - Standard Engine instance
 *   refx, refy - origin's position on screen, in pixels
 *   x, y - coordinate of rectangle's top-left point, in float-point pixels
 *   w, h - rectangle's bounding box, in float-point pixels
 *   r - rectangle's roundness factor (0.0 = minimum roundness, 1.0 = maximum roundness)
 *   ld - length of a single dashed segment in pixels, set to -1 for solid
 *   ld_shift - presently undocumented, must be set to DV_NULL
 *   t - thickness of rectangle's outline, in float-point pixels
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_UBYTE* dtsRectangleFilled_Build(DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, const DT_TM2X2 tm, DT_RECT_SLONG* extent);
DT_SWORD dtsRectangleFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, const DT_TM2X2 tm);
DT_SWORD dtsRectangle(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, DT_FLOAT t, const DT_TM2X2 tm);
DT_SWORD dtsRectangleDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, DT_FLOAT x, DT_FLOAT y, DT_FLOAT w, DT_FLOAT h, DT_FLOAT r, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, const DT_TM2X2 tm);


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
 *   TM - optional Affine transform matrix (DV_NULL = no transform)
 */

DT_UBYTE* dtsPolyFilled_Build(const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, const DT_RECT_SLONG* bound_box, const DT_TM2X2 tm, DT_RECT_SLONG* extent);
DT_SWORD dtsPolyFilled(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, const DT_RECT_SLONG* bound_box, const DT_TM2X2 tm);
DT_SWORD dtsPolyDashed(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, DT_FLOAT ld, DT_FLOAT* ld_shift, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);
DT_SWORD dtsPoly(DT_DTENGINE engine, DT_SLONG refx, DT_SLONG refy, const DT_UBYTE ct[], const DT_FLOAT cx[], const DT_FLOAT cy[], DT_SLONG n, DT_FLOAT t, DT_UBYTE bgn, DT_UBYTE end, const DT_TM2X2 tm);


/* Other Functions */

void dtsRelease(DT_UBYTE* buf);


#endif /* DTYPE_DTSHAPES_H */
