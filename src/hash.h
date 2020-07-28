#pragma once

#include "common.h"

#define HASH_INITIAL 2166136261

// See https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
void hash(uint32_t *v, uint8_t *buf, size_t len);
