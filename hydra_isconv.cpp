#include "hydra_isconv.h"

namespace hydra {

typedef StringUnit<5, unsigned char> isnode_t;
static isnode_t map_int2str[100000];

/************************** 整数转字符串实现 **************************/
__attribute__((constructor)) static void BuildMap()
{
    // map int to str
    for (unsigned int i = 0; i < 100000; ++i)
        map_int2str[i].len = snprintf(map_int2str[i].str, 6, "%u", i);
}

// 整数(最多10个字节)转字符串, Int10ToStrImpl不内联的话性能会降低
template<typename TInt>
static inline unsigned int Int10ToStrImpl(TInt val, char* dst, bool leading_zero = false)
{
    int val_len = 0;
    unsigned int high = 0;

    if (val >= 100000)
    {
        high = val / 100000;
        val -= (TInt)high * 100000; // 用乘法+减法代替求模
    }

    const isnode_t& hnode = map_int2str[high];
    const isnode_t& lnode = map_int2str[val]; // val is low

    if (leading_zero) // 只有64位整数才会走到这个分支
    {
        *(uint64_t*)dst = *(const uint64_t*)"00000000"; // fill leading zero for high
        dst[8] = '0';
        val_len = (high != 0) ? 5 - hnode.len: 10 - lnode.len; // leading num
        dst += val_len;
    }

    if (high != 0)
    {
        *(unsigned int*)dst = *(const unsigned int*)hnode.s;
        dst[4] = hnode.s[4]; 
        dst += hnode.len;

        *(unsigned int*)dst = *(const unsigned int*)"0000"; // fill leading zero for low
        val_len += hnode.len + 5;
        dst += 5 - lnode.len;
    }
    else
        val_len += lnode.len;

    *(unsigned int*)dst = *(const unsigned int*)lnode.s;
    dst[4] = lnode.s[4];
    dst[lnode.len] = 0;

    return val_len;
}

// 整数(最多20个字节)转字符串
static inline unsigned int Int20ToStrImpl(uint64_t val, char* dst)
{
    if (val > 0XFFFFFFFF)
    {
        uint64_t high = (val >= 10000000000ULL) ? val / 10000000000ULL : 0;
        uint64_t low = val - high * 10000000000ULL;
        unsigned int high_len = (high != 0) ? Int10ToStrImpl((unsigned int)high, dst) : 0;
        if (low > 0XFFFFFFFF) // 这里只能用64位运算，因为超出了unsigned int范围
            return Int10ToStrImpl(low, dst + high_len, high_len != 0) + high_len;
        else // unsigned int恰好可以存下，用32位更快
            return Int10ToStrImpl((unsigned int)low, dst + high_len, true) + high_len;
    }
    else // unsigned int恰好可以存下，用32位更快
        return Int10ToStrImpl((unsigned int)val, dst);
}

#define FAST_UINT_TO_STR(size, type) \
isbuf_t fi2s(type val) \
{ \
    isbuf_t buf; \
    buf.len = Int##size##ToStrImpl(val, buf.s); \
    return buf; \
}

#define FAST_INT_TO_STR(size, type, copy_min) \
isbuf_t fi2s(type val) \
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

#define HYDRA_COPY_INT32_MIN \
        if (INT_MIN == val) /* -2^31 */ \
        { \
            memcpy(buf.s, "-2147483648", 12); \
            buf.len = 11; \
        }

#define HYDRA_COPY_INT64_MIN \
        if (LLONG_MIN == val) /* -2^63 */ \
        { \
            memcpy(buf.s, "-9223372036854775808", 21); \
            buf.len = 20; \
        }

// 接口实例化
FAST_UINT_TO_STR(10, unsigned int)
FAST_UINT_TO_STR(20, uint64_t)
FAST_INT_TO_STR(10, int, HYDRA_COPY_INT32_MIN)
FAST_INT_TO_STR(20, int64_t, HYDRA_COPY_INT64_MIN)
/************************** 整数转字符串实现 **************************/

/************************** 字符串转整数实现 **************************/
#define CONV_ONE_BYTE(index) \
    if ((c = str[index]) < '0' or c > '9') goto end; \
    val = val * 10 + (c ^ '0')
        // 在32位环境下，下面的语句在str to int64时很慢
        // val = (val << 3) + (val << 1) + (c ^ '0')

// isspace貌似不会更快
#define FILTER_SPACE() \
        while ((c = *str) == ' ' or c == '\t' or c == '\n' or c == '\r' or c == '\b' or c == '\f') \
                ++str;

template<typename int_t>
static inline int_t fs2i(const char* str)
{
    if (NULL == str) return 0;

    bool neg = false;
    int c = 0;
        int_t val = 0;

        FILTER_SPACE();
    if (str[0] == '-')
        ++str, neg = true;
    else if (str[0] == '+')
        ++str;
        FILTER_SPACE();

        CONV_ONE_BYTE(0);
        CONV_ONE_BYTE(1);
        CONV_ONE_BYTE(2);
        CONV_ONE_BYTE(3);
        CONV_ONE_BYTE(4);
        CONV_ONE_BYTE(5);
        CONV_ONE_BYTE(6);
        CONV_ONE_BYTE(7);
        CONV_ONE_BYTE(8);
        CONV_ONE_BYTE(9);
        CONV_ONE_BYTE(10);
        CONV_ONE_BYTE(11);
        CONV_ONE_BYTE(12);
        CONV_ONE_BYTE(13);
        CONV_ONE_BYTE(14);
        CONV_ONE_BYTE(15);
        CONV_ONE_BYTE(16);
        CONV_ONE_BYTE(17);
        CONV_ONE_BYTE(18);
        CONV_ONE_BYTE(19); // uint64_t也最多只有20位

end:
    return neg ? -val : val;
}

int64_t fs2i(const char* str) { return fs2i<int64_t>(str); }
int fs2i32(const char* str) { return fs2i<int>(str); }
/************************** 字符串转整数实现 **************************/

}
