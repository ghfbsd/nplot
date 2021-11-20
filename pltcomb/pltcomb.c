#include	<stdio.h>
#include	<stdlib.h>
#include	<strings.h>
#define TSIZE	1024
#define LMAX	100
#define ENDRAS	-1
#define DPI	300
#define CPI     2.54

/* Program to combine a series of plot files into a single plot.  Each
   plot file is taken to be a separate graph, and is put in a location
   on the page as specified, with a scale factor also as specified.

   Arguments to the command are a set of scale factor/location 
      description and a file name.  If the file name is missing, then it
      is taken to be the standard input.

      Scale factors and locations:
	 -t{rvt}  -  Type of plot.  Can be r, v, or t.  Don't know what
		     this is really good for.
	 -x{value}{uci} - offset bottom left of diagram by this value
		     of x.  A scale factor of i (inches), c (cm) or
		     u (basic units==dots per inch) can be appended.  If
		     no units, then dots per inch is assumed.
	 -y{value}{uci} - offset bottom left by this value of y.  Similar
		     in scale factors to -x.
	 -f{value} - Fatten all lines by this multiplicative factor.  Should
		     be an even number for best results.
	 -s{value} - For text, the size of the text to be printed.
	 -o{value} - For text, the orientation of text to be printed.
		     0 - horizontal, left->right;
		     1 - vertical, bottom->top;
		     2 - horizontal, upside down;
		     3 - vertical, top->bottom;
	 -{value} -  Scale factor to apply to the coordinates of the plot.  Plot
		     will be expanded or shrunk accordingly.
	 -{value}x - Scale factor for x-direction only.
	 -{value}y - Scale factor for y-direction only.
*/

/* Due to an historical botch in the definition of the pltfil(5) format,
   halfwords and fullwords were written in Vax format, with the byte writing
   order from the least significant to the most significant.  Anything written
   with a halfword or fullword structure is, therefore, reversed, and this must
   be maintained for compatibility's sake.  Similar treatment for puth, below.
   vxxw() and vxxh() are defined in the local libraries.
		G. Helffrich / Dec. 27'87
*/
#define geth(i)		vgeth(i)
#define getw(i)		vgetw(i)
#define puth(v,i)	vputh(v,i)
#define putw(v,i)	vputw(v,i)

static short f; /* Fatbase for adding to each file */

