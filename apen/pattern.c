/* pattern
 * This file contains code for drawing patterns
 * To add more patterns the pat.h, apen.pattern, &
 * apen.setup files need to be edited.
 */

#include <stdio.h>
#include "apen.h"
#include "pat.h"

/*
 * Pattern definitions.
 * A zero or positive number tells the printer to use that gray leve.
 * A negative number tells it to use that pattern number.
 */
float pats[NUMPAT] = {
	0.0,	/* Solid Black */
	1.0,	/* Solid White */
	0.95,	/* Very Light Grey */
	0.8,	/* Lighter Grey */
	0.7,	/* Light Grey */
	0.6,	/* Medium Grey */
	0.45,	/* Dark Grey */
	0.3,	/* Darker Grey */
	0.2,	/* Incredibly Dark Grey */
	-1,	/* Light Stippling */
	-2,	/* Dark Stippling */
	-3,	/* Diagonal down to right */
	-4,	/* Diagonal down to left */
	-5,	/* Crossing diagonal lines */
	-6,	/* Horizontal Stripes */
	-7,	/* Vertical Stripes */
	-8,	/* Horizontal and Vertical Stripes */
	-9,	/* `Brick' pattern */
	-10,	/* Circle Thingies */
	-11,	/* Crossweave pattern */
	-12,	/* Fish Scales */
	-13,	/* Square things */
	-14,	/* ?? */
	-15,	/* medium diagonal down to right */
	-16,	/* medium diagonal down to left */
	-17,	/* medium crossing diagonal lines */
	-18,	/* dark diagonal down to right */
	-19,	/* dark diagonal down to left */
	-20,	/* dark crossing diagonal lines */
	-21,	/* closely spaced diagonal lines down to the right */
	-22,	/* closely spaced diagonal lines down to the left */
	-23,	/* closely spaced horizontal lines */
	-24,	/* closely spaced vertical lines */
	-25,	/* dash */
	-26,	/* volcanic */
	-27,	/* granite */
	-28,	/* vertical chevron */
	-29,	/* horizontal chevron */
	-30,	/* thing6 */
};

static char patused[NUMPAT];
extern float xscale, yscale;
extern char *progname;
static int patinit=0;

short vgeth();

double greycolor(color)
   int color;
/* greycolor -- Convert from a color index into a grey level. */
{
    return pats[color];
}

void dopat(color)
   int color;
{

    int xfirst, yfirst;
    int x1, y1;
    int npts, patnum, saveop;
    register int i;

    /*
     * We want to start a new path.  Draw the old one if needed
     */
    drawpath();


    /* Get pattern to draw */
    patnum = vgeth(stdin);
    if (patnum < 0 || patnum >= NUMPAT) {
	fprintf(stderr, "%s: pattern #%d illegal; using black.\n",
	   progname, patnum);
	patnum = 0;
    }


    puts("GS");
    /* If pattern is "black", fill area with present color. */
    if (patnum == 0 && color != 0) patnum = color;
    if (pats[patnum] >= 0.0) {
	/* Positive ones are gray scales */
	printf("%f setgray\n", pats[patnum]);
    } else {
	/*
	 * Negative ones are pattern indices.
	 * If the printer doesn't already know about patterns, tell it
	 */
	if(!patinit) {
	    dumpfile(PATFILE);
	    patinit = 1;
	}
	printf("pat%d 16 2 0 128 setpattern\n",
		(int)(-pats[patnum]));
    }

    saveop = setpathop(FILL_PATH);
    npts =  vgeth(stdin);
    xfirst = vgeth(stdin)*xscale;
    yfirst = vgeth(stdin)*yscale;
    putpoint(xfirst, yfirst);
    for(i=0; i<npts; i++) {
        x1= vgeth(stdin)*xscale;
        y1= vgeth(stdin)*yscale;
	putpoint(x1, y1);
    }
    drawpath();		/* Force the path to be drawn */

    puts("GR");

    (void) setpathop(saveop);
    if( (xfirst!=x1) && (yfirst!=y1) ) {
        fprintf(stderr,"%s: warning: polygon does not close.\n",
	   progname);
    }
}
