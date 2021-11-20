/*   Routines to parse PostScript stream and classify tokens in it.
     Code hacked from xpdf by Derek B. Noonburg,
     //
     // Lexer.cc
     //
     // Copyright 1996 Derek B. Noonburg
     //

     Retro-hacked back into C by G. Helffrich 22 Dec. 1998
*/

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
typedef unsigned char GBool;
#define gTrue 1
#define gFalse 0
#include "pic.h"

/*------------------------------------------------------------------------

// A '1' in this array means the corresponding character ends a name
// or command.
*/
static char endOfNameChars[128] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,   /* 0x */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* 1x */
  1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,   /* 2x */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,   /* 3x */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* 4x */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0,   /* 5x */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* 6x */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0    /* 7x */
};

/* Static buffer for use */
#define BUFLEN 1024
char buf[BUFLEN];
char *bufp;
int bufl;
FILE *bufs;
char tokBuf[BUFLEN];
#define tokBufSize sizeof(tokBuf)

void getInit(string, stream)
  char *string;
  FILE *stream;
/*Set up string in buffer, and its length.
  Following routines pick up stuff from this buffer, and add to it with
  new input from the specified stream.
*/
{  bufp = buf; bufs = stream;
  (void)strcpy(buf,string); bufl = strlen(buf);
}

char *getBuf(len)
   int *len;
/*Called to get buffer pointer and length after interesting items parsed.
*/
{ *len = bufp-buf;
  return buf;
}

int getChar() {
  int c;

  do {
    if (bufl > 0) {
      c = *bufp++; bufl--;
    } else {
      int remlen = BUFLEN-strlen(buf);
      if (remlen > 0) {
	if (NULL == fgets(buf+strlen(buf),remlen,bufs)) {
	  c = EOF; break;
	}
	bufl = strlen(bufp);
	continue;
      }
      sprintf(buf,"Can't expand PostScript buffer beyond %d chars.\n",
	BUFLEN);
      error(buf);
    }
  } while (0);
  return c;
}

int lookChar() {
  return (bufl>0 ? *bufp : EOF);
}

