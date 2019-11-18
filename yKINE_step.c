/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"

/*===[[ DESIGN NOTES ]]=======================================================*/

/*
 *  steps can be any orientation...
 *     -- normal is up, over ,down
 *     -- hooks are down, over, up
 *     -- magnets and claws are any and all angles
 *
 *
 *  inverse  § AA § [15m] §  15.0r §  -.- §  -.-r  § -        § sq-nm/f-ripple   §
 *
 *  sq-hi/f-ripple
 *  an-lo/c-spiral
 *  tr-nm/
 *  hx-ex
 *  cu
 *
 *
 */

/*===[[ METIS ]]==============================================================*/

/*
 * metis  dn2··  develop a list of step types and standards
 * metis  dn8··  function to change a simple leg move into a true step
 *
 */

#define     YKINE_STRAIGHT  '>'
#define     YKINE_SSQUARE   's'
#define     YKINE_RSQUARE   'S'
#define     YKINE_SDIRECT   'd'
#define     YKINE_RDIRECT   'D'

#define     MAX_SHAPE   20
typedef struct cSHAPE tSHAPE;
static struct cSHAPE {
   char        abbr;
   char        terse       [LEN_TERSE];
   char        name        [LEN_LABEL];
   char        reason      [LEN_DESC];
   char        desc        [LEN_DESC];
};
static tSHAPE      s_shapes    [MAX_SHAPE] = {
   { YKINE_STRAIGHT , "--", "straight" , "fastest and most direct"            , "default with no changes to original"                  },
   { YKINE_SSQUARE  , "ss", "square"   , "perfect for uneven surfaces"        , "up, horizontal, down -- perfect for uneven surfaces"  },
   { YKINE_RSQUARE  , "rs", "rsquare"  , "faster square using acceleration"   , "up, little curve, horizontal, little curve and down"  },
   { YKINE_SDIRECT  , "sd", "direct"   , "perfect for angled surfaces"        , "up, incline/decline over, down"                       },
   { YKINE_RDIRECT  , "rd", "rdirect"  , "faster direct using acceleration"   , "up, little curve, over, little curve and down"        },
   { 't'            , "st", "triangle" , "for getting over low objects"       , "up to high middle, then down to far point"            },
   { 'T'            , "rt", "rtriangle", "faster triangle using acceleration" , "up to high middle, curved, then down to far point"    },
   { 'p'            , "pu", "pullup"   , "for getting over larger objects"    , "up, then to to low middle, down to far, then down"    },
   { 'h'            , "hx", "hexagon"  , "for getting over rough terrain"     , "up, then up to high middle, down above far, then down"},
   { 'a'            , "ar", "arc"      , "more natural, low curving"          , "gentle arcing motion"                                 },
   { 'b'            , "br", "bridge"     "quite spider-like movement"         , "up, over, and down in a low curved motion"            },
   { 'c'            , "cv", "curve"    , "cautious spider-like movement"      , "up, over, and down in a high curved motion"           },
   {  0             , "--", "end-list" , ""                                   , ""                                                     },
};
static int         s_nshape    = 0;
static char        s_cshape    = '-';



#define     MAX_HEIGHT  10
typedef struct cHEIGHT tHEIGHT;
static struct cHEIGHT {
   char        abbr;
   char        terse       [LEN_TERSE];
   char        name        [LEN_LABEL];
   float       value;
   char        desc        [LEN_DESC];
};
static tHEIGHT      s_heights   [MAX_HEIGHT] = {
   { 'z', "--", "zero"     ,  0.0, "no upward movement"                                   },
   { 's', "sc", "scrape"   ,  3.0, "very low height steps"                                },
   { 'l', "lo", "low"      ,  6.1, "low height steps"                                     },
   { 'n', "nm", "normal"   , 12.2, "normal height steps"                                  },
   { 'h', "hi", "high"     , 25.4, "high height steps"                                    },
   { 'x', "ex", "extra"    , 37.6, "extra high height steps"                              },
   {  0 , "--", "end-list" ,  0.0, ""                                                     },
};
static int         s_nheight   = 0;
static char        s_cheight   = '-';



