#include <stdio.h>
#include "apen.h"

/*
 * Needmove is a horrendous kludge used by setfat so it doesn't have
 * to remember the last point drawn before the thickness is changed
 */
extern int plotted, needmove;

void
dump(int x1,int y1,int x2,int y2){
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
