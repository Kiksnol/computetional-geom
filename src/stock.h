/* FILE NAME  : stock.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 24.07.2021
 * PURPOSE    : Default stock preferences
 */

#ifndef __stock_h_
#define __stock_h_

#include "def.h"

/* Stock class */
template<typename Type>
class stock
{
private:
  INT RealSize, MaxSize;
  Type *Array;

public:
  stock( VOID ) : Array(nullptr), RealSize(0), MaxSize(0)
  {

  }

  ~stock( VOID )
  {
    if (Array != nullptr)
      delete[] Array;

    RealSize = 0;
    MaxSize = 0;
  }

  operator Type * ( VOID )
  {
    return this->Array;
  }

  stock( const stock<Type> &CopyData ) :
      Array(new Type[CopyData.RealSize]),
      RealSize(CopyData.RealSize),
      MaxSize(CopyData.RealSize)
    {
      for (INT i = 0; i < RealSize; i++)
        Array[i] = CopyData.Array[i];
    }

    stock & operator=( const stock<Type> &CopyData )
    {
      if (Array != nullptr)
        delete[] Array;
      Array = new Type[CopyData.RealSize];
      RealSize = CopyData.RealSize;
      MaxSize = CopyData.RealSize;
      for (INT i = 0; i < RealSize; i++)
        Array[i] = CopyData.Array[i];
      return *this;
    }

    /* Get size function.
     * ARGUMENTS: None.
     * RETURNS:
     *   (INT) Size;
     */
    INT Size( VOID ) const
    {
      return RealSize;
    }


    stock & Add( const Type &NewData )
    {
      if (RealSize >= MaxSize)
      {
        // increase allocated memory
        if (MaxSize == 0)
          MaxSize = 1;
        else
          MaxSize *= 2;

        Type *new_bulk = new Type[MaxSize];
        if (Array != nullptr)
        {
          // copy old data
          for (INT i = 0; i < RealSize; i++)
            new_bulk[i] = Array[i];
          // remove old data
          delete[] Array;
        }
        Array = new_bulk;
      }
      Array[RealSize++] = NewData;
      return *this;
    }
    stock & operator<<( const Type &NewData )
    {
      return Add(NewData);
    }

    Type & operator[]( INT Index )
    {
      assert(Index >= 0 && Index < RealSize);
      return Array[Index];
    }

    Type & operator[]( INT Index ) const
    {
      assert(Index >= 0 && Index < RealSize);
      return Array[Index];
    }

    /* Array clearing function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Clear( VOID )
    {
      RealSize = 0;
    } /* End of 'Clear' function */

    /* Array walker function.
     * ARGUMENTS:
     *   - Walker function:
     *       Type2 &Walker;
     * RETURNS: None.
     */
    template<typename Type2>
      VOID Walk( const Type2 &Walker )
      {
        for (INT i = 0; i < RealSize; i++)
          Walker(Array[i]);
      }

      /* Array walker function.
     * ARGUMENTS:
     *   - Walker function:
     *       Type2 &Walker;
     * RETURNS: None.
     */
    template<typename Type2>
      VOID Walk2( const Type2 &Walker )
      {
        for (INT i = 0; i < RealSize; i++)
          if ((i < 82 && i > 80) || i < 50 || (i > 95 && i < 100) || (i < 109 && i > 105))
            Walker(Array[i]);
      }
}; /* End of 'stock' class */

#endif /* __stock_h_ */ 

/* END OF 'stock.h' FILE */