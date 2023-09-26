/* FILE NAME  : mthmatr.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 20.07.2021
 * PURPOSE    : Default matrix functions
 */

#ifndef __mth_matr_h_
#define __mth_matr_h_

#include "mthdef.h"

/* Math support namespace */
namespace mth
{
  /* Matrix handle class */
  template <typename Type>
  class matr
  {    
  private:
    Type M[4][4];
    mutable Type InvM[4][4];
    mutable BOOL IsInverseEvaluated;

    /* Evaluate inverse matrix function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (VOID) None.
    */
    VOID EvaluateInverseMatrix( VOID ) const
    {
      INT p[4][3] = {{1, 2, 3}, {0, 2, 3}, {0, 1, 3}, {0, 1, 2}};
      DBL det = !*this;
      Type m[4][4];

      if (this->IsInverseEvaluated)
        return;

      if (det == 0)
        memcpy(this->InvM, matr::Identity().M, sizeof(Type) * 16);

      for (INT i = 0; i < 4; i++)
        for (INT j = 0; j < 4; j++)
          InvM[j][i] = Determ3x3(M[p[i][0]][p[j][0]], M[p[i][0]][p[j][1]], M[p[i][0]][p[j][2]],
                                 M[p[i][1]][p[j][0]], M[p[i][1]][p[j][1]], M[p[i][1]][p[j][2]],
                                 M[p[i][2]][p[j][0]], M[p[i][2]][p[j][1]], M[p[i][2]][p[j][2]]);

      this->IsInverseEvaluated = TRUE;
    } /* End of 'EvaluateInverseMatrix' function */

  public:
    

    /* Matrix construction function.
     * ARGUMENTS:
     *   - None.
    */
    matr( VOID ) : IsInverseEvaluated(FALSE)
    {
      M[0][0] = 0;
      M[0][1] = 0;
      M[0][2] = 0;
      M[0][3] = 0;

      M[1][0] = 0;
      M[1][1] = 0;
      M[1][2] = 0;
      M[1][3] = 0;

      M[2][0] = 0;
      M[2][1] = 0;
      M[2][2] = 0;
      M[2][3] = 0;

      M[3][0] = 0;
      M[3][1] = 0;
      M[3][2] = 0;
      M[3][3] = 0;

      ZeroMemory(InvM, sizeof(Type) * 16);
    } /* End of 'matr construction' function */

    /* Matrix construiction function.
     * ARGUMENTS:
     *   - 16 values.
    */
    matr( Type a00, Type a01, Type a02, Type a03, Type a10, Type a11, Type a12, Type a13,
          Type a20, Type a21, Type a22, Type a23, Type a30, Type a31, Type a32, Type a33 ) : IsInverseEvaluated(FALSE)
    {
      M[0][0] = a00;
      M[0][1] = a01;
      M[0][2] = a02;
      M[0][3] = a03;

      M[1][0] = a10;
      M[1][1] = a11;
      M[1][2] = a12;
      M[1][3] = a13;

      M[2][0] = a20;
      M[2][1] = a21;
      M[2][2] = a22;
      M[2][3] = a23;

      M[3][0] = a30;
      M[3][1] = a31;
      M[3][2] = a32;
      M[3][3] = a33;

      ZeroMemory(InvM, sizeof(Type) * 16);
    } /* End of 'matr construction' function */

    /* Matrix construiction function.
     * ARGUMENTS:
     *   - 4x4 array.
    */
    matr( Type A[4][4] ) : IsInverseEvaluated(FALSE)
    {
      memset(M, 0, 8 * sizeof(Type));
      memcpy(M, A, 16 * sizeof(Type));
    } /* End of 'matr construction' function */

    /* Matrix identity function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (matr) Unit matrix.
    */
    static matr Identity( VOID )
    {
      return matr(1, 0, 0, 0, 
                  0, 1, 0, 0, 
                  0, 0, 1, 0, 
                  0, 0, 0, 1);
    } /* End of 'Identity' function */

    VOID intoIdentity( VOID )
    {
      this->M[0][0] = 1;
      this->M[0][1] = 0;
      this->M[0][2] = 0;
      this->M[0][3] = 0;

      this->M[1][0] = 0;
      this->M[1][1] = 1;
      this->M[1][2] = 0;
      this->M[1][3] = 0;

      this->M[2][0] = 0;
      this->M[2][1] = 0;
      this->M[2][2] = 1;
      this->M[2][3] = 0;

      this->M[3][0] = 0;
      this->M[3][1] = 0;
      this->M[3][2] = 0;
      this->M[3][3] = 1;

    }

