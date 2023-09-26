/* FILE NAME  : render.cpp
 * PROGRAMMER : ND4
 * LAST UPDATE: 26.07.2021
 * PURPOSE    : Render realisation.
 */

#include "../anim.h"
#include "render.h"

/* Primitive creation function.
 * ARGUMENTS:
 *   - prim type:
 *       prim_type Type;
 *   - vertex attributes array:
 *       nicg::vertex<FLT> *V;
 *   - number of vertices:
 *       INT NumOfV;
 *   - index array (for trimesh – by 3 ones, may be NULL)
 *       INT *I;
 *   - number of indices
 *       INT NumOfI;
 * RETURNS: None.
 */
VOID nicg::prim::PrimCreate( nicg::prim_type Type, nicg::vertex *V, INT NumOfV, INT *I, INT NumOfI )
{
  ZeroMemory(this, sizeof(prim));
  this->Type = Type;

  EvalBoundBox(V, I, NumOfI);

  if (V != nullptr && NumOfV != 0)
  {
    glGenBuffers(1, &VBuf);
    glGenVertexArrays(1, &VA);

    glBindVertexArray(VA);
    glBindBuffer(GL_ARRAY_BUFFER, VBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nicg::vertex) * NumOfV, V, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, sizeof(nicg::vertex),
                          (VOID *)0); /* position */
    glVertexAttribPointer(1, 2, GL_FLOAT, FALSE, sizeof(nicg::vertex),
                          (VOID *)sizeof(vec3)); /* texture coordinates */
    glVertexAttribPointer(2, 3, GL_FLOAT, FALSE, sizeof(nicg::vertex),
                          (VOID *)(sizeof(vec3) + sizeof(vec2))); /* normal */
    glVertexAttribPointer(3, 4, GL_FLOAT, FALSE, sizeof(nicg::vertex),
                          (VOID *)(sizeof(vec3) * 2 + sizeof(vec2))); /* color */

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
  }
  if (I != nullptr && NumOfI != 0)
  {
    glGenBuffers(1, &IBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INT) * NumOfI, I, GL_STATIC_DRAW);
    NumOfEl = NumOfI;
  }
  else
    NumOfEl = NumOfV;

  Mtl = anim::GetPtr()->material_manager::GetDefault();
  Mtl->Tex[0] = anim::GetPtr()->texture_manager::Add("BIN/TEXTURES/GIR.g32", 4);
  Trans.intoIdentity();
} /* End of 'PrimCreate' function */

/* Load '.g3dm' function.
 * ARGUMENTS:
 *   - filename to load:
 *       CHAR *FileName;
 * RETURNS: (BOOL) TRUE if success, FALSE otherwise
 */
