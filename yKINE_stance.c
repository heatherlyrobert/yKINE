/*============================----beg-of-source---============================*/
#include    "yKINE.h"
#include    "yKINE_priv.h"

#define   MAX_SPREADS    30
typedef struct cSPREAD  tSPREAD;
struct  cSPREAD {
   char        terse       [LEN_TERSE];
   char        name        [LEN_LABEL];
   int         femu        [6];
   char        desc        [LEN_DESC];
};
static tSPREAD s_spreads [MAX_SPREADS] = {
   /* 12       12345678901234567890      RR   RM   RF   LF   LM   LR  */
   { "star" , "starfish"            , {   0,   0,   0,   0,   0,   0 } , "completely balanced and even radial spread"     },
   { "king" , "king_crab"           , {  30,   0, -30,  30,   0, -30 } , "big sideways legs crab spread"                  },
   { "blue" , "blue_crab"           , {  45,   0, -45,  45,   0, -45 } , "traditional sideways crab leg configuration"    },
   { "ghost", "ghost_crab"          , {  60,   0, -60,  60,   0, -60 } , "all legs sideways giving narrowest profile"     },
   { "tri"  , "tristar"             , {  30, -30,  30, -30,  30, -30 } , "three groups of parallel legs at even spread"   },
   { "naul" , "nautilus_left"       , {  45,  45,  45,  45,  45,  45 } , "legs all loosely radially curled to the left"   },
   { "naur" , "nautilus_right"      , { -45, -45, -45, -45, -45, -45 } , "legs all loosely radially curled to the right"  },
   { "coul" , "conch_left"          , {  80,  80,  80,  80,  80,  80 } , "legs all tightly radially curled to the left"   },
   { "cour" , "conch_right"         , { -80, -80, -80, -80, -80, -80 } , "legs all tightly radially curled to the right"  },
   { "eye"  , "the_eye"             , {  80,   0, -80,  80,   0, -80 } , "two groups of tightly grouped legs at each side"},
   { "cutt" , "cuttlefish"          , {  40,   0, -80,  80,   0, -40 } , "transition stance"                              },
   { "manta", "manta_ray"           , { -40,   0, -75,  75,   0,  40 } , "three groups of legs in a t-shape"              },
   { "angel", "snow_angel"          , { -45,  20, -40,  40, -20,  45 } , "three groups of legs in a y-shape"              },
   { "turt" , "sea_turtle"          , { -30, -85, -85,  85,  85,  30 } , "pour legs back and front to out to sides"       },
   { "seal" , "seal"                , { -30,   0,  30, -30,   0,  30 } , "two legs fore/back, one each to sides"          },
   { "puff" , "pufferfish"          , { -15, -25, -45,  45,  25,  15 } , "wide leg spread, but open in front for access"  },
   { "····" , NULL                  , {   0,   0,   0,   0,   0,   0 } },
};



#define   MAX_VERIFY     200
static char s_verify  [MAX_VERIFY];



#define    INCH2MM     25.400



/*====================------------------------------------====================*/
/*===----                       leg spread/patterns                    ----===*/
/*====================------------------------------------====================*/
static void      o___SPREAD__________________o (void) {;}

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



/*====================------------------------------------====================*/
/*===----                       leg distance/radius                    ----===*/
/*====================------------------------------------====================*/
static void      o___RADIUS__________________o (void) {;}

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
      else if (a_entry [0] == '>')              { x_short = 'w'; x_suffix = '+'; }
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



/*====================------------------------------------====================*/
/*===----                         body heights                         ----===*/
/*====================------------------------------------====================*/
static void      o___HEIGHT__________________o (void) {;}

