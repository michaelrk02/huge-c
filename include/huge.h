#ifndef __HUGE_H__
#define __HUGE_H__

#ifdef HUGE_BUILD
#ifdef _MSC_VER
#define HUGEAPI __declspec(dllexport)
#else
#ifdef _WIN32
#define HUGEAPI __attribute__((dllexport))
#else
#define HUGEAPI __attribute__((visibility("default")))
#endif
#endif
#else
#ifdef _MSC_VER
#define HUGEAPI __declspec(dllimport)
#else
#ifdef _WIN32
#define HUGEAPI __attribute__((dllimport))
#else
#define HUGEAPI
#endif
#endif
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __huge_t *huge_t;

extern HUGEAPI huge_t huge_copy(huge_t source);
extern HUGEAPI void huge_move(huge_t destination, huge_t source);

extern HUGEAPI huge_t huge_zero(void);

extern HUGEAPI huge_t huge_from_bin(const char *str);
extern HUGEAPI huge_t huge_from_oct(const char *str);
extern HUGEAPI huge_t huge_from_dec(const char *str);
extern HUGEAPI huge_t huge_from_hex(const char *str);

extern HUGEAPI huge_t huge_neg_ex(huge_t value, int inplace);
extern HUGEAPI huge_t huge_add_ex(huge_t left, huge_t right, int inplace);
extern HUGEAPI huge_t huge_sub_ex(huge_t left, huge_t right, int inplace);
extern HUGEAPI huge_t huge_mul_ex(huge_t left, huge_t right, int inplace);
extern HUGEAPI huge_t huge_div_ex(huge_t left, huge_t right, int inplace);
extern HUGEAPI huge_t huge_mod_ex(huge_t left, huge_t right, int inplace);

extern HUGEAPI void huge_inc(huge_t huge);
extern HUGEAPI void huge_dec(huge_t huge);

#define huge_neg(x)     huge_neg_ex((x), 0)
#define huge_add(a, b)  huge_add_ex((a), (b), 0)
#define huge_sub(a, b)  huge_sub_ex((a), (b), 0)
#define huge_mul(a, b)  huge_mul_ex((a), (b), 0)
#define huge_div(a, b)  huge_div_ex((a), (b), 0)
#define huge_mod(a, b)  huge_mod_ex((a), (b), 0)

#define huge_neg_i(x)       huge_neg_ex((x), 1)
#define huge_add_i(x, t)    huge_add_ex((x), (t), 1)
#define huge_sub_i(x, t)    huge_sub_ex((x), (t), 1)
#define huge_mul_i(x, t)    huge_mul_ex((x), (t), 1)
#define huge_div_i(x, t)    huge_div_ex((x), (t), 1)
#define huge_mod_i(x, t)    huge_mod_ex((x), (t), 1)

extern HUGEAPI int huge_cmp(huge_t left, huge_t right);
#define huge_eq(a, b)       (huge_cmp(a, b) == 0)
#define huge_neq(a, b)      (huge_cmp(a, b) != 0)
#define huge_gtr(a, b)      (huge_cmp(a, b) > 0)
#define huge_gtr_eq(a, b)   (huge_cmp(a, b) >= 0)
#define huge_lss(a, b)      (huge_cmp(a, b) < 0)
#define huge_lss_eq(a, b)   (huge_cmp(a, b) <= 0)

extern HUGEAPI void huge_free(huge_t huge);

extern HUGEAPI void huge_to_bin(huge_t value, char *str, size_t *size);
extern HUGEAPI void huge_to_oct(huge_t value, char *str, size_t *size);
extern HUGEAPI void huge_to_dec(huge_t value, char *str, size_t *size);
extern HUGEAPI void huge_to_hex(huge_t value, char *str, size_t *size);

#ifdef __cplusplus
}
#endif

#endif
