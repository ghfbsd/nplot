#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <strings.h>
#include <string.h>             /* added string.h 2021-08-29 crb */
#include "nplot.h"
#include "font.h"


/* Type definitions */

enum orient { /* Numeric assignments correspond to plotfil(5) definitions */
   to_horizontal = 0,
   to_vertical = 1,
   to_upside_down = 2,
   to_downward = 3
};

struct txtinfo {		/* Information about current text font */
   int size, 			/* Font size */
       font; 			/* Font identifier */
   enum orient orientation;	/* Font orientation */
   } txtinfo;

enum sticode {tcopy, tsize, tfont, torient, theight};

void draw(), form(), convert(), fatcheck();

/* External variables defined herein */

FILE *plotfile;

short fatsave, origin_offset;

char *progname = "NPLOT";

nplot_(pn,x,y,xst,yst,cap,para,lxst,lyst,lcap)
	int *pn ;
	float *x , *y , *para ;
	char *xst , *yst , *cap ;
	int lxst, lyst, lcap ;
/*
c   VAX VERSION OF nplot.c is a plotting subroutine for "FORTRAN" programs.
c
c	To call it: call nplot(n,x,y,xs,ys,title,para)
c	where argument types are:
c	integer nn
c	real x(nn),y(nn),para(20)  :  nn is array length (max number of pts)
c	integer xs(m),ys(m),title(m) : m >= (number characters)/4
c	OR character xs(m),ys(m),title(m) : m = (number characters)
c	NOTE THAT IF character array is being filled by reading
c                    in values (as opposed to xs='something' statement)
c                    then m should be one more than the desired length
c                    to leave space for ASCII null character at end.
c	
c	A single call to nplot will scale the data in
c	x and y and wil output to file "pltfil" a set of
c	instructions to plot a curve (line or character plot) connecting  
c	the values in x and y; plot axes, grid marks and
c	gridmark labels; use the contents of
c	string xs to label the x axis, ys to label the y axis,
c	and title to label the graph. Subsequent calls to
c	nplot use the information stored in para to
c	plot x and y on the same axis; xs, ys, and title
c	are ignored. By resetting para(12) a new plot is
c	started; the previous size, grid step, labels, etc.
c	are preserved until the appropriate elements of
c	para are reset. Para has useful defaults: calling
c	it the first time with all elements zero sets up
c  	useful scales.
c 	
c
c		Contents of para are:
c		1,2: size in inches. 3,4: minimum values.
c	5,6: max values. 7,8 starts for grid. 9,10: grid step.
c	11 char. size. 12 if  0. set defaults and plot axes.
c	13 ascii value of point symbol, 14 its size.
c          For para(13):
c             0 - make line plot.
c            >0 - plot ascii symbol with the decimal value given,
c                 e.g. 65 is 'a'.
c            <0 - plot special symbol:
c                 -1 = up-triangle;
c                 -2 = down-triangle;
c                 -3 = right-triangle;
c                 -4 = left-triangle;
c                 -5 = square;
c                 -6 = diamond;
c                 -7 = hexagon;
c                 -8 = circle;
c                 These symbols are normally black.  They are drawn in
c                 outline form if -16 is added to any of the values,
c                 e.g. -19 is an outlined rightward pointing triangle.
c		Para(16) selects equal angle mode, if para(16) = 1. or -1., then
c	units on both axis are equal. In addition, para(16) < 0. skips
c 	the drawing of the box around the plot. The two functions of para(16)
c	can be combined. Also, if para(16) < 0., then gridding on 
c	x and y axes is skipped
c		If para(17)=1., x axis grid and label are omitted; if
c	para(18)=1. y axis grid and label are omitted.
c		Para(19) allows plots that save storage in the main
c	calling program. If para(19)=1. the points are uniformly
c	spaced in x; with starting point x(1) and increment x(2).
c	Thus x can be dimensioned 2 in the main program, even though
c	y has full length. If para(19)=-1. x has full length, but
c	points are uniformly spaced in y; with starting point y(1)
c	and increment y(2), so y can be dimensioned 2 in the main program.
c		Para(20) MUST be =0. on first call to nplot
c       and should not be rezeroed on later calls, as it
c	indicates that the plotfile is open. Zeroing it
c	causes all previous plots to be destroyed.
c
c   new version for FORTRAN.    H.K. 11/20/81
c   minor changes 6/82- subroutine renamed nplot.c
c          and set up for NU PDP-11/44, Seth Stein
c   subroutine axmi() modified for constant functions, Craig Bina, 1983
c   minor modifications to para(19) storage option, Seth Stein, 6/84
c   modified for variable plotfile names, Craig Bina, 1985
c
c   VAX VERSION - integers changed to short (i.e. 2 bytes).
c     This is crucial for those written out to "pltfil",
c     as pen/tek expect this (2 bytes in "write" commands, which use
c     the "geth" command).
c                 [EXCEPT the one used in pltext to provide text size
c                 and orientation; THIS IS STILL 4 BYTES (int).]
c     This also eliminates discrepancies between variables defined 
c     in some places as short and others as int, which are no longer
c     equivalent.  Minor bug related to string length in labels fixed.
c     Some stylistic changes also made
c     in response to VAX C compiler warnings. Seth Stein, 6/85 
c
c     Subroutines nline, nmove, fatcheck, convert added, John Werner 8/85
c     header file "nplot.h" added,  define RASTERS substituted for all
c     the 200's.  RASTERS=300 for new Imagen -- jhw 9/9/85
c
c	Modified by EAO Oct. 28, 1985 to allow for gridding when scale
c	does not necessarily increase from left to right or bottom to top
c
c	Modified by EAO, Nov. 7, 1985 to skip drawing if point has same
c	coordinate set as previous one (to RASTERS accuracy).
c
c       Modified for polygonal plotting symbols, Craig Bina, 1985
c
c       Modified for constant scaling of origin placement, C. Bina, 2-86
c
c       Modified for dashed lines via extended para[], C. Bina, 2-86
c
c	Modified for plotting filled circles, John Werner 6/27/86
c
c	Modified to do automatic gridding even when scaling is
c       reversed.  Fixed bug that caused equal X or Y increments
c	( para(19) ) to screw up previous scaling.  --- JHW, 8/7/86
c
c	Output to plot files now uses the standard and local i/o libraries
c	(vputh(), fputc(), etc.).  This is MUCH faster than using 
c	raw io, and uses far fewer system calls.  JHW, 11/4/86
c
c	Subroutine "plabel" added; has syntax same as nlabel except
c	that character string position given in plot coorinates. SS 4/87
c
c       Modified to plot hollow and filled symbols (see para(13)).
c       G. Helffrich 4/29/87.
c
c	Modified to define `pltfil' and `nplot_debug' environment variables
c	to control default plot file name and diagnostic output for nplot.
c       G. Helffrich 5/3/87.
c	
c	Modified by EAO June 28, 1987 to allow para(16) to control drawing
c	of box around plot.
c
c	11/28/87: John Werner ---- Changed to use local library calls
c	vputh and vputw for output so source is the same on SUN and VAX
c
c       Clean up code and add support for multiple fonts.
c       G. Helffrich 5/1/88.
c
c	Modified, 31 Mar 89 by EAO to allow separation of new page and gridding
c	functions of Para(12)
c
c       Add plus and cross symbols.
c       G. Helffrich 8/15/88.
c
c       Add code to use Fortran character string length.
c       G. Helffrich 6/3/99.
c
*/
{
	static char pfile[81] = PLOTFILE ;
	static char ev_plotfile[] = EV_PLOTFILE;
	static char ev_debug[] = EV_NPLOT_DEBUG;

	char *getenv();
	void pltext(),
             plpoly(),	       /* added plpoly() 2021-08-29 crb */
	     gettxtinfo(),
	     settxtinfo();

	float xplt,yplt,dxplt,dyplt;
	double dx, dy;
	short ix0 , iy0 , ixm , iym , ix , iy,  ixp=0, iyp=0, ix0mm;
	int n, i, l;
	short ilon, ixlp, iylp, ilxtnd, ilsnx, ilsny;
	float ixf, iyf, ixlof, iylof, ixlpf, iylpf;
	float fllen, flspc, flcnt, fldst, flm;
	double fldx, fldy;
	char cc, g[60], 
	   *epfile, *epdebug = NULL;
	char e='e';

#define font_extract(v) ((int)(((int)v)/1000))
#define size_extract(v) ((int)(((int)v)%1000))

/* If para(19) is a negative number, set plotfile name to title (=cap) and
   return with para(19) set to zero; else leave as 'pltfil' and continue. */
	if(para[19] < 0.) {
		int l = MIN(sizeof(pfile),lcap);
		strncpy(pfile,cap,l);
		pfile[l-1]='\0';
		para[19]=0.;
 		fprintf(stderr,"NPLOT: plotfile name set to %s\n",pfile);
		return(0);
	}
	if (para[19] == 0.) {
	   /* Check if plot file environment variable is about.  
	      If so, use its value as a plot file name.  */
	   epfile=getenv(ev_plotfile);
	   if (epfile != NULL) strncpy(pfile,epfile,sizeof(pfile));
	   /* Check if debug environment variable is about.  If so, use its
	      value as a plot file name.  */
	   epdebug=getenv(ev_debug);
	}
		
/* check for extended para (dashed lines) mode */
	ilxtnd = ((para[12]==0.)&&(para[13]<0.)) ? 1 : 0;
 	if ((ilxtnd == 1) && (epdebug != NULL)) 
	   fprintf(stderr,"NPLOT: extended mode\n");

/* Copy number of points, n.  Check if sensical value. */
	n = (int)(*pn);
	if((n < 0) || ((epdebug != NULL) && (n == 0))) {
	   fprintf(stderr,"NPLOT: error, non-positive point count\n");
	   return(-1);
	}

/* option to generate plot with equal increments
 *
 * BUG FIX -- jhw 8/7/86:  Only alter para if para[2] == para[4] or
 * para[3] == para[5] (i.e. if the axis hasn't been scaled yet)
 */
	if(para[18]==1.) {
	    dxplt = x[1];
	    xplt = x[0]-dxplt;
	    if (para[2] == para[4]) {
		para[2] = x[0];
		para[4] = x[0] + (n-1)*dxplt;
	    }
	} else if(para[18]==-1.) {
	    dyplt = y[1];
	    yplt = y[0]-dyplt;
	    if (para[3] == para[5]) {
		para[3] = y[0];
		para[5] = y[0] + (n-1)*dyplt;
	    }
	}

	/* Check if first call to nplot.  If so, set up default scaling
	   values if values aren't specified. */
	if(para[11]*para[19] == 0. ) {
           /* First call to nplot */
	   if(size_extract(para[10]) == 0. ) para[10] += 8. ;
	   origin_offset = size_extract(para[10]);
	   if(para[0]  == 0. ) para[0] = 5.5 ;
	   if(para[1]  == 0. ) para[1] =  5.5 ;
	   if(para[12] != 0. ) 
	      if(para[13] <= 0.) para[13] = 2. ;
	   /* find maximum and minimum of x,y if not given in para*/
	   if(para[3] == para[5] ) axmi(n,y,&para[3],&para[5],&para[1],para);
	   if(para[2] == para[4] ) axmi(n,x,&para[2],&para[4],&para[0],para);
	   if(para[15] == 1. || para[15] == -1.) {
	      double dxd, dyd, ddx , ddy  ;
	      dxd = ( para[4] - para[2] ) / para[0] ;
	      dyd = ( para[5] - para[3] ) / para[1] ;
	      if(dxd  >  dyd ) {
		 ddx = ( dyd-dxd)*para[0] * .5 ;
		 para[2] -= ddx ;
		 para[4] += ddx ;
	      } else {
		 ddy = (dxd - dyd )* para[1] * .5 ;
		 para[3] -= ddy ;
		 para[5] += ddy ;
	      }
	   }
	   if(para[8] == 0.) grid(&para[2],&para[4],&para[6],&para[8]);
	   if(para[9] == 0.) grid(&para[3],&para[5],&para[7],&para[9]);
	   if(para[8]*para[9] == 0. ) {
	      fprintf(stderr,"nplot unable to scale data \n");
	      return(-1);
	   }
	}

/* Code executed for all plots */
	/* Set up font information:  name, size, orientation. */
	settxtinfo(&txtinfo, torient, to_horizontal);
	settxtinfo(&txtinfo, tfont, font_extract(para[10]));
	settxtinfo(&txtinfo, tsize, size_extract(para[10]));
        dx = (para[4]-para[2])/(RASTERS *para[0]) ;
	dy = (para[5]-para[3])/(RASTERS *para[1]);
	ix0 = getisz() * 50 ;
	iy0 = getisz() * 40 ;
	ixm = ix0 + RASTERS * para[0] ;
	iym = iy0 + RASTERS * para[1] ;
	cc = 'm' ;

/* open the plotfile */
	if (para[19] == 0.) {
	    if (!(plotfile = fopen(pfile, "w"))) {
		fprintf(stderr, "Nplot: can't open %s\n",pfile);
		return(-1);
	    }
	    para[19] = 1.0;
	}
	if(para[11] == 0. )
		fputc('e', plotfile);
	fatcheck(para);

	if(para[12] == 0. ) {
/* line plot of the data */
	  if (ilxtnd != 1) {
/*	standard line plot */
		for(i=0 ; i < n ; i++ ) {
			if(para[18] == 1.) xplt=xplt+dxplt;
					else xplt = x[i];
			if(para[18] == -1.) yplt = yplt + dyplt;
					else yplt = y [i];
			ix =  (xplt - para[2])/dx  + ix0 ;
			iy =  (yplt - para[3])/dy  + iy0 ;
			
			/* Skip drawing if point is same as previous one */
			if(!i || ix != ixp || iy != iyp) {
			    ixp = ix; iyp = iy;
			    /*
			     * The next 2 lines would make it not draw
			     * outside the box, kinda sorta
			     * if(ix < ix0 || ix > ixm ) cc = 'm' ;
			     * if(iy < iy0 || iy > iym ) cc = 'm' ;
			     */
			    draw(cc,ix,iy);
			    cc = 'd' ;
			}
		}
	  }
	  else {
/*	dashed line plot */
		fllen = (para[20]<=0.) ? 1. : para[20];
		flspc = (para[21]<=0.) ? 0.75 : para[21];
		fllen = fllen * 20.;
		flspc = flspc * 20.;
		for(i=0 ; i < n ; i++ )
		{
			if(para[18] == 1.) xplt=xplt+dxplt;
					else xplt = x[i];
			if(para[18] == -1.) yplt = yplt + dyplt;
					else yplt = y[i];
			ixf =  (xplt - para[2])/dx  + ix0 ;
			iyf =  (yplt - para[3])/dy  + iy0 ;
			ix = ixf;
			iy = iyf;
			if(i > 0)     /* ignore duplicate sequential points */
			  if ((ix==ixp)&&(iy==iyp)) continue;
			ixp=ix;
			iyp=iy;
			if (i<=0) {
				cc = 'm';
				draw(cc,ix,iy);
				ixlof = ixf;
				iylof = iyf;
				ilon = 1;	/* turn on for first segment */
				cc = 'd';
				flcnt = fllen;
			}
			else {
				fldx = ixf-ixlof;
				fldy = iyf-iylof;
				 /* get segment length */
				fldst = sqrt(fldx*fldx+fldy*fldy);
				while (fldst >= flcnt) {
				  ilsnx = (fldx<0.) ? -1 : 1;
				  ilsny = (fldy<0.) ? -1 : 1;
				    /* account for vertical lines */
				  if (fabs(fldx) > fabs(fldy)*0.0001) {
				    flm = fldy / fldx;
				     /* do linear interpolation */
				    ixlpf = ilsnx * flcnt/sqrt(1.+flm*flm);
				    ixlpf = ixlof + ixlpf;
				    iylpf = iylof + (ixlpf-ixlof)*flm;
				  }
				  else {
				    ixlpf = ixlof;
				     /* account for top-to-bottom drawing */
				    iylpf = iylof + ilsny*flcnt;
				  }
				  ixlp = ixlpf;
				  iylp = iylpf;
				  draw(cc,ixlp,iylp);
				  cc = (ilon == 1) ? 'm' : 'd';
				  flcnt = (ilon == 1) ? flspc : fllen;
				  ilon = (ilon == 1) ? 0 : 1;
				  ixlof = ixlpf;
				  iylof = iylpf;
				    /* new distances */
				  fldx = ixf-ixlof;
				  fldy = iyf-iylof;
				  fldst = sqrt(fldx*fldx+fldy*fldy);
				}
				if (fldst > 0) {
				  draw(cc,ix,iy);
				  flcnt -= fldst;
				  ixlof = ixf;
				  iylof = iyf;
				}
			}
		}
	    }
	  }
/* character plot of the data */
	else if (para[12] > 0.)
	{
		struct txtinfo charinfo;
		char kc = para[12];

		double syxx = para[13]*2.5, syyy = para[13]*3.5 ;

		settxtinfo(&charinfo, tfont, 0);
		settxtinfo(&charinfo, torient, to_horizontal);
		settxtinfo(&charinfo, tsize, size_extract(para[13]));
		for(i = 0 ; i < n ; i++) {
			if(para[18] == 1.) xplt=xplt+dxplt;
					else xplt = x[i];
			if(para[18] == -1.) yplt = yplt + dyplt;
					else yplt = y [i];
			iy = (yplt - para[3])/dy - syyy + iy0 ;
			ix = ( xplt - para[2])/dx - syxx + ix0 ;
			pltext(ix, iy, &charinfo, &kc, 1) ;
		}
	}
/* polygon plot of the data */
	else
	{
		int vkc , vis ;
		vkc = -1.*para[12] ;
		if (para[13] <= 0.) para[13]=1. ;
		vis = para[13]*20. ;
		for(i = 0 ; i < n ; i++)
		{
			if(para[18] == 1.) xplt=xplt+dxplt;
					else xplt = x[i];
			if(para[18] == -1.) yplt = yplt + dyplt;
					else yplt = y [i];
			iy = (yplt - para[3])/dy + iy0 ;
			ix = ( xplt - para[2])/dx + ix0 ;
			plpoly(ix,iy,vis,vkc,1) ;
		}
	}

	/* Return if any call but the initial one. */
	if(para[11] >  0. ) return(0);

	/* Draw box around plot area */
	if(para[15] >= 0.) {
	   draw('m',ix0,iy0); draw('d',ix0,iym);
	   draw('d',ixm,iym); draw('d',ixm,iy0);
	   draw('d',ix0,iy0);
	}
/*
	   Draw gridlines and label axis */
/*
		 x axis grid*/
	if (para[15] >= 0. && para[16] != 1.) {
	   struct txtinfo linfo;
	   int isz, iszz, iszm, font_v;
	   short iyg, iygm, iyw;
	   float xx, xinf, xsup;

	   /* Make local copy of font info, and retrive current font
	      size. */
	   gettxtinfo(&txtinfo, tcopy, &linfo);
	   gettxtinfo(&linfo, tsize, &isz);
	   gettxtinfo(&linfo, theight, &font_v);
	   xx = para[6] ;
	   xinf = para[2] ;
	   xsup = para[4] ;
	   if(xsup < xinf) {
	      /* Watch out for inverted axes */
	      float temp = xinf;
	      xinf = xsup;
	      xsup = temp;
	   }

	   /* Find first grid point, and work up to maximum one */
	   iyg = iy0 + 5*isz ;
	   iygm = iym - 5*isz ;
	   iyw = iy0 - font_v*isz ;
	   do {
	      int strwidth;

	      if (fabs(xx) < para[8]*1.e-4 ) xx = 0. ;
	      form(xx,g,&l);
	      strwidth = txtsize(g, l, linfo.font);
	      ix = (xx - para[2]) / dx  + ix0 ;
	      draw('m',ix,iy0);
	      draw('d',ix,iyg);
	      draw('m',ix,iym);
	      draw('d',ix,iygm);
	      iszz = isz;
	      iszm = fabs((para[8]/dx)/strwidth) ;
	      if(iszz > iszm ) iszz = iszm ;
	      settxtinfo(&linfo, tsize, iszz);
	      ix =  ix - iszz*strwidth/2;
	      pltext(ix,iyw,&linfo,g,l);
	      xx += para[8];
	      if(xx - para[8]*.1e-5  > xsup ) break;
	   } while (xx + para[8]*.1e-5  > xinf);
	}
 
/*		
		 y axis grid*/
     	ix0mm = ix0 ;
	if (para[15] >= 0. && para[17] != 1.) {
	   struct txtinfo linfo;
	   int isz, iszz, font_v;
	   short ixg, ixgm, ixw;
	   float yy, ysup, yinf;

	   /* Make local copy of font info, and retrive current font
	      size. */
	   gettxtinfo(&txtinfo, tcopy, &linfo);
	   gettxtinfo(&linfo, tsize, &isz);
	   gettxtinfo(&linfo, theight, &font_v);
	   yy = para[7] ;
	   yinf=para[3] ;
	   ysup=para[5];
	   if(ysup < yinf) {
	      /* Watch out for inverted axes */
	      float temp=yinf ;
	      yinf=ysup;
	      ysup=temp;
	   }
	   ixg = ix0 + 5*isz ;
	   ixgm = ixm - 5*isz ;
	   do {
	      char gblank[sizeof(g)+1];
	      int strwidth;

              if( fabs(yy) < para[9]*1.e-5 ) yy = 0. ;
	      form(yy,g,&l); sprintf(gblank, "%s ", g);
	      iy = (yy-para[3])/dy + iy0 ;
	      draw('m',ix0,iy);
	      draw('d',ixg,iy);
	      draw('m',ixm,iy);
	      draw('d',ixgm,iy);
	      /* Don't let label size get too large.  Decrease character
		 size if digit span > 5. */
	      iszz = l > 5 ? isz*5.0/(float)(l) : isz;
	      settxtinfo(&linfo, tsize, iszz);
	      strwidth = txtsize(gblank, l+1, linfo.font);
	      iy -= (0.3*font_v)*iszz ;
	      ixw = ix0 - iszz*strwidth ;
	      if(iy < iy0 ) iy = iy0 ;
	      pltext(ixw,iy,&linfo,gblank,l+1);
	      yy += para[9] ;
	      if(ix0mm > ixw ) ix0mm = ixw ;
	      if(yy - para[9]*.1e-5 > ysup ) break;
	   } while (yy + para[9]*.1e-5 > yinf );
	}
 
/*		
  label x axis*/
	l=adjust(xst,lxst);
	if (l > 0) {
	   /* There's some text for this label.  Plot it. */
	   int isz, font_v;
	   struct txtinfo linfo;

	   gettxtinfo(&txtinfo, tcopy, &linfo);
	   gettxtinfo(&txtinfo, tsize, &isz);
	   gettxtinfo(&txtinfo, theight, &font_v);
	   ix = (ixm + ix0 - txtsize(xst, l, linfo.font)*isz)/2 ;
	   iy = iy0 - 2.2*font_v*isz ;
	   pltext(ix,iy,&linfo,xst,l);
	}
/*		
  label y axis*/
	l=adjust(yst,lyst);
	if (l > 0) {
	   /* There's some text for this label.  Plot it. */
	   int isz, font_v;
	   struct txtinfo linfo;

	   gettxtinfo(&txtinfo, tcopy, &linfo);
	   settxtinfo(&linfo, torient, to_vertical);
	   gettxtinfo(&linfo, tsize, &isz);
	   gettxtinfo(&linfo, theight, &font_v);
	   ix = (ix0mm - font_v*isz) ;
	   iy = (iym + iy0 - txtsize(yst, l, linfo.font)*isz ) / 2 ;
	   pltext(ix,iy,&linfo,yst,l);
	}
/*
 label the plot*/
	if (lcap>0 && strlen(cap) > 0) {
	   /* There's some text for this label.  Plot it. */
	   int isz, font_v;
	   int l = MIN(lcap,strlen(cap));

	   gettxtinfo(&txtinfo, tsize, &isz);
	   gettxtinfo(&txtinfo, theight, &font_v);
	   ix = (ixm + ix0 - txtsize(cap, lcap, txtinfo.font)*isz) / 2 ;
	   iy = iym + 0.6*font_v*isz ;
	   pltext(ix,iy,&txtinfo,cap,l);
	}
	para[11] = 1. ;
	return(0) ;

}

