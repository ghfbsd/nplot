#include <stdio.h>
#include "apen.h"

int forcegray = 0;	/* Do they want black&white only */

int vertical = 0;	/* Do they want a vertical plot */

int embedded = 0;	/* Do they want the plot embedded in another
			   IMPRESS document? */

int pxmax = YSIZE;	/* maximum physical size of plot */
int pymax = XSIZE;

int xmax  = YSIZE;	/* plot window parameters defaulted */
int xmin  = 0;		/* to maximum physical size */
int ymax  = XSIZE;
int ymin  = 0;

int curfat;		/* line width */
int plotted=0;
int needmove=1;
int pageno=0;		/* current page number */
int xnew,ynew;		/* new pen location */
int xold,yold;		/* old pen location */
float xdscale =	1;	/* global default x-scale */
float ydscale =	1;	/* global default y-scale */
float xscale =	1;	/* global x-scale */
float yscale =	1;	/* global y-scale */
#if 0
float xdscale =	72.0/300;	/* global default x-scale */
float ydscale =	72.0/300;	/* global default y-scale */
float xscale =	72.0/300;	/* global x-scale */
float yscale =	72.0/300;	/* global y-scale */
#endif
