/* FILE NAME  : render.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 26.07.2021
 * PURPOSE    : Render realisation.
 */

#ifndef __rnd_h_
#define __rnd_h_

#include "../../def.h"
#include "render_prim.h"
#include "res/shader.h"
#include "res/materials.h"
#include "res/texture.h"


/* My nsf namespace */
namespace nicg
{
  /* Render class */
  class render : public shader_manager, public material_manager, public texture_manager
  {
  private:
    HWND hWnd;                 /* Work window handle */
    HDC hDC;                   /* Work window memory device context  */
    INT FrameW, FrameH;        /* Work window size */
    HGLRC hGLRC;               /* OpenGL rendering context */
    material *mtl;

  public:
    mth::camera<FLT> Cam;

    render( HWND &hWnd1, INT &W, INT &H ) : Cam(), FrameW(W), FrameH(H)
    {
      INT i;
      UINT nums;
      PIXELFORMATDESCRIPTOR pfd = {0};
      HGLRC hRC;

      INT PixelAttribs[] =
      {
        WGL_DRAW_TO_WINDOW_ARB, TRUE,
        WGL_SUPPORT_OPENGL_ARB, TRUE,
        WGL_DOUBLE_BUFFER_ARB, TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 32,
        0
      };
      INT ContextAttribs[] =
      {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
                                      /* WGL_CONTEXT_CORE_PROFILE_BIT_ARB, */
        0
      };

      hWnd = hWnd1;
      hDC = GetDC(hWnd);

      /* OpenGL init: pixel format setup */
      pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
      pfd.nVersion = 1;
      pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
      pfd.cColorBits = 32;
      pfd.cDepthBits = 32;
      i = ChoosePixelFormat(hDC, &pfd);
      DescribePixelFormat(hDC, i, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
      SetPixelFormat(hDC, i, &pfd);

      /* OpenGL init: setup rendering context */
      hGLRC = wglCreateContext(hDC);
      wglMakeCurrent(hDC, hGLRC);

      /* Initializing GLEW library */
      if (glewInit() != GLEW_OK)
      {
        MessageBox(hWnd, "Error extensions initializing", "Error",
          MB_ICONERROR | MB_OK);
        exit(0);
      }

      if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
      {
        MessageBox(hWnd, "Error: no shaders support", "Error", MB_ICONERROR | MB_OK);
        exit(0);
      }

      /* Enable a new OpenGL profile support */
      wglChoosePixelFormatARB(hDC, PixelAttribs, NULL, 1, &i, &nums);
      hRC = wglCreateContextAttribsARB(hDC, NULL, ContextAttribs);

      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(hGLRC);

      hGLRC = hRC;
      wglMakeCurrent(hDC, hGLRC);
      /* Set default OpenGL parameters */
      glEnable(GL_DEPTH_TEST);

      wglSwapIntervalEXT(0);

      //shader_manager::Add("DEFAULT");
      mtl = material_manager::GetDefault();
      mtl->Tex[0] = texture_manager::Add("BIN/TEXTURES/GIR.g32", 4);
    }

    ~render( VOID )
    {
      texture_manager::Close();
      material_manager::Close();
      shader_manager::Close();
      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(hGLRC);
      ReleaseDC(hWnd, hDC);
      OutputDebugString("Render destructed\n");
    }

    VOID Resize( INT W, INT H )
    {

      /* resize frame compomemts */
      FrameW = W;
      FrameH = H;

      glViewport(0, 0, FrameW, FrameH);
      Cam.Resize(FrameW, FrameH);

      /* recalculate projection */
      //Cam.SetProj();
    }

    VOID Start( VOID )
    {
      /* Clear frame */
      glClearColor(0.3, 0.47, 0.8, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    VOID End( VOID )
    {
      //shader_manager::DeleteShd();
      glFinish();
    }

    VOID CopyFrame( VOID )
    {
      wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
    }

    /* Draw render primitive function.
     * ARGUMENTS:
     *   - matrix:
     *      matr World;
     * RETURNS: None.
     */
    VOID PrimDraw( prim *Pr, matr World )
    {
      matr
        w = World,
        wvp = w * Cam.VP;
      INT gl_prim_type = Pr->Type == prim_type::PRIM_LINES ? GL_LINES :
                         Pr->Type == prim_type::PRIM_TRIMESH ? GL_TRIANGLES :
                         Pr->Type == prim_type::PRIM_TRISTRIP ? GL_TRIANGLE_STRIP :
                         Pr->Type == prim_type::PRIM_PATH ? GL_PATCHES :
                         GL_POINTS;
      shader *shd = Pr->Mtl->Shd;
      //shader *shd = shader_manager::Find("DEFAULT");

      glEnable(GL_PRIMITIVE_RESTART);
      glPrimitiveRestartIndex(-1);
      
      glUseProgram(shd->ProgId);
      Pr->Mtl->Apply(shd);
      shd->UniSet("MatrWVP", wvp);
      shd->UniSet("CamDir", &Cam.Dir[0]);
      shd->UniSet("CamLoc",  &Cam.Loc[0]);

      /* making an array of vertices active */
      glBindVertexArray(Pr->VA);
      if (gl_prim_type == GL_PATCHES)
      {
        glPatchParameteri(GL_PATCH_VERTICES, 16);
        glDrawArrays(gl_prim_type, 0, Pr->NumOfEl);
      }
      else if (Pr->IBuf == 0)
        glDrawArrays(gl_prim_type, 0, Pr->NumOfEl);
      else
      {
        /* making an array of indexes active */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Pr->IBuf);
        /* drawing */
        glDrawElements(gl_prim_type, Pr->NumOfEl, GL_UNSIGNED_INT, NULL);
        /* disable index array */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      }
      /* disable vertex array */
      glBindVertexArray(0);
    } /* End of 'Draw' function */


    /* Model draw func */
    VOID ModelDraw(model *mdl, matr World)
    {
      auto walker = [this, World]
        (prim *Pr)
      {
        PrimDraw(Pr, World);
      };

      mdl->WalkAll(walker);
    }

    /* Model draw func */
    VOID ModelDraw2(model *mdl, matr World)
    {
      auto walker = [this, World]
        (prim *Pr)
      {
        PrimDraw(Pr, World);
      };

      mdl->WalkAll2(walker);
    }
  }; /* End of 'render' class */
} /* End of 'nicg' namespace */

#endif /* __rnd_h_ */
/* END OF 'render.h' FILE */