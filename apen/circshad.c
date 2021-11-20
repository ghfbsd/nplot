/* circle()
 * Fill a circle with a pattern or outline it.
 * reads the following things from the plotfile
 * Mask, X, Y, Radius  (all half-words (16 bits))
 */
#include <stdio.h>
#include "apen.h"
#include "pat.h"

extern float xscale, yscale;
extern int plotmode;
short vgeth();

void circshad(color)
    int color;
{
    int rad, x0, y0;
    int shade, shadnum, pathop;

    /*
     * We want to start a new path.  Draw the old one if needed
     */
    drawpath();

    /* If xscale and yscale are different, a circle turns into an
       ellipse.  Handle this conundrum by averaging the two scale
       values together to obtain the new radius of the circle. */
    shade = vgeth(stdin);
    x0  = (int)(vgeth(stdin) * xscale);
    y0  = (int)(vgeth(stdin) * yscale);
    rad = (int)(vgeth(stdin) * (xscale + yscale)/2.0);
    puts("GS"); puts("NP");
    printf("%d %d %d C\n", x0, y0, rad);

    if (!shade) {
	/* Outline the circle if both masks are zero */
	puts("S");
	shadnum = 0;
    } else {
	/* Fill it otherwise */
	setshad(color,shade); puts("F");
    }
    puts("GR");
}