axmi(n,x,xmi,xma,xskl,para)
	int n ;
	float *x, *xmi , *xma , *xskl , *para ;
{
	float x1 , x2 , xx , dx, csp, symsiz ;
	int i ;
	x1 = x2 = *x ;
	for(i = 1 ; i < n ; i++ )
	{
		xx = x[i] ;
		if(xx  <  x1 ) x1 = xx ;
		if(xx  >  x2 ) x2 = xx ;
	}
	if (x1 == x2) {
		x1=0.;
		x2=2.*x2+(x2==0.);
	}
	dx = (x2 - x1 ) / 9. ;
	symsiz = para[13];
	if (symsiz < 0.) symsiz=0.;
	csp = symsiz*.5 * ( x2 - x1 )/ ( RASTERS * *xskl ) ;
	x2 += csp ;
	x1 -= csp ;
	if(dx == 0 ) return(-1);
	fint(x2,x2+dx,xma);
	fint(-x1,dx-x1,xmi);
	*xmi *= -1. ;
	return(0);
}

grid(xmi,xma,xgo,xgi) 
   float *xmi , *xma , *xgo , *xgi ;
{
	static float r[4] = { 1. ,2. , 5., 10.  } ;
	double min = *xmi, max = *xma;
	float  xgii , xgmag ,xg ;
	short i ;

	if( min == max ) return( -1 ) ;

	/* Take care of reversed axes */
	if( min > max ) {
	    double temp = min;
	    min = max;
	    max = temp;
	}

	fint(min,max,xgo);
	xgii = ( max - min ) / 3. ;
	fmag(xgii,&xgmag ) ;
	*xgi = xgmag ;
	for(i = 0 ; i < 4 ; i++)
	{
		xg = xgmag*r[i] ;
		if(xg > xgii ) break ;
		*xgi = xg ;
	}
	while(*xgo >= min ) *xgo -= *xgi ;
	*xgo += *xgi ;
	return(0);
}