BOOL nicg::model::Load( const CHAR *FileName )
{
  BYTE *mem, *ptr;
  INT flen, p;// t, m;
  std::fstream f;
  DWORD Sign;         /* == "G3DM" */
  INT NumOfPrims;
  INT NumOfMaterials;
  INT NumOfTextures;
  INT *MtlNums;
  struct tagG3DMMTL
  {
    CHAR Name[300];     /* Material name */
  
    /* Illumination coefficients */
    vec3 Ka, Kd, Ks; /* Ambient, diffuse, specular coefficients */
    FLT Ph;          /* Phong power coefficient – shininess */
    FLT Trans;       /* Transparency factor */
    DWORD Tex[8];    /* Texture references 
                          * (8 time: texture number in G3DM file, -1 if no texture) */
  
    /* Shader information */
    CHAR ShaderString[300]; /* Additional shader information */
    DWORD Shader;           /* Shader number (uses after load into memory) */
  } *mtls;
  struct tagTEX1
  {
    CHAR Name[300]; /* Texture name */
    DWORD W, H;     /* Texture image size in pixels */
    DWORD C;        /* Texture image components (1-mono, 3-bgr or 4-bgra) */
  } *tex, *AllTex;
  INT TexNo = 0;
  struct tagTEXN
  {
    INT Nums[100];
  } *Textures;

  ZeroMemory(this, sizeof(model));
  /* Open file */
  f.open(FileName, std::fstream::in | std::fstream::binary);

  /* Obtain file length */
  f.seekg(0, std::fstream::end);
  flen = f.tellg();
  f.seekg(0, std::fstream::beg);

  /* Allocate memory and load whole file to memory */
  try
  {
    mem = new BYTE[flen];
  }
  catch (...)
  {
    f.close();
    return FALSE;
  }

  f.read((CHAR *)mem, flen);
  f.close();
  ptr = mem;
  rdr rd(ptr);

  rd(&Sign);
  if (Sign != *(DWORD *)"G3DM")
  {
    delete [] mem;
    return FALSE;
  }
  rd(&NumOfPrims);
  rd(&NumOfMaterials);
  if (NumOfPrims)
    MtlNums = new INT[NumOfPrims];
  else
    MtlNums = nullptr;
  if (NumOfMaterials)
    Textures = new tagTEXN[NumOfMaterials];
  else
    Textures = nullptr;
  rd(&NumOfTextures);
  if (NumOfTextures)
    AllTex = new tagTEX1[NumOfTextures];
  else
    AllTex = nullptr;

  /* Primitives */
  for (p = 0; p < NumOfPrims; p++)
  {
    prim *Pr = new prim;
    DWORD NumOfVertexes;
    DWORD NumOfFacetIndexes;  /* num of facets * 3 */
    DWORD MtlNo;              /* Material number in table below (in material section) */
    vertex *V;
    INT *Ind;

    rd(&NumOfVertexes);
    rd(&NumOfFacetIndexes);
    rd(&MtlNo);
    V = new vertex[NumOfVertexes];
    rd(V, NumOfVertexes);
    Ind = new INT[NumOfFacetIndexes];
    rd(Ind, NumOfFacetIndexes);
    Pr->PrimAutoNorm(V, Ind, NumOfFacetIndexes);
    Pr->PrimCreate(prim_type::PRIM_TRIMESH, V, NumOfVertexes, Ind, NumOfFacetIndexes);
    Prims << Pr;
    if (MtlNums)
      MtlNums[p] = MtlNo;

    delete[] V;
    delete[] Ind;
  }

  /* Materials */
  mtls = (struct tagG3DMMTL *)ptr, ptr += sizeof(struct tagG3DMMTL) * NumOfMaterials;
  for (INT m = 0; m < NumOfMaterials; m++)
  {
    material mtl;

    mtl = material(mtls[m].Name, mtls[m].Ka, mtls[m].Kd, mtls[m].Ks, mtls[m].Ph, mtls[m].Trans);
    for (INT i = 0; i < 100; i++)
      Textures[m].Nums[i] = (mtls[m].Tex[i] == -1 ? -1 : mtls[m].Tex[i]);
    anim::GetPtr()->material_manager::Add(mtl);
  }
  /* Textures */
  for (INT t = 0; t < NumOfTextures; t++)
  {
    tex = new tagTEX1;
    rd(tex);
    if (AllTex)
      AllTex[TexNo++] = *tex;
    anim::GetPtr()->texture_manager::Add(tex->Name, tex->W, tex->H, tex->C, ptr);
    ptr += tex->W * tex->H * tex->C;
    delete tex;
  }
  /* Apply materials to prims */
  for (p = 0; p < NumOfPrims; p++)
  {
    Prims[p]->Mtl = anim::GetPtr()->material_manager::Find(mtls[MtlNums[p]].Name);
    for (INT m = 0; m < NumOfMaterials; m++)
    {
      for (INT t = 0; t < NumOfTextures; t++)
      {
        INT Num = mtls[MtlNums[p]].Tex[Textures[m].Nums[t]];
    
        if (Num < 0 || Num > NumOfTextures)
          continue;
    
        tagTEX1 tex0 = AllTex[Num];
    
        if (Prims[p]->Mtl->Tex[t] == nullptr)
          Prims[p]->Mtl->Tex[t] = anim::GetPtr()->texture_manager::Find(tex0.Name);
      }
    }
  }
  delete[] mem;
  delete[] MtlNums;
  delete[] Textures;
  delete[] AllTex;
  return TRUE;
} /* End of 'model::Load' function. */

VOID nicg::model::SetShader( const CHAR *ShaderName )
{
  for (INT i = 0; i < Prims.Size(); i++)
    Prims[i]->Mtl->Shd = anim::GetPtr()->shader_manager::Add(ShaderName);
}


