#include <stdio.h>
#include "xplot.h"
#include "font.h"

/* Limits to fonts in font file and interpretation of hardware fonts */
#define MAX_FONTS	30
#define HW_FONTS	20

/* Size factor for plot file character size to CGI font character size */
#define SIZE_FACTOR	1.6

extern int tflag;

void
pstr(str,orient,size)
   char *str;
   int orient;
   short size;
/* Emit the string passed as 'str' as an X string, and then zero
   it.  If the string is null, skip everything. */
{
   int l = strlen(str);
   float xbase, ybase, xup, yup;
   XPoint where;

   if (l) {
      /* Something to print, I guess. */
      where.x = xold;
      where.y = pymax - yold;

      switch(orient) {
	 case 0:
	     xbase = 1; ybase = 0;
	     xup   = 0; yup   = 1;
	     break;
	 case 1:
	     xbase =  0; ybase = 1;
	     xup   = -1; yup   = 0;
	     break;
	 case 2:
	     xbase = -1; ybase = 0;
	     xup   =  0; yup   = -1;
	     break;
	 case 3:
	     xbase = 0; ybase = -1;
	     xup   = 1; yup   = 0;
	     break;
      }
      XDrawString(dpy,pixmap,gc,where.x,where.y,str,l);
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
    short size,move,moveh,movev,vmove,key;
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
    int xorigin = xold, yorigin = yold, orient;
    int fatsave=curfat;

    if( (key=vgetw(stdin)) <0)
	    error("putsym: invalid text key");

    size= (key&0x001f);
    orient= (key&0x0060)>>5;
    font = (key&0xff00) >> 8;

    /* Accumulate string and convert to font code encoding */
    i = 0;
    while ((sym = getstring(stdin)) != 0 && sym != -1)
       /* Read in the entire string, terminate with a null. */
       if (i < sizeof(string)-1) string[i++] = (char) sym;
    string[i] = '\0';
    translate(string,fstring); sym = 0; string[0] = '\0';

    /* Check if we are going to generate our own text (yick) or whether
       we're going to use one of the hardware fonts. */
    if ((font > MAX_FONTS) || (font < 0)) {
       /* Error message */
       fprintf(stderr, "%s: invalid font code (%d); set to zero.\n",
	  progname,font);
       font = 0;
    }
    if ((font >= HW_FONTS) || tflag) {
       char lstr[3];

       /* Figure out which size font to use. */
       font -= HW_FONTS;
Font_sizing:
       size = size > 0 ? size : 1;

       /* Determine horizontal and vertical spacing */
       line_space = 10;  /* Dummy value */
       interletter_spacing = 10; /* Dummy value */

       /* Grind through character text and print it out */
       while ((fc = fstring[sym++]) != fc_end_of_string) {
	  /* Zero out string's characters */
	  for (i = 0; i < sizeof(lstr); i++) lstr[i] = '\0';
	  switch (fc) {
	     case (FC) '\n':
		yorigin -= line_space;
	     case (FC) '\r':
		xold = xorigin; yold = yorigin;
		break;
	     case fc_command_d:
		yold -= line_space/2;
		break;
	     case fc_command_u:
		yold += line_space/2;
		break;
	     case (FC) '\b':
	       /* TODO: save the location of the last letter */
		xold -= interletter_spacing;
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
	     default:
		lstr[0] = (int) fc & 0x007f;
	  }
	  if (strlen(string) + strlen(lstr) > sizeof(string)-1) 
	     pstr(string,orient,size);
	  strcat(string, lstr);
       }
       pstr(string,orient,size); drawpath();
       goto cleanup;
    }

    /* Come here if using stroke characters. */

    fp = previous = getfont(font);
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
		   fp = previous; previous = fp;
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
		{  char error_msg[64];
		   sprintf(error_msg,
		      "Invalid command code in string - %d.\n",fc);
		   error(error_msg);
		}
	     }
	  } else {

	      /* Draw a character from the current font.  Font 0 is special,
		 since it is wired in. */
	      if (fp->fd_id == 0) {
		 if((char) fc != ' ') {
		    int xp, yp,
		       drop = fp->fd_char[(int) fc]->type.wire.dbit * 2;

		    symptr = fp->fd_char[(int) fc]->type.wire.stroke;
		    if (symptr == NULL) break; /* En garde for bad chars */
		    xnew = xold; ynew = yold;
		    do {
		       xyw = *symptr++;
		       xp = xold; yp = yold;
		       oadd(orient, (float) size, 
			  &xp, &yp, (xyw&0160)>>4, (xyw&07) - drop);
		       if( !(xyw&0200) ) dump(xnew,ynew,xp,yp);
		       xnew=xp; ynew=yp;
		    } while( !(xyw&010) );
		 }
		 /* Move along baseline to end of character. */
		 mov(move,0,orient);

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
       drawpath();

cleanup:
    /* Reset the fatbase if needed */
    if (fatsave != curfat)
	setfat(fatsave);
}

void
mov(hadd,vadd,orient)
short hadd,vadd,orient;
{
   switch(orient) {
      case 0:
	 xold += hadd; yold += vadd; break;
      case 1:
	 xold -= vadd; yold += hadd; break;
      case 2:
	 xold -= hadd; yold -= vadd; break;
      case 3:
	 xold += vadd; yold -= hadd; break;
   }
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
#define HFUDGE 1.15
   float xv, yv;

   xv = xval*xscale*size*HFUDGE; yv = yval*yscale*size;
   switch(orient) {
      case 0:
         (*xp) += xv; (*yp) += yv; break;
      case 1:
	 (*xp) -= yv; (*yp) += xv; break;
      case 2:
	 (*xp) -= xv; (*yp) -= yv; break;
      case 3:
	 (*xp) += yv; (*yp) -= xv; break;
   }
}