fint(x1,x2,xi)
	float x1, x2, *xi ;
{
	float xag ;
	fmag((float) (fabs((double) x1)+fabs((double) x2)), &xag);
	*xi = -10.* xag ;
	xag *= 10. ;
l10:	while( *xi < x1 ) *xi += xag ;
	if( *xi < x2 ) return(0);
	*xi -= xag ;
	xag *= .1 ;
	goto l10 ;
}

fmag(x,xmag)
	float x , *xmag ;
{
	float xa ;
	if ( x == 0.) {
		*xmag = 0. ;
		return(0)  ;
	    }
	*xmag = 1. ;
	xa = fabs((double) x);
	while ( *xmag < xa ) *xmag *= 10. ;
	while( *xmag > xa ) *xmag *=  .1  ;
	return(0);
}

#define MAXVERT 200
void
plpoly(ix,iy,ik,ipsymb,shad)
    int shad,ik;
    short ix,iy;
    int ipsymb;
{
    int np,skey,ic,symb;
    short vert[MAXVERT+1][2];
    short radius;
    symb = ipsymb & 0x000f;
    skey=(shad&0377)|((shad&0377)<<8);
    if ((ipsymb>>4) & 0x0001) skey = 0;
    np=3;
    switch (symb) {			/* polygon types */
	case 1:		/* upright triangle */
	    vert[0][0]=ix-ik/2.;
	    vert[0][1]=iy-ik/3.464;
	    vert[1][0]=ix+ik/2.;
	    vert[1][1]=iy-ik/3.464;
	    vert[2][0]=ix;
	    vert[2][1]=iy+ik/1.732;
	    break;
	case 2:		/* inverted triangle */
	    vert[0][0]=ix-ik/2.;
	    vert[0][1]=iy+ik/3.464;
	    vert[1][0]=ix+ik/2.;
	    vert[1][1]=iy+ik/3.464;
	    vert[2][0]=ix;
	    vert[2][1]=iy-ik/1.732;
	    break;
	case 3:		/* rightward triangle */
	    vert[0][0]=ix-ik/3.464;
	    vert[0][1]=iy+ik/2.;
	    vert[1][0]=ix-ik/3.464;
	    vert[1][1]=iy-ik/2.;
	    vert[2][0]=ix+ik/1.732;
	    vert[2][1]=iy;
	    break;
	case 4:		/* leftward triangle */
	    vert[0][0]=ix+ik/3.464;
	    vert[0][1]=iy+ik/2.;
	    vert[1][0]=ix+ik/3.464;
	    vert[1][1]=iy-ik/2.;
	    vert[2][0]=ix-ik/1.732;
	    vert[2][1]=iy;
	    break;
	case 5:		/* square */
	    vert[0][0]=ix-ik/2.828;
	    vert[0][1]=iy+ik/2.828;
	    vert[1][0]=ix-ik/2.828;
	    vert[1][1]=iy-ik/2.828;
	    vert[2][0]=ix+ik/2.828;
	    vert[2][1]=iy-ik/2.828;
	    vert[3][0]=ix+ik/2.828;
	    vert[3][1]=iy+ik/2.828;
	    np=4;
	    break;
	case 6:		/* diamond */
	    vert[0][0]=ix;
	    vert[0][1]=iy+ik/1.7;
	    vert[1][0]=ix-ik/2.5;
	    vert[1][1]=iy;
	    vert[2][0]=ix;
	    vert[2][1]=iy-ik/1.7;
	    vert[3][0]=ix+ik/2.5;
	    vert[3][1]=iy;
	    np=4;
	    break;
	case 7: 		/* hexagon */
	    vert[0][0]=ix-ik/2.;
	    vert[0][1]=iy;
	    vert[1][0]=ix-ik/4.;
	    vert[1][1]=iy-ik/2.309;
	    vert[2][0]=ix+ik/4.;
	    vert[2][1]=iy-ik/2.309;
	    vert[3][0]=ix+ik/2.;
	    vert[3][1]=iy;
	    vert[4][0]=ix+ik/4.;
	    vert[4][1]=iy+ik/2.309;
	    vert[5][0]=ix-ik/4.;
	    vert[5][1]=iy+ik/2.309;
	    np=6;
	    break;
	case 9:		/* plus */
	    { float s1=ik/2.828,
		    s2=ik/2.828/10.0;
	    vert[0][0]=ix-s2; vert[0][1]=iy-s1;
	    vert[1][0]=ix+s2; vert[1][1]=iy-s1;
	    vert[2][0]=ix+s2; vert[2][1]=iy-s2;
	    vert[3][0]=ix+s1; vert[3][1]=iy-s2;
	    vert[4][0]=ix+s1; vert[4][1]=iy+s2;
	    vert[5][0]=ix+s2; vert[5][1]=iy+s2;
	    vert[6][0]=ix+s2; vert[6][1]=iy+s1;
	    vert[7][0]=ix-s2; vert[7][1]=iy+s1;
	    vert[8][0]=ix-s2; vert[8][1]=iy+s2;
	    vert[9][0]=ix-s1; vert[9][1]=iy+s2;
	    vert[10][0]=ix-s1; vert[10][1]=iy-s2;
	    vert[11][0]=ix-s2; vert[11][1]=iy-s2;
	    vert[12][0]=ix-s2; vert[12][1]=iy-s1;
	    np=13;
	    break;
	    }
	case 10:	/* X/cross */
	    { float s1=ik/2.828,
		    s2=ik/2.828/10.0;
	    vert[0][0]=ix;    vert[0][1]=iy-s2;
	    vert[1][0]=ix+s1-s2; vert[1][1]=iy-s1;
	    vert[2][0]=ix+s1; vert[2][1]=iy-s1+s2;
	    vert[3][0]=ix+s2; vert[3][1]=iy;
	    vert[4][0]=ix+s1; vert[4][1]=iy+s1-s2;
	    vert[5][0]=ix+s1-s2; vert[5][1]=iy+s1;
	    vert[6][0]=ix;    vert[6][1]=iy+s2;
	    vert[7][0]=ix-s1+s2; vert[7][1]=iy+s1;
	    vert[8][0]=ix-s1; vert[8][1]=iy+s1-s2;
	    vert[9][0]=ix-s2; vert[9][1]=iy;
	    vert[10][0]=ix-s1;vert[10][1]=iy-s1+s2;
	    vert[11][0]=ix-s1+s2; vert[11][1]=iy-s1;
	    vert[12][0]=ix;   vert[12][1]=iy-s2;
	    np=13;
	    break;
	    }
	case 8:		/* circle and unrecognized codes */
	default:			/* filled circle */
	    radius = ik/2;
	    fputc('c', plotfile);
	    vputh(skey, plotfile);	/* shading mask */
	    vputh(ix, plotfile);	/* center */
	    vputh(iy, plotfile);
	    vputh(radius, plotfile);
	    return;	/* This isn't done as a polygon, so leave */
    }
    fputc('a', plotfile);
    vputh(skey, plotfile);
    vputh(np, plotfile);
    for(ic=0;ic < np;ic++) {
	vputh(vert[ic][0], plotfile);
	vputh(vert[ic][1], plotfile);
    }
    /* close polygon */
    (void) vputh(vert[0][0], plotfile);
    (void) vputh(vert[0][1], plotfile);
}

