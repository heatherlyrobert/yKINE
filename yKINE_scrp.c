/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


#define     MOVE_NULL   '-'
#define     MOVE_PAUSE  'p'
#define     MOVE_SERVO  's'




tSERVO     g_servos  [YKINE_MAX_SERVO] = {
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
   { "end-of-list"  ,   0,   '-',  NULL,  0.0,  0.0,  0.0,  0.0, '-', NULL, '-', NULL, '-', '-',  0.0,  0.0,  0.0, NULL, NULL },
   /* label--------   cnt  exact   curr  degs  xpos  zpos  ypos  --segno--  --coda--- scrp  sav  xpos  zpos  ypos  head  tail */
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
   /* label--------   cnt   description ------------------------------------- */
   { "attn"         , '-',  "IK position calculations relative to attention"  },
   { "crab"         , '-',  "IK position calculations relative to crab pose"  },
   { "F2R"          , '-',  "reflect movements of front legs to get rear"     },
   { "xR2L"         , '-',  "reflect x increments from right to left"         },
   { "zR2L"         , '-',  "reflect z increments from right to left"         },
   { "end-of-list"  , '-',  ""                                                },
};


static float    s_xcenter   = 0.0;
static float    s_zcenter   = 0.0;
static float    s_ycenter   = 0.0;
static float    s_yaw       = 0.0;
static float    s_rotate    = 0.0;
static float    s_pitch     = 0.0;


/*====================------------------------------------====================*/
/*===----                         file access                          ----===*/
/*====================------------------------------------====================*/
static void      o___ACCESS__________________o (void) {;}

static FILE    *s_file  = NULL;
static int      s_lines = 0;
static char     s_recd  [LEN_RECD];

char         /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
yKINE_servo        (char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   /*---(cycle)--------------------------*/
   /*> printf ("start check\n");                                                      <*/
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      if (g_servos [i].label [0] == 'e')   break;
      /*> printf ("checking %s\n", g_servos[i].label);                                <*/
      if (a_source [0] != g_servos [i].label [0])       continue;
      if (strcmp (a_source, g_servos [i].label) != 0)   continue;
      return i;
   }
   return -1;
}


