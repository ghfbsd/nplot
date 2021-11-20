
/*LINTLIBRARY*/
#include <stdio.h>

/* sputw
 * Puts a (long) word (32 bits) to a stream the way Suns store it
 * (i.e.  high byte, ..., low byte, instead of the other way around
 *
 * vputw
 * Puts a long-word (long) (32 bits) to a stream the way
 * the VAX stores it (low byte, ... high byte)
 *
 * These routines make the assumption that an <int> and a <long>
 * are the same size
 */

sputw(w, iop)
    int w;
    register FILE *iop;
{
    register char *p;
    register int i;

    putc((unsigned char)((w >> 24) & 0xFF), iop);
    putc((unsigned char)((w >> 16) & 0xFF), iop);
    putc((unsigned char)((w >> 8) & 0xFF), iop);
    putc((unsigned char)(w & 0xFF), iop);
    return(ferror(iop));
}

vputw(w, iop)
    int w;
    register FILE *iop;
{
    putc((unsigned char)(w & 0xFF), iop);
    putc((unsigned char)((w >> 8) & 0xFF), iop);
    putc((unsigned char)((w >> 16) & 0xFF), iop);
    putc((unsigned char)((w >> 24) & 0xFF), iop);
    return(ferror(iop));
}