    /* Operator * redefinition function.
     * ARGUMENTS:
     *   - matrix to mul:
     *       const matr &m;
     * RETURNS:
     *   (matr) result matrix.
    */
    matr operator *( const matr &m )
    {
      matr r;

      r.M[0][0] += M[0][0] * m.M[0][0];
      r.M[0][0] += M[0][1] * m.M[1][0];
      r.M[0][0] += M[0][2] * m.M[2][0];
      r.M[0][0] += M[0][3] * m.M[3][0];

      r.M[0][1] += M[0][0] * m.M[0][1];
      r.M[0][1] += M[0][1] * m.M[1][1];
      r.M[0][1] += M[0][2] * m.M[2][1];
      r.M[0][1] += M[0][3] * m.M[3][1];

      r.M[0][2] += M[0][0] * m.M[0][2];
      r.M[0][2] += M[0][1] * m.M[1][2];
      r.M[0][2] += M[0][2] * m.M[2][2];
      r.M[0][2] += M[0][3] * m.M[3][2];

      r.M[0][3] += M[0][0] * m.M[0][3];
      r.M[0][3] += M[0][1] * m.M[1][3];
      r.M[0][3] += M[0][2] * m.M[2][3];
      r.M[0][3] += M[0][3] * m.M[3][3];

      //

      r.M[1][0] += M[1][0] * m.M[0][0];
      r.M[1][0] += M[1][1] * m.M[1][0];
      r.M[1][0] += M[1][2] * m.M[2][0];
      r.M[1][0] += M[1][3] * m.M[3][0];

      r.M[1][1] += M[1][0] * m.M[0][1];
      r.M[1][1] += M[1][1] * m.M[1][1];
      r.M[1][1] += M[1][2] * m.M[2][1];
      r.M[1][1] += M[1][3] * m.M[3][1];

      r.M[1][2] += M[1][0] * m.M[0][2];
      r.M[1][2] += M[1][1] * m.M[1][2];
      r.M[1][2] += M[1][2] * m.M[2][2];
      r.M[1][2] += M[1][3] * m.M[3][2];
      
      r.M[1][3] += M[1][0] * m.M[0][3];
      r.M[1][3] += M[1][1] * m.M[1][3];
      r.M[1][3] += M[1][2] * m.M[2][3];
      r.M[1][3] += M[1][3] * m.M[3][3];

      //

      r.M[2][0] += M[2][0] * m.M[0][0];
      r.M[2][0] += M[2][1] * m.M[1][0];
      r.M[2][0] += M[2][2] * m.M[2][0];
      r.M[2][0] += M[2][3] * m.M[3][0];
      
      r.M[2][1] += M[2][0] * m.M[0][1];
      r.M[2][1] += M[2][1] * m.M[1][1];
      r.M[2][1] += M[2][2] * m.M[2][1];
      r.M[2][1] += M[2][3] * m.M[3][1];
      
      r.M[2][2] += M[2][0] * m.M[0][2];
      r.M[2][2] += M[2][1] * m.M[1][2];
      r.M[2][2] += M[2][2] * m.M[2][2];
      r.M[2][2] += M[2][3] * m.M[3][2];
      
      r.M[2][3] += M[2][0] * m.M[0][3];
      r.M[2][3] += M[2][1] * m.M[1][3];
      r.M[2][3] += M[2][2] * m.M[2][3];
      r.M[2][3] += M[2][3] * m.M[3][3];

      //

      r.M[3][0] += M[3][0] * m.M[0][0];
      r.M[3][0] += M[3][1] * m.M[1][0];
      r.M[3][0] += M[3][2] * m.M[2][0];
      r.M[3][0] += M[3][3] * m.M[3][0];
      
      r.M[3][1] += M[3][0] * m.M[0][1];
      r.M[3][1] += M[3][1] * m.M[1][1];
      r.M[3][1] += M[3][2] * m.M[2][1];
      r.M[3][1] += M[3][3] * m.M[3][1];
      
      r.M[3][2] += M[3][0] * m.M[0][2];
      r.M[3][2] += M[3][1] * m.M[1][2];
      r.M[3][2] += M[3][2] * m.M[2][2];
      r.M[3][2] += M[3][3] * m.M[3][2];
      
      r.M[3][3] += M[3][0] * m.M[0][3];
      r.M[3][3] += M[3][1] * m.M[1][3];
      r.M[3][3] += M[3][2] * m.M[2][3];
      r.M[3][3] += M[3][3] * m.M[3][3];

      return r;
    } /* End of 'operator *' function */

