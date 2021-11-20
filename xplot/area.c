#include <stdio.h>
#include "xplot.h"
#include "pat.h"

area()
{
    int xfirst, yfirst, x1, y1;
    int npts, shade, saveop;
    register int i;

    /*
     * We want to start a new path.  Draw the old one if needed
     */
    drawpath();

    shade = vgeth(stdin);
    if (!shade) {
	/* Outline the polygon if both masks are zero */
	saveop = setpathop(DRAW_PATH);
    } else {
	int op = setshad(shade);
	saveop = setpathop(op);
    }
    npts=  vgeth(stdin);
    xfirst = vgeth(stdin)*xscale;
    yfirst = vgeth(stdin)*yscale;
    putpoint(xfirst, yfirst);
    for(i=0; i<npts; i++) {
        x1= vgeth(stdin)*xscale;
        y1= vgeth(stdin)*yscale;
	putpoint(x1, y1);
    }
    drawpath();		/* Force the path to be drawn */

    (void)setpathop(saveop);	/* reset path operation */

    if( (xfirst!=x1) && (yfirst!=y1) ) {
        fprintf(stderr,"%s: warning: polygon does not close\n",progname);
    }
}
