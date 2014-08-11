#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/** Double width CAS examples **/

/** Reference : http://stackoverflow.com/questions/4825400/cmpxchg16b-correct **/

typedef struct uint128_t {
  uint64_t lo;
  uint64_t hi;
} uint128_t __attribute__ (( __aligned__( 16 ) ));

inline bool dwcas_v2(volatile __int128* src, __int128 cmp, __int128 with) {

  return __sync_bool_compare_and_swap(src, cmp, with);
}

inline bool dwcas_v1(volatile uint128_t * src, uint128_t cmp, uint128_t with )
{
    bool result;
    __asm__ __volatile__
    (
        "lock cmpxchg16b %1\n\t"
        "setz %0"
        : "=q" ( result )
        , "+m" ( *src )
        , "+d" ( cmp.hi )
        , "+a" ( cmp.lo )
        : "c" ( with.hi )
        , "b" ( with.lo )
        : "cc"
    );
    return result;
}

int main()
{
    uint128_t test = { 0xdecafbad, 0xfeedbeef };
    uint128_t cmp = test;
    uint128_t with = { 0x55555555, 0xaaaaaaaa };

    bool result = dwcas_v1(&test, cmp, with );
    fprintf(stdout, "Handwritten CAS boolean result result is : %d\n", result);

    __int128 a = 1;
    __int128 b = a;
    __int128 c = 2;
    result = dwcas_v2(&a, b, c); 
    fprintf(stdout, "GCC primitive based CAS boolean result is : %d\n", result);

    return 0;
}
