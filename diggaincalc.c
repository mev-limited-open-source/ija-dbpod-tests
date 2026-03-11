#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdint.h>

static void do_estimate(short millibels)
{
    static const uint32_t factors[11] =
    {
        1074978726, 1076217054, 1078697991, 1083677034,
        1093704174, 1114037653, 1155845721, 1244227712,
        1441782898, 1935975557, 3490598276
    };
    uint32_t diggain;
    unsigned int numer;
    unsigned int denom;
    unsigned int i;
    unsigned int bm;
    uint64_t scale;
    uint32_t remain;
    short work_mb;

    work_mb = millibels;
    denom = 1;
    numer = 1;
    while (work_mb < 0)
    {
        denom *= 10;
        work_mb += 2000;
    }
    while (work_mb >= 2000)
    {
        numer *= 10;
        work_mb -= 2000;
    }
    scale = 1UL << 30;
    remain = 0;
    for (i = 10, bm = 1 << 10; bm; i--, bm >>= 1)
    {
        if (work_mb & bm)
        {
            scale *= factors[i];
            scale += ((uint64_t)remain * factors[i]) >> 30;
            remain = scale & ((1UL << 30) - 1);
            scale >>= 30;
        }

    }
    diggain = ((scale * numer + (((uint64_t)remain * numer) >> 30)) /
            denom) >> (30 - 16);
    printf("millibels = %d, diggain = 0x%lX\n", millibels, diggain);
}

int main(int argc, char **argv)
{
    char *endptr;
    double dB;
    double linear;
    unsigned long diggainval;
    unsigned long superdiggainval;
    unsigned long superdiggainval2;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s: DECIBELS\n", argv[0]);
        fprintf(stderr, "Range for DECIBELS is -96.32 to +96.32\n");
        return 2;
    }


    dB = strtod(argv[1], &endptr);
    if (endptr == argv[1] || *endptr)
    {
        fprintf(stderr, "Argument is not a real number (decibels)\n");
        return 1;
    }

    if (fabs(dB) > 96.32)
    {
        fprintf(stderr, "Argument out of range +/-96.32 dB\n");
        return 1;
    }
    
    linear = pow(10, dB / 20);
    diggainval = linear * 65536;
    superdiggainval = round(linear * (1UL << 30));
    superdiggainval2 = linear * (1UL << 30);
    printf("%g dB gives amplitude ratio %g, diggainval=0x%lX, super=%lu, super2=%lu\n",
            dB, linear, diggainval, superdiggainval, superdiggainval2);
    do_estimate(round(dB * 100));

    return 0;
}

/* vi: set ai et sw=4 ts=4: */
