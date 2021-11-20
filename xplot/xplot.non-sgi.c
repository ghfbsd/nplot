#include "xplot.h"
#include <X11/Xaw/Label.h>

int tflag = 0;
int rwidth, rheight;


/* This is supposed to set up the X window stuff, by creating a
   widget, in which the image is to be put in a background pixmap. */

main(argc,argv)
int argc;
char **argv;
{
	int i, dimset = 0, narg, ppi = 85, x, y;
	char *ptr, *file = NULL;
	Widget child;
	Arg wargs[20];
	void doplot();
	XFontStruct *font;
	XGCValues gcv;

	struct sgttyb gp;
	short old;
	int fd;
	int argf;

	progname = argv[0];

	rwidth = pxmax = 11.0*ppi; rheight = pymax = 8.5*ppi;
	xmin = ymin = 0; xmax = pxmax; ymax = pymax;

	toplevel = XtInitialize(argv[0], "xplot",NULL, 0, &argc, argv); 
	dpy = XtDisplay(toplevel);

	for(i=1; i<argc; i++) {
	   /* Parse remaining args for parameters */
	   ptr = argv[i];
	   if( is_scale(ptr) )
	      continue;
	   if (*ptr == '-') {
	      switch(*++ptr) {
		 case 'f': /* -f -- increase fatbase */
		    fatbase = atoi(++ptr);
		    break;
		 case 'v': /* -v -- vertical orientation */
		    xmax = pymax; ymax = pxmax;
		    rwidth = pymax; rheight = pxmax;
		    {int swap = pxmax; pxmax = pymax; pymax = swap;}
		    break;
		 case 't': /* -t -- Use built-in fonts */
		    tflag = 1;
		    break;
		 default:
		    {  char error_msg[64];
		       sprintf(error_msg, "Unknown option -%s.", ptr);
		       error(error_msg);
		    }
	      }
	   } else {
	      /* Must be file name to use */
	      file = ptr;
	   }
        }

	/* Open file as standard input and then condition it to work
	   unbuffered */
	if (file != NULL) 
	   if(freopen(file,"r", stdin) == NULL) {
	      (void) fflush(stdout);
	      fprintf(stderr,"%s:  cannot open %s\n",progname,file);
	      exit(1);
	   }

        instrm = stdin;
	fd = fileno(instrm);

	gtty(fd,&gp);
	old = gp.sg_flags;
	gp.sg_flags |= (CBREAK);
	stty(fd,&gp);

	fcntl(fd,F_GETFL,argf);
	fcntl(fd,F_SETFL, argf|O_NDELAY);


	/* Parse and set up window geometry */
	if (ptr = XGetDefault(dpy,"xplot","geometry")){
	   /* Got geometry string, parse it */
	   int flags,x_w,y_w;
	   flags = XParseGeometry(ptr,&x,&y,&x_w,&y_w);
	   if (flags & WidthValue) rwidth = x_w;
	   if (flags & HeightValue) rheight = y_w;
	   dimset |= (flags & (WidthValue|HeightValue));
	   if (flags & XValue) {
	      if (XNegative & flags)
		 x += DisplayWidth(dpy,XDefaultScreen(dpy)) - rwidth;
	      dimset |= XValue;
	   }
	   if (flags & YValue) {
	      if (YNegative & flags)
		 y += DisplayHeight(dpy,XDefaultScreen(dpy)) - rheight;
	      dimset |= YValue;
	   }
	}

	pixmap = XCreatePixmap(dpy,DefaultRootWindow(dpy), rwidth, rheight,
		DefaultDepth(dpy, DefaultScreen(dpy)));

	narg = 0;
	XtSetArg(wargs[narg], XtNbitmap, pixmap); narg++;
	XtSetArg(wargs[narg], XtNinternalWidth, 0); narg++;
	XtSetArg(wargs[narg], XtNinternalHeight, 0); narg++;
        XtSetArg(wargs[narg], XtNheight, rheight); narg++;
        XtSetArg(wargs[narg], XtNwidth, rwidth); narg++;
	if (dimset & (XValue|YValue)) {
	   XtSetArg(wargs[narg], XtNx, x); narg++;
	   XtSetArg(wargs[narg], XtNy, y); narg++;
	} 

	child = XtCreateManagedWidget(progname, labelWidgetClass,
						toplevel, wargs, narg);
	XtRealizeWidget(toplevel);
	widgetwin = XtWindow(child);

	narg = 0;
	XtSetArg(wargs[narg], XtNbackground, &bg); narg++;
	XtSetArg(wargs[narg], XtNforeground, &fg); narg++;
	XtSetArg(wargs[narg], XtNfont, &font); narg++;
	XtGetValues(child, wargs, narg);

	gcv.foreground = fg;
	gcv.background = bg;
	gcv.font = font->fid;
	gc = XCreateGC(dpy, pixmap, GCForeground | GCBackground | GCFont,
			 &gcv);
	
	erase();

	inid = XtAddInput(fileno(instrm), XtInputReadMask, doplot, NULL);
	XtMainLoop();

	/* Returned -- reset standard input */
	fcntl(fd,F_SETFL,argf);
	gp.sg_flags = old;
	stty(fd,&gp);
}

closepl()
{
	XFreeGC(dpy,gc);
	XFreePixmap(dpy,pixmap);
	XtDestroyWidget(toplevel);
}

error(s)
register char *s;
{
   (void) fflush(stdout);
   fprintf(stderr,"%s: %s\n",progname,s);
   exit(1);
}

/* Erase should wait for the current page to be absorbed, before creating
  a new one (presumably by erasing the pixmap and then redrawing the widget.*/
erase()
{
/* Wait for button event */
/* Clear pixmap */
	XSetForeground(dpy, gc, bg);
	XFillRectangle(dpy,pixmap,gc,0,0,rwidth,rheight);
	XSetForeground(dpy, gc, fg);
        XSetLineAttributes(dpy,gc,(unsigned)1,LineSolid,CapButt,JoinBevel);
}