void
draw(cc,ix,iy)
	char cc ;
	short ix,iy;
{
	fputc(cc, plotfile);
	vputh(ix, plotfile);
	vputh(iy, plotfile);
}

void
form(x,str,l)
	float x ;
	int *l ;
	char *str;
{
	sprintf(str,"%g",x);
	*l = 0;
	while(*(str++) ) (*l)++ ;
	if (*(str-2) == '.' ) (*l)-- ;
}

adjust(str,l)
	char *str ;
	int l ;
{
	int jj = l<=0 ? 0 : strlen(str);
        return(MIN(jj,l));
}




/* ---------------------------------------------------------------
 * Following subroutines added by John Werner 8/85
 * fortran-callable subroutines:
 *  CALL Nline(x, y, para)
 *  CALL Nmove(x, y, para)
 * Subroutine nline draws a line from the current pen position
 * to the position (x,y) in global coordinates coordintes used for x
 * and y in Nplot.  Nmove moves without drawing.
 */

nline_(x,y,para)
	float *x , *y , *para ;
{
	short ix, iy;

	if (para[19] == 0.) {		/* See if plotfile is open */
		fprintf(stderr,"Nline: plotfile not open\n");
		return(-1);
	}

	fatcheck(para);

	convert(x, y, &ix, &iy, para);
	draw('d',ix,iy);
	return(0);
}

