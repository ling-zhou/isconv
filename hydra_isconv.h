/// int <=> str 快速转换实用接口.
/// 优点：1) 快: fi2s比snprintf快一个数量级.
///       2) 好用: fi2s不强制先声明一个buffer; fs2i提供char*, string重载.
///       3) 安全: 不会出现错误书写fmt(如"%d")导致的错误.
/// @file
/// @date 2013-09-24 12:55:13 
/// @version 1.0.0
/// @author ling-zhou(周龄), master@manpage.cn
/// @namespace hydra

#ifndef __HYDRA_ISCONV_H__
#define __HYDRA_ISCONV_H__ \
        "$Id: hydra_isconv.h 2167 2014-08-14 10:21:56Z baozhou $"

#include <stdint.h>
#include <string>

#if __WORDSIZE == 32   
#define _FASTCALL_          __attribute__((__fastcall__))
#else
#define _FASTCALL_
#endif

namespace hydra {
  
template <int SIZE, typename len_t>
struct StringUnit
{
    char  s[SIZE]; // content
    len_t l;       // content length
};
  
typedef StringUnit<25, unsigned char> isbuf_t;

/**************************** convert int to str ******************************/
/************** unsigned int fi2s(int_t val, char* buf) 型的接口 **************/
// 注意事项：1. buf的长度需要比实际val长度大5个字节(包括\0)
//           2. 返回值为val对应字符串(不包括\0)的长度
_FASTCALL_ unsigned int fi2s(int val, char* buf);
_FASTCALL_ unsigned int fi2s(unsigned int val, char* buf);
_FASTCALL_ unsigned int fi2s(int64_t val, char* buf);
_FASTCALL_ unsigned int fi2s(uint64_t val, char* buf);

#if __WORDSIZE == 32
static inline unsigned int fi2s(long val, char* buf)
{
    return fi2s((int)val, buf);
}

static inline unsigned int fi2s(unsigned long val, char* buf)
{
    return fi2s((unsigned int)val, buf);
}
#else
static inline unsigned int fi2s(long long val, char* buf)
{
    return fi2s((int64_t)val, buf);
}

static inline unsigned int fi2s(unsigned long long val, char* buf)
{
    return fi2s((uint64_t)val, buf);
}
#endif

// usage: 
//       char buf[32];
//       unsigned int len = fi2s(1234567, buf);
/************** unsigned int fi2s(int_t val, char* buf) 型的接口 **************/

/********************** isbuf_t fi2s(int_t val) 型的接口 **********************/
template<typename int_t> static inline isbuf_t fi2s(int_t val)
{
    isbuf_t buf;
    buf.l = fi2s(val, buf.s);
    return buf;
}

#if __WORDSIZE == 32
static inline isbuf_t fi2s(long val)
{
    return fi2s((int)val);
}

static inline isbuf_t fi2s(unsigned long val)
{
    return fi2s((unsigned int)val);
}
#else
static inline isbuf_t fi2s(long long val)
{
    return fi2s((int64_t)val);
}

static inline isbuf_t fi2s(unsigned long long val)
{
    return fi2s((uint64_t)val);
}
#endif

// usage: 
//       printf("%s %s\n", fi2s(12345).s, fi2s(-54321).s, fi2s(111111111111LL).s);
/********************** isbuf_t fi2s(int_t val) 型的接口 **********************/
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

static inline int64_t fs2i(const std::string& str)
{
    return fs2i(str.c_str());
}

static inline int fs2i32(const std::string& str)
{
    return fs2i32(str.c_str());
}

// usage: int64 i64 = fs2i("-1234567891011");
// usage: int i = fs2i32("12345");
/**************************** convert str to int ******************************/

}

#endif
