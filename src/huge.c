#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

huge_t huge_copy(huge_t source) {
    huge_t duplicate;

    assert((source != NULL) && (source->width > 0));
    duplicate = huge_alloc(source->width);
    assert(duplicate != NULL);

    assert(duplicate->width == source->width);
    memcpy(duplicate->bits, source->bits, source->size);

    duplicate->sign = source->sign;

    return duplicate;
}

void huge_move(huge_t destination, huge_t source) {
    assert((destination != NULL) && (source != NULL));
    free(destination->bits);
    destination->bits = source->bits;
    destination->size = source->size;
    destination->width = source->width;
    destination->sign = source->sign;
    source->bits = NULL;
    huge_free(source);
}

huge_t huge_zero(void) {
    return huge_alloc(1);
}

huge_t huge_from_bin(const char *str) {
    huge_t huge;
    size_t width, i;
    unsigned char sign = 0x0;

    assert(str != NULL);
    width = strlen(str);
    assert(width > 0);
    if (str[0] == '-') {
        sign = 0x1;
        width--;
        assert(width > 0);
    }
    huge = huge_alloc(width);
    assert(huge != NULL);
    huge->sign = sign;

    assert(width > 0);
    for (i = 0; i < width; i++) {
        huge_bits_set(huge, width - 1 - i, (unsigned char)(str[i + (size_t)(sign == 0x1)] - '0'));
    }

    if (sign == 0x1) {
        huge_compl_2(huge);
    }

    return huge;
}

huge_t huge_from_oct(const char *str) {
    huge_t huge;
    size_t length, width, i;
    unsigned char sign = 0x0;

    assert(str != NULL);
    length = strlen(str);
    assert(length > 0);
    if (str[0] == '-') {
        sign = 0x1;
        length--;
        assert(length > 0);
    }

    width = length * 3;
    huge = huge_alloc(width);
    assert(huge != NULL);
    huge->sign = sign;

    assert(length > 0);
    for (i = 0; i < length; i++) {
        unsigned char value;
        char ch = str[length - 1 - i + (size_t)(sign == 0x1)];
        size_t j;

        assert(('0' <= ch) && (ch <= '7'));
        value = (ch - '0') & 0x7;

        for (j = 0; j < 3; j++) {
            huge_bits_set(huge, 3 * i + j, (value >> j) & 0x1);
        }
    }

    if (sign == 0x1) {
        huge_compl_2(huge);
    }

    return huge;
}

huge_t huge_from_dec(const char *str) {
    huge_t ten, accum;
    size_t i, length;
    unsigned char sign = 0x0;

    assert(str != NULL);
    length = strlen(str);
    assert(length > 0);
    if (str[0] == '-') {
        sign = 0x1;
        length--;
        assert(length > 0);
    }

    ten = huge_from_bin("1010");
    accum = huge_zero();
    assert(ten != NULL);
    for (i = 0; i < length; i++) {
        unsigned int value;
        char ch = str[i + (size_t)(sign == 0x1)];
        huge_t hvalue;

        assert(('0' <= ch) && (ch <= '9'));
        value = (unsigned int)(ch - '0');
        hvalue = huge_alloc(4);
        assert(hvalue != NULL);
        huge_bits_set(hvalue, 0, value & 0x1);
        huge_bits_set(hvalue, 1, (value >> 1) & 0x1);
        huge_bits_set(hvalue, 2, (value >> 2) & 0x1);
        huge_bits_set(hvalue, 3, (value >> 3) & 0x1);

        huge_mul_i(accum, ten);
        huge_add_i(accum, hvalue);

        huge_free(hvalue);
    }

    if (sign == 0x1) {
        huge_compl_2(accum);
    }
    accum->sign = sign;

    huge_free(ten);

    return accum;
}

