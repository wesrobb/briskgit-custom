#include "rect.h"

#include <assert.h>

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

bool recti_intersect(const recti *a, const recti *b)
{
    assert(a);
    assert(b);

    return !((a->x > b->x + b->w || b->x > a->x + a->w) ||
             (a->y > b->y + b->h || b->y > a->y + a->h));
}

bool rectf_intersect(const rectf *a, const rectf *b)
{
    assert(a);
    assert(b);

    return !((a->x > b->x + b->w || b->x > a->x + a->w) ||
             (a->y > b->y + b->h || b->y > a->y + a->h));
}

bool recti_point_intersect(const recti *r, const vec2i *v)
{
    assert(r);
    assert(v);

    return (v->x >= r->x && v->x <= (r->x + r->w)) &&
           (v->y >= r->y && v->y <= (r->y + r->h));
}

bool rectf_point_intersect(const recti *r, const vec2f *v)
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
