/* FILE NAME  : mthdef.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 20.07.2021
 * PURPOSE    : Default math preferences
 */

#ifndef __mthdef_h_
#define __mthdef_h_

#include <cmath>

#include "../def.h"

/* Pi math constant */
#define PI 3.14159265358979323846
 
/* Degrees to radians conversion */
#define D2R(A) ((A) * (PI / 180.0))
#define Degree2Radian(a) D2R(a)

typedef double DBL;
typedef float FLT;
typedef unsigned __int64 UINT64;
typedef unsigned char BYTE;


#endif /* __mthdef_h_ */

/* END OF 'mthdef.h' FILE */