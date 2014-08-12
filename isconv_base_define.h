/// TODO: license
/// isconv 基础定义
/// @file
/// @date 2013-09-24 12:55:13 
/// @version 1.0.0
/// @author ling-zhou(周龄), nullzhou@foxmail.com
/// @namespace hydra

#ifndef __ISCONV_BASE_DEFINE_H__
#define __ISCONV_BASE_DEFINE_H__

#include <stdint.h>
#include <sys/types.h>

#undef uchar
#undef ushort
#undef uint
#undef ulong
#undef int64
#undef uint64

///@brief 整数类型定义，只是为了使类型写起来更简洁
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
typedef intmax_t       int64;   ///< printf fmt: %jd(man 3 printf)
typedef uintmax_t      uint64;  ///< printf fmt: %ju(man 3 printf)

namespace hydra {

/// @brief 字符串/二进制块 单元，最大容量为SIZE，实际长度为len
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
        TLen l;         // alias name for len
    };
};

/// @brief 固定大小的 字符串/二进制块 单元, 长度为SIZE
template <int SIZE>
struct FixedStrUnit
{
    union
    {
        char str[SIZE];
        char s[SIZE];   // alias name for str
    };
};

}

#endif
