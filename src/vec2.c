#include "vec2.h"

vec2i vec2i_sub(const vec2i *a, const vec2i *b)
{
    vec2i result;
    result.x = a->x - b->x;
    result.y = a->y - b->y;
    return result;
}

vec2 vec2_sub(const vec2 *a, const vec2 *b)
{
    vec2 result;
    result.x = a->x - b->x;
    result.y = a->y - b->y;
    return result;
}