huge_t huge_from_hex(const char *str) {
    huge_t huge;
    size_t length, width, i;
    unsigned char sign = 0x0;

    assert(str != NULL);
    length = strlen(str);
    assert(length > 0);
    if (str[0] == '-') {
        sign = 0x1;
        length--;
        assert(length > 0);
    }
    width = length * 4;
    huge = huge_alloc(width);
    assert(huge != NULL);
    huge->sign = sign;

    assert(length > 0);
    for (i = 0; i < length; i++) {
        unsigned char value;
        char ch = str[length - 1 - i + (size_t)(sign == 0x1)];
        size_t j;

        assert((('0' <= ch) && (ch <= '9')) || (('a' <= ch) && (ch <= 'f')) || (('A' <= ch) && (ch <= 'F')));
        if (('0' <= ch) && (ch <= '9')) {
            value = ch - '0';
        } else if (('a' <= ch) && (ch <= 'f')) {
            value = 10 + ch - 'a';
        } else if (('A' <= ch) && (ch <= 'F')) {
            value = 10 + ch - 'A';
        }

        for (j = 0; j < 4; j++) {
            huge_bits_set(huge, 4 * i + j, (value >> j) & 0x1);
        }
    }

    if (sign == 0x1) {
        huge_compl_2(huge);
    }

    return huge;
}

huge_t huge_neg_ex(huge_t value, int inplace) {
    huge_t negative;
    size_t i;
    int zero = 1;

    assert(value != NULL);
    negative = huge_copy(value);
    assert(negative != NULL);

    for (i = 0; i < value->width; i++) {
        if (huge_bits_get(value, i) == 0x1) {
            zero = 0;
            break;
        }
    }

    if (!zero) {
        huge_compl_2(negative);
        if (value->sign == 0x0) {
            negative->sign = 0x1;
        } else {
            negative->sign = 0x0;
        }
    }

    if (inplace) {
        huge_move(value, negative);
        return value;
    }

    return negative;
}

huge_t huge_add_ex(huge_t left, huge_t right, int inplace) {
    huge_t carry, sum, narrow = NULL;
    unsigned char a, b;
    size_t i, extra;

    huge_expand(left, 1);
    huge_expand(right, 1);

    assert((left != NULL) && (right != NULL));
    if (left->width < right->width) {
        narrow = left;
        extra = right->width - left->width;
    } else if (right->width < left->width) {
        narrow = right;
        extra = left->width - right->width;
    }
    if (narrow != NULL) {
        huge_expand(narrow, extra);
    }
    assert(left->width == right->width);

    sum = huge_alloc(left->width);
    carry = huge_alloc(left->width);
    assert((sum != NULL) && (carry != NULL));

    a = huge_bits_get(left, 0);
    b = huge_bits_get(right, 0);
    huge_bits_set(sum, 0, a ^ b);
    huge_bits_set(carry, 0, a & b);
    for (i = 1; i < left->width; i++) {
        unsigned char c;

        a = huge_bits_get(left, i);
        b = huge_bits_get(right, i);
        c = huge_bits_get(carry, i - 1);

        huge_bits_set(sum, i, (c ^ (a ^ b)) & 0x1);
        huge_bits_set(carry, i, (a & b | b & c | a & c) & 0x1);
    }

    if (left->sign ^ right->sign == 0x1) {
        sum->sign = huge_bits_get(carry, carry->width - 1) == 0x0;
    } else {
        sum->sign = left->sign;
    }

    if (huge_bits_get(sum, sum->width - 1) == sum->sign) {
        huge_shrink(sum, 1);
    }

    if (narrow != NULL) {
        huge_shrink(narrow, extra);
    }

    huge_shrink(left, 1);
    huge_shrink(right, 1);

    huge_free(carry);

    if (inplace) {
        huge_move(left, sum);
        return left;
    }

    return sum;
}

huge_t huge_sub_ex(huge_t left, huge_t right, int inplace) {
    huge_t right_neg, sum, narrow = NULL;
    size_t extra;

    right_neg = huge_neg(right);
    assert(right_neg != NULL);

    sum = huge_add(left, right_neg);
    assert(sum != NULL);

    huge_free(right_neg);

    if (inplace) {
        huge_move(left, sum);
        return left;
    }

    return sum;
}

