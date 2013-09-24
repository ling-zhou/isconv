/// TODO: license
/// int <=> str 快速转换实用接口.\n
/// 优点：1. 快. a) i2s比snprintf快8到35+倍; b) s2i比ato*快15到30+倍
///       2. 好用. a) i2s不用先声明一个buffer; b) s2i提供char*, string重载
///       3. 安全. 不会出现错误书写"%d"之类的fmt导致的错误.
/// @file
/// @date 2013-09-24 12:55:13 
/// @version 1.0.0
/// @author ling-zhou(周龄), nullzhou@foxmail.com
/// @namespace isconv

#ifndef __ISCONV_H__
#define __ISCONV_H__

#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include "isconv_base_define.h"

namespace isconv {

typedef StringUnit<25, uchar> isbuf_t;
typedef StringUnit<5, uchar>  isnode_t;

/// @brief 整数到字符串的字典映射
extern isnode_t g_int_to_str[100000];

/// @brief 字符到整数的字典映射, 
///  比如g_mul_10_1['8']对应8 * 10^1, g_mul_10_8['5']对应5 * 10^8.
extern uchar  g_mul_10_1[58];
extern ushort g_mul_10_2[58];
extern ushort g_mul_10_3[58];
extern uint   g_mul_10_4[58];
extern uint   g_mul_10_5[58];
extern uint   g_mul_10_6[58];
extern uint   g_mul_10_7[58];
extern uint   g_mul_10_8[58];
extern uint64 g_mul_10_9[58]; 
extern uint64 g_mul_10_10[58];
extern uint64 g_mul_10_11[58];
extern uint64 g_mul_10_12[58];
extern uint64 g_mul_10_13[58];
extern uint64 g_mul_10_14[58];
extern uint64 g_mul_10_15[58];
extern uint64 g_mul_10_16[58];
extern uint64 g_mul_10_17[58];
extern uint64 g_mul_10_18[58];
extern uint64 g_mul_10_19[58];

/************************** 整数转字符串 实现开始 *****************************/
/// @brief int to str: 10个字节整数转字符串
template<typename TInt>
static inline uint Int10ToStrImpl(TInt val, char* dst, bool leading_zero = false)
{
    int val_len = 0;
    uint high = 0;

    if (val >= 100000)
    {
        high = val / 100000;
        val -= (TInt)high * 100000; // 用乘法+减法代替求模
    }

    const isnode_t& hnode = g_int_to_str[high];
    const isnode_t& lnode = g_int_to_str[val];

    if (leading_zero)
    {
        *(uint64*)dst = *(const uint64*)"00000000"; // fill leading zero for high
        dst[8] = '0';
        val_len = (high != 0) ? 10 - hnode.len - lnode.len : 10 - lnode.len;
        dst += val_len;
    }

    if (high != 0)
    {
        *(uint*)dst = *(const uint*)hnode.s;
        dst[4] = hnode.s[4]; 
        dst += hnode.len;

        *(uint*)dst = *(const uint*)"0000"; // fill leading zero for low
        val_len += hnode.len + 5;
        dst += 5 - lnode.len;
    }
    else
        val_len += lnode.len;

    *(uint*)dst = *(const uint*)lnode.s;
    dst[4] = lnode.s[4];
    dst[lnode.len] = 0;

    return val_len;
}

/// @brief int to str: 20个字节整数转字符串
static inline uint Int20ToStrImpl(uint64 val, char* dst)
{
    if (val > 0XFFFFFFFF)
    {
        uint64 high = (val >= 10000000000ULL) ? val / 10000000000ULL : 0;
        uint64 low = val - high * 10000000000ULL;
        uint len = (high != 0) ? Int10ToStrImpl((uint)high, dst) : 0;
        if (low > 0XFFFFFFFF) // 这里只能用64位运算，因为超出了uint范围
            return Int10ToStrImpl(low, dst + len, true) + len;
        else // uint恰好可以存下，用32位更快
            return Int10ToStrImpl((uint)low, dst + len, true) + len;
    }
    else // uint恰好可以存下，用32位更快
        return Int10ToStrImpl((uint)val, dst);
}

#define FAST_UINT_TO_STR(size, type) \
static inline isbuf_t FastIntToStr(type val) \
{ \
    isbuf_t buf; \
    buf.len = Int##size##ToStrImpl(val, buf.s); \
    return buf; \
}

#define FAST_INT_TO_STR(size, type, copy_min) \
static inline isbuf_t FastIntToStr(type val) \
{ \
    isbuf_t buf; \
 \
    if (val < 0) \
    { \
        copy_min \
        else \
        { \
            buf.s[0] = '-'; \
            buf.len = Int##size##ToStrImpl(-val, buf.s + 1) + 1; \
        } \
    } \
    else \
        buf.len = Int##size##ToStrImpl(val, buf.s); \
 \
    return buf; \
}

#define COPY_INT32_MIN \
        if (INT_MIN == val) /* -2^31 */ \
        { \
            memcpy(buf.s, "-2147483648", 12); \
            buf.len = 11; \
        }

#define COPY_INT64_MIN \
        if (LLONG_MIN == val) /* -2^63 */ \
        { \
            memcpy(buf.s, "-9223372036854775808", 21); \
            buf.len = 20; \
        }
/************************** 整数转字符串 实现结束 *****************************/

/************************** 字符串转整数 实现开始 *****************************/
#define TO_NUM(v) (v ^ '0')
#define IS_NUM(v) (v >= '0' && v <= '9')
static inline uint64 StrToIntImpl(const char* in)
{
    const uchar* str = (const uchar*)in;
    // 整数最多20字节(参照ULLONG_MAX)
    if (not IS_NUM(str[0]))
        return 0;
    else if (not IS_NUM(str[1]))
        return TO_NUM(str[0]);
    else if (not IS_NUM(str[2]))
        return g_mul_10_1[str[0]] + TO_NUM(str[1]);
    else if (not IS_NUM(str[3]))
        return g_mul_10_2[str[0]] + g_mul_10_1[str[1]] + TO_NUM(str[2]);
    else if (not IS_NUM(str[4]))
    {
        uint a = g_mul_10_3[str[0]] + g_mul_10_2[str[1]];
        uint b = g_mul_10_1[str[2]] + TO_NUM(str[3]);
        return a + b;
    }
    else if (not IS_NUM(str[5]))
    {
        uint a = g_mul_10_4[str[0]] + g_mul_10_3[str[1]];
        uint b = g_mul_10_2[str[2]] + g_mul_10_1[str[3]];
        return a + b + TO_NUM(str[4]);
    }
    else if (not IS_NUM(str[6]))
    {
        uint a = g_mul_10_5[str[0]] + g_mul_10_4[str[1]];
        uint b = g_mul_10_3[str[2]] + g_mul_10_2[str[3]];
        uint c = g_mul_10_1[str[4]] + TO_NUM(str[5]);
        return a + b + c;
    }
    else if (not IS_NUM(str[7]))
    {
        uint a = g_mul_10_6[str[0]] + g_mul_10_5[str[1]];
        uint b = g_mul_10_4[str[2]] + g_mul_10_3[str[3]];
        uint c = g_mul_10_2[str[4]] + g_mul_10_1[str[5]];
        uint d = a + b;
        uint e = c + TO_NUM(str[6]);
        return d + e;
    }
    else if (not IS_NUM(str[8]))
    {
        uint a = g_mul_10_7[str[0]] + g_mul_10_6[str[1]];
        uint b = g_mul_10_5[str[2]] + g_mul_10_4[str[3]];
        uint c = g_mul_10_3[str[4]] + g_mul_10_2[str[5]];
        uint d = g_mul_10_1[str[6]] + TO_NUM(str[7]);
        uint e = a + b;
        uint f = c + d;
        return e + f;
    }
    else if (not IS_NUM(str[9]))
    {
        uint a = g_mul_10_8[str[0]] + g_mul_10_7[str[1]];
        uint b = g_mul_10_6[str[2]] + g_mul_10_5[str[3]];
        uint c = g_mul_10_4[str[4]] + g_mul_10_3[str[5]];
        uint d = g_mul_10_2[str[6]] + g_mul_10_1[str[7]];
        uint e = a + b;
        uint f = c + d;
        return e + f + TO_NUM(str[8]);
    }
    else if (not IS_NUM(str[10]))
    {
        uint64 a = g_mul_10_9[str[0]] + g_mul_10_8[str[1]];
        uint   b = g_mul_10_7[str[2]] + g_mul_10_6[str[3]];
        uint   c = g_mul_10_5[str[4]] + g_mul_10_4[str[5]];
        uint   d = g_mul_10_3[str[6]] + g_mul_10_2[str[7]];
        uint   e = g_mul_10_1[str[8]] + TO_NUM(str[9]);
        uint   f = b + c;
        uint   g = d + e;
        return a + f + g;
    }
    else if (not IS_NUM(str[11]))
    {
        uint64 a = g_mul_10_10[str[0]] + g_mul_10_9[str[1]];
        uint   b = g_mul_10_8[str[2]]  + g_mul_10_7[str[3]];
        uint   c = g_mul_10_6[str[4]]  + g_mul_10_5[str[5]];
        uint   d = g_mul_10_4[str[6]]  + g_mul_10_3[str[7]];
        uint   e = g_mul_10_2[str[8]]  + g_mul_10_1[str[9]];
        uint64 f = a + b;
        uint   g = c + d;
        uint   h = e + TO_NUM(str[10]);
        return f + g + h;
    }
    else if (not IS_NUM(str[12]))
    {
        uint64 a = g_mul_10_11[str[0]] + g_mul_10_10[str[1]];
        uint64 b = g_mul_10_9[str[2]]  + g_mul_10_8[str[3]];
        uint   c = g_mul_10_7[str[4]]  + g_mul_10_6[str[5]];
        uint   d = g_mul_10_5[str[6]]  + g_mul_10_4[str[7]];
        uint   e = g_mul_10_3[str[8]]  + g_mul_10_2[str[9]];
        uint   f = g_mul_10_1[str[10]] + TO_NUM(str[11]);
        uint64 g = a + b;
        uint   h = c + d;
        uint   i = e + f;
        return g + h + i;
    }
    else if (not IS_NUM(str[13]))
    {
        uint64 a = g_mul_10_12[str[0]] + g_mul_10_11[str[1]];
        uint64 b = g_mul_10_10[str[2]] + g_mul_10_9[str[3]];
        uint   c = g_mul_10_8[str[4]]  + g_mul_10_7[str[5]];
        uint   d = g_mul_10_6[str[6]]  + g_mul_10_5[str[7]];
        uint   e = g_mul_10_4[str[8]]  + g_mul_10_3[str[9]];
        uint   f = g_mul_10_2[str[10]] + g_mul_10_1[str[11]];
        uint64 g = a + b;
        uint   h = c + d;
        uint   i = e + f;
        return g + h + i + TO_NUM(str[12]);
    }
    else if (not IS_NUM(str[14]))
    {
        uint64 a = g_mul_10_13[str[0]] + g_mul_10_12[str[1]];
        uint64 b = g_mul_10_11[str[2]] + g_mul_10_10[str[3]];
        uint64 c = g_mul_10_9[str[4]]  + g_mul_10_8[str[5]];
        uint   d = g_mul_10_7[str[6]]  + g_mul_10_6[str[7]];
        uint   e = g_mul_10_5[str[8]]  + g_mul_10_4[str[9]];
        uint   f = g_mul_10_3[str[10]] + g_mul_10_2[str[11]];
        uint   g = g_mul_10_1[str[12]] + TO_NUM(str[13]);
        uint64 h = a + b;
        uint64 i = c + d;
        uint   j = e + f;
        return h + i + j + g;
    }
    else if (not IS_NUM(str[15]))
    {
        uint64 a = g_mul_10_14[str[0]] + g_mul_10_13[str[1]];
        uint64 b = g_mul_10_12[str[2]] + g_mul_10_11[str[3]];
        uint64 c = g_mul_10_10[str[4]] + g_mul_10_9[str[5]];
        uint   d = g_mul_10_8[str[6]]  + g_mul_10_7[str[7]];
        uint   e = g_mul_10_6[str[8]]  + g_mul_10_5[str[9]];
        uint   f = g_mul_10_4[str[10]] + g_mul_10_3[str[11]];
        uint   g = g_mul_10_2[str[12]] + g_mul_10_1[str[13]];
        uint64 h = a + b;
        uint64 i = c + d;
        uint   j = e + f;
        uint   k = g + TO_NUM(str[14]);
        return h + i + j + k;
    }
    else if (not IS_NUM(str[16]))
    {
        uint64 a = g_mul_10_15[str[0]] + g_mul_10_14[str[1]];
        uint64 b = g_mul_10_13[str[2]] + g_mul_10_12[str[3]];
        uint64 c = g_mul_10_11[str[4]] + g_mul_10_10[str[5]];
        uint64 d = g_mul_10_9[str[6]]  + g_mul_10_8[str[7]];
        uint   e = g_mul_10_7[str[8]]  + g_mul_10_6[str[9]];
        uint   f = g_mul_10_5[str[10]] + g_mul_10_4[str[11]];
        uint   g = g_mul_10_3[str[12]] + g_mul_10_2[str[13]];
        uint   h = g_mul_10_1[str[14]] + TO_NUM(str[15]);
        uint64 i = a + b;
        uint64 j = c + d;
        uint   k = e + f;
        uint   l = g + h;
        return i + j + k + l;
    }
    else if (not IS_NUM(str[17]))
    {
        uint64 a = g_mul_10_16[str[0]] + g_mul_10_15[str[1]];
        uint64 b = g_mul_10_14[str[2]] + g_mul_10_13[str[3]];
        uint64 c = g_mul_10_12[str[4]] + g_mul_10_11[str[5]];
        uint64 d = g_mul_10_10[str[6]] + g_mul_10_9[str[7]];
        uint   e = g_mul_10_8[str[8]]  + g_mul_10_7[str[9]];
        uint   f = g_mul_10_6[str[10]] + g_mul_10_5[str[11]];
        uint   g = g_mul_10_4[str[12]] + g_mul_10_3[str[13]];
        uint   h = g_mul_10_2[str[14]] + g_mul_10_1[str[15]];
        uint64 i = a + b;
        uint64 j = c + d;
        uint   k = e + f;
        uint   l = g + h;
        return i + j + k + l + TO_NUM(str[16]);
    }
    else if (not IS_NUM(str[18]))
    {
        uint64 a = g_mul_10_17[str[0]] + g_mul_10_16[str[1]];
        uint64 b = g_mul_10_15[str[2]] + g_mul_10_14[str[3]];
        uint64 c = g_mul_10_13[str[4]] + g_mul_10_12[str[5]];
        uint64 d = g_mul_10_11[str[6]] + g_mul_10_10[str[7]];
        uint64 e = g_mul_10_9[str[8]]  + g_mul_10_8[str[9]];
        uint   f = g_mul_10_7[str[10]] + g_mul_10_6[str[11]];
        uint   g = g_mul_10_5[str[12]] + g_mul_10_4[str[13]];
        uint   h = g_mul_10_3[str[14]] + g_mul_10_2[str[15]];
        uint   i = g_mul_10_1[str[16]] + TO_NUM(str[17]);
        uint64 j = a + b;
        uint64 k = c + d;
        uint64 l = e + f;
        uint   m = g + h;
        return j + k + l + m + i;
    }
    else if (not IS_NUM(str[19]))
    {
        uint64 a = g_mul_10_18[str[0]] + g_mul_10_17[str[1]];
        uint64 b = g_mul_10_16[str[2]] + g_mul_10_15[str[3]];
        uint64 c = g_mul_10_14[str[4]] + g_mul_10_13[str[5]];
        uint64 d = g_mul_10_12[str[6]] + g_mul_10_11[str[7]];
        uint64 e = g_mul_10_10[str[8]] + g_mul_10_9[str[9]];
        uint   f = g_mul_10_8[str[10]] + g_mul_10_7[str[11]];
        uint   g = g_mul_10_6[str[12]] + g_mul_10_5[str[13]];
        uint   h = g_mul_10_4[str[14]] + g_mul_10_3[str[15]];
        uint   i = g_mul_10_2[str[16]] + g_mul_10_1[str[17]];
        uint64 j = a + b;
        uint64 k = c + d;
        uint64 l = e + f;
        uint   m = g + h;
        uint   n = i + TO_NUM(str[18]);
        return j + k + l + m + n;
    }
    else if (not IS_NUM(str[20]))
    {
        uint64 a = g_mul_10_19[str[0]] + g_mul_10_18[str[1]];
        uint64 b = g_mul_10_17[str[2]] + g_mul_10_16[str[3]];
        uint64 c = g_mul_10_15[str[4]] + g_mul_10_14[str[5]];
        uint64 d = g_mul_10_13[str[6]] + g_mul_10_12[str[7]];
        uint64 e = g_mul_10_11[str[8]] + g_mul_10_10[str[9]];
        uint64 f = g_mul_10_9[str[10]] + g_mul_10_8[str[11]];
        uint   g = g_mul_10_7[str[12]] + g_mul_10_6[str[13]];
        uint   h = g_mul_10_5[str[14]] + g_mul_10_4[str[15]];
        uint   i = g_mul_10_3[str[16]] + g_mul_10_2[str[17]];
        uint   j = g_mul_10_1[str[18]] + TO_NUM(str[19]);
        uint64 k = a + b;
        uint64 l = c + d;
        uint64 m = e + f;
        uint   n = g + h;
        uint   o = i + j;
        return k + l + m + n + o;
    }
    else
        return 0;
}
/************************** 字符串转整数 实现结束 *****************************/

/************************ 整数转字符串 接口实例化开始 *************************/
// 测试表明(取决于测试数据和编译优化等级, 这里优化选项是-O2)：
//           针对int32, FastIntToStr比snprintf快25到35+倍，
//           针对int64，FastIntToStr比snprintf快8到16+倍.
// integer to string实例化
FAST_UINT_TO_STR(10, uint)
FAST_UINT_TO_STR(20, uint64)
FAST_INT_TO_STR(10, int, COPY_INT32_MIN)
FAST_INT_TO_STR(20, int64, COPY_INT64_MIN)

/// @brief fast integer to string interface
static inline isbuf_t fi2s(int val) { return FastIntToStr(val); }
static inline isbuf_t fi2s(uint val) { return FastIntToStr(val); }
static inline isbuf_t fi2s(int64 val) { return FastIntToStr(val); }
static inline isbuf_t fi2s(uint64 val) { return FastIntToStr(val); }
#if __WORDSIZE == 32
static inline isbuf_t fi2s(long val) { return FastIntToStr((int)val); }
static inline isbuf_t fi2s(ulong val) { return FastIntToStr((uint)val); }
#endif
// example: printf("%s", fi2s(345).s);
/************************ 整数转字符串 接口实例化结束 *************************/

/************************ 字符串转整数 接口实例化开始 *************************/
// 测试表明(取决于测试数据和编译优化等级, 这里优化选项是-O2)：
//           针对int32, s2i比atoi快15到20+倍
//           针对int64, s2i比atoll快20到30+倍
// 注意事项：只限于转换十进制，比如："12345",
//           转换"12345 321" toint 后为12345,
//           转换"-12345" toint 后为-12345
//           转换"012345" toint 后为12345
// string to integer实例化
static inline int64 FastStrToInt64(const char* str)
{
    if (NULL == str) return 0;
#ifdef ENABLE_SPACE_FILTER
    // 不加此while循环，性能比ato*高20~30+倍!
    // 加此while循环，性能只比ato*高3~4倍.
    // 应该是此循环阻碍了gcc的优化策略.
    while (isspace(*str)) ++str;
#endif
    return ('-' == str[0]) ? -StrToIntImpl(str + 1)
            : (('+' == str[0]) ? StrToIntImpl(str + 1) : StrToIntImpl(str));
}

/// @brief string to integer interface
static inline int64  fs2i64(const char* str) { return FastStrToInt64(str); }
static inline int64  fs2i64(const std::string& str) { return fs2i64(str.c_str()); }
static inline uint64 fs2u64(const char* str) { return fs2i64(str); }
static inline uint64 fs2u64(const std::string& str) { return fs2u64(str.c_str()); }
static inline int    fs2i32(const char* str) { return (int)fs2i64(str); }
static inline int    fs2i32(const std::string& str) { return fs2i32(str.c_str()); }
static inline uint   fs2u32(const char* str) { return (uint)fs2u64(str); }
static inline uint   fs2u32(const std::string& str) { return fs2u32(str.c_str()); }
static inline int64  fs2i(const char* str) { return fs2i64(str); }
static inline int64  fs2i(const std::string& str) { return fs2i64(str); }
static inline uint64 fs2u(const char* str) { return fs2u64(str); }
static inline uint64 fs2u(const std::string& str) { return fs2u64(str); }
static inline long   fs2l(const char* str) { return (long)fs2i64(str); }
static inline long   fs2l(const std::string& str) { return fs2l(str.c_str()); }
static inline ulong  fs2ul(const char* str) { return (ulong)fs2u64(str); }
static inline ulong  fs2ul(const std::string& str) { return fs2ul(str.c_str()); }
// example: int i = fs2i("-12345");
//          uint u = fs2u("12345");
/************************ 字符串转整数 接口实例化结束 *************************/
}

#endif
