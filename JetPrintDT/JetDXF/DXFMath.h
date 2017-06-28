#ifndef __DXF_MATH_H
#define __DXF_MATH_H

#include "JetDxfDef.h"

/*
Description:
Test a double to make sure it is a valid number.
Returns:
True if x != DXF_UNSET_VALUE and _finite(x) is true.
*/
bool DXFIsValid(double x);

bool DXFIsValidFloat(float x);



#if   defined(ON_LITTLE_ENDIAN)

// works on little endian CPUs with IEEE doubles
#define DXF_IS_FINITE(x) (0x7FF0 != (*((unsigned short*)(&x) + 3) & 0x7FF0))
#define DXF_IS_VALID(x)  (x != DXF_UNSET_VALUE && 0x7FF0 != (*((unsigned short*)(&x) + 3) & 0x7FF0))
#define DXF_IS_VALID_FLOAT(x)  (x != DXF_UNSET_FLOAT)
//TODO - ADD FAST ugly bit check#define DXF_IS_VALID_FLOAT(x)  (x != DXF_UNSET_FLOAT && 0x7FF0 != (*((unsigned short*)(&x) + 3) & 0x7FF0))

#elif defined(ON_BIG_ENDIAN)

// works on big endian CPUs with IEEE doubles
#define DXF_IS_FINITE(x) (0x7FF0 != (*((unsigned short*)(&x)) & 0x7FF0))
#define DXF_IS_VALID(x)  (x != DXF_UNSET_VALUE && 0x7FF0 != (*((unsigned short*)(&x)) & 0x7FF0))
#define DXF_IS_VALID_FLOAT(x)  (x != DXF_UNSET_FLOAT)
//TODO - ADD FAST ugly bit check#define DXF_IS_VALID_FLOAT(x)  (x != DXF_UNSET_FLOAT && 0x7FF0 != (*((unsigned short*)(&x) + 3) & 0x7FF0))

#else



#if defined(_GNU_SOURCE)
#define DXF_IS_FINITE(x) (isfinite(x)?true:false)
// If your compiler does not have "isfinite()", try using "finite()".
//#define DXF_IS_FINITE(x) (finite(x)?true:false)
#else
#define DXF_IS_FINITE(x) (_finite(x)?true:false)
#endif
#define DXF_IS_VALID(x)  (x != DXF_UNSET_VALUE && DXF_IS_FINITE(x))
#define DXF_IS_VALID_FLOAT(x)  (x != DXF_UNSET_FLOAT && DXF_IS_FINITE(x))

#endif


/*****************************************************************************
Find interval in an increasing array of doubles

INPUT:
array A monotone increasing (array[i] <= array[i+1]) array of length doubles.
length (>=1) number of doubles in array
t parameter
OUTPUT:
-1:         t < array[0]
i:         (0 <= i <= length-2) array[i] <= t < array[i+1]
length-1:  t == array[length-1]
length:    t  > array[length-1]
COMMENTS:
If length < 1 or array is not monotone increasing, you will get a meaningless answer and may crash your program.
*****************************************************************************/
int    DXFSearchMonotoneArray( // find a value in an increasing array
const double*, // array[]
int,           // length of array
double         // t = value to search for
);


/*
Description:
Compute a binomial coefficient.
Parameters:
i - [in]
j - [in]
Returns:
(i+j)!/(i!j!), if 0 <= i and 0 <= j, and 0 otherwise.
Remarks:
If (i+j) <= 52, this function is fast and returns the exact value of the binomial coefficient.
For (i+j) > 52, the coefficient is computed recursively using the formula  bc(i,j) = bc(i-1,j) + bc(i,j-1).
For (i+j) much larger than 60, this is inefficient. If you need binomial coefficients for large i and j, then you
should probably be using something like Stirling's Formula. (Look up "Stirling" or "Gamma function" in a calculus book.)
*/
double DXFBinomialCoefficient(
int i,
int j
);

 
/*
Description:
Use the quotient rule to compute derivatives of a one parameter rational function F(t) = X(t)/W(t), where W is a scalar and F and X are vectors of dimension dim.
Parameters:
dim - [in]
der_count - [in] number of derivative (>=0)
v_stride - [in] (>= dim+1)
v - [in/out]
v[] is an array of length (der_count+1)*v_stride. The input v[] array contains  derivatives of the numerator and
denominator	functions in the order (X, W), (Xt, Wt), (Xtt, Wtt), ... In general, the (dim+1) coordinates of the S-th derivative
are in (v[n],...,v[n+dim]) where n = d*v_stride. 
In the output v[] array the derivatives of X are replaced with the derivatives of F and the derivatives of W are divided by w = v[dim].
Returns:
True if input is valid; i.e., v[dim] != 0.
See Also:
*/

bool DXFEvaluateQuotientRule(
int dim,
int der_count,
int v_stride,
double *v
);

// If performance is important, then
// you are better off using ((b<a)?a:b)
double DXF_Max(double a, double b);

// If performance is important, then
// you are better off using ((b<a)?a:b)
float DXF_Max(float a, float b);

// If performance is important, then
// you are better off using ((b<a)?a:b)
int DXF_Max(int a, int b);

// If performance is important, then
// you are better off using ((a<b)?a:b)
double DXF_Min(double a, double b);

// If performance is important, then
// you are better off using ((a<b)?a:b)
float DXF_Min(float a, float b);

// If performance is important, then
// you are better off using ((a<b)?a:b)
int DXF_Min(int a, int b);



#endif