int main(argc,argv)
short argc; char **argv;
{
 FILE *fopen(), *pin, *pout, *input;
 char line[LMAX], *lp, *filenam[50], *fp;
 char *fgets(), type, sc, str[2];
 char *textptr;
 short x, y, s, o;
 float xscale, yscale, xfloat, yfloat, sfloat, ofloat, ffloat;
 short fileflag;
 long ftell();
 void putfat(), move();

 pout= stdout;
 x= y= 0; xscale= yscale= 1.0; o= 0; s= 2; type = 'v'; f = 0;
 if( argc < 2 ) {
	fprintf(stderr,
	   "usage: %s {-s# | -x# | -y# | -f# | -# } {'text' | file | -}\n", 
	   *argv);
	exit(1);
	};
 while( --argc ) {
	 argv++;
	 if ((strcmp(*argv,"-") == 0) || ((*argv)[0] != '-')) {
	    /* First character isn't dash.  Might be a file
	       name or a text label to be placed on the page.  If a label,
	       the first character is a quote or prime. */
	    if ((*argv)[0] == '"') {
	       /* Text to be displayed.  Move to coordinate and write it */
	       putfat(f,pout);
	       move(x,y,pout);
	       putc('t',pout);
	       putw( (s+(o<<5)),pout);
	       while( (*argv)++, ((*argv)[0]!='"') && ((*argv)[0]))
		  putc(**argv,pout);
	       putc(0,pout);
	       type = 'i';
	    } else {
	       if ((*argv)[0] == '-')
		  pin= stdin;
	       else
		  if( (pin=fopen(*argv,"r")) == 0) {
		  fprintf(stderr,"cannot open %s\n",*argv);
		  exit(1);
		  };
	       /* Process this plot file */
	       switch( type ) {
		  case 'v':
		     /* File of vectors */
		     putfat(f,pout);
		     putvec(x,y,xscale,yscale,pin,pout);
		     break;
		  case 't':
		     /* File of text */
		     putfat(f,pout);
		     move(x,y,pout);
		     putc('t',pout);
		     putw( (s+(o<<5)), pout);
		     transfer(pin,pout);
		     putc( 0, pout);
		     break;
		  case 'r':
		     /* File of raster data */
		     move(x,y,pout);
		     if( (ftell(pout)&01) ==0 )
			    putc('n',pout);
		     putc('r',pout);
		     transfer(pin,pout);
		     puth(ENDRAS,pout);
		     break;
		  case 'i':
		     /* Internal type, don't need to read a file */
		     break;
	       }
	    };
	    /* Reset parameters for this plot */
	    x= y= 0; xscale= yscale= 1; f= o= 0; s= 2; type = 'v';
	    continue;
	 }

	 /* Scan the next parameter */
	 sc='u';
	 if( is_scale(*argv,&xscale,&yscale) )
	    continue;
	 switch ((*argv)[1]) {
	    case 'x':
	       sscanf( &(*argv)[2], "%f%1[ciu]", &xfloat,str);
	       sc = str[0];
	       x = scale(xfloat,sc);
	       break;
	    case 'y':
	       sscanf( &(*argv)[2] ,"%f%1[ciu]", &yfloat,str);
	       sc = str[0];
	       y = scale(yfloat,sc);
	       break;
	    case 'o':
	       sscanf( &(*argv)[2] ,"%1hd", &o);
	       break;
	    case 's':
	       sscanf( &(*argv)[2] ,"%2hd", &s);
	       break;
	    case 'f':
	       sscanf( &(*argv)[2] ,"%hd", &f);
	       break;
	    case 't':
	       type = ' ';
	       sscanf( &(*argv)[2] ,"%1[vtr]", str);
	       type = str[0];
	       if (type == ' ') { 
		  fprintf(stderr,"invalid plot type- %c\n",(*argv)[2]);
		  exit(1);
	       };
	       break;
	    default:
	       fprintf(stderr,"unrecognized argument '%s'\n",*argv);
	       exit(1);
         }
      }
 fflush(pout);
}

scale(value,scale_factor)
   float value;
   char scale_factor;
{  /* Return the given value as an integer, scaled by the scale factor
      given as the argument */
   switch (scale_factor) {
      case 'u':  /* Internal units.  Don't do anything.  */
	 return value;
      case 'i':  /* Convert inches to internal units */
	 return value*DPI;
      case 'c':  /* Convert centimeters to internal units */
	 return value*DPI/CPI;
   }
}
	 
void move(x,y,pout)
register short x,y;
register FILE *pout;
{
 putc('m',pout);
 puth(x,pout);
 puth(y,pout);
}

void putfat(f,pout)
short f;
register FILE *pout;
{
 if(f<=0) return;
 putc('f',pout);
 puth(f,pout);
 return;
}

transfer(pin,pout)
register FILE *pin, *pout;
{
 register int nread;
 char tbuf[TSIZE];
 do
	{
	 nread= fread(tbuf,1,TSIZE,pin);
	 if(nread < 1) break;
	 fwrite(tbuf,1,nread,pout);
	}  while(nread == TSIZE);
}

