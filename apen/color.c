#include <stdio.h>
#include "apen.h"
#include "pat.h"	/* For faking colors with gray patterns */

#define	MAPSIZE	256

static int usedcolors, lastcolor;
static unsigned char red[MAPSIZE], green[MAPSIZE], blue[MAPSIZE];
extern int forcegray;

void init_color()
{
    register int i;

    /*
     * Set up a color table; we use this even if we're on a B/W monitor
     * when we decide what patterns to use instead of colors
     */
    red[0] = green[0] = blue[0] = 255;	/* Background color is White */
    red[1] = green[1] = blue[1] = 0;	/* Drawing color is Black */

    /* Set all the other colors to black for now */
    for (i = 2; i < MAPSIZE; i++) {
	red[i] = green[i] = blue[i] = 0;
    }
    usedcolors = 2;	/* Can't screw with color #0 */
    lastcolor = -1;
}

int color(r,g,b)
    unsigned char r,g,b;
{
    register int i;
    int thiscolor, diff, mindiff, fakecolor;

    /*
     * See if this color already exists in our colormap.
     */
    thiscolor = -1;
    mindiff = 5000;
    for (i = 0; i < usedcolors; i++) {
	diff = abs(red[i] - r) + abs(green[i] - g) + abs(blue[i] - b);
	if (diff == 0) {
	    thiscolor = i;
	    break;
	} else if (diff < mindiff) {
	    mindiff = diff;
	    thiscolor = i;
	}
    }

    if (diff > 0 && usedcolors < MAPSIZE - 1) {
	red[usedcolors]   = r;
	green[usedcolors] = g;
	blue[usedcolors]  = b;
	thiscolor = usedcolors;
	usedcolors++;
    }

    if (forcegray) {
        if (thiscolor == 1) {
	   /* Color is black; just draw things normally */
	   fakecolor = 0;
        } else if (thiscolor == 0) {
	   /* White.  Use white pattern */
	   fakecolor = 1;
        } else {
	   /*
	    * A non-black color.  Fake it with a gray pattern.
	    * Don't use the last gray; it's too close to black.
	    */
	   fakecolor = GRAYSTART + (thiscolor % (GRAYNUM-1));
        } 
    } else
	fakecolor = (r << 16) | (g << 8) | b;
    
    /*
     * Flush out partial path and set color.
     */
    drawpath(); 
    if (!forcegray && (fakecolor != lastcolor)) 
       printf("<%06X> RGB\n",fakecolor);
    lastcolor = fakecolor;
    
    return (fakecolor);
}