static char  /*--> prepare for use ---------s-------------[ leaf   [ ------ ]-*/
yKINE__scrp_init   (void)
{
   int         i           = 0;
   g_nservo = 0;
   for (i = 0; i < YKINE_MAX_SERVO; ++i) {
      if (g_servos [i].label [0] == 'e')   break;
      g_servos [i].saved  = '-';
      ++g_nservo;
   }
   yKINE_its.scrp_len = 0.0;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        support functions                     ----===*/
/*====================------------------------------------====================*/
static void      o___SUPPORT_________________o (void) {;}

static char  /*--> identify the leg number ---------------[ ------ [ ------ ]-*/
yKINE__scrp_legno  (char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   for (i = 0; i < YKINE_MAX_LEGS; ++i) {
      if (leg_data [i].caps[0] != a_source[0])   continue;
      if (leg_data [i].caps[1] != a_source[1])   continue;
      return i;
   }
   return -1;
}

static char  /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
yKINE__scrp_servo  (char *a_source)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   int         i           =   0;
   int         x_count     =   0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_senter  (__FUNCTION__);
   /*---(cycle)--------------------------*/
   /*> printf ("yKINE__scrp_servo  looking for %s\n", a_source);                       <*/
   for (i = 0; i < g_nservo; ++i) {
      /*---(check legs first)------------*/
      if (a_source [0] != g_servos [i].label [0])       continue;
      if (a_source [1] != g_servos [i].label [1])       continue;
      /*---(check for all segments)------*/
      if (strcmp ("full", a_source + 3) == 0) {
         DEBUG_YKINE_SCRP   yLOG_snote   ("servo label found");
         g_servos [i].scrp = 'y';
         ++x_count;
         continue;
      }
      /*---(check for one segment)-------*/
      if (strcmp (a_source, g_servos [i].label) != 0)   continue;
      DEBUG_YKINE_SCRP   yLOG_snote   ("servo label found");
      g_servos [i].scrp = 'y';
      ++x_count;
      break;
   }
   DEBUG_YKINE_SCRP   yLOG_svalue  ("count"     , x_count);
   --rce;  if (x_count == 0) {
      DEBUG_YKINE_SCRP   yLOG_snote   ("servo label not found");
      DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP   yLOG_sexit   (__FUNCTION__);
   return x_count;
}

static char  /*--> locate a servo entry ------------------[ ------ [ ------ ]-*/
yKINE__scrp_servos (char *a_source)
{  /*---(design notes)-------------------*/
   /*
    *  L=left  , R=right
    *  F=front , M=middle, R=rear
    *  a=all   , +=large , -=small
    *
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
    *  full = all segments
    *
    *  servo labels start with two-char leg, '.', and four-char segment
    *     LF.femu    = left-front leg's femur
    *     aF.femu    = femur on both front legs
    *     La.femu    = femurs on the left side
    *     aa.femu    = all femurs
    *     -F.femu    = femurs on the front small legs
    *     +F.femu    = femurs on the front large legs
    *     L-.femu    = femurs on the left small legs
    *     L+.femu    = femurs on the left large legs
    *     ++.femu    = femurs on all large legs
    *     --.femu    = femurs on all small legs
    *
    *   need to add front to back, side to side, and other mirroring
    *
    *
    */
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   int         i           = 0;
   int         j           = 0;
   int         c           = 0;
   int         x_index     = -1;
   char        x_side      [LEN_LABEL] = "";
   int         x_nside     = 0;
   char        x_rank      [LEN_LABEL] = "";
   int         x_nrank     = 0;
   char        x_label     [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(interpret side-to-side)---------*/
   switch (a_source [0]) {
   case 'L' : strlcpy (x_side  , "L"         , LEN_LABEL);   break;
   case 'R' : strlcpy (x_side  , "R"         , LEN_LABEL);   break;
   case 'l' : strlcpy (x_side  , "l"         , LEN_LABEL);   break;
   case 'r' : strlcpy (x_side  , "r"         , LEN_LABEL);   break;
   case '<' : strlcpy (x_side  , "Ll"        , LEN_LABEL);   break;
   case '>' : strlcpy (x_side  , "Rr"        , LEN_LABEL);   break;
   case '+' : strlcpy (x_side  , "LR"        , LEN_LABEL);   break;
   case '-' : strlcpy (x_side  , "lr"        , LEN_LABEL);   break;
   case 'a' : strlcpy (x_side  , "LRlr"      , LEN_LABEL);   break;
   default  : strlcpy (x_side  , ""          , LEN_LABEL);   break;
   }
   x_nside = strlen (x_side);
   /*> printf ("SCRP_servos  x_side (%d) %s\n", x_nside, x_side);                     <*/
   --rce;  if (x_nside == 0) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(interpret front-to-back)--------*/
   switch (a_source [1]) {
   case 'R' : strlcpy (x_rank  , "R"         , LEN_LABEL);   break;
   case 'M' : strlcpy (x_rank  , "M"         , LEN_LABEL);   break;
   case 'F' : strlcpy (x_rank  , "F"         , LEN_LABEL);   break;
   case 'r' : strlcpy (x_rank  , "r"         , LEN_LABEL);   break;
   case 'f' : strlcpy (x_rank  , "f"         , LEN_LABEL);   break;
   case '^' : strlcpy (x_rank  , "Ff"        , LEN_LABEL);   break;
   case '_' : strlcpy (x_rank  , "Rr"        , LEN_LABEL);   break;
   case '+' : strlcpy (x_rank  , "RMF"       , LEN_LABEL);   break;
   case '-' : strlcpy (x_rank  , "rf"        , LEN_LABEL);   break;
   case 'a' : strlcpy (x_rank  , "RMFrf"     , LEN_LABEL);   break;
   default  : strlcpy (x_rank  , ""          , LEN_LABEL);   break;
   }
   x_nrank = strlen (x_rank);
   /*> printf ("SCRP_servos  x_rank (%d) %s\n", x_nrank, x_rank);                     <*/
   --rce;  if (x_nrank == 0) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(cycle)--------------------------*/
   for (i = 0; i < x_nside; ++i) {
      for (j = 0; j < x_nrank; ++j) {
         sprintf (x_label, "%c%c.%s", x_side [i], x_rank [j], a_source + 3);
         /*> printf ("SCRP_servos     x_label %s\n", x_label);                         <*/
         x_index = yKINE__scrp_servo (x_label);
         if (x_index >= 0)  ++c;
      }
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

int         s_len       = 0;
char       *s_q         = "";               /* strtok delimeters         */
char       *s_context   = NULL;               /* strtok context variable   */

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
      p = strtok_r (NULL  , s_q, &s_context);
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
         x_count = yKINE__scrp_servos (p);
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
            if (g_servos [j].scrp != 'y') continue;
            yKINE_move_create (MOVE_SERVO, g_servos + j, a_type, s_lines, x_degs, x_secs);
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

/*---(parsing variables)-----------------*/
static int     s_count   = -1;
static double  s_secs    = -1.0;
static double  s_femu    = -1.0;
static double  s_pate    = -1.0;
static double  s_tibi    = -1.0;
static double  s_xpos    = -1.0;
static double  s_zpos    = -1.0;
static double  s_ypos    = -1.0;

static char  /*--> prepare a IK/FK move parse ------------[ ------ [ ------ ]-*/
yKINE__parse_prep       (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   --rce;  if (a_verb == NULL) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "can not be null");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_info    ("a_verb"    , a_verb);
   x_len = strlen (a_verb);
   DEBUG_YKINE_SCRP  yLOG_value   ("x_len"     , x_len);
   --rce;  if (x_len != 7) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "must be seven characters");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(initialize)---------------------*/
   s_count     = -1;
   s_secs      = FAILED;
   s_femu      = FAILED;
   s_pate      = FAILED;
   s_tibi      = FAILED;
   s_xpos      = FAILED;
   s_zpos      = FAILED;
   s_ypos      = FAILED;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> parse a IK/FK move --------------------[ ------ [ ------ ]-*/
yKINE__parse_fields     (char a_type)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char       *p           = NULL;
   int         i           = 0;
   char        x_request   [LEN_LABEL];
   int         x_len       = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   DEBUG_YKINE_SCRP   yLOG_char    ("a_type"    , a_type);
   /*---(read fields)--------------------*/
   for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {
      /*---(parse field)-----------------*/
      DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
      p = strtok_r (NULL  , s_q, &s_context);
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
         sprintf (x_request, "%c%c.femu", p [0], p [1]);
         s_count = yKINE__scrp_servos (x_request);
         DEBUG_YKINE_SCRP  yLOG_value   ("count"     , s_count);
         --rce;  if (s_count < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_SEC   :  /*---(secs)---*/
         s_secs = atof (p);
         DEBUG_YKINE_SCRP  yLOG_double  ("seconds"   , s_secs);
         --rce;  if (s_secs <= 0.0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("s_secs"    , "can not be negative");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_XPOS  :  /*---(xpos)---*/
         if (a_type == 'i') {
            s_xpos = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos"    , s_xpos);
         } else {
            s_femu = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("s_femu"    , s_femu);
         }
         break;
      case  FIELD_ZPOS  :  /*---(zpos)---*/
         if (a_type == 'i') {
            s_zpos = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos"    , s_zpos);
         } else {
            s_pate = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("s_pate"    , s_pate);
         }
         break;
      case  FIELD_YPOS  :  /*---(ypos)---*/
         if (a_type == 'i') {
            s_ypos = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos"    , s_ypos);
         } else {
            s_tibi = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("s_tibi"    , s_tibi);
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

static char  /*--> check results of a IK/FK parse --------[ ------ [ ------ ]-*/
yKINE__parse_check      (char a_type)
{
   char        rce         = -10;                /* return code for errors    */
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(check duration)-----------------*/
   --rce;  if (s_secs == -666.0) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("s_secs"    , "never read");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(check angles)-------------------*/
   if (a_type == 'f') {
      --rce;  if (s_femu == -666.0) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("s_femu"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      --rce;  if (s_pate == -666.0) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("s_pate"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      --rce;  if (s_tibi == -666.0) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("s_tibi"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   /*---(check endpoint)-----------------*/
   if (a_type == 'i') {
      --rce;  if (s_xpos == -666.0) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("s_xpos"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      --rce;  if (s_zpos == -666.0) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("s_zpos"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
      --rce;  if (s_ypos == -666.0) {
         DEBUG_YKINE_SCRP  yLOG_warn    ("s_ypos"    , "never read");
         DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   /*---(done)---------------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> adjust coordinates based on body ------[ ------ [ ------ ]-*/
yKINE__parse_adjust     (void)
{
   char        rce         = -10;                /* return code for errors    */
   double      x_degs      = 0.0;
   double      x_rads      = 0.0;
   double      x_dist      = 0.0;
   double      x_orig      = 0.0;
   double      x_yadd      = 0.0;
   double      x_more      = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(center changes)-----------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("shift adjustments");
   s_xpos -= s_xcenter;
   s_zpos -= s_zcenter;
   s_ypos -= s_ycenter;
   DEBUG_YKINE_SCRP  yLOG_double  ("new s_xpos", s_xpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("new s_zpos", s_zpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("new s_ypos", s_ypos);
   /*---(yaw)----------------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("yaw calcs");
   x_dist  = sqrt  ((s_xpos * s_xpos) + (s_zpos * s_zpos));
   DEBUG_YKINE_SCRP  yLOG_double  ("x_dist"    , x_dist);
   x_rads  = - atan2 (s_zpos , s_xpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("x_rads"    , x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_degs"    , x_degs);
   x_rads += (s_yaw * DEG2RAD);
   DEBUG_YKINE_SCRP  yLOG_double  ("new x_rads", x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_SCRP  yLOG_double  ("new x_degs", x_degs);
   s_xpos  =   x_dist * cos (x_rads);
   s_zpos  = -(x_dist * sin (x_rads));
   DEBUG_YKINE_SCRP  yLOG_double  ("new s_xpos", s_xpos);
   DEBUG_YKINE_SCRP  yLOG_double  ("new s_zpos", s_zpos);
   /*---(pitch)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("pitch calcs");
   x_dist  = s_zpos;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_dist"    , x_dist);
   if (s_zpos >= 0.0)  x_rads  =  s_pitch;
   else                x_rads  = -s_pitch;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_rads"    , x_rads);
   x_degs  = x_rads * RAD2DEG;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_degs"    , x_degs);
   x_more  = -(x_dist * sin (x_rads));
   DEBUG_YKINE_SCRP  yLOG_double  ("x_more"    , x_more);
   x_dist += x_more;
   DEBUG_YKINE_SCRP  yLOG_double  ("x_dist new", x_dist);
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
   --rce;  if (s_count < 0) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("servos"    , "count is zero, can not be right");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   --rce;  if (s_count > 1) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("servos"    , "multiple selected, not legal");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(process)------------------------*/
   for (i = 0; i < g_nservo; ++i) {
      /*---(filter)----------------------*/
      if (g_servos [i].scrp != 'y') continue;
      /*---(identify len)----------------*/
      x_leg = i / 3.0;
      DEBUG_YKINE_SCRP  yLOG_value   ("x_leg"     , x_leg);
      /*---(clean values)----------------*/
      s_xpos = x_xsave;
      s_zpos = x_zsave;
      s_ypos = x_ysave;
      /*---(adjust)----------------------*/
      rc = yKINE__parse_adjust ();
      /*---(calc angles)-----------------*/
      rc = yKINE_inverse  (x_leg, s_xpos, s_zpos, s_ypos);
      rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &s_femu, &s_pate, &s_tibi);
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , s_femu);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , s_pate);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , s_tibi);
      /*---(add moves)-------------------*/
      rc = yKINE_move_create (MOVE_SERVO, g_servos + i + 0, a_verb, s_lines, s_femu, s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servos + i + 1, a_verb, s_lines, s_pate, s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servos + i + 2, a_verb, s_lines, s_tibi, s_secs);
      rc = yKINE_move_addloc (g_servos + i + 2, s_xpos, s_zpos, s_ypos);
      /*---(update servo)----------------*/
      g_servos [i + 2].saved  = 'y';
      g_servos [i + 2].xsave  = s_xpos;
      g_servos [i + 2].zsave  = s_zpos;
      g_servos [i + 2].ysave  = s_ypos;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
yKINE__scrp_ik_from     (char *a_verb)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         i           = 0;
   int         j           = 0;
   int         x_len       = 0;
   int         x_leg       = 0.0;
   double      x_xsave     = 0.0;
   double      x_zsave     = 0.0;
   double      x_ysave     = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   x_xsave = s_xpos;
   x_zsave = s_zpos;
   x_ysave = s_ypos;
   for (j = 0; j < g_nservo; ++j) {
      if (g_servos [j].scrp != 'y') continue;
      /*---(identify len)----------------*/
      x_leg = j / 3.0;
      DEBUG_YKINE_SCRP  yLOG_value   ("x_leg"     , x_leg);
      /*---(clean values)----------------*/
      s_xpos = x_xsave;
      s_zpos = x_zsave;
      s_ypos = x_ysave;
      DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos"    , s_xpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos"    , s_zpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos"    , s_ypos);
      /*---(calc new)--------------------*/
      DEBUG_YKINE_SCRP  yLOG_double  ("xsave"     , g_servos [j + 2].xsave );
      DEBUG_YKINE_SCRP  yLOG_double  ("zsave"     , g_servos [j + 2].zsave );
      DEBUG_YKINE_SCRP  yLOG_double  ("ysave"     , g_servos [j + 2].ysave );
      s_xpos += g_servos [j + 2].xsave;
      s_zpos += g_servos [j + 2].zsave;
      s_ypos += g_servos [j + 2].ysave;
      DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos new", s_xpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos new", s_zpos);
      DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos new", s_ypos);
      /*---(adjust)----------------------*/
      rc = yKINE__parse_adjust ();
      /*---(get angles)------------------*/
      rc = yKINE_inverse  (x_leg, s_xpos, s_zpos, s_ypos);
      rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &s_femu, &s_pate, &s_tibi);
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , s_femu);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , s_pate);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , s_tibi);
      rc = yKINE_move_create (MOVE_SERVO, g_servos + j + 0, a_verb, s_lines, s_femu, s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servos + j + 1, a_verb, s_lines, s_pate, s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servos + j + 2, a_verb, s_lines, s_tibi, s_secs);
      rc = yKINE_move_addloc (g_servos + j + 2, s_xpos, s_zpos, s_ypos);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

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
      if (g_servos [j].scrp != 'y') continue;
      /*---(save forward)----------------*/
      yKINE_move_create (MOVE_SERVO, g_servos + j + 0, a_verb, s_lines, s_femu, s_secs);
      yKINE_move_create (MOVE_SERVO, g_servos + j + 1, a_verb, s_lines, s_pate, s_secs);
      yKINE_move_create (MOVE_SERVO, g_servos + j + 2, a_verb, s_lines, s_tibi, s_secs);
      /*---(calc inverse)----------------*/
      x_leg = j / 3.0;
      yKINE_forward     (x_leg, s_femu, s_pate, s_tibi);
      yKINE_endpoint    (x_leg, YKINE_TARG, YKINE_FK, NULL, NULL, &s_xpos, &s_zpos, &s_ypos);
      yKINE_move_addloc (g_servos + j + 2, s_xpos, s_zpos, s_ypos);
      /*---(update servo)----------------*/
      g_servos [j + 2].saved  = 'y';
      g_servos [j + 2].xsave  = s_xpos;
      g_servos [j + 2].zsave  = s_zpos;
      g_servos [j + 2].ysave  = s_ypos;
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
   rc = yKINE__parse_prep     (a_verb);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(parse)--------------------------*/
   rc = yKINE__parse_fields   (a_verb [0]);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   rc = yKINE__parse_check   (a_verb [0]);
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
         rc = yKINE__scrp_ik_from   (a_verb);
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
   rc = yKINE__parse_prep     (a_verb);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(parse)--------------------------*/
   rc = yKINE__parse_fields  (x_type);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   rc = yKINE__parse_check   (x_type);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle)-------------------------*/
   --rce;  switch (a_verb [3]) {
   case 'p' :
      s_yaw      = s_femu;
      s_rotate   = s_pate;
      s_pitch    = s_tibi;
      /*> rc = yKINE__scrp_ik_pure   (a_verb);                                        <*/
      break;
   case 'f' :
      s_yaw     += s_femu;
      s_rotate  += s_pate;
      s_pitch   += s_tibi;
      break;
   default  :
      DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("s_yaw"     , s_yaw);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_rotate"  , s_rotate);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_pitch"   , s_pitch);
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
   rc = yKINE__parse_prep     (a_verb);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(parse)--------------------------*/
   rc = yKINE__parse_fields  (x_type);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   rc = yKINE__parse_check   (x_type);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle)-------------------------*/
   --rce;  switch (a_verb [3]) {
   case 'p' :
      s_xcenter  = s_xpos;
      s_zcenter  = s_zpos;
      s_ycenter  = s_ypos;
      rc = yKINE__scrp_ik_pure   (a_verb);
      break;
   case 'f' :
      s_xcenter += s_xpos;
      s_zcenter += s_zpos;
      s_ycenter += s_ypos;
      break;
   default  :
      DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("s_xcenter" , s_xcenter);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_zcenter" , s_zcenter);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ycenter" , s_ycenter);
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
   rc = yKINE__parse_prep     (a_verb);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(parse)--------------------------*/
   rc = yKINE__parse_fields  (x_type);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(check)--------------------------*/
   rc = yKINE__parse_check   (x_type);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rc;
   }
   /*---(handle)-------------------------*/
   --rce;  switch (a_verb [3]) {
   case 'p' :
      s_xcenter  = s_xpos;
      s_zcenter  = s_zpos;
      s_ycenter  = s_ypos;
      rc = yKINE__scrp_ik_pure   (a_verb);
      break;
   case 'f' :
      s_xcenter += s_xpos;
      s_zcenter += s_zpos;
      s_ycenter += s_ypos;
      break;
   default  :
      DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   DEBUG_YKINE_SCRP  yLOG_double  ("s_xcenter" , s_xcenter);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_zcenter" , s_zcenter);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ycenter" , s_ycenter);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         fk movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___FK_MOVES________________o (void) {;}





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
      p = strtok_r (NULL  , s_q, &s_context);
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
         x_servo = yKINE__scrp_servos (x_request);
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
            if (g_servos [j].scrp != 'y') continue;
            yKINE_move_create (MOVE_SERVO, g_servos + j + 0, "forward", s_lines, x_femu, x_secs);
            yKINE_move_create (MOVE_SERVO, g_servos + j + 1, "forward", s_lines, x_pate, x_secs);
            yKINE_move_create (MOVE_SERVO, g_servos + j + 2, "forward", s_lines, x_tibi, x_secs);
            x_leg = j / 3.0;
            yKINE_forward     (x_leg, x_femu, x_pate, x_tibi);
            yKINE_endpoint    (x_leg, YKINE_TARG, YKINE_FK, NULL, NULL, &x_xpos, &x_zpos, &x_ypos);
            yKINE_move_addloc (g_servos + j + 2, x_xpos, x_zpos, x_ypos);
            /*---(update servo)----------------*/
            g_servos [j + 2].saved  = 'y';
            g_servos [j + 2].xsave  = x_xpos;
            g_servos [j + 2].zsave  = x_zpos;
            g_servos [j + 2].ysave  = x_ypos;
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
      p = strtok_r (NULL  , s_q, &s_context);
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
         x_servo = yKINE__scrp_servos (p);
         --rce;  if (x_servo < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         for (j = 0; j < g_nservo; ++j) {
            if (g_servos [j].scrp != 'y') continue;
            g_servos [j].segno_flag = 'y';
            g_servos [j].segno      = NULL;
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
      p = strtok_r (NULL  , s_q, &s_context);
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
         x_servo = yKINE__scrp_servos (p);
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
            if (g_servos [j].scrp != 'y') continue;
            yKINE_move_repeat     (g_servos + j, x_count, x_times);
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
      p = strtok_r (NULL  , s_q, &s_context);
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
         x_servo = yKINE__scrp_servos (p);
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
            if (g_servos [j].scrp != 'y') continue;
            yKINE_move_dsegno   (g_servos + j, x_times);
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
/*===----                        gait verbs                            ----===*/
/*====================------------------------------------====================*/
static void      o___GAITS___________________o (void) {;}

tMOVE      *s_gait_begin  [YKINE_MAX_SERVO];

static char  /*--> parse a high level repeat -------------[ ------ [ ------ ]-*/
yKINE__scrp_b6gait  (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   char       *p           = NULL;
   int         x_len       = 0;
   int         x_servo     = 0;
   char        x_request   [LEN_LABEL];
   double      x_deg       = 0.0;
   double      x_xpos      = 0.0;
   double      x_zpos      = 0.0;
   double      x_ypos      = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(parse field)-----------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("read next field");
   p = strtok_r (NULL  , s_q, &s_context);
   --rce;  if (p == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   strltrim (p, ySTR_BOTH, LEN_RECD);
   x_len = strlen (p);
   DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);
   /*---(handle)----------------------*/
   sprintf (x_request, "%s.femu", p);
   x_servo = yKINE__scrp_servos (x_request);
   sprintf (x_request, "%s.pate", p);
   x_servo = yKINE__scrp_servos (x_request);
   sprintf (x_request, "%s.tibi", p);
   x_servo = yKINE__scrp_servos (x_request);
   --rce;  if (x_servo < 0) {
      DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
      DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(mark servos)--------------------*/
   for (i = 0; i < g_nservo; ++i) {
      s_gait_begin [i] = NULL;
      if (g_servos [i].scrp != 'y') continue;
      DEBUG_YKINE_SCRP   yLOG_value    ("servo"     , i);
      DEBUG_YKINE_SCRP   yLOG_info     ("name"      , g_servos [i].label);
      yKINE_move_last   (i, NULL, &x_deg);
      yKINE_move_curdata(&x_xpos, &x_zpos, &x_ypos);
      DEBUG_YKINE_SCRP   yLOG_double   ("last deg"  , x_deg);
      yKINE_move_create (YKINE_MOVE_NOTE , g_servos + i, "6_GAIT_BEG"   , s_lines, 0.0  , 0.0);
      yKINE_move_create (YKINE_MOVE_WAIT , g_servos + i, "wait for turn", s_lines, x_deg, 3.0);
      yKINE_move_addloc (g_servos + i, x_xpos, x_zpos, x_ypos);
      yKINE_move_create (YKINE_MOVE_SERVO, g_servos + i, "up to neutral", s_lines, 0.0  , 0.5);
      yKINE_move_create (YKINE_MOVE_SERVO, g_servos + i, "step to first", s_lines, 0.0  , 0.5);
      yKINE_move_create (YKINE_MOVE_WAIT , g_servos + i, "wait for legs", s_lines, 0.0  , 3.0);
      DEBUG_YKINE_SCRP   yLOG_point    ("saved"     , g_servos [i].tail);
      s_gait_begin [i] = g_servos [i].tail;
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

static char  /*--> parse a high level repeat -------------[ ------ [ ------ ]-*/
yKINE__scrp_e6gait  (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   int         i           = 0;
   int         j           = 0;
   int         k           = 0;
   char       *p           = NULL;
   int         x_len       = 0;
   char        x_order     [LEN_LABEL];
   int         x_leg       = 0;
   int         x_snum      = 0;
   tSERVO     *x_servo     = NULL;
   tMOVE      *x_start     = NULL;
   tMOVE      *x_next      = NULL;
   tMOVE      *x_prev      = NULL;
   double      x_inix      = 0.0;
   double      x_iniz      = 0.0;
   double      x_iniy      = 0.0;
   double      x_inid      = 0.0;
   double      x_neux      = 0.0;
   double      x_neuz      = 0.0;
   double      x_neuy      = 0.0;
   double      x_neud      = 0.0;
   double      x_1stx      = 0.0;
   double      x_1stz      = 0.0;
   double      x_1sty      = 0.0;
   double      x_1std      = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(parse leg order)----------------*/
   DEBUG_YKINE_SCRP   yLOG_note    ("parse leg order field");
   p = strtok_r (NULL  , s_q, &s_context);
   --rce;  if (p == NULL) {
      DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   }
   strltrim (p, ySTR_BOTH, LEN_RECD);
   x_len = strlen (p);
   strlcpy (x_order, p, LEN_LABEL);
   DEBUG_YKINE_SCRP  yLOG_info    ("x_order"   , x_order);
   DEBUG_YKINE_SCRP  yLOG_value   ("x_len"     , x_len);
   /*---(walk through leg order)---------*/
   for (i = 0; i < x_len; ++i) {
      x_leg = (int) (x_order [i] - '0');
      DEBUG_YKINE_SCRP  yLOG_value   ("x_leg"     , x_leg);
      /*---(handle three servos each)----*/
      for (j = 0; j < 3; ++j) {
         /*---(figure starting move)-----*/
         x_snum  = (x_leg * 3) + j;
         DEBUG_YKINE_SCRP  yLOG_value   ("x_snum"    , x_snum);
         x_servo = g_servos + x_snum;
         DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
         /*---(get neutral data)---------*/
         x_start = s_gait_begin [x_snum]->s_next;
         DEBUG_YKINE_SCRP  yLOG_point   ("x_start"   , x_start);
         x_neux  = x_start->x_pos;
         x_neuz  = x_start->z_pos;
         x_neuy  = x_start->y_pos;
         x_neud  = x_start->deg_end;
         /*---(delete/copy to first)-----*/
         for (k = 0; k < i; ++k) {
            x_next  = x_start->s_next;
            yKINE_move_create (YKINE_MOVE_SERVO, x_servo , "extended", x_start->line, x_start->deg_end, x_start->sec_dur);
            yKINE_move_addloc (x_servo, x_start->x_pos, x_start->z_pos, x_start->y_pos);
            yKINE_move_delete (x_start);
            x_start = x_next;
            DEBUG_YKINE_SCRP  yLOG_point   ("x_start"   , x_start);
         }
         /*---(get first data)-----------*/
         x_1stx  = x_start->x_pos;
         x_1stz  = x_start->z_pos;
         x_1sty  = x_start->y_pos;
         x_1std  = x_start->deg_end;
         /*---(update first)-------------*/
         x_start->deg_beg = x_start->deg_end;
         /*---(fix first wait)-----------*/
         x_start = s_gait_begin [x_snum]->s_prev->s_prev->s_prev;
         x_start->sec_dur = (double) (5 - i) * 0.500;
         x_inix  = x_start->x_pos;
         x_iniz  = x_start->z_pos;
         x_iniy  = x_start->y_pos;
         x_inid  = x_start->deg_end;
         /*---(fix neutral)--------------*/
         x_start = s_gait_begin [x_snum]->s_prev->s_prev;
         x_start->x_pos           = x_neux;
         x_start->z_pos           = x_neuz;
         x_start->y_pos           = x_neuy;
         x_start->deg_end         = x_neud;
         x_start->s_next->deg_beg = x_neud;
         /*---(fix first)----------------*/
         x_start = s_gait_begin [x_snum]->s_prev;
         x_start->x_pos           = x_1stx;
         x_start->z_pos           = x_1stz;
         x_start->y_pos           = x_1sty;
         x_start->deg_end         = x_1std;
         /*---(fix second wait)----------*/
         x_start = s_gait_begin [x_snum];
         x_start->x_pos           = x_1stx;
         x_start->z_pos           = x_1stz;
         x_start->y_pos           = x_1sty;
         x_start->deg_beg         = x_1std;
         x_start->deg_end         = x_1std;
         x_start->sec_dur = (double) i * 0.500;
         /*---(fix waiting)--------------*/
         x_start = s_gait_begin [x_snum]->s_prev->s_prev->s_prev;
         while (x_start != NULL) {
            x_start->sec_beg = x_start->s_prev->sec_end;
            x_start->sec_end = x_start->sec_beg + x_start->sec_dur;
            x_start = x_start->s_next;
         }
         /*---(add to end)---------------*/
         x_start = x_servo->tail;
         yKINE_move_create (YKINE_MOVE_WAIT , x_servo , "wait turn"    , -1     , x_start->deg_end  , i * 0.500);
         yKINE_move_addloc (x_servo, x_start->x_pos, x_start->z_pos, x_start->y_pos);
         yKINE_move_create (YKINE_MOVE_SERVO, x_servo , "neutral"  , -1           , x_neud          , 0.500           );
         yKINE_move_addloc (x_servo, x_neux, x_neuz, x_neuy);
         yKINE_move_create (YKINE_MOVE_SERVO, x_servo , "back down", -1           , x_inid          , 0.500           );
         yKINE_move_addloc (x_servo, x_inix, x_iniz, x_iniy);
         yKINE_move_create (YKINE_MOVE_WAIT , x_servo , "wait all legs", -1       , x_inid          , (5 - i) * 0.500);
         yKINE_move_addloc (x_servo, x_inix, x_iniz, x_iniy);
         yKINE_move_create (YKINE_MOVE_NOTE , x_servo , "6_GAIT_END", s_lines, 0.0, 0.0);
         yKINE_move_addloc (x_servo, 0.0, 0.0, 0.0);
      }
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        script driver                         ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVER__________________o (void) {;}

static char  /* file reading driver ----------------------[--------[--------]-*/
yKINE__scrp_prep   (void)
{
   int         i           = 0;
   for (i = 0; i < g_nservo; ++i) {
      g_servos [i].scrp  = '-';
   }
   for (i = 0; i < MAX_SCRPARG; ++i) {
      if (strcmp (s_scrparg [i].name, "end-of-list") == 0)  break;
      s_scrparg [i].flag = '-';
   }
   return 0;
}

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
   yKINE__scrp_init ();
   /*---(read lines)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("read lines");
   while (1) {
      /*---(prepare)---------------------*/
      yKINE__scrp_prep     ();
      /*---(read and clean)--------------*/
      ++s_lines;
      DEBUG_YKINE_SCRP  yLOG_value   ("line"      , s_lines);
      fgets (s_recd, LEN_RECD, stdin);
      if (feof (stdin))  {
         DEBUG_YKINE_SCRP  yLOG_note    ("end of file reached");
         break;
      }
      s_len = strlen (s_recd);
      if (s_len <= 0)  {
         DEBUG_YKINE_SCRP  yLOG_note    ("record empty");
         continue;
      }
      s_recd [--s_len] = '\0';
      DEBUG_YKINE_SCRP  yLOG_value   ("length"    , s_len);
      DEBUG_YKINE_SCRP  yLOG_info    ("fixed"     , s_recd);
      if (s_recd [0] == '#') {
         DEBUG_YKINE_SCRP  yLOG_note    ("comment line, skipping");
         continue;
      }

      /*---(get recd type)---------------*/
      p = strtok_r (s_recd, s_q, &s_context);
      if (p == NULL) {
         DEBUG_YKINE_SCRP  yLOG_note    ("can not parse type field");
         continue;
      }
      strltrim  (p, ySTR_BOTH, LEN_RECD);
      strncpy   (x_verb, p,  10);
      DEBUG_YKINE_SCRP  yLOG_info    ("type"      , x_verb);
      /*---(get version)-----------------*/
      p = strtok_r (NULL     , s_q, &s_context);
      if (p == NULL) {
         DEBUG_YKINE_SCRP  yLOG_note    ("can not parse version field");
         continue;
      }
      strltrim  (p, ySTR_BOTH, LEN_RECD);
      if (strlen (p) != 3) {
         DEBUG_YKINE_SCRP  yLOG_note    ("invalid length version field (3)");
         continue;
      }
      if (p[0] != '-' || p[2] != '-') {
         DEBUG_YKINE_SCRP  yLOG_note    ("invalid format version field (-x-)");
         continue;
      }
      x_ver = p[1];
      DEBUG_YKINE_SCRP  yLOG_char    ("version"   , x_ver);
      /*---(handle types)----------------*/
      switch (x_verb [0]) {
      case '6' : /* gait               */
         if        (strcmp ("6_GAIT_BEG"   , x_verb) == 0) {
            yKINE__scrp_b6gait  ();
         } else if (strcmp ("6_GAIT_END"   , x_verb) == 0) {
            yKINE__scrp_e6gait  ();
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
      yKINE_move_last (i, &x_sec, NULL);
      if (x_sec > x_len)  x_len = x_sec;
   }
   yKINE_its.scrp_len = x_len;
   if (a_len != NULL)  *a_len = x_len;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP yLOG_exit    (__FUNCTION__);
   return 0;
}



/*============================----end-of-source---============================*/
