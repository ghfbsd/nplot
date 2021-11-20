#include <stdio.h>
#include "xplot.h"

#define MAXPATHLEN	4096

static int pathop=DRAW_PATH;

static XPoint coords[MAXPATHLEN];
static int nc=0;

/*  putpoint
 *  Add a point to the current path, drawing it if it gets too big.
 */
putpoint(x, y)
	int x, y;
{
    y = pymax - y;	/* Damn screen is upside down */

    if (nc == MAXPATHLEN) {
	/*
	 * We've got as many points stored as we can,
	 * so draw the path, and make the last point the first point
	 * of the new one so it's continuous
	 */
	drawpath();
	coords[0].x = coords[MAXPATHLEN-1].x;
	coords[0].y = coords[MAXPATHLEN-1].y;
	nc = 1;
    }

    /*
     * Now save the new point at the end of the current path.
     */
    coords[nc].x = x;
    coords[nc].y = y;
    nc++;
}

/* drawpath
 * Draw the current path
 */
drawpath()
{

    if (nc > 0) {
	if (pathop == DRAW_PATH) 
	    XDrawLines(dpy,pixmap,gc,coords,nc,CoordModeOrigin);
	else
	    XFillPolygon(dpy,pixmap,gc,coords,nc,Complex,CoordModeOrigin);

	nc = 0;
    }
}

/*
 * Set the current path operation (e.g. DRAW or FILL).
 * Return the old operation.
 */
int setpathop(newop)
    int newop;
{
    int saveop = pathop;
    if (newop != pathop) {
	/* Both draw and fill use FillSolid for color drawing and filling */
	drawpath();
	pathop = newop;
        XSetFillStyle(dpy,gc,newop!=TILE_PATH?FillSolid:FillTiled);
    }
    return(saveop);
}
