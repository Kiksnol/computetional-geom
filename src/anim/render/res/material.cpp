/* FILE NAME: material.cpp
 * PROGRAMMER: VB5
 * LAST UPDATE: 25.07.2021
 * PURPOSE: Material module
 */

#include "../../anim.h"
#include "materials.h"
#include "shader.h"

/* Project namespace */
namespace nicg
{
  /* Constructor by all fields */
  material::material( std::string MatName, vec3 RKa, vec3 RKd, 
    vec3 RKs, FLT RPh, FLT Transp, std::string ShdName ) : Ka(RKa), Kd(RKd), Ks(RKs), Ph(RPh), Trans(Transp)
  {
    for (INT i = 0; i < 8; i++)
      Tex[i] = nullptr;
    strcpy((CHAR *)Name, MatName.c_str());
    this->Shd = anim::GetPtr()->shader_manager::Add(ShdName.c_str());
  } /* End of 'material' constructor */

  /* Add material to manager function.
   * ARGUMENTS:
   *   - material name to add:
   *       const CHAR *MaterialName;
   *   - illumination coefficients:
   *       vec3 Ka, Kd, Ks; FLT Ph;
   *   - shader name to load shader:
   *       const CHAR *ShdName;
   * RETURNS: (material *) created material.
   */
  material * material_manager::Add( const CHAR *MaterialName, vec3 Ka, vec3 Kd, vec3 Ks, FLT Ph, 
    const CHAR *ShdName )
  {
    material *fMtl = nullptr;
    material Mtl(MaterialName, Ka, Kd, Ks, Ph, 1, ShdName);
  
    if ((fMtl = Find(Mtl.Name)) != nullptr)
      return fMtl;
  
    return resource_manager::Add(Mtl);
  } /* End of 'material_manager::Add' function. */

  /* Add material to manager function.
   * ARGUMENTS:
   *   - material to add:
   *       material &NMtl;
   * RETURNS: (material *) created material.
   */
  material * material_manager::Add( material &NMtl )
  {
    material *fMtl = nullptr;
  
    if ((fMtl = Find(NMtl.Name)) != nullptr)
      return fMtl;
  
    return resource_manager::Add(NMtl);
  } /* End of 'material_manager::Add' function. */

  /* Get default material function.
   * ARGUMENTS: None.
   * RETURNS: (material *) default material.
   */
  material * material_manager::GetDefault( VOID )
  {
    material *def = nullptr;

    if ((def = Find("DEFAULT_MAT")) != nullptr)
      return def;

    material mtl("DEFAULT_MAT", vec3(0.24725, 0.2245, 0.0645),
                                vec3(0.34615, 0.3143, 0.0903),
                                vec3(0.797353, 0.723991, 0.208006), 83.2);

    return resource_manager::Add(mtl);
  } /* End of 'material_manager::GetDefault' function. */

  /* Apply material function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID material::Apply( shader *shd )
  {
    INT loc, ProgId = shd->ProgId;
    
    glUseProgram(ProgId);

    if ((loc = glGetUniformLocation(ProgId, "Ka")) != -1)
      glUniform3fv(loc, 1, &Ka[0]);
    if ((loc = glGetUniformLocation(ProgId, "Kd")) != -1)
      glUniform3fv(loc, 1, &Kd[0]);
    if ((loc = glGetUniformLocation(ProgId, "Ks")) != -1)
      glUniform3fv(loc, 1, &Ks[0]);
    if ((loc = glGetUniformLocation(ProgId, "Trans")) != -1)
      glUniform1f(loc, Trans);
    if ((loc = glGetUniformLocation(ProgId, "Ph")) != -1)
      glUniform1f(loc, Ph);
    if ((loc = glGetUniformLocation(ProgId, "Time")) != -1)
      glUniform1f(loc, anim::GetPtr()->Time);


    /* Set textures */
    for (INT i = 0; i < 8; i++)
    {
      CHAR tname[] = "IsTexture0";
    
      tname[9] = '0' + i;
      if (this->Tex[i] != nullptr)
      {
        /* Activate sampler */
        glActiveTexture(GL_TEXTURE0 + i);
        /* Bind texture to sampler */
        glBindTexture(GL_TEXTURE_2D, this->Tex[i]->TexId);
      }

      if (this->Tex[i] != nullptr)
      {
        INT val = Tex[i]->TexId != -1;
        if ((loc = glGetUniformLocation(ProgId, tname)) != -1)
          glUniform1i(loc, val);
      }
    }
  } /* End of 'material::Apply' function */
}


///* Material storage init function.    Source
// * ARGUMENTS: None.
// * RETURNS: None.
// */
//VOID nicg::material::Apply( INT ProgId )
//{ 
//  INT loc;
//
//  if ((loc = glGetUniformLocation(ProgId, "Ka")) != -1)
//    glUniform3fv(loc, 1, &Ka[0]);
//  if ((loc = glGetUniformLocation(ProgId, "Kd")) != -1)
//    glUniform3fv(loc, 1, &Kd[0]);
//  if ((loc = glGetUniformLocation(ProgId, "Ks")) != -1)
//    glUniform3fv(loc, 1, &Ks[0]);
//  if ((loc = glGetUniformLocation(ProgId, "Ph")) != -1)
//    glUniform1f(loc, Ph);
//} /* End of 'Apply' function */

/* END OF 'material.cpp' FILE */