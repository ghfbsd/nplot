/* setshad.c
 * Shading stuff common to both polygon and circle filling
 */

#include <stdio.h>
#include "apen.h"
#include "pat.h"

extern char *progname;

extern int forcegray;

typedef struct {
	int xmask, ymask;
} stype;

static int shadused=1, curshad=1;

void setshad(color,shade)
    int color,shade;
{
    register int j, i;
    int xmask, ymask, shadnum;
    double greylevel, greycolor();

    xmask = shade & 0377;
    if (!xmask) {
	fprintf(stderr, "%s: warning: illegal x mask of zero.  Set to 1.\n",
	   progname);
	xmask = 1;
    }
    ymask = (shade >> 8) & 0377;
    
    if(!ymask)
	ymask=xmask;

    /*
     * a mask of 1 means shade every row; 2 means every other row, etc.
     * to do this, take modulus of row (or column) by the mask.
     * Kludge this into a grey level.  If both masks are 1, entire area
     * is to be filled with current color.
     */
//  if (forcegray) {  /* Commented out - not sure why you want to skip ever */
    if (color == 0 || color == -1) {
       if (xmask == 1 && ymask == 1) 
	  greylevel = greycolor(color);
       else
	  greylevel = 1.0 - (1.0 / xmask / ymask);
       printf("%f setgray\n", greylevel);
    }
}
