#include "hydra_isconv.h"

namespace hydra {

typedef StringUnit<5, unsigned char> isnode_t;
static isnode_t map_int2str[100000];

/************************** 整数转字符串实现 **************************/
__attribute__((constructor)) static void BuildMap()
{
    // map int to str
    for (unsigned int i = 0; i < 100000; ++i)
        map_int2str[i].l = snprintf(map_int2str[i].s, 6, "%u", i);
}

// 整数(最多10个字节)转字符串, Uint10ToStrImpl不内联的话性能会降低
template<typename uint_t>
static inline unsigned int Uint10ToStrImpl(uint_t val, char* dst, bool leading_zero = false)
{
    unsigned int high, low, val_len = 0;
    if (val >= 100000)
    {
        high = val / 100000;
        low = val - (uint_t)high * 100000; // 用乘法+减法代替求模
    }
    else
    {
        high = 0;
        low = val;
    }

    const isnode_t& hnode = map_int2str[high];
    const isnode_t& lnode = map_int2str[low];

    if (leading_zero) // 只有64位整数才会走到这个分支
    {
        *(uint64_t*)dst = *(const uint64_t*)"00000000"; // 先填充8个前缀'0'
        dst[8] = '0'; // 整体最多9个前缀'0', 先全部填上再说
        val_len = (high != 0) ? 5 - hnode.l: 10 - lnode.l; // 整体前缀'0'的实际数目
        dst += val_len; // 偏移到前缀后面第一个字节
    }

    if (high != 0)
    {
        *(unsigned int*)dst = *(const unsigned int*)hnode.s; // high部分先拷贝4个字节
        dst[4] = hnode.s[4]; // high部分再拷贝1个字节(一个node最多5个字节)
        dst += hnode.l; // 偏移到low部分第一个字节

        *(unsigned int*)dst = *(const unsigned int*)"0000"; // low部分最多4个前缀'0'
        val_len += hnode.l + 5; // 得到整体长度
        dst += 5 - lnode.l; // 偏移到low部分第一个非'0'字节
    }
    else
        val_len += lnode.l; // 得到整体长度

    *(unsigned int*)dst = *(const unsigned int*)lnode.s; // low部分先拷贝4个字节
    dst[4] = lnode.s[4]; // low部分再拷贝1个字节(一个node最多5个字节)
    dst[lnode.l] = 0; // 末尾结束字符'\0'

    return val_len;
}

// 整数(最多20个字节)转字符串
static inline unsigned int Uint20ToStrImpl(uint64_t val, char* dst)
{
    if (val > 0XFFFFFFFF)
    {
        uint64_t high, low;
        if (val >= 10000000000ULL)
        {
            high  = val / 10000000000ULL;
            low = val - high * 10000000000ULL;
        }
        else
        {
            high = 0;
            low = val;
        }

        unsigned int high_len = (high != 0) ? Uint10ToStrImpl((unsigned int)high, dst) : 0;
        if (low > 0XFFFFFFFF) // 这里只能用64位运算，因为超出了unsigned int范围
            return Uint10ToStrImpl(low, dst + high_len, high_len != 0) + high_len;
        else // unsigned int恰好可以存下，用32位更快
            return Uint10ToStrImpl((unsigned int)low, dst + high_len, true) + high_len;
    }
    else // unsigned int恰好可以存下，用32位更快
        return Uint10ToStrImpl((unsigned int)val, dst);
}

// 接口实例化
unsigned int fi2s(int val, char* buf)
{
    if (val < 0)
    {
        buf[0] = '-';
        return Uint10ToStrImpl((unsigned int)-val, buf + 1) + 1;
    }
    else
        return Uint10ToStrImpl((unsigned int)val, buf);
}

unsigned int fi2s(unsigned int val, char* buf)
{
    return Uint10ToStrImpl(val, buf);
}

unsigned int fi2s(int64_t val, char* buf)
{
    if (val < 0)
    {
        buf[0] = '-';
        return Uint20ToStrImpl((uint64_t)-val, buf + 1) + 1;
    }
    else
        return Uint20ToStrImpl((uint64_t)val, buf);
}

unsigned int fi2s(uint64_t val, char* buf)
{
    return Uint20ToStrImpl(val, buf);
}
/************************** 整数转字符串实现 **************************/

/************************** 字符串转整数实现 **************************/
#define CONV_ONE_BYTE(index) \
    if ((c = str[index]) < '0' or c > '9') goto end; \
    val = val * 10 + (c ^ '0')
    // 在32位环境下，下面的语句在str to int64时很慢
    // val = (val << 3) + (val << 1) + (c ^ '0')

#define FILTER_SPACE() \
    while ((c = *str) == ' ' or c == '\t' or c == '\n' or c == '\v' or c == '\f' or c == '\r') \
        ++str;
    // while (isspace(*str)) 要更慢一些

template<typename int_t>
static inline int_t fs2i(const char* str)
{
    if (NULL == str) return 0;

    bool neg = false;
    int c = 0;
    int_t val = 0;

    FILTER_SPACE();
    if ((c = str[0]) == '-')
        ++str, neg = true;
    else if (c == '+')
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