Object getObj() {
  char *p;
  int c, c2;
  GBool comment, neg, done;
  int numParen;
  int xi;
  double xf, scale;
  char *s;
  int n, m;
  Object obj;

  /* skip whitespace and comments */
  comment = gFalse;
  while (1) {
    if ((c = getChar()) == EOF)
      return EOF;
    if (comment) {
      if (c == '\r' || c == '\n')
	comment = gFalse;
    } else if (c == '%') {
      comment = gTrue;
    } else if (!isspace(c)) {
      break;
    }
  }

  /* start reading token */
  switch (c) {

  /* number */
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
  case '-': case '.':
    neg = gFalse;
    xi = 0;
    if (c == '-') {
      neg = gTrue;
    } else if (c == '.') {
      goto doReal;
    } else {
      xi = c - '0';
    }
    while (1) {
      c = lookChar();
      if (isdigit(c)) {
	getChar();
	xi = xi * 10 + (c - '0');
      } else if (c == '.') {
	getChar();
	goto doReal;
      } else {
	break;
      }
    }
    if (neg)
      xi = -xi;
    obj = oInt;
    break;
  doReal:
    xf = xi;
    scale = 0.1;
    while (1) {
      c = lookChar();
      if (!isdigit(c))
	break;
      getChar();
      xf = xf + scale * (c - '0');
      scale *= 0.1;
    }
    if (neg)
      xf = -xf;
    obj = oReal;
    break;

  /* string */
  case '(':
    p = tokBuf;
    n = 0;
    numParen = 1;
    done = gFalse;
    s = NULL;
    do {
      c2 = EOF;
      switch (c = getChar()) {

      case EOF:
      case '\r':
      case '\n':
	error("Unterminated string found in PostScript");
	done = gTrue;
	break;

      case '(':
	++numParen;
	break;

      case ')':
	if (--numParen == 0)
	  done = gTrue;
	break;

      case '\\':
	switch (c = getChar()) {
	case 'n':
	  c2 = '\n';
	  break;
	case 'r':
	  c2 = '\r';
	  break;
	case 't':
	  c2 = '\t';
	  break;
	case 'b':
	  c2 = '\b';
	  break;
	case 'f':
	  c2 = '\f';
	  break;
	case '\\':
	case '(':
	case ')':
	  c2 = c;
	  break;
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	  c2 = c - '0';
	  c = lookChar();
	  if (c >= '0' && c <= '7') {
	    getChar();
	    c2 = (c2 << 3) + (c - '0');
	    c = lookChar();
	    if (c >= '0' && c <= '7') {
	      getChar();
	      c2 = (c2 << 3) + (c - '0');
	    }
	  }
	  break;
	case '\r':
	  c = lookChar();
	  if (c == '\n')
	    getChar();
	  break;
	case '\n':
	  break;
	case EOF:
	  error("Unterminated string in PostScript");
	  done = gTrue;
	  break;
	default:
	  c2 = c;
	  break;
	}
	break;

      default:
	c2 = c;
	break;
      }

      if (c2 != EOF) {
	if (n == tokBufSize) {
	  if (!s)
	    s = tokBuf;
	  else
	    s = tokBuf; /* would catenate here if wanted string */
	  p = tokBuf;
	  n = 0;
	}
	*p++ = (char)c2;
	++n;
      }
    } while (!done);
    if (!s)
      s = tokBuf;
    else
      s = tokBuf; /* would catenate here if wanted string */
    obj = oString;
    break;

  /* name */
  case '/':
    p = tokBuf;
    n = 0;
    while ((c = lookChar()) != EOF && !(c < 128 && endOfNameChars[c])) {
      getChar();
      if (c == '#') {
	c2 = lookChar();
	if (c2 >= '0' && c2 <= '9')
	  c = c2 - '0';
	else if (c2 >= 'A' && c2 <= 'F')
	  c = c2 - 'A' + 10;
	else if (c2 >= 'a' && c2 <= 'f')
	  c = c2 - 'a' + 10;
	else
	  goto notEscChar;
	getChar();
	c <<= 4;
	c2 = getChar();
	if (c2 >= '0' && c2 <= '9')
	  c += c2 - '0';
	else if (c2 >= 'A' && c2 <= 'F')
	  c += c2 - 'A' + 10;
	else if (c2 >= 'a' && c2 <= 'f')
	  c += c2 - 'a' + 10;
	else
	  error("Illegal digit in hex char in name in PostScript");
      }
     notEscChar:
      if (++n == tokBufSize) {
	error("Name token too long in PostScript");
	break;
      }
      *p++ = c;
    }
    *p = '\0';
    obj = oName;
    break;

  /* array punctuation */
  case '[':
  case ']':
    tokBuf[0] = c;
    tokBuf[1] = '\0';
    obj = oArray;
    break;

  /* hex string or dict punctuation */
  case '<':
    c = lookChar();

    /* dict punctuation */
    if (c == '<') {
      getChar();
      tokBuf[0] = tokBuf[1] = '<';
      tokBuf[2] = '\0';
      obj = oDict;

    /* hex string */
    } else {
      p = tokBuf;
      m = n = 0;
      c2 = 0;
      s = NULL;
      while (1) {
	c = getChar();
	if (c == '>') {
	  break;
	} else if (c == EOF) {
	  error("Unterminated hex string in PostScript");
	  break;
	} else if (!isspace(c)) {
	  c2 = c2 << 4;
	  if (c >= '0' && c <= '9')
	    c2 += c - '0';
	  else if (c >= 'A' && c <= 'F')
	    c2 += c - 'A' + 10;
	  else if (c >= 'a' && c <= 'f')
	    c2 += c - 'a' + 10;
	  else {
	    sprintf(buf,
	       "Illegal character <%02x> in hex string in PostScript", c);
	    error(buf);
	  }
	  if (++m == 2) {
	    if (n == tokBufSize) {
	      if (!s)
		s = tokBuf;
	      else
		s = tokBuf; /* would append if interested in string here */
	      p = tokBuf;
	      n = 0;
	    }
	    *p++ = (char)c2;
	    ++n;
	    c2 = 0;
	    m = 0;
	  }
	}
      }
      if (!s)
	s = tokBuf;
      else
	s = tokBuf; /* would append if interested in string here */
      if (m == 1)
	s = tokBuf; /* would append ((char)(c2 << 4)) if interested in string
	               here */
      obj = oHex;
    }
    break;

  /* dict punctuation */
  case '>':
    c = lookChar();
    if (c == '>') {
      getChar();
      tokBuf[0] = tokBuf[1] = '>';
      tokBuf[2] = '\0';
    } else {
      error("Illegal character '>' in PostScript");
    }
    obj = oDict;
    break;

  /* error */
  case ')':
  case '{':
  case '}':
    sprintf(buf,"Illegal character '%c' in PostScript", c);
    error(buf);
    break;

  /* command */
  default:
    p = tokBuf;
    *p++ = c;
    n = 1;
    while ((c = lookChar()) != EOF && !(c < 128 && endOfNameChars[c])) {
      getChar();
      if (++n == tokBufSize) {
	error("Command token too long in PostScript");
	break;
      }
      *p++ = c;
    }
    *p = '\0';
    if (tokBuf[0] == 't' && !strcmp(tokBuf, "true"))
      obj = oBool;
    else if (tokBuf[0] == 'f' && !strcmp(tokBuf, "false"))
      obj = oBool;
    else if (tokBuf[0] == 'n' && !strcmp(tokBuf, "null"))
      obj = oNull;
    else
      obj = oCmd;
    break;
  }

  return obj;
}
