/* FILE NAME  : resources.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 24.07.2021
 * PURPOSE    : Shader declaration module.
 */

#ifndef __shaders_h_
#define __shaders_h_

#include "../../../def.h"

#include <fstream>

#include "res.h"

namespace nicg
{

  class shader
  {
    friend class shader_manager;

  public:
    CHAR Name[300]; /* Shader filename prefix */
    INT ProgId;             /* Shader program id */

    shader( VOID ) : ProgId(0), Name("DEFAULT")
    {

    }

    shader( const CHAR *ShdName ) : ProgId(0)
    {
      strcpy(Name, ShdName);
    }

    CHAR * LoadTextFromFile( CHAR *FileName )
    {
      FILE *F;
      CHAR *txt;
      INT flen;

      /* Open file */
      if ((F = fopen(FileName, "rb")) == nullptr)
        return nullptr;

      /* Obtain file length */
      fseek(F, 0, SEEK_END);
      flen = ftell(F);
      rewind(F);

      /* Allocate memory for file text */
      if ((txt = new char[flen + 1]) == nullptr)
      {
        fclose(F);
        return nullptr;
      }

      /* Load text from file */
      memset(txt, 0, flen + 1);
      fread(txt, 1, flen, F);

      fclose(F);
      return txt;
    }

    /* Load shader program from .GLSL files function.
     * ARGUMENTS:
     *   - shader file path to load (path\***.glsl):
     *       CHAR *ShaderFileNamePrefix;
     * RETURNS:
     *   (INT) load shader program Id.
     */
    INT Load( const CHAR *ShaderFileNamePrefix )
    {
      struct
      {
        const CHAR *Name; /* Shader file prefix name (e.g. "VERT") */
        INT Type;   /* Shader OpenFL type (e.g. GL_VERTEX_SHADER) */
        INT Id;     /* Obtained shader Id from OpenGL */
      } shd[] =
      {
        {"VERT", GL_VERTEX_SHADER},
        {"FRAG", GL_FRAGMENT_SHADER},
      };

      //{"CTRL", GL_TESS_CONTROL_SHADER},
      //{"EVAL", GL_TESS_EVALUATION_SHADER},
      //{"GEOM", GL_GEOMETRY_SHADER},

      INT NoofS = sizeof(shd) / sizeof(shd[0]), i, res, prg = 0;
      CHAR *txt ;
      BOOL is_ok = TRUE;
      static CHAR Buf[1000];

      for (i = 0; i < NoofS; i++)
      {
        /* Build file name */
        sprintf(Buf, "BIN/SHADERS/%s/%s.GLSL", ShaderFileNamePrefix, shd[i].Name);
        /* Load shader text from file */
        txt = LoadTextFromFile(Buf);
        if (txt == nullptr)
        {
          if ((i == 0 || i == NoofS - 1))
          {
            Log(ShaderFileNamePrefix, shd[i].Name, "Error load file");
            is_ok = FALSE;
            break;
          }
          continue;
        }
        /* Create shader */
        if ((shd[i].Id = glCreateShader(shd[i].Type)) == 0)
        {
          if ((i == 0 || i == NoofS - 1))
          {
            Log(ShaderFileNamePrefix, shd[i].Name, "Error create shader");
            
            is_ok = FALSE;
            break;
          }
          continue;
        }
        /* Attach shader text to shader */
        glShaderSource(shd[i].Id, 1, &txt, NULL);
        delete[] txt;

        /* Compile shader */
        glCompileShader(shd[i].Id);
        glGetShaderiv(shd[i].Id, GL_COMPILE_STATUS, &res);
        if (res != 1)
        {
          glGetShaderInfoLog(shd[i].Id, sizeof(Buf), &res, Buf);
          Log(ShaderFileNamePrefix, shd[i].Name, Buf);
          is_ok = FALSE;
          break;
        }
      }

      /* Create shader program */
      if (is_ok)
      {
        if ((prg = glCreateProgram()) == 0)
        {
          Log(ShaderFileNamePrefix, "PROG", "Error create program");
          is_ok = FALSE;
        }
        else
        {
          /* Attach shaders to program */
          for (i = 0; i < NoofS; i++)
            if (shd[i].Id != 0)
              glAttachShader(prg, shd[i].Id);
          /* Link shader program */
          glLinkProgram(prg);
          glGetProgramiv(prg, GL_LINK_STATUS, &res);
          if (res != 1)
          {
            glGetProgramInfoLog(prg, sizeof(Buf), &res, Buf);
            Log(ShaderFileNamePrefix, "PROG", Buf);
            is_ok = FALSE;
          }
        }
      }

      if (!is_ok)
      {
        /* Delete all created shaders */
        for (i = 0; i < NoofS; i++)
          if (shd[i].Id != 0)
          {
            if (prg)
              glDetachShader(prg, shd[i].Id);
            glDeleteShader(shd[i].Id);
          }

        /* Delete program */
        if (prg)
          glDeleteProgram(prg);
        prg = 0;
      }

      this->ProgId = prg;
      return prg;
    } /* End of 'Load' function */

