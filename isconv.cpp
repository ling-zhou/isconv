/// TODO: license
#include "isconv.h"

namespace isconv {

isnode_t g_int_to_str[100000];

uchar  g_mul_10_1[58];
ushort g_mul_10_2[58];
ushort g_mul_10_3[58];
uint   g_mul_10_4[58];
uint   g_mul_10_5[58];
uint   g_mul_10_6[58];
uint   g_mul_10_7[58];
uint   g_mul_10_8[58];
uint64 g_mul_10_9[58]; 
uint64 g_mul_10_10[58];
uint64 g_mul_10_11[58];
uint64 g_mul_10_12[58];
uint64 g_mul_10_13[58];
uint64 g_mul_10_14[58];
uint64 g_mul_10_15[58];
uint64 g_mul_10_16[58];
uint64 g_mul_10_17[58];
uint64 g_mul_10_18[58];
uint64 g_mul_10_19[58];

__attribute__((constructor)) static void __build_map__()
{
    // map int to str
    for (uint i = 0; i < 100000; ++i)
    {
        g_int_to_str[i].len = snprintf(g_int_to_str[i].str, 6, "%u", i);
    }

    // map str to int
    for (uint i = '0'; i <= '9'; ++i)
    {
        g_mul_10_1[i]  = (i ^ '0') * 10;
        g_mul_10_2[i]  = (i ^ '0') * 100;
        g_mul_10_3[i]  = (i ^ '0') * 1000;
        g_mul_10_4[i]  = (i ^ '0') * 10000;
        g_mul_10_5[i]  = (i ^ '0') * 100000;
        g_mul_10_6[i]  = (i ^ '0') * 1000000;
        g_mul_10_7[i]  = (i ^ '0') * 10000000;
        g_mul_10_8[i]  = (i ^ '0') * 100000000;
        g_mul_10_9[i]  = (i ^ '0') * 1000000000ULL;
        g_mul_10_10[i] = (i ^ '0') * 10000000000ULL;
        g_mul_10_11[i] = (i ^ '0') * 100000000000ULL;
        g_mul_10_12[i] = (i ^ '0') * 1000000000000ULL;
        g_mul_10_13[i] = (i ^ '0') * 10000000000000ULL;
        g_mul_10_14[i] = (i ^ '0') * 100000000000000ULL;
        g_mul_10_15[i] = (i ^ '0') * 1000000000000000ULL;
        g_mul_10_16[i] = (i ^ '0') * 10000000000000000ULL;
        g_mul_10_17[i] = (i ^ '0') * 100000000000000000ULL;
        g_mul_10_18[i] = (i ^ '0') * 1000000000000000000ULL;
        g_mul_10_19[i] = (i ^ '0') * 10000000000000000000ULL;
    }
}

}