nmove_(x,y,para)
	float *x , *y , *para;
{
	short ix, iy;

	if (para[19] == 0.) {		/* See if plotfile is open */
		fprintf(stderr,"Nmove: plotfile not open\n");
		return(-1);
	}

	fatcheck(para);

	convert(x, y, &ix, &iy, para);
	draw('m',ix,iy);
	return(0);
}

void
convert(x, y, ix, iy, para)	/* convert point to local coordinates */
	float *x, *y, *para;
	short *ix, *iy;
{
	double dx, dy;
	short ix0 , iy0;
	dx = (para[4]-para[2])/(RASTERS *para[0]);
	dy = (para[5]-para[3])/(RASTERS *para[1]);
	ix0 = getisz() * 50;
	iy0 = getisz() * 40;
	*ix =  (*x - para[2])/dx  + ix0 ;
	*iy =  (*y - para[3])/dy  + iy0 ;
}

/* see if line thickness has changed */
void
fatcheck(para)
	float *para;
{
	char f = 'f';
	if (fatsave != para[14]) {
		fatsave = para[14];
		fputc('f', plotfile);
		vputh(fatsave, plotfile);
	}
}

nlabel_(x,y,xst,para,lxst)
	float *x , *y , *para ;
	char *xst ;
	int lxst ;
