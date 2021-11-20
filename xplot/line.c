#include <stdio.h>
#include "xplot.h"

line()	/* draw a line */
{
    register int i;
    int  npts, x, y, xold, yold;

    npts = vgeth(stdin);

    xold = vgeth(stdin);
    yold = vgeth(stdin);
    for (i=1; i<npts; i++) {
	x = vgeth(stdin);
	y = vgeth(stdin);
	dump(xold, yold, x, y);
	xold = x;
	yold = y;
    }
}
