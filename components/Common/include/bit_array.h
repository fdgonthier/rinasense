#ifndef _COMMON_BIT_ARRAY_H
#define _COMMON_BIT_ARRAY_H

#include <stdint.h>
#include <stddef.h>

#include "portability/port.h"

typedef struct _BITARRAY {
    uint32_t nbbits;
} bitarray_t;

#define BITS_PER_UNIT 32
#define UNIT uint32_t

#define BIT_INDEX(n) \
    uint8_t bit = n % BITS_PER_UNIT;\
    uint32_t index = (n - bit) / BITS_PER_UNIT

static inline bitarray_t *bitarray_alloc(uint32_t nbbits)
{
    bitarray_t *p;
    size_t nbUnits = nbbits / BITS_PER_UNIT + 1;
    size_t sz = sizeof(bitarray_t) + nbUnits * sizeof(UNIT);
    p = pvRsMemAlloc(sz);
    if (p) {
        memset(p, 0, sz);
        p->nbbits = nbbits;
    }
    return p;
}

static inline void bitarray_free(bitarray_t *ba)
{
    vRsMemFree(ba);
}

static inline void bitarray_set_bit(bitarray_t *ba, uint32_t n)
{
    BIT_INDEX(n);
    if (n < ba->nbbits)
        *(UNIT *)((void *)ba + sizeof(bitarray_t) + index * sizeof(UNIT)) |= (UNIT)(1 << bit);
}

static inline void bitarray_clear_bit(bitarray_t *ba, uint32_t n)
{
    BIT_INDEX(n);
    if (n < ba->nbbits)
        *(UNIT *)((void *)ba + sizeof(bitarray_t) + index * sizeof(UNIT)) &= (UNIT)~(1 << bit);
}

static inline bool_t bitarray_get_bit(bitarray_t *ba, uint32_t n)
{
    BIT_INDEX(n);
    if (n < ba->nbbits)
        return (*(UNIT *)((void *)ba + sizeof(bitarray_t) + index * sizeof(UNIT)) & (UNIT)(1 << bit)) > 0;
}

#endif // _COMMON_BIT_ARRAY_H
