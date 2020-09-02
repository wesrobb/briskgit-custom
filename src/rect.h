#pragma once

#include "common.h"

typedef struct vec2i vec2i;
typedef struct vec2 vec2;

typedef struct recti {
    int32_t x, y, w, h;
} recti;

typedef struct rect {
    double x, y, w, h;
} rect;

bool recti_is_empty(const recti *r);
bool rect_is_empty(const rect *r);

bool recti_overlap(const recti *a, const recti *b);
bool rect_overlap(const rect *a, const rect *b);

bool recti_intersection(const recti *a, const recti *b, recti *dst);
bool rect_intersection(const rect *a, const rect *b, rect *dst);

bool recti_point_intersect(const recti *r, const vec2i *v);
bool rect_point_intersect(const rect *r, const vec2 *v);

void recti_union(const recti *a, const recti *b, recti *dst);
void rect_union(const rect *a, const rect *b, rect *dst);

recti rect_round(const rect *r);
