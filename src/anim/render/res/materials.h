/* FILE NAME  : materials.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 24.07.2021
 * PURPOSE    : Materials declaration module.
 */

#ifndef __materials_h_
#define __materials_h_

//#include "../../../def.h"
#include "shader.h"
#include "res.h"
#include "texture.h"

namespace nicg
{
  /* material class */
  class material
  {
    /* Manager - friend */
    friend class resource_manager<material, std::string>;
    friend class material_manager;
    friend class render;

  private:

    /* Delete material function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Free( VOID )
    {
      memset(this, 0, sizeof(material));
    } /* End of 'Free' function */
  public:
    vec3 Ka;        /* Ambient coefficient */
    vec3 Kd;        /* Diffuse coefficient */
    vec3 Ks;        /* Specular coefficient */
    FLT Ph;         /* Phong power coefficient */
    FLT Trans;


    shader *Shd;     /* Material shader pointer */
    texture *Tex[8]; /* Texture array */
    CHAR Name[300];  /* Material name */

    /* Class constructor */
    material( VOID ) : Name("")
    {
      for (INT i = 0; i < 8; i++)
        Tex[i] = nullptr;

      //Tex[0] = texture_manager::Add("BIN/TEXTURES/GIR.g32", 4);
    } /* End of 'material' constructor */

    /* Constructor by all fields */
    material( std::string MatName, vec3 RKa, vec3 RKd, 
      vec3 RKs, FLT RPh, FLT Transp = 1, std::string ShdName = "DEFAULT" );

    /* material constructor by string */
    material( std::string MaterialName )
    {
      Ph = 30;
      Trans = 1;
      Ka = vec3(0.1, 0.3, 0.6);
      Ka = vec3(0.3, 0.1, 0.3);
      Ka = vec3(0.6, 0.3, 0.1);

      for (INT i = 0; i < 8; i++)
        Tex[i] = nullptr;

      strcpy((CHAR *)Name, MaterialName.c_str());
    } /* End of 'material' constructor */

    /* Class destructor */
    ~material( VOID )
    {
    } /* End of 'material' destructor */

    /* Apply material function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Apply( shader *shd );
  }; /* End of 'material' class */

  /* material manager class */
  class material_manager : public resource_manager<material, std::string>
  {
  protected:

    ~material_manager( VOID )
    {
    }

    /* Initialize materials function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Init( VOID )
    {
    } /* End of 'Init' function */

    /* Close manager function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Close( VOID )
    {
      resource_manager::Close();
      OutputDebugString("Material manager destructed\n");
    } /* End of 'Close' function */

  public:
    /* Get default material function.
     * ARGUMENTS: None.
     * RETURNS: (material *) default material.
     */
    material * GetDefault( VOID );

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
    material * Add( const CHAR *MaterialName, vec3 Ka, vec3 Kd, vec3 Ks, FLT Ph, const CHAR *ShdName );

    /* Add material to manager function.
     * ARGUMENTS:
     *   - material to add:
     *       material NMtl;
     * RETURNS: (material *) created material.
     */
    material * Add( material &NMtl );

  }; /* End of 'material_manager' class */

  ///* material class */
  //class material : public shader_manager
  //{
  //  friend class material_manager;
  //  friend class render;
  //  friend class resource_manager<material, std::string>;
  //
  //private:
  //  CHAR Name[100]; /* Material name */
  //
  //  /* Illumination coefficients */
  //  vec3
  //    Ka,           /* Ambient coefficient */
  //    Kd,           /* Diffuse coefficient */
  //    Ks;           /* Specular coefficient */
  //  FLT Ph;         /* Phong power coefficient */
  //  FLT Trans;      /* Transparency factor */
  //
  //public:
  //
  //  /* Material constructor */
  //  material( VOID ) : Name(""), Ka(), Kd(), Ks(), Ph(0), Trans(0)
  //  {
  //  } /* End of 'material' function */
  //
  //  /* Material constructor */
  //  material( CHAR *MtlName ) : Ka(), Kd(), Ks(), Ph(0), Trans(0)
  //  {
  //    strcpy((CHAR *)Name, MtlName);
  //  } /* End of 'material' function */
  //
  //  /* Material constructor */
  //  material( CHAR *MtlName, vec3 Ka, vec3 Kd, vec3 Ks, FLT Ph, FLT Trans ) : Ka(Ka), Kd(Kd), Ks(Ks), Ph(Ph), Trans(Trans)
  //  {
  //    strcpy((CHAR *)Name, MtlName);
  //  } /* End of 'material' function */
  //
  //  /* Shader deconstructor */
  //  ~material( VOID )
  //  {
  //  } /* End of '~material' function */
  //
  //  /* Material storage init function.
  //   * ARGUMENTS: None.
  //   * RETURNS: None.
  //   */
  //  VOID Apply( INT ProgId );
  //
  //  /* Unload shader program from memory files function.
  //   * ARGUMENTS: None.
  //   * RETURNS: None.
  //   */
  //  VOID Free( VOID )
  //  {
  //  }
  //}; /* End of 'material' class */
  //
  ///* Material manager class */
  //class material_manager : public resource_manager<material, std::string>
  //{
  //public:
  //  material_manager( VOID ) : resource_manager<material, std::string>()
  //  {
  //  }
  //
  //  /* Update shaders programms.
  //   * ARGUMENTS: 
  //   *   material:
  //   *    - material mtl.
  //   * RETURNS: 
  //   *   material
  //   *    - (material) mtl.
  //   */
  //  material *Add( material &mtl )
  //  {
  //    material *Mtl = nullptr;
  //
  //    if ((Mtl = Find(mtl.Name)) != nullptr)
  //      return Mtl;
  //    return resource_manager::Add(mtl);
  //  } /* End of 'Add' function */
  //
  //  /* Make default material function.
  //   * ARGUMENTS: None.
  //   * RETURNS:
  //   *   (material *) material table.
  //   */
  //  material* GetDef( VOID )
  //  {
  //    //material *mtl = nullptr;
  //    //
  //    //if ((mtl = Find("bronze")) != nullptr)
  //    //  return mtl;
  //
  //    material Mtl;
  //
  //    strcpy((CHAR *)Mtl.Name, "bronze");
  //    Mtl.Ka = vec3(0.25, 0.148, 0.06475);
  //      
  //    Mtl.Kd = vec3(0.4, 0.2368, 0.1036);
  //    Mtl.Ks = vec3(0.774597, 0.458561, 0.200621);
  //    Mtl.Ph = 30;
  //    Mtl.Trans = 1;
  //    return Add(Mtl);
  //  } /* End of 'GetDef' function */
  //
  //  
  //};
}

#endif /* __materials_h_ */

/* END OF 'materials.h' FILE */