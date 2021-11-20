/* Definitions for font codes */
#define MAX_CODE 511 /* Highest character code in font.  Starts at zero. */
enum fontcode {
   /* 1-255 taken up by ascii codes */
   fc_end_of_string = 0,
   fc_undefined = 511,

   /* Command codes, > 512 */
   fc_command_f = 512,  /* font change */
   fc_command_u = 513,  /* half-space up */
   fc_command_d = 514,  /* half-space down */
   fc_command_s = 515,  /* size change, absolute */
   fc_command_sr = 516,  /* size change, relative */
   fc_command_b = 517,  /* broaden line */

   /* Non-ascii character codes */
   fc_em = 256,
   fc_bullet = 257,
   fc_square = 258,
   fc_rule = 259,
   fc_14 = 260,
   fc_12 = 261,
   fc_34 = 262,
   fc_ff = 263,
   fc_fi = 264,
   fc_fl = 265,
   fc_ffi = 266,
   fc_ffl = 267,
   fc_degree = 268,
   fc_dagger = 269,
   fc_ddagger = 270,
   fc_sectioncode = 271,
   fc_cent = 272,
   fc_registered = 273,
   fc_copyright = 274,
   fc_times = 275,
   fc_divides = 276,
   fc_root = 277,
   fc_overbar = 278,
   fc_geq = 279,
   fc_leq = 280,
   fc_ident = 281,
   fc_apeq = 282,
   fc_approx = 283,
   fc_neq = 284,
   fc_plusminus = 285,
   fc_infinity = 286,

   fc_alpha = 301,
   fc_beta = 302,
   fc_gamma = 303,
   fc_delta = 304,
   fc_epsilon = 305,
   fc_zeta = 306,
   fc_eta = 307,
   fc_theta = 308,
   fc_iota = 309,
   fc_kappa = 310,
   fc_lambda = 311,
   fc_mu = 312,
   fc_nu = 313,
   fc_xi = 314,
   fc_omicron = 315,
   fc_pi = 316,
   fc_rho = 317,
   fc_sigma = 318,
   fc_tau = 319,
   fc_upsilon = 320,
   fc_phi = 321,
   fc_chi = 322,
   fc_psi = 323,
   fc_omega = 324,
   fc_endsigma = 315,

   fc_Alpha = 326,
   fc_Beta = 327,
   fc_Gamma = 328,
   fc_Delta = 329,
   fc_Epsilon = 330,
   fc_Zeta = 331,
   fc_Eta = 332,
   fc_Theta = 333,
   fc_Iota = 334,
   fc_Kappa = 335,
   fc_Lambda = 336,
   fc_Mu = 337,
   fc_Nu = 338,
   fc_Xi = 339,
   fc_Omicron = 340,
   fc_Pi = 341,
   fc_Rho = 342,
   fc_Sigma = 343,
   fc_Tau = 344,
   fc_Upsilon = 345,
   fc_Phi = 346,
   fc_Chi = 347,
   fc_Psi = 348,
   fc_Omega = 349,

   fc_rarrow = 360,
   fc_larrow = 361,
   fc_uarrow = 362,
   fc_darrow = 363,
   fc_cup = 364,
   fc_cap = 365,
   fc_subset = 366,
   fc_superset = 367,
   fc_isubset = 368,
   fc_isuperset = 369, 
   fc_partial = 370,
   fc_grad = 371,
   fc_integral = 372,
   fc_propto = 373,
   fc_emptyset = 374,
   fc_member = 375,
   fc_vbar = 376,
   fc_not = 377,
   fc_or = 378,
   fc_circle = 379,
   fc_footmark = 380

};

typedef enum fontcode
   FC;

#define STROKE_END	((int)' ' - (int)'R')

struct character {
   short lwidth, rwidth;	/* Widths to left and right of centerline */
   union {
      struct {			/* Stroke font */
	 short stroke_count,	/* Number of strokes */
	    stroke[1];		/* First stroke; more follow */
      } strk;
      struct {			/* Hardwired font */
	 short dbit;		/* Offset character below baseline if nonzero */
	 char *stroke;		/* Address of strokes defining character */
      } wire;
   } type;
};

struct fontdescr {		/* Information about a font */
   int fd_id,			/* ID for font */
      fd_height,		/* Height of font */
      fd_baseline;		/* Baseline location down from top of font */
   float fd_scale;		/* Scale factor for font */
   struct character *fd_char[MAX_CODE+1];
   };

struct fontdescr *
   getfont();