///* Load '.g3dm' function.
// * ARGUMENTS:
// *   - filename to load:
// *       CHAR *FileName;
// * RETURNS: (BOOL) TRUE if success, FALSE otherwise
// */
//BOOL nicg::model::Load( const CHAR *FileName )
//{
//  BYTE *mem, *ptr;
//  INT flen, p;// t, m;
//  FILE *F;
//  DWORD Sign;         /* == "G3DM" */
//  INT NumOfPrims;
//  INT NumOfMaterials;
//  INT NumOfTextures;
//  INT TexNo = 0;
//  struct tagG3DMMTL
//  {
//    CHAR Name[300];     /* Material name */
//  
//    /* Illumination coefficients */
//    vec3 Ka, Kd, Ks;     /* Ambient, diffuse, specular coefficients */
//    FLT Ph;             /* Phong power coefficient – shininess */
//    FLT Trans;          /* Transparency factor */
//    DWORD Tex[8];       /* Texture references 
//                        * (8 time: texture number in G3DM file, -1 if no texture) */
//  
//    /* Shader information */
//    CHAR ShaderString[300]; /* Additional shader information */
//    DWORD Shader;       /* Shader number (uses after load into memory) */
//  } *mtls;
//  struct tagTEXN
//  {
//    INT Nums[8];
//  } *Textures;
//
//  //struct tagTEX
//  //{
//  //  CHAR Name[100];
//  //  DWORD W, H;    
//  //  DWORD C;
//  //} *AllTex;
//
//  INT *MtlNums;
//
//  ZeroMemory(this, sizeof(model));
//  /* Open file */
//  if ((F = fopen(FileName, "rb")) == NULL)
//    return FALSE;
//
//  /* Obtain file length */
//  fseek(F, 0, SEEK_END);
//  flen = ftell(F);
//  rewind(F);
//
//  /* Allocate memory and load whole file to memory */
//  try
//  {
//    mem = new BYTE[flen];
//  }
//  catch (...)
//  {
//    fclose(F);
//    return FALSE;
//  }
//
//  fread(mem, 1, flen, F);
//  fclose(F);
//  ptr = mem;
//  Sign = *(DWORD *)ptr, ptr += 4;
//  if (Sign != *(DWORD *)"G3DM")
//  {
//    delete [] mem;
//    return FALSE;
//  }
//  NumOfPrims = *(DWORD *)ptr, ptr += 4;
//  MtlNums = new INT[NumOfPrims];
//  NumOfMaterials = *(DWORD *)ptr, ptr += 4;
//  Textures = new tagTEXN[NumOfMaterials];
//  NumOfTextures = *(DWORD *)ptr, ptr += 4;
//  //AllTex = new tagTEX[NumOfTextures];
//
//  /* Primitives */
//  for (p = 0; p < NumOfPrims; p++)
//  {
//    prim *Pr = new prim;
//    DWORD NumOfVertexes;      /* Number of vertexes */
//    DWORD NumOfFacetIndexes;  /* Number of facets * 3 */
//    DWORD MtlNo;              /* Material number in table below (in material section) */
//    vertex *V;
//    INT *Ind;
//
//    NumOfVertexes = *(DWORD *)ptr, ptr += 4;
//    NumOfFacetIndexes = *(DWORD *)ptr, ptr += 4;
//    MtlNo = *(DWORD *)ptr, ptr += 4;
//    V = (vertex *)ptr, ptr += sizeof(vertex) * NumOfVertexes;
//    Ind = (INT *)ptr, ptr += sizeof(INT) * NumOfFacetIndexes;
//    Pr->PrimAutoNorm(V, Ind, NumOfFacetIndexes);
//    Pr->PrimCreate(prim_type::PRIM_TRIMESH, V, NumOfVertexes, Ind, NumOfFacetIndexes);
//    Prims << Pr;
//    MtlNums[p] = MtlNo;
//  }
//
//  /* Materials */
//  mtls = (struct tagG3DMMTL *)ptr, ptr += sizeof(struct tagG3DMMTL) * NumOfMaterials;
//  for (INT m = 0; m < NumOfMaterials; m++)
//  {
//    material mtl;
//
//    mtl = material(mtls[m].Name, mtls[m].Ka, mtls[m].Kd, mtls[m].Ks, mtls[m].Ph, mtls[m].Trans);
//    for (INT i = 0; i < 8; i++)
//      Textures[m].Nums[i] = (mtls[m].Tex[i] == -1 ? -1 : mtls[m].Tex[i]);
//    anim::GetPtr()->material_manager::Add(mtl);
//  }
//
//  /* Textures */
//  for (INT t = 0; t < NumOfTextures; t++)
//  {  
//    struct tagTEX1
//    {
//      CHAR Name[300]; /* Texture name */
//      DWORD W, H;     /* Texture image size in pixels */
//      DWORD C;        /* Texture image components (1-mono, 3-bgr or 4-bgra) */
//    } *tex = (tagTEX1 *)ptr;
//  
//    ptr += sizeof(*tex); // (tagTEX *)ptr;
//  
//    //AllTex[TexNo++] = *tex;
//  
//    anim::GetPtr()->texture_manager::AddImg(tex->Name, tex->W, tex->H, tex->C, ptr);
//    ptr += tex->W * tex->H * tex->C;
//  }
//
//  for (INT p = 0; p < NumOfPrims; p++)
//  {
//    Prims[p]->Mtl = anim::GetPtr()->material_manager::Find(mtls[MtlNums[p]].Name);
//    //
//    //for (INT m = 0; m < NumOfMaterials; m++)
//    //{
//    //  for (INT t = 0; t < NumOfTextures; t++)
//    //  {
//    //    INT Num = mtls[MtlNums[p]].Tex[Textures[m].Nums[t]];
//    //
//    //    if (Num < 0 || Num > 7)
//    //      continue;
//    //
//    //    tagTEX tex0 = AllTex[Num];
//    //
//    //    Prims[p]->Mtl->Tex[t] = anim::GetPtr()->texture_manager::Find(tex0.Name);
//    //  }
//    //}
//  }
//
//  //delete[] AllTex;
//  delete[] Textures;
//  delete[] MtlNums;
//  delete[] mem;
//  return TRUE;
//} /* End of 'Load' function. */

/* END OF 'render.cpp' FILE */