    VOID Free( VOID )
    {
      UINT shdrs[5];
      INT n;

      if (ProgId == 0)
        return;

      glGetAttachedShaders(ProgId, 5, &n, shdrs);
      for (INT i = 0; i < n; i++)
      {
        glDetachShader(ProgId, shdrs[i]);
        glDeleteShader(shdrs[i]);
      }
      glDeleteProgram(ProgId);
    }

    VOID Log( const CHAR *Prefix, const CHAR *Name, const CHAR *Error )
    {
      FILE *F;

      if ((F = fopen("BIN/SHADERS/SHD.LOG", "a")) == NULL)
        return;

      fprintf(F, "%s/%s.GLSL\n%s\n", Prefix, Name, Error);

      fclose(F);
    }

    /* Shader uniform value set function.
     * ARGUMENTS:
     *   - uniform name:
     *       const CHAR *Name;
     *   - uniform value:
     *       const value_type &Value;
     * RETURNS: None.
     */
    template<typename value_type>
        VOID UniSet( const CHAR *Name, const value_type &Value )
        {
          INT loc;
  
          if ((loc = glGetUniformLocation(ProgId, Name)) != -1)
            if constexpr (std::is_convertible_v<FLT, value_type>)
              glUniform1f(loc, Value);
            else if constexpr (std::is_convertible_v<INT, value_type>)
              glUniform1i(loc, Value);
            else if constexpr (std::is_convertible_v<vec3, value_type>)
              glUniform3fv(loc, 1, Value);
            else if constexpr (std::is_convertible_v<matr, value_type>)
              glUniformMatrix4fv(loc, 1, FALSE, Value);
        }

  }; /* End of 'shader' class */

  
  /* Shader manager class */
  class shader_manager : public resource_manager<shader, std::string>
  {
  public:

    ~shader_manager( VOID )
    {
      
    }
    /* Add shader to manager function.
     * ARGUMENTS:
     *   - shader name to add:
     *       const CHAR *ShaderName;
     * RETURNS: (shader *) created shader.
     */
    shader * Add( const CHAR *ShaderName )
    {
      shader *find = nullptr;
      shader shd(ShaderName);

      if ((find = Find(ShaderName)) != nullptr)
        return find;

      shd.Load(ShaderName);
      return resource_manager::Add(shd);
    } /* End of 'Add' function. */

    /* Initialize shaders function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Init( VOID )
    {
    } /* End of 'Init' function */

    /* Update shaders function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID UpdateShd( VOID )
    {
      for (auto &shd : Stock)
      {
        shd.second.Free();
        shd.second.Load(shd.second.Name);
      }
    } /* End of 'UpdateShaders' function. */

    /* Close manager function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Close( VOID )
    {
      resource_manager::Close();
      OutputDebugString("Shader manager destructed\n");
    } /* End of 'Close' function */
  }; /* End of 'shader_manager' class */

}

#endif /* __shaders_h_ */
/* END OF 'resources.h' FILE */
