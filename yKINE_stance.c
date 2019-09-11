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
   { "puff" , "pufferfish"          , { -15, -25, -45,  45,  25,  15 } },
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
/*> typedef struct cRADIUS  tRADIUS;                                                  <* 
 *> struct  cRADIUS {                                                                 <* 
 *>    char        abbr;                                                              <* 
 *>    char        name        [LEN_LABEL];                                           <* 
 *>    float       out;                                                               <* 
 *> };                                                                                <* 
 *> static tRADIUS s_radius [MAX_RADIUS ] = {                                         <* 
 *>    /+ 1     12345678901234567890      RR   RM   RF   LF   LM   LR  +/             <* 
 *>    { '0'   , "-"                   , 25.4 * -7.5  },                              <* 
 *>    { '1'   , "-"                   , 25.4 * -7.25 },                              <* 
 *>    { '2'   , "-"                   , 25.4 * -7.0  },                              <* 
 *>    { '3'   , "-"                   , 25.4 * -6.75 },                              <* 
 *>    { '4'   , "-"                   , 25.4 * -6.5  },                              <* 
 *>    { '5'   , "-"                   , 25.4 * -6.25 },                              <* 
 *>    { 'a'   , "-"                   , 25.4 * -6.0  },                              <* 
 *>    { 'A'   , "-"                   , 25.4 * -5.75 },                              <* 
 *>    { 'b'   , "-"                   , 25.4 * -5.5  },                              <* 
 *>    { 'B'   , "-"                   , 25.4 * -5.25 },                              <* 
 *>    { 'c'   , "-"                   , 25.4 * -5.0  },                              <* 
 *>    { 'C'   , "-"                   , 25.4 * -4.75 },                              <* 
 *>    { 'd'   , "-"                   , 25.4 * -4.5  },                              <* 
 *>    { 'D'   , "-"                   , 25.4 * -4.25 },                              <* 
 *>    { 'e'   , "-"                   , 25.4 * -4.0  },                              <* 
 *>    { 'E'   , "-"                   , 25.4 * -3.75 },                              <* 
 *>    { 'f'   , "femu"                , 25.4 * -3.5  },                              <* 
 *>    { 'F'   , "-"                   , 25.4 * -3.25 },                              <* 
 *>    { 'g'   , "-"                   , 25.4 * -3.0  },                              <* 
 *>    { 'G'   , "-"                   , 25.4 * -2.75 },                              <* 
 *>    { 'h'   , "-"                   , 25.4 * -2.5  },                              <* 
 *>    { 'H'   , "-"                   , 25.4 * -2.25 },                              <* 
 *>    { 'i'   , "-"                   , 25.4 * -2.0  },                              <* 
 *>    { 'I'   , "-"                   , 25.4 * -1.75 },                              <* 
 *>    { 'j'   , "-"                   , 25.4 * -1.5  },                              <* 
 *>    { 'J'   , "-"                   , 25.4 * -1.25 },                              <* 
 *>    { 'k'   , "-"                   , 25.4 * -1.0  },                              <* 
 *>    { 'K'   , "-"                   , 25.4 * -0.75 },                              <* 
 *>    { 'l'   , "-"                   , 25.4 * -0.5  },                              <* 
 *>    { 'L'   , "-"                   , 25.4 * -0.25 },                              <* 
 *>    { 'm'   , "military"            , 25.4 *  0.0  },                              <* 
 *>    { 'M'   , "-"                   , 25.4 *  0.25 },                              <* 
 *>    { 'n'   , "-"                   , 25.4 *  0.5  },                              <* 
 *>    { 'N'   , "-"                   , 25.4 *  0.75 },                              <* 
 *>    { 'o'   , "-"                   , 25.4 *  1.0  },                              <* 
 *>    { 'O'   , "-"                   , 25.4 *  1.25 },                              <* 
 *>    { 'p'   , "-"                   , 25.4 *  1.5  },                              <* 
 *>    { 'P'   , "-"                   , 25.4 *  1.75 },                              <* 
 *>    { 'q'   , "-"                   , 25.4 *  2.0  },                              <* 
 *>    { 'Q'   , "-"                   , 25.4 *  2.25 },                              <* 
 *>    { 'r'   , "-"                   , 25.4 *  2.5  },                              <* 
 *>    { 'R'   , "-"                   , 25.4 *  2.75 },                              <* 
 *>    { 's'   , "-"                   , 25.4 *  3.0  },                              <* 
 *>    { 'S'   , "-"                   , 25.4 *  3.25 },                              <* 
 *>    { 't'   , "reaching"            , 25.4 *  3.5  },                              <* 
 *>    { 'T'   , "-"                   , 25.4 *  3.75 },                              <* 
 *>    { 'u'   , "-"                   , 25.4 *  4.0  },                              <* 
 *>    { 'U'   , "-"                   , 25.4 *  4.25 },                              <* 
 *>    { 'v'   , "-"                   , 25.4 *  4.5  },                              <* 
 *>    { 'V'   , "-"                   , 25.4 *  4.75 },                              <* 
 *>    { 'w'   , "-"                   , 25.4 *  5.0  },                              <* 
 *>    { 'W'   , "-"                   , 25.4 *  5.25 },                              <* 
 *>    { 'x'   , "-"                   , 25.4 *  5.5  },                              <* 
 *>    { 'X'   , "-"                   , 25.4 *  5.75 },                              <* 
 *>    { 'y'   , "-"                   , 25.4 *  6.0  },                              <* 
 *>    { 'Y'   , "-"                   , 25.4 *  6.25 },                              <* 
 *>    { 'z'   , "-"                   , 25.4 *  6.5  },                              <* 
 *>    { 'Z'   , "-"                   , 25.4 *  6.75 },                              <* 
 *>    { '>'   , "farthest"            , 25.4 *  7.0  },                              <* 
 *>    { 0     , NULL                  , 0.0          },                              <* 
 *> };                                                                                <*/



