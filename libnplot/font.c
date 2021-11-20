#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "font.h"
#include "chars.h"

#define MAX_FONTS 10 /* Maximum number of fonts definable */
struct fonts {			/* Information about loaded fonts */
   int f_count;
   struct fontdescr  		/* Pointer to font info */
      *f_info[MAX_FONTS];
   };

/* The default font is font 0, and it is always loadable.  The data describing
   it appears here. */

struct character f0c[MAX_CODE+1];

struct character dummy = {
   -3, 3 		/* Fixed width - 6 units. */
};

struct fontdescr font_zero = {
   0, 10, 0, 1.0,	/* Font id, height, baseline, and scale. */

   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,

   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,

   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,

   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy
};

struct fonts font_list = {
   0,
   NULL				/* Remainder are null */
};

extern char *progname;		/* Program fills this in for error messages */

/* Following table describes translation from 'greek' characters in old
   version to font codes in new version. */
char ct[] = "abcCdDefFgGhIlLmnoOpPrsStwWyY";
FC ft[] = { 
   fc_alpha, fc_beta, fc_partial, fc_grad,
   fc_delta, fc_Delta, fc_epsilon, fc_phi, 
   fc_Phi, fc_gamma, fc_Gamma, fc_eta,
   fc_integral, fc_lambda, fc_Lambda, fc_mu, 
   fc_nu, fc_theta, fc_Theta, fc_pi, 
   fc_Pi, fc_rho, fc_sigma, fc_Sigma, 
   fc_tau, fc_omega, fc_Omega, fc_psi, 
   fc_Psi
};

/*
 * scanint: a function to scan an integer, using n characters of
 *          the input file, ignoring newlines. (scanf won't work
 *          because it also ignores blanks)
 */
int     scanint (n,file)
int     n;
FILE	*file;
{
    char    buf[20];
    int     i, c;

    for (i = 0; i < n; i++)
    {
	while ((c = getc(file)) == '\n');/* discard spare newlines */
	if (c == EOF)
	    return (-1);
	buf[i] = c;
    }

    buf[i] = 0;
    return (atoi (buf));
}

char file_prefix[] = FONTFILE_PREFIX;

FILE *fontfile;

void cleanup(msg)
   char *msg;
/* Close font file if open, and print message if one is given. */
{
   if (fontfile != NULL) fclose(fontfile);
   if (msg != NULL)
      fprintf(stderr, "%s:  %s\n", progname, msg);
}

struct fontdescr *
getfont(code)
   int code;
