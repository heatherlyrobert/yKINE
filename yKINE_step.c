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
 *  inverse  § AA § [15m] §  15.0r §  -.- §  -.-r  § -        § sn/fr        §
 *
 *  fr  = fast-ripple
 *  sn  = square-normal
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
struct cSHAPE {
   char        abbr;
   char        name        [LEN_LABEL];
   char        desc        [LEN_DESC];
};
tSHAPE      s_shapes    [MAX_SHAPE] = {
   { 's', "square"   , "straight up, over, stright down"                      },
   { 'a', "angle"    , "straight up, over, stright down"                      },
   { 't', "triangle" , "up to middle, then down to far point"                 },
   { 'h', "hexagon"  , "up, then up to middle, down above far, then down"     },
   { 'c', "curved"   , "up, over, and down in a curved motion"                },
   {  0 , "end-list" , ""                                                     },
};
int         s_nshape    = 0;
char        s_cshape    = '-';
char        s_cheight   = '-';



#define     MAX_SEQ     20
typedef struct cSEQ  tSEQ;
struct cSEQ {
   char        abbr;
   char        name        [LEN_LABEL];
   char        seq         [LEN_LABEL];
   float       min;
   char        desc        [LEN_DESC];
};
tSEQ        s_seqs      [MAX_SEQ] = {
   { 's', "spiral"         , "0.1.2.3.4.5" , 1.00, "rolling right to left"        },
   { 'S', "spiral (rev)"   , "0.1.2.3.4.5" , 1.00, "rolling left to right"        },
   { 'r', "ripple"         , "0.3.1.4.2.5" , 1.00, "back and forth opposing legs" },
   { 'R', "ripple (rev)"   , "5.2.4.1.3.0" , 1.00, "back and forth opposing legs" },
   { 'd', "double"         , "03.14.25"    , 1.00, "synchronize opposing pairs"   },
   { 'D', "double (rev)"   , "25.14.03"    , 1.00, "synchronize opposing pairs"   },
   { 't', "triple"         , "024.135"     , 1.00, "alternating triangles"        },
   { 'T', "triple (rev)"   , "135.024"     , 1.00, "alternating triangles"        },
   {  0 , "end-list"       , ""                                                   },
};
int         s_nseq      = 0;
char        s_cseq      = '-';
char        s_coff      = '-';




char
ykine_step_init         (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(steps)--------------------------*/
   s_nshape = 0;
   for (i = 0; i < MAX_SHAPE; ++i) {
      if (s_shapes [i].abbr == 0)  break;
      ++s_nshape;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nshape"  , s_nshape);
   s_cshape      = '-';
   s_cheight     = '-';
   myKINE.step_s = -1;
   myKINE.step_h = 0.0;
   /*---(sequencing)---------------------*/
   s_nseq  = 0;
   for (i = 0; i < MAX_SEQ ; ++i) {
      if (s_seqs  [i].abbr == 0)  break;
      ++s_nseq;
   }
   DEBUG_YKINE_SCRP   yLOG_value   ("s_nseq"    , s_nseq);
   s_cseq        = '-';
   s_coff        = '-';
   myKINE.seq    = -1;
   myKINE.off    = 0.0;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
ykine_step_shape        (char *a_step)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
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
   --rce;  if (x_len  <= 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find shape)---------------------*/
   for (i = 0; i < s_nshape; ++i) {
      if (s_shapes [i].abbr != a_step [0])  continue;
      myKINE.step_s = i;
      break;
   }
   --rce;  if (myKINE.step_s < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_cshape  = a_step [0];
   s_cheight = a_step [1];
   /*---(find height)--------------------*/
   --rce;  switch (a_step [1]) {
   case 'l' :   myKINE.step_h =  5.0;         break;
   case 'n' :   myKINE.step_h = 10.0;         break;
   case 'h' :   myKINE.step_h = 25.4;         break;
   case 'x' :   myKINE.step_h = 50.8;         break;
   default  :   myKINE.step_s =  -1;
                myKINE.step_h =  0.0;
                s_cshape  = '-';
                s_cheight = '-';
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
   s_coff        = '-';
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
   --rce;  if (x_len  <= 1) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find sequence)------------------*/
   for (i = 0; i < s_nseq; ++i) {
      if (s_seqs [i].abbr != a_seq [1])  continue;
      myKINE.seq    = i;
      break;
   }
   --rce;  if (myKINE.seq < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   s_cseq    = a_seq [1];
   s_coff    = a_seq [0];
   /*---(find speed)---------------------*/
   --rce;  switch (a_seq  [0]) {
   case 'c' :   myKINE.off    = 1.34;         break;
   case 'n' :   myKINE.off    = 1.00;         break;
   case 'f' :   myKINE.off    = 0.67;         break;
   case 'x' :   myKINE.off    = 0.34;         break;
   default  :   myKINE.off    = 0.00;
                myKINE.seq    = -1;
                s_cseq    = '-';
                s_coff    = '-';
                DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
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
   x_len = strlen (a_mods);
   DEBUG_YKINE_SCRP   yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len  <= 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(find divider)-------------------*/
   p = strchr (a_mods, "/");
   DEBUG_YKINE_SCRP   yLOG_point   ("p"         , p);
   if (p != NULL) {
      p = '\0';
      strlcpy (x_seq, p + 1, LEN_LABEL);
   }
   strlcpy (x_step, a_mods, LEN_LABEL);
   /*---(handle steps)-------------------*/
   if (x_step [0] != '\0') {
   }
   /*---(handle sequence)----------------*/
   if (x_seq  [0] != '\0') {
   }
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
      sprintf (ykine__unit_answer, "STEP global    : s %c/%2d, h %c/%5.1f, q %c/%2d, o %c/%5.2f", s_cshape, myKINE.step_s, s_cheight, myKINE.step_h, s_cseq, myKINE.seq, s_coff, myKINE.off);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}



