#ifndef __HUGE_INTERNAL_H__
#define __HUGE_INTERNAL_H__

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#include <huge.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define INV(x) (~(x) & 0x1)

struct __huge_t {
    unsigned char *bits;
    size_t size;
    size_t width;
    unsigned char sign;
};

extern HUGEAPI huge_t huge_alloc(size_t width);
extern HUGEAPI void huge_resize(huge_t huge, size_t width);
#define huge_expand(huge, offset) huge_resize((huge), (huge)->width + (offset))
#define huge_shrink(huge, offset) huge_resize((huge), (huge)->width - (offset))

extern HUGEAPI void huge_euclid_div(huge_t huge, huge_t divisor, huge_t *quotient, huge_t *remainder);

extern HUGEAPI void huge_compl_1(huge_t huge);
extern HUGEAPI void huge_compl_2(huge_t huge);

extern HUGEAPI void huge_lshl(huge_t huge, size_t count, int resize);
extern HUGEAPI void huge_lshr(huge_t huge, size_t count, int resize);

extern HUGEAPI unsigned char huge_bits_get(huge_t huge, size_t index);
extern HUGEAPI unsigned char huge_bits_set(huge_t huge, size_t index, unsigned char value);

#endif
