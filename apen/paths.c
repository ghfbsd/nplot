#include <stdio.h>
#include <math.h>
#include "apen.h"

static int pathlen=0, pathop=DRAW_PATH;
static short pathx[MAXPATHLEN], pathy[MAXPATHLEN];
int llx, lly, urx, ury, llfst = 1;
float ox = 0.0, oy = 0.0, sa = 1.0, sb = 0.0, sc = 0.0, sd = 1.0;
int plotmode=3;		/* default to `shade over' mode */

void
pusht(state)
   float state[6];
{
   state[0] = ox;
   state[1] = oy;
   state[2] = sa;
   state[3] = sb;
   state[4] = sc;
   state[5] = sd;
}

void
popt(state)
   float state[6];
{
   ox = state[0];
   oy = state[1];
   sa = state[2];
   sb = state[3];
   sc = state[4];
   sd = state[5];
}

/* Coordinate rotation - angle in degrees */
void
rot(theta)
   float theta;
{
   float nsa, nsb, nsc, nsd;
   double rho = theta*4.0*atan(1.0)/180.0;
   nsa = cos(rho)*sa + sin(rho)*sc;
   nsb = cos(rho)*sb + sin(rho)*sd;
   nsc = cos(rho)*sc - sin(rho)*sa;
   nsd = cos(rho)*sd - sin(rho)*sb;

   sa = nsa; sb = nsb; sc = nsc; sd = nsd;
}

/* Coordinate translation  */
void
tran(x,y)
   float x,y;
{
   float nox, noy;
   nox = x*sa + y*sc + ox;
   noy = x*sb + y*sd + oy;

   ox = nox; oy = noy;
}

/* Coordinate scaling  */
void
scal(x,y)
   float x,y;
{
   float nsa, nsb, nsc, nsd;
   nsa = x*sa;
   nsb = x*sb;
   nsc = y*sc;
   nsd = y*sd;

   sa = nsa; sb = nsb; sc = nsc; sd = nsd;
}

/* Calculate bounding box, accounting for coordiate transformations */
void
bbox(x,y)
    int x, y;
{
    int lx = (int)(ox + sa*x + sc*y);
    int ly = (int)(oy + sb*x + sd*y);
    if (llfst) {
       llx = urx = lx;
       lly = ury = ly;
       llfst = 0;
    } else {
       if (lx < llx) llx = lx;
       if (lx > urx) urx = lx;
       if (ly < lly) lly = ly;
       if (ly > ury) ury = ly;
    }
}
    

/*  putpoint
 *  Add a point to the current path, drawing it if it gets too big.
 */
void putpoint(x, y)
	int x, y;
{
    if (pathlen == MAXPATHLEN) {
	/*
	 * We've got as many points stored as we can,
	 * so draw the path, and make the last point the first point
	 * of the new one so it's continuous
	 */
	drawpath();
	pathx[0] = pathx[MAXPATHLEN-1];
	pathy[0] = pathy[MAXPATHLEN-1];
	pathlen = 1;
    }
    /*
     * Now save the new point at the end of the current path.
     */
    pathx[pathlen] = x;
    pathy[pathlen] = y;
    pathlen++;
}

/* drawpath
 * Draw the current path
 */
void drawpath()
{
    if (pathlen > 0) {
	puts("NP");

	pathlen--;
	printf("%d %d M\n", pathx[pathlen], pathy[pathlen]);
	bbox(pathx[pathlen], pathy[pathlen]);
	while(--pathlen >= 0) {
	    printf("%d %d L\n", pathx[pathlen], pathy[pathlen]);
	    bbox(pathx[pathlen], pathy[pathlen]);
	}

	if (pathop == DRAW_PATH) {
	    puts("S");
	} else if (pathop == FILL_PATH) {
	    /* puts("closepath"); */
	    puts("F");
	}

	pathlen = 0;
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
	drawpath();
	pathop = newop;
    }
    return(saveop);
}
