/* FILE NAME  : mth_ray.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 29.07.2021
 * PURPOSE    : Intersections.
 */

#ifndef __mth_ray_h
#define __mth_ray_h

#include "mthdef.h"

/* Math support namespace */
namespace mth
{
  /* Vec3 handle class */
  template <typename Type>
  class ray
  {
  public:
    vec3<Type> Org, Dir;

    /* Dot multiply two vectors function.
     * ARGUMENTS:
     *   - vectors to be dot multiplied:
     *       VEC V1, V2;
     * RETURNS:
     *   (FLT) result scalar.
     */
    Type VecDotVec( vec3<Type> V1, vec3<Type> V2 )
    {
      return V1[0] * V2[0] + V1[1] * V2[1] + V1[2] * V2[2];
    } /* End of 'VecDotVec' function */

    BOOL SphereInter( vec3<Type> C, Type R )
    {
      vec3<Type> a = C - Org;
      FLT OC2, OK, OK2, R2, h2;

      OC2 = VecDotVec(a, a);
      OK = VecDotVec(a, Dir);
      OK2 = VecDotVec(a, Dir) * VecDotVec(a, Dir);
      R2 = R * R;
      h2 = R2 - (OC2 - OK2);
      if (OC2 < R2)
        return TRUE;
      if (OK < 0 || h2 < 0)
        return FALSE;
      return TRUE;
    }

  }; /* End of 'ray' class */
} /* End of 'mth' namespace */


#endif /* __mth_ray_h */

/* END OF 'mth_ray.h' FILE */