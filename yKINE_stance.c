/*============================----beg-of-source---============================*/
#include    "yKINE.h"
#include    "yKINE_priv.h"

#define   MAX_SPREADS    30
typedef struct cSPREAD  tSPREAD;
struct  cSPREAD {
   char        terse       [LEN_TERSE];
   char        name        [LEN_LABEL];
   int         femu        [6];
};
static tSPREAD s_spreads [MAX_SPREADS] = {
   /* 12       12345678901234567890      RR   RM   RF   LF   LM   LR  */
   { "star" , "starfish"            , {   0,   0,   0,   0,   0,   0 } },
   { "king" , "king_crab"           , {  30,   0, -30,  30,   0, -30 } },
   { "seal" , "seal"                , { -30,   0,  30, -30,   0,  30 } },
   { "lobs" , "lobster"             , {   0,   0,   0,   0,   0,   0 } },
   { "tri"  , "tristar"             , {  30, -30,  30, -30,  30, -30 } },
   { "naul" , "nautilus_left"       , {  45,  45,  45,  45,  45,  45 } },
   { "naur" , "nautilus_right"      , { -45, -45, -45, -45, -45, -45 } },
   { "coul" , "conch_left"          , {  80,  80,  80,  80,  80,  80 } },
   { "cour" , "conch_right"         , { -80, -80, -80, -80, -80, -80 } },
   { "blue" , "blue_crab"           , {  45,   0, -45,  45,   0, -45 } },
   { "ghost", "ghost_crab"          , {  60,   0, -60,  60,   0, -60 } },
   { "eye"  , "the_eye"             , {  80,   0, -80,  80,   0, -80 } },
   { "cutt" , "cuttlefish"          , {  40,   0, -80,  80,   0, -40 } },
   { "manta", "manta_ray"           , { -40,   0, -75,  75,   0,  40 } },
   { "angel", "snow_angel"          , { -45,  20, -40,  40, -20,  45 } },
   { "turt" , "sea_turtle"          , { -30, -85, -85,  85,  85,  30 } },
   { "urch" , "urchin"              , {   0,   0,   0,   0,   0,   0 } },
   { "····" , NULL                  , {   0,   0,   0,   0,   0,   0 } },
};