/* subroutine to place label string in xst at position
 x,y (inches) relative to lower left corner of plot produced
 by subroutine nplot. It assumes nplot has been called already
 and uses the same control array para and subroutines.
 Designed to be called from fortran using same conventions
 as nplot.
 VAX VERSION: all integers changed to short (2 bytes): This is
 crucial for output to pltfil.
 Also minor changes to keep C compiler happy. Seth Stein, 6/85
 Call to function 'fatcheck' added: John Werner 8/85
 Modified for constant scaling of origin placement, C. Bina, 2-86
*/
{
	int l;
	short  ix0 , iy0 , ix , iy  ;
	float px,py;
	enum orient rotation;
	void settxtinfo(),
	     pltext();

	px = *x;
	py = *y;
	if (!para[19]) {
	    fprintf(stderr, "Nlabel: plot not opened; call nplot first\n");;
	    return(-1);
	}
	fatcheck(para); /* set line width if needed */

	/* Set up text font, size, and orientation */
	settxtinfo(&txtinfo,tfont,font_extract(para[10]));
	settxtinfo(&txtinfo,tsize,size_extract(para[10]));
	if ((l = ((int) para[18])%360) < 0) l += 360;
	rotation = (enum orient) (0x03 & (l/90 + (l%90 < 45 ? 0 : 1)));
	settxtinfo(&txtinfo,torient,rotation);
	ix0 = getisz() * 50 ;
	iy0 = getisz() * 40 ;
	ix = px*RASTERS  + ix0;
	iy = py*RASTERS  + iy0;
	if(ix <=0 || iy <=0 ) {
	    fprintf(stderr,"nlabel label off plot \n");
	    return(-1);
	}
   	l=adjust(xst,lxst);
	if (l > 0) pltext(ix,iy,&txtinfo,xst,l);
	return(0) ;
}

