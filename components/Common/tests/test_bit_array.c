#include "bit_array.h"
#include "portability/port.h"

void testBitArrayBasics()
{
    bitarray_t *ba;

    ba = bitarray_alloc(33);

    bitarray_set_bit(ba, 0);
    bitarray_set_bit(ba, 1);
    bitarray_set_bit(ba, 2);
    bitarray_set_bit(ba, 32);

    /* Not allowed since the array has 33 bits and is 0 indexed, but
     * should be an harmless no-op. */
    bitarray_set_bit(ba, 100);

    RsAssert(bitarray_get_bit(ba, 1));
    RsAssert(bitarray_get_bit(ba, 2));
    RsAssert(bitarray_get_bit(ba, 32));

    /* Cheating on an opaque data structure! Don't do that! */
    RsAssert(*(uint32_t *)((void *)ba + sizeof(bitarray_t)) == 7);

    bitarray_clear_bit(ba, 1);
    bitarray_clear_bit(ba, 2);

    RsAssert(*(uint32_t *)((void *)ba + sizeof(bitarray_t)) == 1);

    RsAssert(bitarray_get_bit(ba, 32));

    bitarray_free(ba);
}

void testBitArrayLarge()
{
    bitarray_t *ba;

    ba = bitarray_alloc(10000);

    for (int i = 0; i < 10000; i++)
        bitarray_set_bit(ba, i);

    for (int i = 0; i < (10000 / 32) - 1; i++)
        RsAssert(*(uint32_t *)((void *)ba + sizeof(bitarray_t) + i * sizeof(uint32_t)) == -1);

    bitarray_free(ba);
}

int main() {
    testBitArrayBasics();
    testBitArrayLarge();
}
