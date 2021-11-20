/* pattern
 * This file contains code for drawing patterns
 */

#include <stdio.h>
#include "xplot.h"
#include "pat.h"

/*
 * Pattern definitions.
 */
unsigned long pats[] = {
     	0xFFFFFFFF, 0xFFFFFFFF, 	  	/* 0 */
     	0x00000000, 0x00000000, 	  	/* 1 */
     	0x80000000, 0x08000000, 	  
     	0x80000800, 0x80000800, 	  
     	0x88002200, 0x88002200, 	  
     	0x88228822, 0x88228822, 	  
     	0xAA55AA55, 0xAA55AA55, 	  
     	0xDD77DD77, 0xDD77DD77, 	  
     	0xDDFF77FF, 0xDDFF77FF, 	  
     	0x80100420, 0x01084004, 	  
     	0xB130031B, 0xD8C00C8D, 	  
     	0x80402010, 0x08040201, 	  
     	0x01020408, 0x10204080, 	   
     	0x41221408, 0x14224180, 	  
     	0x00000000, 0x000000FF, 	  
     	0x80808080, 0x80808080, 	  
     	0x80808080, 0x808080FF, 	  
     	0xFF808080, 0xFF080808, 	  
     	0x378C8F87, 0x73C8F870, 	  
     	0xF87C3223, 0xC78F1331, 	  
     	0x80402008, 0x0AE00000, 	  
     	0xBF00BFBF, 0xB0B0B0B0, 	  
     	0x102C57FF, 0x01020408, 	  
     	0x6030180C, 0x060381C0, 	  
     	0x03060C18, 0x3060C081, 	  
     	0x63361C1C, 0x3663C1C1, 	  
     	0x783C1E0F, 0x87C3E1F0, 	  
     	0x0F1E3C78, 0xF0E1C387, 	  
     	0x7F3E3E7F, 0xF7E3E3F7, 	  
     	0x88442211, 0x88442211, 	  
     	0x11224488, 0x11224488, 	  
     	0x000000FF, 0x000000FF, 	  
     	0x88888888, 0x88888888, 	  
     	0x00E00087, 0x00007800, 	  
     	0x00058850, 0x20018244, 	  
     	0x40208001, 0x02040080, 	  
     	0x88442211, 0x11224488, 	  
     	0x81422418, 0x81422418, 	  
     	0x80402008, 0x01000000  	  
};
#define NUMPAT	(sizeof(pats)/(2*sizeof(unsigned long)))

Pixmap patused[NUMPAT];
XImage *pimage;
static int patinit=1;
extern int fakecolor;

dopat()
{

    int xfirst, yfirst;
    int x1, y1;
    int npts, patnum, saveop;
    register int i;

    /*
     * We want to start a new path.  Draw the old one if needed
     */
    drawpath();

    /* Get pattern to draw */
    patnum = vgeth(stdin);

    if (patnum == 0 && fakecolor)
	patnum = fakecolor;	/* We're faking a color with a shade of gray */

    if (patnum < 0 || patnum >= NUMPAT) {
	fprintf(stderr, "%s: pattern #%d illegal; using black.\n",
		progname,patnum);
	patnum = 0;
    }

    /* New shade - allocate if possible */
    if (patinit || (patused[patnum] == 0)) {
       Pixmap new;
       int depth, type;

       depth = XDefaultDepth(dpy,XDefaultScreen(dpy));
       new = XCreatePixmap(dpy,widgetwin,8,8,depth);
       type = depth == 1 ? XYBitmap : ZPixmap;

       if (patinit) {
	  XVisualInfo visual, *info;
	  int items;
	  visual.screen = DefaultScreen(dpy);
	  info = XGetVisualInfo(dpy,VisualScreenMask,&visual,&items);
	  if (info == NULL) 
	     error("unable to get visual info for pattern build");
	  /* Can look at info.  Note that offset to next data line is
	     negative so as to write all planes the same */
	  pimage = XCreateImage(dpy,info->visual,depth,type,
	     0,NULL,8,8,8,0);
	  XFree(info);
	  if (depth>1) 
	     pimage->data = (char *)malloc(8*8*sizeof(unsigned long));
	  patinit = 0;
       }
       /* Construct bit map for image, copy to all planes. */
       if (depth == 1) {
	  pimage->data = (char*)&pats[patnum*2];
       } else {
	  XGCValues gcv;
	  int i, word, bit, ix=2*patnum;

	  /* Get foreground & background from GC and set pixels in
	     color pixmap */
	  if (0 == XGetGCValues(dpy,gc,GCForeground|GCBackground,&gcv))
	     error("unable to get GC values for pattern build");
	  for(i=0;i<8*8;i++) {
	     word = i >> 5; bit = i & 0x1f;
	     XPutPixel(pimage, i>>3, i&0x07,
		pats[ix+word] & (1<<(31-bit)) ?
		   gcv.foreground : gcv.background
	     );
	  }
       }
		
       XPutImage(dpy,new,gc,pimage,0,0,0,0,8,8);
       patused[patnum] = new;
    }

    XSetTile(dpy,gc,patused[patnum]);

    saveop = setpathop(patnum==0?FILL_PATH:TILE_PATH);
    npts =  vgeth(stdin);
    xfirst = vgeth(stdin)*xscale;
    yfirst = vgeth(stdin)*yscale;
    putpoint(xfirst, yfirst);
    for(i=0; i<npts; i++) {
        x1= vgeth(stdin)*xscale;
        y1= vgeth(stdin)*yscale;
	putpoint(x1, y1);
    }
    drawpath();		/* Force the path to be drawn */

    (void) setpathop(saveop);
    if( (xfirst!=x1) && (yfirst!=y1) ) {
        fprintf(stderr,"%s: warning: polygon does not close\n",progname);
    }
}