#define     MAX_SPEED   10
typedef struct cSPEED tSPEED;
static struct cSPEED {
   char        abbr;
   char        name        [LEN_LABEL];
   float       value;
   char        desc        [LEN_DESC];
};
static tSPEED      s_speeds   [MAX_SPEED] = {
   { 'j', "ninja"    , 2.00, "steps have huge buffer between"  },
   { 's', "slow"     , 1.67, "steps have large buffer between" },
   { 'c', "cautious" , 1.34, "steps have small buffer between" },
   { 'n', "normal"   , 1.00, "steps are exactly end-to-start"  },
   { 'b', "brisk"    , 0.67, "steps are slightly overlapped"   },
   { 'f', "fast"     , 0.34, "steps are very overlapped"       },
   {  0 , "end-list" , 0.00, ""                                },
};
static int         s_nspeed    = 0;
static char        s_cspeed    = '-';



#define     MAX_SEQ     20
typedef struct cSEQ  tSEQ;
struct cSEQ {
   char        abbr;
   char        name        [LEN_LABEL];
   char        seq         [LEN_LABEL];
   int         grps;
   float       min;
   char        desc        [LEN_DESC];
};
tSEQ        s_seqs      [MAX_SEQ] = {
   { 's', "spiral"         , "0.1.2.3.4.5" , 6, 0.34, "rolling right to left"        },
   { 'S', "r.spiral"       , "5.4.3.2.1.0" , 6, 0.34, "rolling left to right"        },
   { 'r', "ripple"         , "0.3.1.4.2.5" , 6, 0.34, "back and forth opposing legs" },
   { 'R', "r.ripple"       , "5.2.4.1.3.0" , 6, 0.34, "back and forth opposing legs" },
   { 'a', "alternate"      , "0.2.4.1.5.5" , 6, 0.34, "triple sequence"              },
   { 'A', "r.alternate"    , "5.3.1.4.2.0" , 6, 0.34, "triple sequence"              },
   { 'd', "double"         , "03.14.25"    , 3, 1.00, "synchronize opposing pairs"   },
   { 'D', "r.double"       , "25.14.03"    , 3, 1.00, "synchronize opposing pairs"   },
   { 't', "triple"         , "024.135"     , 2, 1.00, "alternating triangles"        },
   { 'T', "r.triple"       , "135.024"     , 2, 1.00, "alternating triangles"        },
   {  0 , "end-list"       , ""                                                   },
};
int         s_nseq      = 0;
char        s_cseq      = '-';



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void      o___PROGRAM_________________o (void) {;};

char
ykine_step__defaults    (void)
{
   s_cshape      = YKINE_STRAIGHT;
   myKINE.step_s =   0;
   s_cheight     = 'z';
   myKINE.step_h = 0.0;
   s_cspeed      = '-';
   myKINE.off    = 0.0;
   s_cseq        = '-';
   myKINE.seq    = -1;
   return 0;
}

