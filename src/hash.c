#include "hash.h"

void hash(uint32_t *v, uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        *v ^= buf[i];
        *v *= 16777619;
    }
}
