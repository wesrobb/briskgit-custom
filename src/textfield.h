#pragma once

#include "common.h"

typedef struct ustr ustr;
typedef struct vec2 vec2;
typedef struct textfield textfield;

textfield* textfield_create(double width, double font_size, int32_t padding);
void textfield_destroy(textfield *tf);
void textfield_input_text(const textfield *tf, 
                          const uint16_t *text, size_t len);
void textfield_keydown(const textfield *tf, int32_t key, uint32_t mods);
void textfield_mouse_moved(const textfield *tf, const vec2 *pos);
void textfield_mouse_pressed(textfield *tf,
                             const vec2 *mouse_pos, const vec2 *pos);
void textfield_mouse_released(const textfield *tf, const vec2 *pos);
void textfield_draw(const textfield *tf, const vec2 *pos);
bool textfield_active(const textfield *tf);
bool textfield_hovered(const textfield *tf);
