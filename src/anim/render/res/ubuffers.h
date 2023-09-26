/*************************************************************
 * Copyright (C) 2022
 *    Computer Graphics Support Group of 30 Phys-Math Lyceum
 *************************************************************/

 /* FILE NAME   : buffers.h
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


#ifndef __buffers_h_
#define __buffers_h_

#include "res.h"

// namespace for buffers manager
namespace nicg
{
  // class buffers
  class ubuffer
  {
    friend class buffer_manager;
    template<typename entry_type, typename index_type>
    friend class resource_manager;
    friend class material;
  private:
    UINT Id;        // open gl id
    UINT BindPoint; // binding point for shader
    INT NumOfQuads; // number of vec4

  public:
    /* default constructor */
    ubuffer( VOID ) : Id(0), BindPoint(0), NumOfQuads(0)
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
      ubuffer & Create( UINT BindingPoint, const data_structure *Ptr )
      {
        if (this != nullptr)
        Free();
        BindPoint = BindingPoint;
        NumOfQuads = sizeof(*Ptr) / 16;

        glGenBuffers(1, &Id);
        glBindBuffer(GL_UNIFORM_BUFFER, Id);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(*Ptr), Ptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        Apply();
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
      VOID Update( const data_structure *Ptr )
      {
        glBindBuffer(GL_UNIFORM_BUFFER, Id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(*Ptr), Ptr);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
      } // end of 'nicg::buffer::Update' function
   
    /* Free buffers 
     * ARGUMENTS: None.
     * RETURNS: None.
     */ 
     VOID Free( VOID );

     /* Denstructor */
     ~ubuffer( VOID )
     {
     }
  };

  class ubuffer_manager : public resource_manager<ubuffer>
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
      ubuffer * CreateUBuffer( UINT BindingPoint, const data_structure *Ptr )
      {
        return Add(ubuffer().Create(BindingPoint, Ptr));
      } /* End of 'nicg::buffer_manager::CreateBuffers' function */

  };
} // end of namespace

#endif /* __buffers_h_ */

/* END OF 'buffers.h' FILE */
