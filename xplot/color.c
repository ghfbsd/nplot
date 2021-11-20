#include <stdio.h>
#include "xplot.h"
#include "pat.h"	/* For faking colors with gray patterns */

#define	MAPSIZE	256

static int usedcolors;
static int incolor;
static unsigned char red[MAPSIZE], green[MAPSIZE], blue[MAPSIZE];
static Colormap colors;
static unsigned int pixel[MAPSIZE];

int fakecolor = 0;
int colorinit = 1;

init_color()
{
    register int i;
    XWindowAttributes wattr;
    Screen *screen = XDefaultScreenOfDisplay(dpy);

    /*
     * Decide whether we're running on a color monitor
     */
    incolor = DefaultDepthOfScreen(screen) > 1;
    (void)XGetWindowAttributes(dpy,widgetwin,&wattr);
    colors = wattr.colormap;

    /*
     * Set up a color table; we use this even if we're on a B/W monitor
     * when we decide what patterns to use instead of colors.
     * Construct default colors, black and white, from background and
     * foreground values.
     */

    red[0] = green[0] = blue[0] = 255;	/* Ensure White presence */
    pixel[0] = bg;

    red[1] = green[1] = blue[1] = 0;	/* Ensure Black presence */
    pixel[1] = fg;

    /* Set all the other colors to black for now */
    for (i = 2; i < MAPSIZE-1; i++) {
	red[i] = green[i] = blue[i] = 0; pixel[i] = pixel[1];
    }
    usedcolors = 2;	/* Can't screw with color #0 */

    colorinit = 0;

}

color()
{
    register int i;
    unsigned char r, g, b;
    int thiscolor, diff, mindiff;

    r = getstring(instrm);
    g = getstring(instrm);
    b = getstring(instrm);

    /* draw the previous path before the color change */
    drawpath();

    /*
     * See if this color already exists in our colormap.
     */
    if (colorinit) init_color();
    thiscolor = -1;
    mindiff = 1 + abs(red[0] - r) + abs(green[0] - g) + abs(blue[0] - b);
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

    if ((diff > 0) && (usedcolors < MAPSIZE - 1)) {
	red[usedcolors]   = r;
	green[usedcolors] = g;
	blue[usedcolors]  = b;
	if (incolor) {
	    /* If we're on a color monitor, stick this into our color map */
	    XColor entry;
	    entry.flags = DoRed | DoGreen | DoBlue;
	    entry.red = r<<8; entry.green = g<<8; entry.blue = b<<8;
	    if (0 == XAllocColor(dpy,colors,&entry)) {
	       fprintf(stderr,
		  "%s:  Unable to allocate colormap entry %d %d %d\n",
		  progname,r,g,b);
	    } else {
	       pixel[usedcolors] = entry.pixel;
	    }
	}
	thiscolor = usedcolors;
	usedcolors++;
    }

    if (incolor) {
	/* On a color monitor; set the drawing colors */
	/* fprintf(stderr, "Setting drawing color to %d\n", pixel[thiscolor]);*/
	XSetForeground(dpy,gc,pixel[thiscolor]);
    } else if (r == 0 && g == 0 && b == 0) {
	/* Doing black on a b/w monitor; This is easy enough */
	fakecolor = 0;
    } else if (r == 255 && g == 255 && b == 255) {
	/* Doing white on a b/w monitor; This is easy too */
	fakecolor = 1;
    } else if (r != 0 || g != 0 || b != 0) {
	/* Doing a color on a b/w monitor; fake it with shades of gray */
	float intensity = ((0.30 * r) + (0.59 * g) + (0.11 * b))/255*GRAYNUM;
	fakecolor = GRAYSTART + (int)intensity;
    }
}
