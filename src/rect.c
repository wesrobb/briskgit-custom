#include "rect.h"

#include <assert.h>
#include <math.h>

#include "common.h"
#include "vec2.h"

bool recti_is_empty(const recti *r)
{
    return r->x == 0 && r->y == 0 && r->w == 0 && r->h == 0;
}

bool rectf_is_empty(const rectf *r)
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

bool rectf_overlap(const rectf *a, const rectf *b)
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

bool rectf_intersection(const rectf *a, const rectf *b, rectf *dst)
{
    assert(a);
    assert(b);
    assert(dst);

    float a_end_x = a->x + a->w;
    float a_end_y = a->y + a->h;
    float b_end_x = b->x + b->w;
    float b_end_y = b->y + b->h;

    float start_x = max(a->x, b->x);
    float start_y = max(a->y, b->y);
    float end_x   = min(a_end_x, b_end_x);
    float end_y   = min(a_end_y, b_end_y);

    float x_overlap = max(0.0f, end_x - start_x);
    float y_overlap = max(0.0f, end_y - start_y);

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

bool rectf_point_intersect(const rectf *r, const vec2f *v)
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

void rectf_union(const rectf *a, const rectf *b, rectf *dst)
{
    assert(a);
    assert(b);
    assert(dst);

    dst->x = min(a->x, b->x);
    dst->y = min(a->y, b->y);
    dst->w = max(a->x + a->w, b->x + b->w) - dst->x;
    dst->h = max(a->y + a->h, b->y + b->h) - dst->y;
}

recti rectf_round(const rectf *r)
{
    recti result = {
        .x = (int32_t)roundf(r->x),
        .y = (int32_t)roundf(r->y),
        .w = (int32_t)roundf(r->w),
        .h = (int32_t)roundf(r->h),
    };

    return result;
}
