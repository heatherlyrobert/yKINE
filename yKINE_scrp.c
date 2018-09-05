/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


#define     MOVE_NULL   '-'
#define     MOVE_PAUSE  'p'
#define     MOVE_SERVO  's'





static char  yKINE__scrp_fk_pure     (char *a_verb);

#define    PARSE_ANGLE        'a'
#define    PARSE_POINT        'p'
#define    PARSE_OTHER        '-'

#define    MAX_VERBS       100
typedef    struct  cVERBS  tVERBS;
struct cVERBS {
   char        name        [LEN_LABEL];     /* full verb name                 */
   char        vers        [LEN_LABEL];     /* allowed versions               */
   char        type;                        /* field interpretation type      */
   char        desc        [LEN_STR  ];     /* english description            */
};
tVERBS   s_verb_info    [MAX_VERBS] = {
   /* ===[[ inverse kinematics ]]================================================*/
   /* verb----------- vers--------- type-- description---------------------------------------- */
   { "ik_pure"      , "a"         , PARSE_POINT , "set an exact endpoint in 3d space"                 },
   { "ik_from"      , "a"         , PARSE_POINT , "set a relative endpoint based on last position"    },
   /* ===[[ forward kinematics ]]================================================*/
   /* verb----------- vers--------- type-- description---------------------------------------- */
   { "fk_pure"      , "a"         , PARSE_ANGLE , "set absolute joint angles on all three joints"     },
   { "fk_from"      , "a"         , PARSE_ANGLE , "set relative joint angles based on last angle"     },
   /* ===[[ body pos/orient ]]===================================================*/
   /* verb----------- vers--------- type-- description---------------------------------------- */
   { "or_pure"      , "a"         , PARSE_ANGLE , "set absolute body orientation angles"              },
   { "or_from"      , "a"         , PARSE_ANGLE , "set relative body orientation from last position"  },
   { "ze_pure"      , "a"         , PARSE_POINT , "set absolute body position in 3d space"            },
   { "ze_from"      , "a"         , PARSE_POINT , "set relative body position based on last position" },
   /* ===[[ music notation ]]====================================================*/
   /* verb----------- vers--------- type-- description---------------------------------------- */
   { "REPEAT"       , "a"         , PARSE_OTHER , "repeat a specific number of steps"                 },
   /* done-------------------*/
};