huge_t huge_mul_ex(huge_t left, huge_t right, int inplace) {
    size_t i, accum_width, shrink_width = 0;
    huge_t *parts, accum;

    assert((left != NULL) && (right != NULL));
    if (left->sign == 0x1) {
        huge_compl_2(left);
    }
    if (right->sign == 0x1) {
        huge_compl_2(right);
    }

    accum_width = left->width + right->width;
    accum = huge_alloc(accum_width);
    parts = (huge_t *)malloc(right->width * sizeof(huge_t));
    for (i = 0; i < right->width; i++) {
        size_t j;

        parts[i] = huge_alloc(accum_width);
        for (j = 0; j < left->width; j++) {
            unsigned char a, b;

            a = huge_bits_get(left, j);
            b = huge_bits_get(right, i);
            huge_bits_set(parts[i], j, a & b);
        }
        huge_lshl(parts[i], i, 0);
    }
    for (i = 0; i < right->width; i++) {
        huge_add_i(accum, parts[i]);
        huge_free(parts[i]);
    }
    i = accum->width - 1;
    for (i = accum->width - 1; (long)i >= 0; i--) {
        if (huge_bits_get(accum, i) == 0x1) {
            break;
        }
        shrink_width++;
    }
    if (shrink_width == accum->width) {
        shrink_width--;
    }
    huge_shrink(accum, shrink_width);
    free(parts);

    if (left->sign == 0x1) {
        huge_compl_2(left);
    }
    if (right->sign == 0x1) {
        huge_compl_2(right);
    }

    accum->sign = left->sign ^ right->sign;
    if (accum->sign == 0x1) {
        huge_compl_2(accum);
    }

    if (inplace) {
        huge_move(left, accum);
        return left;
    }

    return accum;
}

huge_t huge_div_ex(huge_t left, huge_t right, int inplace) {
    huge_t quotient;

    assert((left != NULL) && (right != NULL));
    huge_euclid_div(left, right, &quotient, NULL);

    if (inplace) {
        huge_move(left, quotient);
        return left;
    }

    return quotient;
}

huge_t huge_mod_ex(huge_t left, huge_t right, int inplace) {
    huge_t remainder;

    assert((left != NULL) && (right != NULL));
    huge_euclid_div(left, right, NULL, &remainder);

    if (inplace) {
        huge_move(left, remainder);
        return left;
    }

    return remainder;
}

void huge_inc(huge_t huge) {
    huge_t one;

    assert(huge != NULL);
    one = huge_alloc(1);
    assert(one != NULL);
    huge_bits_set(one, 0, 0x1);

    huge_add_i(huge, one);
    huge_free(one);
}

void huge_dec(huge_t huge) {
    huge_t one;

    assert(huge != NULL);
    one = huge_alloc(1);
    assert(one != NULL);
    huge_bits_set(one, 0, 0x1);

    huge_sub_i(huge, one);
    huge_free(one);
}

int huge_cmp(huge_t left, huge_t right) {
    huge_t sum;
    unsigned char sign;
    int equal = 1, result;
    size_t i;

    sum = huge_sub(left, right);
    assert(sum != NULL);
    sign = sum->sign;

    for (i = 0; i < sum->width; i++) {
        if (huge_bits_get(sum, i) == 0x1) {
            equal = 0;
            break;
        }
    }
    if (!equal) {
        result = (sign == 0x1) ? -1 : 1;
    } else {
        result = 0;
    }

    huge_free(sum);

    return result;
}

void huge_free(huge_t huge) {
    if (huge != NULL) {
        if (huge->bits != NULL) {
            free(huge->bits);
        }
        free(huge);
    }
}

void huge_to_bin(huge_t huge, char *str, size_t *size) {
    size_t size_out;

    assert(huge != NULL);
    size_out = huge->width + 1;
    if (huge->sign == 0x1) {
        size_out++;
    }
    if (size != NULL) {
        *size = size_out;
    }

    if (str != NULL) {
        size_t i, pad = 0;

        memset(str, 0, size_out);
        if (huge->sign == 0x1) {
            pad = 1;
            *str = '-';
            huge_compl_2(huge);
        }
        for (i = 0; i < huge->width; i++) {
            *(str + pad + huge->width - 1 - i) = '0' + (char)huge_bits_get(huge, i);
        }
        if (huge->sign == 0x1) {
            huge_compl_2(huge);
        }
    }
}

