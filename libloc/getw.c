/*LINTLIBRARY*/
#include <stdio.h>

/* vgetw
 * gets a long-word (long) (32 bits) from a stream
 * the way the VAX stores it: low byte, next byte, next byte, high byte
 *
 * sgetw
 * gets a long-word (long) (32 bits) from a stream
 * the way the SUN stores it: high byte, next byte, next byte, low byte
 */

short vgetw(iop)
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

short sgetw(iop)
    register FILE *iop;
{
    short w;

    w = (unsigned char)getc(iop);
    w = w*256 + (unsigned char)getc(iop);
    w = w*256 + (unsigned char)getc(iop);
    w = w*256 + (unsigned char)getc(iop);

    if (feof(iop))
	return(EOF);
    return(w);
}
