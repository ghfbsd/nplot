
/*LINTLIBRARY*/
#include <stdio.h>

/* sputh
 * Puts a half-word (short) (16 bits) to a stream the way the Suns
 * store it
 * (i.e.  high byte, low byte, instead of the other way around
 *
 * vputh
 * Puts a half-word (short) (16 bits) to a stream the way
 * the VAX stores it (low byte, high byte)
 */

sputh(w, iop)
    int w;
    register FILE *iop;
{
    short temp=w;

    putc((temp >> 8) & 0xFF, iop);
    putc(temp & 0xFF, iop);
    return(ferror(iop));
}

vputh(w, iop)
    int w;
    register FILE *iop;
{
    short temp=w;

    putc((unsigned char)(temp & 0xFF), iop);
    putc((unsigned char)((temp >> 8) & 0xFF), iop);
    return(ferror(iop));
}

