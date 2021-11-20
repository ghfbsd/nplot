#include	<stdio.h>
#include	<ctype.h>
#include	"apen.h"

extern float xscale, yscale;
extern int plotted, embedded, pageno;
extern int xmin, xmax, ymin, ymax;
extern int xold, yold, xnew, ynew;
extern int pxmax, pymax;
extern char *progname;
int vgeth();

void doplot(footer,fatbase)
   struct foot_info *footer;
   int fatbase;
{
    register int c;
    int current_color;
    void initpage();

    initpage(footer,fatbase);	 /* set up coordinate system */
    current_color = color((unsigned char)0,(unsigned char)0,(unsigned char)0);
    while((c=getchar()) != EOF) {
	switch (c) {   /* command list */
	    case 'm':	/* move */
		xold = vgeth(stdin)*xscale;
		yold = vgeth(stdin)*yscale;
		break;
	    case 'd':	/* draw */
		xnew=(int)vgeth(stdin)*xscale;
		ynew=(int)vgeth(stdin)*yscale;
	        dump(xold,yold,xnew,ynew);
		xold=xnew;
		yold=ynew;
		plotted=1;
		break;
	    case 't':	/* text */
		putsym();
		plotted=1;
		break;
	    case 'b':	/* break */
	    case 'e':	/* erase */
		if (plotted) {	/* Don't erase when we don't need it */
		    drawpath();
		    if (!embedded) {
			puts("end\nshowpage"); initpage(footer,fatbase);
		    }
		    plotted  = 0;
		}
		break;
	    case 'f':	/* fatbase */
		setfat(vgeth(stdin)+fatbase );
		break;
	    case 'r':	/* raster data */
		putras();
		plotted=1;
		break;
	    case 'w':	/* window */
		xmin=(vgeth(stdin))*xscale;
		if(xmin<0) xmin=0;
		xmax=(vgeth(stdin))*xscale;
		if(xmax>pxmax) xmax=pxmax;
		ymin=(vgeth(stdin))*yscale;
		if(ymin<0) ymin=0;
		ymax=(vgeth(stdin))*yscale;
		if(ymax>pymax) ymax=pymax;
		break;
	    case 'x':	/* display x-hairs */
		break;
	    case 'p':	/* purge buffers */
		(void) fflush(stdout);
		break;
	    case 'n':	/* no op */
		break;
	    case 'a':	/* shade polygon */
		area(current_color);
		plotted=1;
		break;
	    case 'A':	/* shade polygon with pattern */
		dopat(current_color);
		plotted=1;
		break;
	    case 'o':	/* change OR mode */
		/* ormode(); This is impossible on the LaserWriter */
		break;
	    case 'c':	/* Shade a Circle */
		circshad(current_color);
		plotted=1;
		break;
	    case 'l':	/* draw a line */
		line();
		plotted=1;
		break;
	    case 'C':	/* Color */
		{ unsigned char r, g, b;
		   r = getchar(); g = getchar(); b = getchar();
		   current_color = color(r,g,b);
		}
		break;
	    default:	 /* error */
		fprintf(stderr,"\n%s: invalid plot command %c\n",
		   progname, c);
		(void) fflush(stderr);
		exit(1);
	    }
	}
    drawpath();
    puts ("end");
    /* PostScript EPS definition does not forbid use of "showpage"
       operator and some interpreters require it to process an
       EPS image. */
    /*if (!embedded)*/ puts("showpage");
    plotted = 0;
    (void) fflush(stdout);
}
