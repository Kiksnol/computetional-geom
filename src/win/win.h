/* FILE NAME  : win.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 24.07.2021
 * PURPOSE    : WinAPI preferences
 */

#ifndef __win_h_
#define __win_h_

#include "../mth/mth.h"

#define INIT_TIMER 30
#define REFRESHED_TIMER 30

namespace nicg
{
  class win
  {
  protected:
    HWND hWnd;           // window handle
    HINSTANCE hInstance; // application handle
    INT MouseWheel;

  private:
    BOOL IsFullScreen;
    RECT FullScreenSaveRect;
    BOOL IsInit;

    static LRESULT CALLBACK WinFunc( HWND hWnd, UINT Msg,
                                     WPARAM wParam, LPARAM lParam )
    {
      win *Win = nullptr;
      HDC hDC;
      PAINTSTRUCT ps;

      switch (Msg)
      {
      case WM_GETMINMAXINFO:
        Win->OnGetMinMaxInfo((MINMAXINFO *)lParam);
        return 0;
      case WM_CREATE:
        SetWindowLongPtr(hWnd, 0, (UINT_PTR)((CREATESTRUCT *)lParam)->lpCreateParams);
      default:
        Win = reinterpret_cast<win *>(GetWindowLongPtr(hWnd, 0));
        if (Win != nullptr)
          switch (Msg)
          {
          case WM_CREATE:
            Win->hWnd = hWnd;
            return Win->OnCreate((CREATESTRUCT *)lParam) ? 0 : -1;
          case WM_SIZE:
            Win->OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
            //SendMessage(hWnd, WM_TIMER, 0, 0);
            return 0;
          case WM_ERASEBKGND:
            return (LRESULT)Win->OnEraseBackground((HDC)wParam);
          case WM_PAINT:
            hDC = BeginPaint(hWnd, &ps);
            Win->OnPaint(hDC, &ps);
            EndPaint(hWnd, &ps);
            return 0;
          case WM_TIMER:
            Win->OnTimer((UINT)wParam);
            return 0;
          case WM_MOUSEWHEEL:
            Win->OnMouseWheel((INT)(SHORT)LOWORD(lParam),
                              (INT)(SHORT)HIWORD(lParam),
                              (INT)(SHORT)HIWORD(wParam),
                              (UINT)(SHORT)LOWORD(wParam));
            return 0;
          case WM_RBUTTONDOWN:
            Win->OnRButtonDown(FALSE,
                              (INT)(SHORT)LOWORD(lParam),
                              (INT)(SHORT)HIWORD(lParam),
                              (UINT)(SHORT)LOWORD(wParam));
            return 0;
          case WM_MBUTTONDOWN:
            Win->OnMButtonDown(FALSE,
                              (INT)(SHORT)LOWORD(lParam),
                              (INT)(SHORT)HIWORD(lParam),
                              (UINT)(SHORT)LOWORD(wParam));
            return 0;
          case WM_LBUTTONDOWN:
            Win->OnLButtonDown(FALSE,
                              (INT)(SHORT)LOWORD(lParam),
                              (INT)(SHORT)HIWORD(lParam),
                              (UINT)(SHORT)LOWORD(wParam));
            return 0;
          case WM_RBUTTONUP:
            Win->OnRButtonUp((INT)(SHORT)LOWORD(lParam),
                            (INT)(SHORT)HIWORD(lParam),
                            (UINT)(SHORT)LOWORD(wParam));
            return 0;
          case WM_MBUTTONUP:
            Win->OnMButtonUp((INT)(SHORT)LOWORD(lParam),
                            (INT)(SHORT)HIWORD(lParam),
                            (UINT)(SHORT)LOWORD(wParam));
            return 0;
          case WM_LBUTTONUP:
            Win->OnLButtonUp((INT)(SHORT)LOWORD(lParam),
                            (INT)(SHORT)HIWORD(lParam),
                            (UINT)(SHORT)LOWORD(wParam));
            return 0;
          case WM_KEYDOWN:
            Win->OnKeyDown(wParam);
            return 0;
          case WM_CLOSE:
            Win->OnClose();
            return 0;
          case WM_DESTROY:
            Win->OnDestroy();
            return 0;
          }
      }
      return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

  public:
    BOOL IsActive;
    INT W, H;            // window size

    win( HINSTANCE hInst = GetModuleHandle(nullptr) ) : hInstance(hInst), MouseWheel(0), 
      IsFullScreen(FALSE), W(0), H(0), IsInit(FALSE),
      hWnd((HWND)0), FullScreenSaveRect(), IsActive(TRUE)
    {
      WNDCLASS wc;

      wc.style = CS_VREDRAW | CS_HREDRAW;

      wc.cbClsExtra = 0; /*  */
      wc.cbWndExtra = sizeof(win *); /*  */
      wc.hbrBackground = (HBRUSH)COLOR_WINDOW; /* Background color */
      wc.hCursor = LoadCursor(nullptr, IDC_CROSS); /* Cursor Load */
      wc.hIcon = LoadIcon(nullptr, IDI_EXCLAMATION); /* Icon Load */
      wc.hInstance = hInstance; /* Window register class description */
      wc.lpfnWndProc = WinFunc; /**/
      wc.lpszClassName = "T05ANIM ND4 proj"; /* Window class name */
      wc.lpszMenuName = nullptr;

      /* Class registration */
      if (!RegisterClass(&wc))
      {
          MessageBox(NULL, "Error register window class", ERROR, MB_OK);
          return;
      }

      hWnd = CreateWindow("T05ANIM ND4 proj",
        "ND4 1st proj", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL,
        hInstance, reinterpret_cast<VOID *>(this));

      //SendMessage(hWnd, WM_CREATE, 0, 0);

      /* Show and redraw window */
      ShowWindow(hWnd, SW_SHOWNORMAL);
      UpdateWindow(hWnd);
    }

    ~win( VOID )
    {

    }

    VOID Run( VOID )
    {
      MSG msg;

      /* Message loop */
      while (TRUE)
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
          if (msg.message == WM_QUIT)
            break;
          DispatchMessage(&msg);
        }
        else
          Idle();
    }