putvec(x,y,xscale,yscale,pin,pout)
register FILE *pin, *pout;
short x,y;
float xscale, yscale;
{
 register char c;
 short header;
 short count,i;
 long ftell(), wd, tsize;

 while( (c=getc(pin)) != (char)-1 )
	{
	 switch(c)
		{
		 case 'm':
		 case 'd':
			putc(c,pout);
			puth((int) ((geth(pin)+x)*xscale),pout);
			puth((int) ((geth(pin)+y)*yscale),pout);
			break;
		 case 't':
			/* Text.  Shrink size down by average of scale
			   factors. */
			putc(c,pout);
			wd = getw(pin);
			tsize = (long) ((float) (wd & 0x0000001f) * 
						(xscale+yscale)/2.0 + 0.5);
			wd = (wd & 0xffffffe0) | 
			     (tsize < 1 ? 1 : tsize);
			putw(wd,pout);
			while( putc(getc(pin),pout) );
			break;
		 case 'c':
			putc(c,pout);
			puth(geth(pin),pout);
			puth((int) ((geth(pin)+x)*xscale),pout);
			puth((int) ((geth(pin)+y)*yscale),pout);
			puth((int) (geth(pin)*(xscale+yscale)/2.0),pout);
			break;
		 case 'C':
			putc(c,pout);
			for (i=1; i <= 3; i++) putc(getc(pin),pout);
			break;
		 case 'a':
		 case 'A':
			putc(c,pout);
			puth(geth(pin),pout);
			count = geth(pin);
			puth(count,pout);
			for(i=0; i <= count; i++) {
				puth((int) ((geth(pin)+x)*xscale),pout);
				puth((int) ((geth(pin)+y)*yscale),pout);
				}
			break;
		 case 'f':
			putc(c,pout);
			puth(f+geth(pin),pout);
			break;
		 case 'o':
			putc(c,pout);
			puth(geth(pin),pout);
			break;
		 case 'p':
		 case 'n':
		 case 'x':
			putc(c,pout);
			break;
		 case 'w':
			putc(c,pout);
			puth((int) ((geth(pin)+x)*xscale),pout);
			puth((int) ((geth(pin)+y)*yscale),pout);
			puth((int) ((geth(pin)+x)*xscale),pout);
			puth((int) ((geth(pin)+y)*yscale),pout);
			break;
		 case 'r':
			if( (ftell(pout)&01) == 0 )
				putc('n',pout);
			putc('r',pout);
			while( (header=geth(pin)) != ENDRAS )
				{
				 puth(header,pout);
				 header &= 0377;
				 while(header--)
					puth(geth(pin),pout);
				}
			puth(ENDRAS,pout);
			break;
		 case 'e':
		 case 'b':
			break;	/* eat 'e' and 'b' */
		}
	}
}

/* This procedure puts a halfword backwards into the output stream.  This
   is due to an historical botch in the definition of the pltfil(5) format.
   Halfwords and fullwords were written in Vax format, with the leading and
   trailing bytes swapped.  Anything written with a halfword or fullword
   structure is, therefore, reversed, and this must be maintained for
   compatibility's sake.  Similar treatment for puth, below.
		G. Helffrich / Dec. 27'87
*/

#include	<ctype.h>

int is_scale(cptr,x,y)
	char *cptr;
	float *x,*y;
{
	double scale, atof();
	register char *s=cptr;
	int xflag=0, yflag=0;

	if(*s++ != '-') 	/* Better start with a minus */
	   return(0);
	if( isdigit(*s) || ( *s=='.' && isdigit(s[1])) ) {
		if((scale=atof(s)) == 0.0)
			return(0);
		while(isdigit(*s) || *s=='.')
			s++;
		/* Look for trailing x or y for scaling in that dimension
		   only. */
		switch(*s) {
			case '\0':
				xflag= yflag= 1;
				break;
			case 'x':
			case 'X':
				xflag=1;
				break;
			case 'y':
			case 'Y':
				yflag=1;
				break;
			default:
				return(0);
		}
		/* If present, better be last thing in the string. */
		if ((*s != '\0') && (*++s != '\0'))
		   return(0);
		if(xflag) *x *= scale;
		if(yflag) *y *= scale;
		return(1);
	}
	return(0);
}
