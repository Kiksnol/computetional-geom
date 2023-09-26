/* FILE NAME  : texture.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 26.07.2021
 * PURPOSE    : Texture declaration module.
 */

#ifndef __texture_h_
#define __texture_h_

#include "res.h"

#include <fstream>

namespace nicg
{
  /* texture class */
  class texture
  {
    /* Manager - friend */
    friend class resource_manager<texture, std::string>;
    friend class texture_manager;
    friend class render;
    friend class material;

  private:
    INT W, H;   /* Texture size in pixels */
    UINT TexId; /* OpenGL texture Id */

    /* Delete texture function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Free( VOID )
    {
      memset(this, 0, sizeof(texture));
    } /* End of 'Free' function */
  public:
    CHAR Name[300]; /* Texture name */

    /* Class constructor */
    texture( VOID ) : Name("")
    {
    } /* End of 'texture' constructor */

    /* texture constructor by string */
    texture( std::string TextureName )
    {
      strcpy((CHAR *)Name, TextureName.c_str());
    } /* End of 'texture' constructor */

    /* Class destructor */
    ~texture( VOID )
    {
    } /* End of 'texture' destructor */

    /* Apply texture function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Apply( VOID )
    {
    }

    /* Add texture from image function.
     * ARGUMENTS:
     *   - image file name:
     *       CHAR *Name;
     *   - image width:
     *       INT W;
     *   - image hight:
     *       INT H;
     *   - components:
     *       INT C;
     *   - color bits:
     *       VOID *Bits;
     * RETURNS: None.
     */
    VOID LoadArray( const CHAR *Name, INT w, INT h, INT C, VOID *Bits )
    {
      FLT mips;

      mips = w > h ? h : w;
      mips = log(mips) / log(2);
      if (mips < 1)
        mips = 1;

      /* Allocate texture space */
      glGenTextures(1, &TexId);
      glBindTexture(GL_TEXTURE_2D, TexId);

      /* Upload texture */
      glTexStorage2D(GL_TEXTURE_2D, mips, C == 3 ? GL_RGB8 : C == 4 ? GL_RGBA8 : GL_R8, w, h);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, Bits);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      strcpy(this->Name, Name);
      W = w;
      H = h;

      glBindTexture(GL_TEXTURE_2D, 0);
    } /* End of 'LoadArray' function */