char
ykine_step_init         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(shapes)-------------------------*/
   s_nshape = 0;
   for (i = 0; i < MAX_SHAPE; ++i) {
      if (s_shapes [i].abbr == 0)  break;
      ++s_nshape;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nshape"  , s_nshape);
   /*---(heights)------------------------*/
   s_nheight = 0;
   for (i = 0; i < MAX_HEIGHT; ++i) {
      if (s_heights [i].abbr == 0)  break;
      ++s_nheight;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nheight" , s_nheight);
   /*---(speeds)-------------------------*/
   s_nspeed  = 0;
   for (i = 0; i < MAX_SPEED ; ++i) {
      if (s_speeds  [i].abbr == 0)  break;
      ++s_nspeed;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nspeed"  , s_nspeed);
   /*---(sequencing)---------------------*/
   s_nseq  = 0;
   for (i = 0; i < MAX_SEQ ; ++i) {
      if (s_seqs  [i].abbr == 0)  break;
      ++s_nseq;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nseq"    , s_nseq);
   /*---(set defaults)-------------------*/
   ykine_step__defaults ();
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        input parsing                         ----===*/
/*====================------------------------------------====================*/
static void      o___PARSING_________________o (void) {;};

char
ykine_step_shape        (char *a_step)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
   char        x_shape     [LEN_TERSE] = "";
   char        x_height    [LEN_TERSE] = "";
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("a_step"    , a_step);
   --rce;  if (a_step == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_len = strlen (a_step);
   DEBUG_YKINE_SCRP   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len  != 5) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_char    ("delim"     , a_step [2]);
   --rce;  if (a_step [2] != '-') {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find shape)---------------------*/
   s_cshape  = '-';
   for (i = 0; i < s_nshape; ++i) {
      if (s_shapes [i].terse [0] != a_step [0])  continue;
      if (s_shapes [i].terse [1] != a_step [1])  continue;
      s_cshape  = s_shapes [i].abbr;
      myKINE.step_s = i;
      break;
   }
   --rce;  if (s_cshape == '-') {
      ykine_step__defaults ();
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find height)--------------------*/
   s_cheight = '-';
   for (i = 0; i < s_nheight; ++i) {
      if (s_heights [i].terse [0] != a_step [3])  continue;
      if (s_heights [i].terse [1] != a_step [4])  continue;
      s_cheight = s_heights [i].abbr;
      myKINE.step_h = s_heights [i].value;
      break;
   }
   --rce;  if (s_cheight == '-') {
      ykine_step__defaults ();
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_step_seq          (char *a_seq)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("a_seq"     , a_seq);
   --rce;  if (a_seq == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_len = strlen (a_seq);
   DEBUG_YKINE_SCRP   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len  <= 6) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_char    ("delim"     , a_seq [1]);
   --rce;  if (a_seq [1] != '-') {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find speed)---------------------*/
   for (i = 0; i < s_nspeed; ++i) {
      if (s_speeds [i].abbr != a_seq [0])  continue;
      s_cspeed   = s_speeds [i].abbr;
      myKINE.off = s_speeds [i].value;
      break;
   }
   --rce;  if (s_cspeed == '-') {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find sequence)------------------*/
   for (i = 0; i < s_nseq; ++i) {
      if (strcmp (s_seqs [i].name, a_seq + 2) != 0)  continue;
      s_cseq     = s_seqs   [i].abbr;
      myKINE.seq = i;
      break;
   }
   --rce;  if (s_cseq == '-') {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      s_cspeed      = '-';
      myKINE.off    = 0.0;
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_stepping          (char *a_mods)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_len       =    0;
   char       *p           = NULL;
   char        x_step      [LEN_LABEL] = "";
   char        x_seq       [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   ykine_step__defaults ();
   --rce;  if (myKINE.b >= 0.0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("a_mods"    , a_mods);
   --rce;  if (a_mods == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find divider)-------------------*/
   p = strchr (a_mods, '/');
   DEBUG_YKINE_SCRP   yLOG_point   ("p"         , p);
   if (p != NULL) {
      strlcpy (x_seq, p + 1, LEN_LABEL);
      strlcpy (x_step, a_mods, p - a_mods + 1);
   } else  strlcpy (x_step, a_mods, LEN_LABEL);
   /*> printf ("step <%s>, seq <%s>\n", x_step, x_seq);                               <*/
   /*---(handle)-------------------------*/
   if (rc == 0)  rc = ykine_step_shape (x_step);
   DEBUG_YKINE_SCRP   yLOG_value   ("shape"     , rc);
   if (rc == 0)  rc = ykine_step_seq   (x_seq);
   else          rc = ykine_step_seq   ("");
   DEBUG_YKINE_SCRP   yLOG_value   ("seq"       , rc);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      bookend moves                           ----===*/
/*====================------------------------------------====================*/
static void      o___BOOKENDS________________o (void) {;};

static float  s_yb   = 0.0;
static float  s_ye   = 0.0;

char
ykine_step_accels       (void)
{
   char        x_end       =    0;
   strlcpy (myKINE.a_raise , "", LEN_LABEL);
   strlcpy (myKINE.a_middle, "", LEN_LABEL);
   strlcpy (myKINE.a_plant , "", LEN_LABEL);
   if (myKINE.b >= 0.0)  return 0;
   switch (s_cshape) {
   case YKINE_STRAIGHT :
      ykine_accel_make (myKINE.a_acceln, myKINE.a_exact, myKINE.a_speedn, myKINE.a_deceln, myKINE.a_middle);
      break;
   case YKINE_SSQUARE  : case YKINE_SDIRECT  :
      x_end = ACCEL_TURTLE; /* fastest through sharp corners  */
      if (myKINE.a_speedn < ACCEL_TURTLE)  x_end = myKINE.a_speedn;
      ykine_accel_make (myKINE.a_acceln, myKINE.a_exact, myKINE.a_speedn, x_end          , myKINE.a_raise );
      ykine_accel_make (x_end          , myKINE.a_exact, myKINE.a_speedn, x_end          , myKINE.a_middle);
      ykine_accel_make (x_end          , myKINE.a_exact, myKINE.a_speedn, myKINE.a_deceln, myKINE.a_plant );
      break;
   case YKINE_RSQUARE  : case YKINE_RDIRECT  :
      x_end = ACCEL_MOD;  /* fastest through curved corners  */
      if (myKINE.a_speedn < ACCEL_MOD)     x_end = myKINE.a_speedn;
      ykine_accel_make (myKINE.a_acceln, myKINE.a_exact, myKINE.a_speedn, x_end          , myKINE.a_raise );
      ykine_accel_make (x_end          , myKINE.a_exact, myKINE.a_speedn, x_end          , myKINE.a_middle);
      ykine_accel_make (x_end          , myKINE.a_exact, myKINE.a_speedn, myKINE.a_deceln, myKINE.a_plant );
      break;
   }
   return 0;
}

char
ykine_step_raise       (char a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_accel     [LEN_LABEL] = "";
   float       xe, ze, ye;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_value   ("b"         , myKINE.b);
   --rce;  if (myKINE.b >= 0.0)  {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  switch (a_verb) {
   case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI : case YKINE_FK :
      DEBUG_YKINE_SCRP   yLOG_note    ("no raise required body/fk moves");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("step_s"    , myKINE.step_s);
   --rce;  if (myKINE.step_s <= 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save endpoint)---------------*/
   xe         = myKINE.xe;
   ze         = myKINE.ze;
   s_ye       = myKINE.ye;
   /*---(set for raise)---------------*/
   myKINE.xe  = myKINE.xb;
   myKINE.ze  = myKINE.zb;
   /*---(increase height)-------------*/
   switch (s_cshape) {
   case YKINE_SSQUARE : case YKINE_RSQUARE :
      DEBUG_YKINE_SCRP  yLOG_note    ("square handler");
      if (myKINE.yb >= myKINE.ye)  myKINE.ye = myKINE.yb + myKINE.step_h;
      else                         myKINE.ye = myKINE.ye + myKINE.step_h;
      ye         = myKINE.ye;
      break;
   case YKINE_SDIRECT : case YKINE_RDIRECT :
      DEBUG_YKINE_SCRP  yLOG_note    ("direct handler");
      myKINE.ye = myKINE.yb + myKINE.step_h;
      ye        = s_ye      + myKINE.step_h;
      break;
      break;
   }
   /*---(inverse kinematics)----------*/
   rc = yKINE_inverse (myKINE.leg, myKINE.xe, myKINE.ze, myKINE.ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);
   rc = yKINE_angles  (myKINE.leg, YKINE_IK, &myKINE.ce, &myKINE.fe, &myKINE.pe, &myKINE.te);
   /*---(create moves)-------------------*/
   DEBUG_YKINE_SCRP   yLOG_complex ("beg"       , "%6.1fx, %6.1fz, %6.1fy", myKINE.xb, myKINE.zb, myKINE.yb);
   DEBUG_YKINE_SCRP   yLOG_complex ("end"       , "%6.1fx, %6.1fz, %6.1fy", myKINE.xe, myKINE.ze, myKINE.ye);
   DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fe, myKINE.pe, myKINE.te);
   ykine_accel_append (YKINE_IK, 'r', myKINE.a_raise);
   /*---(put endpoint back)-----------*/
   myKINE.xe = xe;
   myKINE.ze = ze;
   myKINE.yb = myKINE.ye;
   myKINE.ye = ye;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_step_plant       (char a_verb)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_value   ("b"         , myKINE.b);
   --rce;  if (myKINE.b >= 0.0)  {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  switch (a_verb) {
   case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI : case YKINE_FK :
      DEBUG_YKINE_SCRP   yLOG_note    ("no raise required body/fk moves");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("step_s"    , myKINE.step_s);
   --rce;  if (myKINE.step_s <= 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(set for raise)---------------*/
   myKINE.xb  = myKINE.xe;
   myKINE.zb  = myKINE.ze;
   myKINE.yb  = myKINE.ye;
   /*---(decrease height)-------------*/
   myKINE.ye  = s_ye;
   /*---(inverse kinematics)----------*/
   rc = yKINE_inverse (myKINE.leg, myKINE.xe, myKINE.ze, myKINE.ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);
   rc = yKINE_angles  (myKINE.leg, YKINE_IK, &myKINE.ce, &myKINE.fe, &myKINE.pe, &myKINE.te);
   /*---(create moves)-------------------*/
   DEBUG_YKINE_SCRP   yLOG_complex ("beg"       , "%6.1fx, %6.1fz, %6.1fy", myKINE.xb, myKINE.zb, myKINE.yb);
   DEBUG_YKINE_SCRP   yLOG_complex ("end"       , "%6.1fx, %6.1fz, %6.1fy", myKINE.xe, myKINE.ze, myKINE.ye);
   DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fe, myKINE.pe, myKINE.te);
   ykine_accel_append (YKINE_IK, 'p', myKINE.a_plant);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     y-position shapers                       ----===*/
/*====================------------------------------------====================*/
static void      o___YSHAPERS________________o (void) {;};

char
ykine_step__rounded     (float a_xzlen, float a_pct, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   float       x_dist      =  0.0;
   char        x_stage     =    0;
   float       x_frac      =  0.0;
   float       x_round     =  5.0;
   float       x_mid       =  0.0;
   float       x_rev       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_complex ("args"      , "%6.1fxz, %6.3fp, %p", a_xzlen, a_pct, y);
   /*---(defense)------------------------*/
   --rce;  if (y == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(determine stage)----------------*/
   x_mid  = a_xzlen / 2.0;
   x_dist = a_pct * a_xzlen;
   x_rev  = a_xzlen - x_dist;
   if (x_dist < 5.0)   x_stage += 1;
   if (x_rev  < 5.0)   x_stage += 2;
   DEBUG_YKINE_SCRP   yLOG_complex ("calcs"     , "mid %6.1f, rev %6.1f, rev %6.1f, stage %d", x_mid, x_dist, x_rev, x_stage);
   /*---(deal with contension)-----------*/
   if (x_stage == 3) {
      if (x_dist <= x_mid)  x_stage = 1;
      else                  x_stage = 2;
      DEBUG_YKINE_SCRP   yLOG_value   ("new stage" , x_stage);
   }
   /*---(handle adjustment)--------------*/
   x_frac = (3.1415927 / 2.0) / x_round;
   switch (x_stage) {
   case  0 :
      DEBUG_YKINE_SCRP   yLOG_note    ("middle stage");
      *y += x_round;
      break;
   case  1 :
      DEBUG_YKINE_SCRP   yLOG_note    ("raise stage");
      *y += sin (x_dist * x_frac) * x_round;
      break;
   case  2 :
      DEBUG_YKINE_SCRP   yLOG_note    ("plant stage");
      *y += sin (x_rev * x_frac) * x_round;
      break;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("new y"     , *y);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_step_yshaper      (float a_xzlen, float a_pct, float *y)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_complex ("args"      , "%6.1fxz, %6.3fp, %p", a_xzlen, a_pct, y);
   DEBUG_YKINE_SCRP   yLOG_char    ("cshape"    , s_cshape);
   switch (s_cshape) {
   case YKINE_STRAIGHT :
      DEBUG_YKINE_SCRP   yLOG_note    ("straight, nothing to do");
      break;
   case YKINE_SSQUARE  : case YKINE_SDIRECT  :
      DEBUG_YKINE_SCRP   yLOG_note    ("sharp, nothing to do");
      break;
   case YKINE_RSQUARE  : case YKINE_RDIRECT  :
      DEBUG_YKINE_SCRP   yLOG_note    ("rounded, calling handler");
      rc = ykine_step__rounded (a_xzlen, a_pct, y);
      break;
   default :
      DEBUG_YKINE_SCRP   yLOG_note    ("unknown, nothing to do");
      break;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine_step__unit        (char *a_question, int a_num)
{
   int         i           =    0;
   int         x_pos       =    0;
   char        x_msg       [LEN_STR];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "STEP unit        : question not understood", LEN_STR);
   /*---(answer)------------------------------------------*/
   if      (strcmp (a_question, "global"  ) == 0) {
      sprintf (ykine__unit_answer, "STEP global    : s %c/%2d, h %c/%5.1f, q %c/%2d, o %c/%5.2f", s_cshape, myKINE.step_s, s_cheight, myKINE.step_h, s_cseq, myKINE.seq, s_cspeed, myKINE.off);
   }
   else if (strcmp (a_question, "accels"  ) == 0) {
      sprintf (ykine__unit_answer, "STEP accels    : [ r %-6.6s, m %-6.6s, p %-6.6s ]", myKINE.a_raise, myKINE.a_middle, myKINE.a_plant);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}



