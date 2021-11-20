/* Includes for xplot */

/* This retrieves string defns from X library */
#define XTSTRINGDEFINES

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sgtty.h>
#include <fcntl.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>


Display *dpy;
Window widgetwin;
Widget toplevel;
Pixmap pixmap;
GC gc;
Pixel fg, bg;
XtInputId inid;
char *progname;

FILE *instrm;

int xmin, xmax, ymin, ymax, pxmax, pymax;

float xscale, yscale, xdscale, ydscale;
int plotmode;
int curfat, needmove;
int fakecolor;
int fatbase, vertical;
int plotted, needmove;
int xold, yold, xnew, ynew;

#define DRAW_PATH		1
#define FILL_PATH		2
#define TILE_PATH		3
