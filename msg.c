#include <string.h> /* strlen */
#include <graph.h>

#include "vga.h" /* XRES and YRES */

void
print_title(const char *title)
{
  /* use graphics mode to center and print a title line */

  _setcolor(1); /* blue */
  _rectangle(_GFILLINTERIOR, 0,0, XRES-1,10); /* 10px high, in 640x480 */

  /* default font is 8x8 */
  _moveto((XRES/2)-(8 * strlen(title)/2), 2); /* centered x,y */
  _setcolor(7); /* white */
  _outgtext(title);
}

void
print_message(const char *msg)
{
  /* use graphics mode to print a message line */

  _setcolor(2); /* green */

  /* 10px high, in 640x480 */
  _rectangle(_GFILLINTERIOR, 0,YRES-11, XRES-1,YRES-1);

  /* default font is 8x8 */
  _moveto(1, YRES-9); /* x,y */
  _setcolor(15); /* br white */
  _outgtext(msg);
}

void
print_error(const char *error)
{
  /* use graphics mode to print an error message */

  _setcolor(4); /* red */

  /* 10px high, in 640x480 */
  _rectangle(_GFILLINTERIOR, 0,YRES-11, XRES-1,XRES-1);

  /* default font is 8x8 */
  _moveto(1, YRES-9); /* x,y */
  _setcolor(14); /* br yellow */
  _outgtext(error);
}
