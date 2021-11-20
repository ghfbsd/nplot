/*
 * plotfile debugger, part 1 of 2
 * modified i/o and text routines for PDP->VAX conversion
 *
 * splotdb.c     Craig Bina, 1982, 1985
 */

#include	<stdio.h>
#include	<stdlib.h>	/* added stdlib.h 2021-08-29 crb */
#include	"plotdb.h"

FILE *pltin, *temp;
char *cptr;
int fgetc(), getw();

main(argc, argv)
    int argc;
    char **argv;
{
    FILE *freopen();
    int filenam = 0;

    pltin = stdin;

    do {	/* loop over arg list */
	argc--;
	argv++;
	cptr = *argv;
	if (argc && (*cptr == '-')) {
	    cptr++;
	    switch (*cptr) {
	      case 'f':
		filenam = 1;	/* list filenames */
		break;
	      default:
		fprintf(stderr, "Usage: plotdb [-f] [file] ...\n");
		exit(1);
	    }
	    continue;
	}
	if (argc)
	    if ((temp = freopen(cptr, "r", pltin)) == NULL) {
		fprintf(stderr, "plotdb: cannot open %s\n", cptr);
		continue;
	    } else {
		pltin = temp;
		if (filenam)
		    printf(FILENAME, cptr);
	    }

	doread();

    }
    while (argc);
    exit(1);
}

doread()
{
    register int c;
    int x, y, xmax, ymax, fat;
    int r, g, b;

    while ((c = getc(pltin)) != EOF) {
	switch (c) {	/* command list */
	  case 'm':	/* move */
	    x = vgeth(pltin);
	    y = vgeth(pltin);
	    printf(MOVE, x, y);
	    break;
	  case 'd':	/* draw */
	    x = vgeth(pltin);
	    y = vgeth(pltin);
	    printf(DRAW, x, y);
	    break;
	  case 't':	/* text */
	    getsym();
	    break;
	  case 'b':	/* break */
	    printf(BREAK);
	    break;
	  case 'e':	/* erase */
	    printf(ERASE);
	    break;
	  case 'f':	/* fat */
	    fat = vgeth(pltin);
	    printf(FATBASE, fat);
	    break;
	  case 'r':	/* raster data */
	    getras();
	    break;
	  case 'w':	/* window */
	    x = vgeth(pltin);
	    xmax = vgeth(pltin);
	    y = vgeth(pltin);
	    ymax = vgeth(pltin);
	    printf(WINDOW, x, xmax, y, ymax);
	    break;
	  case 'x':	/* display x-hairs */
	    printf(XHAIRS);
	    break;
	  case 'p':	/* purge pltout buffers */
	    printf(PURGE);
	    break;
	  case 'n':	/* no op */
	    printf(NOOP);
	    break;
	  case 'a':	/* shade polygon */
	    getarea();
	    break;
	  case 'A':	/* shade polygon */
	  case 's':	/* unsupported; compatability */
	    getApat();
	    break;
	  case 'c':	/* shade circle */
	    getcirc();
	    break;
	  case 'l':	/* draw line */
	    getline();
	    break;
	  case 'C':	/* Set RGB color */
	    r = getc(pltin);
	    g = getc(pltin);
	    b = getc(pltin);
	    printf(COLOR, r, g, b);
	    break;
	  default:	/* error */
	    printf(ERROR, c);
	}
    }
}

getarea()
{
    int xfirst, yfirst, x, y;
    int npts, shade, xmask, ymask;
    register int i;

    shade = vgeth(pltin);
    xmask = (shade & 0377);
    ymask = (shade >> 8) & 0377;
    if (ymask == 0)
	ymask = xmask;
    npts = vgeth(pltin);
    printf(AREAKEY, xmask, ymask, npts);
    /*
     * Both masks equal to zero is legal now; it means outline the polygon
     * if(xmask == 0) printf(AREAWARN);
     */
    if (shade && !xmask)
	printf(AREAWARN);
    x = xfirst = vgeth(pltin);
    y = yfirst = vgeth(pltin);
    printf(AREAPOINT, x, y);
    for (i = 1; i <= npts; i++) {
	x = vgeth(pltin);
	y = vgeth(pltin);
	printf(AREAPOINT, x, y);
    }
    if ((xfirst != x) || (yfirst != y))
	printf(AREAERROR);
}

