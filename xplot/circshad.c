/* circle()
 * Fill a circle with a pattern or outline it.
 * reads the following things from the plotfile
 * Mask, X, Y, Radius  (all half-words (16 bits))
 */
#include <stdio.h>
#include "xplot.h"
#include "pat.h"

circshad()
{
    XPoint center;
    int radius;
    int shade, shadnum;

    /*
     * We want to start a new path.  Draw the old one if needed
     */
    drawpath();

    shade = vgeth(stdin);
    center.x  = vgeth(stdin) * xscale;
    center.y  = pymax - (vgeth(stdin) * yscale);
    radius    = vgeth(stdin) * (xscale + yscale) / 2.0;
    center.x -= radius; center.y -= radius;

    if (!shade) {
	/* Outline the circle if both masks are zero */
	XDrawArc(dpy,pixmap,gc,center.x,center.y,
	   (unsigned)(2*radius),(unsigned)(2*radius),0,360*64);
    } else {
	/* Fill it otherwise */
	(void)setshad(shade);
	XFillArc(dpy,pixmap,gc,center.x,center.y,
	   (unsigned)(2*radius),(unsigned)(2*radius),0,360*64);
    }
}
