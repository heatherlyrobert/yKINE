/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"



tSERVO     g_servo_info  [YKINE_MAX_SERVO] = {
   /*---(big legs)--------------------*/
   /* label---------- leg------ seg-------- cnt  exact   curr  degs  xpos  zpos  ypos  --segno--  --coda--- scrp  sav  xpos  zpos  ypos  head  tail */
   { "RR.femu"      , YKINE_RR, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RR.pate"      , YKINE_RR, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RR.tibi"      , YKINE_RR, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RM.femu"      , YKINE_RM, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RM.pate"      , YKINE_RM, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RM.tibi"      , YKINE_RM, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RF.femu"      , YKINE_RF, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RF.pate"      , YKINE_RF, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RF.tibi"      , YKINE_RF, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LF.femu"      , YKINE_LF, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LF.pate"      , YKINE_LF, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LF.tibi"      , YKINE_LF, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LM.femu"      , YKINE_LM, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LM.pate"      , YKINE_LM, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LM.tibi"      , YKINE_LM, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LR.femu"      , YKINE_LR, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LR.pate"      , YKINE_LR, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LR.tibi"      , YKINE_LR, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   /*---(little legs)-----------------*/
   /* label---------- leg------ seg-------- cnt  exact   curr  degs  xpos  zpos  ypos  --segno--  --coda--- scrp  sav  xpos  zpos  ypos  head  tail */
   { "rr.femu"      , YKINE_RP, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rr.pate"      , YKINE_RP, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rr.tibi"      , YKINE_RP, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rf.femu"      , YKINE_RA, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rf.pate"      , YKINE_RA, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rf.tibi"      , YKINE_RA, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lf.femu"      , YKINE_RA, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lf.pate"      , YKINE_RA, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lf.tibi"      , YKINE_RA, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lr.femu"      , YKINE_RP, YKINE_FEMU,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lr.pate"      , YKINE_RP, YKINE_PATE,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lr.tibi"      , YKINE_RP, YKINE_TIBI,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   /*---(body)------------------------*/
   /* label---------- leg------ seg-------- cnt  exact   curr  degs  xpos  zpos  ypos  --segno--  --coda--- scrp  sav  xpos  zpos  ypos  head  tail */
   { "--.center"    , -1      , -1        ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   /*---(done)------------------------*/
   { "end-of-list"  , -1      , -1        ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
};
int         g_nservo;

static char     s_sides   [LEN_LABEL];
static int      s_nside   =    0;
static char     s_ranks   [LEN_LABEL];
static int      s_nrank   =    0;
static char     s_seg     [LEN_LABEL];



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void      o___PROGRAM_________________o (void) {;}

char  /*--> prepare for use ---------s-------------[ leaf   [ ------ ]-*/
ykine_servo_init        (void)
{
   int         i           = 0;
   g_nservo = 0;
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      if (g_servo_info [i].label [0] == 'e')   break;
      g_servo_info [i].saved  = '-';
      ++g_nservo;
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        support functions                     ----===*/
/*====================------------------------------------====================*/
static void      o___SUPPORT_________________o (void) {;}

char  /* file reading driver ----------------------[--------[--------]-*/
ykine_servo_prep   (void)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;          /* loop iterator                  */
   /*---(cycle)--------------------------*/
   for (i = 0; i < g_nservo; ++i) {
      g_servo_info [i].scrp = '-';
   }
   /*---(testing cleanup (DUP))----------*/
   strlcpy (s_sides , ""          , LEN_LABEL);
   s_nside = 0;
   strlcpy (s_ranks , ""          , LEN_LABEL);
   s_nrank = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char
ykine_servo_find        (int a_leg, int a_seg)
{
   /*---(locals)-----------+-----------+-*/
   int         i           =    0;          /* loop iterator                  */
   int         c           =    0;
   char        rc          =   -1;
   /*---(prepare)------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(cycle)--------------------------*/
   for (i = 0; i < g_nservo; ++i) {
      if (g_servo_info [i].leg != a_leg)  continue;
      if (g_servo_info [i].seg != a_seg)  continue;
      rc = i;
   }
   DEBUG_YKINE_SCRP   yLOG_sint    (rc);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        interpretation                        ----===*/
/*====================------------------------------------====================*/
static void      o___INTERPRET_______________o (void) {;}

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_servo_side        (char a_char)
{  /*---(design notes)-------------------*/
   /*
    *  bigs       : L=left  , R=right , A=all
    *  smalls     : l=left  , r=right , a=all
    *  both       : <=left  , >=right , *=all
    *  center     : -
    *
    */
   switch (a_char) {
   case 'L' : strlcpy (s_sides , "L"         , LEN_LABEL);   break;
   case 'R' : strlcpy (s_sides , "R"         , LEN_LABEL);   break;
   case 'l' : strlcpy (s_sides , "l"         , LEN_LABEL);   break;
   case 'r' : strlcpy (s_sides , "r"         , LEN_LABEL);   break;
   case '<' : strlcpy (s_sides , "Ll"        , LEN_LABEL);   break;
   case '>' : strlcpy (s_sides , "Rr"        , LEN_LABEL);   break;
   case 'A' : strlcpy (s_sides , "LR"        , LEN_LABEL);   break;
   case 'a' : strlcpy (s_sides , "lr"        , LEN_LABEL);   break;
   case '*' : strlcpy (s_sides , "LRlr"      , LEN_LABEL);   break;
   case '-' : strlcpy (s_sides , "-"         , LEN_LABEL);   break;
   default  : strlcpy (s_sides , ""          , LEN_LABEL);
              return -11;
              break;
   }
   s_nside = strlen (s_sides);
   return 0;
}

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_servo_rank        (char a_char)
{  /*---(design notes)-------------------*/
   /*
    *  bigs       : R=rear  , M=middle, F=front , A=all
    *  smalls     : r=rear  ,         , f=front , a=all
    *  both       : v=rear  ,         , ^=front , *=all
    *  center     : -
    *
    */
   switch (a_char) {
   case 'R' : strlcpy (s_ranks , "R"         , LEN_LABEL);   break;
   case 'M' : strlcpy (s_ranks , "M"         , LEN_LABEL);   break;
   case 'F' : strlcpy (s_ranks , "F"         , LEN_LABEL);   break;
   case 'r' : strlcpy (s_ranks , "r"         , LEN_LABEL);   break;
   case 'f' : strlcpy (s_ranks , "f"         , LEN_LABEL);   break;
   case '^' : strlcpy (s_ranks , "Ff"        , LEN_LABEL);   break;
   case 'v' : strlcpy (s_ranks , "Rr"        , LEN_LABEL);   break;
   case 'A' : strlcpy (s_ranks , "RMF"       , LEN_LABEL);   break;
   case 'a' : strlcpy (s_ranks , "rf"        , LEN_LABEL);   break;
   case '*' : strlcpy (s_ranks , "RMFrf"     , LEN_LABEL);   break;
   case '-' : strlcpy (s_ranks , "-"         , LEN_LABEL);   break;
   default  : strlcpy (s_ranks , ""          , LEN_LABEL);
              return -11;
              break;
   }
   s_nrank = strlen (s_ranks);
   return 0;
}

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_servo_segment     (char *a_char)
{  /*---(design notes)-------------------*/
   /*
    *  coxa = coxa
    *  troc = trocanter
    *  femu = femur
    *  pate = patella
    *  tibi = tibia
    *  meta = metatarsus
    *  tars = tarsus
    *  foot = foot
    *  claw = claw
    *  magn = magnet
    *  hook = hook
    *  full = all segment endpoint
    *  ---- = center
    *
    */
   char       *x_valid     = " femu pate tibi full center ";
   char        x_check     [LEN_LABEL];
   char       *p           = NULL;
   if (a_char == NULL)         return -11;
   if (strlen (a_char) <  4)   return -12;
   if (strlen (a_char) > 10)   return -13;
   sprintf (x_check, " %s ", a_char);
   if (strstr (x_valid, x_check) == NULL) {
      return -13;
   }
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       marking individuals                    ----===*/
/*====================------------------------------------====================*/
static void      o___MARKING_________________o (void) {;}

char  /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_servo_one         (char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   int         i           =   0;
   int         x_count     =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_snote   (a_source);
   /*---(cycle)--------------------------*/
   for (i = 0; i < g_nservo; ++i) {
      /*---(check legs first)------------*/
      if (a_source [0] != g_servo_info [i].label [0])       continue;
      if (a_source [1] != g_servo_info [i].label [1])       continue;
      /*---(check for all segments)------*/
      if (strcmp ("full", a_source + 3) == 0) {
         DEBUG_YKINE_SCRP   yLOG_snote   ("found (LEG)");
         g_servo_info [i].scrp = 'y';
         ++x_count;
         continue;
      }
      /*---(check for one segment)-------*/
      if (strcmp (a_source, g_servo_info [i].label) != 0)   continue;
      DEBUG_YKINE_SCRP   yLOG_snote   ("found");
      DEBUG_YKINE_SCRP   yLOG_sint    (i);
      g_servo_info [i].scrp = 'y';
      ++x_count;
      break;
   }
   --rce;  if (x_count == 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("NONE");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_svalue  ("count"     , x_count);
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return x_count;
}

char  /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_servos            (char *a_source)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors   */
   char        rc          =    0;                /* generic return code      */
   int         i           = 0;
   int         j           = 0;
   int         c           = 0;
   int         x_index     = -1;
   int         x_nside     = 0;
   int         x_nrank     = 0;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_point   ("a_source"  , a_source);
   /*---(defense)------------------------*/
   --rce;  if (a_source == NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_info    ("a_source"  , a_source);
   --rce;  if (strlen (a_source) <  7) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strlen (a_source) > 13) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(interpret side-to-side)---------*/
   rc = ykine_servo_side (a_source [0]);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_info    ("s_sides"   , s_sides);
   /*---(interpret front-to-back)--------*/
   rc = ykine_servo_rank (a_source [1]);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_info    ("s_ranks"   , s_ranks);
   /*---(interpret segment)--------------*/
   rc = ykine_servo_segment (a_source + 3);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cycle)--------------------------*/
   for (i = 0; i < s_nside; ++i) {
      for (j = 0; j < s_nrank; ++j) {
         sprintf (x_label, "%c%c.%s", s_sides [i], s_ranks [j], a_source + 3);
         DEBUG_YKINE_SCRP  yLOG_info    ("x_label"   , x_label);
         x_index = ykine_servo_one (x_label);
         if (x_index > 0)  ++c;
      }
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("c"         , c);
   --rce;  if (c <= 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return c;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__unit_servo       (char *a_question)
{
   int         i           =    0;
   int         x_pos       =    0;
   char        x_msg       [LEN_STR];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "SCRP unit        : question not understood", LEN_STR);
   /*---(answer)------------------------------------------*/
   if (strcmp (a_question, "sides"   ) == 0) {
      sprintf (ykine__unit_answer, "SERVO sides    : %d sides=%-5.5s, %d ranks=%s", s_nside, s_sides, s_nrank, s_ranks);
   }
   else if (strcmp (a_question, "servos"  ) == 0) {
      for (i = 0; i < YKINE_MAX_SERVO; ++i) {
         x_msg [x_pos] = 0;
         if (g_servo_info [i].label [0] == 'e')  break;
         if (x_pos > 0 && ((x_pos + 1) % 4) == 0) {
            x_msg [x_pos] = ' ';
            ++x_pos;
         }
         if (g_servo_info [i].scrp == 'y')  x_msg [x_pos] = g_servo_info [i].label [3];
         else                           x_msg [x_pos] = '_';
         ++x_pos;
      }
      sprintf (ykine__unit_answer, "SERVO servos   : %s", x_msg);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}


