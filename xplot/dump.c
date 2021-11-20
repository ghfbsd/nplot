#include <stdio.h>
#include "xplot.h"

/*
 * Needmove is a horrendous kludge used by setfat so it doesn't have
 * to remember the last point drawn before the thickness is changed
 */
extern int needmove;

void
dump(x1,y1,x2,y2)
	int x1,y1,x2,y2;
{
	static int xlst, ylst;

	if( clip(&x1,&y1,&x2,&y2) ) {
		return;
	}
	if( (x1 != xlst) || (y1 != ylst) || !plotted || needmove) {
	    drawpath();		/* Need to do a move */
	    putpoint(x1, y1);
	}
	putpoint(x2, y2);
	xlst= x2;
	ylst= y2;
	plotted = 1;
	needmove = 0;
}