plabel_(x,y,xst,para,lxst)
	float *x , *y , *para ;
	char *xst ;
	int lxst;
/* subroutine to place label string in xst at position
   x,y in plot window coordinates being used by subroutine nplot. 
   It assumes nplot has been called already and uses the same 
   control array para and subroutines.  Designed to be called from 
   fortran using same conventions as nplot. In particular, 
   para (11) sets character size, and para(19) controls orientation.
*/
{
	int l;
	short  ix0 , iy0 , ix , iy  ;
	float px,py;
	double dx, dy;
	enum orient rotation;
	void settxtinfo(),
	     pltext();

	px = *x;
	py = *y;
	if (!para[19]) {
	    fprintf(stderr, "Plabel: plot not opened; call nplot first\n");;
	    return(-1);
	}
	fatcheck(para); /* set line width if needed */
	settxtinfo(&txtinfo,tfont,font_extract(para[10]));
	settxtinfo(&txtinfo,tsize,size_extract(para[10]));
	if ((l = ((int) para[18])%360) < 0) l += 360;
	rotation = (enum orient) (0x03 & (l/90 + (l%90 < 45 ? 0 : 1)));
	settxtinfo(&txtinfo,torient,rotation);
	ix0 = getisz() * 50 ;
	iy0 = getisz() * 40 ;
    	dx = (para[4]-para[2])/(RASTERS *para[0]) ;
	dy = (para[5]-para[3])/(RASTERS *para[1]);
			ix =  (px - para[2])/dx  + ix0 ;
			iy =  (py - para[3])/dy  + iy0 ;
	if(ix <=0 || iy <=0 ) {
	    fprintf(stderr,"plabel label off plot \n");
	    return(-1);
	}
   	l=adjust(xst,lxst);
	if (l > 0) pltext(ix,iy,&txtinfo,xst,l);
	return(0) ;
}