    /* Operator ! redefinition function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (DBL) matrix determinant.
    */
    DBL operator !( VOID )
    {
      return Determ();
    } /* End of 'operator !' function */

    /* Determinant finder for matrix 3x3 function.
     * ARGUMENTS:
     *   - 9 numbers (3x3 as a matrix).
     * RETURNS:
     *   (DBL) matrix determinant.
    */
    DBL Determ3x3( Type A11, Type A12, Type A13,
                    Type A21, Type A22, Type A23,
                    Type A31, Type A32, Type A33 )
    {
      return A11 * A22 * A33 - A11* A23 * A32 - A12 * A21 * A33 + A12 * A23 * A31 + A13 * A21 * A32 - A13 * A22 * A31;
    } /* End of 'Determ3x3' function */

    /* Determinant finder for matrix 4x4 function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (DBL) matrix determinant.
    */
    DBL Determ( VOID )
    {
      return M[0][0] * Determ3x3(M[1][1], M[1][2], M[1][3],
                                 M[2][1], M[2][2], M[2][3],
                                 M[3][1], M[3][2], M[2][3]) -
             M[0][1] * Determ3x3(M[1][0], M[1][2], M[1][3],
                                 M[2][0], M[2][2], M[2][3],
                                 M[3][0], M[3][2], M[2][3]) +
             M[0][2] * Determ3x3(M[1][0], M[1][1], M[1][3],
                                 M[2][0], M[2][1], M[2][3],
                                 M[3][0], M[3][1], M[2][3]) - 
             M[0][3] * Determ3x3(M[1][0], M[1][1], M[1][2],
                                 M[2][0], M[2][1], M[2][2],
                                 M[3][0], M[3][1], M[2][2]);
    } /* End of 'Determ' function */


    /* Inverse finder for matrix 4x4 function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (matr) onversed matrix.
    */
    matr Inverse( VOID )
    {
      DBL det = Determ();

      if (det == 0)
        return Identity();

      InvM[0][0] = Determ3x3(M[1][1], M[1][2], M[1][3],
                            M[2][1], M[2][2], M[2][3],
                            M[3][1], M[3][2], M[3][3]) / det;

      InvM[0][1] = -Determ3x3(M[1][0], M[1][2], M[1][3],
                             M[2][0], M[2][2], M[2][3],
                             M[3][0], M[3][2], M[3][3]) / det;

      InvM[0][2] = Determ3x3(M[1][0], M[1][1], M[1][3],
                            M[2][0], M[2][1], M[2][3],
                            M[3][0], M[3][1], M[3][3]) / det;

      InvM[0][3] = -Determ3x3(M[1][0], M[1][1], M[1][2],
                             M[2][0], M[2][1], M[2][2],
                             M[3][0], M[3][1], M[3][2]) / det;

      InvM[1][0] = -Determ3x3(M[0][1], M[0][2], M[0][3],
                              M[2][1], M[2][2], M[2][3],
                              M[3][1], M[3][2], M[3][3]) / det;

      InvM[1][1] = Determ3x3(M[0][0], M[0][2], M[0][3],
                             M[2][0], M[2][2], M[2][3],
                             M[3][0], M[3][2], M[3][3]) / det;

      InvM[1][2] = -Determ3x3(M[0][0], M[0][1], M[0][3],
                              M[2][0], M[2][1], M[2][3],
                              M[3][0], M[3][1], M[3][3]) / det;

      InvM[1][3] = Determ3x3(M[0][0], M[0][1], M[0][2],
                            M[2][0], M[2][1], M[2][2],
                            M[3][0], M[3][1], M[3][2]) / det;

      InvM[2][0] = Determ3x3(M[0][1], M[0][2], M[0][3],
                             M[1][1], M[1][2], M[1][3],
                             M[3][1], M[3][2], M[3][3]) / det;

      InvM[2][1] = -Determ3x3(M[0][0], M[0][2], M[0][3],
                              M[1][0], M[1][2], M[1][3],
                              M[3][0], M[3][2], M[3][3]) / det;

      InvM[2][2] = Determ3x3(M[0][0], M[0][1], M[0][3],
                             M[1][0], M[1][1], M[1][3],
                             M[3][0], M[3][1], M[3][3]) / det;

      InvM[2][3] = -Determ3x3(M[0][0], M[0][1], M[0][2],
                             M[1][0], M[1][1], M[1][2],
                             M[3][0], M[3][1], M[3][2]) / det;

      InvM[3][0] = -Determ3x3(M[0][1], M[0][2], M[0][3],
                              M[1][1], M[1][2], M[1][3],
                              M[2][1], M[2][2], M[2][3]) / det;

      InvM[3][1] = Determ3x3(M[0][0], M[0][2], M[0][3],
                             M[1][0], M[1][2], M[1][3],
                             M[2][0], M[2][2], M[2][3]) / det;

      InvM[3][2] = -Determ3x3(M[0][0], M[0][1], M[0][3],
                              M[1][0], M[1][1], M[1][3],
                              M[2][0], M[2][1], M[2][3]) / det;

      InvM[3][3] = Determ3x3(M[0][0], M[0][1], M[0][2],
                            M[1][0], M[1][1], M[1][2],
                            M[2][0], M[2][1], M[2][2])  / det;

      return InvM;
    } /* End of 'Inverse' function */

