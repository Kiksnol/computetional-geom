/* FILE NAME  : main.cpp
 * PROGRAMMER : ND4
 * LAST UPDATE: 31.07.2021
 * PURPOSE    : Main project file.
 */

#include "../src/units/units.h"

#pragma warning(disable: 28251)

/* The main program function.
 * ARGUMENTS:
 *   - handle of application instance:
 *       HINSTANCE hInstance;
 *   - dummy handle of previous application instance (not used):
 *       HINSTANCE hPrevInstance;
 *   - command line string:
 *       CHAR *CmdLine;
 *   - show window command parameter (see SW_***):
 *       INT CmdShow;
 * RETURNS:
 *   (INT) Error level for operation system (0 for success).
 */
INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, CHAR *CmdLine, INT CmdShow )
{
  nicg::anim *myw = nicg::anim::GetPtr();

  *myw << new nicg::units::ctrl;
  *myw << new nicg::units::plane;

  myw->Run();
} /* End of 'WinMain' function */

/* END OF 'main.cpp' FILE */