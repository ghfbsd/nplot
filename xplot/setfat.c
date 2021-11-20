#include <stdio.h>
#include "xplot.h"

/* set the line thickness to newfat */
int setfat(newfat)
    int newfat;
{
    if (newfat < 0) {
	faterr(newfat, 0);
	newfat = 0;
    } else if (newfat > 20) {
	faterr(newfat,20);
	newfat = 20;
    }
    drawpath();		/* draw the current path first */

    newfat = newfat > fatlim ? fatlim : newfat; /* Limit to fatlim value */

    XSetLineAttributes(dpy,gc,(unsigned int)newfat,LineSolid,CapButt,JoinBevel);

    curfat = newfat;
    /*
     * KLUDGE: We really should save the last point on the path
     * before we draw it, and then put it back on the new path.
     * This would be too hard, so we'll cheat
     */
    needmove = 1;
}

faterr(bad, good)
    int bad, good;
{
    fprintf(stderr, 
       "%s: line thickness of %d out of range; set to %d\n", progname, bad, good);
}
