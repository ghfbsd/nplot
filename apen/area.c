#include <stdio.h>
#include "apen.h"
#include "pat.h"
short vgeth();

extern float xscale, yscale;
extern char *progname; 

void area(color)
    int color;
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
	/* Fill it otherwise */
	puts("GS");
	setshad(color,shade);
	saveop = setpathop(FILL_PATH);
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

    if (shade)
	puts("GR");

    (void)setpathop(saveop);	/* reset path operation */

    if( (xfirst!=x1) && (yfirst!=y1) ) {
        fprintf(stderr,"%s: warning: polygon does not close\n",
	   progname);
    }
}
