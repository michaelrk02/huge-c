#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#define BYTES(width) ((width) / 8 + (size_t)((width) % 8 > 0))

huge_t huge_alloc(size_t width) {
    huge_t huge;

    huge = (huge_t)malloc(sizeof(struct __huge_t));
    assert(huge != NULL);

    assert(width > 0);
    huge->size = BYTES(width);
    huge->bits = (unsigned char *)malloc(huge->size);
    assert(huge->bits != NULL);
    memset(huge->bits, 0, huge->size);

    huge->width = width;
    huge->sign = 0x0;

    return huge;
}

void huge_resize(huge_t huge, size_t width) {
    size_t size_new, width_old;

    assert((huge != NULL) && (width > 0));
    width_old = huge->width;
    size_new = BYTES(width);
    if (size_new != huge->size) {
        unsigned char *bits_new;

        bits_new = (unsigned char *)malloc(size_new);
        assert(bits_new != NULL);

        memcpy(bits_new, huge->bits, MIN(size_new, huge->size));
        free(huge->bits);
        huge->bits = bits_new;
        huge->size = size_new;
    }

    huge->width = width;

    if (width > width_old) {
        size_t i;

        for (i = width_old; i < width; i++) {
            huge_bits_set(huge, i, huge->sign);
        }
    }
}

void huge_euclid_div(huge_t huge, huge_t divisor, huge_t *quotient, huge_t *remainder) {
    huge_t quotient_out, remainder_out, zero;
    size_t i;

    assert((huge != NULL) && (divisor != NULL));
    zero = huge_zero();
    assert(zero != NULL);
    assert(huge_neq(divisor, zero));
    huge_free(zero);

    if (huge->sign == 0x1) {
        huge_compl_2(huge);
    }
    if (divisor->sign == 0x1) {
        huge_compl_2(divisor);
    }

    quotient_out = huge_zero();
    remainder_out = huge_zero();
    for (i = huge->width - 1; (long)i >= 0; i--) {
        huge_lshl(remainder_out, 1, 1);
        huge_bits_set(remainder_out, 0, huge_bits_get(huge, i));
        if (huge_gtr_eq(remainder_out, divisor)) {
            huge_sub_i(remainder_out, divisor);

            if (i >= quotient_out->width) {
                huge_expand(quotient_out, i + 1 - quotient_out->width);
            }

            huge_bits_set(quotient_out, i, 0x1);
        }
    }

    quotient_out->sign = huge->sign ^ divisor->sign;
    if (quotient_out->sign == 0x1) {
        huge_t zero;

        zero = huge_zero();
        if (huge_gtr(remainder_out, zero)) {
            huge_inc(quotient_out);
        }

        huge_free(zero);
    }

    if (huge->sign == 0x1) {
        huge_compl_2(huge);
    }
    if (divisor->sign == 0x1) {
        huge_compl_2(divisor);
    }

    if (quotient != NULL) {
        *quotient = quotient_out;
    } else {
        huge_free(quotient_out);
    }

    if (remainder != NULL) {
        *remainder = remainder_out;
    } else {
        huge_free(remainder_out);
    }
}

void huge_compl_1(huge_t huge) {
    size_t i;

    assert(huge != NULL);
    for (i = 0; i < huge->width; i++) {
        huge_bits_set(huge, i, (unsigned char)(huge_bits_get(huge, i) == 0x0));
    }
}

void huge_compl_2(huge_t huge) {
    huge_t carry;
    unsigned char a, b;
    size_t i;

    assert(huge != NULL);
    huge_compl_1(huge);

    carry = huge_alloc(huge->width);
    assert(carry != NULL);
    a = huge_bits_get(huge, 0);
    b = 0x1;
    huge_bits_set(huge, 0, a ^ b);
    huge_bits_set(carry, 0, a & b);
    for (i = 1; i < huge->width; i++) {
        a = huge_bits_get(huge, i);
        b = huge_bits_get(carry, i - 1);

        huge_bits_set(huge, i, (a ^ b) & 0x1);
        huge_bits_set(carry, i, (a & b) & 0x1);
    }

    huge_free(carry);
}

void huge_lshl(huge_t huge, size_t count, int resize) {
    long i;
    size_t prev_width;

    assert(huge != NULL);
    if (count == 0) {
        return;
    }
    prev_width = huge->width;

    if (resize) {
        huge_expand(huge, count);
    }

    for (i = (long)prev_width - 1 - (resize ? 0 : (long)count); i >= 0; i--) {
        huge_bits_set(huge, (size_t)i + count, huge_bits_get(huge, (size_t)i));
    }
    for (i = 0; i < (long)count; i++) {
        huge_bits_set(huge, (size_t)i, 0x0);
    }
}

void huge_lshr(huge_t huge, size_t count, int resize) {
    size_t i, next_width;

    assert(huge != NULL);
    if (count == 0) {
        return;
    }

    next_width = huge->width - count;
    for (i = 0; i < next_width; i++) {
        huge_bits_set(huge, i, huge_bits_get(huge, i + count));
    }
    if (resize) {
        huge_shrink(huge, count);
    } else {
        for (i = next_width; i < huge->width; i++) {
            huge_bits_set(huge, i, 0x0);
        }
    }
}

unsigned char huge_bits_get(huge_t huge, size_t index) {
    size_t position, offset;

    assert((huge != NULL) && (index < huge->width));
    position = index / 8;
    offset = index % 8;

    assert(huge->bits != NULL);
    return (huge->bits[position] >> offset) & 0x1;
}

unsigned char huge_bits_set(huge_t huge, size_t index, unsigned char value) {
    size_t position, offset;
    unsigned char byte;

    assert((huge != NULL) && (index < huge->width));
    position = index / 8;
    offset = index % 8;

    assert(huge->bits != NULL);
    byte = huge->bits[position];
    byte = byte & ~(unsigned char)(0x1 << offset);
    byte = byte | ((value & 0x1) << offset);
    huge->bits[position] = byte;

    return value & 0x1;
}

