/*LINTLIBRARY*/
#include <stdio.h>

/* sgeth
 * gets a half-word (short) (16 bits) from a stream
 * the way the SUN stores it: high byte, low byte
 *
 * vgeth
 * gets a half-word (short) (16 bits) from a stream
 * the way the VAX stores it: low byte, high byte
 */

short vgeth(iop)
    register FILE *iop;
{
    short w;

    w = (unsigned char)getc(iop);
    w = w + (unsigned char)getc(iop) * 256;

    if (feof(iop))
	return(EOF);
    return(w);
}

short sgeth(iop)
    register FILE *iop;
{
    short w;

    w = (unsigned char)getc(iop);
    w = w*256 + (unsigned char)getc(iop);

    if (feof(iop))
	return(EOF);
    return(w);
}