tSERVO     g_servo_info  [YKINE_MAX_SERVO] = {
   /*---(big legs)--------------------*/
   /* label--------   cnt  exact   curr  degs  xpos  zpos  ypos  --segno--  --coda--- scrp  sav  xpos  zpos  ypos  head  tail */
   { "RR.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RR.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RR.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RM.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RM.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RM.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RF.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RF.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "RF.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LF.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LF.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LF.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LM.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LM.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LM.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LR.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LR.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "LR.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   /* label--------   cnt  exact   curr  degs  xpos  zpos  ypos  --segno--  --coda--- scrp  sav  xpos  zpos  ypos  head  tail */
   /*---(little legs)-----------------*/
   /* label--------   cnt  exact   curr  degs  xpos  zpos  ypos  --segno--  --coda--- scrp  sav  xpos  zpos  ypos  head  tail */
   { "rr.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rr.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rr.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rf.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rf.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "rf.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lf.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lf.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lf.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lr.femu"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lr.pate"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   { "lr.tibi"      ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   /*---(body)------------------------*/
   /* label--------   cnt  exact   curr  degs  xpos  zpos  ypos  --segno--  --coda--- scrp  sav  xpos  zpos  ypos  head  tail */
   { "--.center"    ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   /*---(done)------------------------*/
   { "end-of-list"  ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
};
int         g_nservo;


#define    MAX_SCRPARG      100
typedef    struct  cSCRPARG   tSCRPARG;
struct cSCRPARG {
   char        name        [LEN_LABEL];
   char        flag;
   char        desc        [LEN_STR  ];
};
tSCRPARG   s_scrparg [MAX_SCRPARG] = {
   /*---(fixed positions)----------------*/
   /* label--------   cnt   description ------------------------------------- */
   { "attn"         , '-',  "IK position calculations relative to attention"  },
   { "crab"         , '-',  "IK position calculations relative to crab pose"  },
   /*---(reflection)---------------------*/
   /* label--------   cnt   description ------------------------------------- */
   { "R2L"          , '-',  "reflect movements of right legs to left"         },
   { "xR2L"         , '-',  "reflect x increments from right to left"         },
   { "zR2L"         , '-',  "reflect z increments from right to left"         },
   { "F2R"          , '-',  "reflect movements of front legs to rear"         },
   { "xF2R"         , '-',  "reflect x increments from front to rear"         },
   { "zF2R"         , '-',  "reflect z increments from front to rear"         },
   /* label--------   cnt   description ------------------------------------- */
   { "end-of-list"  , '-',  ""                                                },
};


#define     MAX_GAITS       100
typedef     struct cGAIT    tGAIT;
struct cGAIT {
   char        name        [LEN_LABEL];
   float       xpos        [12];
   float       zpos        [12];
   float       ypos        [12];
};
tGAIT       s_gait_info [MAX_GAITS] = {
   /* label--------------- , */
   {  "crab_left"          , /* x */   0.0, -45.0, -40.0, -30.0, -20.0, -10.0,   0.0,  10.0,  20.0,  30.0,  40.0,  45.0,
      /*------------------*/ /* z */   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,
      /*------------------*/ /* y */  10.0,   5.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   5.0},
   {  "end-of-list"        , /* x */   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,
      /*------------------*/ /* z */   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,
      /*------------------*/ /* y */   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0},
};



/*====================------------------------------------====================*/
/*===----                         file access                          ----===*/
/*====================------------------------------------====================*/
static void      o___ACCESS__________________o (void) {;}


char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
yKINE_servo        (char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(cycle)--------------------------*/
   /*> printf ("start check\n");                                                      <*/
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      if (g_servo_info [i].label [0] == 'e')   break;
      /*> printf ("checking %s¦", g_servo_info[i].label);                                <*/
      if (a_source [0] != g_servo_info [i].label [0])       continue;
      if (strcmp (a_source, g_servo_info [i].label) != 0)   continue;
      return i;
   }
   return -1;
}


char  /*--> prepare for use ---------s-------------[ leaf   [ ------ ]-*/
ykine_scrp_init    (void)
{
   int         i           = 0;
   g_nservo = 0;
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      if (g_servo_info [i].label [0] == 'e')   break;
      g_servo_info [i].saved  = '-';
      ++g_nservo;
   }
   myKINE.scrp_len    = 0.0;
   myKINE.s_lines     =   0;
   strlcpy (myKINE.s_q, "", LEN_LABEL);
   myKINE.s_context   = NULL;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        support functions                     ----===*/
/*====================------------------------------------====================*/
static void      o___SUPPORT_________________o (void) {;}

static char     s_sides   [LEN_LABEL];
static int      s_nside   =    0;
static char     s_ranks   [LEN_LABEL];
static int      s_nrank   =    0;
static char     s_seg     [LEN_LABEL];

char  /* file reading driver ----------------------[--------[--------]-*/
ykine__scrp_prep   (void)
{
   int         i           = 0;
   for (i = 0; i < g_nservo; ++i) {
      g_servo_info [i].scrp  = '-';
   }
   for (i = 0; i < MAX_SCRPARG; ++i) {
      if (strcmp (s_scrparg [i].name, "end-of-list") == 0)  break;
      s_scrparg [i].flag = '-';
   }
   /*---(testing cleanup (DUP))----------*/
   strlcpy (s_sides , ""          , LEN_LABEL);
   s_nside = 0;
   strlcpy (s_ranks , ""          , LEN_LABEL);
   s_nrank = 0;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine__scrp_side        (char a_char)
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
ykine__scrp_rank        (char a_char)
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
ykine__scrp_seg         (char *a_char)
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

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine_scrp_verb         (char *a_char)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   int         i           =   0;
   int         x_index     =  -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_spoint  (a_char);
   /*---(defense)------------------------*/
   --rce;  if (a_char == NULL) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("NULL");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_snote   (a_char);
   /*---(cycle)--------------------------*/
   for (i = 0; i < MAX_VERBS; ++i) {
      /*---(check legs first)------------*/
      if (a_char [0] != s_verb_info [i].name  [0])       continue;
      if (strcmp (a_char, s_verb_info [i].name) != 0)    continue;
      DEBUG_YKINE_SCRP   yLOG_snote   ("FOUND");
      x_index = i;
      break;
   }
   if (x_index <  0) {
      DEBUG_YKINE_SCRP   yLOG_sexitr  (__FUNCTION__, x_index);
      return x_index;
   }
   DEBUG_YKINE_SCRP   yLOG_sint    (x_index);
   /*---(save)---------------------------*/
   strncpy   (myKINE.s_verb, s_verb_info [x_index].name, LEN_LABEL);
   myKINE.s_type = s_verb_info [x_index].type;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return x_index;
}

static char  /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
ykine__scrp_servo  (char *a_source)
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
ykine__scrp_servos      (char *a_source)
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
   rc = ykine__scrp_side (a_source [0]);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_info    ("s_sides"   , s_sides);
   /*---(interpret front-to-back)--------*/
   rc = ykine__scrp_rank (a_source [1]);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_info    ("s_ranks"   , s_ranks);
   /*---(interpret segment)--------------*/
   rc = ykine__scrp_seg  (a_source + 3);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cycle)--------------------------*/
   for (i = 0; i < s_nside; ++i) {
      for (j = 0; j < s_nrank; ++j) {
         sprintf (x_label, "%c%c.%s", s_sides [i], s_ranks [j], a_source + 3);
         DEBUG_YKINE_SCRP  yLOG_info    ("x_label"   , x_label);
         x_index = ykine__scrp_servo (x_label);
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

static char  /*--> reture an argument value --------------[ ------ [ ------ ]-*/
yKINE__scrp_argval (char *a_name)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   for (i = 0; i < MAX_SCRPARG; ++i) {
      if (strcmp (s_scrparg [i].name, "end-of-list") == 0)  break;
      if (a_name[0] != s_scrparg [i].name [0])                   continue;
      if (strcmp (s_scrparg [i].name, a_name) != 0)              continue;
      return s_scrparg [i].flag;
   }
   return -1;
}

static char  /*--> parse an argument list ----------------[ ------ [ ------ ]-*/
yKINE__scrp_args   (char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   char       *p           = NULL;
   char       *q           = ", ";               /* strtok delimeters         */
   char       *s           = NULL;               /* strtok context variable   */
   int         i           = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(start the parse)-------------*/
   /*> printf ("a_source %s\n", a_source);                                            <*/
   p = strtok_r (a_source, q, &s);
   while (p != NULL) {
      /*> printf ("  search for %s\n", p);                                            <*/
      for (i = 0; i < MAX_SCRPARG; ++i) {
         /*> printf ("    checking %s\n", s_scrparg [i].name);                        <*/
         if (strcmp (s_scrparg [i].name, "end-of-list") == 0)  break;
         if (p[0] != s_scrparg [i].name [0])                   continue;
         if (strcmp (s_scrparg [i].name, p) != 0)              continue;
         s_scrparg [i].flag = 'y';
         /*> printf ("    found    %s\n", s_scrparg [i].name);                        <*/
         break;
      }
      p = strtok_r (NULL    , q, &s);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        leg movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___MOVES___________________o (void) {;}


/*---(prefix)-------------------------*/
#define     FIELD_SVO      1
#define     FIELD_SEC      2
/*---(servo related)------------------*/
#define     FIELD_DEG      3
/*---(leg related)--------------------*/
#define     FIELD_FEMU     3
#define     FIELD_PATE     4
#define     FIELD_TIBI     5
/*---(position related)---------------*/
#define     FIELD_XPOS     3
#define     FIELD_ZPOS     4
#define     FIELD_YPOS     5
/*---(repeat related)-----------------*/
#define     FIELD_TIMES    2
#define     FIELD_COUNT    3
/*---(suffix)-------------------------*/
#define     FIELD_ARGS     6

static char  /*--> parse a move entry --------------------[ ------ [ ------ ]-*/
yKINE__scrp_move   (char *a_type)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   char       *p           = NULL;
   int         i           = 0;
   int         j           = 0;
   int         x_len       = 0;
   int         x_count     = -1;
   double      x_degs      = -1;
   double      x_secs      = -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
      p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);
      --rce;  if (p == NULL) {
         DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
         DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (p);
      DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_SVO   :  /*---(servo)----*/
         x_count = ykine__scrp_servos (p);
         DEBUG_YKINE_SCRP  yLOG_value   ("count"     , x_count);
         --rce;  if (x_count < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_SEC   :  /*---(seconds)--*/
         x_secs = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("seconds"   , x_secs);
         break;
      case  FIELD_DEG   :  /*---(degrees)--*/
         x_degs = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("degrees"   , x_degs);
         for (j = 0; j < g_nservo; ++j) {
            if (g_servo_info [j].scrp != 'y') continue;
            yKINE_move_create (MOVE_SERVO, g_servo_info + j, a_type, myKINE.s_lines, x_degs, x_secs);
         }
         break;
      case  FIELD_ARGS  :  /*---(args)-----*/
         yKINE__scrp_args(p);
         break;
      }
      DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");
   } 
   DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       parsing functions                      ----===*/
/*====================------------------------------------====================*/
static void      o___PARSING_________________o (void) {;}

#define        FAILED    -666.0

char  /*--> parse a IK/FK move --------------------[ ------ [ ------ ]-*/
ykine_parse_read       (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *p           = NULL;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defaults)-----------------------*/
   strlcpy (myKINE.s_verb, "", LEN_LABEL);
   myKINE.s_type      = '-';
   myKINE.s_vers      = '-';
   myKINE.s_count     = -1;
   myKINE.s_secs      = FAILED;
   myKINE.s_femu      = FAILED;
   myKINE.s_pate      = FAILED;
   myKINE.s_tibi      = FAILED;
   myKINE.s_xpos      = FAILED;
   myKINE.s_zpos      = FAILED;
   myKINE.s_ypos      = FAILED;
   /*---(source line)--------------------*/
   ++myKINE.s_lines;
   DEBUG_YKINE_SCRP   yLOG_value   ("line"      , myKINE.s_lines);
   /*---(get record)---------------------*/
   fgets (myKINE.s_recd, LEN_RECD, stdin);
   --rce;  if (feof (stdin))  {
      DEBUG_YKINE_SCRP   yLOG_note    ("end of file reached");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(defense)------------------------*/
   myKINE.s_len = strlen (myKINE.s_recd);
   if (myKINE.s_len <= 0)  {
      DEBUG_YKINE_SCRP   yLOG_note    ("record empty");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (myKINE.s_recd [0] == '#') {
      DEBUG_YKINE_SCRP   yLOG_note    ("comment line, skipping");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(clean)--------------------------*/
   myKINE.s_recd [--myKINE.s_len] = '£';
   DEBUG_YKINE_SCRP   yLOG_value   ("length"    , myKINE.s_len);
   DEBUG_YKINE_SCRP   yLOG_info    ("fixed"     , myKINE.s_recd);
   /*---(get recd type)---------------*/
   p = strtok_r (myKINE.s_recd, myKINE.s_q, &myKINE.s_context);
   --rce;  if (p == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("can not parse type field");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strltrim  (p, ySTR_BOTH, LEN_RECD);
   rc = ykine_scrp_verb (p);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("can not identify verb");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(get version)-----------------*/
   p = strtok_r (NULL     , myKINE.s_q, &myKINE.s_context);
   --rce;  if (p == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("can not parse version field");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   strltrim  (p, ySTR_BOTH, LEN_RECD);
   --rce;  if (strlen (p) != 3) {
      DEBUG_YKINE_SCRP   yLOG_note    ("invalid length version field (3)");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (p[0] != '-' || p[2] != '-') {
      DEBUG_YKINE_SCRP   yLOG_note    ("invalid format version field (-x-)");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr (s_verb_info [rc].vers, p [1]) == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("not an allowed version");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   myKINE.s_vers = p[1];
   DEBUG_YKINE_SCRP  yLOG_char    ("version"   , myKINE.s_vers);
   /*---(complete)--------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> prepare a IK/FK move parse ------------[ ------ [ ------ ]-*/
ykine_parse_prep       (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   rc = ykine_scrp_verb (a_verb);
   --rce;  if (rc < 0) {
      DEBUG_YKINE_SCRP   yLOG_note    ("can not identify verb");
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(initialize)---------------------*/
   myKINE.s_count     = -1;
   myKINE.s_secs      = FAILED;
   myKINE.s_femu      = FAILED;
   myKINE.s_pate      = FAILED;
   myKINE.s_tibi      = FAILED;
   myKINE.s_xpos      = FAILED;
   myKINE.s_zpos      = FAILED;
   myKINE.s_ypos      = FAILED;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}


char  /*--> parse a IK/FK move --------------------[ ------ [ ------ ]-*/
ykine_parse_fields      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char       *p           = NULL;
   int         i           = 0;
   char        x_request   [LEN_LABEL];
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
      p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);
      --rce;  if (p == NULL) {
         DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
         DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (p);
      DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
      DEBUG_YKINE_SCRP  yLOG_value   ("x_len"     , x_len);
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_SVO   :  /*---(servo)----*/
         if (p [0] == '-')  sprintf (x_request, "%c%c.center", p [0], p [1]);
         else               sprintf (x_request, "%c%c.femu"  , p [0], p [1]);
         myKINE.s_count = ykine__scrp_servos (x_request);
         DEBUG_YKINE_SCRP  yLOG_value   ("count"     , myKINE.s_count);
         --rce;  if (myKINE.s_count < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_SEC   :  /*---(secs)---*/
         myKINE.s_secs = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("seconds"   , myKINE.s_secs);
         --rce;  if (myKINE.s_secs < 0.0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_secs"    , "can not be negative");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_XPOS  :  /*---(xpos)---*/
         if (myKINE.s_type == PARSE_POINT) {
            myKINE.s_xpos = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_xpos"    , myKINE.s_xpos);
         } else {
            myKINE.s_femu = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_femu"    , myKINE.s_femu);
         }
         break;
      case  FIELD_ZPOS  :  /*---(zpos)---*/
         if (myKINE.s_type == PARSE_POINT) {
            myKINE.s_zpos = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_zpos"    , myKINE.s_zpos);
         } else {
            myKINE.s_pate = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_pate"    , myKINE.s_pate);
         }
         break;
      case  FIELD_YPOS  :  /*---(ypos)---*/
         if (myKINE.s_type == PARSE_POINT) {
            myKINE.s_ypos = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_ypos"    , myKINE.s_ypos);
         } else {
            myKINE.s_tibi = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_tibi"    , myKINE.s_tibi);
         }
         break;
      }
      /*---(done)------------------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");
   } 
   DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");
   /*---(done)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> check results of a IK/FK parse --------[ ------ [ ------ ]-*/
ykine_parse_check      (void)
{
   char        rce         = -10;                /* return code for errors    */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(check duration)-----------------*/
   --rce;  if (myKINE.s_secs == FAILED) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_secs"    , "never read");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check angles)-------------------*/
   if (myKINE.s_type == PARSE_ANGLE) {
      --rce;  if (myKINE.s_femu == FAILED) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_femu"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      --rce;  if (myKINE.s_pate == FAILED) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_pate"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      --rce;  if (myKINE.s_tibi == FAILED) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_tibi"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   /*---(check endpoint)-----------------*/
   else if (myKINE.s_type == PARSE_POINT) {
      --rce;  if (myKINE.s_xpos == FAILED) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_xpos"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      --rce;  if (myKINE.s_zpos == FAILED) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_zpos"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      --rce;  if (myKINE.s_ypos == FAILED) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_ypos"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   /*---(bad type)-----------------------*/
   else {
      DEBUG_YKINE_SCRP  yLOG_warn    ("myKINE.s_type"    , "unknown type");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(done)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> adjust coordinates based on body ------[ ------ [ ------ ]-*/
ykine__parse_adjust     (void)
{
   char        rce         = -10;                /* return code for errors    */
   double      x_degs      = 0.0;
   double      x_rads      = 0.0;
   double      x_dist      = 0.0;
   double      x_orig      = 0.0;
   double      x_vert      = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(center changes)-----------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("shift adjustments");
   myKINE.s_xpos -= myKINE.s_xcenter;
   myKINE.s_zpos -= myKINE.s_zcenter;
   myKINE.s_ypos -= myKINE.s_ycenter;
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_xpos", myKINE.s_xpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_zpos", myKINE.s_zpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_ypos", myKINE.s_ypos);
   /*---(yaw)----------------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("yaw calcs");
   x_dist  = sqrt  ((myKINE.s_xpos * myKINE.s_xpos) + (myKINE.s_zpos * myKINE.s_zpos));
   DEBUG_YKINE_SCRP  yLOG_double  ("x_dist"    , x_dist);
   x_rads  = - atan2 (myKINE.s_zpos , myKINE.s_xpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("x_rads"    , x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_degs"    , x_degs);
   x_rads += (myKINE.s_yaw * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_double  ("new x_rads", x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_SCRP  yLOG_double  ("new x_degs", x_degs);
   myKINE.s_xpos  =   x_dist * cos (x_rads);
   myKINE.s_zpos  = -(x_dist * sin (x_rads));
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_xpos", myKINE.s_xpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_zpos", myKINE.s_zpos);
   /*---(rotate)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("rotate calcs");
   x_dist  = myKINE.s_xpos;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_dist"    , x_dist);
   x_rads  = myKINE.s_rotate * DEG2RAD;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_rads"    , x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_degs"    , x_degs);
   x_vert  = x_dist * sin (x_rads);
   DEBUG_YKINE_SCRP  yLOG_double  ("x_vert"    , x_vert);
   x_vert  = myKINE.s_ypos + x_vert;
   DEBUG_YKINE_SCRP  yLOG_double  ("new x_vert", x_vert);
   myKINE.s_xpos  = myKINE.s_xpos - (myKINE.s_ypos * sin (x_rads));
   myKINE.s_ypos  = x_vert * cos (x_rads);
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_xpos", myKINE.s_xpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_ypos", myKINE.s_ypos);
   /*---(pitch)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("pitch calcs");
   x_dist  = myKINE.s_zpos;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_dist"    , x_dist);
   x_rads  = myKINE.s_pitch * DEG2RAD;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_rads"    , x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_degs"    , x_degs);
   x_vert  = x_dist * sin (x_rads);
   DEBUG_YKINE_SCRP  yLOG_double  ("x_vert"    , x_vert);
   x_vert  = myKINE.s_ypos + x_vert;
   DEBUG_YKINE_SCRP  yLOG_double  ("new x_vert", x_vert);
   myKINE.s_zpos  = myKINE.s_zpos - (myKINE.s_ypos * sin (x_rads));
   myKINE.s_ypos  = x_vert * cos (x_rads);
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_zpos", myKINE.s_zpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("new myKINE.s_ypos", myKINE.s_ypos);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         ik movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___IK_MOVES________________o (void) {;}

static char  /*--> save abolute ik based move ------------[ ------ [ ------ ]-*/
yKINE__scrp_ik_pure     (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          =   0;
   int         i           = 0;
   int         x_leg       = 0.0;
   double      x_xsave     = 0.0;
   double      x_zsave     = 0.0;
   double      x_ysave     = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   --rce;  if (myKINE.s_count < 0) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("servos"    , "count is zero, can not be right");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (myKINE.s_count > 1) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("servos"    , "multiple selected, not legal");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process)------------------------*/
   for (i = 0; i < g_nservo; ++i) {
      /*---(filter)----------------------*/
      if (g_servo_info [i].scrp != 'y') continue;
      /*---(identify len)----------------*/
      x_leg = i / 3.0;
      DEBUG_YKINE_SCRP  yLOG_value   ("x_leg"     , x_leg);
      /*---(clean values)----------------*/
      myKINE.s_xpos = x_xsave;
      myKINE.s_zpos = x_zsave;
      myKINE.s_ypos = x_ysave;
      /*---(adjust)----------------------*/
      rc = ykine__parse_adjust ();
      /*---(calc angles)-----------------*/
      rc = yKINE_inverse  (x_leg, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse rc", rc);
      rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &myKINE.s_femu, &myKINE.s_pate, &myKINE.s_tibi);
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , myKINE.s_femu);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , myKINE.s_pate);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , myKINE.s_tibi);
      /*---(add moves)-------------------*/
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + i + 0, a_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + i + 1, a_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + i + 2, a_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);
      rc = yKINE_move_addloc (g_servo_info + i + 2, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      /*---(update servo)----------------*/
      g_servo_info [i + 2].saved  = 'y';
      g_servo_info [i + 2].xsave  = myKINE.s_xpos;
      g_servo_info [i + 2].zsave  = myKINE.s_zpos;
      g_servo_info [i + 2].ysave  = myKINE.s_ypos;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_scrp_ik_from     (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          = 0;
   int         j           = 0;
   int         x_leg       = 0.0;
   double      x_xsave     = 0.0;
   double      x_zsave     = 0.0;
   double      x_ysave     = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   x_xsave = myKINE.s_xpos;
   x_zsave = myKINE.s_zpos;
   x_ysave = myKINE.s_ypos;
   for (j = 0; j < g_nservo; ++j) {
      if (g_servo_info [j].scrp != 'y') continue;
      /*---(identify len)----------------*/
      x_leg = j / 3.0;
      DEBUG_YKINE_SCRP  yLOG_value   ("x_leg"     , x_leg);
      /*---(clean values)----------------*/
      myKINE.s_xpos = x_xsave;
      myKINE.s_zpos = x_zsave;
      myKINE.s_ypos = x_ysave;
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_xpos"    , myKINE.s_xpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_zpos"    , myKINE.s_zpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_ypos"    , myKINE.s_ypos);
      /*---(calc new)--------------------*/
      DEBUG_YKINE_SCRP  yLOG_double  ("xsave"     , g_servo_info [j + 2].xsave );
      DEBUG_YKINE_SCRP  yLOG_double  ("zsave"     , g_servo_info [j + 2].zsave );
      DEBUG_YKINE_SCRP  yLOG_double  ("ysave"     , g_servo_info [j + 2].ysave );
      myKINE.s_xpos += g_servo_info [j + 2].xsave;
      myKINE.s_zpos += g_servo_info [j + 2].zsave;
      myKINE.s_ypos += g_servo_info [j + 2].ysave;
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_xpos new", myKINE.s_xpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_zpos new", myKINE.s_zpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("myKINE.s_ypos new", myKINE.s_ypos);
      /*---(adjust)----------------------*/
      rc = ykine__parse_adjust ();
      /*---(get angles)------------------*/
      rc = yKINE_inverse  (x_leg, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &myKINE.s_femu, &myKINE.s_pate, &myKINE.s_tibi);
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , myKINE.s_femu);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , myKINE.s_pate);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , myKINE.s_tibi);
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + j + 0, a_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + j + 1, a_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + j + 2, a_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);
      rc = yKINE_move_addloc (g_servo_info + j + 2, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> drive a IK/FK parse -------------------[ ------ [ ------ ]-*/
yKINE__scrp_kine   (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   /*> rc = ykine_parse_prep     (a_verb);                                            <* 
    *> if (rc < 0) {                                                                  <* 
    *>    DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                              <* 
    *>    return rc;                                                                  <* 
    *> }                                                                              <*/
   /*---(parse)--------------------------*/
   rc = ykine_parse_fields   ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   rc = ykine_parse_check   ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle FK)----------------------*/
   if (a_verb [0] == 'f') {
      --rce;  switch (a_verb [3]) {
      case 'p' :
         rc = yKINE__scrp_fk_pure   (a_verb);
         break;
      default  :
         DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   /*---(handle IK)----------------------*/
   if (a_verb [0] == 'i') {
      --rce;  switch (a_verb [3]) {
      case 'p' :
         rc = yKINE__scrp_ik_pure   (a_verb);
         break;
      case 'f' :
         rc = ykine_scrp_ik_from   (a_verb);
         break;
      default  :
         DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> make changes to body cente ------------[ ------ [ ------ ]-*/
yKINE__scrp_orient (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_len       = 0;
   char        x_type      = 'f';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   /*> rc = ykine_parse_prep     (a_verb);                                            <* 
    *> if (rc < 0) {                                                                  <* 
    *>    DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                              <* 
    *>    return rc;                                                                  <* 
    *> }                                                                              <*/
   /*---(parse)--------------------------*/
   rc = ykine_parse_fields  ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   rc = ykine_parse_check   ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle)-------------------------*/
   --rce;  switch (a_verb [3]) {
   case 'p' :
      myKINE.s_yaw      = myKINE.s_femu;
      myKINE.s_rotate   = myKINE.s_pate;
      myKINE.s_pitch    = myKINE.s_tibi;
      /*> rc = yKINE__scrp_ik_pure   (a_verb);                                        <*/
      break;
   case 'f' :
      myKINE.s_yaw     += myKINE.s_femu;
      myKINE.s_rotate  += myKINE.s_pate;
      myKINE.s_pitch   += myKINE.s_tibi;
      break;
   default  :
      DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("s_yaw"     , myKINE.s_yaw);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_rotate"  , myKINE.s_rotate);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_pitch"   , myKINE.s_pitch);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> make changes to body cente ------------[ ------ [ ------ ]-*/
yKINE__scrp_zero   (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_len       = 0;
   char        x_type      = 'i';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   /*> rc = ykine_parse_prep     (a_verb);                                            <* 
    *> if (rc < 0) {                                                                  <* 
    *>    DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                              <* 
    *>    return rc;                                                                  <* 
    *> }                                                                              <*/
   /*---(parse)--------------------------*/
   rc = ykine_parse_fields  ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   rc = ykine_parse_check   ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle)-------------------------*/
   --rce;  switch (a_verb [3]) {
   case 'p' :
      myKINE.s_xcenter  = myKINE.s_xpos;
      myKINE.s_zcenter  = myKINE.s_zpos;
      myKINE.s_ycenter  = myKINE.s_ypos;
      rc = yKINE__scrp_ik_pure   (a_verb);
      break;
   case 'f' :
      myKINE.s_xcenter += myKINE.s_xpos;
      myKINE.s_zcenter += myKINE.s_zpos;
      myKINE.s_ycenter += myKINE.s_ypos;
      break;
   default  :
      DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("s_xcenter" , myKINE.s_xcenter);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_zcenter" , myKINE.s_zcenter);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ycenter" , myKINE.s_ycenter);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> make changes to body tilt -------------[ ------ [ ------ ]-*/
yKINE__scrp_tilt   (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_len       = 0;
   char        x_type      = 'i';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   /*> rc = ykine_parse_prep     (a_verb);                                            <* 
    *> if (rc < 0) {                                                                  <* 
    *>    DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                              <* 
    *>    return rc;                                                                  <* 
    *> }                                                                              <*/
   /*---(parse)--------------------------*/
   rc = ykine_parse_fields  ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   rc = ykine_parse_check   ();
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle)-------------------------*/
   --rce;  switch (a_verb [3]) {
   case 'p' :
      myKINE.s_xcenter  = myKINE.s_xpos;
      myKINE.s_zcenter  = myKINE.s_zpos;
      myKINE.s_ycenter  = myKINE.s_ypos;
      rc = yKINE__scrp_ik_pure   (a_verb);
      break;
   case 'f' :
      myKINE.s_xcenter += myKINE.s_xpos;
      myKINE.s_zcenter += myKINE.s_zpos;
      myKINE.s_ycenter += myKINE.s_ypos;
      break;
   default  :
      DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("s_xcenter" , myKINE.s_xcenter);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_zcenter" , myKINE.s_zcenter);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ycenter" , myKINE.s_ycenter);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         fk movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___FK_MOVES________________o (void) {;}

static char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
yKINE__scrp_fk_pure     (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         i           = 0;
   int         j           = 0;
   int         x_len       = 0;
   int         x_leg       = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (j = 0; j < g_nservo; ++j) {
      if (g_servo_info [j].scrp != 'y') continue;
      /*---(save forward)----------------*/
      yKINE_move_create (MOVE_SERVO, g_servo_info + j + 0, a_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);
      yKINE_move_create (MOVE_SERVO, g_servo_info + j + 1, a_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);
      yKINE_move_create (MOVE_SERVO, g_servo_info + j + 2, a_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);
      /*---(calc inverse)----------------*/
      x_leg = j / 3.0;
      rc = yKINE_forward     (x_leg, myKINE.s_femu, myKINE.s_pate, myKINE.s_tibi);
      DEBUG_YKINE_SCRP  yLOG_value   ("fk pure rc", rc);
      rc = yKINE_endpoint    (x_leg, YKINE_TARG, YKINE_FK, NULL, NULL, &myKINE.s_xpos, &myKINE.s_zpos, &myKINE.s_ypos);
      rc = yKINE_move_addloc (g_servo_info + j + 2, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      /*---(update servo)----------------*/
      g_servo_info [j + 2].saved  = 'y';
      g_servo_info [j + 2].xsave  = myKINE.s_xpos;
      g_servo_info [j + 2].zsave  = myKINE.s_zpos;
      g_servo_info [j + 2].ysave  = myKINE.s_ypos;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> parse a move entry --------------------[ ------ [ ------ ]-*/
yKINE__scrp_fk_OLD (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   char       *p           = NULL;
   int         i           = 0;
   int         j           = 0;
   char        x_request   [LEN_LABEL];
   int         x_len       = 0;
   int         x_servo     = -1;
   int         x_leg       =  0;
   float       x_secs      = -1;
   double      x_femu      = 0.0;
   double      x_pate      = 0.0;
   double      x_tibi      = 0.0;
   double      x_xpos      = 0.0;
   double      x_zpos      = 0.0;
   double      x_ypos      = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
      p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);
      --rce;  if (p == NULL) {
         DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
         DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (p);
      DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_SVO   :  /*---(servo)----*/
         sprintf (x_request, "%s.femu", p);
         x_servo = ykine__scrp_servos (x_request);
         --rce;  if (x_servo < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_SEC   :  /*---(seconds)--*/
         x_secs = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("seconds"   , x_secs);
         break;
      case  FIELD_FEMU  :  /*---(degrees)--*/
         x_femu = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , x_femu);
         break;
      case  FIELD_PATE  :  /*---(degrees)--*/
         x_pate = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , x_pate);
         break;
      case  FIELD_TIBI  :  /*---(degrees)--*/
         x_tibi = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , x_tibi);
         for (j = 0; j < g_nservo; ++j) {
            if (g_servo_info [j].scrp != 'y') continue;
            yKINE_move_create (MOVE_SERVO, g_servo_info + j + 0, "forward", myKINE.s_lines, x_femu, x_secs);
            yKINE_move_create (MOVE_SERVO, g_servo_info + j + 1, "forward", myKINE.s_lines, x_pate, x_secs);
            yKINE_move_create (MOVE_SERVO, g_servo_info + j + 2, "forward", myKINE.s_lines, x_tibi, x_secs);
            x_leg = j / 3.0;
            yKINE_forward     (x_leg, x_femu, x_pate, x_tibi);
            yKINE_endpoint    (x_leg, YKINE_TARG, YKINE_FK, NULL, NULL, &x_xpos, &x_zpos, &x_ypos);
            yKINE_move_addloc (g_servo_info + j + 2, x_xpos, x_zpos, x_ypos);
            /*---(update servo)----------------*/
            g_servo_info [j + 2].saved  = 'y';
            g_servo_info [j + 2].xsave  = x_xpos;
            g_servo_info [j + 2].zsave  = x_zpos;
            g_servo_info [j + 2].ysave  = x_ypos;
         }
         break;
      case  FIELD_ARGS  :  /*---(args)-----*/
         yKINE__scrp_args(p);
         break;
      }
      DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");
   } 
   DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                          repeating                           ----===*/
/*====================------------------------------------====================*/
static void      o___REPEATS_________________o (void) {;}

static char  /*--> parse a segno marker ------------------[ ------ [ ------ ]-*/
yKINE__scrp_segno  (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   char       *p           = NULL;
   int         i           = 0;
   int         j           = 0;
   int         x_len       = 0;
   int         x_servo     = -1;
   int         x_count     = -1;
   int         x_times     = -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
      p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);
      --rce;  if (p == NULL) {
         DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
         DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (p);
      DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_SVO   :  /*---(servo to repeat)----*/
         x_servo = ykine__scrp_servos (p);
         --rce;  if (x_servo < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         for (j = 0; j < g_nservo; ++j) {
            if (g_servo_info [j].scrp != 'y') continue;
            g_servo_info [j].segno_flag = 'y';
            g_servo_info [j].segno      = NULL;
         }
         break;
      case  FIELD_ARGS  :  /*---(args)-----*/
         yKINE__scrp_args(p);
         break;
      }
      DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");
   } 
   DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> parse a low level repeat --------------[ ------ [ ------ ]-*/
yKINE__scrp_repeat (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   char       *p           = NULL;
   int         i           = 0;
   int         j           = 0;
   int         x_len       = 0;
   int         x_servo     = -1;
   int         x_count     = -1;
   int         x_times     = -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
      p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);
      --rce;  if (p == NULL) {
         DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
         DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (p);
      DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_SVO   :  /*---(servo to repeat)----*/
         x_servo = ykine__scrp_servos (p);
         --rce;  if (x_servo < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_TIMES :  /*---(times to repeat)----*/
         x_times = atoi (p);
         DEBUG_YKINE_SCRP  yLOG_value   ("x_times"   , x_times);
         --rce;  if (x_times < 1 || x_times > 100) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("times"     , "out of range (1 - 100)");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_COUNT :  /*---(moves to repeat)----*/
         x_count = atoi (p);
         DEBUG_YKINE_SCRP  yLOG_value   ("x_count"   , x_count);
         --rce;  if (x_count < 1 || x_count > 100) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("moves"     , "out of range (1 - 100)");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         for (j = 0; j < g_nservo; ++j) {
            if (g_servo_info [j].scrp != 'y') continue;
            yKINE_move_repeat     (g_servo_info + j, x_count, x_times);
         }
         break;
      case  FIELD_ARGS  :  /*---(args)-----*/
         yKINE__scrp_args(p);
         break;
      }
      DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");
   } 
   DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> parse a high level repeat -------------[ ------ [ ------ ]-*/
yKINE__scrp_dsegno (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   char       *p           = NULL;
   int         i           = 0;
   int         j           = 0;
   int         x_len       = 0;
   int         x_servo     = -1;
   int         x_times     = -1;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read fields)--------------------*/
   for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
      p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);
      --rce;  if (p == NULL) {
         DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
         DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
         break;
      }
      strltrim (p, ySTR_BOTH, LEN_RECD);
      x_len = strlen (p);
      DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_SVO   :  /*---(servo to repeat)----*/
         x_servo = ykine__scrp_servos (p);
         --rce;  if (x_servo < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_TIMES :  /*---(times to repeat)----*/
         x_times = atoi (p);
         DEBUG_YKINE_SCRP  yLOG_value   ("x_times"   , x_times);
         --rce;  if (x_times < 1 || x_times > 100) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("times"     , "out of range (1 - 100)");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         for (j = 0; j < g_nservo; ++j) {
            if (g_servo_info [j].scrp != 'y') continue;
            yKINE_move_dsegno   (g_servo_info + j, x_times);
         }
         break;
      case  FIELD_COUNT :  /*---(moves to repeat)----*/
         break;
      case  FIELD_ARGS  :  /*---(args)-----*/
         yKINE__scrp_args(p);
         break;
      }
      DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");
   } 
   DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                        script driver                         ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVER__________________o (void) {;}

char         /* file reading driver ----------------------[--------[--------]-*/
yKINE_script       (double *a_len)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         rc          = 0;
   char       *p;
   char        x_verb      [20]        = "";;
   char        x_ver       = '-';
   int         i           = 0;
   double      x_len       = 0.0;
   double      x_sec       = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   ykine_scrp_init  ();
   /*---(read lines)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("read lines");
   while (1) {
      /*---(prepare)---------------------*/
      ykine__scrp_prep     ();
      /*---(read and clean)--------------*/
      ykine_parse_read     ();
      /*---(handle types)----------------*/
      switch (x_verb [0]) {
      case 'w' : /* walking            */
         ykine_scrp_walk        ( 5);
         break;
      case 'c' : /* circle             */
         ykine_scrp_turn        (20);
         break;
      case '1' : /* gait               */
         if        (strcmp ("12_GAIT_BEG"   , x_verb) == 0) {
            ykine_gait_begin    (12);
         } else if (strcmp ("12_GAIT_END"   , x_verb) == 0) {
            ykine_gait_update   (12);
         }
         /*> yKINE__scrp_gate      ();                                                <*/
         break;
      case '6' : /* gait               */
         if        (strcmp ("6_GAIT_BEG"   , x_verb) == 0) {
            ykine_gait_begin    (6);
         } else if (strcmp ("6_GAIT_END"   , x_verb) == 0) {
            ykine_gait_update   (6);
         }
         /*> yKINE__scrp_gate      ();                                                <*/
         break;
      case 'R' : /* repeat             */
         yKINE__scrp_repeat    ();
         break;
      case 'S' : /* segno              */
         yKINE__scrp_segno     ();
         break;
      case 'D' : /* dal segno          */
         yKINE__scrp_dsegno    ();
         break;
      case 'z' : /* zero/center adjustments */
         yKINE__scrp_zero      (x_verb);
         break;
      case 'o' : /* orient body             */
         yKINE__scrp_orient    (x_verb);
         break;
      case 's' : /* servo, start       */
         yKINE__scrp_move      (x_verb);
         break;
      case 'f' : /* fk based positioning */
      case 'i' : /* ik based positioning */
         yKINE__scrp_kine      (x_verb);
         break;
      default  :
         DEBUG_YKINE_SCRP  yLOG_note    ("verb not recognized and skipped");
         break;
      }

   }
   /*---(fix length)---------------------*/
   x_len = 0.0;
   for (i = 0; i < g_nservo; ++i) {
      yKINE_move_last_servo (i, &x_sec, NULL);
      if (x_sec > x_len)  x_len = x_sec;
   }
   myKINE.scrp_len = x_len;
   if (a_len != NULL)  *a_len = x_len;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__unit_scrp        (char *a_question, int a_num)
{
   int         i           =    0;
   int         x_pos       =    0;
   char        x_msg       [LEN_STR];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "SCRP unit        : question not understood", LEN_STR);
   /*---(answer)------------------------------------------*/
   if (strcmp (a_question, "sides"   ) == 0) {
      sprintf (ykine__unit_answer, "SCRP sides     : %d sides=%-5.5s, %d ranks=%s", s_nside, s_sides, s_nrank, s_ranks);
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
      sprintf (ykine__unit_answer, "SCRP servos    : %s", x_msg);
   }
   else if (strcmp (a_question, "verb"    ) == 0) {
      sprintf (ykine__unit_answer, "SCRP verb      : %-7.7s    , vers %c, type %c", myKINE.s_verb, myKINE.s_vers, myKINE.s_type);
   }
   else if (strcmp (a_question, "angles"   ) == 0) {
      sprintf (ykine__unit_answer, "SCRP angles    : %8.3fs, %8.2ff, %8.2fp, %8.2ft", myKINE.s_secs, myKINE.s_femu, myKINE.s_pate, myKINE.s_tibi);
   }
   else if (strcmp (a_question, "positions") == 0) {
      sprintf (ykine__unit_answer, "SCRP positions : %8.3fs, %8.2fx, %8.2fz, %8.2fy", myKINE.s_secs, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}



/*============================----end-of-source---============================*/