char
ykine_stance_height     (char a_from, double a_start, char *a_entry, double *a_result)
{
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
   } else if (x_len == 2) {
      if (strchr (x_lower, a_entry [0]) != NULL) {
         if (strchr ("·+-'", a_entry [1]) != NULL) {
            x_short  = a_entry [0];
            x_suffix = a_entry [1];
         }
      }
   } else if (x_len == 4) {
      if      (strcmp ("over", a_entry) == 0)  { x_short = 'x'; x_suffix = '·'; }
      else if (strcmp ("towr", a_entry) == 0)  { x_short = 'w'; x_suffix = '·'; }
      else if (strcmp ("high", a_entry) == 0)  { x_short = 'v'; x_suffix = '·'; }
      else if (strcmp ("attn", a_entry) == 0)  { x_short = 'u'; x_suffix = '·'; }
      else if (strcmp ("flex", a_entry) == 0)  { x_short = 's'; x_suffix = '·'; }
      else if (strcmp ("easy", a_entry) == 0)  { x_short = 'q'; x_suffix = '·'; }
      else if (strcmp ("flat", a_entry) == 0)  { x_short = 'n'; x_suffix = '·'; }
   }
   /*---(shortcut)-----------------------*/
   if (x_short != '·') {
      if (a_result != NULL) {
         *a_result = ((x_short - 'j') / 2.0) * INCH2MM;
         switch (x_suffix) {
         case '-' :  *a_result -= INCH2MM * 0.125;  break;
         case '+' :  *a_result += INCH2MM * 0.125;  break;
         case '\'':  *a_result += INCH2MM * 0.250;  break;
         }
      }
      DEBUG_YKINE_SCRP   yLOG_double  ("shortcut"  , *a_result);
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
ykine_stance_height_head (int *n, char *a_short)
{
   return ykine_stance_radius_head (n, a_short);
}

char
ykine_stance_height_next (int *n, char *a_short)
{
   return ykine_stance_radius_next (n, a_short);
}



/*====================------------------------------------====================*/
/*===----                      distance of leg drop                    ----===*/
/*====================------------------------------------====================*/
static void      o___LEGDROP_________________o (void) {;}

char
ykine_stance_legdrop    (char a_from, double a_start, char *a_entry, double *a_result)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   char        n           =   -1;
   int         i           =    0;
   int         x_len       =    0;
   char        x_short     =  '·';
   char        x_suffix    =  '·';
   char       *x_upper     = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   char       *x_lower     = "abcdefghijklmnopqrstuvwxyz";
   int         x_sign      =    1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   x_len  = strlen (a_entry);
   DEBUG_YKINE_SCRP   yLOG_complex ("a_entry"   , "%d:%s", x_len, a_entry);
   /*---(check shortcut)-----------------*/
   if (x_len == 1) {
      if      (strchr (x_lower, a_entry [0]) != NULL) {
         x_short  = a_entry [0];
         x_sign   = -1;
      } else if (strchr (x_upper, a_entry [0]) != NULL) {
         x_short  = tolower (a_entry [0]);
         x_sign   = +1;
      }
   } else if (x_len == 2) {
      if      (strchr (x_lower, a_entry [0]) != NULL) {
         if (strchr ("·+-'", a_entry [1]) != NULL) {
            x_short  = a_entry [0];
            x_sign   = -1;
            x_suffix = a_entry [1];
         }
      } else if (strchr (x_upper, a_entry [0]) != NULL) {
         if (strchr ("·+-'", a_entry [1]) != NULL) {
            x_short  = tolower (a_entry [0]);
            x_sign   = +1;
            x_suffix = a_entry [1];
         }
      }
   } else if (x_len == 4) {
      if      (strcmp ("zero", a_entry) == 0)  { x_short = 'a'; x_suffix = '·'; }
   }
   /*---(shortcut)-----------------------*/
   if (x_short != '·') {
      if (a_result != NULL) {
         *a_result = ((x_short - 'a') / 2.0) * INCH2MM * x_sign;
         switch (x_suffix) {
         case '-' :  *a_result -= INCH2MM * 0.125;  break;
         case '+' :  *a_result += INCH2MM * 0.125;  break;
         case '\'':  *a_result += INCH2MM * 0.250;  break;
         }
      }
      DEBUG_YKINE_SCRP   yLOG_double  ("shortcut"  , *a_result);
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
ykine_stance_legdrop_head (int *n, char *a_short)
{
   a_short [0]  = 'Z';
   a_short [1]  = '·';
   a_short [2]  = '\0';
   *n = 0;
   return 0;
}

char
ykine_stance_legdrop_next (int *n, char *a_short)
{
   a_short [2]  = '\0';
   if (a_short [0] < 'A' || (a_short [0] > 'Z' && a_short [0] < 'a') || a_short [0] > 'z')    return -1;
   if (a_short [1] == '\0') a_short [1] = '·';
   if (strchr ("-·+'", a_short [1]) == NULL)      return -3;
   if (a_short [0] == 'z' && a_short [1] == '·')  return -4;
   switch (a_short [1]) {
   case '\'' : a_short [1] = '+';   break;
   case '+'  : a_short [1] = '·';   break;
   case '·'  : a_short [1] = '-';   break;
   case '-'  :
               if (a_short [0] == toupper (a_short [0]))  a_short [0] -= 1;
               else                                       a_short [0] += 1;
               a_short [1] = '\'';
               break;
   }
   if (a_short [0] == 'A' && a_short [1] == '\'')  a_short [0] = 'a';
   ++(*n);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                verifying legal leg positions                 ----===*/
/*====================------------------------------------====================*/
static void      o___VERIFY__________________o (void) {;}

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
      /*> yt += s_vadj;                                                               <*/
      DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", xt, zt, yt);
      /*---(check ik)--------------------*/
      rc = yKINE_inverse (a_leg, xt, zt, yt);
      /*> printf ("%-10.10sd, %-10.10so, %-10.10sy     pos %8.2fx, %8.2fz, %8.2fy     rc %4d\n", d_str, o_str, y_str, xt, zt, yt, rc);   <*/
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
      if      (strcmp (y_str  , "a·") == 0)        s_verify [i] = '´';
      else if (strcmp (x_short, "f·") == 0)        s_verify [i] = '|';
      else if (strcmp (x_short, "m·") == 0)        s_verify [i] = '¨';
      else if (strcmp (x_short, "t·") == 0)        s_verify [i] = '|';
      else                                         s_verify [i] = '·';
      rcs = ykine_stance_radius_next (&i, x_short);
   }
   s_verify [++i] = '\0';
   /*---(print)--------------------------*/
   ykine_stance_verify_radius (a_leg);
   printf ("%s", s_verify);
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
   char        rcs         =    0;
   int         i           =    0;
   rcs = ykine_stance_legdrop_head (&i, y_str);
   y_str [0] = 'S';
   while (rcs >= 0) {
      if (strchr ("tuvwxyz", y_str [0]) != NULL)  break;
      switch (y_str [1]) {
      case '+' :  case '-' :
         break;
      case '·' :
         printf  ("%c ", y_str [0]);
         ykine_stance_verify_height ();
         break;
      case '\'':
         printf  ("' ");
         ykine_stance_verify_height ();
         break;
      }
      rcs = ykine_stance_legdrop_next (&i, y_str);
   }
   return 0;
}

/*> char                                                                              <* 
 *> ykine_stance_verify_clear (void)                                                  <* 
 *> {                                                                                 <* 
 *>    for (y = 0; y < MAX_VERIFY; ++y) {                                             <* 
 *>       for (x = 0; x < MAX_VERIFY; ++x) {                                          <* 
 *>          s_verify [y][x] = '·';                                                   <* 
 *>       }                                                                           <* 
 *>    }                                                                              <* 
 *> }                                                                                 <*/

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
      printf ("\n%-20.20s ==================================================================================\n", t);
      printf ("  ");
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



