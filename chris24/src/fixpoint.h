#if 0

#ifndef FIXPOINT_H
#define FIXPOINT_H


#include <limits.h>

#define FRACBITS 16

typedef int fixed_t;


fixed_t ToFixed( int a )
{
    return a << FRACBITS;
}


int FromFixed( fixed_t a )
{
    return a >> FRACBITS;
}


fixed_t FixedMul( fixed_t a, fixed_t b )
{
    return ((long long) a * (long long) b) >> FRACBITS;
}


fixed_t FixedDiv2 ( fixed_t a, fixed_t b )
{
    long long c;
    c = ((long long)a<<16) / ((long long)b);
    return (fixed_t) c;
}


fixed_t FixedDiv( fixed_t a, fixed_t b )
{
    if ( (abs(a)>>14) >= abs(b) ) {
        return (a^b) < 0 ? INT_MIN : INT_MAX;
    }
    return FixedDiv2(a,b);
}




#endif // #IFNDEF FIXPOINT_H

#endif