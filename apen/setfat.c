#include <stdio.h>

extern int curfat, needmove;

/* set the line thickness to newfat */

void faterr(bad, good)
    int bad, good;
{
    fprintf(stderr, "apen: line thickness of %d out of range; set to %d\n",
	bad, good);
}

void setfat(newfat)
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
    printf("%d setlinewidth\n", newfat);

    curfat = newfat;
    /*
     * KLUDGE: We really should save the last point on the path
     * before we draw it, and then put it back on the new path.
     * This would be too hard, so we'll cheat
     */
    needmove = 1;
}
