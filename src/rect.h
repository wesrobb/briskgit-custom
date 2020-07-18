#pragma once

#include "common.h"

typedef struct vec2i vec2i;
typedef struct vec2f vec2f;

typedef struct recti {
    int32_t x, y, w, h;
} recti;

typedef struct rectf {
    float x, y, w, h;
} rectf;

bool recti_is_empty(const recti *r);
bool rectf_is_empty(const rectf *r);

bool recti_intersect(const recti *a, const recti *b);
bool rectf_intersect(const rectf *a, const rectf *b);

bool recti_point_intersect(const recti *r, const vec2i *v);
bool rectf_point_intersect(const recti *r, const vec2f *v);

void recti_union(const recti *a, const recti *b, recti *dst);
void rectf_union(const rectf *a, const rectf *b, rectf *dst);