void huge_to_oct(huge_t huge, char *str, size_t *size) {
    size_t oct_width, size_out;

    assert(huge != NULL);
    oct_width = huge->width / 3 + (size_t)(huge->width % 3 > 0);
    size_out = oct_width + 1;
    if (huge->sign == 0x1) {
        size_out++;
    }
    if (size != NULL) {
        *size = size_out;
    }

    if (str != NULL) {
        size_t i, pad = 0;

        memset(str, 0, size_out);
        if (huge->sign == 0x1) {
            pad = 1;
            *str = '-';
            huge_compl_2(huge);
        }
        for (i = 0; i < oct_width; i++) {
            unsigned char value;
            size_t j;

            value = 0x0;
            for (j = 0; j < 3; j++) {
                size_t index;

                index = 3 * i + j;
                if (index < huge->width) {
                    value = value | (huge_bits_get(huge, index) << j);
                }
            }

            *(str + pad + oct_width - 1 - i) = '0' + (char)value;
        }
        if (huge->sign == 0x1) {
            huge_compl_2(huge);
        }
    }
}

void huge_to_dec(huge_t huge, char *str, size_t *size) {
    size_t i, dec_width = 0, size_out = 1;
    huge_t temp, zero, ten;
    unsigned char sign;

    assert(huge != NULL);
    sign = huge->sign;
    zero = huge_zero();
    ten = huge_from_bin("1010");
    assert((zero != NULL) && (ten != NULL));
    if (sign == 0x1) {
        size_out++;
        huge_compl_2(huge);
        huge->sign = 0x0;
    }

    temp = huge_copy(huge);
    assert(temp != NULL);
    while (huge_gtr(temp, zero)) {
        dec_width++;
        huge_div_i(temp, ten);
    }
    huge_free(temp);

    size_out = size_out + dec_width;
    if (size != NULL) {
        *size = size_out;
    }
    if (str != NULL) {
        size_t i, pad = (size_t)(sign == 0x1);

        memset(str, 0, size_out);
        if (sign == 0x1) {
            *str = '-';
        }

        temp = huge_copy(huge);
        assert(temp != NULL);
        i = 0;
        while ((i < dec_width) && huge_gtr(temp, zero)) {
            huge_t rem;
            unsigned int value = 0;

            rem = huge_mod(temp, ten);
            assert(rem->width > 0);
            value |= (huge_bits_get(rem, 0));
            if (rem->width >= 2) {
                value |= (huge_bits_get(rem, 1) << 1);
            }
            if (rem->width >= 3) {
                value |= (huge_bits_get(rem, 2) << 2);
            }
            if (rem->width >= 4) {
                value |= (huge_bits_get(rem, 3) << 3);
            }
            *(str + pad + dec_width - 1 - i) = (char)value + '0';

            huge_div_i(temp, ten);
            huge_free(rem);

            i++;
        }
        if (dec_width == 0) {
            *str = '0';
        }
        huge_free(temp);
    }

    if (sign == 0x1) {
        huge_compl_2(huge);
        huge->sign = sign;
    }

    huge_free(ten);
    huge_free(zero);
}

void huge_to_hex(huge_t huge, char *str, size_t *size) {
    size_t hex_width, size_out;

    assert(huge != NULL);
    hex_width = huge->width / 4 + (size_t)(huge->width % 4 > 0);
    size_out = hex_width + 1;
    if (huge->sign == 0x1) {
        size_out++;
    }
    if (size != NULL) {
        *size = size_out;
    }

    if (str != NULL) {
        size_t i, pad = 0;

        memset(str, 0, size_out);
        if (huge->sign == 0x1) {
            pad = 1;
            *str = '-';
            huge_compl_2(huge);
        }
        for (i = 0; i < hex_width; i++) {
            unsigned char value;
            size_t j;

            value = 0x0;
            for (j = 0; j < 4; j++) {
                size_t index;

                index = 4 * i + j;
                if (index < huge->width) {
                    value = value | (huge_bits_get(huge, index) << j);
                }
            }

            if (value < 10) {
                *(str + pad + hex_width - 1 - i) = '0' + (char)value;
            } else {
                *(str + pad + hex_width - 1 - i) = 'a' + (char)(value - 10);
            }
        }
        if (huge->sign == 0x1) {
            huge_compl_2(huge);
        }
    }
}