/*  Load the font which is indicated by the value 'code'.  If the font
   is already present, fine.  Otherwise, read it in if there is a free
   slot.  The routine returns a pointer if loaded successfully, 
   NULL otherwise.
*/
{
   int i;

   /* Search in loaded fonts. */
   for (i = 0; i < font_list.f_count; i++)
      if (code == font_list.f_info[i]->fd_id)
	 return(font_list.f_info[i]);

   /* If request for font zero, then build its definition. */
   if (code == 0) {
      /* Install Roman/ASCII characters */
      for (i = ' '; i < 256; i++)
	 if (ascii.saddr[i] != 0) {
	    int ii = i - ' ';
	    struct character *c = &(f0c[i]);

	    c->lwidth = dummy.lwidth; c->rwidth = dummy.rwidth;
	    c->type.wire.stroke = ascii.svec+(ascii.saddr[ii] & 01777);
	    c->type.wire.dbit = (ascii.saddr[ii] & 02000) != 0;
	    font_zero.fd_char[i] = c;
	 };
      /* Install odd 'greek' characters */
      for (i = 0; i < sizeof(ct)-1; i++) {
	 int ii = ct[i] - ' ',
	    fi = (int) ft[i];
	 struct character *c = &(f0c[fi]);

	 c->lwidth = dummy.lwidth; c->rwidth = dummy.rwidth;
	 c->type.wire.stroke = greek.svec + (greek.saddr[ii] & 01777);
	 c->type.wire.dbit = (greek.saddr[ii] & 02000) != 0;
	 font_zero.fd_char[fi] = c;
      };
      font_list.f_info[font_list.f_count++] = &font_zero;
      return(&font_zero);
   }
      
   /* Not already loaded.  Read in from the font file. */
   {
      char filename[256], file_type_r = 'r';
      struct fontdescr *new;
      int ich;

      /* Build up font file name, and open it for reading. */
      fontfile = NULL;
      sprintf(filename,"%s%03d",file_prefix,code);
      if ((fontfile = fopen(filename, &file_type_r)) == NULL) {
	 cleanup(" Can't open font file, default font used.");
	 return(getfont(0));
      }

      new = (struct fontdescr *) malloc((unsigned) sizeof(struct fontdescr));
      if (new == NULL) {
	 cleanup(" Can't allocate font storage, default font used.");
	 return(getfont(0));
      }

      /* Read in lines of font.  First line is the height of the font.
	 Lines are encoded with the character position
	 the stroke count, and the strokes.  First stroke pair are the
	 left and right widths for the character. */
      
      fscanf(fontfile, "%d %d %f", &new->fd_height, &new->fd_baseline,
	 &new->fd_scale);
      while ((ich = scanint(5, fontfile)) >= 0) { /* changed >0 to >=0 2021-08-29 crb */
	 int i, nch, offs = (int) 'R';
	 struct character *newc;
	 short *sp;

	 nch = scanint (3, fontfile);
	 newc = (struct character *) 
	    malloc((unsigned)(sizeof(struct character) + 
			      sizeof(short)*2*(nch - 1)));
	 if (newc == NULL) {
	    cleanup("Unable to allocate font storage, default font used.");
	    return(getfont(0));
	 }

	 /* Read in strokes comprising this letter. */
	 newc->type.strk.stroke_count = nch - 1;
	 newc->lwidth = (int) getc(fontfile) - offs;
	 newc->rwidth = (int) getc(fontfile) - offs;
	 for (sp = &(newc->type.strk.stroke[0]), i = 1; i < nch; i++) {
	    if ((i == 32) || (i == 68) || (i == 104) || (i == 140))
		(void) getc(fontfile); /* skip newlines */
	    *(sp++) = (int) getc(fontfile) - offs;
	    *(sp++) = (int) getc(fontfile) - offs;
	 }
	 (void) getc(fontfile);

	 /* Put the letter into the descriptor for the font. */
	 new->fd_char[ich] = newc;
      }

      /* Put the undefined character into all the unused slots,
	 and return the new structure. */
      for (i = 0; i < MAX_CODE; i++)
	 if (new->fd_char[i] == NULL)
	    new->fd_char[i] = new->fd_char[(int)fc_undefined];
      new->fd_id = code;

      cleanup(NULL);
      font_list.f_info[font_list.f_count++] = new;
      return(new);
   }
}


/* Routines for parsing character strings and turning them into encoded
   font code strings.
*/