#define   MAX_HEIGHTS    100
typedef struct cHEIGHT  tHEIGHT;
struct  cHEIGHT {
   char        abbr;
   char        terse       [LEN_TERSE];
   char        name        [LEN_LABEL];
   float       y;
};
static tHEIGHT s_heights [MAX_HEIGHTS] = {
   /* 1       12345678901234567890       */
   { '^', "-40-" , "-"                   , 25.4 * -3.0  },
   { 'Z', "-30-" , "-"                   , 25.4 * -2.75 },
   { 'z', "-20-" , "-"                   , 25.4 * -2.5  },
   { 'Y', "-10-" , "-"                   , 25.4 * -2.25 },
   { 'y', "-10-" , "-"                   , 25.4 * -2.0  },
   { 'X', "-00-" , "-"                   , 25.4 * -1.75 },
   { 'x', "over" , "overwatch"           , 25.4 * -1.5  },
   { 'W', "-11-" , "-"                   , 25.4 * -1.25 },
   { 'w', "towr" , "tower"               , 25.4 * -1.0  },
   { 'V', "-22-" , "-"                   , 25.4 * -0.75 },
   { 'v', "high" , "high"                , 25.4 * -0.5  },
   { 'U', "-33-" , "-"                   , 25.4 * -0.25 },
   { 'u', "attn" , "attention"           , 25.4 *  0.0  },
   { 'T', "-44-" , "-"                   , 25.4 *  0.25 },
   { 't', "-aa-" , "-"                   , 25.4 *  0.5  },
   { 'S', "-a2-" , "-"                   , 25.4 *  0.75 },
   { 's', "max"  , "max flex"            , 25.4 *  1.0  },
   { 'R', "-b2-" , "-"                   , 25.4 *  1.25 },
   { 'r', "-cc-" , "-"                   , 25.4 *  1.5  },
   { 'Q', "-c2-" , "-"                   , 25.4 *  1.75 },
   { 'q', "easy" , "easy/relaxed"        , 25.4 *  2.0  },
   { 'P', "-55-" , "-"                   , 25.4 *  2.25 },
   { 'p', "low"  , "low to ground"       , 25.4 *  2.5  },
   { 'O', "-66-" , "-"                   , 25.4 *  2.75 },
   { 'o', "rub"  , "just clearing"       , 25.4 *  3.0  },
   { 'N', "-77-" , "-"                   , 25.4 *  3.25 },
   { 'n', "flat" , "flat/grounded"       , 25.4 *  3.5  },
   { 'M', "-88-" , "-"                   , 25.4 *  3.75 },
   { 'm', "-dd-" , "-"                   , 25.4 *  4.0  },
   { 'L', "-d2-" , "-"                   , 25.4 *  4.25 },
   { 'l', "-ee-" , "-"                   , 25.4 *  4.5  },
   { 'K', "-e2-" , "-"                   , 25.4 *  4.75 },
   { 'k', "-ff-" , "-"                   , 25.4 *  5.0  },
   { 'J', "-f2-" , "-"                   , 25.4 *  5.25 },
   { 'j', "zero" , "thorax/coxz"         , 25.4 *  5.5  },
   { 'I', "-f3-" , "-"                   , 25.4 *  5.75 },
   { 'i', "-hh-" , "-"                   , 25.4 *  6.0  },
   { 'H', "-h2-" , "-"                   , 25.4 *  6.25 },
   { 'h', "back" , "over its back"       , 25.4 *  6.5  },
   { 'G', "-h3-" , "-"                   , 25.4 *  6.75 },
   { 'g', "-jj-" , "-"                   , 25.4 *  7.0  },
   { 'F', "-j2-" , "-"                   , 25.4 *  7.25 },
   { 'f', "-kk-" , "-"                   , 25.4 *  7.5  },
   { 'E', "-k2-" , "-"                   , 25.4 *  7.75 },
   { 'e', "-ll-" , "-"                   , 25.4 *  8.0  },
   { 'D', "-l2-" , "-"                   , 25.4 *  8.25 },
   { 'd', "-mm-" , "-"                   , 25.4 *  8.5  },
   { 'C', "-m2-" , "-"                   , 25.4 *  8.75 },
   { 'c', "-nn-" , "-"                   , 25.4 *  9.0  },
   { 'B', "-n2-" , "-"                   , 25.4 *  9.25 },
   { 'b', "-oo-" , "-"                   , 25.4 *  9.5  },
   { 'A', "-o2-" , "-"                   , 25.4 *  9.75 },
   { 'a', "-pp-" , "-"                   , 25.4 * 10.0  },
   { '-', "-p2-" , "-"                   , 25.4 * 10.25 },
   { 'è', "-qq-" , "-"                   , 25.4 * 10.5  },
   { 'é', "-q2-" , "-"                   , 25.4 * 10.75 },
   { 'ê', "-rr-" , "-"                   , 25.4 * 11.0  },
   { 'ë', "-r2-" , "-"                   , 25.4 * 11.25 },
   { 'ì', "-ss-" , "-"                   , 25.4 * 11.5  },
   { 'í', "-s2-" , "-"                   , 25.4 * 11.75 },
   { 'î', "-tt-" , "-"                   , 25.4 * 12.0  },
   { 'ï', "-t2-" , "-"                   , 25.4 * 12.25 },
   { 'ð', "-uu-" , "-"                   , 25.4 * 12.5  },
   { 'ñ', "-u2-" , "-"                   , 25.4 * 12.75 },
   { 'ò', "-vv-" , "-"                   , 25.4 * 13.0  },
   { 'ó', "-v2-" , "-"                   , 25.4 * 13.25 },
   { 'ô', "-ww-" , "-"                   , 25.4 * 13.5  },
   { 'õ', "-w2-" , "-"                   , 25.4 * 13.75 },
   { 'ö', "-xx-" , "-"                   , 25.4 * 14.0  },
   { '÷', "-x2-" , "-"                   , 25.4 * 14.25 },
   { 'ø', "-yy-" , "-"                   , 25.4 * 14.5  },
   { 'ù', "-y2-" , "-"                   , 25.4 * 14.75 },
   { 'ú', "-zz-" , "-"                   , 25.4 * 15.0  },
   { '·', "····" , NULL                  , 0.0          },
};


