/*============================----beg-of-source---============================*/

/*---(headers)---------------------------*/
#include    "yKINE.h"
#include    "yKINE_priv.h"


#define     MOVE_NULL   '-'
#define     MOVE_PAUSE  'p'
#define     MOVE_SERVO  's'

#define    PARSE_ANGLE        'a'
#define    PARSE_POINT        'p'
#define    PARSE_MUSIC        'c'
#define    PARSE_OTHER        '-'

#define    MAX_VERBS       100
typedef    struct  cVERBS  tVERBS;
struct cVERBS {
   char        name        [LEN_LABEL];     /* full verb name                 */
   char        active;                      /* allowed for use                */
   char        vers        [LEN_LABEL];     /* allowed versions               */
   char        target;                      /* leg/seg targeting override     */
   char        from;                        /* pure vs from coding            */
   char        type;                        /* field interpretation type      */
   char        (*call)     (void);          /* function pointer               */
   char        desc        [LEN_STR  ];     /* english description            */
};
tVERBS   s_verb_info    [MAX_VERBS] = {
   /* ===[[ inverse kinematics ]]================================================*/
   /* verb----------- actv- vers--------- targ---------- rel-------- type--------- call----------------- description---------------------------------------- */
   { "ik_pure"      , 'y' , "ab"        , YKINE_INVERSE, YKINE_PURE, PARSE_POINT , ykine_scrp_ik       , "set an exact endpoint in 3d space"                 },
   { "ik_down"      , '-' , "ab"        , YKINE_INVERSE, YKINE_DOWN, PARSE_POINT , ykine_scrp_ik       , "set an exact endpoint, except rel to ground (y)"   },
   { "ik_from"      , 'y' , "ab"        , YKINE_INVERSE, YKINE_FROM, PARSE_POINT , ykine_scrp_ik       , "set a relative endpoint based on last position"    },
   /* ===[[ forward kinematics ]]================================================*/
   /* verb----------- actv- vers--------- targ---------- rel-------- type--------- call----------------- description---------------------------------------- */
   { "fk_pure"      , 'y' , "ab"        , YKINE_FORWARD, YKINE_PURE, PARSE_ANGLE , ykine_scrp_fk       , "set absolute joint angles on all three joints"     },
   { "fk_from"      , 'y' , "ab"        , YKINE_FORWARD, YKINE_FROM, PARSE_ANGLE , ykine_scrp_fk       , "set relative joint angles based on last angle"     },
   /* ===[[ body pos/orient ]]===================================================*/
   /* verb----------- actv- vers--------- targ---------- rel-------- type--------- call----------------- description---------------------------------------- */
   { "ze_pure"      , 'y' , "ab"        , YKINE_ZERO   , YKINE_PURE, PARSE_POINT , ykine_scrp_zero     , "set absolute body position in 3d space"            },
   { "ze_from"      , 'y' , "ab"        , YKINE_ZERO   , YKINE_FROM, PARSE_POINT , ykine_scrp_zero     , "set relative body position based on last position" },
   { "or_pure"      , 'y' , "ab"        , YKINE_ORIENT , YKINE_PURE, PARSE_ANGLE , ykine_scrp_orient   , "set absolute body orientation angles"              },
   { "or_from"      , 'y' , "ab"        , YKINE_ORIENT , YKINE_FROM, PARSE_ANGLE , ykine_scrp_orient   , "set relative body orientation from last position"  },
   /* ===[[ music notation ]]====================================================*/
   /* verb----------- actv- vers--------- targ---------- rel-------- type--------- call----------------- description---------------------------------------- */
   { "meter"        , 'y' , "a"         , YKINE_NONE   , YKINE_NONE, PARSE_MUSIC , ykine_scrp_repeat   , "time signature for rhythm/beat"                    },
   { "tempo"        , 'y' , "a"         , YKINE_NONE   , YKINE_NONE, PARSE_MUSIC , ykine_scrp_repeat   , "pace of music in beats-per-minute"                 },
   { "REPEAT"       , 'y' , "ab"        , YKINE_NONE   , YKINE_NONE, PARSE_MUSIC , ykine_scrp_repeat   , "repeat a specific number of steps"                 },
   /* ===[[ gait framework ]]====================================================*/
   /* verb----------- actv- vers--------- targ---------- rel-------- type--------- call----------------- description---------------------------------------- */
   { "06_GAIT_BEG"  , 'y' , "a"         , YKINE_NONE   , YKINE_NONE, PARSE_OTHER , ykine_gait_06_beg   , "begin a 6-step gait description"                   },
   { "06_GAIT_END"  , 'y' , "a"         , YKINE_NONE   , YKINE_NONE, PARSE_OTHER , ykine_gait_06_end   , "end a 6-step gait description"                     },
   { "12_GAIT_BEG"  , 'y' , "a"         , YKINE_NONE   , YKINE_NONE, PARSE_OTHER , ykine_gait_12_beg   , "begin a 12-step gait description"                  },
   { "12_GAIT_END"  , 'y' , "a"         , YKINE_NONE   , YKINE_NONE, PARSE_OTHER , ykine_gait_12_end   , "end a 12-step gait description"                    },
   /* ===[[ complex gaits ]]=====================================================*/
   /* verb----------- actv- vers--------- targ---------- rel-------- type--------- call----------------- description---------------------------------------- */
   { "walk"         , '-' , "a"         , YKINE_NONE   , YKINE_NONE, PARSE_OTHER , NULL                , "repeat a specific number of steps"                 },
   { "circle"       , '-' , "a"         , YKINE_NONE   , YKINE_NONE, PARSE_OTHER , NULL                , "repeat a specific number of steps"                 },
   /* done-------------------*/
};


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
   myKINE.scrp_len    = 0.0;
   myKINE.s_lines     =   0;
   strlcpy (myKINE.s_q, "", LEN_LABEL);
   myKINE.s_context   = NULL;
   return 0;
}

