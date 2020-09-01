#include "vec2.h"

vec2i vec2i_sub(const vec2i *a, const vec2i *b)
{
    vec2i result;
    result.x = a->x - b->x;
    result.y = a->y - b->y;
    return result;
}

vec2f vec2f_sub(const vec2f *a, const vec2f *b)
{
    vec2f result;
    result.x = a->x - b->x;
    result.y = a->y - b->y;
    return result;
}