struct two_letter_codes {
   char *ccc;
   FC fcc;
} codes[] = {
   "em", fc_em,		"hy", (enum fontcode) ((int) '-'),
   "bu", fc_bullet,	"sq", fc_square,
   "ru", fc_rule,	"14", fc_14,
   "12", fc_12,		"34", fc_34,
   "fi", fc_fi,		"fl", fc_fl,
   "ff", fc_ff,		"Fi", fc_ffi,
   "Fl", fc_ffl,	"de", fc_degree,
   "dg", fc_dagger,	"fm", fc_footmark,
   "ct", fc_cent,	"rg", fc_registered,
   "co", fc_copyright,	"pl", (enum fontcode) ((int) '+'),
   "mi", (enum fontcode) ((int) '-'),	"eq", (enum fontcode) ((int) '='),
   "**", (enum fontcode) ((int) '*'),	"sc", fc_sectioncode,
   "aa", (enum fontcode) ((int) '\''),	"ga", (enum fontcode) ((int) '`'),
   "ul", (enum fontcode) ((int) '_'),	"sl", (enum fontcode) ((int) '/'),

   "*a", fc_alpha,	"*b", fc_beta,
   "*g", fc_gamma,	"*d", fc_delta,
   "*e", fc_epsilon,	"*z", fc_zeta,
   "*y", fc_eta,	"*h", fc_theta,
   "*i", fc_iota,	"*k", fc_kappa,
   "*l", fc_lambda,	"*m", fc_mu,
   "*n", fc_nu,		"*c", fc_xi,
   "*o", fc_omicron,	"*p", fc_pi,
   "*r", fc_rho,	"*s", fc_sigma,
   "ts", fc_endsigma,	"*t", fc_tau,
   "*u", fc_upsilon,	"*f", fc_phi,
   "*x", fc_chi,	"*q", fc_psi,
   "*w", fc_omega,	
   "*A", fc_Alpha,	"*B", fc_Beta,
   "*G", fc_Gamma,	"*D", fc_Delta,
   "*E", fc_Epsilon,	"*Z", fc_Zeta,
   "*Y", fc_Eta,	"*H", fc_Theta,
   "*I", fc_Iota,	"*K", fc_Kappa,
   "*L", fc_Lambda,	"*M", fc_Mu,
   "*N", fc_Nu,		"*C", fc_Xi,
   "*O", fc_Omicron,	"*P", fc_Pi,
   "*R", fc_Rho,	"*S", fc_Sigma,
                     	"*T", fc_Tau,
   "*U", fc_Upsilon,	"*F", fc_Phi,
   "*X", fc_Chi,	"*Q", fc_Psi,
   "*W", fc_Omega,	

   "sr", fc_root,	"rn", fc_overbar,
   ">=", fc_geq,	"<=", fc_leq,
   "==", fc_ident,	"~=", fc_apeq,
   "ap", fc_approx,	"!=", fc_neq,
   "->", fc_rarrow,	"<-", fc_larrow,
   "ua", fc_uarrow,	"da", fc_darrow,
   "mu", fc_times,	"di", fc_divides,
   "+-", fc_plusminus,	"cu", fc_cup,
   "ca", fc_cap,	"sb", fc_subset,
   "sp", fc_superset,	"ib", fc_isubset,
   "ip", fc_isuperset,  "if", fc_infinity,
   "pd", fc_partial,	"gr", fc_grad,
   "no", fc_not,	"is", fc_integral,
   "pt", fc_propto,	"es", fc_emptyset,
   "mo", fc_member,	"br", fc_vbar,
   "dd", fc_ddagger,	"rh", fc_undefined,
   "lh", fc_undefined,	"bs", fc_undefined,
   "or", fc_or,		"ci", fc_circle,

   "..", (enum fontcode) 0
};

FC
special_character(string)
   char *string;
/* Decode the two characters into a special character code.  If invalid,
   eat up two characters and return the undefined character code. */
{
   struct two_letter_codes *fc;

   /* Now search font tables to translate the two character code into 
      a font code. */
   for (fc = codes; (int) (fc->fcc) != 0; fc++)
      if (string[0] == fc->ccc[0] && string[1] == fc->ccc[1])
	 return(fc->fcc);
   return(fc_undefined);
}
   
struct encode_font {
   char id[2];
   short code;
} fontcode_array[] = {
   {"P ", 256},	/* previous font */
   {"R ", 1},	/* Roman */
   {"I ", 2},	/* Italic */
   {"B ", 3},	/* Bold */
   {"BI", 4},	/* Bold italic */
   {"GG", 5},	/* German gothic */
   {"RG", 6},	/* Romanesque gothic */
   {"EG", 7},	/* English gothic */
   {"H ", 8},   /* Helvetica */
   {"C ", 9},	/* Cyrillic */
   {"S ", 10},	/* Script */

   {"H1", 20},	/* Hardware font 1 */
   {"H2", 21},	/* Hardware font 2 */
   {"H3", 22},	/* Hardware font 3 */
   {"H4", 23},	/* Hardware font 4 */
   {"H5", 24},	/* Hardware font 5 */
   {"H1", 25},	/* Hardware font 6 */
   {"H2", 26},	/* Hardware font 7 */
   {"H3", 27},	/* Hardware font 8 */
   {"H4", 28},	/* Hardware font 9 */
   {"H5", 29},	/* Hardware font 10 */

   {"  ", 0}	/* End-of-list */
};

