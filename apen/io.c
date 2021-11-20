/*LINTLIBRARY*/
#include <stdio.h>

/* vgeth
 * gets a half-word (short) (16 bits) from a stream
 * the way the VAX stores it: low byte, high byte
 */

int vgeth(iop)
    register FILE *iop;
{
    short w;

    w = (unsigned char)getc(iop);
    w = w + (unsigned char)getc(iop) * 256;

    if (feof(iop))
	return(EOF);
    return(w);
}


/* vgetw
 * gets a long-word (long) (32 bits) from a stream
 * the way the VAX stores it: low byte, next byte, next byte, high byte
 */

int vgetw(iop)
    register FILE *iop;
{
    short w;

    w = (unsigned char)getc(iop);
    w = w + (unsigned char)getc(iop) * 256;
    w = w + (unsigned char)getc(iop) * 256 * 256;
    w = w + (unsigned char)getc(iop) * 256 * 256 * 256;

    if (feof(iop))
	return(EOF);
    return(w);
}