#define   MAX_RADIUS     100
typedef struct cRADIUS  tRADIUS;
struct  cRADIUS {
   char        abbr;
   char        name        [LEN_LABEL];
   float       out;
};
static tRADIUS s_radius [MAX_RADIUS ] = {
   /* 1     12345678901234567890      RR   RM   RF   LF   LM   LR  */
   { '0'   , "-"                   , 25.4 * -7.5  },
   { '1'   , "-"                   , 25.4 * -7.25 },
   { '2'   , "-"                   , 25.4 * -7.0  },
   { '3'   , "-"                   , 25.4 * -6.75 },
   { '4'   , "-"                   , 25.4 * -6.5  },
   { '5'   , "-"                   , 25.4 * -6.25 },
   { 'a'   , "-"                   , 25.4 * -6.0  },
   { 'A'   , "-"                   , 25.4 * -5.75 },
   { 'b'   , "-"                   , 25.4 * -5.5  },
   { 'B'   , "-"                   , 25.4 * -5.25 },
   { 'c'   , "-"                   , 25.4 * -5.0  },
   { 'C'   , "-"                   , 25.4 * -4.75 },
   { 'd'   , "-"                   , 25.4 * -4.5  },
   { 'D'   , "-"                   , 25.4 * -4.25 },
   { 'e'   , "-"                   , 25.4 * -4.0  },
   { 'E'   , "-"                   , 25.4 * -3.75 },
   { 'f'   , "femu"                , 25.4 * -3.5  },
   { 'F'   , "-"                   , 25.4 * -3.25 },
   { 'g'   , "-"                   , 25.4 * -3.0  },
   { 'G'   , "-"                   , 25.4 * -2.75 },
   { 'h'   , "-"                   , 25.4 * -2.5  },
   { 'H'   , "-"                   , 25.4 * -2.25 },
   { 'i'   , "-"                   , 25.4 * -2.0  },
   { 'I'   , "-"                   , 25.4 * -1.75 },
   { 'j'   , "-"                   , 25.4 * -1.5  },
   { 'J'   , "-"                   , 25.4 * -1.25 },
   { 'k'   , "-"                   , 25.4 * -1.0  },
   { 'K'   , "-"                   , 25.4 * -0.75 },
   { 'l'   , "-"                   , 25.4 * -0.5  },
   { 'L'   , "-"                   , 25.4 * -0.25 },
   { 'm'   , "military"            , 25.4 *  0.0  },
   { 'M'   , "-"                   , 25.4 *  0.25 },
   { 'n'   , "-"                   , 25.4 *  0.5  },
   { 'N'   , "-"                   , 25.4 *  0.75 },
   { 'o'   , "-"                   , 25.4 *  1.0  },
   { 'O'   , "-"                   , 25.4 *  1.25 },
   { 'p'   , "-"                   , 25.4 *  1.5  },
   { 'P'   , "-"                   , 25.4 *  1.75 },
   { 'q'   , "-"                   , 25.4 *  2.0  },
   { 'Q'   , "-"                   , 25.4 *  2.25 },
   { 'r'   , "-"                   , 25.4 *  2.5  },
   { 'R'   , "-"                   , 25.4 *  2.75 },
   { 's'   , "-"                   , 25.4 *  3.0  },
   { 'S'   , "-"                   , 25.4 *  3.25 },
   { 't'   , "reaching"            , 25.4 *  3.5  },
   { 'T'   , "-"                   , 25.4 *  3.75 },
   { 'u'   , "-"                   , 25.4 *  4.0  },
   { 'U'   , "-"                   , 25.4 *  4.25 },
   { 'v'   , "-"                   , 25.4 *  4.5  },
   { 'V'   , "-"                   , 25.4 *  4.75 },
   { 'w'   , "-"                   , 25.4 *  5.0  },
   { 'W'   , "-"                   , 25.4 *  5.25 },
   { 'x'   , "-"                   , 25.4 *  5.5  },
   { 'X'   , "-"                   , 25.4 *  5.75 },
   { 'y'   , "-"                   , 25.4 *  6.0  },
   { 'Y'   , "-"                   , 25.4 *  6.25 },
   { 'z'   , "-"                   , 25.4 *  6.5  },
   { 'Z'   , "-"                   , 25.4 *  6.75 },
   { '>'   , "farthest"            , 25.4 *  7.0  },
   { 0     , NULL                  , 0.0          },
};



