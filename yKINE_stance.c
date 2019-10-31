/*============================----beg-of-source---============================*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



/*===[[ METIS ]]==============================================================*/
/*
 * metis  dm8··  stance changes must use a step to reposition the foot
 * metis  dn4··  hex/xz conversions are making arachne footprints wrong
 *
 *
 *
 */



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
static char s_verify  [MAX_VERIFY][MAX_VERIFY];



/*====================------------------------------------====================*/
/*===----                       leg spread/patterns                    ----===*/
/*====================------------------------------------====================*/
static void      o___SPREAD__________________o (void) {;}

char
ykine_stance_spread     (char a_leg, double a_start, char *a_entry, double *a_result)
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
   else {
      rc  = yPARSE_adjval   (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("pure"      , *a_result);
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
ykine_stance_radius     (double a_start, char *a_entry, double *a_result)
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
   /*---(normal)-------------------------*/
   if (x_short == '·') {
      rc  = yPARSE_adjval   (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_double  ("pure"      , *a_result);
   }
   /*---(shortcut)-----------------------*/
   else {
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

float
yKINE_radius         (char *a_entry)
{
   double     x_val;
   ykine_stance_radius (0.0, a_entry, &x_val);
   return x_val;
}



/*====================------------------------------------====================*/
/*===----                         body heights                         ----===*/
/*====================------------------------------------====================*/
static void      o___HEIGHT__________________o (void) {;}

char
ykine_stance_height     (char a_verb, double a_start, char *a_entry, double *a_result)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        n           =   -1;
   int         i           =    0;
   int         x_len       =    0;
   char        x_short     =  '·';
   char        x_suffix    =  '·';
   char       *x_lower     = "abcdefghijklmnopqrstuvwxyz";
   char        x_zero      =  'j';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_complex ("pointers"  , "entry %p, result %p", a_entry, a_result);
   --rce;  if (a_entry == NULL || a_result == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare)------------------------*/
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
   /*---(normal)-------------------------*/
   if (x_short == '·') {
      rc  = yPARSE_adjval   (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("pure"      , *a_result);
   }
   /*---(shortcuted)---------------------*/
   else {
      /*---(adjust for body)-------------*/
      switch (a_verb) {
      case YKINE_ZE : case YKINE_PO : case YKINE_ST :
         x_zero = 'u';
         break;
      default :
         x_zero = 'j';
         break;
      }
      /*---(calculate)-------------------*/
      *a_result = ((x_short - x_zero) / 2.0) * INCH2MM;
      /*---(tweak)-----------------------*/
      switch (x_suffix) {
      case '-' :  *a_result -= INCH2MM * 0.125;  break;
      case '+' :  *a_result += INCH2MM * 0.125;  break;
      case '\'':  *a_result += INCH2MM * 0.250;  break;
      }
      DEBUG_YKINE_SCRP   yLOG_double  ("shortcut"  , *a_result);
      /*---(done)------------------------*/
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

/*> float                                                                             <* 
 *> yKINE_height         (char *a_entry)                                              <* 
 *> {                                                                                 <* 
 *>    double     x_val;                                                              <* 
 *>    ykine_stance_height (YKINE_PURE, 0.0, a_entry, &x_val);                        <* 
 *>    return x_val;                                                                  <* 
 *> }                                                                                 <*/



/*====================------------------------------------====================*/
/*===----                      distance of leg drop                    ----===*/
/*====================------------------------------------====================*/
static void      o___LEGDROP_________________o (void) {;}

char
ykine_stance_scale      (double a_start, char *a_entry, double *a_result)
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
   /*---(normal)-------------------------*/
   if (x_short == '·') {
      rc  = yPARSE_adjval   (a_start, a_entry, a_result);
      DEBUG_YKINE_SCRP   yLOG_value   ("pure"      , *a_result);
   }
   /*---(shortcut)-----------------------*/
   else {
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
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
} 

char
ykine_stance_scale_head   (int *n, char *a_short)
{
   a_short [0]  = 'Z';
   a_short [1]  = '·';
   a_short [2]  = '\0';
   *n = 0;
   return 0;
}

char
ykine_stance_scale_next   (int *n, char *a_short)
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

float
yKINE_scale          (char *a_entry)
{
   double     x_val;
   ykine_stance_scale  (0.0, a_entry, &x_val);
   return x_val;
}



/*====================------------------------------------====================*/
/*===----                      stance verb handler                     ----===*/
/*====================------------------------------------====================*/
static void      o___STANCE__________________o (void) {;}

char
ykine_stance            (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   char       *x_one       [LEN_LABEL];
   char       *x_two       [LEN_LABEL];
   char       *x_thr       [LEN_LABEL];
   char       *x_label     [LEN_LABEL];
   char        x_recd      [LEN_RECD];
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(gather fields)---------------*/
   rc = ykine_legs_prepare (x_one, x_two, x_thr, x_label);
   DEBUG_YKINE_SCRP   yLOG_value   ("prepare"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(create record)------------------*/
   if (myKINE.b >= 0)   sprintf (x_recd, "zero    (%6.1f, 0.0r, 0.0r, %s, %s)", myKINE.b, x_thr, x_label);
   else                 sprintf (x_recd, "zero    (%s, 0.0r, 0.0r, %s, %s)"   , myKINE.accel, x_thr, x_label);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_recd"    , x_recd);
   /*---(parse)--------------------------*/
   rc = yPARSE_hidden (&(myKINE.s_nline), &(myKINE.s_cline), x_recd);
   myKINE.s_hidden = 'y';
   DEBUG_YKINE_SCRP   yLOG_point   ("parse"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)-------------------------*/
   rc = ykine_body_zero ();
   DEBUG_YKINE_SCRP  yLOG_point   ("zero"      , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(create record)------------------*/
   if (myKINE.b >= 0)   sprintf (x_recd, "radial  (AA, %6.1f, %s, %s, %s, %s)", myKINE.b, x_one, x_two, "-.-", x_label);
   else                 sprintf (x_recd, "radial  (AA, %s, %s, %s, %s, %s)"   , myKINE.accel, x_one, x_two, "-.-", x_label);
   DEBUG_YKINE_SCRP   yLOG_info    ("x_recd"    , x_recd);
   /*---(parse)--------------------------*/
   rc = yPARSE_hidden (&(myKINE.s_nline), &(myKINE.s_cline), x_recd);
   myKINE.s_hidden = 'y';
   DEBUG_YKINE_SCRP   yLOG_point   ("parse"     , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(run)-------------------------*/
   rc = ykine_legs_rk   ();
   DEBUG_YKINE_SCRP  yLOG_point   ("radial"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                verifying legal leg positions                 ----===*/
/*====================------------------------------------====================*/
static void      o___VERIFY__________________o (void) {;}

static char        d_str       [LEN_LABEL];
static char        o_str       [LEN_LABEL];
static char        y_str       [LEN_LABEL];
static int         s_row;
static int         s_col;

char
ykine_stance_verify_radius (char a_leg)
{
   char        rc;
   char        rcs;
   float       x, z, y;                       /* temporary coordinates     */
   /*---(background)---------------------*/
   rcs = ykine_stance_radius_head (&s_col, o_str);
   while (rcs >= 0) {
      if      (strcmp (y_str, "a·") == 0)        s_verify [s_row][s_col] = '´';
      else if (strcmp (o_str, "f·") == 0)        s_verify [s_row][s_col] = '|';
      else if (strcmp (o_str, "m·") == 0)        s_verify [s_row][s_col] = '¨';
      else if (strcmp (o_str, "t·") == 0)        s_verify [s_row][s_col] = '|';
      else                                       s_verify [s_row][s_col] = '·';
      rcs = ykine_stance_radius_next (&s_col, o_str);
   }
   /*---(values)-------------------------*/
   rcs = ykine_stance_radius_head (&s_col, o_str);
   while (rcs >= 0) {
      rc = ykine__legs_rk_getter (a_leg, d_str, o_str, y_str, &x, &z, &y);
      DEBUG_YKINE_SCRP  yLOG_complex ("position"  , "%8.2fx, %8.2fz, %8.2fy", x, z, y);
      /*---(check ik)--------------------*/
      rc = yKINE_inverse (a_leg, x, z, y);
      /*> printf ("%-10.10sd, %-10.10so, %-10.10sy     pos %8.2fx, %8.2fz, %8.2fy     rc %4d\n", d_str, o_str, y_str, x, z, y, rc);   <*/
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      if (rc >= 0)   s_verify [s_row][s_col] = '¬';
      rcs = ykine_stance_radius_next (&s_col, o_str);
   }
   s_verify [s_row][++s_col] = '\0';
   /*> switch (y_str [1]) {                                                           <* 
    *> case '·' : case '\'':  printf ("%s\n", s_verify [s_row]);  break;              <* 
    *> }                                                                              <*/
   return 0;
}

char
ykine_stance_verify     (void)
{
   char        rcs         =    0;
   char        t           [LEN_LABEL];
   int         i;
   /*---(clear)-----------------------*/
   for (s_row = 0; s_row < MAX_VERIFY; ++s_row) {
      for (s_col = 0; s_col < MAX_VERIFY; ++s_col) {
         s_verify [s_row][s_col] = '·';
      }
   }
   /*---(header)----------------------*/
   strcpy (d_str, "star");
   /*> sprintf (t, "%s configuration", d_str);                                                                          <* 
    *> printf ("\n%-20.20s ==================================================================================\n", t);   <* 
    *> printf ("  ");                                                                                                   <*/
   /*> rcs = ykine_stance_radius_head (&s_col, y_str);                                <* 
    *> while (rcs >= 0) {                                                             <* 
    *>    switch (y_str [1]) {                                                        <* 
    *>    case '+' :  case '-' :  printf  (" ");             break;                   <* 
    *>    case '·' :              printf  ("%c", y_str [0]); break;                   <* 
    *>    case '\'':              printf  ("'");             break;                   <* 
    *>    }                                                                           <* 
    *>    rcs = ykine_stance_radius_next (&s_col, y_str);                             <* 
    *> }                                                                              <* 
    *> printf ("\n");                                                                 <*/
   /*---(run)-------------------------*/
   rcs = ykine_stance_scale_head (&s_row, y_str);
   while (rcs >= 0) {
      /*> switch (y_str [1]) {                                                        <* 
       *> case '·' :   printf  ("%c ", y_str [0]);  break;                            <* 
       *> case '\'':   printf  ("' ");              break;                            <* 
       *> }                                                                           <*/
      ykine_stance_verify_radius    (YKINE_RR);
      rcs = ykine_stance_scale_next   (&s_row, y_str);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
yKINE_verify_rc            (int a_row, int a_col)
{
   return s_verify [a_row][a_col];
}



/*====================------------------------------------====================*/
/*===----                    hex converions                            ----===*/
/*====================------------------------------------====================*/
static void      o___HEXAGONAL_______________o (void) {;}

char
yKINE_xz2hexdo          (float x, float z, int *c, int *r, float *d, float *o)
{
   /*---(locals)-----------+-----+-----+-*/
   float       x_inch, xd, zd, fd, x_deg;
   int         x_base;
   int         rc          =    0;
   char        x_rig       =  '-';
   /*---(header)-------------------------*/
   /*> printf ("%8.2fx, %8.2fz   ", x, z);                                            <*/
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
   /*> printf ("  dir %8.2fd, %8.2fd   ", fd, x_deg);                                 <*/
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
   /*---(complete)-----------------------*/
   return rc;
}

char
yKINE_xz2hex            (float x, float z, int *c, int *r)
{
   return yKINE_xz2hexdo (x, z, c, r, NULL, NULL);
}

char
yKINE_hex2xz            (int c, int r, float *x, float *z)
{
   /*---(header)-------------------------*/
   /*> printf ("%6dr, %6dc\n", r, c);                                                 <*/
   *x = c * MM2COL;
   *z = r * MM2ROW;
   if (c % 2 != 0)  *z -= 14.664;
   /*---(complete)-----------------------*/
   return 0;
}



