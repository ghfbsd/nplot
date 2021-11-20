#include	<stdio.h>
#include	<ctype.h>

extern float xscale, yscale, xdscale, ydscale;

is_scale(cptr)
	char *cptr;
{
	double scale, atof();
	register char *s=cptr;
	int xflag=0, yflag=0;
	if(*s == '-') s++;
	if( isdigit(*s) || ( *s=='.' && isdigit(s[1])) ) {
		if((scale=atof(s)) == 0.0)
			return(0);
		while(isdigit(*s) || *s=='.')
			s++;
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
		if(xflag) xscale= xdscale*scale;
		if(yflag) yscale= ydscale*scale;
		return(1);
	}
	return(0);
}