    /* Load texture from '.g32' or '.g24' file function.
     * ARGUMENTS:
     *   - filename to load:
     *       std::string FileName;
     *   - texture flag to load:
     *       INT tag;
     * RETURNS: (texture &) created texture
     */
    VOID LoadFromFile( std::string FileName, INT tag )
    {
      DWORD d = 0;
      DWORD *img;
      INT
        w = 0,
        h = 0,
        x,
        y;
      std::fstream f;
    
      f.open(FileName, std::fstream::in | std::fstream::binary);
      if (!f.is_open())
        return;
      f.read((CHAR *)&w, 2);
      f.read((CHAR *)&h, 2);
    
      if ((img = (DWORD *)malloc(4 * w * h)) == NULL)
      {
        f.close();
        return;
      }
    
      for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
        {
          f.read((CHAR *)&d, tag);
          d |= 0xFF000000;
          img[(h - y - 1) * w + x] = d;
        }
      this->LoadArray(FileName.c_str(), w, h, tag, img);

      free(img);
      f.close();
    } /* End of 'LoadFromFile' function */
  }; /* End of 'texture' class */

  /* texture manager class */
  class texture_manager : public resource_manager<texture, std::string>
  {
  protected:

    ~texture_manager( VOID )
    {
    }

    /* Initialize textures function.
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
      OutputDebugString("Texture manager destructed\n");
    } /* End of 'Close' function */

  public:
    /* Add texture to manager function.
     * ARGUMENTS:
     *   - name to add:
     *       const CHAR *TexName;
     *   - width and height:
     *       INT W, H;
     *   - image components (rgb, rgba):
     *       INT C;
     *   - data to save texture:
     *       VOID *Bits;
     * RETURNS: (texture *) added texture
     */
    texture * Add( const CHAR *TexName, INT W, INT H, INT C, VOID *Bits )
    {
      texture *fi;
      texture Tex(TexName);

      if ((fi = Find(TexName)) != nullptr)
        return fi;

      Tex.LoadArray(TexName, W, H, C, Bits);
      return resource_manager::Add(Tex);
    } /* End of 'Add' function */

    /* Add texture by filename function.
     * ARGUMENTS:
     *   - filename to add:
     *       const CHAR *FileName;
     *   - flag to load (G24/G32)
     *       INT flag;
     * RETURNS: (texture *) added texture
     */
    texture * Add( const CHAR *FileName, INT flag )
    {
      texture *fi;
      texture Tex(FileName);

      if ((fi = Find(FileName)) != nullptr)
        return fi;

      Tex.LoadFromFile(FileName, flag);
      return resource_manager::Add(Tex);
    } /* End of 'Add' function */

  }; /* End of 'texture_manager' class */

  ///* Texture class */
  //class texture// : public texture_manager
  //{
  //private:
  //  CHAR Name[100]; /* Material name */
  //  
  //  INT W, H;               /* Texture size in pixels */
  //  UINT TexId;             /* OpenGL texture Id */ 
  //public:
  //  /* Texture constructor */
  //  texture( VOID ) : Name(""), W(0), H(0), TexId(0)
  //  {
  //  } /* End of 'texture' function */
  //
  //  /* Texture constructor */
  //  texture( const CHAR *TexName ) : W(0), H(0), TexId(0)
  //  {
  //    strcpy((CHAR *)Name, "");
  //    strcpy((CHAR *)Name, TexName);
  //  } /* End of 'texture' function */
  //
  //  /* Texture constructor */
  //  texture( const CHAR *TexName, INT W, INT H ) : W(W), H(H), TexId(0)
  //  {
  //    strcpy((CHAR *)Name, TexName);
  //  } /* End of 'texture' function */
  //
  //  /* Texture deconstructor */
  //  ~texture( VOID )
  //  {
  //  } /* End of '~texture' function */
  //
  //  /* Add texture from image function.
  //   * ARGUMENTS:
  //   *   - image file name:
  //   *       CHAR *Name;
  //   *   - image width:
  //   *       INT W;
  //   *   - image hight:
  //   *       INT H;
  //   *   - components:
  //   *       INT C;
  //   *   - color bits:
  //   *       VOID *Bits;
  //   * RETURNS: None
  //   */
  //  VOID ImgAdd( const CHAR *Name, INT w, INT h, INT C, VOID *Bits )
  //  {
  //    FLT mips;
  //  
  //    mips = w > h ? h : w;
  //    mips = log(mips) / log(2);
  //    if (mips < 1)
  //      mips = 1;
  //  
  //    /* Allocate texture space */
  //    glGenTextures(1, &TexId);
  //    glBindTexture(GL_TEXTURE_2D, TexId);
  //  
  //    /* Upload texture */
  //    glTexStorage2D(GL_TEXTURE_2D, mips, C == 3 ? GL_RGB8 : C == 4 ? GL_RGBA8 : GL_R8, w, h);
  //    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, Bits);
  //    glGenerateMipmap(GL_TEXTURE_2D);
  //  
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //  
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //  
  //    strncpy(this->Name, Name, 100 - 1);
  //    this->W = w;
  //    this->H = h;
  //  
  //    glBindTexture(GL_TEXTURE_2D, 0);
  //  } /* End of 'AddImg' function */
  //
  //  /* Texture load from *.G32 or *.G24 file function.
  //   * ARGUMENTS:
  //   *   - Texture file name:
  //   *       CHAR *FileName;
  //   *   - G32 or G24 tag:
  //   *       INT tag;
  //   * RETURNS:
  //   *   (INT) new img stock number.
  //   */
  //  VOID Load( std::string FileName, INT tag )
  //  {
  //    DWORD d = 0;
  //    DWORD *img;
  //    INT
  //      w = 0,
  //      h = 0,
  //      x,
  //      y;
  //    std::fstream f;
  //  
  //    f.open(FileName, std::fstream::in | std::fstream::binary);
  //    if (!f.is_open())
  //      return;
  //
  //    f.read((CHAR *)&w, 2);
  //    f.read((CHAR *)&h, 2);
  //
  //    if ((img = (DWORD *)malloc(4 * w * h)) == nullptr)
  //    {
  //      f.close();
  //      return;
  //    }
  //  
  //    for (x = 0; x < w; x++)
  //      for (y = 0; y < h; y++)
  //      {
  //        f.read((CHAR *)&d, tag);
  //        d |= 0xFF000000;
  //        img[(h - y - 1) * w + x] = d;
  //      }
  //    this->ImgAdd(FileName.c_str(), w, h, tag, img);
  //  
  //    free(img);
  //    f.close();
  //  } /* End of 'G32Load' function */
  //
  //  /* Unload texture from memory files function.
  //   * ARGUMENTS: None.
  //   * RETURNS: None.
  //   */
  //  VOID Free( VOID )
  //  {
  //    ZeroMemory(this, 0);
  //  } /* End of 'Free' function */
  //
  //  friend class texture_manager;
  //  friend class render;
  //  friend class material;
  //  friend class resource_manager<texture, std::string>;
  //}; /* End of 'texture' class */
  //
  ///* Texture manager class */
  //class texture_manager : public resource_manager<texture, std::string>
  //{
  //public:
  //  texture_manager( VOID ) : resource_manager<texture, std::string>()
  //  {
  //  }
  //
  //  /* Add new texture.
  //   * ARGUMENTS: 
  //   *   texture name:
  //   *    - const CHAR *TexName.
  //   *   texture weight and height:
  //   *    - INT W, INT H.
  //   *   image components:
  //   *    - INT C.
  //   *   data:
  //   *    - VOID *Bits.
  //   * RETURNS: (texture *) added texture.
  //   */
  //  texture *AddImg( const CHAR *TexName, INT W, INT H, INT C, VOID *Bits )
  //  {
  //    texture *tex;
  //    texture Tex(TexName);
  //
  //    if ((tex = Find(TexName)) != nullptr)
  //      return tex;
  //
  //    Tex.ImgAdd(TexName, W, H, C, Bits);
  //    return resource_manager::Add(Tex);
  //  } /* End of 'AddImg' function */
  //
  //  /* Add new texture.
  //   * ARGUMENTS: 
  //   *   texture name:
  //   *    - const CHAR *TexName.
  //   *   tag:
  //   *    - INT tag.
  //   * RETURNS: (texture *) added texture.
  //   */
  //  texture *AddFromFile( const CHAR *TexName, INT tag )
  //  {
  //    texture *tex;
  //    texture Tex(TexName);
  //
  //    if ((tex = Find(TexName)) != nullptr)
  //      return tex;
  //
  //    Tex.Load(TexName, tag);
  //    return resource_manager::Add(Tex);
  //  } /* End of 'AddFromFile' function */
  //}; /* End of 'texture_manager' class */
}

#endif __texture_h_

/* END OF 'texture.h' FILE */