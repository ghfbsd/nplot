/* THIS CODE IS DISGUSTING */

#include <stdio.h>
#include "xplot.h"

#define code(x,y) (x<xmin?1:(x>xmax?2:0))|(y<ymin?4:(y>ymax?8:0))

void
clipreg()
   /* Parse the clip region */
{
    xmin=(vgeth(stdin))*xscale;
    if(xmin<0) xmin=0;
    xmax=(vgeth(stdin))*xscale;
    if(xmax>pxmax) xmax=pxmax;
    ymin=(vgeth(stdin))*yscale;
    if(ymin<0) ymin=0;
    ymax=(vgeth(stdin))*yscale;
    if(ymax>pymax) ymax=pymax;
}

clip(x1,y1,x2,y2)	/* window the plot */
int *x1,*y1,*x2,*y2;
   {
	register int c1,c2,temp;
	int swap;
	c1=code(*x1,*y1);
	c2=code(*x2,*y2);
	swap=0;
	if(!(c1||c2)) return(0); /* line completely in bounds */
	while(c1|c2)
	   {
		if (c1&c2) return(1);  /* line completely out of bounds */
		if (!c1) {	/* interchange endpoints */
		    temp= *x1;*x1= *x2;*x2=temp;
		    temp= *y1;*y1= *y2;*y2=temp;
		    temp=c1;c1=c2;c2=temp;
		    swap= ~swap;
		}
		if(c1<4) {	/* move endpoint in x */
		    temp=(c1&2?xmax:xmin);
		    *y1= solve(temp,*x1,*y1,*x2,*y2);
		    *x1=temp;
		} else {		/* move endpoint in y */
		    temp=(c1&8?ymax:ymin);
		    *x1= solve(temp,*y1,*x1,*y2,*x2);
		    *y1=temp;
		}
		c1=code(*x1,*y1);
	   }
	if( swap ) {	/* put endpoints in order */
	    temp= *x1; *x1= *x2; *x2=temp;
	    temp= *y1; *y1= *y2; *y2=temp;
	}
	return(0);
   }


solve(pnot,p1,q1,p2,q2) /* solve linear eqn in integers */
    int pnot,p1,q1,p2,q2;
{
    register int pmid,qmid;
    if(p1>p2) {
	pmid=p1;p1=p2;p2=pmid;
	qmid=q1;q1=q2;q2=qmid;
    }
    if(pnot<=p1) return(q1);
    if(pnot>=p2) return(q2);
    while(1) {  /* iterate until convergence */
	pmid= (p1+p2)>>1;
	qmid= (q1+q2)>>1;
	if(pmid<pnot) {
	    p1=pmid;
	    q1=qmid;
	} else if(pmid>pnot) {
	    p2=pmid;
	    q2=qmid;
	} else
	    return(qmid);
    }
}