    static matr RotateZ( DBL AngleeInDegree )
    {
      DBL a = AngleeInDegree * (PI / 180), c = cos(a), s = sin(a);

      return matr(c, s, 0, 0, 
                 -s, c, 0, 0, 
                 0, 0, 1, 0, 
                 0, 0, 0, 1);
    } /* End of 'RotateZ' function */

    vec3<Type> TransformNormal( const vec3<Type> &N ) const
    {
      EvaluateInverseMatrix();
      return vec3<Type>(N.X * InvM[0][0] + N.Y * InvM[0][1] + N.Z * InvM[0][2],
                     N.X * InvM[1][0] + N.Y * InvM[1][1] + N.Z * InvM[1][2],
                     N.X * InvM[2][0] + N.Y * InvM[2][1] + N.Z * InvM[2][2]);
    } /* End of 'TransformNormal' function */

    static matr RotateY( DBL AngleeInDegree )
    {
      DBL a = AngleeInDegree * (PI / 180), c = cos(a), s = sin(a);

      return matr(c, 0, -s, 0, 
                 0, 1, 0, 0, 
                 s, 0, c, 0, 
                 0, 0, 0, 1);
    } /* End of 'RotateY' function */

    static matr RotateX( DBL AngleeInDegree )
    {
      DBL a = AngleeInDegree * (PI / 180), c = cos(a), s = sin(a);

      return matr(1, 0, 0, 0, 
                 0, c, s, 0, 
                 0, -s, c, 0, 
                 0, 0, 0, 1);
    } /* End of 'RotateX' function */

    static matr Scale( const vec3<Type> &S )
    {
      return matr(S.X, 0, 0, 0,
                  0, S.Y, 0, 0,
                  0, 0, S.Z, 0,
                  0, 0, 0, 1);
    } /* End of 'Scale' function */

    static matr Translate( const vec3<Type> &T )
    {
      return matr(1, 0, 0, 0,
                  0, 1, 0, 0,
                  0, 0, 1, 0,
                  T.X, T.Y, T.Z, 1);
    } /* End of 'Translate' function */

    matr Transpose( VOID )
    {
      return matr(M[0][0], M[1][0], M[2][0], M[3][0],
                 M[0][1], M[1][1], M[2][1], M[3][1],
                 M[0][2], M[1][2], M[2][2], M[3][2],
                 M[0][3], M[1][3], M[2][3], M[3][3]);
    } /* End of 'Transpose' function */

    static matr Rotate( const DBL AngleInDegrees, vec3<Type> &V )
    {
      DBL a = D2R(AngleInDegrees), s = sin(a), c = cos(a);
      vec3<Type> R = V.Normalizing();

      return matr(c + R.X * R.X * (1 - c), R.X * R.Y * (1 - c) + R.Z * s, R.X * R.Z * (1 - c) - R.Y * s, 0,
                  R.Y * R.X * (1 - c) - R.Z * s, c + R.Y * R.Y * (1 - c), R.Y * R.Z * (1 - c) + V.Y * s, 0,
                  R.Z * R.X * (1 - c) + R.Y * s, R.Z * R.Y * (1 - c) - R.X * s, c + R.Z * R.Z * (1 - c), 0,
                  0, 0, 0, 1);
    } /* End of '' function */