short
font_change(cp)
   char *cp;
{
   char id[2];
   struct encode_font *ef;

   /* Check if one letter or two letter code. */
   if (cp[1] == '(') {
      /* Two letter */
      id[0] = cp[2]; id[1] = cp[3];
   } else {
      id[0] = cp[1]; id[1] = ' ';
   }

   /* Now search font tables to translate the code into a font id. */
   for (ef = fontcode_array; ef->code != 0; ef++)
      if (!strncmp(ef->id, id, 2)) return(ef->code);
   
   return(-1);
}

int
nfont_(id,lid)
   char *id;
   int lid;
/* Return internal font code for name given by the id string. */
{

   if (lid > 1 && id[1] != ' ') {
      /* If more than one character, put a left paren on the front */
      char id_copy[4];

      sprintf(id_copy, "(%2.2s", id);
      return (font_change(id_copy-1));
   }
   return(font_change(id-1));
}

void
translate(string,len,fstring,code)
   char *string;
   int len;
   FC *fstring;
   int code;
/* Translate the character string from external representation into
   internal representation (font codes).  Characters are translated into
   individual codes.  Commands are translated into a command code, followed
   by the command. */
{
   char *cp = string,
	c,
	esc = 033;
   FC *sp = fstring,
      font;
   int l=len;

   /* Scan across characters in the string */
   while ((c = *cp)&&(l-->0)) {
      if (c == esc) {
	 /* Old-style escape character.  Turn old-style code into a new
	    one. */
	 c = *++cp; l--; if (!c) break;
	 switch (c) {
	 case 'U':  /* Full line up */
	    *sp = fc_command_u;
	 case 'u':  /* Half line up */
	    *sp = fc_command_u;
	    break;
	 case 'D':  /* Full line down */
	    *sp = fc_command_d;
	 case 'd':  /* Half line down */
	    *sp = fc_command_d;
	    break;
	 case 'b':  /* Back space */
	    *sp = (enum fontcode) (int) '\b';
	    break;
	 case 'F':  /* Fatten line */
	    *sp++ = fc_command_b; *sp = (enum fontcode) 1;
	    break;
	 case 'f':  /* Un-fatten line */
	    *sp++ = fc_command_b; *sp = (enum fontcode) -1;
	    break;
	 case 'S':  /* Increase size */
	    *sp++ = fc_command_sr; *sp = (enum fontcode) 1;
	    break;
	 case 's':  /* Increase size */
	    *sp++ = fc_command_sr; *sp = (enum fontcode) -1;
	    break;
	 case 'g':  /* Greek equivalent of next character */
	    { int i;

	       c = *++cp; --l; *sp = fc_undefined;
	       for (i=0; i < sizeof(ct); i++)
		  if (c == ct[i]) {
		     *sp = ft[i]; break;
		  }
	    }
	    break;
	 }
	 sp++;

      } else if (c == '\\') {
	 /* Escape character.  The fun begins here */
	 c = *++cp; --l;
	 switch (c) {
	    case '\\': /* Just the backslash */
	       *sp++ = (enum fontcode) ((int) c); break;
	    case '(': /* Special character */
	       *sp++ = special_character(cp+1); 
	       cp += 2; l -= 2; break;
	    case '\'': /* close quote */
	       *sp++ = (enum fontcode) ((int) '\''); break;
	    case '`': /* open quote */
	       *sp++ = (enum fontcode) ((int) '`'); break;
	    case '-': /* hyphen or minus */
	       *sp++ = (enum fontcode) ((int) '-'); break;
	    case 'n': /* newline */
	       *sp++ = (enum fontcode) ((int) '\n'); break;
	    case 'r': /* return */
	       *sp++ = (enum fontcode) ((int) '\r'); break;

	    /* Remaining things are commands. */
	    case 'f': /* \f -- font change */
	       font = (enum fontcode) font_change(cp);
	       sp[0] = fc_command_f; sp[1] = font;
	       sp += 2; l -= cp[1] == '(' ? 3 : 1;
	               cp += cp[1] == '(' ? 3 : 1;
	       break;
	    case 's': /* \s -- size change */
	       {  FC cmd = fc_command_s;
		  int size, negate = 1;

		  if (cp[1] == '+' || cp[1] == '-') {
		     /* Relative size change */
		     negate = cp[1] == '-' ? -1 : 1;
		     cmd = fc_command_sr; cp++; l--;
		  }
		  cp++; l--; /* move beyond s */
		  if (sscanf(cp,"%d",&size) == 1) {
		     /* Parse off number and move to end of it. */
		     sp[0] = cmd; sp[1] = (FC) (negate*size); sp += 2;
		     while(*cp >= '0' && *cp <= '9') l--, cp++; l++, cp--;
		  } else
		     *sp++ = fc_undefined;
	       }
	       break;
	    case 'u': /* \u -- half line space up */
	       *sp++ = fc_command_u; break;
	    case 'd': /* \d -- half line space down */
	       *sp++ = fc_command_d; break;
	    default:
	       *sp++ = fc_undefined; break;
	 }
      } else {
	 /* Just a regular character */
	 *sp++ = (enum fontcode) ((int) c & 0xff);
      }
      cp++;
   }
   *sp = fc_end_of_string;
}

