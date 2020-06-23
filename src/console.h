#pragma once

#include "common.h"

typedef struct eva_framebuffer eva_framebuffer;

void console_keydown(int32_t key, uint32_t mods);

// Returns true if drawing took place, false otherwise.
void console_draw(const eva_framebuffer *);
