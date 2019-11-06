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
   { '-', "--", "straight" , "fastest and most direct"            , "default with no changes to original"                  },
   { 's', "sq", "square"   , "perfect for uneven surfaces"        , "up, horizontal, down -- perfect for uneven surfaces"  },
   { 'S', "rs", "rsquare"  , "faster square using acceleration"   , "up, little curve, horizontal, little curve and down"  },
   { 'd', "di", "direct"   , "perfect for angled surfaces"        , "up, incline/decline over, down"                       },
   { 'D', "rd", "rdirect"  , "faster direct using acceleration"   , "up, little curve, over, little curve and down"        },
   { 't', "tr", "triangle" , "for getting over low objects"       , "up to high middle, then down to far point"            },
   { 'T', "rt", "rtriangle", "faster triangle using acceleration" , "up to high middle, curved, then down to far point"    },
   { 'p', "pu", "pullup"   , "for getting over larger objects"    , "up, then to to low middle, down to far, then down"    },
   { 'h', "hx", "hexagon"  , "for getting over rough terrain"     , "up, then up to high middle, down above far, then down"},
   { 'a', "ar", "arc"      , "more natural, low curving"          , "gentle arcing motion"                                 },
   { 'b', "br", "bridge"     "quite spider-like movement"         , "up, over, and down in a low curved motion"            },
   { 'c', "cv", "curve"    , "cautious spider-like movement"      , "up, over, and down in a high curved motion"           },
   {  0 , "--", "end-list" , ""                                   , ""                                                     },
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
   { 'z', "ze", "zero"     ,  0.0, "no upward movement"                                   },
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
   s_cshape      = '-';
   myKINE.step_s = -1;
   /*---(heights)------------------------*/
   s_nheight = 0;
   for (i = 0; i < MAX_HEIGHT; ++i) {
      if (s_heights [i].abbr == 0)  break;
      ++s_nheight;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nheight" , s_nheight);
   s_cheight     = '-';
   myKINE.step_h = 0.0;
   /*---(speeds)-------------------------*/
   s_nspeed  = 0;
   for (i = 0; i < MAX_SPEED ; ++i) {
      if (s_speeds  [i].abbr == 0)  break;
      ++s_nspeed;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nspeed"  , s_nspeed);
   s_cspeed      = '-';
   myKINE.off    = 0.0;
   /*---(sequencing)---------------------*/
   s_nseq  = 0;
   for (i = 0; i < MAX_SEQ ; ++i) {
      if (s_seqs  [i].abbr == 0)  break;
      ++s_nseq;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nseq"    , s_nseq);
   s_cseq        = '-';
   myKINE.seq    = -1;
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
   /*---(prepare)------------------------*/
   s_cshape      = '-';
   s_cheight     = '-';
   myKINE.step_s = -1;
   myKINE.step_h = 0.0;
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
   for (i = 0; i < s_nshape; ++i) {
      if (s_shapes [i].terse [0] != a_step [0])  continue;
      if (s_shapes [i].terse [1] != a_step [1])  continue;
      s_cshape  = s_shapes [i].abbr;
      myKINE.step_s = i;
      break;
   }
   --rce;  if (s_cshape == '-') {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find height)--------------------*/
   for (i = 0; i < s_nheight; ++i) {
      if (s_heights [i].terse [0] != a_step [3])  continue;
      if (s_heights [i].terse [1] != a_step [4])  continue;
      s_cheight = s_heights [i].abbr;
      myKINE.step_h = s_heights [i].value;
      break;
   }
   --rce;  if (s_cheight == '-') {
      s_cshape      = '-';
      myKINE.step_s = -1;
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
   /*---(prepare)------------------------*/
   s_cseq        = '-';
   s_cspeed      = '-';
   myKINE.seq    = -1;
   myKINE.off    = 0.0;
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
   /*---(defense)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_point   ("a_mods"    , a_mods);
   --rce;  if (a_mods == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*> x_len = strlen (a_mods);                                                       <* 
    *> DEBUG_YKINE_SCRP   yLOG_value   ("x_len"     , x_len);                         <* 
    *> --rce;  if (x_len  <= 0) {                                                     <* 
    *>    DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);                        <* 
    *>    return rce;                                                                 <* 
    *> }                                                                              <*/
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
ykine_step_raise       (char a_verb, char a_leg, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   switch (a_verb) {
   case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI : case YKINE_FK :
      DEBUG_YKINE_SCRP  yLOG_note    ("no raise required body/fk moves");
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("step_s"    , myKINE.step_s);
   --rce;  if (myKINE.step_s < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(increate height)-------------*/
   s_yb = myKINE.yb;
   s_ye = myKINE.ye;
   switch (s_cshape) {
   case 's' :
      DEBUG_YKINE_SCRP  yLOG_note    ("square handler");
      if (s_yb >= s_ye) myKINE.ye = myKINE.yb += myKINE.step_h;
      else              myKINE.yb = myKINE.ye += myKINE.step_h;
      break;
   case 'd' :
      DEBUG_YKINE_SCRP  yLOG_note    ("direct handler");
      myKINE.yb += myKINE.step_h;
      myKINE.ye += myKINE.step_h;
      break;
   }
   DEBUG_YKINE_SCRP   yLOG_complex ("y-pos"     , "yb1 %6.1f, yb2 %6.1f, ye1 %6.1f, ye2 %6.1f", myKINE.yb, s_yb, myKINE.ye, s_ye);
   /*---(inverse kinematics)----------*/
   rc = yKINE_inverse (a_leg, myKINE.xb, myKINE.zb, myKINE.yb);
   DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);
   /*---(get angles)------------------*/
   rc = yKINE_angles  (a_leg, YKINE_IK, &myKINE.cb, &myKINE.fb, &myKINE.pb, &myKINE.tb);
   DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fb, myKINE.pb, myKINE.tb);
   /*---(create moves)-------------------*/
   ykine_accel__single (a_verb, a_leg, myKINE.fb, myKINE.pb, myKINE.tb, 2.0, a_label, '>');
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_step_plant       (char a_verb, char a_leg, char *a_label)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   switch (a_verb) {
   case YKINE_ZE : case YKINE_PO : case YKINE_OR : case YKINE_TI : case YKINE_FK :
      DEBUG_YKINE_SCRP  yLOG_note    ("no raise required body/fk moves");
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("step_s"    , myKINE.step_s);
   --rce;  if (myKINE.step_s < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(decrease height)-------------*/
   myKINE.yb  = s_yb;
   myKINE.ye  = s_ye;
   /*---(inverse kinematics)----------*/
   rc = yKINE_inverse (a_leg, myKINE.xe, myKINE.ze, myKINE.ye);
   DEBUG_YKINE_SCRP  yLOG_value   ("kinematics", rc);
   /*---(get angles)------------------*/
   rc = yKINE_angles  (a_leg, YKINE_IK, &myKINE.ce, &myKINE.fe, &myKINE.pe, &myKINE.te);
   DEBUG_YKINE_SCRP   yLOG_complex ("degrees"   , "%8.3ff, %8.3fp, %8.3ft", myKINE.fe, myKINE.pe, myKINE.te);
   /*---(create moves)-------------------*/
   ykine_accel__single (a_verb, a_leg, myKINE.fe, myKINE.pe, myKINE.te, 2.0, a_label, '>');
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
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
   if (strcmp (a_question, "global"  ) == 0) {
      sprintf (ykine__unit_answer, "STEP global    : s %c/%2d, h %c/%5.1f, q %c/%2d, o %c/%5.2f", s_cshape, myKINE.step_s, s_cheight, myKINE.step_h, s_cseq, myKINE.seq, s_cspeed, myKINE.off);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}