strsize(string, code)
   enum fontcode *string;
   int code;
/* Return the size of a string in units of the font size.  The string
   has already been coded into fontcode format.  The code is the
   id of the prevailing font at the beginning of the string. */
{
   float size = 0.0, scale = 1.0;
   int rsize = 0;
   struct fontdescr *pfd,
      *previous;
   enum fontcode *sp = string;

   /* Load initial font */
   previous = pfd = getfont(code);

   /* Look up characters and accumulate sizes. */
   while (*sp != fc_end_of_string) {
      enum fontcode c = *sp;

      /* Command code, not a font code. */
      switch (c) {
	 case fc_command_f: /* Font change */
	    if ((int)sp[1] > MAX_FONTS) {
	       struct fontdescr *temp = pfd;
	       pfd = previous; previous = temp;
	    } else {
	       previous = pfd;
	       pfd = getfont(sp[1]);
	    }
	    sp++;
	    break;
	 case fc_command_s: /* Absolute font size change */
	    rsize = 0;
	    scale = ((float)sp[1])/((float)(pfd->fd_height));
	    sp++;
	    break;
	 case fc_command_sr: /* Relative font size change */
	    rsize += (int)sp[1]; 
	    scale = ((float)(rsize + pfd->fd_height))/((float)(pfd->fd_height));
	    sp++;
	    break;
	 case fc_command_b: /* Don't affect char. size, but two font codes */
	    sp++;
	 case fc_command_d: /* Don't affect character size */
	 case fc_command_u:
	    break;
	 default: /* May be either a character or some undefined code. */
	    if ( ((int) c) > MAX_CODE) {
	       fprintf(stderr, 
		  "%s: Internal error - unrecognized FC %d, giving up.\n",
		  progname,(int)c);
	       exit(1);
	    }
	    /* Character code.  Sum up size for this character. */
	    size += pfd->fd_scale * scale *
	       (pfd->fd_char[(int)c]->rwidth - pfd->fd_char[(int)c]->lwidth);
      }
      ++sp;
   }

   return((int)(size+0.5));
}

txtsize(characters, len, code)
   char *characters;
   int len;
   int code;
/* Return the size of the character string using the prevailing font
   given by code.  */
{
   enum fontcode fs[256];
   int max_length = sizeof(fs)/sizeof(enum fontcode);
   int lc = len;

   /* Translate string. */
   if (len > max_length-1) {
      fprintf(stderr,"%s: Warning -- string too long, truncated.\n",
	 progname);
      lc = max_length-1;
   }
   translate(characters, lc, fs, code);
   
   /* Determine and return length */
   return(strsize(fs, code));
}
