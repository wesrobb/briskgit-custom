#pragma once

#include "common.h"

typedef struct ustr ustr;
typedef struct vec2f vec2f;
typedef struct textfield textfield;

textfield* textfield_create(const vec2f *pos, float width, 
                            float font_size, int32_t padding);
void textfield_destroy(textfield *tf);
void textfield_input_text(const textfield *tf, 
                          const uint16_t *text, size_t len);
void textfield_mouse_moved(const textfield *tf, const vec2f *pos);
void textfield_mouse_pressed(const textfield *tf, const vec2f *pos);
void textfield_mouse_released(const textfield *tf, const vec2f *pos);
void textfield_draw(const textfield *tf);
bool textfield_active(const textfield *tf);
bool textfield_hovered(const textfield *tf);
