#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "font.h"

/* minor VAX changes to eliminate warnings, S. Stein, 6/85 */
/* Implemented usage of hardware fonts, G. Helffrich 6/88 */

#define puthalf(e)		(void) sputh(e,stdout)

#define PPI			72	/* Points in an inch */
#define DPI			300	/* Resolution of LaserWriter? */

static char 
   fn_roman[] = "Times-Roman",
   fn_italic[] = "Times-Italic",
   fn_bold[] = "Times-Bold",
   fn_bolditalic[] = "Times-BoldItalic",
   fn_cour[] = "Courier", 
   fn_coub[] = "Courier-Bold", 
   fn_helv[] = "Helvetica";
static int
           fN_cour[] = {0,1,2, 3, 4, 5},
           fN_coub[] = { 6, 7},
	   fN_helv[] = { 8},
	   fs_all[] =   { 6, 7, 8, 9,10,12,14,16,18,20,22,24,28,32,36, 0},
           fN_roman[] = { 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
struct Fdesc {
   char *fn;
   int *fs, *fi;
};
static struct Fdesc fdesc[] = {
      {fn_roman, fs_all, fN_roman},
      {fn_italic, fs_all, fN_roman},
      {fn_bold, fs_all, fN_roman},
      {fn_bolditalic, fs_all, fN_roman},
      {fn_cour, fs_all, fN_cour},
      {fn_coub, fs_all, fN_coub},
      {fn_helv, fs_all, fN_helv},
};
#define FONTS		sizeof(fdesc)/sizeof(struct Fdesc)

/* Maximum fonts loadable */
#define MAX_FONTS	30
#define HW_FONTS	20

extern int curfat, xold, yold, xnew, ynew;
extern float xscale, yscale;

void
pstr(str)
   char *str;
/* Emit the string passed as 'str' as a Postscript string, and then zero
   it.  If the string is null, skip everything. */
{
   int l = strlen(str);

   if (l) {
      /* Something to print, I guess. */
      printf("(%s) show\n",str);
      str[0] = '\0';
   }
}

putsym()
/*
 * interpret characters into vectors
 */
{
    void oadd(),
       mov(),
       recalculate_sizes();
    short size,move,moveh,movev,vmove,add,drop,xp,yp,key;
    int sym, font, fontsize, fontindex,
	line_space, interletter_spacing,
	*fsptr;
    char xyw, 
       *symptr,
       string[256];
    struct fontdescr *fp, *previous;
    FC fstring[sizeof(string) * 2],
       fc;
    register int a,b,i;
    int xorigin, yorigin, orient;
    int fatsave=curfat;
    float svox, svoy, svsx, svsy;
    float state[6];

    if( (key=vgetw(stdin)) <0)
	    error("putsym: invalid text key");

    size= (key&0x001f);
    orient= (key&0x0060)>>5;
    font = (key&0xff00) >> 8;

    /* Accumulate string and convert to font code encoding */
    i = 0;
    while ((sym = getchar()) != 0 && sym != -1)
       /* Read in the entire string, terminate with a null. */
       if (i < sizeof(string)-1) string[i++] = (char) sym;
    string[i] = '\0';
    translate(string,fstring); sym = 0; string[0] = '\0';

    /* Set up for drawing characters.  For orientations other than normal,
       set up origin so that string is properly rotated. */
    drawpath();
    if (orient) {
       pusht(state);
       printf("GS %d %d translate %d rotate %d %d translate\n",
       xold, yold, 90*orient, -xold, -yold);
       tran((float)(xold),(float)(yold)); rot(90.0*orient);
       tran((float)(-xold),(float)(-yold));
    }
    xorigin = xold; yorigin = yold; 

    /* Check if we are going to generate our own text (yick) or whether
       we're going to use one of the hardware fonts. */
    if ((font > MAX_FONTS) || (font < 0)) {
       /* Error message */
       fprintf(stderr, "apen: invalid font code (%d); set to zero.\n",font);
       font = 0;
    }
    if (font >= HW_FONTS) {
       char lstr[3];

       /* Figure out which size font to use. */
       font -= HW_FONTS;
Font_sizing:
       size = size > 0 ? size : 1;
       i = (int) size*xscale*20.0/8.0;
       fsptr = fdesc[font].fs; fontsize = 1000;
       for (a = 0 ; fsptr[a] != 0; a++) {
	  b = abs(fsptr[a] - i);
	  if (b < fontsize) {
	     fontsize = b; fontindex = a;
	  }
       }

       line_space = 
	  (int) (((float) *(fdesc[font].fs + fontindex)+2)/PPI*DPI);
       interletter_spacing = *(fdesc[font].fs + fontindex)*2-1;

       printf("/%s findfont %d scalefont setfont\n", 
	  fdesc[font].fn, (int) ((float) fdesc[font].fs[fontindex]/PPI*DPI));
       printf("%d %d moveto\n", xold, yold);

       /* Grind through character text and print it out */
       while ((fc = fstring[sym++]) != fc_end_of_string) {
	  /* Zero out string's characters */
	  for (i = 0; i < sizeof(lstr); i++) lstr[i] = '\0';
	  switch (fc) {
	     case (FC) '\n':
		yorigin -= line_space;
	     case (FC) '\r':
		printf("%d %d moveto", xorigin, yorigin);
		break;
	     case fc_command_d:
		printf("0 %d rmoveto\n", -line_space/2);
		break;
	     case fc_command_u:
		printf("0 %d rmoveto\n", line_space/2);
		break;
	     case (FC) '\b':
	       /* TODO: save the location of the last letter */
		printf("%d 0 rmoveto\n", -interletter_spacing);
		break;
	     case fc_command_s:
		size = (int) fstring[sym++];
		goto Font_sizing;
	     case fc_command_sr:
		size += (int) fstring[sym++];
		goto Font_sizing;
	     case fc_command_b:
		/* Ignore character emboldening in hardware fonts */
		sym++;
		break;
	     case (FC) '(': /* Parentheses, need to escape them. */
	     case (FC) ')':
	     case (FC) '\\':
		sprintf(lstr,"\\%c", (int) fc);
		break;
	     default:
		lstr[0] = (int) fc & 0x007f;
	  }
	  if (strlen(string) + strlen(lstr) > sizeof(string)-1) pstr(string);
	  strcat(string, lstr);
       }
       pstr(string); drawpath();
       goto cleanup;
    }

    /* Come here if using stroke characters. */

    /* Don't miter lines that are in characters.  Makes them look weird.
       Fat characters are rough if line ends are not rounded, so round line
       ends are also selected. */
    puts("SJ");

    fp = getfont(font); previous = NULL;
    recalculate_sizes(fp, size, &move, &vmove);

    sym = 0;
    while((fc = fstring[sym++]) != fc_end_of_string) {
       if((int) fc < ' ') {
	  switch ((int) fc) {	/* standard carriage controls */
	     case '\b':
		mov(-move,0,orient);
		continue;
	     case '\t':
	        do mov(move, 0, orient);
		while ((abs(xold - xorigin)/move)%8);
		continue;
	     case '\n':
		yorigin -= vmove;
	     case '\r':
		xold = xorigin;
		yold = yorigin;
		continue;
	  }
	  /* Turn any unrecognized control characters into blanks. */
	  fc = (FC) ' ';
       }
       /* Check for commands in the text. */
       if ((int)fc & 0x200) {
	  /* Parse a command. */
	  switch (fc) {
	     int newfont;

	     case fc_command_f: /* Font change */
		newfont = (int) fstring[sym++];
		if (newfont > MAX_FONTS) {
		   /* Request to go to previous font */
		   if (previous != NULL) fp = previous;
		   previous = fp;
		} else {
		   previous = fp;
		   fp = getfont(newfont);
		}
		recalculate_sizes(fp, size, &move, &vmove);
		break;
	     case fc_command_u: /* Half-line up */
		mov(0,vmove/2,orient);
		break;
	     case fc_command_d: /* Half-line down */
		mov(0,-(vmove/2),orient);
		break;
	     case fc_command_b: /* Fatten or un-fatten line */
		setfat(curfat + (int) fstring[sym++]);
		break;
	     case fc_command_s: /* Absolute size change */
		fc = fstring[sym++];
		size = (int) fc > 0 ? (int) fc : 1;
		recalculate_sizes(fp, size, &move, &vmove);
		break;
	     case fc_command_sr: /* Relative size change */
		fc = fstring[sym++];
		size += (int) fc; size = size > 0 ? size : 1;
		recalculate_sizes(fp, size, &move, &vmove);
		break;
	     default:
		fprintf(stderr, "Invalid command code: %d.\n", fc);
		exit(1);
	     }
	  } else {

	      /* Draw a character from the current font.  Font 0 is special,
		 since it is wired in. */
	      if (fp->fd_id == 0) {
		 if((char) fc != ' ') {
		    symptr = fp->fd_char[(int) fc]->type.wire.stroke;
		    if (symptr == NULL) break; /* En garde for bad chars */
		    drop = fp->fd_char[(int) fc]->type.wire.dbit * 2;
		    xnew = xold;
		    ynew = yold;
		    do {
		       xyw = *symptr++;
		       a = (((xyw&0160)>>4)*size) * xscale;
		       b = (((xyw&07) - drop)*size) * yscale;
		       xp=xold+a; yp=yold+b;
		       if( !(xyw&0200) ) dump(xnew,ynew,xp,yp);
		       xnew=xp; ynew=yp;
		    } while( !(xyw&010) );
		 }
		 xold += move;

	     } else {
		/* Draw a Hershey font character */
		int xp = xold,
		    yp = yold,
		    i,
		    xorigin = xold,
		    yorigin = yold;
		struct character *c;
		float hsize;

		/* Offset to center of character, and move to position of
		   the first stroke. */
		c = fp->fd_char[(int)fc];
		hsize = size*fp->fd_scale;
		oadd(orient, hsize, &xorigin, &yorigin, - c->lwidth, 0);
		xold = xorigin; yold = yorigin;
		oadd(orient, hsize, &xold, &yold, c->type.strk.stroke[0],
		   fp->fd_baseline - c->type.strk.stroke[1]);
		/* Follow strokes through rest of character. */
		for (i = 2; i < 2*c->type.strk.stroke_count; i += 2)
		   if (c->type.strk.stroke[i] == STROKE_END) {
		      /* Move - pick up new coordinate following */
		      i += 2;
		      xold = xorigin; yold = yorigin;
		      oadd(orient, hsize, &xold, &yold, c->type.strk.stroke[i],
			 fp->fd_baseline - c->type.strk.stroke[i+1]);
		   } else {
		      /* Draw - draw to current location */
		      xp = xorigin; yp = yorigin;
		      oadd(orient, hsize, &xp, &yp, c->type.strk.stroke[i], 
			 fp->fd_baseline - c->type.strk.stroke[i+1]);
		      dump(xold, yold, xp, yp);
		      xold = xp; yold = yp;
		   }

		/* Position at right side of character when done. */
		yold = yorigin; xold = xorigin;
		oadd(orient, hsize, &xold, &yold, c->rwidth, 0);
	     }
	  }
       }
       drawpath(); puts("RJ");

cleanup:
    /* Reset the fatbase if needed */
    if (fatsave != curfat)
	setfat(fatsave);
    if (orient) {
       puts("GR");
       popt(state);
    }
}

void
mov(hadd,vadd,orient)
short hadd,vadd,orient;
{
   xold += hadd; yold += vadd;
}

void
recalculate_sizes(fp, size, h, v)
   struct fontdescr *fp;
   short size;
   short *h, *v;
/* Recalculate the horizontal and vertical move values. */
{
   struct character *fc_blank;
   
   fc_blank = fp->fd_char[(int)' '];
   *h  = (fc_blank->rwidth - fc_blank->lwidth)*fp->fd_scale*size*xscale;
   *v = fp->fd_height*size*yscale;
}

void
oadd(orient, size, xp, yp, xval, yval)
   int orient,
      *xp, *yp,
      xval, yval;
   float size;
/* Do an oriented add of (x,y) pair to the absolute (x,y) of the plot */
{
   float xv, yv;

   xv = xval*xscale*size; yv = yval*yscale*size;
   (*xp) += xv; (*yp) += yv;
}