#define   MAX_HEIGHTS    100
typedef struct cHEIGHT  tHEIGHT;
struct  cHEIGHT {
   char        terse       [LEN_TERSE];
   char        name        [LEN_LABEL];
   float       y;
};
static tHEIGHT s_heights [MAX_HEIGHTS] = {
   /* 1       12345678901234567890       */
   { "-40-" , "-"                   , 25.4 * -3.0  },
   { "-30-" , "-"                   , 25.4 * -2.75 },
   { "-20-" , "-"                   , 25.4 * -2.5  },
   { "-10-" , "-"                   , 25.4 * -2.25 },
   { "-10-" , "-"                   , 25.4 * -2.0  },
   { "-00-" , "-"                   , 25.4 * -1.75 },
   { "over" , "overwatch"           , 25.4 * -1.5  },
   { "-11-" , "-"                   , 25.4 * -1.25 },
   { "towr" , "tower"               , 25.4 * -1.0  },
   { "-22-" , "-"                   , 25.4 * -0.75 },
   { "high" , "high"                , 25.4 * -0.5  },
   { "-33-" , "-"                   , 25.4 * -0.25 },
   { "attn" , "attention"           , 25.4 *  0.0  },
   { "-44-" , "-"                   , 25.4 *  0.25 },
   { "-aa-" , "-"                   , 25.4 *  0.5  },
   { "-a2-" , "-"                   , 25.4 *  0.75 },
   { "max"  , "max flex"            , 25.4 *  1.0  },
   { "-b2-" , "-"                   , 25.4 *  1.25 },
   { "-cc-" , "-"                   , 25.4 *  1.5  },
   { "-c2-" , "-"                   , 25.4 *  1.75 },
   { "easy" , "easy/relaxed"        , 25.4 *  2.0  },
   { "-55-" , "-"                   , 25.4 *  2.25 },
   { "low"  , "low to ground"       , 25.4 *  2.5  },
   { "-66-" , "-"                   , 25.4 *  2.75 },
   { "rub"  , "just clearing"       , 25.4 *  3.0  },
   { "-77-" , "-"                   , 25.4 *  3.25 },
   { "flat" , "flat/grounded"       , 25.4 *  3.5  },
   { "-88-" , "-"                   , 25.4 *  3.75 },
   { "-dd-" , "-"                   , 25.4 *  4.0  },
   { "-d2-" , "-"                   , 25.4 *  4.25 },
   { "-ee-" , "-"                   , 25.4 *  4.5  },
   { "-e2-" , "-"                   , 25.4 *  4.75 },
   { "-ff-" , "-"                   , 25.4 *  5.0  },
   { "-f2-" , "-"                   , 25.4 *  5.25 },
   { "zero" , "thorax/coxz"         , 25.4 *  5.5  },
   { "-f3-" , "-"                   , 25.4 *  5.75 },
   { "-hh-" , "-"                   , 25.4 *  6.0  },
   { "-h2-" , "-"                   , 25.4 *  6.25 },
   { "back" , "over its back"       , 25.4 *  6.5  },
   { "-h3-" , "-"                   , 25.4 *  6.75 },
   { "-jj-" , "-"                   , 25.4 *  7.0  },
   { "-j2-" , "-"                   , 25.4 *  7.25 },
   { "-kk-" , "-"                   , 25.4 *  7.5  },
   { "-k2-" , "-"                   , 25.4 *  7.75 },
   { "-ll-" , "-"                   , 25.4 *  8.0  },
   { "-l2-" , "-"                   , 25.4 *  8.25 },
   { "-mm-" , "-"                   , 25.4 *  8.5  },
   { "-m2-" , "-"                   , 25.4 *  8.75 },
   { "-nn-" , "-"                   , 25.4 *  9.0  },
   { "-n2-" , "-"                   , 25.4 *  9.25 },
   { "-oo-" , "-"                   , 25.4 *  9.5  },
   { "-o2-" , "-"                   , 25.4 *  9.75 },
   { "-pp-" , "-"                   , 25.4 * 10.0  },
   { "-p2-" , "-"                   , 25.4 * 10.25 },
   { "-qq-" , "-"                   , 25.4 * 10.5  },
   { "-q2-" , "-"                   , 25.4 * 10.75 },
   { "-rr-" , "-"                   , 25.4 * 11.0  },
   { "-r2-" , "-"                   , 25.4 * 11.25 },
   { "-ss-" , "-"                   , 25.4 * 11.5  },
   { "-s2-" , "-"                   , 25.4 * 11.75 },
   { "-tt-" , "-"                   , 25.4 * 12.0  },
   { "-t2-" , "-"                   , 25.4 * 12.25 },
   { "-uu-" , "-"                   , 25.4 * 12.5  },
   { "-u2-" , "-"                   , 25.4 * 12.75 },
   { "-vv-" , "-"                   , 25.4 * 13.0  },
   { "-v2-" , "-"                   , 25.4 * 13.25 },
   { "-ww-" , "-"                   , 25.4 * 13.5  },
   { "-w2-" , "-"                   , 25.4 * 13.75 },
   { "-xx-" , "-"                   , 25.4 * 14.0  },
   { "-x2-" , "-"                   , 25.4 * 14.25 },
   { "-yy-" , "-"                   , 25.4 * 14.5  },
   { "-y2-" , "-"                   , 25.4 * 14.75 },
   { "-zz-" , "-"                   , 25.4 * 15.0  },
   { "····" , NULL                  , 0.0          },
};


