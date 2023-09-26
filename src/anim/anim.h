/* FILE NAME  : anim.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 01.08.2021
 * PURPOSE    : Animation handle functions.
 */

#ifndef __anim_h_
#define __anim_h_

#include "timer.h"
#include "input.h"
#include "../stock.h"
#include "render/render.h"
#include "render/render_prim.h"

namespace nicg
{
  class anim;

  class unit
  {
  public:
    virtual ~unit( VOID )
    {

    }

    /* Unit inter frame events handle function.
     * ARGUMENTS:
     *   - animation context:
     *      anim *Ani;
     * RETURNS: None.
     */
    virtual VOID Response( anim *Ani )
    {
    } /* End of 'Response' function */

    /* Unit render function.
     * ARGUMENTS:
     *   - animation context:
     *      anim *Ani;
     * RETURNS: None.
     */
    virtual VOID Render( anim *Ani )
    {
    } /* End of 'Response' function */
  }; /* End of 'unit' class */

  class anim : public win, public timer, public input, public render
  {
  private:

    anim( HINSTANCE hInst = GetModuleHandle(nullptr) ) : win(hInst), input(win::MouseWheel, win::hWnd), render(win::hWnd, W, H)
    {
      //CHAR Buf[_MAX_PATH];
      //GetCurrentDirectory(sizeof(Buf), Buf);
      //anim::Path = (std::string)Buf;
    }

    ~anim( VOID )
    {
      Units.Walk([](unit *Uni){delete Uni;});
    }

    stock<unit*> Units;
    static anim Instance; // Single tone

  public:
    //static std::string Path;

    VOID Timer( VOID ) override
    {
      timer::Response();

      CHAR Buf[300];

      sprintf_s(Buf, 300, "%.3f\n", FPS);
      //SetWindowText(win::hWnd, Buf);

      //CHAR Buf[30];
      
      //sprintf_s(Buf, 300, "CAMLOC: X: %.3f Y: %.3f Z: %.3f\n", Cam.Loc[0], Cam.Loc[1], Cam.Loc[2]);
      //sprintf_s(Buf, 300, "CAMAT: X: %.3f Y: %.3f Z: %.3f\n", Cam.At[0], Cam.At[1], Cam.At[2]);
      SetWindowText(win::hWnd, Buf);

      this->Render();
      render::CopyFrame();
    }

    VOID Idle( VOID ) override
    {
      timer::Response();

      this->Render();

      render::CopyFrame();
    }

    VOID Render( VOID )
    {
      timer::Response();
      input::Response();
      this->Start();

      Units.Walk([this](unit *Uni){Uni->Response(this);});

      Units.Walk([this](unit *Uni){Uni->Render(this);});

      //if ((INT)this->GlobalTime % 2 == 0)
      //  shader_manager::UpdateShd();


      render::End();
    }



    VOID Resize( VOID ) override
    {
      render::Resize(W, H);
      this->Render();
      render::CopyFrame();
    }

    static anim & Get( VOID )
    {
      return Instance;
    }

    /* Get instance pointer function.
     * ARGUMENTS: None.
     * RETURNS:
     *   (anim *) Instance pointer.
    */
    static anim * GetPtr( VOID )
    {
      return &Instance;
    }

    /* Initialization function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID Init( VOID ) override
    {
      //render::Start();
    } /* End of 'Init' function */

    anim & operator <<( unit *Uni )
    {
      Units << Uni;

      return *this;
    }

    VOID DestroyWnd( VOID )
    {
    }

    VOID Close( VOID ) override
    {
      render::End();
    }
  };
}

#endif /* __anim_h_ */

/* END OF 'anim.h' FILE */