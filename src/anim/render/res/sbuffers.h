/*************************************************************
 * Copyright (C) 2022
 *    Computer Graphics Support Group of 30 Phys-Math Lyceum
 *************************************************************/

 /* FILE NAME   : sbuffers.h
  * PURPOSE     : Ray marching project.
  *               Animation module.
  * PROGRAMMER  : CGSG-SummerCamp'2022.
  *               Vlad A. Biserov.
  * LAST UPDATE : 17.07.2022
  * NOTE        : None.
  *
  * No part of this file may be changed without agreement of
  * Computer Graphics Support Group of 30 Phys-Math Lyceum
  */


#ifndef __sbuffers_h_
#define __sbuffers_h_

#include "res.h"

// namespace for buffers manager
namespace nicg
{
  // class buffers
  class sbuffer
  {
    friend class sbuffer_manager;
    template<typename entry_type, typename index_type>
    friend class resource_manager;
    friend class material;
  private:
    UINT Id;        // open gl id
    UINT BindPoint; // binding point for shader
    UINT BufSize;

  public:
    /* default constructor */
    sbuffer( VOID ) : Id(0), BindPoint(0), BufSize(0)
    {
    } // end of default consructor

    /* Create buffer function
     * ARGUMENTS: 
     *   - binding point:
     *       UINT BindingPoint;
     *   - data structure:
     *       const data_structure *Ptr;
     * RETURNS:
     *   (buffer &) pointer to buffer
     */
    template<typename data_structure>
      sbuffer & Create( UINT BindingPoint, const data_structure *Ptr, const INT Size )
      {
        Free();
        size_t x = 2;
        if (Size != 0)
          x = std::log2(Size) + 1;
        BufSize = std::pow(2, x);
        BindPoint = BindingPoint;

        glGenBuffers(1, &Id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, Id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, BufSize, nullptr, GL_DYNAMIC_DRAW);
        Apply();
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); 
        Update(Ptr, Size, 0);
        return *this;
      } // end of 'nicg::buffer::Create' function

    /* Apply buffers 
     * ARGUMENTS: None.
     * RETURNS: None.
     */ 
    VOID Apply( VOID );
   
    /* Update buffers 
     * ARGUMENTS:
     *   - data structure:
     *       const data_structure &Ptr;
     * RETURNS: None.
     */ 
    template<typename data_structure>
      VOID Update( const data_structure *Ptr, INT Size, INT Shift = 0 )
      {
        if (Size > BufSize)
          Create(BindPoint, Ptr, Size);
        else
        {
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, Id);
          glBufferSubData(GL_SHADER_STORAGE_BUFFER, Shift, Size, Ptr);
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
      } // end of 'nicg::buffer::Update' function
   
    /* Free buffers 
     * ARGUMENTS: None.
     * RETURNS: None.
     */ 
     VOID Free( VOID );

     /* Denstructor */
     ~sbuffer( VOID )
     {
     }
  };

  class sbuffer_manager : public resource_manager<sbuffer>
  {
  public:
    /* Create buffer function
     * ARGUMENTS: 
     *   - binding point:
     *       UINT BindingPoint;
     *   - data structure:
     *       const data_structure *Ptr;
     *   - number of vec4;
     * RETURNS:
     *   (buffer &) pointer to buffer
     */
    template<typename data_structure>
      sbuffer * CreateSBuffer( UINT BindingPoint, const data_structure *Ptr, const INT Size )
      {
        return Add(sbuffer().Create(BindingPoint, Ptr, Size));
      } /* End of 'nicg::buffer_manager::CreateBuffers' function */
  };
} // end of namespace

#endif /* __buffers_h_ */

/* END OF 'buffers.h' FILE */
