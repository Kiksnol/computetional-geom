/* FILE NAME  : winmsg.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 24.07.2021
 * PURPOSE    : 
 */

#include "win.h"



namespace nicg
{
  VOID win::OnGetMinMaxInfo( MINMAXINFO *MinMax )
  {
    MinMax->ptMaxTrackSize.y = GetSystemMetrics(SM_CYMAXTRACK) +
      GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER) * 2;
  }

  BOOL win::OnCreate( CREATESTRUCT *CS )
  {
    SetTimer(this->hWnd, INIT_TIMER, 1, nullptr);
    return TRUE;
  }

  /* WM_SIZE window message handle function.
   * ARGUMENTS:
   *   - sizing flag (see SIZE_***, like SIZE_MAXIMIZED)
   *       UINT State;
   *   - new width and height of client area:
   *       INT W, H;
   * RETURNS: None.
   */
  VOID win::OnSize( UINT State, INT W, INT H )
  {
    this->W = W;
    this->H = H;
    if (IsInit)
    {
      Resize();
    }
  }

  /* WM_TIMER window message handle function.
   * ARGUMENTS:
   *   - specified the timer identifier.
   *       INT Id;
   * RETURNS: None.
   */
  VOID win::OnTimer( INT Id )
  {
    if (!IsInit)
      {
        KillTimer(hWnd, INIT_TIMER);
        SetTimer(hWnd, REFRESHED_TIMER, 1, nullptr);
        IsInit = TRUE;
        // call callbacks
        Init();
        Resize();
      }

    Timer();
  }

  /* WM_MOUSEWHEEL window message handle function.
   * ARGUMENTS:
   *   - mouse window position:
   *       INT X, Y;
   *   - mouse wheel relative delta value:
   *       INT Z;
   *   - mouse keys bits (see MK_*** bits constants):
   *       UINT Keys;
   * RETURNS: None.
   */
  VOID win::OnMouseWheel( INT X, INT Y, INT Z, UINT Keys )
  {
    MouseWheel += Z;
  }

  VOID win::OnRButtonDown( BOOL IsDblClick, INT X, INT Y, UINT Keys )
  {
   
  }
   
  VOID win::OnRButtonUp( INT X, INT Y, UINT Keys )
  {

  }

  VOID win::OnMButtonDown( BOOL IsDblClick, INT X, INT Y, UINT Keys )
  {

  }

  VOID win::OnMButtonUp( INT X, INT Y, UINT Keys )
  {

  }

  VOID win::OnLButtonDown( BOOL IsDblClick, INT X, INT Y, UINT Keys )
  {

  }

  VOID win::OnLButtonUp( INT X, INT Y, UINT Keys )
  {

  }

  /* WM_ERASEBKGND window message handle function.
   * ARGUMENTS:
   *   - device context of client area:
   *       HDC hDC;
   * RETURNS:
   *   (BOOL) TRUE if background is erased, FALSE otherwise.
   */
  BOOL win::OnEraseBackground( HDC hDC )
  {
    if (IsInit)
        Erase(hDC);

    return TRUE;
  }

  /* WM_PAINT window message handle function.
   * ARGUMENTS:
   *   - window device context:
   *       HDC hDC;
   *   - paint message structure pointer:
   *       PAINTSTRUCT *PS;
   * RETURNS: None.
   */
  VOID win::OnPaint( HDC hDC, PAINTSTRUCT *PS )
  {
    if (IsInit)
        Paint(hDC);
  }

  VOID win::OnClose( VOID )
  {
    if (IsInit)
    {
      if (MessageBox(hWnd, "Are you sure to exit?", "Exit from game", MB_YESNO) == IDYES)
        win::OnDestroy();
    }
  }

  /* WM_DESTROY window message handle function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID win::OnDestroy( VOID )
  {
    if (IsInit)
      {
        Close();
        KillTimer(hWnd, REFRESHED_TIMER);
        IsInit = FALSE;
      }
      else
        KillTimer(hWnd, INIT_TIMER);

    PostQuitMessage(30);
  }

  VOID win::OnKeyDown( UINT Key )
  {
    if (Key == 27)
      win::OnDestroy();
  }

  /* WM_ACTIVATE window message handle function.
   * ARGUMENTS:
   *   - reason (WA_CLICKACTIVE, WA_ACTIVE or WA_INACTIVE):
   *       UINT Reason;
   *   - handle of active window:
   *       HWND hWndActDeact;
   *   - minimized flag:
   *       BOOL IsMinimized;
   * RETURNS: None.
   */
  VOID win::OnActivate( UINT Reason, HWND hWndActDeact, BOOL IsMinimized )
  {
    IsActive = Reason == WA_CLICKACTIVE || Reason == WA_ACTIVE;

    if (IsInit)
        Activate();

    //if (IsInit)
    //    Activate();
  } /* End of 'win::OnActivate' function */
}

/* END OF 'winmsg.h' FILE */