struct textword {
   /* This structure defines the word which describes text orientation, etc. */
   unsigned       :16,		/* for future use */
      font_code   :8,		/* font identification code */
		  :1,		/* unused - should be zero */
      orientation :2,		/* Text motion:
				   0 - horizontal and down
				   1 - vertical and to left
				   2 - horizontal and up
				   3 - vertical and to right */
      size        :5;		/* Font size */
   };
#define MAX_FONT_CODE	255
#define MAX_FONT_SIZE	31

void
pltext(ix,iy,info,text,nl)
	int nl;
	short ix,iy;
	char *text;
	struct txtinfo *info;
{
	int item;
	/*
	union {
	   struct textword tword;
	   long int text_word;
	} item;
	*/

	/* Copy font information from text structure into the
	   word in preparation for writing out. */
	/*
	item.tword.font_code = (unsigned) info->font;
	item.tword.orientation = (unsigned) info->orientation;
	item.tword.size = (unsigned) info->size;
	*/
	item = (info->size & 0x001f)
	   | (((int)info->orientation << 5) & 0x0060)
	   | ((info->font << 8) & 0xff00);

	/* Move to origin of text and put the text there */
	(void) draw('m',ix,iy);
	(void) fputc('t', plotfile);
	(void) vputw(item, plotfile);
	(void) fwrite(text, nl, 1, plotfile);
	(void) fputc('\0', plotfile);
}

void
gettxtinfo(where, what, to_what)
   struct txtinfo *where;
   enum sticode what;
   union {
      int integer;
      struct txtinfo structure;
   } *to_what;

/* Get the item in the text descriptor described by 'what' and return
   it to the thing pointed to by to_what.
*/
{
   struct fontdescr *fd;

   switch (what) {
      case tfont:
	 to_what->integer = (int) where->font; break;
      case tsize:
	 to_what->integer = (int) where->size; break;
      case torient:
	 to_what->integer = (int) where->orientation; break;
      case tcopy:
	 to_what->structure = *where; break;
      case theight:
	 fd = getfont(where->font);
	 if (fd == NULL) {
	    fprintf(stderr, 
	       "NPLOT: Couldn't get width for font %d, using default.\n",
	       where->font);
	    fd = getfont(0);
	 }
	 to_what->integer = fd->fd_height;
	 break;
      default:
	 fprintf(stderr,
	    "NPLOT: Internal error - gettxtinfo called with invalid code\n");
	 exit(1);
   }
}

void
settxtinfo(where, what, to_what)
   struct txtinfo *where;
   enum sticode what;
   int to_what;

/* Set the item in the text descriptor described by 'what' to the
   value given by to_what.
*/
{
   switch (what) {
      case tfont:
	 where->font = to_what <= MAX_FONT_CODE ? to_what : 0;
	 break;
      case tsize:
	 where->size = to_what <= MAX_FONT_SIZE ? to_what : 0;
	 break;
      case torient:
	 where->orientation = (enum orient) to_what;
	 break;
      default:
	 fprintf(stderr,
	    "NPLOT: Internal error - settxtinfo called with invalid code\n");
	 exit(1);
   }
}
