/*
 * plot maximum enclosing box for raster data
 */

#include <stdio.h>
#include "apen.h"
short vgeth();

extern int xold, yold;
extern float xscale, yscale;
void putras()
   {
	register int header,count,y;
	int rxmin, rymin, rxmax, rymax, offset, raster;

	raster = 0;
	rxmin= rxmax= xold;
	rymin= 32767;
	rymax= yold;
	while( (header=vgeth(stdin)) != ENDRAS )
	  {
		count= header&0377;
		offset= (header>>8)&0377;
		if (count == 0377)	/* escape */
		  {	switch (offset)
			  {
			  case 't':
			  case 'T':
				for (count=0; getchar()>0; count++)
					;
				if (count&1)
					getchar();
				break;
			  }
			continue;
		  }
		y = yold + (offset*16)*yscale;
		if(rymin > y) rymin=y;
		y += (count*16)*yscale;
		if(rymax < y) rymax=y;
		while(count--)
		    (void) vgeth(stdin);
		raster++;
	  }
	rxmax += (raster)*xscale + 1;
	dump(rxmin,rymin,rxmin,rymax);
	dump(rxmin,rymax,rxmax,rymax);
	dump(rxmax,rymax,rxmax,rymin);
	dump(rxmax,rymin,rxmin,rymin);
   }