static char s_verify  [MAX_RADIUS];
#define    INCH2MM     25.400

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
   for (i = 0; i < MAX_RADIUS; ++i) {
      if (s_spreads [i].terse [0] == '\0')            break;
      if (s_spreads [i].terse [0] == '·')             break;
      DEBUG_YKINE_SCRP   yLOG_info    ("checking"  , s_spreads [i].terse);
      if (s_spreads [i].terse [0] != a_entry [0])     continue;
      if (strcmp (s_spreads [i].terse, a_entry) != 0) continue;
      n = i;
      DEBUG_YKINE_SCRP   yLOG_value   ("found"     , n);
      break;
   }
   /*---(literal)------------------------*/
   if (n >= 0) {
      DEBUG_YKINE_SCRP   yLOG_info    ("source"    , s_spreads [n].name);
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

char         /*-> outward xz distance from military position --[--.---.---.--]*/
ykine_stance_radius     (char a_from, double a_start, char *a_entry, double *a_result)
{  /*---(design notes)-------------------*/
   /*
    * actual shortcut grid is from center in inches (groups of 25.4mm).  so,
    * the result compared to military position, which is not exact in inches,
    * will not fall on exact 25.4mm increments.
    *
    * shortcuts are [a-z] going from 3" to 16" outward in 1/2" from center
    * optional suffixes are ' for +1/4", + for +1/8", and - for -1/8"
    */
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        n           =   -1;
   int         i           =    0;
   int         x_len       =    0;
   char        x_short     =  '·';
   char        x_suffix    =  '·';
   char       *x_lower     = "abcdefghijklmnopqrstuvwxyz";
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   x_len  = strlen (a_entry);
   DEBUG_YKINE_SCRP   yLOG_complex ("a_entry"   , "%d:%s", x_len, a_entry);
   /*---(check shortcut)-----------------*/
   if (x_len == 1) {
      if (strchr (x_lower, a_entry [0]) != NULL)  x_short = a_entry [0];
      else if (a_entry [0] == '>')              { x_short = 'w'; x_suffix = '-'; }
      else if (a_entry [0] == '<')                x_short = 'a';
   } else if (x_len == 2) {
      if (strchr (x_lower, a_entry [0]) != NULL) {
         if (strchr ("·+-'", a_entry [1]) != NULL) {
            x_short  = a_entry [0];
            x_suffix = a_entry [1];
         }
      }
   }
   /*---(shortcut)-----------------------*/
   if (x_short != '·') {
      if (a_result != NULL) {
         *a_result = (((x_short - 'a') / 2.0) + 3.0) * INCH2MM;
         switch (x_suffix) {
         case '-' :  *a_result -= INCH2MM * 0.125;  break;
         case '+' :  *a_result += INCH2MM * 0.125;  break;
         case '\'':  *a_result += INCH2MM * 0.250;  break;
         }
      }
      DEBUG_YKINE_SCRP   yLOG_double  ("shortcut"  , *a_result);
      *a_result -= yKINE_seglen (YKINE_THOR) + yKINE_seglen (YKINE_COXA) + yKINE_seglen (YKINE_FEMU) + yKINE_seglen (YKINE_PATE);
      DEBUG_YKINE_SCRP   yLOG_double  ("adjusted"  , *a_result);
   }
   /*---(absolute)-----------------------*/
   else if (a_from == YKINE_PURE) {
      rc  = yPARSE_adjval   (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_double  ("pure"      , *a_result);
   }
   /*---(relative)-----------------------*/
   else {
      rc  = yPARSE_adjfrom  (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_double  ("from"      , *a_result);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
} 

char
ykine_stance_radius_head (int *n, char *a_short)
{
   a_short [0]  = 'a';
   a_short [1]  = '·';
   a_short [2]  = '\0';
   *n = 0;
   return 0;
}

char
ykine_stance_radius_next (int *n, char *a_short)
{
   a_short [2]  = '\0';
   if (a_short [0] < 'a' || a_short [0] > 'z')    return -1;
   if (a_short [1] == '\0') a_short [1] = '·';
   if (strchr ("-·+'", a_short [1]) == NULL)      return -3;
   if (a_short [0] == 'z' && a_short [1] == '·')  return -4;
   switch (a_short [1]) {
   case '-'  : a_short [1] = '·';   break;
   case '·'  : a_short [1] = '+';   break;
   case '+'  : a_short [1] = '\'';  break;
   case '\'' : a_short [0] += 1; a_short [1] = '-';  break;
   }
   ++(*n);
   return 0;
}

int
ykine_stance_radius_max  (void)
{
   char       rcs          =    0;
   int        n            =    0;
   char       x_short      [LEN_LABEL];
   rcs = ykine_stance_radius_head (&n, x_short);
   while (rcs >= 0) {
      rcs = ykine_stance_radius_next (&n, x_short);
   }
   return ++n;
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
   char        rcs;
   int         i;
   float       xt, zt, yt, dt, ot, tt, xz;       /* temporary coordinates     */
   float       f, p, t;
   float       x_full, x_diff;
   float       xi, zi, xo, zo;
   rcs = ykine_stance_radius_head (&i, o_str);
   while (rcs >= 0) {
      rc = ykine__legs_rk_getter (a_leg, d_str, o_str, y_str, &xt, &zt, &yt);
      DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xt, zt, yt);
      /*---(check ik)--------------------*/
      rc = yKINE_inverse (a_leg, xt, zt, yt);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      if (rc >= 0)   s_verify [i] = '¬';
      rcs = ykine_stance_radius_next (&i, o_str);
   }
   return 0;
}

char
ykine_stance_verify_leg    (char a_leg)
{
   char        rcs         =    0;
   int         i           =    0;
   char        x_short     [LEN_LABEL];
   /*---(clear)--------------------------*/
   rcs = ykine_stance_radius_head (&i, x_short);
   while (rcs >= 0) {
      if      (strcmp (y_str, "attn") == 0)        s_verify [i] = '-';
      else if (strcmp (y_str, "max" ) == 0)        s_verify [i] = '´';
      else if (strcmp (y_str, "flat") == 0)        s_verify [i] = '=';
      else if (strcmp (y_str, "zero") == 0)        s_verify [i] = '´';
      else if (strcmp (y_str, "back") == 0)        s_verify [i] = '-';
      else if (strcmp (x_short, "f·") == 0)        s_verify [i] = '|';
      else if (strcmp (x_short, "m·") == 0)        s_verify [i] = '¨';
      else if (strcmp (x_short, "t·") == 0)        s_verify [i] = '|';
      else                                         s_verify [i] = '·';
      rcs = ykine_stance_radius_next (&i, x_short);
   }
   s_verify [++i] = '\0';
   /*---(print)--------------------------*/
   ykine_stance_verify_radius (a_leg);
   printf ("   %s", s_verify);
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
      if (s_heights [i].terse [0] != '-')  printf ("%-5.5s ", s_heights [i].terse);
      else                                 printf ("%-5.5s ", " "                );
      if (s_heights [i].abbr == tolower (s_heights [i].abbr)) {
         printf ("%c %8.2f  ", s_heights [i].abbr, s_heights [i].y);
      }  else if (s_heights [i].abbr < 0) {
         printf ("%c %8.2f  ", s_heights [i].abbr, s_heights [i].y);
      } else {
         printf ("%c %8.2f  ", '-'               , s_heights [i].y);
      }
      ykine_stance_verify_height ();
   }
   return 0;
}

char
ykine_stance_verify     (void)
{
   char        rcs         =    0;
   int         i, j, k;
   char        t           [LEN_LABEL];
   char        x_short     [LEN_LABEL];
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
      printf ("\n%-20.20s =====================================================================================================\n", t);
      printf ("terse a  ---y---     ");
      for (k = YKINE_RR; k <= YKINE_LR; ++k) {
         /*---(clear)--------------------------*/
         rcs = ykine_stance_radius_head (&j, x_short);
         while (rcs >= 0) {
            switch (x_short [1]) {
            case '+' :  case '-' :  printf  (" ");               break;
            case '·' :              printf  ("%c", x_short [0]); break;
            case '\'':              printf  ("'");               break;
            }
            rcs = ykine_stance_radius_next (&j, x_short);
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

#define    MM2COL      25.400
#define    MM2ROW      29.328
/*> #define    MM2ROW      21.997                                                     <*/

char
yKINE_xz2hexdo          (float x, float z, int *c, int *r, float *d, float *o)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_inch, xd, zd, fd, x_deg;
   int         x_base;
   int         rc          =    0;
   char        x_rig       =  '-';
   /*---(header)-------------------------*/
   /*> printf ("%8.2fx, %8.2fz\n", x, z);                                             <*/
   /*---(set base col)-------------------*/
   x_inch = x / MM2COL;
   x_base = round (x_inch);
   xd     = x_inch - x_base;
   *c     = x_base;
   /*> printf ("  col %8.2fx, %8.2fi, %4db, %5.2fd   ", x, x_inch, x_base, xd);       <*/
   /*---(set base row)-------------------*/
   if (*c %  2 !=  0)  z -= 14.664;
   x_inch = z / MM2ROW;
   x_base = round (x_inch);
   zd     = x_inch - x_base;
   *r     = x_base;
   /*> printf ("  row %8.2fz, %8.2fi, %4db, %5.2fd   ", z, x_inch, x_base, zd);       <*/
   /*---(formulate rc)-------------------*/
   fd     = sqrt ((xd * xd) + (zd * zd));
   x_deg  = atan2 (xd, zd) * RAD2DEG;
   while (x_deg <   0.0)  x_deg += 360.0;
   while (x_deg > 360.0)  x_deg -= 360.0;
   /*> printf ("  dir %8.2fd, %8.2fd   ", fd, x_deg);                                     <*/
   if (fd >= 0.33) {
      /*---(direction)--*/
      if      (x_deg <   15)   rc =  1;
      else if (x_deg <   45)   rc =  3;
      else if (x_deg <   75)   rc =  2;
      else if (x_deg <  115)   rc =  6;
      else if (x_deg <  135)   rc =  4;
      else if (x_deg <  165)   rc = 12;
      else if (x_deg <= 195)   rc =  8;
      else if (x_deg <= 225)   rc = 24;
      else if (x_deg <= 255)   rc = 16;
      else if (x_deg <= 285)   rc = 48;
      else if (x_deg <= 315)   rc = 32;
      else if (x_deg <= 345)   rc = 33;
      else                     rc =  1;
   }
   if (d != NULL)  *d = x_deg;
   if (o != NULL)  *o = fd;
   /*> printf ("--   rc %3d\n", rc);                                                  <*/
   /*> printf ("%8.2fz, %8db, %6di, %3dm, %3dr, %3drc\n", z, x_big, x_int, x_mod, *r, rc);   <*/
   /*> printf ("\n");                                                                 <*/
   /*---(complete)-----------------------*/
   return rc;
}

char
yKINE_xz2hex            (float x, float z, int *c, int *r)
{
   return yKINE_xz2hexdo (x, z, *c, *r, NULL, NULL);
}

char
yKINE_hex2xz            (int c, int r, float *x, float *z)
{
   /*---(header)-------------------------*/
   /*> printf ("%6dr, %6dc\n", r, c);                                                 <*/
   *x = c * MM2COL;
   *z = r * MM2ROW;
   if (c % 2 != 0)  *z += 14.664;
   /*---(complete)-----------------------*/
   return 0;
}



