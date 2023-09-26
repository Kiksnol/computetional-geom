/* FILE NAME  : mth_vec4.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 20.07.2021
 * PURPOSE    : Default math preferences
 */

#ifndef __mth_vec4_h_
#define __mth_vec4_h_

#include "mthdef.h"

/* Math support namespace */
namespace mth
{
  /* vec4 handle class */
  template <typename Type>
  class vec4
  {
  private:
    Type X, Y, Z, W;
  public:
    /* Vector construction function.
     * ARGUMENTS:
     *   - None.
    */
    vec4( VOID ) : X(0), Y(0), Z(0), W(0)
    {
    } /* End of 'vec4 constructor' function */

    /* Vector construction function.
     * ARGUMENTS:
     *   - 3 coords:
     *       Type A, B, C;
    */
    vec4( Type A, Type B, Type C, Type D ) : X(A), Y(B), Z(C), W(D)
    {
    } /* End of 'vec4 constructor' function */

    /* Vector construction function.
     * ARGUMENTS:
     *   - 1 coord for all:
     *       Type A;
    */
    vec4( Type A ) : X(A), Y(A), Z(A), W(A)
    {
    } /* End of 'vec4 constructor' function */

    /* Operator + redefinition function.
     * ARGUMENTS:
     *   - Vector to add:
     *       const vec4 &val;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator +( const vec4 &val )
    {
      return vec4(X + val.X, Y + val.Y, Z + val.Z, W + val.W);
    } /* End of 'operator +' function */

    /* Operator - redefinition function.
     * ARGUMENTS:
     *   - Vector to sub:
     *       const vec4 &val;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator -( const vec4 &val )
    {
      return vec4(X - val.X, Y - val.Y, Z - val.Z, W - val.W);
    } /* End of 'operator -' function */

    /* Operator & redefinition function.
     * ARGUMENTS:
     *   - Vector to dot:
     *       const vec4 &val;
     * RETURNS:
     *   (vec4) result vector;
    */
    Type operator &( const vec4 &val )
    {
      return X * val.X + Y * val.Y + Z * val.Z + W * val.W;
    } /* End of 'operator &' function */

    /* Operator % redefinition function.
     * ARGUMENTS:
     *   - Vector to cross:
     *       const vec4 &val;
     * RETURNS:
     *   (vec4) result vector;
    */
    //vec4  operator %( const vec4 &val )
    //{
    //  return vec4(Y * val.Z - Z * val.Y, Z * val.X - X * val.Z, X * val.Y - Y * val.X);
    //} /* End of 'operator %' function */

    /* Operator - redefinition function. Negative.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator -( VOID )
    {
      return vec4(-X, -Y, -Z, -W);
    } /* End of 'operator -' function */

    /* Operator ! redefinition function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (DBL) vector length;
    */
    DBL operator !( VOID )
    {
      return sqrt(X * X + Y * Y + Z * Z + W * W);
    } /* End of 'operator !' function */

    /* Operator * redefinition function.
     * ARGUMENTS:
     *   - Number to multiply:
     *       const Type N;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator *( const Type N )
    {
      return vec4(X * N, Y * N, Z * N, W * N);
    } /* End of 'operator *' function */

    /* Operator / redefinition function.
     * ARGUMENTS:
     *   - Number to divide:
     *       const Type N;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator /( const Type N )
    {
      return vec4(X / N, Y / N, Z / N, W / N);
    } /* End of 'operator /' function */

    /* Operator += redefinition function.
     * ARGUMENTS:
     *   - Vector to add:
     *       const vec4 &val;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator +=( const vec4 &val )
    {
      X += val.X;
      Y += val.Y;
      Z += val.Z;
      W += val.W;

      return *this;
    } /* End of 'operator +=' function */

    /* Operator -= redefinition function.
     * ARGUMENTS:
     *   - Vector to sub:
     *       const vec4 &val;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator -=( const vec4 &val )
    {
      X -= val.X;
      Y -= val.Y;
      Z -= val.Z;
      W -= val.W;

      return *this;
    } /* End of 'operator -=' function */

