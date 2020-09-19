#include "rect.h"

#include <assert.h>
#include <math.h>

#include "common.h"
#include "vec2.h"

bool recti_is_empty(const recti *r)
{
    return r->x == 0 && r->y == 0 && r->w == 0 && r->h == 0;
}

bool rect_is_empty(const rect *r)
{
    return r->x == 0 && r->y == 0 && r->w == 0 && r->h == 0;
}

bool recti_overlap(const recti *a, const recti *b)
{
    assert(a);
    assert(b);

    return !((a->x > b->x + b->w || b->x > a->x + a->w) ||
             (a->y > b->y + b->h || b->y > a->y + a->h));
}

bool rect_overlap(const rect *a, const rect *b)
{
    assert(a);
    assert(b);

    return !((a->x > b->x + b->w || b->x > a->x + a->w) ||
             (a->y > b->y + b->h || b->y > a->y + a->h));
}

bool recti_intersection(const recti *a, const recti *b, recti *dst)
{
    assert(a);
    assert(b);
    assert(dst);

    int32_t a_end_x = a->x + a->w;
    int32_t a_end_y = a->y + a->h;
    int32_t b_end_x = b->x + b->w;
    int32_t b_end_y = b->y + b->h;

    int32_t start_x = max(a->x, b->x);
    int32_t start_y = max(a->y, b->y);
    int32_t end_x   = min(a_end_x, b_end_x);
    int32_t end_y   = min(a_end_y, b_end_y);

    int32_t x_overlap = max(0, end_x - start_x);
    int32_t y_overlap = max(0, end_y - start_y);

    if (x_overlap > 0 && y_overlap > 0)
    {
        dst->x = start_x;
        dst->y = start_y;
        dst->w = end_x - start_x;
        dst->h = end_y - start_y;

        return true;
    }

    return false;
}

bool rect_intersection(const rect *a, const rect *b, rect *dst)
{
    assert(a);
    assert(b);
    assert(dst);

    double a_end_x = a->x + a->w;
    double a_end_y = a->y + a->h;
    double b_end_x = b->x + b->w;
    double b_end_y = b->y + b->h;

    double start_x = max(a->x, b->x);
    double start_y = max(a->y, b->y);
    double end_x   = min(a_end_x, b_end_x);
    double end_y   = min(a_end_y, b_end_y);

    double x_overlap = max(0.0f, end_x - start_x);
    double y_overlap = max(0.0f, end_y - start_y);

    if (x_overlap > 0.0f && y_overlap > 0.0f)
    {
        dst->x = start_x;
        dst->y = start_y;
        dst->w = end_x - start_x;
        dst->h = end_y - start_y;

        return true;
    }

    return false;
}

bool recti_point_intersect(const recti *r, const vec2i *v)
{
    assert(r);
    assert(v);

    return (v->x >= r->x && v->x <= (r->x + r->w)) &&
           (v->y >= r->y && v->y <= (r->y + r->h));
}

bool rect_point_intersect(const rect *r, const vec2 *v)
{
    assert(r);
    assert(v);

    return (v->x >= r->x && v->x <= (r->x + r->w)) &&
           (v->y >= r->y && v->y <= (r->y + r->h));
}

void recti_union(const recti *a, const recti *b, recti *dst)
{
    assert(a);
    assert(b);
    assert(dst);

    recti result;

    result.x = min(a->x, b->x);
    result.y = min(a->y, b->y);
    result.w = max(a->x + a->w, b->x + b->w) - result.x;
    result.h = max(a->y + a->h, b->y + b->h) - result.y;

    *dst = result;
}

void rect_union(const rect *a, const rect *b, rect *dst)
{
    assert(a);
    assert(b);
    assert(dst);

    dst->x = min(a->x, b->x);
    dst->y = min(a->y, b->y);
    dst->w = max(a->x + a->w, b->x + b->w) - dst->x;
    dst->h = max(a->y + a->h, b->y + b->h) - dst->y;
}

recti rect_round(const rect *r)
{
    double rounded_x = round(r->x);

    recti result = {
        .x = (int32_t)rounded_x,
        .y = (int32_t)round(r->y),
        .w = (int32_t)round(r->w),
        .h = (int32_t)round(r->h),
    };

    return result;
}
