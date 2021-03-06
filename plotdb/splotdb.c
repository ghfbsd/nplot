/*
 * plotfile debugger, part 1 of 2
 * modified i/o and text routines for PDP->VAX conversion
 *
 * splotdb.c     Craig Bina, 1982, 1985
 */

#include	<stdio.h>
#include        <stdlib.h>	/* added stdlib.h 2021-08-29 crb */
#include	"plotdb.h"

#define egetc(s)	egetx(s,fgetc)
#define egetw(s)	edgetx(s,fgetc)
#define egetlw(s)	egetx(s,getw)

FILE *pltin,*temp;
char *cptr;
int fgetc(),getw();

main(argc,argv)
int argc; 
char **argv;
{
	FILE *freopen();
	int filenam = 0;

	pltin=stdin;

	do {	/* loop over arg list */
		argc--; 
		argv++;
		cptr= *argv;
		if(argc && (*cptr == '-')) {
			cptr++;
			switch(*cptr) {
			case 'f':
				filenam=1;	/* list filenames */
				break;
			default:
				fprintf(stderr,"Usage: plotdb [-f] [file] ...\n");
				exit(1);
			}
			continue;
		}
		if(argc)
			if((temp=freopen(cptr,"r",pltin)) == NULL) {
				fprintf(stderr,"plotdb: cannot open %s\n",cptr);
				continue;
			}
			else {
				pltin=temp;
				if(filenam) printf(FILENAME,cptr);
			}

		doread();

	} 
	while(argc);
	exit(1);	/* fixed empty argument 2021-08-28 crb */
}

doread()
{
	register int c;
	int x,y,xmax,ymax,fat;
	while((c=getc(pltin)) !=  EOF) {
		switch (c) {	/* command list */
		case 'm':		/* move */
			x=egetw(pltin);
			y=egetw(pltin);
			printf(MOVE,x,y);
			break;
		case 'd':		/* draw */
			x=egetw(pltin);
			y=egetw(pltin);
			printf(DRAW,x,y);
			break;
		case 't':		/* text */
			getsym();
			break;
		case 'b':		/* break */
			printf(BREAK);
			break;
		case 'e':		/* erase */
			printf(ERASE);
			break;
		case 'f':		/* fat */
			fat=egetw(pltin);
			printf(FATBASE,fat);
			break;
		case 'r':		/* raster data */
			getras();
			break;
		case 'w':		/* window */
			x=egetw(pltin);
			xmax=egetw(pltin);
			y=egetw(pltin);
			ymax=egetw(pltin);
			printf(WINDOW,x,xmax,y,ymax);
			break;
		case 'x':		/* display x-hairs */
			printf(XHAIRS);
			break;
		case 'p':		/* purge pltout buffers */
			printf(PURGE);
			break;
		case 'n':		/* no op */
			printf(NOOP);
			break;
		case 'a':		/* shade polygon */
			getarea();
			break;
		default: 		/* error */
			printf(ERROR,c);
		}
	}
}

getarea()
{
	int xfirst,yfirst,x,y;
	int npts,shade,xmask,ymask;
	register int i;

	shade=egetw(pltin);
	xmask=(shade&0377);
	ymask=(shade>>8)&0377;
	if(ymask == 0) ymask=xmask;
	npts=egetw(pltin);
	printf(AREAKEY,xmask,ymask,npts);
	if(xmask == 0) printf(AREAWARN);
	x=xfirst=egetw(pltin);
	y=yfirst=egetw(pltin);
	printf(AREAPOINT,x,y);
	for(i=1;i <= npts;i++) {
		x=egetw(pltin);
		y=egetw(pltin);
		printf(AREAPOINT,x,y);
	}
	if((xfirst != x) || (yfirst != y)) printf(AREAERROR);
}

getsym()
{
	char c,*outstr,hold[10];
	short size,orient;
	short key;

	if((key=egetw(pltin)) < 0) {
		printf(TEXTERROR,key);
		key=0;
	}
	size=(key&037);
	orient=(key&0140)>>5;
	printf(TEXTKEY,size,orient);
	while(1) {
		c=egetc(pltin);
		switch(c) {
		case -1:
			sprintf(hold,TEXTNUM,0377);
			printf(TEXTOUT,hold);
			printf(TEXTEND);
			return;
		case 000:
			sprintf(hold,TEXTNUM,0);
			printf(TEXTOUT,hold);
			printf(TEXTEND);
			return;
		case 010:
			outstr="\\b";
			break;
		case 011:
			outstr="\\t";
			break;
		case 012:
			outstr="\\n";
			break;
		case 015:
			outstr="\\r";
			break;
		case 033:
			if(((c=egetc(pltin)) == 0) || (c == -1)) {
				sprintf(hold,TEXTNESC,(c == 0)?0:0377);
				printf(TEXTOUT,hold);
				printf(TEXTEND);
				return;
			}
			if(c <= 040) sprintf(hold,TEXTNESC,c);
			else sprintf(hold,TEXTCESC,c);
			outstr=hold;
			break;
		default:
			if(c <= 040) sprintf(hold,TEXTNUM,c);
			else sprintf(hold,TEXTCHAR,c);
			outstr=hold;
		}
		printf(TEXTOUT,outstr);
	}
}

getras()
{
	int w,count,offset;

	printf(RASTHEAD);
	while(w=egetw(pltin)) {
		count=(w&0377);
		offset=(w>>8)&0377;
		printf(RASTKEY,count,offset);
		if(w == -1) {
			printf(RASTEND);
			return;
		}
		while(count-- > 0) {
			w=egetw(pltin);
			printf(RASTWORD,w);
		}
	}
}

egetx(s,func)
FILE *s;
int (*func)();
{
	int r;

	r=(*func)(s);
	if(feof(s)) {
		fprintf(stderr,"plotdb:  premature EOF:  %s\n",cptr);
		exit(1);
	}
	return(r);
}

edgetx(s,func)
FILE *s;
int (*func)();
{
	int r;

	r=(*func)(s);
	if(feof(s)) {
		fprintf(stderr,"plotdb:  premature EOF:  %s\n",cptr);
		exit(1);
	}

	r+=256 * (*func)(s);
	if(feof(s)) {
		fprintf(stderr,"plotdb:  premature EOF:  %s\n",cptr);
		exit(1);
	}

	return(r);
}