    /* Operator *= redefinition function.
     * ARGUMENTS:
     *   - Number to multiply:
     *       const Type N;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator *=( const Type N )
    {
      X *= N;
      Y *= N;
      Z *= N;
      W *= N;

      return *this;
    } /* End of 'operator *=' function */

    /* Operator /= redefinition function.
     * ARGUMENTS:
     *   - Number to divide:
     *       const Type N;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator /=( const Type N )
    {
      X /= N;
      Y /= N;
      Z /= N;
      W /= N;

      return *this;
    } /* End of 'operator /=' function */

    /* Squared length function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (Type) vector length;
    */
    Type Length2( VOID )
    {
      return X * X + Y * Y + Z * Z + W * W;
    } /* End of 'Length2' function */

    /* Distance finder function.
     * ARGUMENTS:
     *   - Vector to find dist:
     *       const vec4 &val;
     * RETURNS:
     *   (DBL) distance;
    */
    DBL Distance( const vec4 &val )
    {
      return sqrt((X - val.X) * (X - val.X) + (Y - val.Y) * (Y - val.Y) + (Z - val.Z) * (Z - val.Z) + (W - val.W) * (W - val.W));
    } /* End of 'Distance' function */

    /* Operator * redefinition function.
     * ARGUMENTS:
     *   - Vector to mul:
     *       const vec4 &val;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator *( const vec4 &val )
    {
      return vec4(X * val.X, Y * val.Y, Z * val.Z, W * val.W);
    } /* End of 'operator *' function */

    /* Operator *= redefinition function.
     * ARGUMENTS:
     *   - Vector to mul:
     *       const vec4 &val;
     * RETURNS:
     *   (vec4) result vector;
    */
    vec4 operator *=( const vec4 &val )
    {
      X *= val.X;
      Y *= val.Y;
      Z *= val.Z;
      W *= val.W;

      return *this;
    } /* End of 'operator *=' function */

    /* Normalize function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   None.
    */
    VOID Normalize( VOID )
    {
      *this /= !*this;
    } /* End of 'Normalize' function */

    /* Normalize function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (vec4 &) normalized vector.
    */
    vec4 & Normalizing( VOID )
    {
      return *this /= !*this;
    } /* End of 'Normalizing' function */

    /* Zero vector function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   (vec4) zero vector.
    */
    vec4 Zero( VOID )
    {
      return vec4(0);
    } /* End of 'Zero' function */

    /* Operator [] redefinition function.
     * ARGUMENTS:
     *   - Element number:
     *       INT i;
     * RETURNS:
     *   (Type &) element.
    */
    Type & operator []( INT i )
    {
      switch(i)
      {
      case 0:
        return this->X;
      case 1:
        return this->Y;
      case 2:
        return this->Z;
      case 3:
        return this->W;
      default:
        if (i < 0)
          return this->X;
        else
          return this->W;
      }
    } /* End of 'operator []' function */

    /* Vector output function.
     * ARGUMENTS:
     *   - None.
     * RETURNS:
     *   None.
    */
    VOID Out( VOID )
    {
      std::cout << "<" << X << " " << Y << " " << Z << " " << W << ">";
    } /* End of 'Out' function */

    /* Operator << redefinition function.
     * ARGUMENTS:
     *   - String to output:
     *       std::ostream &C;
     *   - Vector to output:
     *       vec4 &V;
     * RETURNS:
     *   (std::ostream &) string.
    */
    friend std::ostream & operator << ( std::ostream &C, vec4 &V )
    {
      C << "<";
      C << V[0] << " ";
      C << V[1] << " ";
      C << V[2] << " ";
      C << V[3] << ">";

      return C;
    } /* End of 'operator <<' function */

    template <typename Type1>
    friend class matr;
  }; /* End of 'vec4' class */
} /* End of 'mth' namespace */


#endif /* __mth_vec4_h_ */

/* END OF 'mth_vec4.h' FILE */