    vec3<Type> TransformPoint( const vec3<Type> &V )
    {
      return vec3<Type>(V.X * M[0][0] + V.Y * M[1][0] + V.Z * M[2][0] + M[3][0],
                  V.X * M[0][1] + V.Y * M[1][1] + V.Z * M[2][1] + M[3][1],
                  V.X * M[0][2] + V.Y * M[1][2] + V.Z * M[2][2] + M[3][2]);
    } /* End of '' function */

    vec3<Type> TransformVector( const vec3<Type> &V )
    {
      return vec3<Type>(V.X * M[0][0] + V.Y * M[1][0] + V.Z * M[2][0],
                V.X * M[0][1] + V.Y * M[1][1] + V.Z * M[2][1],
                V.X * M[0][2] + V.Y * M[1][2] + V.Z * M[2][2]);
    } /* End of '' function */
    
    vec3<Type> Transform4x4( const vec3<Type> &V )
    {
      FLT w = V.X * M[0][3] + V.Y * M[1][3] + V.Z * M[2][3] + M[3][3];

      return vec3<Type>((V.X * M[0][0] + V.Y * M[1][0] + V.Z * M[2][0] + M[3][0]) / w,
                (V.X * M[0][1] + V.Y * M[1][1] + V.Z * M[2][1] + M[3][1]) / w,
                (V.X * M[0][2] + V.Y * M[1][2] + V.Z * M[2][2] + M[3][2]) / w);
    } /* End of '' function */

    /* Operator << redefinition function.
     * ARGUMENTS:
     *   - String to output:
     *       std::ostream &C;
     *   - Matrix to output:
     *       matr &V;
     * RETURNS:
     *   (std::ostream &) string.
    */
    friend std::ostream & operator << ( std::ostream &C, matr &M )
    {
      C << "[ ";
      for (int i = 0; i < 4; i++)
        C << M.M[0][i] << " ";
      C << "]" << std::endl;

      C << "[ ";
      for (int i = 0; i < 4; i++)
        C << M.M[1][i] << " ";
      C << "]" << std::endl;

      C << "[ ";
      for (int i = 0; i < 4; i++)
        C << M.M[2][i] << " ";
      C << "]" << std::endl;

      C << "[ ";
      for (int i = 0; i < 4; i++)
        C << M.M[3][i] << " ";
      C << "]" << std::endl;

      return C;
    } /* End of '' function */

    static matr Frustum( FLT L, FLT R, FLT B, FLT T, FLT N, FLT F )
    {
      return matr(2 * N / (R - L), 0, 0, 0,
                  0, 2 * N / (T - B), 0, 0,
                  (R + L) / (R - L), (T + B) / (T - B), -(F + N) / (F - N), -1,
                  0, 0, -2 * N * F / (F - N), 0);
    }

    static matr View( vec3<Type> Loc, vec3<Type> At, vec3<FLT> Up1 )
    {
      vec3<Type>
        D = (At - Loc).Normalizing(), /* Dir - direction */
        R = (D % Up1).Normalizing(), /* Right */
        U = (R % D); /* Up */
        

      return matr(R.X, U.X, -D.X, 0,
                     R.Y, U.Y, -D.Y, 0,
                     R.Z, U.Z, -D.Z, 0,
                     -(Loc & R), -(Loc & U), Loc & D, 1);
    }

    operator Type * ( VOID )
    {
      return this->M[0];
    }

    static vec3<Type> TransformPoint( const vec3<Type> &V, const matr<Type> &M )
    {
      return vec3<Type>(V.X * M.M[0][0] + V.Y * M.M[1][0] + V.Z * M.M[2][0] + M.M[3][0],
                  V.X * M.M[0][1] + V.Y * M.M[1][1] + V.Z * M.M[2][1] + M.M[3][1],
                  V.X * M.M[0][2] + V.Y * M.M[1][2] + V.Z * M.M[2][2] + M.M[3][2]);
    } /* End of '' function */

    operator const Type * ( VOID ) const
    {
      return this->M[0];
    }

  }; /* End of 'matr' class */
} /* End of 'mth' namespace */

#endif /* __mth_matr_h_ */

/* END OF 'mthmatr.h' FILE */