/* setshad.c
 * Shading stuff common to both polygon and circle filling
 */

#include <stdio.h>
#include "xplot.h"
#include "pat.h"

long int bitstr[32*GRAYNUM] = {
/*1*/0x80808080, 0x00000000, 0x00000000, 0x00000000,
     0x08080808, 0x00000000, 0x00000000, 0x00000000,
     0x80808080, 0x00000000, 0x00000000, 0x00000000,
     0x08080808, 0x00000000, 0x00000000, 0x00000000,
     0x80808080, 0x00000000, 0x00000000, 0x00000000,
     0x08080808, 0x00000000, 0x00000000, 0x00000000,
     0x80808080, 0x00000000, 0x00000000, 0x00000000,
     0x08080808, 0x00000000, 0x00000000, 0x00000000,
    
/*2*/0x80808080, 0x00000000, 0x08080808, 0x00000000,
     0x80808080, 0x00000000, 0x08080808, 0x00000000,
     0x80808080, 0x00000000, 0x08080808, 0x00000000,
     0x80808080, 0x00000000, 0x08080808, 0x00000000,
     0x80808080, 0x00000000, 0x08080808, 0x00000000,
     0x80808080, 0x00000000, 0x08080808, 0x00000000,
     0x80808080, 0x00000000, 0x08080808, 0x00000000,
     0x80808080, 0x00000000, 0x08080808, 0x00000000,
    
/*3*/0x88888888, 0x00000000, 0x22222222, 0x00000000,
     0x88888888, 0x00000000, 0x22222222, 0x00000000,
     0x88888888, 0x00000000, 0x22222222, 0x00000000,
     0x88888888, 0x00000000, 0x22222222, 0x00000000,
     0x88888888, 0x00000000, 0x22222222, 0x00000000,
     0x88888888, 0x00000000, 0x22222222, 0x00000000,
     0x88888888, 0x00000000, 0x22222222, 0x00000000,
     0x88888888, 0x00000000, 0x22222222, 0x00000000,
    
/*4*/0x88888888, 0x22222222, 0x88888888, 0x22222222,
     0x88888888, 0x22222222, 0x88888888, 0x22222222,
     0x88888888, 0x22222222, 0x88888888, 0x22222222,
     0x88888888, 0x22222222, 0x88888888, 0x22222222,
     0x88888888, 0x22222222, 0x88888888, 0x22222222,
     0x88888888, 0x22222222, 0x88888888, 0x22222222,
     0x88888888, 0x22222222, 0x88888888, 0x22222222,
     0x88888888, 0x22222222, 0x88888888, 0x22222222,
    
/*5*/0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
     0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
     0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
     0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
     0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
     0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
     0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
     0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555,
    
/*6*/0xDDDDDDDD, 0x77777777, 0xDDDDDDDD, 0x77777777,
     0xDDDDDDDD, 0x77777777, 0xDDDDDDDD, 0x77777777,
     0xDDDDDDDD, 0x77777777, 0xDDDDDDDD, 0x77777777,
     0xDDDDDDDD, 0x77777777, 0xDDDDDDDD, 0x77777777,
     0xDDDDDDDD, 0x77777777, 0xDDDDDDDD, 0x77777777,
     0xDDDDDDDD, 0x77777777, 0xDDDDDDDD, 0x77777777,
     0xDDDDDDDD, 0x77777777, 0xDDDDDDDD, 0x77777777,
     0xDDDDDDDD, 0x77777777, 0xDDDDDDDD, 0x77777777,
    
/*7*/0xDDDDDDDD, 0xFFFFFFFF, 0x77777777, 0xFFFFFFFF,
     0xDDDDDDDD, 0xFFFFFFFF, 0x77777777, 0xFFFFFFFF,
     0xDDDDDDDD, 0xFFFFFFFF, 0x77777777, 0xFFFFFFFF,
     0xDDDDDDDD, 0xFFFFFFFF, 0x77777777, 0xFFFFFFFF,
     0xDDDDDDDD, 0xFFFFFFFF, 0x77777777, 0xFFFFFFFF,
     0xDDDDDDDD, 0xFFFFFFFF, 0x77777777, 0xFFFFFFFF,
     0xDDDDDDDD, 0xFFFFFFFF, 0x77777777, 0xFFFFFFFF,
     0xDDDDDDDD, 0xFFFFFFFF, 0x77777777, 0xFFFFFFFF,
};

Pixmap shades[GRAYNUM] = {
   0, 0, 0, 0, 0, 0, 0
};
int noshades = 1;

XImage *simage;

setshad(shade)
    int shade;
{
    register int j, i;
    int xmask, ymask, index, op;
    double sqrt();

    xmask = shade & 0377;
    ymask = (shade >> 8) & 0377;

    if (ymask && !xmask) {
	fprintf(stderr, "%s: warning: illegal x mask of zero.  Set to 1.\n",
	   progname);
	xmask = 1;
    }
    
    if(!ymask)
       ymask=xmask;

    /*
     * a mask of 1 means shade every row; 2 means every other row, etc.
     * to do this, take modulus of row (or column) by the mask.
     */
    index = GRAYNUM - (int)sqrt((double)(xmask*ymask));
    
    /* New shade - allocate if possible */
    if ((index < GRAYNUM) && (shades[index] == 0)) {
       Pixmap new;
       int depth, type;

       depth = XDefaultDepth(dpy,XDefaultScreen(dpy));
       new = XCreatePixmap(dpy,widgetwin,32,32,depth);
       type = depth == 1 ? XYBitmap : ZPixmap;

       if (noshades) {
	  XVisualInfo visual, *info;
	  int items;
	  visual.screen = DefaultScreen(dpy);
	  info = XGetVisualInfo(dpy,VisualScreenMask,&visual,&items);
	  if (info == NULL) 
	     error("unable to get visual info for pattern build");
	  /* Can look at info.  Note that offset to next data line is
	     negative so as to write all planes the same */
	  simage = XCreateImage(dpy,info->visual,depth,type,
	     0,NULL,32,32,32,0);
	  XFree(info);
	  if (depth>1) 
	     simage->data = (char *)malloc(32*32*sizeof(unsigned long));
	  noshades = 0;
       }
       /* Construct bit map for image, copy to all planes. */
       if (depth == 1) {
	  simage->data = (char*)&bitstr[index*32];
       } else {
	  XGCValues gcv;
	  int i, word, bit, ix = index*32;

	  /* Get foreground & background from GC and set pixels in
	     color pixmap */
	  if (0 == XGetGCValues(dpy,gc,GCForeground|GCBackground,&gcv))
	     error("unable to get GC values for pattern build");
	  for(i=0;i<32*32;i++) {
	     word = i >> 5; bit = i & 0x1f;
	     XPutPixel(simage, bit, word,
		bitstr[ix+word] & (1<<(31-bit)) ?
		   gcv.foreground : gcv.background
	     );
	  }
       }
		
       XPutImage(dpy,new,gc,simage,0,0,0,0,32,32);
       shades[index] = new;
    }
       
    if ((xmask == 1) && (ymask == 1)) {
	/* Solid color - could be black, white or anything between */
	XSetFillStyle(dpy,gc,FillSolid);
	if (fakecolor) {
	    /* Some color, faked or otherwise */
	} else {
	    /* White */
	}
	op = FILL_PATH;
    } else {
	/* Gray patterns are 3 - 8 */
	XSetFillStyle(dpy,gc,FillTiled);
	XSetTile(dpy,gc,shades[index]);
	op = TILE_PATH;
    }
    return(op);
}