getline()
{
    int x, y, npts;
    register int i;

    npts = vgeth(pltin);
    printf(LINE, npts);
    for (i = 0; i < npts; i++) {
	x = vgeth(pltin);
	y = vgeth(pltin);
	printf(POINT, x, y);
    }
}

getApat()
{
    int xfirst, yfirst, x, y;
    int npts, patnum;
    register int i;

    patnum = vgeth(pltin);
    npts = vgeth(pltin);
    printf(APATKEY, patnum, npts);
    x = xfirst = vgeth(pltin);
    y = yfirst = vgeth(pltin);
    printf(AREAPOINT, x, y);
    for (i = 1; i <= npts; i++) {
	x = vgeth(pltin);
	y = vgeth(pltin);
	printf(AREAPOINT, x, y);
    }
    if ((xfirst != x) || (yfirst != y))
	printf(AREAERROR);
}

getcirc()
{
    int x, y, rad, shade, xmask, ymask;

    shade = vgeth(pltin);
    xmask = (shade & 0377);
    ymask = (shade >> 8) & 0377;
    if (ymask == 0)
	ymask = xmask;
    /*
     * Both masks equal to zero is legal now; it means outline the polygon
     * if(xmask == 0) printf(AREAWARN);
     */
    if (shade && !xmask)
	printf(AREAWARN);

    x = vgeth(pltin);
    y = vgeth(pltin);
    rad = vgeth(pltin);
    printf(CIRCLE, xmask, ymask, x, y, rad);
}

getsym()
{
    register int c;
    char *outstr, hold[10];
    short font, size, orient;
    int key;

    if ((key = vgetw(pltin)) < 0) {
	printf(TEXTERROR, key);
	key = 0;
    }

    size = (key & 0x001f);
    orient = (key & 0x0060) >> 5;
    font = (key & 0xff00) >> 8;
    printf(TEXTKEY, size, orient, font);
    while (1) {
	c = fgetc(pltin);
	switch (c) {
	  case -1:		/* Nonportable and probably wrong */
	    sprintf(hold, TEXTNUM, 0377);
	    printf(TEXTOUT, hold);
	    printf(TEXTEND);
	    return;
	  case 000:
	    sprintf(hold, TEXTNUM, 0);
	    printf(TEXTOUT, hold);
	    printf(TEXTEND);
	    return;
	  case 010:
	    outstr = "\\b";
	    break;
	  case 011:
	    outstr = "\\t";
	    break;
	  case 012:
	    outstr = "\\n";
	    break;
	  case 015:
	    outstr = "\\r";
	    break;
	  case 033:
	    if (((c = fgetc(pltin)) == 0) || (c == -1)) {
		sprintf(hold, TEXTNESC, (c == 0) ? 0 : 0377);
		printf(TEXTOUT, hold);
		printf(TEXTEND);
		return;
	    }
	    if (c <= 040)
		sprintf(hold, TEXTNESC, c);
	    else
		sprintf(hold, TEXTCESC, c);
	    outstr = hold;
	    break;
	  default:
	    if (c <= 040)
		sprintf(hold, TEXTNUM, c);
	    else
		sprintf(hold, TEXTCHAR, c);
	    outstr = hold;
	}
	printf(TEXTOUT, outstr);
    }
}

getras()
{
    int w, count, offset;

    printf(RASTHEAD);
    while (w = vgeth(pltin)) {
	count = (w & 0377);
	offset = (w >> 8) & 0377;
	printf(RASTKEY, count, offset);
	if (w == -1) {
	    printf(RASTEND);
	    return;
	}
	while (count-- > 0) {
	    w = vgeth(pltin);
	    printf(RASTWORD, w);
	}
    }
}

egetx(s, func)
    FILE *s;
    int (*func) ();

{
    int r;

    r = (*func) (s);
    if (feof(s)) {
	fprintf(stderr, "plotdb:  premature EOF:  %s\n", cptr);
	exit(1);
    }
    return (r);
}

edgetx(s, func)
    FILE *s;
    int (*func) ();

{
    int r;

    r = (*func) (s);
    if (feof(s)) {
	fprintf(stderr, "plotdb:  premature EOF:  %s\n", cptr);
	exit(1);
    }
    r += 256 * (*func) (s);
    if (feof(s)) {
	fprintf(stderr, "plotdb:  premature EOF:  %s\n", cptr);
	exit(1);
    }
    return (r);
}
