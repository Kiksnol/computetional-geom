/* FILE NAME  : input.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 24.07.2021
 * PURPOSE    : Timer handle functions.
 */

#ifndef __input_h_
#define __input_h_

#include "../def.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm")

#define ND4_GET_JOYSTIC_AXIS(A) \
   (2.0 * (ji.dw ## A ## pos - jc.w ## A ## min) / (jc.w ## A ## max - jc.w ## A ## min) - 1)


namespace nicg
{
  class keyboard
  {
  private:
    BYTE KeysOld[256];

  public:
    BYTE KeysClick[256];
    BYTE Keys[256];

    keyboard( VOID )
    {
      ZeroMemory(Keys, 256);
      ZeroMemory(KeysClick, 256);
      ZeroMemory(KeysOld, 256);
    }

    /* Keyboyard response function.
     * ARGUMENTS: None.
     * RETURNS: None.
    */
    VOID Response( VOID )
    {
      (VOID)GetKeyboardState(Keys);
        for (INT i = 0; i < 256; i++)
        {
          Keys[i] >>= 7;
          KeysClick[i] = Keys[i] && !KeysOld[i];
        }
        memcpy(KeysOld, Keys, 256);
    } /* End of 'Response' function */
  }; /* End of 'keyboard' class */

  class joystick
  {
  protected:
    DBL
      JX,
      JY,
      JZ,
      JR;
    BYTE
      JBut[32], 
      JButClick[32],
      JButOld[32]; 


  public:
    joystick( VOID ) : JX(0), JY(0), JZ(0), JR(0)
    {
      ZeroMemory(JBut, 32);
      ZeroMemory(JButOld, 32);
      ZeroMemory(JButClick, 32);
    }

    /* Joystick response function.
     * ARGUMENTS: None.
     * RETURNS: None.
    */
    VOID Response( VOID )
    {
      INT i;

      if (joyGetNumDevs() > 0)
      {
        JOYCAPS jc;

        /* get joysick info */
        if (joyGetDevCaps(JOYSTICKID1, &jc, sizeof(jc)) == JOYERR_NOERROR)
        {
          JOYINFOEX ji;

          ji.dwSize = sizeof(JOYINFOEX);
          ji.dwFlags = JOY_RETURNALL;
          if (joyGetPosEx(JOYSTICKID1, &ji) == JOYERR_NOERROR)
          {
            /* buttons */
            for (i = 0; i < 32; i++)
            {
              JBut[i] = (ji.dwButtons >> i) & 1;
              JButClick[i] = JBut[i] && !JButOld[i];
              JButOld[i] = JBut[i];
            }



            /* axes */
            JX = ND4_GET_JOYSTIC_AXIS(X);
            JY = ND4_GET_JOYSTIC_AXIS(Y);
            JZ = ND4_GET_JOYSTIC_AXIS(Z);
            JR = ND4_GET_JOYSTIC_AXIS(R);
          }
        }
      }
    } /* End of 'Response' function */
  }; /* End of 'joystick' class */

  class mouse
  {
  private:
    HWND hWndM;
  public:
    INT &Wheel, Mdz, Mdx, Mdy, Mz, Mx, My;

    mouse( INT &WheelRef, HWND hWnd ) : Wheel(WheelRef), Mdz(0), Mdx(0), Mdy(0), Mz(0), Mx(0), My(0), hWndM(hWnd)
    {

    }

    /* Mouse response function.
     * ARGUMENTS: None.
     * RETURNS: None.
    */
    VOID Response( VOID )
    {
      POINT pt;

      GetCursorPos(&pt);
      ScreenToClient(hWndM, &pt);

      /* transform coordinates on the screen */
      Mdx = pt.x - Mx;
      Mdy = pt.y - My;
      Mdz = Wheel;
      /* absolute val */
      Mx = pt.x;
      My = pt.y;
      Mz += Wheel;

      Wheel = 0;
    } /* End of 'Response' function */
  }; /* End of 'mouse' class */

  class input : public keyboard, public mouse, public joystick
  {
  public:
    input( INT &WheelRef, HWND hWnd ) : mouse(WheelRef, hWnd)
    {
    }

    /* Input response function.
     * ARGUMENTS: None.
     * RETURNS: None.
    */
    VOID Response( VOID )
    {
      keyboard::Response();
      mouse::Response();
      joystick::Response();
    } /* End of 'Response' function */

  }; /* End of 'input' class */
}

#endif /* __input_h_ */

/* END OF 'input.h' FILE */