#include <stdio.h>
#include "apen.h"

#define puthalf(e)		(void) sputh(e,stdout)

extern int vertical, embedded, pageno;

void initpage(footer,fatbase)
   struct foot_info *footer;
   int fatbase;
{
   pageno += 1;
   printf ("%%%%Page: %d %d\n",pageno,pageno);
   puts("AD begin");	/* Set up local dictionary */
   puts("GR");		/* Restore to original graphics state */
   if (footer->fi_footer) {        /* Emit footer if wanted */
      puts ( "GS") ;
      puts ( "/Helvetica findfont 8 scalefont setfont") ;
      puts ( "matrix defaultmatrix setmatrix") ;
      puts ( "8.5 72 mul 0 translate 90 rotate");
      puts ( "0.5 72 mul 0.5 72 mul moveto") ;
      printf ("(%s     %s     %s) show \n", 
	 footer->fi_user,footer->fi_dir,footer->fi_filename); 
      puts ( "10.5 72 mul 0.5 72 mul moveto");
      printf ( "(%s) stringwidth pop neg 0 rmoveto (%s) show\n",
	 footer->fi_date, footer->fi_date) ;
      puts("GR");
   }
   if (!vertical) {
      /*
       * Horizontal coordinate system.  Page turned sideways;
       * origin in lower-left corner
       */
      puts("600 0 translate");	/* Move origin */
      puts("90 rotate");		/* Rotate coordinates */
      tran(600.0, 0.0); rot(90.0);
   }
   puts("72 300 div dup scale");
   scal(72.0/300.0, 72.0/300.0);
   if (vertical & !(vertical & 0x0002)) {
      /* Not aligned to bottom, move up YSIZE - XSIZE units from 
	  bottom of the page. */
       printf("0 %d translate\n",YSIZE-XSIZE);
       tran(0.0, (double)(YSIZE-XSIZE));
   }
   if (fatbase) setfat(fatbase);	/* Set up excess line width */
}
