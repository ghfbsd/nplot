#include <stdio.h>
#include "nplot.h"

/*
 * nshad_():  a faked-up version of nplot_() which takes the passed
 * array of points to be the coordinates of a polygon and stipples
 * it with a shade of grey using the `a' command in plotfil(5).
 *
 * 11/28/87: John Werner ---- Changed to use local library call
 * vputh to do output so source is compatible between SUN and VAX
 */

/*LINTLIBRARY*/
nshad_(pn,x,y,spass,para)
	int *pn , *spass ;
	float *x , *y , *para ;
{
	float xplt,yplt;
	short xclos,yclos;
	double dx, dy;
	short ix0 , iy0 , ix , iy;
	int n, i;
  	int skey,shad;
	char aa='a';
        shad = *spass;

	n = (int)(*pn);
	if(n < 0) {
		fprintf(stderr,"NSHAD: error, negative point count\n");
		return(-1);
	}
	if (n==0) return(0);

	if(para[11]*para[19] == 0. ) {
		fprintf(stderr,"NSHAD: error, nplot has not been called\n");
		return(-1);
	}

	dx = (para[4]-para[2])/(RASTERS *para[0]) ;
	dy = (para[5]-para[3])/(RASTERS *para[1]);
	ix0 = getisz() * 50 ;
	iy0 = getisz() * 40 ;

	skey=(shad&0377)|((shad&0377)<<8);
	putc('a', plotfile);
	vputh(skey, plotfile);
	vputh(n, plotfile);

	for(i = 0 ; i < n ; i++) {
		xplt = x[i];
		yplt = y [i];
		iy = (yplt - para[3])/dy + iy0 ;
		ix = ( xplt - para[2])/dx + ix0 ;
		if (i==0) {
			xclos = ix;
			yclos = iy;
		}

		vputh(ix, plotfile);
		vputh(iy, plotfile);
	}
	/* close polygon */
	vputh(xclos, plotfile);
	vputh(yclos, plotfile);
	return(0);
}
