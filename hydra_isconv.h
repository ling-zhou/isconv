/// int <=> str 快速转换实用接口.
/// 优点：1) 快: fi2s比snprintf快一个数量级.
///       2) 好用: fi2s不用先声明一个buffer; fs2i提供char*, string重载.
///       3) 安全: 不会出现错误书写fmt(如"%d")导致的错误.
/// @file
/// @date 2013-09-24 12:55:13 
/// @version 1.0.0
/// @author ling-zhou(周龄), master@manpage.cn
/// @namespace hydra

#ifndef __HYDRA_ISCONV_H__
#define __HYDRA_ISCONV_H__ \
        "$Id: hydra_isconv.h 2137 2014-08-11 09:22:02Z baozhou $"

#include <stdint.h>
#include <string>

#if __WORDSIZE == 32   
#define _FASTCALL_          __attribute__((__fastcall__))
#else
#define _FASTCALL_
#endif

namespace hydra {

template <int SIZE, typename TLen>
struct StringUnit
{
    union
    {
        char str[SIZE];
        char s[SIZE];   // alias name for str
    };

    union
    {
        TLen len;
        TLen l;   // alias name for len
    };
};

typedef StringUnit<25, unsigned char> isbuf_t;

/**************************** convert int to str ******************************/
_FASTCALL_ isbuf_t fi2s(int val);
_FASTCALL_ isbuf_t fi2s(unsigned int val);
_FASTCALL_ isbuf_t fi2s(int64_t val);
_FASTCALL_ isbuf_t fi2s(uint64_t val);
#if __WORDSIZE == 32
static inline isbuf_t fi2s(long val) { return fi2s((int)val); }
static inline isbuf_t fi2s(unsigned long val) { return fi2s((unsigned int)val); }
#else
static inline isbuf_t fi2s(long long val) { return fi2s((int64_t)val); }
static inline isbuf_t fi2s(unsigned long long val) { return fi2s((uint64_t)val); }
#endif
// usage: printf("%s %s %s\n", fi2s(12345).s, fi2s(321).s, fi2s(888888888888LL).s);
/**************************** convert int to str ******************************/

/**************************** convert str to int ******************************/
// 注意事项：1. 只限于转换十进制，比如："12345",
//           2. 转换"12345 321"后为12345,
//           3. 转换"-12345"后为-12345
//           4. 转换"+12345"后为12345
//           5. 转换"012345"后为12345
//           6. 如果确定整数在(unsigned)int范围内，则调用fs2i32更快
//           7. unsigned用int/int64返回可能溢出，转换成unsigned即可恢复
_FASTCALL_ int64_t fs2i(const char* str);
_FASTCALL_ int fs2i32(const char* str);
static inline int64_t fs2i(const std::string& str) { return fs2i(str.c_str()); }
static inline int fs2i32(const std::string& str) { return fs2i32(str.c_str()); }
// usage: int64 i64 = fs2i("1234567891011");
// usage: int i = fs2i32("12345");
/**************************** convert str to int ******************************/

}

#endif