    VOID FlipFullScreen( VOID )
    {
      if (!IsFullScreen)
      {
        HMONITOR hMon;
        MONITORINFOEX moninfo;
        RECT rc;

        IsFullScreen = TRUE;

        /* Save old window size and position */
        GetWindowRect(hWnd, &FullScreenSaveRect);

        /* Get closest monitor */
        hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        /* Get monitor information */
        moninfo.cbSize = sizeof(moninfo);
        GetMonitorInfo(hMon, (MONITORINFO *)&moninfo);

        rc = moninfo.rcMonitor;
        AdjustWindowRect(&rc, GetWindowLong(hWnd, GWL_STYLE), FALSE);

        /* Expand window to full screen */
        SetWindowPos(hWnd, HWND_TOP,
          rc.left, rc.top,
          rc.right - rc.left, rc.bottom - rc.top,
          SWP_NOOWNERZORDER);
      }
      else
      {
        IsFullScreen = FALSE;

        /* Restore window size and position */
        SetWindowPos(hWnd, HWND_NOTOPMOST,
          FullScreenSaveRect.left, FullScreenSaveRect.top,
          FullScreenSaveRect.right - FullScreenSaveRect.left, FullScreenSaveRect.bottom - FullScreenSaveRect.top,
          SWP_NOOWNERZORDER);
      }
    }
  private:
    // Message cracks


    VOID OnGetMinMaxInfo( MINMAXINFO *MinMax );

    BOOL OnCreate( CREATESTRUCT *CS );

    /* WM_SIZE window message handle function.
     * ARGUMENTS:
     *   - sizing flag (see SIZE_***, like SIZE_MAXIMIZED)
     *       UINT State;
     *   - new width and height of client area:
     *       INT W, H;
     * RETURNS: None.
     */
    VOID OnSize( UINT State, INT W, INT H );

    /* WM_TIMER window message handle function.
     * ARGUMENTS:
     *   - specified the timer identifier.
     *       INT Id;
     * RETURNS: None.
     */
    VOID OnTimer( INT Id );

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
    VOID OnMouseWheel( INT X, INT Y, INT Z, UINT Keys );

    VOID OnRButtonDown( BOOL IsDblClick, INT X, INT Y, UINT Keys );
     
    VOID OnRButtonUp( INT X, INT Y, UINT Keys );

    VOID OnMButtonDown( BOOL IsDblClick, INT X, INT Y, UINT Keys );

    VOID OnMButtonUp( INT X, INT Y, UINT Keys );

    VOID OnLButtonDown( BOOL IsDblClick, INT X, INT Y, UINT Keys );

    VOID OnLButtonUp( INT X, INT Y, UINT Keys );

    /* WM_ERASEBKGND window message handle function.
     * ARGUMENTS:
     *   - device context of client area:
     *       HDC hDC;
     * RETURNS:
     *   (BOOL) TRUE if background is erased, FALSE otherwise.
     */
    BOOL OnEraseBackground( HDC hDC );

    /* WM_PAINT window message handle function.
     * ARGUMENTS:
     *   - window device context:
     *       HDC hDC;
     *   - paint message structure pointer:
     *       PAINTSTRUCT *PS;
     * RETURNS: None.
     */
    VOID OnPaint( HDC hDC, PAINTSTRUCT *PS );

    VOID OnClose( VOID );

    VOID OnKeyDown( UINT Key );

    /* WM_DESTROY window message handle function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID OnDestroy( VOID );

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
    VOID OnActivate( UINT Reason, HWND hWndActDeact, BOOL IsMinimized );

  private:
    /* Initialization function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    virtual VOID Init( VOID )
    {
    } /* End of 'Init' function */

    virtual VOID Close( VOID )
    {

    }

    virtual VOID Resize( VOID )
    {
    }

    virtual VOID Timer( VOID )
    {
    }

    virtual VOID Activate( VOID )
    {

    }

    virtual VOID Idle( VOID )
    {

    }

    virtual VOID Erase( HDC hDC )
    {

    }

    virtual VOID Paint( HDC hDC )
    {
    }
  };
}
#endif /* __win_h_ */

/* END OF 'win.h' FILE */