// This divide routine is dedicated to 16bit division
// and is smaller than the HiTech C generic / operator.
// It takes two ints, dividend and divisor and returns
// the byte quotient
// The remainder is in dividend

#include <htc.h>
#include "smalldivide.h"


int iDividend;
int iDivisor;
int iQuotient;

 

void SmallDivide()
{
	iQuotient=0;
	while(iDividend >= iDivisor)
    {
	    iQuotient++;
        iDividend -=iDivisor;
    }
}

