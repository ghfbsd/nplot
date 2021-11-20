/* Includes for xplot */

/* This retrieves string defns from X library */
#define XTSTRINGDEFINES

#include <stdio.h>
#include <math.h>
#include <string.h>
#if !defined(__linux__)
#include <sgtty.h>
#else
#include <termios.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>


extern Display *dpy;
extern Window widgetwin;
extern Widget toplevel;
extern Pixmap pixmap;
extern GC gc;
extern Pixel fg, bg;
extern XtInputId inid;
extern char *progname;

extern FILE *instrm;

extern int xmin, xmax, ymin, ymax, pxmax, pymax;

extern float xscale, yscale, xdscale, ydscale;
extern int plotmode;
extern int curfat, needmove;
extern int fakecolor;
extern int fatbase, fatlim, vertical;
extern int plotted, needmove;
extern int xold, yold, xnew, ynew;

void dump(int, int, int, int);

#define DRAW_PATH		1
#define FILL_PATH		2
#define TILE_PATH		3
