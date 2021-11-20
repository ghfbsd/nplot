#include <stdio.h>
#include "xplot.h"

Display *dpy;
Window widgetwin;
Widget toplevel;
Pixmap pixmap;
GC gc;
Pixel fg, bg;
XtInputId inid;

int pxmax;		/* maximum physical size of plot */
int pymax;

int xmax;		/* plot window parameters defaulted */
int xmin;		/* to maximum physical size */
int ymax;
int ymin;

int fatbase, curfat, fatlim=2;	/* line width */
int plotted=0;
int needmove=1;

int xnew,ynew;			/* new pen location */
int xold,yold;			/* old pen location */

float xdscale =	83.0/300;	/* global default x-scale */
float ydscale =	83.0/300;	/* global default y-scale */
float xscale =	83.0/300;	/* global x-scale */
float yscale =	83.0/300;	/* global y-scale */

FILE *instrm;		/* Input stream */

char *progname;		/* Program name */
