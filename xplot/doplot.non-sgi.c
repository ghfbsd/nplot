#include	<stdio.h>
#include	<ctype.h>
#include	<strings.h>
#include	"xplot.h"

void buttonhandler(w,client_data,event)
Widget w;
caddr_t client_data;
XEvent *event;
{
	closepl();
	exit(0);
}

#define LINE 512
char buf[LINE];
char cptr[LINE];
char *curp;

int len = 0;
int endinp;

doplot(client_data,fds,id)
   caddr_t client_data;
   int *fds; /* None of these arguments are actually used... */
   int *id;
{
   char c;

   while(readit(instrm)) {
      while((len > 0) && (!endinp)){
	 len--;
	 c = *curp++;
	 switch(c) {
	 case 'm':	/* move */
	     xold = vgeth(stdin)*xscale;
	     yold = vgeth(stdin)*yscale;
	     needmove = 1;
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
	     if (plotted) {
		 erase();
	     }
	     plotted = 0;
	     break;
	 case 'f':	/* fatbase */
	     setfat(vgeth(stdin)+fatbase);
	     break;
	 case 'w':	/* window */
	     clipreg();
	     break;
	 case 'x':	/* display x-hairs */
	     break;
	 case 'p':	/* purge buffers */
	     (void) XFlush(dpy);
	     break;
	 case 'n':	/* no op */
	     break;
	 case 'a':	/* shade polygon */
	     area();
	     plotted=1;
	     break;
	 case 'A':	/* shade polygon with pattern */
	 case 's':	/* This is unsupported */
	     dopat();
	     plotted=1;
	     break;
	 case 'o':	/* change OR mode */
	     /* ormode(); This is impossible on the LaserWriter */
	     break;
	 case 'c':	/* Shade a Circle */
	     circshad();
	     plotted=1;
	     break;
	 case 'l':	/* draw a line */
	     line();
	     plotted=1;
	     break;
	 case 'C':	/* Set the color */
	     color();
	     break;
	 case 'r':	/* raster data - obsolete feature */
	 default:	 /* error */
	     {  char error_msg[64];
		sprintf(error_msg,"invalid plot command %c.",c);
		error(error_msg);
	     }
	 }
      }
   }
   drawpath();
   if (errno != EWOULDBLOCK) {
       if (fork()) exit(0);
       XtRemoveInput(inid);
	  XtAddEventHandler(toplevel,ButtonPressMask,False,
			 (XtEventHandler)buttonhandler, NULL);
   } 
   XClearArea(dpy,widgetwin,0,0,0,0,True);
}

int vgeth(strm)
/* Return next halfword from stream which is encoded in VAX byte order */
   FILE *strm;
{
   short x;
   (void) getcoords(&x,1,strm);
   return x;
}

int vgetw(strm)
/* Return next word from stream which is encoded in VAX byte order */
   FILE *strm;
{
   short x[2];
   (void) getcoords(x,2,strm);
   return (x[1]<<16) | x[0];
}

int getcoords(x,n,strm)
short *x;
int n;
FILE *strm;
{
	unsigned char c,d;
	int i;

	while (len < 2*n) {
		len++;
		curp--;
		if (!readit(strm))
			return 0;
		len--;
		curp++;
	}
	len -= 2 * n;
	for (i=0;i< n;i++){
		c = *curp++;
		d = *curp++;
		x[i] = (short) ((d << 8) | c);
/*		printf(" %d",x[i]); */
	}
/*	printf("\n"); */
	return 1;
}

int getstring(strm)
   FILE *strm;
{
	char c;
	int lenp;

	if (len <= 0) {
	   if (!readit(strm))
	      return -1;
	}
	c = *curp; len--; curp++;
	return c;
}

/* Have to do my own buffering: */
#define WAITINTERVAL 50000

int readit(strm)
FILE *strm;
{
	int lenp;

	endinp = 0;
	if (len > 0)
		bcopy(curp, cptr, len);
	if ((lenp = fread(cptr+len, 1, LINE - len, strm)) == 0){
		usleep(WAITINTERVAL); /* Wait in case stuff is on the way */
		if ((lenp = fread(cptr+len, 1, LINE - len, strm)) == 0){
			endinp = 1;
			return 0;
		}
	}
	len += lenp;
	curp = cptr;
	return 1;
}
