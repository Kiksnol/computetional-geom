/*************************************************************
 * Copyright (C) 2022
 *    Computer Graphics Support Group of 30 Phys-Math Lyceum
 *************************************************************/

 /* FILE NAME   : sbuffers.cpp
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


#include "sbuffers.h"
#include "../../../def.h"

/* Apply buffers 
 * ARGUMENTS: None.
 * RETURNS: None.
 */ 
VOID nicg::sbuffer::Apply( VOID )
{
  if (BindPoint != 0)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BindPoint, Id);
} // end of 'nicg::buffer::Apply' function

/* Free buffers 
 * ARGUMENTS: None.
 * RETURNS: None.
 */ 
VOID nicg::sbuffer::Free( VOID )
{
  if (Id != 0)
  {
    glDeleteBuffers(1, &Id);
    Id = 0;
    BindPoint = 0;
    BufSize = 0;
  }
} // end of 'nicg::buffer::Free' function

/* END OF 'buffers.cpp' FILE */