static char s_verify  [MAX_RADIUS];

char
ykine_stance_spread     (char a_from, char a_leg, double a_start, char *a_entry, double *a_result)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =   -1;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_value   ("a_leg"     , a_leg);
   /*---(defense)------------------------*/
   --rce;  if (a_leg < YKINE_RR || a_leg > YKINE_LR) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --a_leg;
   /*---(search)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_info    ("a_entry"   , a_entry);
   for (i = 0; i < MAX_SPREADS; ++i) {
      if (s_spreads [i].terse     == NULL)                 break;
      if (s_spreads [i].terse [0] == '·')                  break;
      DEBUG_YKINE_SCRP   yLOG_info    ("checking"  , s_spreads [i].terse);
      if (s_spreads [i].terse [0] == '-')                  continue;
      if (s_spreads [i].terse [0] != a_entry [0])          continue;
      if (strcmp (s_spreads [i].terse, a_entry) != 0)      continue;
      n = i;
      DEBUG_YKINE_SCRP   yLOG_value   ("found"     , n);
      break;
   }
   /*---(literal)------------------------*/
   if (n >= 0) {
      DEBUG_YKINE_SCRP   yLOG_value   ("source"    , s_spreads [n].femu [a_leg]);
      if (a_result != NULL)  *a_result = s_spreads [n].femu [a_leg];
      DEBUG_YKINE_SCRP   yLOG_value   ("literal"   , *a_result);
   }
   /*---(absolute)-----------------------*/
   else if (a_from == YKINE_PURE) {
      rc  = yPARSE_adjval   (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("pure"      , *a_result);
   }
   /*---(relative)-----------------------*/
   else if (a_from == YKINE_FROM) {
      rc  = yPARSE_adjfrom  (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("from"      , *a_result);
   }
   /*---(bad type)-----------------------*/
   else {
      DEBUG_YKINE_SCRP   yLOG_note    ("type not known");
      rc = -1;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
} 

char
ykine_stance_radius     (char a_from, double a_start, char *a_entry, double *a_result)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        n           =   -1;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(search)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_info    ("a_entry"   , a_entry);
   for (i = 0; i < MAX_RADIUS; ++i) {
      if (s_radius  [i].abbr == NULL)                 break;
      if (s_radius  [i].abbr == '·')                  break;
      DEBUG_YKINE_SCRP   yLOG_char    ("checking"  , s_radius  [i].abbr);
      if (s_radius  [i].abbr == '-')                  continue;
      if (s_radius  [i].abbr != a_entry [0])          continue;
      n = i;
      DEBUG_YKINE_SCRP   yLOG_value   ("found"     , n);
      break;
   }
   /*---(literal)------------------------*/
   if (n >= 0) {
      DEBUG_YKINE_SCRP   yLOG_value   ("source"    , s_radius  [n].out);
      if (a_result != NULL)  *a_result = s_radius  [n].out;
      DEBUG_YKINE_SCRP   yLOG_value   ("literal"   , *a_result);
   }
   /*---(absolute)-----------------------*/
   else if (a_from == YKINE_PURE) {
      rc  = yPARSE_adjval   (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("pure"      , *a_result);
   }
   /*---(relative)-----------------------*/
   else {
      rc  = yPARSE_adjfrom  (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("from"      , *a_result);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
} 

char
ykine_stance_height     (char a_from, double a_start, char *a_entry, double *a_result)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        n           =   -1;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(search)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_info    ("a_entry"   , a_entry);
   for (i = 0; i < MAX_HEIGHTS; ++i) {
      if (s_heights [i].terse     == NULL)                 break;
      if (s_heights [i].terse [0] == '·')                  break;
      DEBUG_YKINE_SCRP   yLOG_info    ("checking"  , s_heights [i].terse);
      /*> if (s_heights [i].terse [0] == '-')                  continue;              <*/
      if (s_heights [i].terse [0] != a_entry [0])          continue;
      if (strcmp (s_heights [i].terse, a_entry) != 0)      continue;
      n = i;
      DEBUG_YKINE_SCRP   yLOG_value   ("found"     , n);
      break;
   }
   /*---(literal)------------------------*/
   if (n >= 0) {
      DEBUG_YKINE_SCRP   yLOG_value   ("source"    , s_heights [n].y);
      if (a_result != NULL)  *a_result = s_heights [n].y;
      DEBUG_YKINE_SCRP   yLOG_value   ("literal"   , *a_result);
   }
   /*---(absolute)-----------------------*/
   else if (a_from == YKINE_PURE) {
      rc  = yPARSE_adjval   (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("pure"      , *a_result);
   }
   /*---(relative)-----------------------*/
   else {
      rc  = yPARSE_adjfrom  (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("from"      , *a_result);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
} 



static float       s_hadji, s_hadjo, s_vadj;
static char        d_str       [LEN_LABEL];
static char        o_str       [LEN_LABEL];
static char        y_str       [LEN_LABEL];
static float       cp;

char
ykine_stance_verify_radius (char a_leg)
{
   char        rc;
   int         i;
   double      xt, zt, yt, dt, ot, tt, xz;       /* temporary coordinates     */
   float       f, p, t;
   float       x_full, x_diff;
   float       xi, zi, xo, zo;
   for (i = 0; i < MAX_RADIUS; ++i) {
      if (s_radius  [i].abbr == NULL)                 break;
      if (s_radius  [i].abbr == '·')                  break;
      sprintf (o_str, "%c", s_radius [i].abbr);
      /*---(calc inner triangle)---------*/
      xi      =  s_hadji * cos (cp * DEG2RAD);
      zi      = -s_hadji * sin (cp * DEG2RAD);
      DEBUG_YKINE_SCRP  yLOG_complex ("inner"     , "%8.2fh, %8.2fx, %8.2fz", s_hadji, xi, zi);
      /*---(prepare constants)-----------*/
      ykine_stance_spread (myKINE.s_from, a_leg, 0.0   , d_str, &dt);
      ykine_stance_radius (myKINE.s_from       , 0.0   , o_str, &ot);
      ykine_stance_height (myKINE.s_from       , 0.0   , y_str, &yt);
      DEBUG_YKINE_SCRP  yLOG_complex ("values"    , "%8.3fd, %8.2fo, %8.2fy", dt, ot, yt);
      /*---(convert from cx)-------------*/
      ot    += s_hadjo;
      x_full = cp + dt;
      if (x_full > 360)  x_full -= 360;
      if (x_full <   0)  x_full += 360;
      /*---(convert from rk)-------------*/
      /*> x_full = cp - dt;                                                              <*/
      xz     = ot;
      xo     =  xz * cos (x_full * DEG2RAD);
      zo     = -xz * sin (x_full * DEG2RAD);
      DEBUG_YKINE_SCRP  yLOG_complex ("fix_out"   , "%8.3fd, %8.3ff, %8.2fo, %8.2fxo, %8.2fzo", dt, x_full, ot, zo, zo);
      xt     = xi + xo;
      zt     = zi + zo;
      yt    -= s_vadj;
      DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xt, zt, yt);
      /*---(check ik)--------------------*/
      rc = yKINE_inverse (a_leg, xt, zt, yt);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      if (rc <  0)    continue;
      /*---(retrieve angles)-------------*/
      rc = yKINE_angles   (a_leg, YKINE_IK, NULL, &f, &p, &t);
      s_verify [i] = 'y';
   }
   return 0;
}

char
ykine_stance_verify_leg    (char a_leg)
{
   int         i;
   /*---(clear)--------------------------*/
   for (i = 0; i < MAX_RADIUS; ++i)    s_verify [i] = NULL;
   for (i = 0; i < MAX_RADIUS; ++i) {
      if (s_radius [i].abbr == NULL)   break;
      if      (strcmp (y_str, "attn") == 0)   s_verify [i] = '-';
      else if (strcmp (y_str, "max" ) == 0)   s_verify [i] = '´';
      else if (strcmp (y_str, "flat") == 0)   s_verify [i] = '=';
      else if (strcmp (y_str, "zero") == 0)   s_verify [i] = '´';
      else if (strcmp (y_str, "back") == 0)   s_verify [i] = '-';
      else if (s_radius [i].abbr == 'f')      s_verify [i] = '|';
      else if (s_radius [i].abbr == 'm')      s_verify [i] = '¨';
      else if (s_radius [i].abbr == 't')      s_verify [i] = '|';
      else                                    s_verify [i] = '·';
   }
   /*---(print)--------------------------*/
   ykine_stance_verify_radius (a_leg);
   for (i = 0; i < MAX_RADIUS; ++i) {
      if (s_verify [i] == NULL)  break;
      printf ("%c ", s_verify [i]);
   }
   return 0;
}

char
ykine_stance_verify_height (void)
{
   int         i;
   for (i = YKINE_RR; i <= YKINE_LR; ++i) {
      cp  = yKINE_legdeg (i);
      ykine_stance_verify_leg    (i);
      printf ("   ");
      break;   /* TURNS OUT ALL LEGS ARE THE SAME */
   }
   printf ("\n");
   return 0;
}

char
ykine_stance_verify_spread (void)
{
   int         i;
   for (i = 0; i < MAX_HEIGHTS; ++i) {
      if (s_heights [i].terse     == NULL)                 break;
      if (s_heights [i].terse [0] == '·')                  break;
      strlcpy (y_str, s_heights [i].terse, LEN_LABEL);
      if (s_heights [i].terse [0] != '-') {
         printf ("%-5.5s %8.2f  ", s_heights [i].terse, s_heights [i].y);
      } else {
         printf ("%-5.5s %8.2f  ", " "                , s_heights [i].y);
      }
      ykine_stance_verify_height ();
   }
   return 0;
}

char
ykine_stance_verify     (void)
{
   int         i, j, k;
   char        t           [LEN_LABEL];
   /*---(prepare constants)-----------*/
   /*> s_hadj  = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);   <*/
   s_hadji = yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA);
   s_hadjo = yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
   s_vadj  = yKINE_seglen (YKINE_TIBI) + yKINE_seglen (YKINE_FOOT);
   for (i = 0; i < MAX_SPREADS; ++i) {
      if (s_spreads [i].terse     == NULL)                 break;
      if (s_spreads [i].terse [0] == '·')                  break;
      if (s_spreads [i].terse [0] == '-')                  continue;
      strlcpy (d_str, s_spreads [i].terse, LEN_LABEL);
      sprintf (t, "%s configuration", d_str);
      printf ("\n%-20.20s ================================================================================================================\n", t);
      printf ("terse  ---y---  ");
      for (k = YKINE_RR; k <= YKINE_LR; ++k) {
         for (j = 0; j < MAX_RADIUS; ++j) {
            if      (s_radius [j].abbr == NULL)   break;
            else if (s_radius [j].abbr == '·')    break;
            else if (s_radius [j].abbr <= '9')    printf ("- ");
            else if (s_radius [j].abbr == tolower (s_radius [j].abbr))  printf ("%c ", s_radius [j].abbr);
            else  printf ("- ");
         }
         printf ("   ");
         break;   /* TURNS OUT ALL LEGS ARE THE SAME */
      }
      printf ("\n");
      ykine_stance_verify_spread ();
      break;  /* TURNS OUT ONLY ONE PATTERN  */
   }
   return 0;
}