char  /* file reading driver ----------------------[--------[--------]-*/
ykine__scrp_prep   (void)
{
   int         i           = 0;
   ykine_servo_prep ();
   for (i = 0; i < MAX_SCRPARG; ++i) {
      if (strcmp (s_scrparg [i].name, "end-of-list") == 0)  break;
      s_scrparg [i].flag = '-';
   }
   /*---(complete)-----------------------*/
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
#define     FIELD_COUNT    2
#define     FIELD_TIMES    3
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
         x_count = ykine_servos (p);
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
   myKINE.s_iverb     = -1;
   myKINE.s_targ      = '-';
   myKINE.s_type      = '-';
   myKINE.s_vers      = '-';
   myKINE.s_count     = -1;
   myKINE.s_beats     = 0.0;
   myKINE.s_secs      = 0.0;
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
   /*---(clean)--------------------------*/
   myKINE.s_len = strlen (myKINE.s_recd);
   if (myKINE.s_len > 0)  myKINE.s_recd [--myKINE.s_len] = 0;
   DEBUG_YKINE_SCRP   yLOG_value   ("length"    , myKINE.s_len);
   DEBUG_YKINE_SCRP   yLOG_info    ("fixed"     , myKINE.s_recd);
   /*---(defense)------------------------*/
   if (myKINE.s_len <= 0)  {
      DEBUG_YKINE_SCRP   yLOG_note    ("record empty/blank");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (myKINE.s_recd [0] == '#') {
      DEBUG_YKINE_SCRP   yLOG_note    ("comment line, skipping");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (myKINE.s_len <= 5)  {
      DEBUG_YKINE_SCRP   yLOG_note    ("record too short");
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
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
   myKINE.s_iverb = rc;
   myKINE.s_targ = s_verb_info [myKINE.s_iverb].target;
   myKINE.s_from = s_verb_info [myKINE.s_iverb].from;
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
ykine_parse_fields          (float *a, float *b, float *c, float *d)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          =   0;                /* generic return code       */
   char       *p           = NULL;
   int         i           = 0;
   char        x_request   [LEN_LABEL];
   int         x_len       = 0;
   float       x_temp      =  0.0;
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
      if (i == FIELD_ARGS) {
         DEBUG_YKINE_SCRP  yLOG_note    ("done");
         DEBUG_YKINE_SCRP  yLOG_note    ("over");
         DEBUG_YKINE_SCRP  yLOG_note    ("gone");
         DEBUG_YKINE_SCRP  yLOG_note    ("finito");
         break;
      }
      /*---(handle)----------------------*/
      switch (i) {
      case  FIELD_SVO   :  /*---(servo)----*/
         switch (p [0]) {
         case 'z' :  sprintf (x_request, "%c%c.zero"  , p [0], p [1]); break;
         case 'o' :  sprintf (x_request, "%c%c.yaw"   , p [0], p [1]); break;
         case 'i' :  sprintf (x_request, "%c%c.tibi"  , p [0], p [1]); break;
         default  :  sprintf (x_request, "%c%c.femu"  , p [0], p [1]); break;
         }
         myKINE.s_count = ykine_servos (x_request);
         DEBUG_YKINE_SCRP  yLOG_value   ("count"     , myKINE.s_count);
         --rce;  if (myKINE.s_count < 0) {
            DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");
            DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
            return rce;
         }
         break;
      case  FIELD_SEC   :  /*---(secs)---*/
         x_temp = atof (p);
         if (a != NULL) {
            if (x_temp < 0.0) {
               DEBUG_YKINE_SCRP  yLOG_note    ("sec/count value can not be negative");
               DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);
               return rce;
            }
            *a = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("a"         , *a);
         }
         break;
      case  FIELD_XPOS  :  /*---(xpos)---*/
         if (b != NULL) {
            *b = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("b"         , *b);
         }
         break;
      case  FIELD_ZPOS  :  /*---(zpos)---*/
         if (c != NULL) {
            *c = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("c"         , *c);
         }
         break;
      case  FIELD_YPOS  :  /*---(ypos)---*/
         if (d != NULL) {
            *d = atof (p);
            DEBUG_YKINE_SCRP  yLOG_double  ("d"         , *d);
         }
         break;
      case  FIELD_ARGS  :  /*---(args)-----*/
         rc = yKINE__scrp_args (p);
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


char  /*--> parse a IK/FK move --------------------[ ------ [ ------ ]-*/
ykine_parse_fields_OLD      (void)
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
         if (p [0] == '-')  sprintf (x_request, "%c%c.zero"  , p [0], p [1]);
         else               sprintf (x_request, "%c%c.femu"  , p [0], p [1]);
         myKINE.s_count = ykine_servos (x_request);
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
   x_rads  = myKINE.s_roll * DEG2RAD;
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


char  /*--> parse a full record -------------------[ ------ [ ------ ]-*/
ykine_parse             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors    */
   char        rc          =    0;
   char        x_type      =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(read and clean)-----------------*/
   rc = ykine_parse_read     ();
   DEBUG_YKINE_SCRP  yLOG_value   ("read"      , rc);
   --rce;  if (feof (stdin)) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
      return rce;
   }
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   DEBUG_YKINE_SCRP  yLOG_value   ("iverb"     , myKINE.s_iverb);
   if (myKINE.s_iverb < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(prepare)------------------------*/
   rc = ykine__scrp_prep     ();
   DEBUG_YKINE_SCRP  yLOG_value   ("prep"      , rc);
   if (rc < 0) {
      DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(parsing)------------------------*/
   x_type = s_verb_info [myKINE.s_iverb].type;
   DEBUG_YKINE_SCRP  yLOG_char    ("type"      , x_type);
   switch (x_type) {
   case PARSE_POINT :
      rc = ykine_parse_fields  (&myKINE.s_beats, &myKINE.s_xpos, &myKINE.s_zpos, &myKINE.s_ypos);
      break;
   case PARSE_ANGLE :
      rc = ykine_parse_fields  (&myKINE.s_beats, &myKINE.s_femu, &myKINE.s_pate, &myKINE.s_tibi);
      break;
   case PARSE_MUSIC :
      rc = ykine_parse_fields  (&myKINE.s_1st  , &myKINE.s_2nd , NULL, NULL);
      break;
   }
   DEBUG_YKINE_SCRP  yLOG_note    ("checking");
   if (x_type != PARSE_OTHER) {
      DEBUG_YKINE_SCRP  yLOG_value   ("parse"     , rc);
      if (rc < 0) {
         DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
   }
   DEBUG_YKINE_SCRP  yLOG_note    ("checking two");
   /*---(check)--------------------------*/
   if (x_type == PARSE_POINT || x_type == PARSE_ANGLE) {
      rc = ykine_parse_check   ();
      DEBUG_YKINE_SCRP  yLOG_value   ("parse"     , rc);
      if (rc < 0) {
         DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
   }
   DEBUG_YKINE_SCRP  yLOG_note    ("checking three");
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         fk movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___FK_MOVES________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_fk_adjust   (int a_leg, int a_seg, float *a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(filter)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from != YKINE_FROM) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(from)---------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("dsave"     , x_servo->dsave);
   DEBUG_YKINE_SCRP  yLOG_double  ("a_deg"     , *a_deg);
   *a_deg += x_servo->dsave;
   DEBUG_YKINE_SCRP  yLOG_double  ("a_deg new" , *a_deg);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_fk_servo    (int a_leg, int a_seg, float a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   myKINE.s_secs = myKINE.s_beats * x_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_secs"    , myKINE.s_secs);
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, a_deg, myKINE.s_secs);
   if (rc == 0)  rc = yKINE_endpoint    (a_leg, a_seg, YKINE_FK, NULL, NULL, &myKINE.s_xpos, &myKINE.s_zpos, &myKINE.s_ypos);
   if (rc == 0)  rc = yKINE_move_addloc (x_servo, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = myKINE.s_xpos;
   x_servo->zsave  = myKINE.s_zpos;
   x_servo->ysave  = myKINE.s_ypos;
   x_servo->dsave  = a_deg;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle forward verbs ------------------[ ------ [ ------ ]-*/
ykine_scrp_fk           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   int         x_leg       = 0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      if (ykine_servo_unfocused (x_leg, YKINE_FEMU))  continue;
      /*---(adjust angles)---------------*/
      rc = ykine__scrp_fk_adjust (x_leg, YKINE_FEMU, &myKINE.s_femu);
      rc = ykine__scrp_fk_adjust (x_leg, YKINE_PATE, &myKINE.s_pate);
      rc = ykine__scrp_fk_adjust (x_leg, YKINE_TIBI, &myKINE.s_tibi);
      /*---(calculate endpoint)----------*/
      rc = yKINE_forward     (x_leg, myKINE.s_femu, myKINE.s_pate, myKINE.s_tibi);
      DEBUG_YKINE_SCRP  yLOG_value   ("forward"   , rc);
      /*---(process moves)---------------*/
      rc = ykine__scrp_fk_servo  (x_leg, YKINE_FEMU, myKINE.s_femu);
      rc = ykine__scrp_fk_servo  (x_leg, YKINE_PATE, myKINE.s_pate);
      rc = ykine__scrp_fk_servo  (x_leg, YKINE_TIBI, myKINE.s_tibi);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         ik movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___IK_MOVES________________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_ik_adjust   (int a_leg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_seg       =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(filter)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from != YKINE_FROM && myKINE.s_from != YKINE_DOWN) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, YKINE_TIBI);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(x-pos)--------------------------*/
   if (myKINE.s_from == YKINE_FROM) {
      DEBUG_YKINE_SCRP  yLOG_double  ("xsave"     , x_servo->xsave);
      DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos"    , myKINE.s_xpos);
      myKINE.s_xpos += x_servo->xsave;
      DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos new", myKINE.s_xpos);
   }
   /*---(z-pos)--------------------------*/
   if (myKINE.s_from == YKINE_FROM) {
      DEBUG_YKINE_SCRP  yLOG_double  ("zsave"     , x_servo->zsave);
      DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos"    , myKINE.s_zpos);
      myKINE.s_zpos += x_servo->zsave;
      DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos new", myKINE.s_zpos);
   }
   /*---(y-pos)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("ysave"     , x_servo->ysave);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos"    , myKINE.s_ypos);
   myKINE.s_ypos += x_servo->ysave;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos new", myKINE.s_ypos);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_ik_servo    (int a_leg, int a_seg, float a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (a_leg, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("s_beats"   , myKINE.s_beats);
   DEBUG_YKINE_SCRP  yLOG_double  ("pase"      , x_servo->pace);
   myKINE.s_secs = myKINE.s_beats * x_servo->pace;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_secs"    , myKINE.s_secs);
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, a_deg, myKINE.s_secs);
   if (rc == 0)  rc = yKINE_endpoint    (a_leg, a_seg, YKINE_IK, NULL, NULL, &myKINE.s_xpos, &myKINE.s_zpos, &myKINE.s_ypos);
   if (rc == 0)  rc = yKINE_move_addloc (x_servo, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = myKINE.s_xpos;
   x_servo->zsave  = myKINE.s_zpos;
   x_servo->ysave  = myKINE.s_ypos;
   x_servo->dsave  = a_deg;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle inverse verbs ------------------[ ------ [ ------ ]-*/
ykine_scrp_ik           (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      if (ykine_servo_unfocused (x_leg, YKINE_TIBI))  continue;
      /*---(adjust positions)------------*/
      rc = ykine__scrp_ik_adjust (x_leg);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      /*---(inverse kinematics)----------*/
      rc = yKINE_inverse_adapt (x_leg, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      DEBUG_YKINE_SCRP  yLOG_value   ("inverse"   , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      /*---(retrieve angles)-------------*/
      rc = yKINE_angles   (x_leg, YKINE_IK, NULL, &myKINE.s_femu, &myKINE.s_pate, &myKINE.s_tibi);
      DEBUG_YKINE_SCRP  yLOG_value   ("angles"    , rc);
      if (rc <  0) {
         DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
         return rc;
      }
      DEBUG_YKINE_SCRP  yLOG_double  ("femu deg"  , myKINE.s_femu);
      DEBUG_YKINE_SCRP  yLOG_double  ("pate deg"  , myKINE.s_pate);
      DEBUG_YKINE_SCRP  yLOG_double  ("tibi deg"  , myKINE.s_tibi);
      /*---(process moves)---------------*/
      rc = ykine__scrp_ik_servo  (x_leg, YKINE_FEMU, myKINE.s_femu);
      rc = ykine__scrp_ik_servo  (x_leg, YKINE_PATE, myKINE.s_pate);
      rc = ykine__scrp_ik_servo  (x_leg, YKINE_TIBI, myKINE.s_tibi);
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save abolute ik based move ------------[ ------ [ ------ ]-*/
ykine_scrp_ik_pure      (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   double      x_xsave     =  0.0;
   double      x_zsave     =  0.0;
   double      x_ysave     =  0.0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (x_leg = 0; x_leg < YKINE_MAX_LEGS; ++x_leg) {
      /*---(filter)----------------------*/
      if (ykine_servo_unfocused (x_leg, YKINE_FEMU))  continue;
      /*---(clean values)----------------*/
      /*> myKINE.s_xpos = x_xsave;                                                    <* 
       *> myKINE.s_zpos = x_zsave;                                                    <* 
       *> myKINE.s_ypos = x_ysave;                                                    <*/
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
      /*> rc = yKINE_move_create (MOVE_SERVO, g_servo_info + i + 0, myKINE.s_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);   <* 
       *> rc = yKINE_move_create (MOVE_SERVO, g_servo_info + i + 1, myKINE.s_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);   <* 
       *> rc = yKINE_move_create (MOVE_SERVO, g_servo_info + i + 2, myKINE.s_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);   <* 
       *> rc = yKINE_move_addloc (g_servo_info + i + 2, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);                               <*/
      /*---(add femur)-------------------*/
      x_servo = ykine_servo_pointer (x_leg, YKINE_FEMU);
      if (x_servo ==  NULL)  continue;
      rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);
      /*---(add patella)-----------------*/
      x_servo = ykine_servo_pointer (x_leg, YKINE_PATE);
      if (x_servo ==  NULL)  continue;
      rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);
      /*---(add tibia)-------------------*/
      x_servo = ykine_servo_pointer (x_leg, YKINE_TIBI);
      if (x_servo ==  NULL)  continue;
      rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);
      /*---(add endpoint)----------------*/
      rc = yKINE_move_addloc (x_servo, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
      /*---(update servo)----------------*/
      x_servo->saved  = 'y';
      x_servo->xsave  = myKINE.s_xpos;
      x_servo->zsave  = myKINE.s_zpos;
      x_servo->ysave  = myKINE.s_ypos;
      /*---(done)------------------------*/
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine_scrp_ik_from      (void)
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
   /*---(parse)--------------------------*/
   rc = ykine_parse_fields_OLD   ();
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
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + j + 0, myKINE.s_verb, myKINE.s_lines, myKINE.s_femu, myKINE.s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + j + 1, myKINE.s_verb, myKINE.s_lines, myKINE.s_pate, myKINE.s_secs);
      rc = yKINE_move_create (MOVE_SERVO, g_servo_info + j + 2, myKINE.s_verb, myKINE.s_lines, myKINE.s_tibi, myKINE.s_secs);
      rc = yKINE_move_addloc (g_servo_info + j + 2, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   }
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       body movements                         ----===*/
/*====================------------------------------------====================*/
static void      o___BODY_MOVES______________o (void) {;}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_zero_adjust (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_seg       =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(filter)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from != YKINE_FROM) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(x-pos)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("xsave"     , x_servo->xsave);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos"    , myKINE.s_xpos);
   myKINE.s_xpos += x_servo->xsave;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_xpos new", myKINE.s_xpos);
   /*---(z-pos)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("zsave"     , x_servo->zsave);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos"    , myKINE.s_zpos);
   myKINE.s_zpos += x_servo->zsave;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_zpos new", myKINE.s_zpos);
   /*---(y-pos)--------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("ysave"     , x_servo->ysave);
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos"    , myKINE.s_ypos);
   myKINE.s_ypos += x_servo->ysave;
   DEBUG_YKINE_SCRP  yLOG_double  ("s_ypos new", myKINE.s_ypos);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle body centering -----------------[ ------ [ ------ ]-*/
ykine_scrp_zero         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   int         x_leg       =  0.0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   x_leg = YKINE_BODY;
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, YKINE_FOCU);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(adjust positions)------------*/
   rc = ykine__scrp_zero_adjust ();
   if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(process moves)---------------*/
   myKINE.s_secs = myKINE.s_beats * x_servo->pace;
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, 0.0, myKINE.s_secs);
   if (rc == 0)  rc = yKINE_move_addloc (x_servo, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = myKINE.s_xpos;
   x_servo->zsave  = myKINE.s_zpos;
   x_servo->ysave  = myKINE.s_ypos;
   x_servo->dsave  = 0.0;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_orient_adjust   (int a_seg, float *a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(filter)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_char    ("from"      , myKINE.s_from);
   if (myKINE.s_from != YKINE_FROM) {
      DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(from)---------------------------*/
   DEBUG_YKINE_SCRP  yLOG_double  ("dsave"     , x_servo->dsave);
   DEBUG_YKINE_SCRP  yLOG_double  ("a_deg"     , *a_deg);
   *a_deg += x_servo->dsave;
   DEBUG_YKINE_SCRP  yLOG_double  ("a_deg new" , *a_deg);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char  /*--> save relative ik based move -----------[ ------ [ ------ ]-*/
ykine__scrp_orient_servo    (int a_seg, float a_deg)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;
   tSERVO     *x_servo     =    0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(servo)--------------------------*/
   x_servo = ykine_servo_pointer (YKINE_BODY, a_seg);
   DEBUG_YKINE_SCRP  yLOG_point   ("x_servo"   , x_servo);
   --rce;  if (x_servo ==  NULL) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(calculate)----------------------*/
   myKINE.s_secs = myKINE.s_beats * x_servo->pace;
   if (rc == 0)  rc = yKINE_move_create (MOVE_SERVO, x_servo, myKINE.s_verb, myKINE.s_lines, a_deg, myKINE.s_secs);
   --rce;  if (rc <  0) {
      DEBUG_YKINE_SCRP   yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)---------------------------*/
   x_servo->saved  = 'y';
   x_servo->xsave  = 0.0;
   x_servo->zsave  = 0.0;
   x_servo->ysave  = 0.0;
   x_servo->dsave  = a_deg;
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> handle forward verbs ------------------[ ------ [ ------ ]-*/
ykine_scrp_orient       (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;                /* return code for errors    */
   char        rc          = 0;
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);
   /*---(adjust angles)---------------*/
   rc = ykine__scrp_orient_adjust (YKINE_YAW  , &myKINE.s_femu);
   rc = ykine__scrp_orient_adjust (YKINE_PITCH, &myKINE.s_pate);
   rc = ykine__scrp_orient_adjust (YKINE_ROLL , &myKINE.s_tibi);
   /*---(process moves)---------------*/
   rc = ykine__scrp_orient_servo  (YKINE_YAW  , myKINE.s_femu);
   rc = ykine__scrp_orient_servo  (YKINE_PITCH, myKINE.s_pate);
   rc = ykine__scrp_orient_servo  (YKINE_ROLL , myKINE.s_tibi);
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);
   return 0;
}








/*> char  /+--> make changes to body cente ------------[ ------ [ ------ ]-+/          <* 
 *> ykine_scrp_orient      (void)                                                      <* 
 *> {                                                                                  <* 
 *>    /+---(locals)-----------+-----------+-+/                                        <* 
 *>    char        rce         = -10;                /+ return code for errors    +/   <* 
 *>    char        rc          = 0;                                                    <* 
 *>    int         x_len       = 0;                                                    <* 
 *>    char        x_type      = YKINE_FROM;                                           <* 
 *>    /+---(header)-------------------------+/                                        <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                 <* 
 *>    /+---(handle)-------------------------+/                                        <* 
 *>    --rce;  switch (myKINE.s_verb [3]) {                                            <* 
 *>    case YKINE_PURE :                                                               <* 
 *>       myKINE.s_yaw      = myKINE.s_femu;                                           <* 
 *>       myKINE.s_rotate   = myKINE.s_pate;                                           <* 
 *>       myKINE.s_pitch    = myKINE.s_tibi;                                           <* 
 *>       break;                                                                       <* 
 *>    case YKINE_FROM :                                                               <* 
 *>       myKINE.s_yaw     += myKINE.s_femu;                                           <* 
 *>       myKINE.s_rotate  += myKINE.s_pate;                                           <* 
 *>       myKINE.s_pitch   += myKINE.s_tibi;                                           <* 
 *>       break;                                                                       <* 
 *>    default  :                                                                      <* 
 *>       DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");        <* 
 *>       DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                               <* 
 *>       return rce;                                                                  <* 
 *>    }                                                                               <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_yaw"     , myKINE.s_yaw);                    <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_rotate"  , myKINE.s_rotate);                 <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_pitch"   , myKINE.s_pitch);                  <* 
 *>    /+---(complete)-----------------------+/                                        <* 
 *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                 <* 
 *>    return 0;                                                                       <* 
 *> }                                                                                  <*/

/*> char  /+--> make changes to body cente ------------[ ------ [ ------ ]-+/                  <* 
 *> ykine_scrp_zero        (void)                                                              <* 
 *> {                                                                                          <* 
 *>    /+---(locals)-----------+-----------+-+/                                                <* 
 *>    char        rce         = -10;                /+ return code for errors    +/           <* 
 *>    char        rc          = 0;                                                            <* 
 *>    int         x_len       = 0;                                                            <* 
 *>    char        x_type      = 'i';                                                          <* 
 *>    /+---(header)-------------------------+/                                                <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                         <* 
 *>    /+---(handle)-------------------------+/                                                <* 
 *>    --rce;  switch (myKINE.s_verb [3]) {                                                    <* 
 *>    case YKINE_PURE :                                                                       <* 
 *>       myKINE.s_xcenter  = myKINE.s_xpos;                                                   <* 
 *>       myKINE.s_zcenter  = myKINE.s_zpos;                                                   <* 
 *>       myKINE.s_ycenter  = myKINE.s_ypos;                                                   <* 
 *>       /+> rc = ykine_scrp_ik_pure   ();                                              <+/   <* 
 *>       break;                                                                               <* 
 *>    case YKINE_FROM :                                                                       <* 
 *>       myKINE.s_xcenter += myKINE.s_xpos;                                                   <* 
 *>       myKINE.s_zcenter += myKINE.s_zpos;                                                   <* 
 *>       myKINE.s_ycenter += myKINE.s_ypos;                                                   <* 
 *>       break;                                                                               <* 
 *>    default  :                                                                              <* 
 *>       DEBUG_YKINE_SCRP  yLOG_warn    ("s_verb"    , "verb not understood");                <* 
 *>       DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                                       <* 
 *>       return rce;                                                                          <* 
 *>    }                                                                                       <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_xcenter" , myKINE.s_xcenter);                        <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_zcenter" , myKINE.s_zcenter);                        <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_ycenter" , myKINE.s_ycenter);                        <* 
 *>    /+---(complete)-----------------------+/                                                <* 
 *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                         <* 
 *>    return 0;                                                                               <* 
 *> }                                                                                          <*/

/*> static char  /+--> make changes to body tilt -------------[ ------ [ ------ ]-+/           <* 
 *> yKINE__scrp_tilt   (char *a_verb)                                                          <* 
 *> {                                                                                          <* 
 *>    /+---(locals)-----------+-----------+-+/                                                <* 
 *>    char        rce         = -10;                /+ return code for errors    +/           <* 
 *>    char        rc          = 0;                                                            <* 
 *>    int         x_len       = 0;                                                            <* 
 *>    char        x_type      = 'i';                                                          <* 
 *>    /+---(header)-------------------------+/                                                <* 
 *>    DEBUG_YKINE_SCRP   yLOG_enter   (__FUNCTION__);                                         <* 
 *>    /+---(parse)--------------------------+/                                                <* 
 *>    rc = ykine_parse_fields_OLD  ();                                                            <* 
 *>    if (rc < 0) {                                                                           <* 
 *>       DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                                       <* 
 *>       return rc;                                                                           <* 
 *>    }                                                                                       <* 
 *>    /+---(check)--------------------------+/                                                <* 
 *>    rc = ykine_parse_check   ();                                                            <* 
 *>    if (rc < 0) {                                                                           <* 
 *>       DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                                       <* 
 *>       return rc;                                                                           <* 
 *>    }                                                                                       <* 
 *>    /+---(handle)-------------------------+/                                                <* 
 *>    --rce;  switch (a_verb [3]) {                                                           <* 
 *>    case YKINE_PURE :                                                                       <* 
 *>       myKINE.s_xcenter  = myKINE.s_xpos;                                                   <* 
 *>       myKINE.s_zcenter  = myKINE.s_zpos;                                                   <* 
 *>       myKINE.s_ycenter  = myKINE.s_ypos;                                                   <* 
 *>       /+> rc = ykine_scrp_ik_pure   (a_verb);                                        <+/   <* 
 *>       break;                                                                               <* 
 *>    case YKINE_FROM :                                                                       <* 
 *>       myKINE.s_xcenter += myKINE.s_xpos;                                                   <* 
 *>       myKINE.s_zcenter += myKINE.s_zpos;                                                   <* 
 *>       myKINE.s_ycenter += myKINE.s_ypos;                                                   <* 
 *>       break;                                                                               <* 
 *>    default  :                                                                              <* 
 *>       DEBUG_YKINE_SCRP  yLOG_warn    ("a_verb"    , "verb not understood");                <* 
 *>       DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                                       <* 
 *>       return rce;                                                                          <* 
 *>    }                                                                                       <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_xcenter" , myKINE.s_xcenter);                        <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_zcenter" , myKINE.s_zcenter);                        <* 
 *>    DEBUG_YKINE_SCRP  yLOG_double  ("s_ycenter" , myKINE.s_ycenter);                        <* 
 *>    /+---(complete)-----------------------+/                                                <* 
 *>    DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                                         <* 
 *>    return 0;                                                                               <* 
 *> }                                                                                          <*/



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
         x_servo = ykine_servos (p);
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

char  /*--> parse a low level repeat --------------[ ------ [ ------ ]-*/
ykine_scrp_repeat       (void)
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
   /*> for (i = FIELD_SVO  ; i <= FIELD_ARGS ; ++i) {                                      <* 
    *>    /+---(parse field)-----------------+/                                            <* 
    *>    DEBUG_YKINE_SCRP   yLOG_note    ("read next field");                             <* 
    *>    p = strtok_r (NULL  , myKINE.s_q, &myKINE.s_context);                            <* 
    *>    --rce;  if (p == NULL) {                                                         <* 
    *>       DEBUG_YKINE_SCRP   yLOG_note    ("strtok_r came up empty");                   <* 
    *>       DEBUG_YKINE_SCRP   yLOG_exit    (__FUNCTION__);                               <* 
    *>       break;                                                                        <* 
    *>    }                                                                                <* 
    *>    strltrim (p, ySTR_BOTH, LEN_RECD);                                               <* 
    *>    x_len = strlen (p);                                                              <* 
    *>    DEBUG_YKINE_SCRP  yLOG_info    ("field"     , p);                                <* 
    *>    /+---(handle)----------------------+/                                            <* 
    *>    switch (i) {                                                                     <* 
    *>    case  FIELD_SVO   :  /+---(servo to repeat)----+/                                <* 
    *>       x_servo = ykine_servos (p);                                                   <* 
    *>       --rce;  if (x_servo < 0) {                                                    <* 
    *>          DEBUG_YKINE_SCRP  yLOG_warn    ("servo"     , "not found");                <* 
    *>          DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                             <* 
    *>          return rce;                                                                <* 
    *>       }                                                                             <* 
    *>       break;                                                                        <* 
    *>    case  FIELD_COUNT :  /+---(moves to repeat)----+/                                <* 
    *>       x_count = atoi (p);                                                           <* 
    *>       DEBUG_YKINE_SCRP  yLOG_value   ("x_count"   , x_count);                       <* 
    *>       --rce;  if (x_count < 1 || x_count > 100) {                                   <* 
    *>          DEBUG_YKINE_SCRP  yLOG_warn    ("moves"     , "out of range (1 - 100)");   <* 
    *>          DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                             <* 
    *>          return rce;                                                                <* 
    *>       }                                                                             <* 
    *>       break;                                                                        <* 
    *>    case  FIELD_TIMES :  /+---(times to repeat)----+/                                <* 
    *>       x_times = atoi (p);                                                           <* 
    *>       DEBUG_YKINE_SCRP  yLOG_value   ("x_times"   , x_times);                       <* 
    *>       --rce;  if (x_times < 1 || x_times > 100) {                                   <* 
    *>          DEBUG_YKINE_SCRP  yLOG_warn    ("times"     , "out of range (1 - 100)");   <* 
    *>          DEBUG_YKINE_SCRP  yLOG_exit    (__FUNCTION__);                             <* 
    *>          return rce;                                                                <* 
    *>       }                                                                             <* 
    *>       for (j = 0; j < g_nservo; ++j) {                                              <* 
    *>          if (g_servo_info [j].scrp != 'y') continue;                                <* 
    *>          yKINE_move_repeat     (g_servo_info + j, x_count, x_times);                <* 
    *>       }                                                                             <* 
    *>       break;                                                                        <* 
    *>    case  FIELD_ARGS  :  /+---(args)-----+/                                          <* 
    *>       yKINE__scrp_args(p);                                                          <* 
    *>       break;                                                                        <* 
    *>    }                                                                                <* 
    *>    DEBUG_YKINE_SCRP   yLOG_note    ("done with loop");                              <* 
    *> }                                                                                   <*/
   DEBUG_YKINE_SCRP   yLOG_note    ("done parsing fields");
   for (j = 0; j < g_nservo; ++j) {
      if (g_servo_info [j].scrp != 'y') continue;
      if (myKINE.s_verb [0] == 'z') {
         yKINE_move_repeat     (g_servo_info + j    , (int) myKINE.s_1st, (int) myKINE.s_2nd);
      } else if (myKINE.s_verb [0] == 'i') {
         yKINE_move_repeat     (g_servo_info + j - 2, (int) myKINE.s_1st, (int) myKINE.s_2nd);
         yKINE_move_repeat     (g_servo_info + j - 1, (int) myKINE.s_1st, (int) myKINE.s_2nd);
         yKINE_move_repeat     (g_servo_info + j    , (int) myKINE.s_1st, (int) myKINE.s_2nd);
      } else {
         yKINE_move_repeat     (g_servo_info + j    , (int) myKINE.s_1st, (int) myKINE.s_2nd);
         yKINE_move_repeat     (g_servo_info + j + 1, (int) myKINE.s_1st, (int) myKINE.s_2nd);
         yKINE_move_repeat     (g_servo_info + j + 2, (int) myKINE.s_1st, (int) myKINE.s_2nd);
      }
   }
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
         x_servo = ykine_servos (p);
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
yKINE_script       (float *a_len)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   char        x_verb      [20] = "";;
   int         i           =    0;
   float       x_len       =  0.0;
   float       x_sec       =  0.0;
   char        x_type      =  '-';
   char        x_active    =  '-';
   /*---(header)-------------------------*/
   DEBUG_YKINE_SCRP  yLOG_enter   (__FUNCTION__);
   /*---(prepare)------------------------*/
   ykine_scrp_init  ();
   /*---(read lines)---------------------*/
   DEBUG_YKINE_SCRP  yLOG_note    ("read lines");
   while (1) {
      rc = ykine_parse ();
      DEBUG_YKINE_SCRP  yLOG_value   ("read"      , rc);
      --rce;  if (feof (stdin)) {
         DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
         break;
      }
      if (rc < 0) {
         DEBUG_YKINE_SCRP  yLOG_exitr   (__FUNCTION__, rc);
         break;
      }
      /*---(handle verb)-----------------*/
      x_active = s_verb_info [myKINE.s_iverb].active;
      DEBUG_YKINE_SCRP  yLOG_char    ("active"    , x_active);
      if (x_active == 'y') {
         rc = s_verb_info [myKINE.s_iverb].call ();
      }
      /*> switch (x_verb [0]) {                                                                 <* 
       *> case 'w' : /+ walking            +/                                                   <* 
       *>    ykine_scrp_walk        ( 5);                                                       <* 
       *>    break;                                                                             <* 
       *> case 'c' : /+ circle             +/                                                   <* 
       *>    ykine_scrp_turn        (20);                                                       <* 
       *>    break;                                                                             <* 
       *> case '1' : /+ gait               +/                                                   <* 
       *>    if        (strcmp ("12_GAIT_BEG"   , x_verb) == 0) {                               <* 
       *>       ykine_gait_begin    (12);                                                       <* 
       *>    } else if (strcmp ("12_GAIT_END"   , x_verb) == 0) {                               <* 
       *>       ykine_gait_update   (12);                                                       <* 
       *>    }                                                                                  <* 
       *>    /+> yKINE__scrp_gate      ();                                                <+/   <* 
       *>    break;                                                                             <* 
       *> case '6' : /+ gait               +/                                                   <* 
       *>    if        (strcmp ("6_GAIT_BEG"   , x_verb) == 0) {                                <* 
       *>       ykine_gait_begin    (6);                                                        <* 
       *>    } else if (strcmp ("6_GAIT_END"   , x_verb) == 0) {                                <* 
       *>       ykine_gait_update   (6);                                                        <* 
       *>    }                                                                                  <* 
       *>    /+> yKINE__scrp_gate      ();                                                <+/   <* 
       *>    break;                                                                             <* 
       *> case 'R' : /+ repeat             +/                                                   <* 
       *>    ykine_scrp_repeat    ();                                                          <* 
       *>    break;                                                                             <* 
       *> case 'S' : /+ segno              +/                                                   <* 
       *>    yKINE__scrp_segno     ();                                                          <* 
       *>    break;                                                                             <* 
       *> case 'D' : /+ dal segno          +/                                                   <* 
       *>    yKINE__scrp_dsegno    ();                                                          <* 
       *>    break;                                                                             <* 
       *> case 'z' : /+ zero/center adjustments +/                                              <* 
       *>    ykine_scrp_zero      ();                                                          <* 
       *>    break;                                                                             <* 
       *> case 'o' : /+ orient body             +/                                              <* 
       *>    ykine_scrp_orient    ();                                                          <* 
       *>    break;                                                                             <* 
       *> /+> case 's' : /+ servo, start       +/                                         <*    <* 
       *>  *>    yKINE__scrp_move      (x_verb);                                          <*    <* 
       *>  *>    break;                                                                   <+/   <* 
       *> case 'f' : /+ fk based positioning +/                                                 <* 
       *> case 'i' : /+ ik based positioning +/                                                 <* 
       *>    break;                                                                             <* 
       *> default  :                                                                            <* 
       *>    DEBUG_YKINE_SCRP  yLOG_note    ("verb not recognized and skipped");                <* 
       *>    break;                                                                             <* 
       *> }                                                                                     <*/
   }
   /*---(fix length)---------------------*/
   x_len = 0.0;
   for (i = 0; i < g_nservo; ++i) {
      yKINE_move_last_servo (i, &x_sec, NULL);
      /*> printf ("#%-2d, %-10.10s, %fs\n", i, g_servo_info [i].label, x_sec);        <*/
      if (x_sec > x_len)  x_len = x_sec;
   }
   myKINE.scrp_len = x_len;
   if (a_len != NULL)  *a_len = x_len;
   /*> printf ("so, %fs and %fs\n", x_len, myKINE.scrp_len);                          <*/
   /*> exit (1);                                                                      <*/
   /*---(complete)-----------------------*/
   DEBUG_YKINE_SCRP yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char
ykine_parse_fields_pos  (void)
{
   return ykine_parse_fields (&(myKINE.s_beats), &(myKINE.s_xpos), &(myKINE.s_zpos), &(myKINE.s_ypos));
}

char
ykine_parse_fields_deg  (void)
{
   return ykine_parse_fields (&(myKINE.s_beats), &(myKINE.s_femu), &(myKINE.s_pate), &(myKINE.s_tibi));
}

char*      /*----: unit testing accessor for clean validation interface ------*/
ykine__unit_scrp        (char *a_question, int a_num)
{
   int         i           =    0;
   int         x_pos       =    0;
   char        x_msg       [LEN_STR];
   /*---(preprare)-----------------------*/
   strlcpy  (ykine__unit_answer, "SCRP unit        : question not understood", LEN_STR);
   /*---(answer)------------------------------------------*/
   if (strcmp (a_question, "verb"    ) == 0) {
      sprintf (ykine__unit_answer, "SCRP verb      : %-7.7s    , vers %c, type %c", myKINE.s_verb, myKINE.s_vers, myKINE.s_type);
   }
   else if (strcmp (a_question, "angles"   ) == 0) {
      sprintf (ykine__unit_answer, "SCRP angles    : %8.3fb, %8.2ff, %8.2fp, %8.2ft", myKINE.s_beats, myKINE.s_femu, myKINE.s_pate, myKINE.s_tibi);
   }
   else if (strcmp (a_question, "positions") == 0) {
      sprintf (ykine__unit_answer, "SCRP positions : %8.3fb, %8.2fx, %8.2fz, %8.2fy", myKINE.s_beats, myKINE.s_xpos, myKINE.s_zpos, myKINE.s_ypos);
   }
   /*---(complete)----------------------------------------*/
   return ykine__unit_answer;
}



/*============================----end-of-source---============================*/
